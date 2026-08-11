#include "global.h"

/* Promoted from assembly; each function below is byte-for-byte
 * identical to the original. Order is address order and must
 * stay that way -- the linker places this file's .text as one
 * contiguous block at 0x08025598.
 * sub_08025598 @ 0x08025598
 */

u8 sub_08025598(s16 a1, s16 a2)
{
    u8 *p;
    u8 *rows;
    u8 *units;
    int t;
    int off;
    int id;

    p = gMapData;
    t = a2 * 2;
    rows = p + 0x417A;
    off = *(u16 *)(rows + t) + a1;
    units = p + 0x51A;
    id = units[off];

    if ((u16)id == 0)
        return 0;

    if (gArmyRecords[((u16)id >> 6) + 1].unk1c & 2)
        return 1;

    return 0;
}
