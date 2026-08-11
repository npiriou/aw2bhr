#include "global.h"

/* Promoted from assembly; each function below is byte-for-byte
 * identical to the original. Order is address order and must
 * stay that way -- the linker places this file's .text as one
 * contiguous block at 0x08003ED0.
 * sub_08003ED0 @ 0x08003ED0
 */

/* MATCHED, wave 40 (W40-A). Redraws every cell of the map: for each row, for
 * each column, hand sub_08003DC4 the cell's terrain byte off the +0x1432 array,
 * indexed the way every other reader in this block indexes it -- through the
 * +0x417A row-offset table. */

struct Unk3F44Map
{
    /* 0x0000 */ u16 width;
    /* 0x0002 */ u16 height;
    /* 0x0004 */ u8 filler_0004[0x0A22 - 0x0004];
    /* 0x0A22 */ u16 plane[(0x1432 - 0x0A22) / 2];
    /* 0x1432 */ u8 cell[0x417A - 0x1432];
    /* 0x417A */ u16 rowOffset[1];
};
#define MAP ((struct Unk3F44Map *)gMapData)

void sub_08003ED0(void)
{
    int x, y;

    for (y = 0; y < MAP->height; y++)
        for (x = 0; x < MAP->width; x++)
            sub_08003DC4(x, y, MAP->cell[MAP->rowOffset[y] + x]);
}
