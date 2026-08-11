#include "global.h"

/* Promoted from assembly; each function below is byte-for-byte
 * identical to the original. Order is address order and must
 * stay that way -- the linker places this file's .text as one
 * contiguous block at 0x08021CB4.
 * sub_08021CB4 @ 0x08021CB4
 */

/* Refreshes the cached terrain byte of every entry in the gUnknown_03003150
 * list from the map plane: entry.flags = plane[rowOffset[entry.y] + entry.x].
 *
 * The bound is an `int` INDEX, not a pointer: the ROM's `cmp r3, r4; bgt` is a
 * SIGNED compare, and a pointer comparison is unsigned in agbcc (`bhi`).
 * strength_reduce rewrites `i > 0x5b` into the pointer form but keeps the
 * signedness of the original induction variable, which is what leaves `bgt`
 * behind.  The bound is also tested BEFORE the 0xFF terminator on every
 * iteration but not on entry, which is loop rotation of a `while (flags !=
 * 0xff)` whose body ends in the `if (i > 0x5b) break;`. */
struct Unk21CB4Map
{
    /* 0x0000 */ u8 filler_0000[0x1432];
    /* 0x1432 */ u8 plane[0x417A - 0x1432];
    /* 0x417A */ u16 rowOffset[1];
};

void sub_08021CB4(void)
{
    struct Unk21CB4Map *map;
    int i;

    i = 0;

    while (gUnknown_03003150[i].flags != 0xff)
    {
        map = (struct Unk21CB4Map *)gMapData;
        gUnknown_03003150[i].flags =
            map->plane[map->rowOffset[gUnknown_03003150[i].y] + gUnknown_03003150[i].x];
        i++;
        if (i > 0x5b)
            break;
    }
}
