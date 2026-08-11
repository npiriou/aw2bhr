#include "global.h"

/* Promoted from assembly; each function below is byte-for-byte
 * identical to the original. Order is address order and must
 * stay that way -- the linker places this file's .text as one
 * contiguous block at 0x08027180.
 * sub_08027180 @ 0x08027180
 */

void sub_08027180(int a1)
{
    gArmyRecords[a1].unk3b = 0;
}
