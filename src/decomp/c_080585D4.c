#include "global.h"

/* Promoted from assembly; each function below is byte-for-byte
 * identical to the original. Order is address order and must
 * stay that way -- the linker places this file's .text as one
 * contiguous block at 0x080585D4.
 * sub_080585D4 @ 0x080585D4
 */

/* Weighted count over the whole map: every passable cell (gUnknown_03003340
 * non-negative as an s8) whose terrain byte is accepted by gUnknown_085767D5
 * and which sub_08026FD0 does not reject scores 0x1e if its terrain code is
 * exactly 8 and 1 otherwise.
 *
 * gUnknown_0816D93C IS gMapData. The ROM word at 0x0816D93C holds
 * 0x08499590 (read out of baserom.gba), so the `ldr rN,=gUnknown_0816D93C;
 * ldr rM,[rN]; ldr rK,[rM]` chain on the HEIGHT reads and the plain
 * `ldr r0,=gMapData; ldr r0,[r0]` on the WIDTH read are the same
 * object -- -fforce-addr gave this function a private address-constant word for
 * one of the two references and gen_lds.py invented a symbol for it. The honest
 * spelling names gMapData throughout and trymatch reports
 * `different symbols that resolve to the same address`; the promotion carries
 * "rodata": ["0x0816D93C"]. Do NOT declare gUnknown_0816D93C.
 *
 * Three spellings, each worth measurable bytes, in the order they were found:
 *
 *  - `p += 0x1432;` and not `cells = p + 0x1432;`. The ROM's in-place
 *    `adds r1,r1,r6` is the tell -- a separate `cells` local keeps p live and
 *    emits the three-operand form. Worth 7 bytes.
 *  - `rows = p + 0x417a;` must stay its OWN local. Written inline as
 *    `*(u16 *)(p + 0x417a + t)` agbcc reassociates the constant to LAST
 *    (`adds r0,r4,r1; adds r0,r0,r3`) where the ROM adds the pool constant
 *    first. Worth 9 bytes across the two blocks.
 *  - The second block ends `p += off; if ((*p & 0x1f) == 8)`, not `p[off]`.
 *    That was the last 2 bytes: with `p[off]` the address temp coalesces with
 *    off's register (`adds r0,r1,r0`), with `p += off` it must land in p's
 *    (`adds r1,r1,r0`). Block ONE keeps `p[off]` -- there the address is used
 *    twice, is CSE'd into a third register, and coalesces with neither.
 *
 * The whole cell-address block is written out TWICE (once before the call, once
 * after); the ROM reloads gMapData through the address constant it
 * parked in r8, so the source really does repeat it rather than binding the
 * cell pointer across the call. `sub sp,#8` holds y+1 and the 0x417A pool
 * constant across the bl -- both are the loop optimiser's and the reload's
 * work, not source temporaries.
 *
 * MATCHED. */
int sub_080585D4(void)
{
    int acc;
    int x;
    int y;
    u8 *p;
    u8 *rows;
    int t;
    int off;

    acc = 0;

    for (y = 0; y < *(u16 *)(gMapData + 2); y++)
    {
        for (x = 0; x < *(u16 *)gMapData; x++)
        {
            if ((s8)gUnknown_03003340[y][x] >= 0)
            {
                p = gMapData;
                t = y * 2;
                rows = p + 0x417a;
                off = *(u16 *)(rows + t) + x;
                p += 0x1432;
                if (gUnknown_085767D5[p[off] & 0x1f] != 0
                    && sub_08026FD0(gUnknown_03003F38, p[off]) == 0)
                {
                    p = gMapData;
                    rows = p + 0x417a;
                    off = *(u16 *)(rows + t) + x;
                    p += 0x1432;
                    p += off;
                    if ((*p & 0x1f) == 8)
                        acc += 0x1e;
                    else
                        acc += 1;
                }
            }
        }
    }

    return acc;
}
