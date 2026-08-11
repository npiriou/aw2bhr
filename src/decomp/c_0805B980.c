#include "global.h"

/* Promoted from assembly; each function below is byte-for-byte
 * identical to the original. Order is address order and must
 * stay that way -- the linker places this file's .text as one
 * contiguous block at 0x0805B980.
 * sub_0805B980 @ 0x0805B980, sub_0805BA34 @ 0x0805BA34, sub_0805BAFC @ 0x0805BAFC, sub_0805BB8C @ 0x0805BB8C, sub_0805BBF8 @ 0x0805BBF8
 */

/* sub_08059A0C's shape without the predicate call and without the count: fills
 * the gUnknown_03003F20 scratch list with {x, y, terrain} for every passable
 * map cell whose byte in the 0x3C72 plane is non-zero, and terminates it with a
 * 0xFFFF value halfword. Nothing is returned -- `pop {r0}; bx r0` with no value
 * computed after the store.
 *
 * The 4-byte {u8 x; u8 y; s16 v;} record is c_0804151C.c's layout for this same
 * buffer; gUnknown_03003F20 stays declared `struct Unk03003338 *` and is cast
 * here, exactly as the promoted readers do. Do NOT reshape struct Unk03003338.
 *
 * Cell addressing is sub_080415E4's idiom -- p, then t, then rows, then off,
 * then cells -- with 0x3C72 in place of 0x12. `t = y * 2` and
 * `&gUnknown_03003340[y]` both end up in the OUTER loop's preheader (sl and r4);
 * that is LICM, not source, and neither is authored.
 */

struct Unk5B980Cell
{
    /* 00 */ u8 x;
    /* 01 */ u8 y;
    /* 02 */ s16 v;
};
struct Unk41EA8Map
{
    /* 0x0000 */ u16 width;
    /* 0x0002 */ u16 height;
    /* 0x0004 */ u8 filler_0004[0x0e];
    /* 0x0012 */ u8 unit[0x1420];
    /* 0x1432 */ u8 terrain[0x2D48];
    /* 0x417A */ u16 rowOffset[1];
};
/* The blob struct UnitTypeData's unk14 points at; only the +0x1a table of
 * per-terrain-code permission bytes is proved here. */
struct UnitTypeDataUnk14
{
    /* 0x00 */ u8 filler_00[0x1a];
    /* 0x1a */ u8 terrainOk[0x20];
};

void sub_0805B980(void)
{
    struct Unk5B980Cell *out;
    u8 *p;
    u8 *rows;
    u8 *cells;
    int t;
    int off;
    int x;
    int y;

    out = (struct Unk5B980Cell *)gUnknown_03003F20;

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
                cells = p + 0x3c72;
                if (cells[off] != 0)
                {
                    out->x = x;
                    out->y = y;
                    out->v = (s8)gUnknown_03003340[y][x];
                    out++;
                }
            }
        }
    }

    out->v = 0xFFFF;
}

u8 sub_0805BA34(int x, int y, u16 *out)
{
    struct UnitRecord *unit;
    struct UnitTypeDataUnk14 *tbl;
    int idx;
    int best;

    idx = ((struct Unk41EA8Map *)gMapData)->rowOffset[y] + x;

    if (((struct Unk41EA8Map *)gMapData)->unit[idx] != 0)
        return 0;

    tbl = (struct UnitTypeDataUnk14 *)gUnitTypeData[23].unk14;

    if (tbl->terrainOk[((struct Unk41EA8Map *)gMapData)->terrain[idx]
                       & 0x1f] == 0)
        return 0;

    unit = &gUnitRecords[gUnknown_030040D8->unk07[0]];

    best = 9999;
    out[0] = best;

    sub_0805BAFC(x - 1, y, unit->unk00, out);
    sub_0805BAFC(x + 1, y, unit->unk00, out);
    sub_0805BAFC(x, y - 1, unit->unk00, out);
    sub_0805BAFC(x, y + 1, unit->unk00, out);

    if (out[0] == best)
        return 0;

    return 1;
}

void sub_0805BAFC(int x, int y, int t, u16 *out)
{
    s8 *costs;
    int idx;
    int c;

    if (x < 0)
        return;
    if (y < 0)
        return;

    if (x >= ((struct Unk41EA8Map *)gMapData)->width)
        return;
    if (y >= ((struct Unk41EA8Map *)gMapData)->height)
        return;

    idx = ((struct Unk41EA8Map *)gMapData)->rowOffset[y] + x;

    if (((struct Unk41EA8Map *)gMapData)->unit[idx] != 0)
        return;

    costs = gCoDataTable[1].unk38[0].unk18[0];

    c = (((struct Unk41EA8Map *)gMapData)->terrain[idx] & 0x1f)
        + gUnitTypeData[t].unk19 * 32;

    if (costs[c] == -1)
        return;

    out[0] = x;
    out[1] = y;
}

u8 sub_0805BB8C(int x, int y)
{
    int n;

    if (((struct Unk41EA8Map *)gMapData)
            ->unit[((struct Unk41EA8Map *)gMapData)->rowOffset[y] + x]
        != 0)
        return 0;

    n = sub_0805BBF8(x - 1, y) + sub_0805BBF8(x + 1, y)
        + sub_0805BBF8(x, y - 1) + sub_0805BBF8(x, y + 1);

    if (n > 0)
        return 1;

    return 0;
}

int sub_0805BBF8(int x, int y)
{
    s8 *costs;
    int idx;
    int c;

    if (x < 0)
        return 0;
    if (y < 0)
        return 0;

    if (x >= ((struct Unk41EA8Map *)gMapData)->width)
        return 0;
    if (y >= ((struct Unk41EA8Map *)gMapData)->height)
        return 0;

    idx = ((struct Unk41EA8Map *)gMapData)->rowOffset[y] + x;

    if (((struct Unk41EA8Map *)gMapData)->unit[idx] != 0)
        return 0;

    costs = gCoDataTable[1].unk38[0].unk18[0];

    c = (((struct Unk41EA8Map *)gMapData)->terrain[idx] & 0x1f)
        + gUnitTypeData[1].unk19 * 32;

    if (costs[c] == -1)
        return 0;

    return 1;
}
