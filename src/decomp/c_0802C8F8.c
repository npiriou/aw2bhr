#include "global.h"

/* Promoted from assembly; each function below is byte-for-byte
 * identical to the original. Order is address order and must
 * stay that way -- the linker places this file's .text as one
 * contiguous block at 0x0802C8F8.
 * sub_0802C8F8 @ 0x0802C8F8, sub_0802C958 @ 0x0802C958
 */

bool8 sub_0802C8F8(void)
{
    u8 *p;
    u8 *rows;
    u8 *tiles;
    struct UnitRecord *e;
    int t;
    int off;

    p = gMapData;
    t = gUnknown_03003100.pos.unk02 * 2;
    rows = p + 0x417A;
    off = *(u16 *)(rows + t) + gUnknown_03003100.pos.unk00;
    tiles = p + 0x12;

    if (tiles[off] == 0)
        return TRUE;

    e = &gUnitRecords[tiles[off]];

    if (!sub_08025FC0((struct UnitRecord *)gUnknown_030040D8, e))
        return TRUE;

    return FALSE;
}

bool8 sub_0802C958(void)
{
    if (!sub_0802C8F8())
        return TRUE;

    if (!sub_0802CBA0())
        return TRUE;

    sub_0801F838(0xff);
    gUnknown_03003340[gUnknown_03003100.pos.unk02][gUnknown_03003100.pos.unk00] = 0;

    if (sub_0804151C())
        return FALSE;

    return TRUE;
}
