#include "global.h"

/* Promoted from assembly; each function below is byte-for-byte
 * identical to the original. Order is address order and must
 * stay that way -- the linker places this file's .text as one
 * contiguous block at 0x08043304.
 * sub_08043304 @ 0x08043304, sub_0804334C @ 0x0804334C, sub_0804338C @ 0x0804338C
 */

/* The pointer parameter's +0x00 is a `struct UnitRecord *` into the unit
 * table and +0x06 an s16.  `(p->unk00 - gUnitRecords) >> 6` is the
 * 0-based army number -- the table is grouped 64 entries to an army, the same
 * `>> 6` unknown-globals.h already records -- and the `+ 1` makes it the
 * 1-based one sub_08043050 indexes by.  The whole `sub/lsl/add/neg/asr #8`
 * chain is agbcc's EXACT division for the pointer difference (multiply by
 * -inverse(3), negate, then one `asr` that carries both the /4 of the exact
 * division and the >> 6) -- it is one operator, not five.
 */
struct Unk43304
{
    /* 0x00 */ struct UnitRecord *unk00;
    /* 0x04 */ u8 filler_04[0x02];
    /* 0x06 */ s16 unk06;
};

int sub_08043304(struct Unk43304 *p)
{
    if ((sub_08043050(((p->unk00 - gUnitRecords) >> 6) + 1) & 0x20) == 0)
        return p->unk06;

    return p->unk06 * 2;
}

int sub_0804334C(struct Unk43304 *p)
{
    int r;

    if (sub_08043050(((p->unk00 - gUnitRecords) >> 6) + 1) & 0x40)
        r = p->unk06;
    else
        r = 0;

    return r;
}

int sub_0804338C(struct Unk43304 *p)
{
    return sub_08042FFC(((p->unk00 - gUnitRecords) >> 6) + 1);
}
