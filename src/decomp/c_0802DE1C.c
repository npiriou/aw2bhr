#include "global.h"

/* Promoted from assembly; each function below is byte-for-byte
 * identical to the original. Order is address order and must
 * stay that way -- the linker places this file's .text as one
 * contiguous block at 0x0802DE1C.
 * sub_0802DE1C @ 0x0802DE1C, sub_0802DEFC @ 0x0802DEFC
 */

#include "hardware.h"

void sub_0802DE1C(void)
{
    u8 *p;
    u8 *rows;
    s8 *terrain;
    int t;
    int off;
    int v;

    sub_08023824();
    sub_080236E8();
    sub_08023908(4);

    p = gMapData;
    t = gUnknown_030033E4.unk02 * 2;
    rows = p + 0x417A;
    off = *(u16 *)(rows + t) + gUnknown_030033E4.unk00;
    terrain = (s8 *)(p + 0x2852);

    if (terrain[off] < 0)
        sub_08023274(1);
    else
        sub_08023274(1);

    sub_08039264();

    if (!sub_0802DBF8())
        return;

    v = gpKeySt->held & 1;

    if (v != 0)
    {
        sub_0802E60C(((union Unk802C57CBuf *)&gUnknown_030033E4)->spos.unk00,
            ((union Unk802C57CBuf *)&gUnknown_030033E4)->spos.unk02);
        return;
    }

    if (!(gpKeySt->held & 2))
        return;

    if (gUnknown_03003FC0.unk32 != 0)
        sub_08034534(0x11, gUnknown_03003F38, 0, 0);

    sub_08029088(gUnknown_030040D8->unk02, gUnknown_030040D8->unk03);
    sub_08035810();
    sub_080258CC();
    sub_08022A08();
    gUnknown_03003334 = v;
    sub_0803B4DC(0x66);
}

void sub_0802DEFC(void)
{
    u8 *p;
    u8 *rows;
    u8 *tiles;
    int t;
    int off;
    int id;
    struct UnitRecord *e;

    sub_08023824();
    sub_0802361C();
    sub_08023908(4);
    sub_08023274(5);

    if (!sub_0802DBF8())
        return;

    if (gpKeySt->held & 2)
    {
        sub_0803B4DC(0x66);
        gUnknown_03003334 = 0;
        return;
    }

    p = gMapData;
    t = gUnknown_030033E4.unk02 * 2;
    rows = p + 0x417A;
    off = *(u16 *)(rows + t) + gUnknown_030033E4.unk00;
    tiles = p + 0x12;
    id = tiles[off];

    if (id == 0)
        return;

    if (((u32)id >> 6) + 1 != gCurrentArmyIndex)
        return;

    e = &gUnitRecords[id];

    if (e->unk01 & 1)
        return;

    if (!(gpKeySt->held & 1))
        return;

    if (gUnknown_03003FC0.unk32 != 0)
        sub_08034534(0x12, id, 0, 0);

    sub_0804018C(e);
}
