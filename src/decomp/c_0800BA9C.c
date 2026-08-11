#include "global.h"

/* Promoted from assembly; each function below is byte-for-byte
 * identical to the original. Order is address order and must
 * stay that way -- the linker places this file's .text as one
 * contiguous block at 0x0800BA9C.
 * sub_0800BA9C @ 0x0800BA9C, sub_0800BB2C @ 0x0800BB2C
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

int sub_0800BA9C(int x, int y)
{
    int t;
    int v;

    if (sub_0800B528(x, y) < 0)
        return 0;

    sub_08007CA0(x, y);
    t = MAP->terrain[MAP->rowOffset[y] + x];
    sub_080011F4(x, y, 0xd);

    v = sub_0800B61C(x, y);

    if (v < 0)
    {
        sub_080011F4(x, y, t);
        return 0;
    }

    sub_08001158(x, y, v);

    if (sub_0800119C(x, y, 7) == 0)
        sub_0800BB2C(x, y);

    return 1;
}

void sub_0800BB2C(int x, int y)
{
    int k = 0x2a;

    if (y > 0)
    {
        int n = y - 1;
        if (sub_0800B4F0(x, n))
        {
            int v = sub_0800B61C(x, n);
            if (v < 0)
            {
                sub_080011F4(x, n, 7);
                sub_08001158(x, n, k);
            }
            else
                sub_08001158(x, n, v);
        }
    }

    if (x > 0)
    {
        int n = x - 1;
        if (sub_0800B4F0(n, y))
        {
            int v = sub_0800B61C(n, y);
            if (v < 0)
            {
                sub_080011F4(n, y, 7);
                sub_08001158(n, y, k);
            }
            else
                sub_08001158(n, y, v);
        }
    }

    if (x < MAP->width - 1)
    {
        int n = x + 1;
        if (sub_0800B4F0(n, y))
        {
            int v = sub_0800B61C(n, y);
            if (v < 0)
            {
                sub_080011F4(n, y, 7);
                sub_08001158(n, y, k);
            }
            else
                sub_08001158(n, y, v);
        }
    }

    if (y < MAP->height - 1)
    {
        int n = y + 1;
        if (sub_0800B4F0(x, n))
        {
            int v = sub_0800B61C(x, n);
            if (v < 0)
            {
                sub_080011F4(x, n, 7);
                sub_08001158(x, n, k);
            }
            else
                sub_08001158(x, n, v);
        }
    }
}
