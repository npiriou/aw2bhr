#include "global.h"

/* Promoted from assembly; each function below is byte-for-byte
 * identical to the original. Order is address order and must
 * stay that way -- the linker places this file's .text as one
 * contiguous block at 0x08026340.
 * sub_08026340 @ 0x08026340, sub_08026368 @ 0x08026368
 */

u8 sub_08026340(void)
{
    u8 count = 0;
    u8 i;

    for (i = 1; i <= 4; i++)
    {
        if (gUnknown_03003FF8[i] != 0)
            count++;
    }

    return count;
}

u32 sub_08026368(u8 a)
{
    u32 v = gArmyRecords[a].unk34 + gArmyRecords[a].unk35 + gArmyRecords[a].unk36;

    if (v > 999)
        v = 9999;

    return v;
}
