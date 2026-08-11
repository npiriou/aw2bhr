#include "global.h"

/* Promoted from assembly; each function below is byte-for-byte
 * identical to the original. Order is address order and must
 * stay that way -- the linker places this file's .text as one
 * contiguous block at 0x0800C2D0.
 * sub_0800C2D0 @ 0x0800C2D0, sub_0800C454 @ 0x0800C454
 */

/* sub_0800C22C's callee -- see c_0800C22C.c, whose 3x3 walk supplies the third
 * argument this one takes.  `f` is that flag: nonzero for the orthogonal
 * neighbours, zero for the diagonals, and all it gates is the four-way
 * sub_08009F10 sweep in the middle.
 *
 * The struct is c_0800C22C.c's / c_0800AF24.c's, copied verbatim.
 *
 * gMapData is read FIVE times, so -fforce-addr parks its address in
 * this unit's own .rodata and the ROM's `ldr r4, =gUnknown_0808D874;
 * ldr r0, [r4]; ldr r1, [r0]` is a THREE-level chain: 0x0808D874 is the pool
 * word, it holds 0x08499590, and gMapData is the pointer variable.
 * Write the global's name -- the ROM word pointing AT it is not a symbol.
 * The later `ldr r4, [r4]` that swaps the pool address for &gMapData
 * in sl is CSE's, not the source's.
 *
 * THE ONE NON-OBVIOUS SPELLING, and it cost the only rewrite here: the two
 * column neighbours must be `MAP->rowOffset[y] + (x - 1)` with the
 * parentheses.  All three spellings differ, measured side by side:
 *   `rowOffset[y] + (x - 1)`  ->  ldrh; subs r0,#1; adds r0,r0,x   <- the ROM
 *   `rowOffset[y] - 1 + x`    ->  subs x,#1 hoisted ABOVE the ldrh
 *   `rowOffset[y] + x - 1`    ->  the -1 folds into the 0x1432 pool word,
 *                                 which becomes 0x1431 and loses the reloc
 * Written up in docs/agbcc-codegen.md.
 *
 * `c` (the terrain byte) is live across every call, which is what puts it in
 * r8 and forces the high-register push; the trailing if/else-if chain tests it
 * in source order 1, 5, 3, 4 and is NOT a switch -- a switch over those four
 * would have been ordered. */

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

void sub_0800C2D0(int x, int y, int f)
{
    int c;
    int v;

    c = MAP->terrain[MAP->rowOffset[y] + x];
    v = sub_08007DD0(x, y);

    if (v < 0 && c == 7)
        v = 0x2A;

    if (c != 2)
    {
        if (v == 0x2A)
            sub_080011F4(x, y, 7);
        sub_08001158(x, y, v);
    }

    if (f)
    {
        if (MAP->terrain[MAP->rowOffset[y - 1] + x] == 2)
            sub_08009F10(x, y - 1);
        if (MAP->terrain[MAP->rowOffset[y + 1] + x] == 2)
            sub_08009F10(x, y + 1);
        if (MAP->terrain[MAP->rowOffset[y] + (x - 1)] == 2)
            sub_08009F10(x - 1, y);
        if (MAP->terrain[MAP->rowOffset[y] + (x + 1)] == 2)
            sub_08009F10(x + 1, y);
    }

    sub_0800ABD0(x, y);

    if (c == 1)
        sub_08007F14(x, y, 1);
    else if (c == 5)
    {
        v = sub_0800F418(x, y);
        sub_08001158(x, y, v);
        v = sub_080016D0(x, y);
        sub_08001158(x, y, v);
    }
    else if (c == 3)
        sub_0800B048(x, y);
    else if (c == 4)
    {
        v = sub_080016D0(x, y);
        sub_08001158(x, y, v);
    }
}

void sub_0800C454(int x, int y, int t)
{
    int a;
    int b;
    int u;

    if ((s8)gUnknown_0200B0B0->unk12 > 0x3B && sub_0800C840(x, y) == 0)
        return;

    if (sub_0800C6E8(t, &a, &b))
    {
        sub_0800C7A4(t);
        sub_0800C608(a, b);
        sub_080011F4(a, b, 1);
        sub_08007F14(a, b, 1);
    }

    if (sub_0800C840(x, y) == 2)
        sub_0800C7A4(MAP->terrain[MAP->rowOffset[y] + x]);

    u = (u16)sub_080012DC(t);

    if (sub_0800C840(x, y))
    {
        if (MAP->terrain[MAP->rowOffset[y] + x] == t)
            return;
        sub_0800C608(x, y);
    }

    sub_080011F4(x, y, t);
    sub_08007F14(x, y, u);
    sub_0800C574(x, y, t);
    sub_080219AC();
    sub_0800C75C(t, x, y);
    sub_0800A588(x, y);
    sub_0800ABD0(x, y);
    sub_08007F9C(x, y);
    sub_0800BEE4(x, y);
    sub_0800EC20(x, y);
}
