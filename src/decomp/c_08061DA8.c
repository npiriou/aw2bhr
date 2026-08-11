#include "global.h"

/* Promoted from assembly; each function below is byte-for-byte
 * identical to the original. Order is address order and must
 * stay that way -- the linker places this file's .text as one
 * contiguous block at 0x08061DA8.
 * sub_08061DA8 @ 0x08061DA8
 */

/* Promoted from assembly; each function below is byte-for-byte
 * identical to the original. Order is address order and must
 * stay that way -- the linker places this file's .text as one
 * contiguous block at 0x08061DA8.
 * sub_08061DA8 @ 0x08061DA8
 */

int sub_08061DA8(int index)
{
    struct ArmyRecord *p = &gArmyRecords[index];

    return p->unk0c + p->unk0d + p->unk0e + p->unk0f + 1;
}
