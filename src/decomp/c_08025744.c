#include "global.h"

/* Promoted from assembly; each function below is byte-for-byte
 * identical to the original. Order is address order and must
 * stay that way -- the linker places this file's .text as one
 * contiguous block at 0x08025744.
 * sub_08025744 @ 0x08025744
 */

u8 sub_08025744(int a1, int a2)
{
    u8 *p;
    u8 *rows;
    u8 *units;
    u8 *flags;
    u8 *plane;
    int t;
    int off;
    int id;

    p = gMapData;
    t = (s16)a2 * 2;
    rows = p + 0x417A;
    off = *(u16 *)(rows + t) + (s16)a1;
    units = p + 0x51A;
    id = units[off];

    if (id == 0)
        return 0;

    flags = p + 0x1E42;

    if (flags[off] == 0)
        return 0;

    plane = p + 0x12;

    if (plane[off] == 0 && (gUnitRecords[id].unk01 & 4) == 0)
        return 0;

    return sub_08026F5C(id);
}
