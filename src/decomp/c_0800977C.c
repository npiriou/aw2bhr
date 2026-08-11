#include "global.h"

/* Promoted from assembly; each function below is byte-for-byte
 * identical to the original. Order is address order and must
 * stay that way -- the linker places this file's .text as one
 * contiguous block at 0x0800977C.
 * sub_0800977C @ 0x0800977C
 */

/* Returns whether the cell at (x, y) is a join candidate.
 *
 * This names gMapData DIRECTLY, which is what the original source did,
 * so agbcc's -fforce-addr parks the address in this unit's own .rodata.  That
 * word is the ROM's gUnknown_0808D81C and the build now places it there --
 * see tools/split_rodata.py.  The `c_local` workaround spelling is NOT used
 * here: it is what held this function parked, at 75.2% / 38.6% on a register
 * allocation inversion, and naming the global directly is byte-exact.
 */
int sub_0800977C(int x, int y)
{
    int t;
    int terrain;
    u8 ok = 0;

    {
        u8 *p;
        u8 *rows;
        u8 *cells;
        int idx;

        p = gMapData;
        t = y * 2;
        rows = p + 0x417A;
        idx = *(u16 *)(rows + t) + x;
        cells = p + 0x1432;
        terrain = *(cells + idx);
    }

    if (terrain == 0xC)
    {
        int tile;

        ok = sub_08009BF4(x, y) != 0;

        {
            u8 *p;
            u8 *rows;
            u8 *tiles;
            int off;

            p = gMapData;
            rows = p + 0x417A;
            off = (*(u16 *)(rows + t) + x) * 2;
            tiles = p + 0xA22;
            tile = *(u16 *)(tiles + off);
        }

        if (tile == 0x16 || tile == 0x36)
        {
            if (x > 0 && sub_08009720(x - 1, y))
                ok = 0;
            if (x < *(u16 *)gMapData - 1 && sub_08009720(x + 1, y))
                ok = 0;

            if (ok)
            {
                u8 *pe;
                int cnt = 0;

                if (x > 0)
                {
                    u8 *p;
                    u8 *rows;
                    u8 *cells;
                    int t2;
                    int idx;

                    p = gMapData;
                    t2 = y * 2;
                    rows = p + 0x417A;
                    idx = *(u16 *)(rows + t2);
                    idx--;
                    idx += x;
                    cells = p + 0x1432;
                    if (*(cells + idx) == 0xC)
                        cnt = 1;
                }

                pe = gMapData;

                if (x < *(u16 *)pe - 1)
                {
                    u8 *rows;
                    u8 *cells;
                    int t2;
                    int idx;

                    t2 = y * 2;
                    rows = pe + 0x417A;
                    idx = *(u16 *)(rows + t2);
                    idx++;
                    idx += x;
                    cells = pe + 0x1432;
                    if (*(cells + idx) == 0xC)
                        cnt++;
                }
                if (cnt > 1)
                    ok = 0;
            }
        }
        else
        {
            if (y > 0 && sub_08009720(x, y - 1))
                ok = 0;
            if (y < *(u16 *)(gMapData + 2) - 1 && sub_08009720(x, y + 1))
                ok = 0;

            if (ok)
            {
                u8 *pe;
                int cnt = 0;

                if (y > 0)
                {
                    u8 *p;
                    u8 *rows;
                    u8 *cells;
                    int t2;
                    int idx;

                    p = gMapData;
                    t2 = (y - 1) * 2;
                    rows = p + 0x417A;
                    idx = *(u16 *)(rows + t2);
                    idx += x;
                    cells = p + 0x1432;
                    if (*(cells + idx) == 0xC)
                        cnt = 1;
                }

                pe = gMapData;

                if (y < *(u16 *)(pe + 2) - 1)
                {
                    u8 *rows;
                    u8 *cells;
                    int t2;
                    int idx;

                    t2 = (y + 1) * 2;
                    rows = pe + 0x417A;
                    idx = *(u16 *)(rows + t2);
                    idx += x;
                    cells = pe + 0x1432;
                    if (*(cells + idx) == 0xC)
                        cnt++;
                }
                if (cnt > 1)
                    ok = 0;
            }
        }
    }

    return terrain == 2 || ok;
}
