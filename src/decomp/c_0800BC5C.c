#include "global.h"

/* Promoted from assembly; each function below is byte-for-byte
 * identical to the original. Order is address order and must
 * stay that way -- the linker places this file's .text as one
 * contiguous block at 0x0800BC5C.
 * sub_0800BC5C @ 0x0800BC5C, sub_0800BC98 @ 0x0800BC98
 */

/* Reports whether the terrain at (x, y) is one of the two bridge codes. Same
 * struct-through-a-cast reading of gMapData as sub_0800B1FC -- see
 * that function for why the flat pointer spelling does not match. */
struct MapScreen
{
    /* 0x0000 */ u8 filler_0000[0x0A22];
    /* 0x0A22 */ u16 cells[(0x1432 - 0x0A22) / 2];
    /* 0x1432 */ u8 terrain[0x417A - 0x1432];
    /* 0x417A */ u16 rowOffset[1];
};

int sub_0800BC5C(int x, int y)
{
    struct MapScreen *s = (struct MapScreen *)gMapData;
    u8 t = s->terrain[s->rowOffset[y] + x];

    if (t == 0x13 || t == 7)
        return 1;
    else
        return 0;
}

int sub_0800BC98(int x, int y)
{
    struct MapScreen *s = (struct MapScreen *)gMapData;

    if (s->terrain[s->rowOffset[y] + x] != 7)
        return 0;
    else
        return 1;
}
