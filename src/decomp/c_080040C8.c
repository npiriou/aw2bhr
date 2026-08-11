#include "global.h"

/* Promoted from assembly; each function below is byte-for-byte
 * identical to the original. Order is address order and must
 * stay that way -- the linker places this file's .text as one
 * contiguous block at 0x080040C8.
 * sub_080040C8 @ 0x080040C8, sub_0800449C @ 0x0800449C
 */

struct Unk3F44Map
{
    /* 0x0000 */ u16 width;
    /* 0x0002 */ u16 height;
    /* 0x0004 */ u8 filler_0004[0x0A22 - 0x0004];
    /* 0x0A22 */ u16 plane[(0x1432 - 0x0A22) / 2];
    /* 0x1432 */ u8 cell[0x417A - 0x1432];
    /* 0x417A */ u16 rowOffset[1];
};
#define MAP ((struct Unk3F44Map *)gMapData)

void sub_080040C8(void)
{
    int x, y;
    int i, n;
    int lim, cnt;
    int a, b, w, h;
    int t;
    int r;

    t = 7;
    for (y = 0; y < MAP->height; y++)
    {
        for (x = 0; x < MAP->width; x++)
        {
            MAP->plane[MAP->rowOffset[y] + x] = 0x2A;
            MAP->cell[MAP->rowOffset[y] + x] = t;
        }
    }

    n = sub_08003B6C(0xC, 8);
    for (i = n; i > 0; i--)
    {
        a = sub_08003B6C(0x1C, 2);
        b = sub_08003B6C(0x12, 2);
        w = sub_08003B6C(0xA, 2);
        h = sub_08003B6C(0xC - w, 2);
        sub_0800401C(a, b, w, h, 1);
    }

    n = sub_08003B6C(0xC, 8);
    for (i = n; i > 0; i--)
    {
        a = sub_08003B6C(0x1C, 2);
        b = sub_08003B6C(0x12, 2);
        w = sub_08003B6C(0xA, 4);
        h = sub_08003B6C(0xE - w, 4);
        sub_0800401C(a, b, w, h, 1);
    }

    lim = sub_08003B6C(0x32, 0x28);
    cnt = 0;
    for (i = 100; i > 0; i--)
    {
        a = sub_08003B6C(0x1A, 4);
        b = sub_08003B6C(0x10, 4);
        if (MAP->cell[MAP->rowOffset[b] + a] == 1)
        {
            sub_080011F4(a, b, 3);
            sub_08007F14(a, b, 0x20);
            cnt++;
        }
        if (cnt > lim)
            break;
    }

    n = sub_08003B6C(6, 3);
    for (i = n; i > 0; i--)
    {
        a = sub_08003B6C(0x1C, 2);
        b = sub_08003B6C(0x12, 2);
        w = sub_08003B6C(4, 1);
        h = sub_08003B6C(5 - w, 1);
        sub_0800401C(a, b, w, h, 0x20);
    }

    n = 7 - n;
    for (i = n; i > 0; i--)
    {
        a = sub_08003B6C(0x1C, 2);
        b = sub_08003B6C(0x12, 2);
        w = sub_08003B6C(4, 2);
        h = sub_08003B6C(6 - w, 2);
        sub_0800401C(a, b, w, h, 0x20);
    }

    lim = sub_08003B6C(0x32, 0x28);
    cnt = 0;
    for (i = 0x32; i > 0; i--)
    {
        a = sub_08003B6C(0x1A, 4);
        b = sub_08003B6C(0x10, 4);
        if (MAP->cell[MAP->rowOffset[b] + a] == 1)
        {
            sub_080011F4(a, b, 4);
            sub_08007F14(a, b, 0x87);
            cnt++;
        }
        if (cnt > lim)
            break;
    }

    lim = sub_08003B6C(0x32, 0x28);
    cnt = 0;
    for (i = 100; i > 0; i--)
    {
        a = sub_08003B6C(0x1C, 2);
        b = sub_08003B6C(0x12, 2);
        if (sub_0800BCD0(a, b) != 0)
        {
            sub_080011F4(a, b, 0x13);
            sub_08007F14(a, b, 0x168);
            cnt++;
        }
        if (cnt > lim)
            break;
    }

    for (y = 1; y < MAP->height - 1; y++)
    {
        for (x = n = 1; x < MAP->width - 1; x++)
        {
            if (sub_08003B6C(0xC8, 0x64) > 0x95
             && MAP->cell[MAP->rowOffset[y] + x] == 1)
            {
                if (MAP->cell[MAP->rowOffset[y - 1] + x] == 7)
                    sub_0800BA9C(x, y - 1);
                if (MAP->cell[MAP->rowOffset[y + 1] + x] == 7)
                    sub_0800BA9C(x, y + 1);
                r = MAP->rowOffset[y] - 1;
                if (MAP->cell[r + x] == 7)
                    sub_0800BA9C(x - 1, y);
                r = MAP->rowOffset[y] + 1;
                if (MAP->cell[r + x] == 7)
                    sub_0800BA9C(x + 1, y);
            }
        }
    }
}

