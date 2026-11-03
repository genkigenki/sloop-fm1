/* SPDX-License-Identifier: GPL-3.0-only
 * SMK-37 Pro: Copyright (C) 2026 M. Geisler (genkigenki/makkuro) */
/* SMK-37: HOME menu > LIGHTS > PAD LIGHT, how bright the 16 pads are: 100 % (as before), 75 %, 50 %, 25 %. Every pad
 * colour goes out through padl_apply (lcd.c padled_set), so every screen and layer follows it.
 * The bright states follow the setting, the dim ones keep most of their light, so a dim pad never goes dark and the
 * levels stay apart at every setting: a colour's brightest component v is multiplied by
 *     s + (1 - s) K / (v + K)        s = the setting, K = PADL_KNEE (the dim states' level, an empty step's 40)
 * which is 1 for a faint colour and s for a full one (25 %: 40 -> 25, 110 -> 49, 255 -> 90). The hue stays (all three
 * components by the same factor); a lit component never rounds to 0. Kept in the SMK-37 settings (project.c smkset_t,
 * its spare byte: 0 = 100 %, so data of earlier builds reads as 100 %). */
#pragma once

enum { PADL_100, PADL_75, PADL_50, PADL_25, PADL_N };
static const char *const PADL_NAME[PADL_N] = {"100 %", "75 %", "50 %", "25 %"};
#define PADL_KNEE 40u
static uint8_t pad_light;                          /* menu PAD LIGHT (PADL_*; 0 = 100 %) */

static uint32_t padl_scale(uint32_t step) { return 256u - 64u * (step % PADL_N); }   /* s of 256 */

/* one colour at setting step: r, g, b in place */
static void padl_apply(uint32_t step, uint8_t *r, uint8_t *g, uint8_t *b)
{
    uint32_t s = padl_scale(step), v = *r, num, den;
    if (s >= 256u)
        return;                                    /* (100 %: as it is) */
    if (*g > v)
        v = *g;
    if (*b > v)
        v = *b;
    if (!v)
        return;                                    /* (off stays off) */
    num = s * (v + PADL_KNEE) + (256u - s) * PADL_KNEE;   /* the factor num / den, exact (rounded once: brighter stays */
    den = 256u * (v + PADL_KNEE);                          /* brighter) */
#define PADL_C(c) ((c) ? (uint8_t)(((c) * num + den / 2u) / den ? ((c) * num + den / 2u) / den : 1u) : 0u)
    *r = PADL_C(*r), *g = PADL_C(*g), *b = PADL_C(*b);
#undef PADL_C
}
