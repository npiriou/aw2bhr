#include "global.h"

/* Promoted from assembly; each function below is byte-for-byte
 * identical to the original. Order is address order and must
 * stay that way -- the linker places this file's .text as one
 * contiguous block at 0x08043070.
 * sub_08043070 @ 0x08043070
 */

/* The per-army repair/income multiplier, applied as a percentage: the byte at
 * gUnitTypeData[c].unk1e[e][d] scales sub_080430B0's result, which is
 * itself a percentage that the `+ 100` turns into a multiplier.  A zero
 * product is raised to 1, so a matchup that "works at all" never rounds away
 * to nothing, but a zero scale byte short-circuits to 0 before that.
 *
 * The return is `int` and NOT the u16 unknown-functions.h used to declare:
 * a u16-returning agbcc function narrows its own result in the epilogue and
 * this one does not.  See the note on the declaration.
 *
 * `base` is its own statement, evaluated unconditionally ahead of the
 * `v > 0` test -- the ROM does the `bl` and the `+ 0x64` before the `cmp`.
 */
int sub_08043070(int a1, int a2, int a3, int a4, int a5)
{
    int v = gUnitTypeData[a3].unk1e[a5][a4];
    int base = sub_080430B0(a1, a2, a3) + 100;
    int r;

    if (v > 0)
    {
        r = Div(v * base, 100);
        if (r == 0)
            r = 1;
    }
    else
    {
        r = 0;
    }

    return r;
}
