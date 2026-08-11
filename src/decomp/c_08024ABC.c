#include "global.h"

/* Promoted from assembly; each function below is byte-for-byte
 * identical to the original. Order is address order and must
 * stay that way -- the linker places this file's .text as one
 * contiguous block at 0x08024ABC.
 * sub_08024ABC @ 0x08024ABC
 */

struct Unk08024ABCBlk
{
    struct UnitRecord unk00[64];
};
struct Unk08024ABCArg
{
    /* 0x00 */ struct UnitRecord *unk00;
    /* 0x04 */ u8 filler_04[0x06];
    /* 0x0a */ u16 unk0a;
    /* 0x0c */ u8 filler_0c[0x04];
    /* 0x10 */ u16 unk10;
    /* 0x12 */ u8 filler_12[0x06];
    /* 0x18 */ u16 unk18;
};

void sub_08024ABC(struct Unk08024ABCArg *a, struct Unk08024ABCArg *b, s16 c, u8 d)
{
    const struct UnitTypeData *t;
    u16 army;
    u32 v1;
    s16 v2;
    u32 v3;

    army = ((struct Unk08024ABCBlk *)a->unk00
            - (struct Unk08024ABCBlk *)gUnitRecords) + 1;
    v3 = 0;
    v1 = 0;
    v2 = 0;
    t = &gUnitTypeData[a->unk00->unk00];

    if (c == 1)
    {
        v2 = sub_080433F8(a->unk00->unk00, b->unk00->unk00, 1);
        if (v2 != 0)
            v1 = (u16)sub_08043070(gArmyRecords[army].unk1d,
                              gArmyRecords[army].unk1e,
                              a->unk00->unk00, b->unk00->unk00, c);
        if (t->unk0e == 1 && a->unk00->unk04_7 != 0)
        {
            if ((b->unk00->unk01 & 0x20) != 0)
                v3 = (u16)sub_08043070(gArmyRecords[army].unk1d,
                                  gArmyRecords[army].unk1e,
                                  a->unk00->unk00, 0x19, v3);
            else
                v3 = (u16)sub_08043070(gArmyRecords[army].unk1d,
                                  gArmyRecords[army].unk1e,
                                  a->unk00->unk00, b->unk00->unk00, v3);
        }
    }
    else if (t->unk0e <= c && c <= sub_08042D50(army, a->unk00->unk00)
             && a->unk00->unk04_7 != 0 && d == 1)
    {
        if ((b->unk00->unk01 & 0x20) != 0)
            v3 = (u16)sub_08043070(gArmyRecords[army].unk1d,
                              gArmyRecords[army].unk1e,
                              a->unk00->unk00, 0x19, v3);
        else
            v3 = (u16)sub_08043070(gArmyRecords[army].unk1d,
                              gArmyRecords[army].unk1e,
                              a->unk00->unk00, b->unk00->unk00, v3);
    }

    if (v3 <= v1)
    {
        if (v2 != 0)
        {
            a->unk18 = 5;
            a->unk10 = v1;
        }
    }
    else
    {
        a->unk18 = 1;
        a->unk10 = v3;
        a->unk0a--;
    }
}
