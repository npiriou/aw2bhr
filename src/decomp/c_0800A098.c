#include "global.h"

/* Promoted from assembly; each function below is byte-for-byte
 * identical to the original. Order is address order and must
 * stay that way -- the linker places this file's .text as one
 * contiguous block at 0x0800A098.
 * sub_0800A098 @ 0x0800A098, sub_0800A2EC @ 0x0800A2EC
 */

/* MATCHED (wave 37, W37-C).
 *
 * Four one-sided repairs around (x, y), gated on the cell's own tile: the two
 * horizontal neighbours when it is 0x39, the two vertical ones when it is
 * 0x18.  Each arm needs the neighbour to be free (sub_0800164C), the ORIGINAL
 * cell still to read as that tile, and sub_0800AA30 for that direction to be
 * clear; then it stamps kind 2 with a fixed tile and, when the terrain byte one
 * row up and one column left/right is 0xD, calls sub_0800BA9C on it.
 *
 * `c` is spilled to the stack because its live range spans both halves; the
 * inner re-reads of the same cell are in the ROM too, at the same index.
 *
 * THE COMMA IS LOAD-BEARING and is a reproduction of what CSE did, not the
 * original source.  The terrain index is `(rowOffset[ny] -+ 1) + x`, with the
 * constant applied to the ROW and x added afterwards.  Written inline agbcc
 * folds it two other ways, both wrong and both measured on this function:
 *   - `rowOffset[ny] + x - 1` reassociates to `(rowOffset + x) - 1` and sinks
 *     the -1 into the terrain base, emitting `ldr =0x1431` where the ROM has
 *     `ldr =0x1432`;
 *   - `rowOffset[ny] - 1 + x` reassociates to `rowOffset + (x - 1)`, which
 *     then CSEs with the `x - 1` this function already has -- with `nx` in the
 *     two 0x39 arms (2 bytes short each, 592 of 596) and with sub_0800BA9C's
 *     own argument in the two 0x18 arms (right size, wrong registers);
 *   - binding `int row = rowOffset[ny] - 1;` as a STATEMENT fixes the
 *     arithmetic but hoists the whole row load above the sub_080011F4 /
 *     sub_08001158 pair, where the ROM has it after them.
 * `(t = <row expr>, t + x)` is the only spelling that keeps the association
 * AND the position; `t` is declared per-arm so each is its own short-lived
 * allocno and stays in r0.
 *
 * gMapData is named honestly throughout.  agbcc force-addrs it once,
 * for the first statement, and that .rodata word is the ROM's
 * gUnknown_0808D834 (promotion needs "rodata": ["0x0808D834"]); the later arms
 * use the plain literal-pool address, which is exactly the mix the ROM has. */
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

void sub_0800A098(int x, int y)
{
    int c = MAP->cells[MAP->rowOffset[y] + x];

    if (c == 0x39)
    {
        if (x > 0)
        {
            int nx = x - 1;
            if (sub_0800164C(nx, y) == 0
             && MAP->cells[MAP->rowOffset[y] + x] == 0x39
             && sub_0800AA30(nx, y, 0) == 0)
            {
                int t;
                sub_080011F4(nx, y, 2);
                sub_08001158(nx, y, 0x11d);
                if (MAP->terrain[(t = MAP->rowOffset[y - 1] - 1, t + x)] == 0xd)
                    sub_0800BA9C(nx, y - 1);
            }
        }
        if (x < MAP->width - 1)
        {
            int nx = x + 1;
            if (sub_0800164C(nx, y) == 0
             && MAP->cells[MAP->rowOffset[y] + x] == 0x39
             && sub_0800AA30(nx, y, 1) == 0)
            {
                int t;
                sub_080011F4(nx, y, 2);
                sub_08001158(nx, y, 0xfd);
                if (MAP->terrain[(t = MAP->rowOffset[y - 1] + 1, t + x)] == 0xd)
                    sub_0800BA9C(nx, y - 1);
            }
        }
    }
    if (c == 0x18)
    {
        if (y > 0)
        {
            int ny = y - 1;
            if (sub_0800164C(x, ny) == 0
             && MAP->cells[MAP->rowOffset[y] + x] == 0x18
             && sub_0800AA30(x, ny, 2) == 0)
            {
                int t;
                sub_080011F4(x, ny, 2);
                sub_08001158(x, ny, 0xfc);
                if (MAP->terrain[(t = MAP->rowOffset[ny] - 1, t + x)] == 0xd)
                    sub_0800BA9C(x - 1, ny);
            }
        }
        if (y < MAP->height - 1)
        {
            int ny = y + 1;
            if (sub_0800164C(x, ny) == 0
             && MAP->cells[MAP->rowOffset[y] + x] == 0x18
             && sub_0800AA30(x, ny, 4) == 0)
            {
                int t;
                sub_080011F4(x, ny, 2);
                sub_08001158(x, ny, 0x11c);
                if (MAP->terrain[(t = MAP->rowOffset[ny] - 1, t + x)] == 0xd)
                    sub_0800BA9C(x - 1, ny);
            }
        }
    }
}

void sub_0800A2EC(int x, int y)
{
    if (y > 0)
    {
        int n = y - 1;
        if (sub_08009B38(x, n))
        {
            sub_080011F4(x, n, 1);
            sub_08001158(x, n, 1);
        }
    }

    if (y < MAP->height - 1)
    {
        int n = y + 1;
        if (sub_08009B38(x, n))
        {
            sub_080011F4(x, n, 1);
            sub_08001158(x, n, 1);
        }
    }

    if (x > 0)
    {
        int n = x - 1;
        if (sub_08009B38(n, y))
        {
            sub_080011F4(n, y, 1);
            sub_08001158(n, y, 1);
        }
    }

    if (x < MAP->width - 1)
    {
        int n = x + 1;
        if (sub_08009B38(n, y))
        {
            sub_080011F4(n, y, 1);
            sub_08001158(n, y, 1);
        }
    }
}
