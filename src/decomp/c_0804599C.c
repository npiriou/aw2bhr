#include "global.h"

/* Promoted from assembly; each function below is byte-for-byte
 * identical to the original. Order is address order and must
 * stay that way -- the linker places this file's .text as one
 * contiguous block at 0x0804599C.
 * sub_0804599C @ 0x0804599C
 */

/* The gMapData map read in its usual three-step form (see
 * src/decomp/c_08001158.c): row-offset table at +0x417A indexed by y*2, plus
 * x, into a plane base. The plane here is +0x1432, the 5-bit terrain code
 * gUnknown_08551CA0 is keyed by, and the cursor is
 * gUnknown_030040D8->unk02 / ->unk03. */
int sub_0804599C(void)
{
    u8 *p;
    u8 *rows;
    u8 *tiles;
    int t;
    int off;

    p = gMapData;
    t = gUnknown_030040D8->unk03 * 2;
    rows = p + 0x417A;
    off = *(u16 *)(rows + t) + gUnknown_030040D8->unk02;
    tiles = p + 0x1432;

    if ((tiles[off] & 0x1F) == 8)
        return 1;

    return 0;
}
