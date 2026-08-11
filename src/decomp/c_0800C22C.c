#include "global.h"

/* Promoted from assembly; each function below is byte-for-byte
 * identical to the original. Order is address order and must
 * stay that way -- the linker places this file's .text as one
 * contiguous block at 0x0800C22C.
 * sub_0800C22C @ 0x0800C22C
 */

/* The same 3x3 neighbourhood walk as sub_0800BEE4 -- see c_0800BEE4.c -- but
 * over sub_0800C2D0, which takes a third argument.  The flag is 0 for the four
 * diagonals and 1 for the four orthogonal neighbours plus the two vertical
 * centres; the centre cell itself is never visited.
 *
 * The struct is c_0800BEE4.c's, copied verbatim: only width at +0 and height
 * at +2 are read here.
 *
 * gMapData is read FOUR times, which is what puts its address in this
 * unit's own `.rodata` under -fforce-addr -- hence the three-level chain
 * `ldr rN, =.LC; ldr rN, [rN]; ldr r0, [rN]; ldrh` where the neighbours with
 * one read get a plain inline pool word.  The `ldr r7, [r7]` that replaces the
 * pool address with the loaded pointer partway down is CSE's doing, not the
 * source's: nothing in the C changes at that point. */

struct MapScreen
{
    /* 0x0000 */ u16 width;
    /* 0x0002 */ u16 height;
    /* 0x0004 */ u8 filler_0004[0x0A22 - 4];
    /* 0x0A22 */ u16 cells[(0x1432 - 0x0A22) / 2];
    /* 0x1432 */ u8 terrain[0x417A - 0x1432];
    /* 0x417A */ u16 rowOffset[1];
};
#define MAP ((struct MapScreen *)gMapData)

void sub_0800C22C(int x, int y)
{
    if (y > 0)
    {
        int n = y - 1;
        if (x > 0)
            sub_0800C2D0(x - 1, n, 0);
        sub_0800C2D0(x, n, 1);
        if (x < MAP->width - 1)
            sub_0800C2D0(x + 1, n, 0);
    }

    if (x > 0)
        sub_0800C2D0(x - 1, y, 1);

    if (x < MAP->width - 1)
        sub_0800C2D0(x + 1, y, 1);

    if (y < MAP->height - 1)
    {
        int n = y + 1;
        if (x > 0)
            sub_0800C2D0(x - 1, n, 0);
        sub_0800C2D0(x, n, 1);
        if (x < MAP->width - 1)
            sub_0800C2D0(x + 1, n, 0);
    }
}
