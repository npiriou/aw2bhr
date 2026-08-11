#include "global.h"

/* Promoted from assembly; each function below is byte-for-byte
 * identical to the original. Order is address order and must
 * stay that way -- the linker places this file's .text as one
 * contiguous block at 0x08001158.
 * sub_08001158 @ 0x08001158
 */

/* Promoted from assembly; each function below is byte-for-byte
 * identical to the original. Order is address order and must
 * stay that way -- the linker places this file's .text as one
 * contiguous block at 0x08001158.
 * sub_08001158 @ 0x08001158
 */

void sub_08001158(int x, int y, int val)
{
    u8 *p;
    u8 *rows;
    u8 *tiles;
    int t;
    int off;

    if (val < 0)
        return;

    p = gMapData;
    t = y * 2;
    rows = p + 0x417A;
    off = (*(u16 *)(rows + t) + x) * 2;
    tiles = p + 0xA22;
    *(u16 *)(tiles + off) = val & 0x1FF;
}
