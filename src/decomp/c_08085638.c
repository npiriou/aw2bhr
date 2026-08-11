#include "global.h"

/* Promoted from assembly; each function below is byte-for-byte
 * identical to the original. Order is address order and must
 * stay that way -- the linker places this file's .text as one
 * contiguous block at 0x08085638.
 * sub_08085638 @ 0x08085638, sub_080856A0 @ 0x080856A0
 */

int sub_08085638(int a1, int a2)
{
    switch (sub_08043190(gArmyRecords[a1].unk1d, gArmyRecords[a1].unk1e, a2))
    {
    case -1:
        return 0x95;
    case -2:
        return 0x96;
    case -3:
        return 0x97;
    default:
        return 0x98;
    case 2:
        return 0x99;
    case 3:
        return 0x9a;
    }
}

int sub_080856A0(int a1, int a2)
{
    switch (sub_08043200(gArmyRecords[a1].unk1d, gArmyRecords[a1].unk1e, a2))
    {
    case -1:
        return 0x95;
    case -2:
        return 0x96;
    case -3:
        return 0x97;
    default:
        return 0x98;
    case 2:
        return 0x99;
    case 3:
        return 0x9a;
    }
}
