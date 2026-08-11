#include "global.h"

/* Promoted from assembly; each function below is byte-for-byte
 * identical to the original. Order is address order and must
 * stay that way -- the linker places this file's .text as one
 * contiguous block at 0x08008D70.
 * sub_08008D70 @ 0x08008D70
 */

int sub_08008D70(int x, int y)
{
    if (y < *(u16 *)(gMapData + 2) - 1)
    {
        u8 *p;
        u8 *rows;
        u8 *tiles;
        int n;
        int t;
        int off;

        p = gMapData;
        n = y + 1;
        t = n * 2;
        rows = p + 0x417A;
        off = (*(u16 *)(rows + t) + x) * 2;
        tiles = p + 0xA22;

        if (*(u16 *)(tiles + off) == 0x36)
            return 0x36;
    }

    if (y > 0)
    {
        u8 *p;
        u8 *rows;
        u8 *tiles;
        int n;
        int t;
        int off;

        p = gMapData;
        n = y - 1;
        t = n * 2;
        rows = p + 0x417A;
        off = (*(u16 *)(rows + t) + x) * 2;
        tiles = p + 0xA22;

        if (*(u16 *)(tiles + off) == 0x36)
            return 0x36;
    }

    if (x < *(u16 *)gMapData - 1)
    {
        u8 *p;
        u8 *rows;
        u8 *tiles;
        int t;
        int off;

        p = gMapData;
        t = y * 2;
        rows = p + 0x417A;
        off = (*(u16 *)(rows + t) + (x + 1)) * 2;
        tiles = p + 0xA22;

        if (*(u16 *)(tiles + off) == 0x14)
            return 0x14;
    }

    if (x > 0)
    {
        u8 *p;
        u8 *rows;
        u8 *tiles;
        int t;
        int off;

        p = gMapData;
        t = y * 2;
        rows = p + 0x417A;
        off = (*(u16 *)(rows + t) + (x - 1)) * 2;
        tiles = p + 0xA22;

        if (*(u16 *)(tiles + off) == 0x14)
            return 0x14;
    }

    return -1;
}
