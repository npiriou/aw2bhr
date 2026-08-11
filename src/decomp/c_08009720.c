#include "global.h"

/* Promoted from assembly; each function below is byte-for-byte
 * identical to the original. Order is address order and must
 * stay that way -- the linker places this file's .text as one
 * contiguous block at 0x08009720.
 * sub_08009720 @ 0x08009720
 */

int sub_08009720(int x, int y)
{
    u8 *p;
    u8 *rows;
    u8 *cells;
    int t;
    int idx;
    int terrain;
    int r = 0;

    p = gMapData;
    t = y * 2;
    rows = p + 0x417A;
    idx = *(u16 *)(rows + t) + x;
    cells = p + 0x1432;
    terrain = *(cells + idx);

    switch (terrain)
    {
    case 7:
    case 0xD:
    case 0x13:
        r = 1;
        break;
    case 2:
        r = sub_080094EC(x, y);
        break;
    }

    return r;
}
