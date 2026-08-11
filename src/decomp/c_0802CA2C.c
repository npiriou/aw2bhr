#include "global.h"

/* Promoted from assembly; each function below is byte-for-byte
 * identical to the original. Order is address order and must
 * stay that way -- the linker places this file's .text as one
 * contiguous block at 0x0802CA2C.
 * sub_0802CA2C @ 0x0802CA2C, sub_0802CA78 @ 0x0802CA78, sub_0802CB20 @ 0x0802CB20
 */

int sub_0802CA2C(void)
{
    if (!sub_0802C8F8())
        return 1;

    if (!sub_0802CBA0())
        return 1;

    sub_0801F838(0xff);
    gUnknown_03003340[gUnknown_03003100.pos.unk02][gUnknown_03003100.pos.unk00] = 0;

    if (sub_08041758())
        return 0;

    return 1;
}

int sub_0802CA78(void)
{
    u8 *p;
    u8 *rows;
    u8 *tiles;
    int t;
    int off;
    u32 cur;

    if (gUnknown_030033E8[0] + gUnknown_030033E8[1] != 0)
        return 1;

    p = gMapData;
    t = gUnknown_03003100.pos.unk02 * 2;
    rows = p + 0x417A;
    off = *(u16 *)(rows + t) + gUnknown_03003100.pos.unk00;
    tiles = p + 0x12;

    if (tiles[off] != 0)
        return 1;

    cur = gUnknown_03003100.raw;

    if (cur != gUnknown_03003F24.raw
        && gUnitTypeData[gUnknown_030040D8->unk00].unk0e != 1)
        return 1;

    if (gUnitTypeData[gUnknown_030040D8->unk00].unk0e == 0)
        return 1;

    sub_0801F838(0xff);
    sub_08020354(gUnknown_03003100.pos.unk00, gUnknown_03003100.pos.unk02,
        (struct UnitRecord *)gUnknown_030040D8);

    if (!sub_080413E8())
        return 1;

    return 0;
}

int sub_0802CB20(void)
{
    u8 *p;
    u8 *rows;
    u8 *tiles;
    int t;
    int off;
    u32 cur;

    if (gUnitTypeData[gUnknown_030040D8->unk00].unk0e == 1)
        return 1;

    p = gMapData;
    t = gUnknown_03003100.pos.unk02 * 2;
    rows = p + 0x417A;
    off = *(u16 *)(rows + t) + gUnknown_03003100.pos.unk00;
    tiles = p + 0x12;

    if (tiles[off] != 0)
        return 1;

    cur = gUnknown_03003100.raw;

    if (cur != gUnknown_03003F24.raw)
        return 1;

    if (gUnitTypeData[gUnknown_030040D8->unk00].unk0e == 0)
        return 1;

    sub_0801F838(0xff);
    sub_08020354(gUnknown_03003100.pos.unk00, gUnknown_03003100.pos.unk02,
        (struct UnitRecord *)gUnknown_030040D8);

    if (!sub_080413E8())
        return 2;

    return 1;
}
