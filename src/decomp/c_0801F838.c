#include "global.h"

/* Promoted from assembly; each function below is byte-for-byte
 * identical to the original. Order is address order and must
 * stay that way -- the linker places this file's .text as one
 * contiguous block at 0x0801F838.
 * sub_0801F838 @ 0x0801F838
 */

/* Fills every cell of the gMapData screen (u16 width at +0, u16
 * height at +2) with one byte, through the row-pointer table
 * gUnknown_03003340. Both bounds are re-read every iteration because the
 * `strb` goes through a pointer the compiler cannot see past, which is why
 * the naive nested `for` is what the ROM has. */
void sub_0801F838(u8 a)
{
    int x;
    int y;

    for (y = 0; y < *(u16 *)(gMapData + 2); y++)
        for (x = 0; x < *(u16 *)gMapData; x++)
            gUnknown_03003340[y][x] = a;
}
