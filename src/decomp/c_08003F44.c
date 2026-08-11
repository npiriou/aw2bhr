#include "global.h"

/* Promoted from assembly; each function below is byte-for-byte
 * identical to the original. Order is address order and must
 * stay that way -- the linker places this file's .text as one
 * contiguous block at 0x08003F44.
 * sub_08003F44 @ 0x08003F44
 */

struct Unk3F44Map
{
    /* 0x0000 */ u8 filler_0000[0x0A22];
    /* 0x0A22 */ u16 plane[(0x1432 - 0x0A22) / 2];
    /* 0x1432 */ u8 cell[0x417A - 0x1432];
    /* 0x417A */ u16 rowOffset[1];
};
#define MAP ((struct Unk3F44Map *)gMapData)

void sub_08003F44(int x, int y, int v)
{
    if (x > 0x1D)
        x = 0x1D;
    if (y > 0x13)
        y = 0x13;

    MAP->plane[MAP->rowOffset[y] + x] = v;

    switch (v)
    {
    case 1:
        MAP->cell[MAP->rowOffset[y] + x] = v;
        break;
    case 0x20:
        MAP->cell[MAP->rowOffset[y] + x] = 3;
        break;
    case 0x87:
        if (MAP->cell[MAP->rowOffset[y] + x] != 4)
        {
            MAP->cell[MAP->rowOffset[y] + x] = 4;
            MAP->plane[MAP->rowOffset[y] + x] = v;
            sub_0800CF28(x, y);
        }
        break;
    case 0x2A:
        sub_08007CA0(x, y);
        break;
    }
}
