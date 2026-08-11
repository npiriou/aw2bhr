#include "global.h"

/* Promoted from assembly; each function below is byte-for-byte
 * identical to the original. Order is address order and must
 * stay that way -- the linker places this file's .text as one
 * contiguous block at 0x080094EC.
 * sub_080094EC @ 0x080094EC
 */

/* Same screen-struct address computation as sub_08001158, reading the tile
 * instead of writing it. The two 0xFC/0xFD tests merge into the unsigned
 * range compare `t - 0xfc <= 1`; 0x11C/0x11D stay separate because they are
 * materialised by `movs #0x8e; lsls #1` and then `adds #1`.
 *
 * Return type: `int`. Its 12 callers all consume the result as a plain truth
 * test (sub_0800B528 does `bl; cmp r0, #0; beq`) and none forwards it, so
 * nothing narrows it and there is no evidence for a u8/bool8 return. */
int sub_080094EC(int x, int y)
{
    u8 *p;
    u8 *rows;
    u8 *tiles;
    int t;
    int off;
    int tile;

    p = gMapData;
    t = y * 2;
    rows = p + 0x417A;
    off = (*(u16 *)(rows + t) + x) * 2;
    tiles = p + 0xA22;
    tile = *(u16 *)(tiles + off);

    return tile == 0xFC || tile == 0xFD || tile == 0x11C || tile == 0x11D;
}
