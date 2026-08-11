#include "global.h"

/* Promoted from assembly; each function below is byte-for-byte
 * identical to the original. Order is address order and must
 * stay that way -- the linker places this file's .text as one
 * contiguous block at 0x08007D70.
 * sub_08007D70 @ 0x08007D70, sub_08007DB0 @ 0x08007DB0
 */

/* Re-tile the cell at (x, y) and, unless it is in the last two columns, the one
 * to its right. `sub_08001158(x, y, sub_080016D0(x, y))` is a genuine nesting
 * and not two statements: sub_080016D0's result is moved to r2 and used as the
 * third argument -- see sub_080016D0's own note for why it returns at all.
 *
 * `*(u16 *)gMapData` is the map width at +0 of the screen descriptor,
 * the same object c_08001158.c indexes at +0x417A. The `x++` is a real
 * mutation of the parameter: the ROM bumps r4 in place and both later calls use
 * the new value. */
void sub_08007D70(int x, int y)
{
    sub_08001158(x, y, sub_080016D0(x, y));

    if (x < *(u16 *)gMapData - 2)
    {
        x++;
        sub_08001158(x, y, sub_080016D0(x, y));
    }
}

/* Ask sub_08007DD0 for a tile id and write it only if it is not the negative
 * "no tile" answer.
 *
 * The `lsls #0x10; asrs #0x10` is the re-narrowing of an s16 RETURN, not a cast
 * this function performs: sub_08007DD0's own exits are an `ldrsh`, a literal
 * -1, and an `asrs #0x10`, so the callee body agrees with the call site. The
 * local is what keeps the single narrowing shared between the test and the
 * argument. */
void sub_08007DB0(int x, int y)
{
    s16 v;

    v = sub_08007DD0(x, y);

    if (v >= 0)
        sub_08001158(x, y, v);
}
