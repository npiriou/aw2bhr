#include "global.h"

/* Promoted from assembly; each function below is byte-for-byte
 * identical to the original. Order is address order and must
 * stay that way -- the linker places this file's .text as one
 * contiguous block at 0x0800CEF8.
 * sub_0800CEF8 @ 0x0800CEF8
 */

/* The tail half of sub_08007D70 with the first re-tile dropped: only the cell
 * to the RIGHT of (x, y) is re-tiled, and only when one exists.  Same guard,
 * one column narrower -- sub_08007D70 stops two columns from the edge
 * (`x < width - 2`) because it also touches x itself, this one stops one
 * column short.
 *
 * `sub_08001158(x, y, sub_080016D0(x, y))` is a genuine nesting and not two
 * statements: sub_080016D0's result is moved to r2 and used as the third
 * argument.  The `x++` is a real mutation of the parameter -- the ROM bumps r4
 * in place and both calls use the new value, and the nested call's second
 * argument costs nothing because r1 still holds y.
 *
 * `*(u16 *)gMapData` is the map width at +0 of the screen descriptor,
 * the same object c_08007D70.c and c_0800BEE4.c read. */
void sub_0800CEF8(int x, int y)
{
    if (x < *(u16 *)gMapData - 1)
    {
        x++;
        sub_08001158(x, y, sub_080016D0(x, y));
    }
}
