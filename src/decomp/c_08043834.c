#include "global.h"

/* Promoted from assembly; each function below is byte-for-byte
 * identical to the original. Order is address order and must
 * stay that way -- the linker places this file's .text as one
 * contiguous block at 0x08043834.
 * sub_08043834 @ 0x08043834, sub_08043898 @ 0x08043898, sub_080438FC @ 0x080438FC
 */

void sub_08043834(int a)
{
    u16 (*tbl)[16];
    int i;

    gUnknown_030005D0 = 0;
    sub_08011E54(gUnknown_08102824, (void *)0x06010000, 0x740);
    tbl = gUnknown_08104264;
    i = gArmyRecords[a].unk1a - 1;
    ApplyPaletteExt(tbl[i], 0x2e0, 0x20);
    sub_08011E54(gUnknown_081259CC, (void *)0x06010840, 0xc0);
}

void sub_08043898(int x, int y, int i)
{
    switch (gArmyRecords[i].unk1e)
    {
    case 1:
    default:
        PutSprite(0, x, y + 0x18, gUnknown_084A0052, 0x702f);
        break;
    case 2:
        PutSprite(0, x, y + 0x18, gUnknown_084A005A, 0x7033);
        break;
    }
}

void sub_080438FC(int a, int b, int c)
{
    int va;
    int vy;
    int vu;
    int vt;
    int lo;
    int hi;
    int m;

    vu = gArmyRecords[c].unk20;
    lo = sub_080441D4(c);
    hi = sub_08044208(c) - lo;
    m = sub_0804419C(c);
    vt = gUnknown_03004008;
    va = a;
    vy = b + 0x18;

    sub_080439A8(&va, &vy, &vu, m, lo, 0, vu >= lo, &vt);
    sub_080439A8(&va, &vy, &vu, m, hi, 1, vu >= hi, &vt);
}
