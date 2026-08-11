#include "global.h"

/* Promoted from assembly; each function below is byte-for-byte
 * identical to the original. Order is address order and must
 * stay that way -- the linker places this file's .text as one
 * contiguous block at 0x08018C0C.
 * sub_08018C0C @ 0x08018C0C
 */

bool8 sub_08018C0C(s16 a)
{
    u8 i = gUnknown_0200C528[a].unk04->unk08;

    gUnknown_03003FC0.unk38[i] = gUnknown_0200C528[a].unk04->unk0a;
    gArmyRecords[i].unk1b = gUnknown_0200C528[a].unk04->unk0a;
    gUnknown_0200C528[a].unk04++;
    return TRUE;
}
