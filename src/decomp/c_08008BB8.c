#include "global.h"

/* Promoted from assembly; each function below is byte-for-byte
 * identical to the original. Order is address order and must
 * stay that way -- the linker places this file's .text as one
 * contiguous block at 0x08008BB8.
 * sub_08008BB8 @ 0x08008BB8
 */

void sub_08008BB8(int x, int y)
{
    int v;

    v = sub_08008B70(x, y);

    if (v > 0)
    {
        s8 *costs;
        u8 *p;
        u8 *rows;
        u8 *cells;
        int t;
        int idx;
        int c;

        /* WAVE 37 final sweep: `.unk18` was `s8 *` when this function was
         * verified; W37-H later widened it to `s8 *[3]`. Element 0 is at the
         * same offset, so `[0]` is byte-identical to the spelling that matched. */
        costs = gCoDataTable[1].unk38[0].unk18[0];

        p = gMapData;
        t = y * 2;
        rows = p + 0x417A;
        idx = *(u16 *)(rows + t) + x;
        cells = p + 0x1432;
        c = (*(cells + idx) & 0x1f) + gUnitTypeData[v & 0x3f].unk19 * 32;

        if (costs[c] == -1)
            sub_08008A8C(0, x, y);
    }
}
