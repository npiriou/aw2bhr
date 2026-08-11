#include "global.h"

/* Promoted from assembly; each function below is byte-for-byte
 * identical to the original. Order is address order and must
 * stay that way -- the linker places this file's .text as one
 * contiguous block at 0x080011F4.
 * sub_080011F4 @ 0x080011F4
 */

/* The write half of the c_08001158.c map idiom: gMapData is a base
 * pointer, +0x417A is a per-ROW halfword offset table indexed y * 2, and the
 * row offset plus x indexes a BYTE plane at +0x1432 (c_08001158.c writes the
 * halfword plane at +0xA22 with the same arithmetic scaled by two).
 *
 * The locals are punctuation -- one per address step -- and they are what
 * keeps the two pool constants in the ROM's order. Value 7 is the terrain
 * kind sub_0800164C and sub_0800168C also single out. */
void sub_080011F4(int x, int y, int val)
{
    u8 *p;
    u8 *rows;
    u8 *tiles;
    int t;
    int off;

    p = gMapData;
    t = y * 2;
    rows = p + 0x417A;
    off = *(u16 *)(rows + t) + x;
    tiles = p + 0x1432;
    tiles[off] = val;

    if (val == 7)
        sub_08008BB8(x, y);
}
