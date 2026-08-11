#include "global.h"

/* Promoted from assembly; each function below is byte-for-byte
 * identical to the original. Order is address order and must
 * stay that way -- the linker places this file's .text as one
 * contiguous block at 0x080201E0.
 * sub_080201E0 @ 0x080201E0
 */

/* sub_08020354's signed-coordinate twin against a different overlay writer.
 * The second sub_08042D50 call is spelled out again rather than bound to a
 * local: the ROM recomputes the whole army-number chain and re-issues the call
 * before comparing the result against 1. */
void sub_080201E0(s16 x, s16 y, struct UnitRecord *e)
{
    sub_080200EC(x, y,
                 sub_08042D50(((e - gUnitRecords) >> 6) + 1, e->unk00), 1);
    if (sub_08042D50(((e - gUnitRecords) >> 6) + 1, e->unk00) != 1)
        sub_080200EC(x, y, gUnitTypeData[e->unk00].unk0e - 1, -1);
}
