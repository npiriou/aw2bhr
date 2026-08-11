#include "global.h"

/* Promoted from assembly; each function below is byte-for-byte
 * identical to the original. Order is address order and must
 * stay that way -- the linker places this file's .text as one
 * contiguous block at 0x0802D5E8.
 * sub_0802D5E8 @ 0x0802D5E8
 */

void sub_0802D5E8(s16 a1, s16 a2)
{
    u8 *p;
    u8 *rows;
    u8 *tiles;
    int t;
    int off;
    int v;

    v = 0;
    p = gMapData;
    t = a2 * 2;
    rows = p + 0x417A;
    off = *(u16 *)(rows + t);
    off = a1 + off;
    tiles = p + 0x1432;

    switch (tiles[off] & 0x1f)
    {
    case 6:
    case 8:
    case 14:
        v = 7;
        break;

    case 10:
        v = 0x10;
        break;

    case 11:
        v = 0x20;
        break;
    }

    sub_0802D67C(v);
    sub_080152C0((s32)gUnknown_0849AFE8, 0);
    sub_08034F7C();
}
