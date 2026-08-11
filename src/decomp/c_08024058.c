#include "global.h"

/* Promoted from assembly; each function below is byte-for-byte
 * identical to the original. Order is address order and must
 * stay that way -- the linker places this file's .text as one
 * contiguous block at 0x08024058.
 * sub_08024058 @ 0x08024058
 */

struct Unk24058Map
{
    /* 0x0000 */ u8 filler_0000[0x1432];
    /* 0x1432 */ u8 plane[0x417A - 0x1432];
    /* 0x417A */ u16 rowOffset[1];
};

void sub_08024058(s16 a1, s16 a2)
{
    struct Unk24058Map *map;
    int idx;
    u8 v;

    map = (struct Unk24058Map *)gMapData;
    idx = map->rowOffset[a2] + a1;
    v = (map->plane[idx] & 0x1f) + gUnknown_03004084;
    map->plane[idx] = v;

    sub_080240B4(a1, a2, v & 0xe0);
    sub_08024268();
    sub_080219AC();
    sub_08026D68();
}
