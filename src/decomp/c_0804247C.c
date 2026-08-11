#include "global.h"

/* Promoted from assembly; each function below is byte-for-byte
 * identical to the original. Order is address order and must
 * stay that way -- the linker places this file's .text as one
 * contiguous block at 0x0804247C.
 * sub_0804247C @ 0x0804247C
 */

/* Promoted from assembly; each function below is byte-for-byte
 * identical to the original. Order is address order and must
 * stay that way -- the linker places this file's .text as one
 * contiguous block at 0x0804247C.
 * sub_0804247C @ 0x0804247C
 */

/* The c_08001158.c map-cell idiom: gMapData is a `u8 *` whose +0x417A
 * is a table of per-row halfword offsets and whose +0x1432 is the cell array
 * this one reads (c_08001158.c uses +0xA22 for tiles). The four locals are what
 * keep the two constants in their own pool words and the adds on the base,
 * rather than folding into the load displacement. */

/* Wave 38 (W38-K): return retyped `int` -> `bool8`. Byte-neutral here (the body
 * returns literal 0/1), but the sole caller sub_08041758 re-narrows the result
 * with `lsls #0x18; lsrs #0x18` before `cmp #1`, which agbcc emits only for a
 * narrow-returning callee. See include/unknown-functions.h. */
bool8 sub_0804247C(s16 a, s16 b)
{
    u8 *p;
    u8 *rows;
    u8 *cells;
    int t;
    int off;

    p = gMapData;
    t = b * 2;
    rows = p + 0x417a;
    off = *(u16 *)(rows + t) + a;
    cells = p + 0x1432;

    if ((cells[off] & 0x1f) == 0x11)
        return 1;
    else
        return 0;
}
