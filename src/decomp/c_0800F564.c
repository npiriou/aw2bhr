#include "global.h"

/* Promoted from assembly; each function below is byte-for-byte
 * identical to the original. Order is address order and must
 * stay that way -- the linker places this file's .text as one
 * contiguous block at 0x0800F564.
 * sub_0800F564 @ 0x0800F564
 */

/* A leaf predicate on the tile one step away from (x, y) in direction `dir`:
 * 0 = off the map or not a road tile, 1 = road, 2 = road that continues along
 * `dir`, 3 = road that continues on the perpendicular axis.
 *
 * The row offset is loaded into `off` on its own line before `nx` is computed.
 * That is forced: folding the load into `off = (*(u16 *)(rows + t) + nx) * 2`
 * hoists the whole of `nx` above the `ldrh`, and dropping `rows` reassociates
 * `p + 0x417A + t` into `(t + p) + 0x417A`. Only this split reproduces
 * `lsls; ldr =0x417A; adds; adds; ldrh`.
 *
 * The first `||` chain starts with 0x142, 0x143 because that pair is the only
 * one agbcc folds into a range (`tile - 0x142 <= 1u`): `fold` merges two
 * adjacent equality tests only when they are the two operands of one `||`, and
 * left association means every later term is ORed against a tree rather than
 * against a comparison. Every other constant in each chain is reached by
 * `adds`/`subs` off the previous one, so the emitted order IS the source
 * order.
 *
 * The four `result = 3` tests are the same condition written out four times;
 * cross-jumping merges dir 0 with dir 1 and dir 2 with dir 3, which is why two
 * copies survive in the ROM rather than one or four. */
int sub_0800F564(int x, int y, int dir)
{
    u8 *p;
    u8 *rows;
    u8 *tiles;
    int nx;
    int ny;
    int t;
    int off;
    int tile;
    int result;

    p = gMapData;
    ny = y + gUnknown_08488954[dir];
    t = ny * 2;
    rows = p + 0x417A;
    off = *(u16 *)(rows + t);
    nx = x + gUnknown_0848894C[dir];
    off = (off + nx) * 2;
    tiles = p + 0xA22;
    tile = *(u16 *)(tiles + off);
    result = 0;

    if (nx >= 0 && nx < *(u16 *)p && ny >= 0 && ny < *(u16 *)(p + 2))
    {
        if (tile == 0x142 || tile == 0x143 || tile == 0x140 || tile == 0x141
            || tile == 0x160 || tile == 0x161 || tile == 0x162 || tile == 0x163
            || tile == 0x122 || tile == 0x123 || tile == 0x121 || tile == 0x120
            || tile == 0x103 || tile == 0x102)
        {
            result = 1;

            if (dir == 0)
            {
                if (tile == 0x142 || tile == 0x140 || tile == 0x160 || tile == 0x162
                    || tile == 0x122 || tile == 0x121 || tile == 0x120)
                    result = 2;
                else if (tile == 0x142 || tile == 0x143 || tile == 0x121 || tile == 0x120
                         || tile == 0x103 || tile == 0x102)
                    result = 3;
            }
            else if (dir == 1)
            {
                if (tile == 0x141 || tile == 0x142 || tile == 0x161 || tile == 0x162
                    || tile == 0x122 || tile == 0x121 || tile == 0x120)
                    result = 2;
                else if (tile == 0x142 || tile == 0x143 || tile == 0x121 || tile == 0x120
                         || tile == 0x103 || tile == 0x102)
                    result = 3;
            }
            else if (dir == 2)
            {
                if (tile == 0x143 || tile == 0x140 || tile == 0x141 || tile == 0x163
                    || tile == 0x123 || tile == 0x103 || tile == 0x102)
                    result = 2;
                else if (tile == 0x142 || tile == 0x143 || tile == 0x121 || tile == 0x120
                         || tile == 0x103 || tile == 0x102)
                    result = 3;
            }
            else if (dir == 3)
            {
                if (tile == 0x143 || tile == 0x160 || tile == 0x161 || tile == 0x163
                    || tile == 0x123 || tile == 0x103 || tile == 0x102)
                    result = 2;
                else if (tile == 0x142 || tile == 0x143 || tile == 0x121 || tile == 0x120
                         || tile == 0x103 || tile == 0x102)
                    result = 3;
            }
        }
    }

    return result;
}
