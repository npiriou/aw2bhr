#include "global.h"

/* Promoted from assembly; each function below is byte-for-byte
 * identical to the original. Order is address order and must
 * stay that way -- the linker places this file's .text as one
 * contiguous block at 0x0802C8BC.
 * sub_0802C8BC @ 0x0802C8BC
 */

/* "Is the cell under the cursor occupied": the gMapData map read in
 * its usual three-step form (src/decomp/c_08001158.c), against the +0x12 unit
 * plane and the gUnknown_03003100 cursor. Read UNSIGNED -- both halves come
 * back with a plain `ldrh`, which is the union's `pos` view and not `spos`. */
bool8 sub_0802C8BC(void)
{
    u8 *p;
    u8 *rows;
    u8 *tiles;
    int t;
    int off;

    p = gMapData;
    t = gUnknown_03003100.pos.unk02 * 2;
    rows = p + 0x417A;
    off = *(u16 *)(rows + t) + gUnknown_03003100.pos.unk00;
    tiles = p + 0x12;

    if (tiles[off] == 0)
        return FALSE;

    return TRUE;
}
