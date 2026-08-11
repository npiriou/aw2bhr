#include "global.h"

/* Promoted from assembly; each function below is byte-for-byte
 * identical to the original. Order is address order and must
 * stay that way -- the linker places this file's .text as one
 * contiguous block at 0x080211DC.
 * sub_080211DC @ 0x080211DC
 */

struct Unk211DCMap
{
    /* 0x0000 */ u8 filler_0000[0x1432];
    /* 0x1432 */ u8 plane[0x417A - 0x1432];
    /* 0x417A */ u16 rowOffset[1];
};

void sub_080211DC(u8 a1, s8 a2)
{
    struct UnitRecord *e;
    struct Unk211DCMap *map;
    int bonus;

    e = &gUnitRecords[a1];
    bonus = 0;

    if (e->unk00 == 0)
        return;

    if ((e->unk01 & 6) == 2)
        return;

    if (e->unk00 <= 2)
    {
        map = (struct Unk211DCMap *)gMapData;

        if ((map->plane[map->rowOffset[e->unk03] + e->unk02] & 0x1f) == 3)
            bonus = 3;
    }

    sub_080210C8(e->unk02, e->unk03,
                 bonus + sub_08042D84(((e - gUnitRecords) >> 6) + 1, e->unk00),
                 gArmyRecords[(a1 >> 6) + 1].unk1c,
                 a2, (a1 >> 6) + 1);
}
