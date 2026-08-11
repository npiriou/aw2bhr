#include "global.h"

/* Promoted from assembly; each function below is byte-for-byte
 * identical to the original. Order is address order and must
 * stay that way -- the linker places this file's .text as one
 * contiguous block at 0x0800BCD0.
 * sub_0800BCD0 @ 0x0800BCD0
 */

/* MATCHED (wave 37, W37-C).
 *
 * sub_0800BC5C's bridge test written out NINE times, once per cell of the 3x3
 * block around (x, y), accumulated into a 9-bit mask; the answer is whether all
 * nine are bridge.  The centre is tested first and short-circuits the rest.
 *
 * TWO register-allocation facts, both measured here and both worth reusing:
 *
 *  - `y - 1` and `y + 1` are written INLINE in all six row references, with no
 *    `int n` binding them.  With `int n = y - 1;` the draft is 484 of 488: the
 *    scaled index `n * 2` survives from the middle arm into the following
 *    `x < width - 1` arm and is reused, where the ROM recomputes `lsls rN,n,#1`
 *    in every one of the three arms (2 bytes x 2 groups).  Binding the row
 *    value to a local (`int r = rowOffset[n] + 1;`) instead of the comma made
 *    no difference at all -- this function has no calls, so a leading
 *    initialiser and a comma are the same RTL.  Inlining the neighbour
 *    expression is what stops the multiply being carried across.
 *
 *  - `t` and `c` are FUNCTION scope, one pair for all nine tests.  Declared per
 *    block they are nine independent allocno pairs and four of the nine come
 *    out with `t` and `c` swapped (`ldrb r1,[r1]; movs r0,#0` where the ROM has
 *    `ldrb r0,[r1]; movs r1,#0`) -- size-exact, wrong registers.  One pair for
 *    the whole function makes the assignment uniform, and it costs nothing
 *    because there is no call in this function, so r0/r1 are free to hold a
 *    long-lived pseudo.  This is the wave-17 "binding locals are punctuation"
 *    rule read the other way round: with no calls, FEWER locals is what the ROM
 *    has.
 *
 * The `(i = row, i + x)` comma is the sub_0800A098 lever: it keeps the -+1 on
 * the ROW rather than letting agbcc sink it into the 0x1432 terrain base. */
struct MapScreen
{
    /* 0x0000 */ u16 width;
    /* 0x0002 */ u16 height;
    /* 0x0004 */ u8 filler_0004[0x0A22 - 4];
    /* 0x0A22 */ u16 cells[(0x1432 - 0x0A22) / 2];
    /* 0x1432 */ u8 terrain[0x417A - 0x1432];
    /* 0x417A */ u16 rowOffset[1];
};
#define MAP ((struct MapScreen *)gMapData)

int sub_0800BCD0(int x, int y)
{
    int m = 0;
    int t;
    int c;

    t = MAP->terrain[MAP->rowOffset[y] + x];
    c = 0;
    if (t == 0x13 || t == 7)
        c = 1;
    m |= c << 4;

    if (m == 0)
        return 0;

    if (y > 0)
    {
        if (x > 0)
        {
            int i;
            t = MAP->terrain[(i = MAP->rowOffset[y - 1] - 1, i + x)];
            c = 0;
            if (t == 0x13 || t == 7)
                c = 1;
            m |= c << 8;
        }
        t = MAP->terrain[MAP->rowOffset[y - 1] + x];
        c = 0;
        if (t == 0x13 || t == 7)
            c = 1;
        m |= c << 7;

        if (x < MAP->width - 1)
        {
            int i;
            t = MAP->terrain[(i = MAP->rowOffset[y - 1] + 1, i + x)];
            c = 0;
            if (t == 0x13 || t == 7)
                c = 1;
            m |= c << 6;
        }
    }

    if (x > 0)
    {
        int i;
        t = MAP->terrain[(i = MAP->rowOffset[y] - 1, i + x)];
        c = 0;
        if (t == 0x13 || t == 7)
            c = 1;
        m |= c << 5;
    }

    if (x < MAP->width - 1)
    {
        int i;
        t = MAP->terrain[(i = MAP->rowOffset[y] + 1, i + x)];
        c = 0;
        if (t == 0x13 || t == 7)
            c = 1;
        m |= c << 3;
    }

    if (y < MAP->height - 1)
    {
        if (x > 0)
        {
            int i;
            t = MAP->terrain[(i = MAP->rowOffset[y + 1] - 1, i + x)];
            c = 0;
            if (t == 0x13 || t == 7)
                c = 1;
            m |= c << 2;
        }
        t = MAP->terrain[MAP->rowOffset[y + 1] + x];
        c = 0;
        if (t == 0x13 || t == 7)
            c = 1;
        m |= c << 1;

        if (x < MAP->width - 1)
        {
            int i;
            t = MAP->terrain[(i = MAP->rowOffset[y + 1] + 1, i + x)];
            c = 0;
            if (t == 0x13 || t == 7)
                c = 1;
            m |= c;
        }
    }

    return m == 0x1ff;
}
