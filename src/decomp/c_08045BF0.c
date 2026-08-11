#include "global.h"

/* Promoted from assembly; each function below is byte-for-byte
 * identical to the original. Order is address order and must
 * stay that way -- the linker places this file's .text as one
 * contiguous block at 0x08045BF0.
 * sub_08045BF0 @ 0x08045BF0
 */

/* Reads the byte at +0x19 of the gMapData record the halfword at
 * +0x417E points to, and returns whether its top two bits are 0 while the byte
 * itself is non-zero.
 *
 * The cell is held in an `int`, not a `u8`, and that is measured rather than
 * stylistic: with a `u8` local agbcc knows the value is zero-extended and
 * emits `lsrs r0,#6`, whereas the ROM has `asrs r0,#6`. Widening at the
 * assignment is what keeps the shift arithmetic. (The `>> 6` on
 * gUnknown_03003F38 elsewhere in the tree really is `lsrs` -- the two are not
 * the same expression.) */
int sub_08045BF0(void)
{
    u8 *p;
    int v;

    p = gMapData;
    v = (p + *(u16 *)(p + 0x417E))[0x19];

    if (v == 0)
        return 0;
    if ((v >> 6) == 0)
        return 1;

    return 0;
}
