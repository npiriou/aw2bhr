#include "global.h"

/* Promoted from assembly; each function below is byte-for-byte
 * identical to the original. Order is address order and must
 * stay that way -- the linker places this file's .text as one
 * contiguous block at 0x08001704.
 * sub_08001704 @ 0x08001704, sub_08001A04 @ 0x08001A04
 */

/* The tile-edit query behind sub_080016D0: given a cell (x, y) and the object
 * id v standing on it, translate v between the two rows of gUnknown_0848591C.
 *
 * Which direction depends on the terrain of the cell to the LEFT -- hence the
 * `x > 0` guard and the `- 1` on the row offset. For 27 terrain codes the
 * lookup runs row0 -> row1, and row1's 0x21 entry additionally consults
 * sub_0800119C on the cell BELOW (x, y + 1) before answering 3. Every other
 * terrain, and every cell on the left edge, runs row1 -> row0.
 *
 * The two rows are walked as POINTERS, not subscripted: the ROM advances both
 * r3 and r4 by 2 every iteration while keeping a separate counter against
 * 0x30, which subscripting cannot produce -- `pa[i]` leaves the base invariant
 * and strength-reduces the exit test onto the pointer instead. Binding both
 * before the `if` is what shares one pool word across all three loops.
 *
 * `off` is split over two statements deliberately: written as one expression,
 * fold reassociates `row - 1 + x` into `(x - 1) + row` and emits the `subs` on
 * the parameter instead of on the loaded row offset. */
int sub_08001704(int x, int y, int v)
{
    const s16 *pa;
    const s16 *pb;
    u8 *p;
    u8 *rows;
    u8 *tiles;
    int t;
    int off;
    int i;

    pa = gUnknown_0848591C[0];
    pb = gUnknown_0848591C[1];

    if (x <= 0)
    {
        for (i = 0; i <= 0x30; i++)
        {
            if (v == *pb)
                return *pa;
            pa++;
            pb++;
        }
    }
    else
    {
        p = gMapData;
        t = y * 2;
        rows = p + 0x417A;
        off = *(u16 *)(rows + t) - 1;
        off += x;
        tiles = p + 0x1432;

        switch (tiles[off])
        {
        case 3:
        case 4:
        case 6:
        case 8:
        case 10:
        case 11:
        case 14:
        case 38:
        case 40:
        case 42:
        case 43:
        case 46:
        case 70:
        case 72:
        case 74:
        case 75:
        case 78:
        case 102:
        case 104:
        case 106:
        case 107:
        case 110:
        case 134:
        case 136:
        case 138:
        case 139:
        case 142:
            for (i = 0; i <= 0x30; i++)
            {
                if (v == *pa)
                {
                    if (*pb == 0x21 && sub_0800119C(x, y + 1, 3))
                        return 3;
                    return *pb;
                }
                pa++;
                pb++;
            }
            break;
        default:
            for (i = 0; i <= 0x30; i++)
            {
                if (v == *pb)
                    return *pa;
                pa++;
                pb++;
            }
            break;
        }
    }

    return -1;
}

/* sub_08001704's twin, and the relationship is NOT a parameter swap: this is
 * the SAME function with the `*pb == 0x21` / sub_0800119C special case deleted
 * from the row0 -> row1 branch. Everything else -- the `x > 0` guard, the
 * cell-to-the-left terrain switch with the same 27 cases, both directions of
 * the gUnknown_0848591C walk, the -1 miss return -- is identical source.
 *
 * The register differences follow from that one deletion: with no call, `y` is
 * dead after the row offset is built, so it loses its callee-saved register
 * (r6 here, r7 there) and no `mov ip` scratch is needed; and the two row
 * pointers land the other way round (r4/r3 rather than r3/r4). */
int sub_08001A04(int x, int y, int v)
{
    const s16 *pa;
    const s16 *pb;
    u8 *p;
    u8 *rows;
    u8 *tiles;
    int t;
    int off;
    int i;

    pa = gUnknown_0848591C[0];
    pb = gUnknown_0848591C[1];

    if (x <= 0)
    {
        for (i = 0; i <= 0x30; i++)
        {
            if (v == *pb)
                return *pa;
            pa++;
            pb++;
        }
    }
    else
    {
        p = gMapData;
        t = y * 2;
        rows = p + 0x417A;
        off = *(u16 *)(rows + t) - 1;
        off += x;
        tiles = p + 0x1432;

        switch (tiles[off])
        {
        case 3:
        case 4:
        case 6:
        case 8:
        case 10:
        case 11:
        case 14:
        case 38:
        case 40:
        case 42:
        case 43:
        case 46:
        case 70:
        case 72:
        case 74:
        case 75:
        case 78:
        case 102:
        case 104:
        case 106:
        case 107:
        case 110:
        case 134:
        case 136:
        case 138:
        case 139:
        case 142:
            for (i = 0; i <= 0x30; i++)
            {
                if (v == *pa)
                    return *pb;
                pa++;
                pb++;
            }
            break;
        default:
            for (i = 0; i <= 0x30; i++)
            {
                if (v == *pb)
                    return *pa;
                pa++;
                pb++;
            }
            break;
        }
    }

    return -1;
}
