#include "global.h"

/* Promoted from assembly; each function below is byte-for-byte
 * identical to the original. Order is address order and must
 * stay that way -- the linker places this file's .text as one
 * contiguous block at 0x08024A2C.
 * sub_08024A2C @ 0x08024A2C
 */

struct Unk24A2CMap
{
    /* 0x0000 */ u8 filler_0000[0x1432];
    /* 0x1432 */ u8 plane[0x417A - 0x1432];
    /* 0x417A */ u16 rowOffset[1];
};

void sub_08024A2C(struct Unk030013D0 *a1, s16 a2)
{
    struct UnitRecord *e;
    struct Unk24A2CMap *map;
    int idx;
    int t;

    e = &gUnitRecords[a2];
    a1->unk00 = e;

    map = (struct Unk24A2CMap *)gMapData;
    idx = map->rowOffset[e->unk03] + e->unk02;
    t = map->plane[idx] & 0x1f;

    a1->unk04 = t;
    a1->unk06 = (s8)sub_080249EC((u16)(((e - gUnitRecords) >> 6) + 1),
                                 t, e->unk00);
    a1->unk08 = a1->unk00->unk04_0;
    a1->unk0a = a1->unk00->unk04_7;
    a1->unk18 = 0;
    a1->unk10 = 0;
    a1->unk12 = 0;
}
