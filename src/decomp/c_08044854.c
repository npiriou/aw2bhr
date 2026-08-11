#include "global.h"

/* Promoted from assembly; each function below is byte-for-byte
 * identical to the original. Order is address order and must
 * stay that way -- the linker places this file's .text as one
 * contiguous block at 0x08044854.
 * sub_08044854 @ 0x08044854
 */

/* MATCHED. Byte-for-byte the same function as sub_08026100. Read that one --
 * the comma chain in the index expression is three separate position readouts
 * and none of them is guessable. */
void sub_08044854(int x, int y, int c)
{
    u8 *p;
    u8 *rows;
    u8 *cells;
    int t;
    int idx;
    struct UnitRecord *u;
    struct UnitRecord **pp;

    if (x < 0)
        return;
    if (y < 0)
        return;

    p = gMapData;

    if (x >= *(u16 *)p)
        return;
    if (y >= *(u16 *)(p + 2))
        return;

    u = &gUnitRecords[(pp = &gUnitRecords, t = y * 2,
        idx = *(u16 *)((rows = p + 0x417A) + t) + x,
        (cells = p + 0x51A)[idx])];

    if (u->unk00 == 0)
        return;
    if ((u->unk01 & 8) != 0)
        return;

    if (u->unk04_0 < c + 1)
        u->unk04_0 = 1;
    else
        u->unk04_0 = u->unk04_0 - c;
}
