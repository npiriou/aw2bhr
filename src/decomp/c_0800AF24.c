#include "global.h"

/* Promoted from assembly; each function below is byte-for-byte
 * identical to the original. Order is address order and must
 * stay that way -- the linker places this file's .text as one
 * contiguous block at 0x0800AF24.
 * sub_0800AF24 @ 0x0800AF24, sub_0800AF74 @ 0x0800AF74, sub_0800AFCC @ 0x0800AFCC
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

void sub_0800AF24(int x, int y)
{
    if (y > 0)
    {
        int n = y - 1;
        if (sub_0800119C(x, n, 3))
            sub_0800B048(x, n);
    }

    if (y < MAP->height - 1)
    {
        int n = y + 1;
        if (sub_0800119C(x, n, 3))
            sub_0800B048(x, n);
    }
}

void sub_0800AF74(int x, int y)
{
    if (sub_0800168C(x, y) == 0)
        sub_080011F4(x, y, 1);

    sub_080011F4(x, y, 3);
    sub_0800B048(x, y);
    sub_0800A588(x, y);
    sub_0800ABD0(x, y);
    sub_08007F9C(x, y);
    sub_0800BEE4(x, y);
    sub_0800EC20(x, y);
}

int sub_0800AFCC(int x, int y)
{
    switch (MAP->terrain[MAP->rowOffset[y] + x] & 0x1f)
    {
    case 3:
    case 4:
    case 6:
    case 8:
    case 10:
    case 11:
    case 14:
        return 1;
    }

    return 0;
}
