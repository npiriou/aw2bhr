#include "global.h"

/* Promoted from assembly; each function below is byte-for-byte
 * identical to the original. Order is address order and must
 * stay that way -- the linker places this file's .text as one
 * contiguous block at 0x08018C80.
 * sub_08018C80 @ 0x08018C80
 */

bool8 sub_08018C80(s16 a)
{
    u8 i = gUnknown_0200C528[a].unk04->unk08;

    gArmyRecords[i].unk1d = gUnknown_0200C528[a].unk04->unk0a;
    gUnknown_0200C528[a].unk04++;
    return TRUE;
}
