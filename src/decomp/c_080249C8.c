#include "global.h"

/* Promoted from assembly; each function below is byte-for-byte
 * identical to the original. Order is address order and must
 * stay that way -- the linker places this file's .text as one
 * contiguous block at 0x080249C8.
 * sub_080249C8 @ 0x080249C8
 */

int sub_080249C8(int a)
{
    int i = a & 0xE0;

    if (i == 0)
        return 0;

    return gArmyRecords[i >> 5].unk1a;
}
