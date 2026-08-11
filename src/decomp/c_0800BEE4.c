#include "global.h"

/* Promoted from assembly; each function below is byte-for-byte
 * identical to the original. Order is address order and must
 * stay that way -- the linker places this file's .text as one
 * contiguous block at 0x0800BEE4.
 * sub_0800BEE4 @ 0x0800BEE4
 */

struct MapScreen
{
    /* 0x0000 */ u16 width;
    /* 0x0002 */ u16 height;
    /* 0x0004 */ u8 filler_0004[0x0A22 - 4];
    /* 0x0A22 */ u16 cells[(0x1432 - 0x0A22) / 2];
    /* 0x1432 */ u8 terrain[0x417A - 0x1432];
    /* 0x417A */ u16 rowOffset[1];
};
#define MAP ((struct MapScreen *)gMapData)

void sub_0800BEE4(int x, int y)
{
    if (y > 0)
    {
        int n = y - 1;
        if (x > 0)
            sub_0800BEB8(x - 1, n);
        sub_0800BEB8(x, n);
        if (x < MAP->width - 1)
            sub_0800BEB8(x + 1, n);
    }

    if (x > 0)
        sub_0800BEB8(x - 1, y);

    if (x < MAP->width - 1)
        sub_0800BEB8(x + 1, y);

    if (y < MAP->height - 1)
    {
        int n = y + 1;
        if (x > 0)
            sub_0800BEB8(x - 1, n);
        sub_0800BEB8(x, n);
        if (x < MAP->width - 1)
            sub_0800BEB8(x + 1, n);
    }
}
