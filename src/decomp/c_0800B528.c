#include "global.h"

/* Promoted from assembly; each function below is byte-for-byte
 * identical to the original. Order is address order and must
 * stay that way -- the linker places this file's .text as one
 * contiguous block at 0x0800B528.
 * sub_0800B528 @ 0x0800B528
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

int sub_0800B528(int x, int y)
{
    int n;

    if (MAP->terrain[MAP->rowOffset[y] + x] == 0x13 || sub_080094EC(x, y))
        return -1;

    n = 0;

    if (y > 1)
        n = sub_080015E4(x, y - 1);

    if (y < MAP->height - 1)
        n += sub_080015E4(x, y + 1);

    if (x > 0)
        n += sub_080015E4(x - 1, y);

    if (x < MAP->width - 1)
        n += sub_080015E4(x + 1, y);

    if (n == 4)
        n = -1;

    return n;
}
