#include "global.h"

/* Promoted from assembly; each function below is byte-for-byte
 * identical to the original. Order is address order and must
 * stay that way -- the linker places this file's .text as one
 * contiguous block at 0x08025AEC.
 * sub_08025AEC @ 0x08025AEC
 */

struct UnitRecord *sub_08025AEC(void)
{
    int i;

    for (i = gUnknown_03003F2C + 1; i < gUnknown_03003F2C + 0x33; i++)
    {
        if (gUnitRecords[i].unk00 == 0)
            return &gUnitRecords[i];
    }

    return 0;
}
