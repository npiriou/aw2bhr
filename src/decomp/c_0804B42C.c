#include "global.h"

/* Promoted from assembly; each function below is byte-for-byte
 * identical to the original. Order is address order and must
 * stay that way -- the linker places this file's .text as one
 * contiguous block at 0x0804B42C.
 * sub_0804B42C @ 0x0804B42C, sub_0804B4C4 @ 0x0804B4C4
 */

/* Scores the four cardinal neighbours of (x, y) on the gMapData map
 * and returns the terrain code of the best-scoring in-bounds one. Byte-
 * identical twin of sub_0804B4C4, which reads the adjacent table
 * gUnknown_08551CBD instead of gUnknown_08551CA0.
 *
 * Two things in here are not free choices:
 *   `best` is `u8`, NOT `int` or `u32`. All three produce the same unsigned
 * `cmp; bcs` -- the two operands are zero-extended QImode values, so gcc picks
 * the unsigned compare whatever the wider type is -- but only the QImode local
 * makes `best = tbl[t]` a second `ldrb` off the surviving address register.
 * Anything wider is a `mov` off the value CSE already has, which is the same
 * size and the wrong two bytes. Suspect the TYPE before the allocation.
 *   `d` walks gUnknown_08551CDC and is materialised BEFORE `rows`/`terrain`.
 * The .rodata address-constant order in the ROM (08499590, 08551CDC, 0x417a,
 * 0x1432, 08551CA0) is the readout: a `gUnknown_08551CDC[i]` subscript inside
 * the loop is strength-reduced into the same pointer but creates its address
 * LAST, after the two loop-invariant sums.
 */
int sub_0804B42C(int x, int y)
{
    u8 *p;
    const s32 *d;
    u8 *rows;
    u8 *terrain;
    u8 best;
    int result;
    int i;
    int nx;
    int ny;
    int t;

    best = 0;
    result = 0;
    p = gMapData;
    d = gUnknown_08551CDC[0];
    rows = p + 0x417A;
    terrain = p + 0x1432;

    for (i = 0; i < 4; i++)
    {
        nx = x + d[0];
        ny = y + d[1];

        if (nx <= *(u16 *)p && nx >= 0 && ny <= *(u16 *)(p + 2) && ny >= 0)
        {
            t = *(terrain + (*(u16 *)(rows + ny * 2) + nx)) & 0x1f;

            if (best < gUnknown_08551CA0[t])
            {
                result = t;
                best = gUnknown_08551CA0[t];
            }
        }

        d += 2;
    }

    return result;
}

/* Byte-identical twin of sub_0804B42C -- SAME SOURCE apart from the scoring
 * table, gUnknown_08551CBD instead of gUnknown_08551CA0. That symbol's ODD
 * address is real: the two tables are adjacent 29- and 31-byte runs that tile
 * 0x08551CA0..0x08551CDC exactly, and they differ in ROM only at element 5.
 * See sub_0804B42C for the `u8 best` and pointer-materialisation notes.
 */
int sub_0804B4C4(int x, int y)
{
    u8 *p;
    const s32 *d;
    u8 *rows;
    u8 *terrain;
    u8 best;
    int result;
    int i;
    int nx;
    int ny;
    int t;

    best = 0;
    result = 0;
    p = gMapData;
    d = gUnknown_08551CDC[0];
    rows = p + 0x417A;
    terrain = p + 0x1432;

    for (i = 0; i < 4; i++)
    {
        nx = x + d[0];
        ny = y + d[1];

        if (nx <= *(u16 *)p && nx >= 0 && ny <= *(u16 *)(p + 2) && ny >= 0)
        {
            t = *(terrain + (*(u16 *)(rows + ny * 2) + nx)) & 0x1f;

            if (best < gUnknown_08551CBD[t])
            {
                result = t;
                best = gUnknown_08551CBD[t];
            }
        }

        d += 2;
    }

    return result;
}
