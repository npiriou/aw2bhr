#include "global.h"

/* Promoted from assembly; each function below is byte-for-byte
 * identical to the original. Order is address order and must
 * stay that way -- the linker places this file's .text as one
 * contiguous block at 0x08007BA4.
 * sub_08007BA4 @ 0x08007BA4, sub_08007C04 @ 0x08007C04, sub_08007CA0 @ 0x08007CA0
 */

void sub_08007BA4(int x, int y)
{
    int v;

    v = sub_08007DD0(x, y);

    if (v == 0)
    {
        v = 0x2A;
    }
    else if (v < 0)
    {
        u8 *p;
        u8 *rows;
        u8 *cells;
        int t;
        int idx;

        p = gMapData;
        t = y * 2;
        rows = p + 0x417A;
        idx = *(u16 *)(rows + t) + x;
        cells = p + 0x1432;
        if (*(cells + idx) == 7)
            v = 0x2A;
    }

    sub_08001158(x, y, v);

    if (v == 0x2A)
        sub_080011F4(x, y, 7);
}

void sub_08007C04(int x, int y)
{
    if (y > 0)
    {
        int n = y - 1;

        if (x > 0)
            sub_08007BA4(x - 1, n);

        sub_08007BA4(x, n);

        if (x < *(u16 *)gMapData - 1)
            sub_08007BA4(x + 1, n);
    }

    if (x > 0)
        sub_08007BA4(x - 1, y);

    sub_08007BA4(x, y);

    if (x < *(u16 *)gMapData - 1)
        sub_08007BA4(x + 1, y);

    if (y < *(u16 *)(gMapData + 2) - 1)
    {
        y++;

        if (x > 0)
            sub_08007BA4(x - 1, y);

        sub_08007BA4(x, y);

        if (x < *(u16 *)gMapData - 1)
            sub_08007BA4(x + 1, y);
    }
}

void sub_08007CA0(int x, int y)
{
    if (sub_0800119C(x, y, 7))
        return;

    if (sub_0800C840(x, y))
        sub_0800C608(x, y);

    if (sub_0800977C(x, y))
    {
        if (sub_080094EC(x, y))
            sub_08001158(x, y, 0x2A);
        else
            sub_08007F14(x, y, 1);
    }

    if (sub_0800119C(x, y, 0x13))
    {
        sub_080011F4(x, y, 7);
        sub_08001158(x, y, 0x2A);
    }
    else if (sub_08008C34(x, y))
    {
        sub_080011F4(x, y, 7);
        sub_08007BA4(x, y);
    }
    else
    {
        sub_080011F4(x, y, 7);
        sub_08007C04(x, y);
        sub_0800A588(x, y);
        sub_0800ABD0(x, y);
        sub_08007F9C(x, y);
        sub_0800EC20(x, y);
    }
}
