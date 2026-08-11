#include "global.h"

/* Promoted from assembly; each function below is byte-for-byte
 * identical to the original. Order is address order and must
 * stay that way -- the linker places this file's .text as one
 * contiguous block at 0x0800B048.
 * sub_0800B048 @ 0x0800B048
 */

/* The vertical-connector re-tile for cell (x, y): picks the sub_08001158 tile
 * id from what sub_0800119C reports about the cells above (y-1, y-2) and below
 * (y+1, y+2), then does the one-cell-right fixup c_0800CEF8.c also does.
 *
 * The struct is c_0800AF24.c's -- this function is that file's neighbour and
 * sub_0800AF24 / sub_0800AF74 both call it.  Only width and height are read
 * here, which is why the pool word stays a plain inline `.word
 * gMapData`: two reads is under the four -fforce-addr needs.
 *
 * THE DISCRIMINATOR, and the only thing this cost a rewrite over: both
 * two-call tests over (y - 2) are `== 0 && == 0` with the SMALL tile id as the
 * then-arm, not `|| ` with 0x22 as the then-arm.  The two spellings are the
 * same predicate and differ only in block layout, and the ROM says which:
 *   `if (A == 0 && B == 0) two; else h22;`  ->  bne, bne, `2`, b, then 0x22
 *   `if (A || B) h22; else two;`            ->  bne, BEQ, 0x22, b, then `2`
 * The ROM has TWO `bne`s to the same label and the small id as fall-through,
 * so the second test is not inverted and the `== 0 &&` form is the source.
 * That is the same shape as the function's own opening test, which is a good
 * consistency check.  Written up in docs/agbcc-codegen.md.
 *
 * The four shared tails (_0800B080, _0800B184, _0800B190, _0800B194) are
 * cross-jumping, not source structure: the y+1 block really is written out
 * three times, once per arm, and jump.c merged the identical suffixes.  Do not
 * try to hoist it -- hoisting emits it ONCE and the arms then differ.
 *
 * `sub_0800AFCC(x - 1, y - 1) ? 3 : 0x43` is a real `?:` on an argument: the
 * ROM presets `movs r2,#0x43` before the `cmp` and overwrites it with 3 only on
 * the nonzero side.
 *
 * The tail's `x++` mutates the parameter in place the way c_0800CEF8.c's does,
 * and the width test uses the OLD x -- `cmp r5, width-2` comes before
 * `adds r5, #1`. */

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

void sub_0800B048(int x, int y)
{
    if (sub_0800119C(x, y - 1, 1) == 0 && sub_0800119C(x, y - 1, 3) == 0)
    {
        if (sub_0800119C(x, y + 1, 3))
        {
            sub_08001158(x, y, 2);
            if (sub_0800119C(x, y + 2, 3))
                sub_08001158(x, y + 1, 0x22);
            else
                sub_08001158(x, y + 1, 0x23);
        }
        else
        {
            sub_08001158(x, y, 0x20);
        }
    }
    else if (sub_0800119C(x, y - 1, 1))
    {
        sub_08001158(x, y - 1, sub_0800AFCC(x - 1, y - 1) ? 3 : 0x43);

        if (sub_0800119C(x, y - 1, 3))
        {
            if (sub_0800119C(x, y - 2, 3) == 0 && sub_0800119C(x, y - 2, 1) == 0)
                sub_08001158(x, y - 1, 2);
            else
                sub_08001158(x, y - 1, 0x22);
        }

        if (sub_0800119C(x, y + 1, 3))
        {
            sub_08001158(x, y, 0x22);
            if (sub_0800119C(x, y + 2, 3))
                sub_08001158(x, y + 1, 0x22);
            else
                sub_08001158(x, y + 1, 0x23);
        }
        else
        {
            sub_08001158(x, y, 0x23);
        }
    }
    else
    {
        if (sub_0800119C(x, y - 2, 3) == 0 && sub_0800119C(x, y - 2, 1) == 0)
            sub_08001158(x, y - 1, 2);
        else
            sub_08001158(x, y - 1, 0x22);

        if (sub_0800119C(x, y + 1, 3))
        {
            sub_08001158(x, y, 0x22);
            if (sub_0800119C(x, y + 2, 3))
                sub_08001158(x, y + 1, 0x22);
            else
                sub_08001158(x, y + 1, 0x23);
        }
        else
        {
            sub_08001158(x, y, 0x23);
        }
    }

    if (x < MAP->width - 2)
    {
        x++;
        if (sub_0800119C(x, y, 1) && y < MAP->height - 1)
        {
            if (sub_0800119C(x, y + 1, 3))
                sub_08001158(x, y, 3);
            else
                sub_08001158(x, y, 0x21);
        }
    }
}
