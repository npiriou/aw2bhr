#include "global.h"

/* Promoted from assembly; each function below is byte-for-byte
 * identical to the original. Order is address order and must
 * stay that way -- the linker places this file's .text as one
 * contiguous block at 0x0802700C.
 * sub_0802700C @ 0x0802700C, sub_0802706C @ 0x0802706C
 */

bool8 sub_0802700C(int a1, int a2, int a3)
{
    u8 *p;
    u8 *rows;
    u8 *tiles;
    int t;
    int off;
    int army;

    p = gMapData;
    t = a3 * 2;
    rows = p + 0x417A;
    off = *(u16 *)(rows + t) + a2;
    tiles = p + 0x1432;
    army = tiles[off];
    army = army >> 5;

    if (army == 0)
        return FALSE;

    if (gArmyRecords[a1].unk2a == gArmyRecords[army].unk2a)
        return TRUE;

    return FALSE;
}

bool8 sub_0802706C(u8 a1, u16 a2, u16 a3)
{
    if (gUnknown_03003FC0.unk0d != 0
        && (gArmyRecords[a3].unk1c & 2) == 0
        && gUnitTypeData[a1].unk14 != NULL)
        return TRUE;

    return FALSE;
}
