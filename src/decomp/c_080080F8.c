#include "global.h"

/* Promoted from assembly; each function below is byte-for-byte
 * identical to the original. Order is address order and must
 * stay that way -- the linker places this file's .text as one
 * contiguous block at 0x080080F8.
 * sub_080080F8 @ 0x080080F8
 */

/* Repaints one cell through four independent terrain tests, each a
 * sub_0800119C query on the same (x, y). The first re-reads the cell's own
 * tile through the c_08001158.c fetch idiom (rows at +0x417A, tiles at +0xA22)
 * and, when it is one of the two bridge ids, clears it unless the cell below
 * is joinable.
 *
 * sub_0800B61C returns s16 -- the `lsls #0x10 / asrs #0x10` before the sign
 * test is agbcc re-narrowing a narrow-returning callee, and the narrowed value
 * in r2 is then handed straight to sub_08001158 as its third argument.
 *
 * PERMUTER WIN, so this is byte-exact but is NOT claimed to be the original
 * source. Written the plain way -- `t = y * 2;` and `... * 2` with the literal
 * inline, and `sub_08001158(x, y, sub_080016D0(x, y))` nested -- every
 * instruction, branch target, literal pool word and relocation is already
 * correct and exactly THREE bytes differ: the cell's tile lands in r0 where
 * the ROM uses r2 (`ldrh r2,[r1]` / `cmp r2,#67` / `cmp r2,#3`). That was
 * re-derived from scratch here against the c_080081E0.c exemplar and
 * reproduced the same 98.7% independently, so the residual is structural
 * rather than an artefact of how the expression is spelled -- W37-D had
 * already ruled out int v, u16 v, no local at all, a two-case switch, and
 * hoisting every local to function scope.
 *
 * What closes it is `scale`: binding the constant 2 to a local that BOTH
 * multiplies read keeps a register occupied across the tile fetch, so r0 is
 * no longer free at the `ldrh` and the allocator falls to r2. Splitting the
 * nested sub_080016D0 call into `tile` is the second half of the same
 * pressure change. Both are permuter output, kept because they are what the
 * bytes require. */
void sub_080080F8(int x, int y)
{
    s16 height;
    int scale;

    if (sub_0800119C(x, y, 1))
    {
        u8 *p;
        u8 *rows;
        u8 *tiles;
        int t;
        int off;
        int v;

        p = gMapData;
        scale = 2;
        t = y * scale;
        rows = p + 0x417A;
        off = (*(u16 *)(rows + t) + x) * scale;
        tiles = p + 0xA22;
        v = *(u16 *)(tiles + off);

        if (v == 0x43 || v == 3)
        {
            if (!sub_0800119C(x, y + 1, 3))
                sub_08001158(x, y, 1);
        }

        v = sub_080016D0(x, y);
        sub_08001158(x, y, v);
    }

    if (sub_0800119C(x, y, 3))
        sub_0800B048(x, y);

    if (sub_0800119C(x, y, 5))
    {
        sub_08001158(x, y, sub_0800F418(x, y));
        sub_08001158(x, y, sub_080016D0(x, y));
    }

    if (sub_0800119C(x, y, 0xd))
    {
        s16 w;

        height = sub_0800B61C(x, y);
        w = height;

        if (w < 0)
            sub_08007CA0(x, y);
        else
            sub_08001158(x, y, w);
    }
}
