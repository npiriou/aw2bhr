#include "global.h"

/* Promoted from assembly; each function below is byte-for-byte
 * identical to the original. Order is address order and must
 * stay that way -- the linker places this file's .text as one
 * contiguous block at 0x08057F00.
 * sub_08057F00 @ 0x08057F00, sub_08057F54 @ 0x08057F54, sub_08057FA8 @ 0x08057FA8, sub_08057FE8 @ 0x08057FE8, sub_08058058 @ 0x08058058, sub_08058144 @ 0x08058144
 */

/* c_0804151C.c's list builder again, appending to the gUnknown_03003F20 list
 * from the element index it is HANDED and returning the index it reached: the
 * caller's running count in and out. The same
 * `if ((u8)(gUnknown_030040D8->unk00 - 1) > 1) return 0;` guard as
 * sub_0804151C, and it sits AFTER the cursor is formed, as it does there.
 *
 * The cursor is formed in TWO statements. `(struct Cell *)gUnknown_03003F20 + n`
 * as one expression scales n before loading the pointer
 * (`lsls r1,r7,#2; ldr r0,[r0]; adds r2,r0,r1`); the ROM loads first
 * (`ldr r2,[r0]; lsls r0,r7,#2; adds r2,r2,r0`), which is the separate `+=`.
 *
 * Predicate: the map cell's low 5 terrain bits equal 0x11. The value written is
 * the cell's own signed terrain byte biased by 5 and floored at 0 -- `<= 4`
 * stores 0 on the FALLTHROUGH arm and the `bgt` goes to the `- 5` arm, so the
 * comparison is written that way round.
 *
 * Cell addressing is sub_080415E4's idiom -- p, then t, then rows, then off,
 * then cells -- keeping 0x417A and 0x1432 in pool words.
 */

struct Unk58058Cell
{
    /* 00 */ u8 x;
    /* 01 */ u8 y;
    /* 02 */ s16 v;
};
/* The 4-byte record sub_08057EC0 hands back: a (column, row) pair plus the
 * s16 priority it stamps with 0x7FFF on the way out.  It has no struct tag in
 * include/ yet -- gUnknown_03003F20's declared `struct Unk03003338 *` is a
 * different, 0x08-strided type -- so it is spelled locally here. */
struct Unk08057EC0Rec
{
    /* 0x00 */ u8 unk00;
    /* 0x01 */ u8 unk01;
    /* 0x02 */ s16 unk02;
};

int sub_08057F00(int a1)
{
    int i;
    int count;

    count = 0;

    for (i = gUnknown_03003F2C; i < gUnknown_03003F2C + 0x40; i++)
    {
        if (gUnitRecords[i].unk00 != 0
         && gUnitTypeData[gUnitRecords[i].unk00].unk1b == a1)
            count++;
    }

    return count;
}

int sub_08057F54(int a1)
{
    int i;
    int count;

    count = 0;

    for (i = gUnknown_03003F2C; i < gUnknown_03003F2C + 0x40; i++)
    {
        if (gUnitRecords[i].unk00 != 0
         && (gUnitTypeData[gUnitRecords[i].unk00].unk1a & a1) != 0)
            count++;
    }

    return count;
}

int sub_08057FA8(int a1)
{
    int i;
    int count;

    count = 0;

    for (i = gUnknown_03003F2C; i < gUnknown_03003F2C + 0x40; i++)
    {
        if (gUnitRecords[i].unk00 == a1)
            count++;
    }

    return count;
}

int sub_08057FE8(int a1)
{
    int i;
    int j;
    int count;

    count = 0;

    for (i = 0; i < 4; i++)
    {
        if ((gArmyRecords[gCurrentArmyIndex].unk2c >> i) & 1)
        {
            for (j = i * 64; j < i * 64 + 0x40; j++)
            {
                if (gUnitRecords[j].unk00 == a1)
                    count++;
            }
        }
    }

    return count;
}

int sub_08058058(int n)
{
    struct Unk58058Cell *out;
    u8 *p;
    u8 *rows;
    u8 *cells;
    int t;
    int off;
    int x;
    int y;

    out = (struct Unk58058Cell *)gUnknown_03003F20;
    out += n;

    if ((u8)(gUnknown_030040D8->unk00 - 1) > 1)
        return 0;

    for (y = 0; y < *(u16 *)(gMapData + 2); y++)
    {
        for (x = 0; x < *(u16 *)gMapData; x++)
        {
            if ((s8)gUnknown_03003340[y][x] >= 0)
            {
                p = gMapData;
                t = y * 2;
                rows = p + 0x417a;
                off = *(u16 *)(rows + t) + x;
                cells = p + 0x1432;
                if ((cells[off] & 0x1f) == 0x11)
                {
                    n++;
                    out->x = x;
                    out->y = y;
                    if ((s8)gUnknown_03003340[y][x] <= 4)
                        out->v = 0;
                    else
                        out->v = (s8)gUnknown_03003340[y][x] - 5;
                    out++;
                }
            }
        }
    }

    out->v = 0xFFFF;
    return n;
}

/* Repeatedly pulls the next candidate cell off sub_08057EC0 and maps it through
 * gMapData's rowOffset table (+0x417A, u16 per row) into the s8 cell
 * table at +0x193A; that byte selects a gPropertyList record.  The first
 * record whose unk03[a2] is not above a1 wins: the counter is bumped and the
 * record returned.  NOTE the return type -- the object really is a
 * `struct PropertyListEntry *`; see the comment in include/unknown-functions.h.
 *
 * `arrp` binds gPropertyList's ADDRESS rather than dereferencing it in
 * place.  That is what puts the `ldr r6, =...` in the loop preheader: written
 * as a bare `gPropertyList[v]` the address load stays inside the loop, and
 * the pool word moves with it. */
struct UnitRecord *sub_08058144(int a1, int a2)
{
    struct Unk08057EC0Rec *e;
    struct PropertyListEntry **arrp;
    struct PropertyListEntry *q;
    u8 *p;
    u8 *rows;
    u8 *cells;
    int t;
    int idx;
    int v;

    arrp = &gPropertyList;

    do
    {
        e = (struct Unk08057EC0Rec *)sub_08057EC0();
        if (e == 0)
            return 0;

        p = gMapData;
        t = e->unk01 * 2;
        rows = p + 0x417A;
        idx = *(u16 *)(rows + t) + e->unk00;
        cells = p + 0x193A;
        v = *(s8 *)(cells + idx);
        q = &(*arrp)[v];
    } while (q->unk03[a2] > a1);

    q->unk03[a2]++;

    return (struct UnitRecord *)q;
}
