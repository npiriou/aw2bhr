#include "global.h"

/* Promoted from assembly; each function below is byte-for-byte
 * identical to the original. Order is address order and must
 * stay that way -- the linker places this file's .text as one
 * contiguous block at 0x08029978.
 * sub_08029978 @ 0x08029978, sub_08029A48 @ 0x08029A48
 */

int sub_08029978(struct UnitRecord *p, u8 a2)
{
    int acc;
    u16 v;

    acc = 0;
    v = p->unk04_0 != 0
        ? (Div(p->unk04_0 - 1, 10) + 1) * gUnitTypeData[p->unk00].unk08
        : 0;

    while (p->unk04_7 != gUnitTypeData[p->unk00].unk0b)
    {
        if (a2)
        {
            if (gArmyRecords[gCurrentArmyIndex].unk00 < v)
                break;

            sub_08025B28(gCurrentArmyIndex, v);
        }

        acc += v;
        p->unk04_7++;
    }

    return acc;
}

int sub_08029A48(struct UnitRecord *p, u8 a2)
{
    int acc;
    u16 v;

    acc = 0;
    v = p->unk04_0 != 0 ? Div(p->unk04_0 - 1, 10) + 1 : 0;

    while (p->unk06_0 != gUnitTypeData[p->unk00].unk10)
    {
        if (a2)
        {
            if (gArmyRecords[gCurrentArmyIndex].unk00 < v)
                break;

            sub_08025B28(gCurrentArmyIndex, v);
        }

        acc += v;
        p->unk06_0++;
    }

    return acc;
}
