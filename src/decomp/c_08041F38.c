#include "global.h"

/* Promoted from assembly; each function below is byte-for-byte
 * identical to the original. Order is address order and must
 * stay that way -- the linker places this file's .text as one
 * contiguous block at 0x08041F38.
 * sub_08041F38 @ 0x08041F38, sub_08041FE0 @ 0x08041FE0
 */

/* Probes the four cells around (x, y) with sub_08041EA8 and returns a
 * direction bitmask: 4 = left, 8 = right, 1 = up, 2 = down.
 *
 * `r` is initialised BEFORE the `id == 0` test, not after the unit lookup --
 * that is what puts `movs r4,#0` ahead of the `cmp`. The first `r |= 4` comes
 * out as a bare `movs r4,#4` because cprop knows r is still 0 there; only the
 * last one carries the u8 truncation, the rest being provably in range.
 *
 * unit->unk00 is re-loaded before each of the four calls: gUnitRecords is
 * not const, so every `bl` kills the MEM. */
u8 sub_08041F38(int x, int y, int id)
{
    struct UnitRecord *unit;
    u8 r;

    r = 0;

    if (id == 0)
        return 0;

    unit = &gUnitRecords[id];

    if (sub_08041EA8(x - 1, y, unit->unk00) == 1)
        r |= 4;

    if (sub_08041EA8(x + 1, y, unit->unk00) == 1)
        r |= 8;

    if (sub_08041EA8(x, y - 1, unit->unk00) == 1)
        r |= 1;

    if (sub_08041EA8(x, y + 1, unit->unk00) == 1)
        r |= 2;

    return r;
}

/* The same army-number idiom the matched sub_0804203C uses one function over:
 * `(p - gUnitRecords) >> 6` is the exact division by the 0x0c stride
 * (`mul 0x55555555; neg; asr #2`) with the `>> 6` merged into the ROM's single
 * `asr #8`, and `+ 1` makes it the 1-based army sub_08042D50 takes.
 *
 * p->unk00 is loaded ONCE and reused for both the gUnitTypeData subscript
 * and sub_08042D50's second argument -- there is no call between the two uses.
 * The 0x780 mask is the four-bit bitfield unk04_7; testing a bitfield against
 * zero needs only the mask, not the usual extract shift pair. */
int sub_08041FE0(struct UnitRecord *p)
{
    if (gUnitTypeData[p->unk00].unk11 == 1)
        return 1;

    if (sub_08042D50(((p - gUnitRecords) >> 6) + 1, p->unk00) > 1)
        return 0;

    if (p->unk04_7 != 0)
        return 1;

    return 0;
}