void sub_0800449C(void)
{
    int x, y;
    int i, n;
    int lim, cnt;
    int a, b, w, h;
    int t;

    t = 7;
    for (y = 0; y < MAP->height; y++)
    {
        for (x = 0; x < MAP->width; x++)
        {
            MAP->plane[MAP->rowOffset[y] + x] = 0x2A;
            MAP->cell[MAP->rowOffset[y] + x] = t;
        }
    }

    for (i = sub_08003B6C(0x1E, 0x19); i > 0; i--)
    {
        a = sub_08003B6C(0x1E, 0);
        b = sub_08003B6C(0x1E, 0);
        w = sub_08003B6C(0xA, 6);
        h = sub_08003B6C(0x10 - w, 6);
        sub_0800401C(a, b, w, h, 1);
    }

    for (i = sub_08003B6C(0x1E, 0x19); i > 0; i--)
    {
        a = sub_08003B6C(0x1E, 0);
        b = sub_08003B6C(0x1E, 0);
        w = sub_08003B6C(0xC, 4);
        h = sub_08003B6C(0x10 - w, 4);
        sub_0800401C(a, b, w, h, 1);
    }

    lim = sub_08003B6C(100, 90);
    cnt = 0;
    for (i = 100; i > 0; i--)
    {
        a = sub_08003B6C(0x1D, 0);
        b = sub_08003B6C(0x13, 0);
        if (MAP->cell[MAP->rowOffset[b] + a] == 1)
        {
            sub_080011F4(a, b, 3);
            sub_08007F14(a, b, 0x20);
            cnt++;
        }
        if (cnt > lim)
            break;
    }

    n = sub_08003B6C(0xA, 7);
    for (i = n; i > 0; i--)
    {
        a = sub_08003B6C(0x1C, 2);
        b = sub_08003B6C(0x12, 2);
        w = sub_08003B6C(4, 2);
        h = sub_08003B6C(6 - w, 2);
        sub_0800401C(a, b, w, h, 0x20);
    }

    for (i = 12 - n; i > 0; i--)
    {
        a = sub_08003B6C(0x1C, 2);
        b = sub_08003B6C(0x12, 2);
        w = sub_08003B6C(4, 2);
        h = sub_08003B6C(6 - w, 2);
        sub_0800401C(a, b, w, h, 0x20);
    }

    lim = sub_08003B6C(100, 0x50);
    cnt = 0;
    for (i = 100; i > 0; i--)
    {
        a = sub_08003B6C(0x1A, 4);
        b = sub_08003B6C(0x10, 4);
        if (MAP->cell[MAP->rowOffset[b] + a] == 1)
        {
            sub_080011F4(a, b, 4);
            sub_08007F14(a, b, 0x87);
            cnt++;
        }
        if (cnt > lim)
            break;
    }
}
