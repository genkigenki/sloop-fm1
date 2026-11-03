/* SPDX-License-Identifier: GPL-3.0-only
 * SMK-37 Pro: Copyright (C) 2026 M. Geisler (genkigenki/makkuro) */
/* SMK-37 menu PAD LIGHT (firmware/src/smk_padlight.h): 100 % changes nothing; at every setting the pads' dim states stay
 * visible (at least 60 % of their 100 % level, and 8), the levels the pads tell apart (ui_input.c, ui_daw.c: empty /
 * set / playhead, dim / lit, used / playing ...) keep a ratio of 1.5 at least, brighter stays brighter, the hue stays,
 * off stays off. */
#include <stdint.h>
#include <stdio.h>
#include "smk_padlight.h"
static int fails;
static void check(int ok, const char *what) { printf("smk_padlight: %-72s %s\n", what, ok ? "ok" : "FAIL"); fails += !ok; }

static uint32_t vmax(const uint8_t *c) { uint32_t v = c[0]; if (c[1] > v) v = c[1]; if (c[2] > v) v = c[2]; return v; }
static uint32_t lit(uint32_t step, const uint8_t *c, uint8_t *o)
{
    o[0] = c[0], o[1] = c[1], o[2] = c[2];
    padl_apply(step, &o[0], &o[1], &o[2]);
    return vmax(o);
}

/* the track colours (ui_input.c PAL) and the states the pads show, as dim / bright pairs of one hue */
static const uint8_t PAL[4][3] = {{40, 124, 255}, {30, 204, 112}, {255, 198, 24}, {255, 98, 26}};
#define W(x) {x, x, x}
static const uint8_t PAIRS[][2][3] = {
    {W(40), W(200)},                               /* STEPS: an empty step, the playhead */
    {{0, 40, 0}, {0, 110, 0}},                     /* SAVE held: a used slot, playing */
    {{0, 15, 15}, {0, 70, 70}},                    /* SAVE held: loop / song */
    {{15, 0, 0}, {90, 0, 0}},                      /* SAVE held: SONG REC */
    {{0, 30, 0}, {0, 90, 0}},                      /* SCALE held: the scale, its root */
    {W(40), W(200)},                               /* DAW: a NOTE pad's blink */
    {{100, 80, 10}, {255, 198, 24}},               /* DAW: a CC / toggle pad off / on */
};
/* the dim states whose light must stay (ui_input.c, ui_daw.c) */
static const uint8_t DIMS[][3] = {W(40), W(18), {0, 15, 15}, {15, 0, 0}, {0, 30, 0}, {0, 40, 0}, W(16)};

int main(void)
{
    uint32_t st, i, k, v, ok;
    uint8_t o[3], o2[3];
    check(PADL_N == 4u && pad_light == PADL_100 && padl_scale(PADL_100) == 256u && padl_scale(PADL_25) == 64u,
          "four steps 100 75 50 25 %, 100 % the default (0: data of earlier builds)");
    for (v = 0, ok = 1; v < 256u; v++)
        for (k = 0; k < 256u; k += 17u) {
            uint8_t c[3] = {(uint8_t)v, (uint8_t)k, (uint8_t)(255u - v)};
            lit(PADL_100, c, o);
            ok &= o[0] == c[0] && o[1] == c[1] && o[2] == c[2];
        }
    check(ok, "100 %: every colour as it is");
    for (st = 0, ok = 1; st < PADL_N; st++) {
        uint32_t prev = 0;
        for (v = 1; v < 256u; v++) {
            uint8_t c[3] = {(uint8_t)v, 0, 0};
            uint32_t m = lit(st, c, o);
            ok &= m >= 1u && m <= v && m >= prev;
            prev = m;
        }
        {
            uint8_t z[3] = {0, 0, 0};
            ok &= lit(st, z, o) == 0u;
        }
    }
    check(ok, "every step: off stays off, a lit pad stays lit, never brighter, brighter stays brighter");
    for (st = 0, ok = 1; st < PADL_N; st++)
        for (i = 0; i < sizeof DIMS / sizeof DIMS[0]; i++) {
            uint32_t m = lit(st, DIMS[i], o), m0 = vmax(DIMS[i]);
            ok &= m * 10u >= m0 * 6u && m >= 8u;
            if (st == PADL_25)
                printf("  25 %%: dim %3u -> %3u\n", (unsigned)m0, (unsigned)m);
        }
    check(ok, "every step: the dim states keep 60 % of their light and at least 8");
    for (st = 0, ok = 1; st < PADL_N; st++) {
        for (i = 0; i < sizeof PAIRS / sizeof PAIRS[0]; i++) {
            uint32_t a = lit(st, PAIRS[i][0], o), b = lit(st, PAIRS[i][1], o2);
            ok &= b * 2u >= a * 3u;
        }
        for (i = 0; i < 4u; i++) {                 /* each track colour: lit / 3/8 (DRUM KIT, STEPS), lit / 1/8 (PLAY:
                                                    * the note and its guide, col >> 1 and >> 3) */
            const uint8_t *c = PAL[i];
            uint8_t d[3] = {(uint8_t)(c[0] * 3u >> 3), (uint8_t)(c[1] * 3u >> 3), (uint8_t)(c[2] * 3u >> 3)};
            uint8_t h[3] = {(uint8_t)(c[0] >> 1), (uint8_t)(c[1] >> 1), (uint8_t)(c[2] >> 1)};
            uint8_t g[3] = {(uint8_t)(c[0] >> 3), (uint8_t)(c[1] >> 3), (uint8_t)(c[2] >> 3)};
            ok &= lit(st, c, o) * 2u >= lit(st, d, o2) * 3u;
            ok &= lit(st, h, o) * 2u >= lit(st, g, o2) * 3u;
            lit(st, c, o);                         /* the hue: the components' order kept */
            ok &= (c[0] >= c[1]) == (o[0] >= o[1]) && (c[1] >= c[2]) == (o[1] >= o[2]);
        }
        if (st == PADL_25) {
            uint8_t f[3] = {255, 255, 255};
            printf("  25 %%: full 255 -> %u, playhead 200 -> %u\n", (unsigned)lit(st, f, o), (unsigned)lit(st, PAIRS[0][1], o2));
        }
    }
    check(ok, "every step: dim / bright pairs apart by 1.5 at least, the hue kept");
    printf("smk_padlight: %s\n", fails ? "FAILED" : "all ok");
    return fails != 0;
}
