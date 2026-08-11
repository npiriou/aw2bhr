#include "global.h"

/* Promoted from assembly; each function below is byte-for-byte
 * identical to the original. Order is address order and must
 * stay that way -- the linker places this file's .text as one
 * contiguous block at 0x0805BD40.
 * sub_0805BD40 @ 0x0805BD40
 */

struct Unk41EA8Map
{
    /* 0x0000 */ u16 width;
    /* 0x0002 */ u16 height;
    /* 0x0004 */ u8 filler_0004[0x0e];
    /* 0x0012 */ u8 unit[0x1420];
    /* 0x1432 */ u8 terrain[0x2D48];
    /* 0x417A */ u16 rowOffset[1];
};

int sub_0805BD40(int x, int y, int t, int id, s16 *out)
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

    if (id == gUnknown_030045C8)
        return 0;

    idx = ((struct Unk41EA8Map *)gMapData)->rowOffset[y] + x;

    if (((struct Unk41EA8Map *)gMapData)->unit[idx] != 0)
        return 0;

    costs = gCoDataTable[1].unk38[0].unk18[0];

    c = (((struct Unk41EA8Map *)gMapData)->terrain[idx] & 0x1f)
        + gUnitTypeData[t].unk19 * 32;

    if (costs[c] != -1)
    {
        out[0] = x;
        out[1] = y;
        return 1;
    }

    return 0;
}
