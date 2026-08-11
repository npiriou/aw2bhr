#include "global.h"

/* Promoted from assembly; each function below is byte-for-byte
 * identical to the original. Order is address order and must
 * stay that way -- the linker places this file's .text as one
 * contiguous block at 0x0805A268.
 * sub_0805A268 @ 0x0805A268, sub_0805A388 @ 0x0805A388
 */

/* sub_0805A514's sibling (src/decomp/c_0805A514.c) and a producer for the same
 * cell list sub_0805A744 consumes: sweeps the 0x40-unit window at
 * gUnknown_03003F2C and emits {x, y, threat} for every unit that passes, then
 * terminates the array with 0xFFFF exactly as sub_0805A514 does.
 *
 * Two disjoint acceptance arms sharing one emit body. The first takes units
 * sub_08042084 accepts whose class byte is live in gUnknown_084995A8; the
 * second is the fallback and only opens when the active player's class byte is
 * 0x13 or 0x14 -- spelled as the ROM's `(u8)(c - 0x13) <= 1`, one subtract and
 * a byte truncation rather than two compares. THE EMIT BODY IS WRITTEN OUT
 * TWICE ON PURPOSE: agbcc cross-jumps the two copies and the shared tail it
 * produces (from the second `u->unk03` reload onward, reached by a `b`) is
 * exactly the ROM's. A single shared body cannot produce that layout.
 *
 * NEITHER gUnknown_0816D984 NOR gUnknown_0816D988 IS A GLOBAL. The ROM words
 * hold 0x030040D8 and 0x03003F2C -- the ADDRESSES of gUnknown_030040D8 and
 * gUnknown_03003F2C, checked in baserom.gba. Both are -fforce-addr copies (each
 * object is read on both sides of a merge), the pattern c_0805A95C.c
 * documents. Named honestly here; promotion must carry
 * "rodata": ["0x0816D984", "0x0816D988"].
 *
 * `flag` holds 0 / -1 rather than 0 / 1: the ROM builds it with
 * `movs r1,#1; rsbs r1,r1,#0`, so the original wrote -1.
 *
 * MATCHED, 1 attempt after 1 probe. */

struct Unk5A514Cell
{
    /* 0x00 */ u8 x;
    /* 0x01 */ u8 y;
    /* 0x02 */ s16 v;
};

void sub_0805A268(struct Unk5A514Cell *out)
{
    struct UnitRecord *u;
    int i;
    int flag;

    flag = 0;
    if ((u8)(gUnknown_030040D8->unk00 - 0x13) <= 1)
        flag = -1;

    for (i = gUnknown_03003F2C; i < gUnknown_03003F2C + 0x40; i++)
    {
        u = &gUnitRecords[i];
        if (u->unk00 == 0)
            continue;

        if (sub_08042084((u8 *)u) && gUnknown_084995A8[gUnknown_030040D8->unk00] != 0)
        {
            if ((s8)gUnknown_03003340[u->unk03][u->unk02] < 0)
                continue;
            if (u->unk01 & 8)
                continue;
            out->x = u->unk02;
            out->y = u->unk03;
            out->v = (s8)gUnknown_03003340[u->unk03][u->unk02];
            out++;
        }
        else
        {
            if (flag == 0)
                continue;
            if (u->unk00 != 0x16)
                continue;
            if (u->unk08 != 0)
                continue;
            if ((s8)gUnknown_03003340[u->unk03][u->unk02] < 0)
                continue;
            out->x = u->unk02;
            out->y = u->unk03;
            out->v = (s8)gUnknown_03003340[u->unk03][u->unk02];
            out++;
        }
    }

    out->v = 0xFFFF;
}

void sub_0805A388(struct Unk5A514Cell *out)
{
    struct UnitRecord *u;
    u8 *q;
    u8 *rows;
    u8 *cells;
    u8 *props;
    u8 *cell;
    u8 *tbl;
    int x;
    int y;
    int t;
    int off;
    int flag;
    int key;

    flag = 0;
    key = gUnknown_08576841[gUnknown_030040D8->unk00];
    if ((u8)(gUnknown_030040D8->unk00 - 0x13) <= 1)
        flag = -1;

    for (y = 0; y < *(u16 *)(gMapData + 2); y++)
    {
        for (x = 0; x < *(u16 *)gMapData; x++)
        {
            if ((s8)gUnknown_03003340[y][x] < 0)
                continue;

            q = gMapData;
            t = y * 2;
            rows = q + 0x417a;
            off = *(u16 *)(rows + t) + x;
            props = q + 0x12;
            cell = props + off;

            if (*cell != 0)
            {
                if ((*cell & 0xc0) != gUnknown_03003F2C)
                    continue;
                u = &gUnitRecords[*cell];
                if (flag == 0 && sub_08042084((u8 *)u)
                 && gUnknown_084995A8[gUnknown_030040D8->unk00] != 0)
                {
                    out->x = x;
                    out->y = y;
                    out->v = (s8)gUnknown_03003340[y][x];
                    out++;
                }
                else
                {
                    if (flag == 0)
                        continue;
                    if (u->unk00 != 0x16)
                        continue;
                    if (u->unk07 != 0)
                        continue;
                    out->x = x;
                    out->y = y;
                    out->v = (s8)gUnknown_03003340[y][x];
                    out++;
                }
            }
            else
            {
                tbl = gUnknown_0857685A;
                cells = q + 0x1432;
                if (tbl[cells[off] & 0x1f] != key)
                    continue;
                if ((cells[off] & 0xe0) != gUnknown_03004084)
                    continue;
                out->x = x;
                out->y = y;
                out->v = (s8)gUnknown_03003340[y][x];
                out++;
            }
        }
    }

    out->v = 0xFFFF;
}
