#include "global.h"

/* Promoted from assembly; each function below is byte-for-byte
 * identical to the original. Order is address order and must
 * stay that way -- the linker places this file's .text as one
 * contiguous block at 0x0805A514.
 * sub_0805A514 @ 0x0805A514, sub_0805A5E0 @ 0x0805A5E0
 */

struct Unk5A514Cell
{
    /* 0x00 */ u8 x;
    /* 0x01 */ u8 y;
    /* 0x02 */ s16 v;
};

void sub_0805A514(struct Unk5A514Cell *out)
{
    struct UnitRecord *u;
    u16 pos[2];
    int i;

    for (i = gUnknown_03003F2C; i < gUnknown_03003F2C + 0x40; i++)
    {
        u = &gUnitRecords[i];
        if (u->unk00 == 0)
            continue;
        if ((u->unk09 & 0x38) != 0x18)
            continue;
        if (gUnknown_03004730[i & 0x3f] != 0)
            continue;
        if ((s8)gUnknown_03003340[u->unk03][u->unk02] == -1)
            continue;
        if (sub_0805ACA8(u->unk02, u->unk03, pos) != 1)
            continue;
        out->x = u->unk02;
        out->y = u->unk03;
        out->v = (s8)gUnknown_03003340[u->unk03][u->unk02];
        out++;
    }

    out->v = 0xFFFF;
}

void sub_0805A5E0(int *out)
{
    struct UnitRecord *p;
    int i;
    u16 best;
    int t;
    int v;

    best = 0x7FFF;
    *out = -1;

    for (i = gUnknown_03003F2C; i < gUnknown_03003F2C + 0x40; i++)
    {
        p = &gUnitRecords[i];
        if (p->unk00 == 0)
            continue;
        if (gUnknown_084995A8[p->unk00] == 0)
            continue;
        if (p->unk01 & 8)
            continue;
        if ((p->unk09 & 7) != 1)
            continue;
        if (&gUnitRecords[i] == (struct UnitRecord *)gUnknown_030040D8)
            continue;
        if ((s8)gUnknown_03003340[p->unk03][p->unk02] < 0)
            continue;

        t = (u16)((5 - gUnknown_08576828[p->unk00]) * 16);
        v = p->unk06_0 + t;

        if ((s16)v < (s16)best)
        {
            *out = p - gUnitRecords;
            best = v;
        }
    }
}
