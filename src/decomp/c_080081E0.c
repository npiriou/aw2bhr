#include "global.h"

/* Promoted from assembly; each function below is byte-for-byte
 * identical to the original. Order is address order and must
 * stay that way -- the linker places this file's .text as one
 * contiguous block at 0x080081E0.
 * sub_080081E0 @ 0x080081E0
 */

/* Repaints the eight neighbours of (x, y) that sub_080015E4 accepts, each
 * through the promoted c_08001158.c tile-fetch idiom (rows at +0x417A, tiles
 * at +0xA22).  sub_080083E0 is the same function against the next pool word.
 *
 * This names gMapData DIRECTLY, which is what the original source did,
 * so agbcc's -fforce-addr parks the address in this unit's own .rodata.  That
 * word is the ROM's gUnknown_0808D7F8, and the build now places it there --
 * see tools/split_rodata.py.  The `u8 **const gUnknown_0808D7F8` workaround
 * spelling is NOT used here and is not needed.
 */
void sub_080081E0(int x, int y)
{
    if (y > 0)
    {
        int n = y - 1;

        if (x > 0)
        {
            if (sub_080015E4(x - 1, n))
            {
                u8 *p;
                u8 *rows;
                u8 *tiles;
                int t;
                int off;

                p = gMapData;
                t = n * 2;
                rows = p + 0x417A;
                off = (*(u16 *)(rows + t) + (x - 1)) * 2;
                tiles = p + 0xA22;
                sub_08007F14(x - 1, n, *(u16 *)(tiles + off));
            }
        }
        if (sub_080015E4(x, n))
        {
            u8 *p;
            u8 *rows;
            u8 *tiles;
            int t;
            int off;

            p = gMapData;
            t = n * 2;
            rows = p + 0x417A;
            off = (*(u16 *)(rows + t) + (x)) * 2;
            tiles = p + 0xA22;
            sub_08007F14(x, n, *(u16 *)(tiles + off));
        }
        if (x < *(u16 *)gMapData - 1)
        {
            if (sub_080015E4(x + 1, n))
            {
                u8 *p;
                u8 *rows;
                u8 *tiles;
                int t;
                int off;

                p = gMapData;
                t = n * 2;
                rows = p + 0x417A;
                off = (*(u16 *)(rows + t) + (x + 1)) * 2;
                tiles = p + 0xA22;
                sub_08007F14(x + 1, n, *(u16 *)(tiles + off));
            }
        }
    }
    if (x > 0)
    {
        if (sub_080015E4(x - 1, y))
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
            sub_08007F14(x - 1, y, *(u16 *)(tiles + off));
        }
    }
    if (x < *(u16 *)gMapData - 1)
    {
        if (sub_080015E4(x + 1, y))
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
            sub_08007F14(x + 1, y, *(u16 *)(tiles + off));
        }
    }
    if (y < *(u16 *)(gMapData + 2) - 1)
    {
        int n = y + 1;

        if (x > 0)
        {
            if (sub_080015E4(x - 1, n))
            {
                u8 *p;
                u8 *rows;
                u8 *tiles;
                int t;
                int off;

                p = gMapData;
                t = n * 2;
                rows = p + 0x417A;
                off = (*(u16 *)(rows + t) + (x - 1)) * 2;
                tiles = p + 0xA22;
                sub_08007F14(x - 1, n, *(u16 *)(tiles + off));
            }
        }
        if (sub_080015E4(x, n))
        {
            u8 *p;
            u8 *rows;
            u8 *tiles;
            int t;
            int off;

            p = gMapData;
            t = n * 2;
            rows = p + 0x417A;
            off = (*(u16 *)(rows + t) + (x)) * 2;
            tiles = p + 0xA22;
            sub_08007F14(x, n, *(u16 *)(tiles + off));
        }
        if (x < *(u16 *)gMapData - 1)
        {
            if (sub_080015E4(x + 1, n))
            {
                u8 *p;
                u8 *rows;
                u8 *tiles;
                int t;
                int off;

                p = gMapData;
                t = n * 2;
                rows = p + 0x417A;
                off = (*(u16 *)(rows + t) + (x + 1)) * 2;
                tiles = p + 0xA22;
                sub_08007F14(x + 1, n, *(u16 *)(tiles + off));
            }
        }
    }
}
