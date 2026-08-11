#include "global.h"

/* Promoted from assembly; each function below is byte-for-byte
 * identical to the original. Order is address order and must
 * stay that way -- the linker places this file's .text as one
 * contiguous block at 0x0800F318.
 * sub_0800F318 @ 0x0800F318, sub_0800F368 @ 0x0800F368, sub_0800F3B8 @ 0x0800F3B8, sub_0800F418 @ 0x0800F418, sub_0800F4E0 @ 0x0800F4E0
 */

int sub_0800F318(int x, int y)
{
    u8 *p;
    u8 *rows;
    u8 *cells;
    u8 *tiles;
    int t;
    int idx;
    int off;
    int terrain;
    int tile;
    int r;

    p = gMapData;
    t = y * 2;
    rows = p + 0x417A;
    idx = *(u16 *)(rows + t) + x;
    cells = p + 0x1432;
    terrain = *(cells + idx);
    off = idx * 2;
    tiles = p + 0xA22;
    tile = *(u16 *)(tiles + off);

    r = 0;
    if (terrain == 5 || (terrain == 0xC && (tile == 0x13 || tile == 0x14)))
        r = 1;
    return r;
}

int sub_0800F368(int x, int y)
{
    u8 *p;
    u8 *rows;
    u8 *cells;
    u8 *tiles;
    int t;
    int idx;
    int off;
    int terrain;
    int tile;
    int r;

    p = gMapData;
    t = y * 2;
    rows = p + 0x417A;
    idx = *(u16 *)(rows + t) + x;
    cells = p + 0x1432;
    terrain = *(cells + idx);
    off = idx * 2;
    tiles = p + 0xA22;
    tile = *(u16 *)(tiles + off);

    r = 0;
    if (terrain == 5 || (terrain == 0xC && (tile == 0x16 || tile == 0x36)))
        r = 1;
    return r;
}

int sub_0800F3B8(int x, int y)
{
    int m = 0;

    if (y > 0)
        m |= sub_0800F368(x, y - 1) << 7;
    if (x > 0)
        m |= sub_0800F318(x - 1, y) << 5;
    if (x < *(u16 *)gMapData - 1)
        m |= sub_0800F318(x + 1, y) << 3;
    if (y < *(u16 *)(gMapData + 2) - 1)
        m |= sub_0800F368(x, y + 1) << 1;
    return m;
}

int sub_0800F418(int x, int y)
{
    int m = 0;

    if (y > 0)
    {
        int ny = y - 1;

        m |= sub_0800F368(x, ny) << 7;
        if (x < *(u16 *)gMapData - 1)
            m |= sub_0800F2E0(x + 1, ny) << 6;
    }
    if (x > 0)
        m |= sub_0800F318(x - 1, y) << 5;
    m |= sub_0800F2E0(x, y) << 4;
    if (x < *(u16 *)gMapData - 1)
        m |= sub_0800F318(x + 1, y) << 3;
    if (y < *(u16 *)(gMapData + 2) - 1)
    {
        y++;
        if (x > 0)
            m |= sub_0800F2E0(x - 1, y) << 2;
        m |= sub_0800F368(x, y) << 1;
        if (x < *(u16 *)gMapData - 1)
            m |= sub_0800F2E0(x + 1, y);
    }
    return gUnknown_084865C4[m];
}

void sub_0800F4E0(int x, int y)
{
    if (sub_0800119C(x, y, 2) || sub_0800119C(x, y, 0xC))
    {
        sub_08009264(x, y);
    }
    else
    {
        sub_080011F4(x, y, 5);
        sub_08001158(x, y, sub_0800F418(x, y));
        sub_08001158(x, y, sub_080016D0(x, y));
        sub_0800A588(x, y);
        sub_0800ABD0(x, y);
        sub_08007F9C(x, y);
        sub_0800BEE4(x, y);
        sub_0800EC20(x, y);
    }
}
