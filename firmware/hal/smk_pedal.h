/* SPDX-License-Identifier: GPL-3.0-only
 * SMK-37 Pro port: Copyright (C) 2026 M. Geisler (genkigenki/makkuro) */
/* SMK-37 Pro: the PEDAL jack as a sustain switch, for any app on the SMK layer. smk_input.h reads the jack raw (mux C
 * address 6, 0..1023: fm1_in.pedal); smk_pedal_poll turns that into down / up. No hardware access, no globals: the
 * app keeps an smk_pedal_t, calls smk_pedal_poll as often as it likes with a millisecond clock (once the scan has
 * run: fm1_in.frames past a few hundred, so the reading is the jack's) and sends what it wants (CC 64, its sustain).
 *
 * Polarity (the app's setting, SMK_PEDAL_*): AUTO (0, the default) learns the reading at rest: once it has stayed
 * within SMK_PEDAL_STEADY_D for SMK_PEDAL_STEADY_MS near either end (SMK_PEDAL_END), so a reading in the middle (no
 * pedal, a floating input) is never taken, and nor is a single first reading. Until then the pedal is up.
 * OPEN: at rest the reading is high (a normally open pedal on the jack's pull-up), CLOSED: low. A pedal plugged in
 * after the rest was learnt the other way round reads inverted: the app offers smk_pedal_relearn (or OPEN / CLOSED).
 * Down: more than SMK_PEDAL_ON from the rest, up again below SMK_PEDAL_OFF (hysteresis); a change counts once it has
 * held SMK_PEDAL_DEB_MS (contact bounce, a spike, plugging in). While up, the learnt rest follows a slow drift.
 * The stock firmware averages the jack and maps it over a curve (an expression pedal too); this reads a switch only.
 * Host test: tests/smk_pedal_test.c. */
#pragma once
#include <stdint.h>

enum { SMK_PEDAL_AUTO, SMK_PEDAL_OPEN, SMK_PEDAL_CLOSED, SMK_PEDAL_N };
#define SMK_PEDAL_ON 600          /* from the rest: down */
#define SMK_PEDAL_OFF 400         /* .. up again (a middle reading, 512, never changes the state in OPEN / CLOSED) */
#define SMK_PEDAL_DEB_MS 10u      /* a change holds this long */
#define SMK_PEDAL_STEADY_MS 200u  /* AUTO: the rest holds this long */
#define SMK_PEDAL_STEADY_D 24     /* .. within this */
#define SMK_PEDAL_END 200         /* .. this near 0 or 1023 */

typedef struct {
    int16_t rest, cand;           /* AUTO: the learnt rest; the candidate */
    uint32_t t;                   /* the candidate's start, then a pending change's */
    uint8_t learnt, down, pend, seen;
} smk_pedal_t;

static void smk_pedal_relearn(smk_pedal_t *p) { p->learnt = p->seen = 0; }

/* raw 0..1023, mode SMK_PEDAL_*, ms a millisecond clock: 1 while the pedal is down */
static int smk_pedal_poll(smk_pedal_t *p, uint32_t raw, uint32_t mode, uint32_t ms)
{
    int32_t r = raw > 1023u ? 1023 : (int32_t)raw, d;
    int want;
    if (mode == SMK_PEDAL_OPEN) {
        d = 1023 - r;
    } else if (mode == SMK_PEDAL_CLOSED) {
        d = r;
    } else if (!p->learnt) {
        d = r - p->cand;
        if (!p->seen)
            p->cand = (int16_t)r, d = 0;
        p->cand = (int16_t)(p->cand + d / 8);      /* (the candidate follows the mean: noise does not throw it) */
        if (!p->seen || d < -SMK_PEDAL_STEADY_D || d > SMK_PEDAL_STEADY_D)
            p->t = ms, p->seen = 1;
        else if (ms - p->t >= SMK_PEDAL_STEADY_MS && (p->cand < SMK_PEDAL_END || p->cand > 1023 - SMK_PEDAL_END))
            p->rest = p->cand, p->learnt = 1;
        p->down = p->pend = 0;
        return 0;
    } else {
        d = r - p->rest;
        if (d < 0)
            d = -d;
        if (!p->down && d < SMK_PEDAL_STEADY_D)
            p->rest = (int16_t)(p->rest + (r - p->rest) / 4);   /* (drift) */
    }
    want = d > (p->down ? SMK_PEDAL_OFF : SMK_PEDAL_ON);
    if (want == p->down)
        p->pend = 0;
    else if (!p->pend)
        p->pend = 1, p->t = ms;
    else if (ms - p->t >= SMK_PEDAL_DEB_MS)
        p->down = (uint8_t)want, p->pend = 0;
    return p->down;
}
