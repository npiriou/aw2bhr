#include "global.h"

/* Promoted from assembly; each function below is byte-for-byte
 * identical to the original. Order is address order and must
 * stay that way -- the linker places this file's .text as one
 * contiguous block at 0x0802E724.
 * sub_0802E724 @ 0x0802E724
 */

bool8 sub_0802E724(s16 a1, s16 a2)
{
    struct UnitRecord **ptbl;
    u8 *p;
    u8 *rows;
    u8 *tiles;
    struct UnitRecord *e;
    int t;
    int off;
    int army;

    ptbl = &gUnitRecords;

    p = gMapData;
    t = a2 * 2;
    rows = p + 0x417A;
    off = *(u16 *)(rows + t) + a1;
    tiles = p + 0x12;
    e = &(*ptbl)[tiles[off]];

    army = gUnknown_03003F38 & 0xc0;

    if (army != gUnknown_03003F2C)
        return FALSE;

    if (gUnknown_03003340[a2][a1] > 0x78)
        return FALSE;

    if (tiles[off] == 0)
        return TRUE;

    if ((tiles[off] & 0xc0) != army)
        return FALSE;

    if ((u8)sub_08025FC0((struct UnitRecord *)gUnknown_030040D8, e) == 1)
        return TRUE;

    if ((u8)sub_080422A8(a1, a2) == 1)
        return TRUE;

    return FALSE;
}
