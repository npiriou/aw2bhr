#include "global.h"

/* Promoted from assembly; each function below is byte-for-byte
 * identical to the original. Order is address order and must
 * stay that way -- the linker places this file's .text as one
 * contiguous block at 0x080015E4.
 * sub_080015E4 @ 0x080015E4
 */

/* A walkability predicate over the gMapData map: the cell must not be
 * kind 7, 0xd or 0x13, must pass sub_08008C34, and -- for kind 2 only -- must
 * pass sub_080094EC as well.
 *
 * BOTH CALL RESULTS ARE BOUND before the test chain, because the ROM makes both
 * calls unconditionally. Folded into the `&&` chain, sub_08008C34 would be
 * called after the three kind compares and short-circuited away; the ROM calls
 * it first and only then materialises the shared `movs r1, #0`.
 *
 * `movs r1, #0` sits between the call and the compares -- one shared result
 * variable the five `beq`/`bne` fall through to -- which is the same shape
 * c_0800164C.c documents next door. The row/tile arithmetic is that file's
 * idiom verbatim: rows at +0x417A indexed `y * 2`, tiles at +0x1432. */
int sub_080015E4(int x, int y)
{
    u8 *p;
    u8 *rows;
    u8 *tiles;
    int t;
    int off;
    int v;
    int w;
    int s;
    int r;

    p = gMapData;
    t = y * 2;
    rows = p + 0x417A;
    off = *(u16 *)(rows + t) + x;
    tiles = p + 0x1432;
    v = tiles[off];

    if (v == 2)
        s = sub_080094EC(x, y);
    else
        s = 0;

    w = sub_08008C34(x, y);

    r = 0;
    if (v != 7 && v != 0xd && v != 0x13 && w == 0 && s == 0)
        r = 1;

    return r;
}
