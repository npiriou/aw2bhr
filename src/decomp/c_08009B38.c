#include "global.h"

/* Promoted from assembly; each function below is byte-for-byte
 * identical to the original. Order is address order and must
 * stay that way -- the linker places this file's .text as one
 * contiguous block at 0x08009B38.
 * sub_08009B38 @ 0x08009B38, sub_08009B84 @ 0x08009B84
 */

int sub_08009B38(int x, int y)
{
    u8 *p;
    u8 *rows;
    u8 *cells;
    int t;
    int idx;
    int terrain;

    p = gMapData;
    t = y * 2;
    rows = p + 0x417A;
    idx = *(u16 *)(rows + t) + x;
    cells = p + 0x1432;
    terrain = *(cells + idx);

    if (terrain == 2)
        return sub_080094EC(x, y) == 0;
    return 0;
}

int sub_08009B84(int x, int y)
{
    int v;
    int w;
    int m;

    v = sub_0800A6AC(x, y);
    if (v == 0)
    {
        v = sub_0800A884(x, y);
        if (v != 0)
        {
            w = sub_0800A95C(x, y);
            if (w != 0)
                v = w;
        }
    }
    else
    {
        w = sub_0800A95C(x, y);
        if (w != 0)
        {
            m = v & 0xFE00;
            if (m == 0x4000)
                v = w;
            if (m == 0x2000 && (v & 0xFE00) == m)
                v = w;
            if (m == 0)
                v = w;
        }
    }
    return v;
}
