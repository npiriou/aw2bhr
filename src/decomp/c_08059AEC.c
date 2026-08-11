#include "global.h"

/* Promoted from assembly; each function below is byte-for-byte
 * identical to the original. Order is address order and must
 * stay that way -- the linker places this file's .text as one
 * contiguous block at 0x08059AEC.
 * sub_08059AEC @ 0x08059AEC
 */

void sub_08059AEC(void)
{
    u8 n;
    int i;

    if (sub_08042D50(gCurrentArmyIndex, gUnknown_030040D8->unk00) > 1)
        n = sub_08042D50(gCurrentArmyIndex, gUnknown_030040D8->unk00);
    else
        n = gUnitTypeData[gUnknown_030040D8->unk00].unk0e;

    for (i = 0; i < n; i++)
        sub_0801FD9C((u8)(0x79 + i));
}
