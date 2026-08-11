#include "global.h"

/* Promoted from assembly; each function below is byte-for-byte
 * identical to the original. Order is address order and must
 * stay that way -- the linker places this file's .text as one
 * contiguous block at 0x080202A4.
 * sub_080202A4 @ 0x080202A4
 */

/* The unit-record dispatch of sub_0802032C's family, with the fourth argument
 * clamped: the record's own 7-bit unk06_0 unless that is already at or above
 * the type's sub_08042D1C ceiling, in which case the ceiling.
 *
 * The clamp has to land in a `u16` LOCAL rather than in the argument
 * expression. Written as one `(s16)(a < f() ? a : (u16)f())` the compiler folds
 * the (s16) into the (u16) and the ROM's second narrowing disappears; a u16
 * local keeps the two apart -- `lsls #0x10; lsrs #0x10` when the call result is
 * stored, `lsls #0x10; asrs #0x10` at the use -- which is exactly the ROM, and
 * it is also what moves the merged value into r3 with no extra copy.
 *
 * The parameter is declared `struct Unk030040D8 *` because that is what every
 * caller hands it; the body casts to the identical `struct UnitRecord` to
 * reach unk06_0 and the pointer difference, the same way c_08074320.c does. */
void sub_080202A4(struct Unk030040D8 *a1)
{
    struct UnitRecord *e;
    u16 v;

    e = (struct UnitRecord *)a1;

    if (e->unk06_0 < sub_08042D1C(((e - gUnitRecords) >> 6) + 1, e->unk00))
        v = e->unk06_0;
    else
        v = sub_08042D1C(((e - gUnitRecords) >> 6) + 1, e->unk00);

    gUnknown_030013EC(e->unk02, e->unk03, e->unk00, (s16)v, 1);
}
