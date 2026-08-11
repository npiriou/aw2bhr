#include "global.h"

/* Promoted from assembly; each function below is byte-for-byte
 * identical to the original. Order is address order and must
 * stay that way -- the linker places this file's .text as one
 * contiguous block at 0x08043D84.
 * sub_08043D84 @ 0x08043D84
 */

/* Promoted from assembly; each function below is byte-for-byte
 * identical to the original. Order is address order and must
 * stay that way -- the linker places this file's .text as one
 * contiguous block at 0x08043D84.
 * sub_08043D84 @ 0x08043D84
 */

u16 sub_08043D84(u8 a1)
{
    return gCoDataTable[gArmyRecords[a1].unk1d].unk04;
}
