/* SPDX-License-Identifier: GPL-3.0-only */
/* SMK-37 analog scan order (firmware/hal/smk_input.h smk__slot_next / smk__slot_addr / smk__analog): every pad, fader,
 * wheel, the pedal and the battery once a cycle at the mux address and channel the stock table gives; every 8th cycle
 * the battery once more (SMK_SLOT_BATT), straight after its first conversion, on mux C (ch2) at address 7 as the
 * stock firmware reads it (after another mux C conversion), and that second reading is the battery's (fm1_in.batt),
 * the first only batt1st. The pads keep their rate (one tick longer every 8th cycle). Built -O2 (the IRQ lock's asm is
 * left out). */
#define BOARD_SMK37 1
#include "smk_input.h"
#include <stdio.h>
static int fails;
static void check(int ok, const char *what) { printf("smk_scan: %-74s %s\n", what, ok ? "ok" : "FAIL"); fails += !ok; }

int main(void)
{
    static const uint8_t CH[3] = {SMK_ADC_PADA, SMK_ADC_PADB, SMK_ADC_MUXC};
    uint32_t s = 0, cyc = 0, ticks = 0, n, seen[25] = {0}, batt_cycles = 0, bad_order = 0, prev = 0, ok_addr = 1;
    uint32_t cyc_ticks[16] = {0};
    while (cyc < 64u) {                             /* 64 cycles */
        uint32_t ns = smk__slot_next(s, cyc);
        seen[s]++;
        if (s < SMK_SLOT_BATT && smk__slot_addr(s) != s / 3u)
            ok_addr = 0;
        if (s == SMK_SLOT_BATT) {
            batt_cycles++;
            if (prev != SMK_SLOT_BATT - 1u || (cyc & 7u) || smk__slot_addr(s) != 7u || CH[prev % 3u] != SMK_ADC_MUXC)
                bad_order++;
        }
        if (cyc < 16u)
            cyc_ticks[cyc]++;
        prev = s;
        s = ns;
        ticks++;
        cyc += !ns;
    }
    check(ok_addr && smk__slot_addr(23) == 7u && CH[23 % 3u] == SMK_ADC_MUXC, "slot 23: mux C (ch2) at address 7, the battery, as before");
    check(batt_cycles == 8u && bad_order == 0, "the battery again every 8th cycle, right after slot 23, ch2 at address 7");
    check(cyc_ticks[0] == 25u && cyc_ticks[1] == 24u && cyc_ticks[7] == 24u && cyc_ticks[8] == 25u,
          "a cycle 24 ticks, every 8th 25 (the pads 2.4 ms apart, one 0.1 ms longer every 19.2 ms)");
    for (n = 0; n < 24u; n++)
        if (seen[n] != 64u)
            bad_order++;
    check(bad_order == 0, "every slot 0..23 (16 pads, 4 faders, 2 wheels, pedal, battery) once a cycle");

    fm1_in.batt = fm1_in.batt1st = 0;
    smk__analog(23, 540);
    check(fm1_in.batt == 0 && fm1_in.batt1st == 540, "slot 23's reading is batt1st only (debug)");
    smk__analog(SMK_SLOT_BATT, 598);
    check(fm1_in.batt == 598 && fm1_in.batt1st == 540, "slot 24's is the battery (fm1_adc_read(FM1_ADC_BATT))");
    check(fm1_adc_read(FM1_ADC_BATT) == 598, "fm1_adc_read: the battery");
    smk__analog(20, 1000);
    check(fm1_in.pedal == 1000 && fm1_in.batt == 598, "slot 20: the pedal (mux C address 6)");
    printf("smk_scan: %s\n", fails ? "FAILED" : "all ok");
    return fails != 0;
}
