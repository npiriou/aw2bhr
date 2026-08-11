#include "global.h"

/* Promoted from assembly; each function below is byte-for-byte
 * identical to the original. Order is address order and must
 * stay that way -- the linker places this file's .text as one
 * contiguous block at 0x0800A588.
 * sub_0800A588 @ 0x0800A588, sub_0800A6AC @ 0x0800A6AC, sub_0800A798 @ 0x0800A798, sub_0800A884 @ 0x0800A884, sub_0800A95C @ 0x0800A95C
 */

/* MATCHED (wave 37, W37-C).
 *
 * The same four-cardinal-neighbour sweep as sub_0800A3D4, over a different
 * predicate: for each in-bounds neighbour sub_08009B38 accepts, classify it
 * with sub_0800A95C and either repair it (negative) or redraw it (positive).
 *
 * `v` lives in r2 because it is sub_08001158's third argument -- the
 * `adds r2, r0, #0` after the `bl` is the copy into that parameter, not a
 * narrowing, and the two `cmp r2, #0` tests (`bge` then `ble`) are what make
 * sub_0800A95C's return signed and NOT s16 (an s16 return would add a
 * `lsls #16; asrs #16` here that the ROM does not have).
 *
 * The first block reaches `bl sub_08009B38` with r0 never written -- x is
 * still in it from the prologue -- exactly as in sub_0800A3D4.
 *
 * gMapData is named honestly; agbcc's own -fforce-addr copy is the
 * ROM's gUnknown_0808D840 pool word (promotion needs "rodata":
 * ["0x0808D840"]).  That is why no `u8 **const *pp` local is needed here,
 * unlike the older c_0800A3D4.c / c_0800AA30.c spelling, and why
 * `MAP->height - 1` needs no `lim` binding to place the pool register: with the
 * honest spelling agbcc emits the `adds r7, r1, #0` copy after the bound by
 * itself. */
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

void sub_0800A588(int x, int y)
{
    if (y > 0)
    {
        int n = y - 1;
        if (sub_08009B38(x, n))
        {
            int v = sub_0800A95C(x, n);
            if (v < 0)
                sub_08007F68(x, n, 1);
            else if (v > 0)
            {
                sub_08001158(x, n, v);
                sub_0800A098(x, n);
            }
        }
    }

    if (y < MAP->height - 1)
    {
        int n = y + 1;
        if (sub_08009B38(x, n))
        {
            int v = sub_0800A95C(x, n);
            if (v < 0)
                sub_08007F68(x, n, 1);
            else if (v > 0)
            {
                sub_08001158(x, n, v);
                sub_0800A098(x, n);
            }
        }
    }

    if (x > 0)
    {
        int n = x - 1;
        if (sub_08009B38(n, y))
        {
            int v = sub_0800A95C(n, y);
            if (v < 0)
                sub_08007F68(n, y, 1);
            else if (v > 0)
            {
                sub_08001158(n, y, v);
                sub_0800A098(n, y);
            }
        }
    }

    if (x < MAP->width - 1)
    {
        int n = x + 1;
        if (sub_08009B38(n, y))
        {
            int v = sub_0800A95C(n, y);
            if (v < 0)
                sub_08007F68(n, y, 1);
            else if (v > 0)
            {
                sub_08001158(n, y, v);
                sub_0800A098(n, y);
            }
        }
    }
}

int sub_0800A6AC(int x, int y)
{
    int m;

    if (sub_080094EC(x, y) || sub_080015E4(x, y) == 0)
        return -1;

    m = 0;

    if (y > 0)
    {
        int n = y - 1;
        if (x > 0)
            m = sub_080015E4(x - 1, n) << 8;
        m |= sub_080015E4(x, n) << 7;
        if (x < MAP->width - 1)
            m |= sub_080015E4(x + 1, n) << 6;
    }
    if (x > 0)
        m |= sub_080015E4(x - 1, y) << 5;
    m |= 0x10;
    if (x < MAP->width - 1)
        m |= sub_080015E4(x + 1, y) << 3;
    if (y < MAP->height - 1)
    {
        int n = y + 1;
        if (x > 0)
            m |= sub_080015E4(x - 1, n) << 2;
        m |= sub_080015E4(x, n) << 1;
        if (x < MAP->width - 1)
            m |= sub_080015E4(x + 1, n);
    }

    return gUnknown_08486BC4[m];
}

int sub_0800A798(int x, int y)
{
    int m;

    if (sub_0800164C(x, y) == 0)
        return -1;

    m = 0;

    if (y > 0)
    {
        int n = y - 1;
        if (x > 0)
            m = sub_080015E4(x - 1, n) << 8;
        m |= sub_080015E4(x, n) << 7;
        if (x < MAP->width - 1)
            m |= sub_080015E4(x + 1, n) << 6;
    }
    if (x > 0)
        m |= sub_080015E4(x - 1, y) << 5;
    if (sub_080094EC(x, y) == 0)
        m |= 0x10;
    if (x < MAP->width - 1)
        m |= sub_080015E4(x + 1, y) << 3;
    if (y < MAP->height - 1)
    {
        int n = y + 1;
        if (x > 0)
            m |= sub_080015E4(x - 1, n) << 2;
        m |= sub_080015E4(x, n) << 1;
        if (x < MAP->width - 1)
            m |= sub_080015E4(x + 1, n);
    }

    return gUnknown_08486BC4[m];
}

int sub_0800A884(int x, int y)
{
    int m = 0;

    if (y > 0)
    {
        int n = y - 1;
        if (x > 0)
            m = sub_080015E4(x - 1, n) << 8;
        m |= sub_080015E4(x, n) << 7;
        if (x < MAP->width - 1)
            m |= sub_080015E4(x + 1, n) << 6;
    }
    if (x > 0)
        m |= sub_080015E4(x - 1, y) << 5;
    m |= 0x10;
    if (x < MAP->width - 1)
        m |= sub_080015E4(x + 1, y) << 3;
    if (y < MAP->height - 1)
    {
        int n = y + 1;
        if (x > 0)
            m |= sub_080015E4(x - 1, n) << 2;
        m |= sub_080015E4(x, n) << 1;
        if (x < MAP->width - 1)
            m |= sub_080015E4(x + 1, n);
    }

    return gUnknown_084867C4[m];
}

int sub_0800A95C(int x, int y)
{
    int m = 0;

    if (y > 0)
    {
        int ny = y - 1;
        if (x > 0)
            m = sub_08009918(x - 1, ny) << 8;
        m |= sub_08009918(x, ny) << 7;
        if (x < MAP->width - 1)
            m |= sub_08009918(x + 1, ny) << 6;
    }
    if (x > 0)
        m |= sub_08009918(x - 1, y) << 5;
    if (x < MAP->width - 1)
        m |= sub_08009918(x + 1, y) << 3;
    if (y < MAP->height - 1)
    {
        int ny = y + 1;
        if (x > 0)
            m |= sub_08009918(x - 1, ny) << 2;
        m |= sub_08009918(x, ny) << 1;
        if (x < MAP->width - 1)
            m |= sub_08009918(x + 1, ny);
    }

    return gUnknown_08486BC4[m];
}
