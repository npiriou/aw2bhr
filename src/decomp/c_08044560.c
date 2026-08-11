#include "global.h"

/* Promoted from assembly; each function below is byte-for-byte
 * identical to the original. Order is address order and must
 * stay that way -- the linker places this file's .text as one
 * contiguous block at 0x08044560.
 * sub_08044560 @ 0x08044560
 */

void sub_08044560(void)
{
    gArmyRecords[gCurrentArmyIndex].unk26 = Div(gArmyRecords[gCurrentArmyIndex].unk00, 300);
    gArmyRecords[gCurrentArmyIndex].unk28 = 10;
}
