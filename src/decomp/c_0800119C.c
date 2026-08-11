#include "global.h"

/* Promoted from assembly; each function below is byte-for-byte
 * identical to the original. Order is address order and must
 * stay that way -- the linker places this file's .text as one
 * contiguous block at 0x0800119C.
 * sub_0800119C @ 0x0800119C
 */

/* A bounds-checked cell-kind test on the gMapData map: outside the
 * width/height stored at +0 and +2 the answer is 0, otherwise it is whether the
 * cell equals the caller's kind.
 *
 * gMapData IS NOT BOUND BEFORE THE GUARD. The ROM loads it only at the
 * third test -- the first two compare the coordinates against zero and need
 * nothing -- and then keeps that one load across the whole tail. Naming it
 * inline in the guard and binding `p` only afterwards is what places the load
 * there; a `p = gMapData;` at the top hoists it two compares early.
 *
 * `movs r2, #0` ahead of the cell compare is the shared result variable, the
 * same shape as c_0800164C.c next door; the row/tile arithmetic is that file's
 * idiom verbatim. */
int sub_0800119C(int x, int y, int k)
{
    u8 *p;
    u8 *rows;
    u8 *tiles;
    int t;
    int off;
    int r;

    if (x < 0 || y < 0 || y > *(u16 *)(gMapData + 2) - 1
        || x > *(u16 *)gMapData - 1)
        return 0;

    p = gMapData;
    r = 0;
    t = y * 2;
    rows = p + 0x417A;
    off = *(u16 *)(rows + t) + x;
    tiles = p + 0x1432;

    if (tiles[off] == k)
        r = 1;

    return r;
}
