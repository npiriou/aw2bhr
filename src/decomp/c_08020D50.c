#include "global.h"

/* Promoted from assembly; each function below is byte-for-byte
 * identical to the original. Order is address order and must
 * stay that way -- the linker places this file's .text as one
 * contiguous block at 0x08020D50.
 * sub_08020D50 @ 0x08020D50
 */

/* sub_08020354's signed-coordinate twin against a third overlay writer. Here
 * the guard reads gUnitTypeData[t].unk0f rather than re-calling
 * sub_08042D50, and the ROM keeps the element ADDRESS live across the test to
 * reach .unk0e -- one subscript expression, two members. */
void sub_08020D50(s16 x, s16 y, struct UnitRecord *e)
{
    sub_08020B88(x, y,
                 sub_08042D50(((e - gUnitRecords) >> 6) + 1, e->unk00), 0);
    if (gUnitTypeData[e->unk00].unk0f != 1)
        sub_08020B88(x, y, gUnitTypeData[e->unk00].unk0e - 1, -1);
}
