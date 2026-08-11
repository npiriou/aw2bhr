#include "global.h"

/* Promoted from assembly; each function below is byte-for-byte
 * identical to the original. Order is address order and must
 * stay that way -- the linker places this file's .text as one
 * contiguous block at 0x0800F2E0.
 * sub_0800F2E0 @ 0x0800F2E0
 */

/* The c_08001158.c map idiom again -- row-offset table at +0x417A, byte plane
 * at +0x1432 -- with a two-value membership test on top: kinds 5 and 0xc.
 *
 * `movs r0, #0` sits AHEAD of both compares and both arms fall into the same
 * exit, so the 0 is one shared result variable rather than a `return 0` per
 * arm; the `beq` on the first value jumps INTO the `r = 1` block, which is what
 * `||` produces. */
int sub_0800F2E0(int x, int y)
{
    u8 *p;
    u8 *rows;
    u8 *tiles;
    int t;
    int off;
    int v;
    int r;

    p = gMapData;
    t = y * 2;
    rows = p + 0x417A;
    off = *(u16 *)(rows + t) + x;
    tiles = p + 0x1432;
    v = tiles[off];

    r = 0;

    if (v == 5 || v == 0xC)
        r = 1;

    return r;
}
