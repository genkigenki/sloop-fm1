/* SPDX-License-Identifier: GPL-3.0-only */
/* SMK-37 PEDAL jack (firmware/hal/smk_pedal.h): a sustain switch from the raw reading. AUTO learns the rest only once
 * it is steady near an end (never the first reading, which may be the 0 before the scan has read the jack, never a
 * middle reading), either polarity, hysteresis, debounce (bounce, a lone spike), the drift, OPEN / CLOSED fixed, the
 * re-learn. Then a fuzz: noise around either rest never makes a press. */
#include <stdio.h>
#include <stdlib.h>
#include "../firmware/hal/smk_pedal.h"
static int fails;
static void check(int ok, const char *what) { printf("smk_pedal: %-74s %s\n", what, ok ? "ok" : "FAIL"); fails += !ok; }

static smk_pedal_t p;
static uint32_t ms, edges;
static int last;
/* the reading raw for n ms, polled every 1 ms (the app may poll faster or slower) */
static int feed(uint32_t raw, uint32_t mode, uint32_t n)
{
    int d = last;
    while (n--) {
        d = smk_pedal_poll(&p, raw, mode, ms++);
        edges += d != last;
        last = d;
    }
    return d;
}
static void reset(void)
{
    smk_pedal_t z = {0};
    p = z, ms = 1000, edges = 0, last = 0;
}

int main(void)
{
    uint32_t i;
    reset();
    check(feed(0, SMK_PEDAL_AUTO, 3) == 0 && !p.learnt, "AUTO: a first reading (0 before the scan) is not the rest");
    feed(1010, SMK_PEDAL_AUTO, 150);
    check(!p.learnt && last == 0, "AUTO: not yet learnt after 150 ms steady");
    feed(1010, SMK_PEDAL_AUTO, 100);
    check(p.learnt && p.rest >= 1003 && last == 0, "AUTO: learnt once 200 ms steady (high: a normally open pedal)");
    check(feed(5, SMK_PEDAL_AUTO, 30) == 1, "AUTO high rest: pressed (low) is down");
    check(feed(1012, SMK_PEDAL_AUTO, 30) == 0 && edges == 2, "AUTO high rest: let go is up, one edge each");

    reset();                                    /* a normally closed pedal: rest low */
    feed(8, SMK_PEDAL_AUTO, 250);
    check(p.learnt && p.rest == 8, "AUTO: a low rest is learnt (a normally closed pedal)");
    check(feed(1020, SMK_PEDAL_AUTO, 30) == 1 && feed(10, SMK_PEDAL_AUTO, 30) == 0, "AUTO low rest: open is down, closed up");

    reset();                                    /* no pedal, a floating input: never learnt, never down */
    for (i = 0; i < 3000u; i++)
        feed(450u + (uint32_t)(rand() % 120), SMK_PEDAL_AUTO, 1);
    check(!p.learnt && edges == 0, "AUTO: a middle reading (no pedal) is never the rest, never down");
    feed(512, SMK_PEDAL_AUTO, 1000);
    check(!p.learnt && edges == 0, "AUTO: a steady middle reading neither");

    reset();
    feed(1000, SMK_PEDAL_AUTO, 300);
    feed(5, SMK_PEDAL_AUTO, 6);
    check(last == 0, "debounce: 6 ms pressed is not yet down");
    feed(5, SMK_PEDAL_AUTO, 6);
    check(last == 1, "debounce: ... 12 ms is");
    for (i = 0; i < 20u; i++) {                 /* contact bounce on the release */
        feed(1000, SMK_PEDAL_AUTO, 2);
        feed(5, SMK_PEDAL_AUTO, 1);
    }
    check(last == 1, "debounce: bouncing contacts (shorter than 10 ms) change nothing");
    feed(1000, SMK_PEDAL_AUTO, 15);
    check(last == 0 && edges == 2, "debounce: then up, one edge each way");
    feed(5, SMK_PEDAL_AUTO, 3);                 /* a lone spike */
    feed(1000, SMK_PEDAL_AUTO, 20);
    check(last == 0 && edges == 2, "debounce: a 3 ms spike is no press");

    reset();                                    /* hysteresis: half way in between never flips it */
    feed(1000, SMK_PEDAL_AUTO, 300);
    check(feed(550, SMK_PEDAL_AUTO, 50) == 0, "hysteresis: 450 from the rest is not down (on above 600)");
    check(feed(300, SMK_PEDAL_AUTO, 50) == 1, "hysteresis: 700 from the rest is down");
    check(feed(550, SMK_PEDAL_AUTO, 50) == 1, "hysteresis: back to 450 stays down (off below 400)");
    check(feed(700, SMK_PEDAL_AUTO, 50) == 0, "hysteresis: 300 is up");

    reset();                                    /* the rest drifts (temperature): followed while up */
    feed(1000, SMK_PEDAL_AUTO, 300);
    for (i = 0; i < 200u; i++)
        feed(1000u - i, SMK_PEDAL_AUTO, 20);
    check(p.rest <= 830 && p.rest >= 790 && edges == 0, "drift: a slow drift of the rest is followed, no press");

    reset();                                    /* fixed */
    check(feed(1015, SMK_PEDAL_OPEN, 30) == 0 && feed(3, SMK_PEDAL_OPEN, 30) == 1 && feed(1015, SMK_PEDAL_OPEN, 30) == 0,
          "OPEN: high up, low down, at once (nothing to learn)");
    check(feed(3, SMK_PEDAL_CLOSED, 30) == 0 && feed(1015, SMK_PEDAL_CLOSED, 30) == 1, "CLOSED: low up, high down");
    reset();
    check(feed(512, SMK_PEDAL_OPEN, 30) == 0 && feed(512, SMK_PEDAL_CLOSED, 30) == 0, "OPEN / CLOSED: a middle reading leaves it up");

    reset();                                    /* learnt high, then a normally closed pedal goes in: re-learn */
    feed(1000, SMK_PEDAL_AUTO, 300);
    check(feed(4, SMK_PEDAL_AUTO, 500) == 1, "plugged in the other way: reads down (inverted) ...");
    smk_pedal_relearn(&p);
    check(feed(4, SMK_PEDAL_AUTO, 5) == 0 && !p.learnt, "... re-learn: up at once, learning");
    feed(4, SMK_PEDAL_AUTO, 250);
    check(p.learnt && feed(1000, SMK_PEDAL_AUTO, 30) == 1 && feed(4, SMK_PEDAL_AUTO, 30) == 0, "... learnt low: right again");

    reset();                                    /* fuzz: noise +-20 around either rest, polled at random rates */
    {
        uint32_t f, bad = 0;
        for (f = 0; f < 200u; f++) {
            uint32_t rest = f & 1u ? 1000u : 20u, n;
            reset();
            for (n = 0; n < 4000u; n++) {
                int32_t r = (int32_t)rest + rand() % 41 - 20;
                last = smk_pedal_poll(&p, (uint32_t)(r < 0 ? 0 : r), SMK_PEDAL_AUTO, ms);
                ms += (uint32_t)(rand() % 4);
                bad += last != 0;
            }
            bad += !p.learnt;
        }
        check(bad == 0, "fuzz: noise around a rest (either end, any poll rate): learnt, never down");
    }
    printf("smk_pedal: %s\n", fails ? "FAILED" : "all ok");
    return fails != 0;
}
