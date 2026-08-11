#include "global.h"

/* Promoted from assembly; each function below is byte-for-byte
 * identical to the original. Order is address order and must
 * stay that way -- the linker places this file's .text as one
 * contiguous block at 0x0800164C.
 * sub_0800164C @ 0x0800164C, sub_0800168C @ 0x0800168C, sub_080016D0 @ 0x080016D0
 */

/* A predicate over the byte plane sub_080011F4 writes: kinds 7 and 0xd answer
 * 0 outright, and everything else answers "is it not 0x13".
 *
 * The tail is do_store_flag's branchless returned !=:
 * movs r1,#0x13; eors r1,r2; rsbs r0,r1; orrs r0,r1; lsrs r0,#0x1f. The
 * movs r0, #0 sits AHEAD of the compares, so the 0 is one shared result
 * variable that the two beqs fall through to -- not a return 0 per arm. */
int sub_0800164C(int x, int y)
{
    u8 *p;
    u8 *rows;
    u8 *tiles;
    int t;
    int off;
    int v;
    int r;

    p = gMapData;
    t = y * 2;
    rows = p + 0x417A;
    off = *(u16 *)(rows + t) + x;
    tiles = p + 0x1432;
    v = tiles[off];

    r = 0;

    if (v != 7 && v != 0xD)
        r = (v != 0x13);

    return r;
}

/* sub_0800164C's neighbour and NOT a copy of it: one more excluded kind
 * (0x13, which is the one sub_0800164C compares against) and a different
 * final comparand (2). Same shared-r tail; see that file for the idiom. */
int sub_0800168C(int x, int y)
{
    u8 *p;
    u8 *rows;
    u8 *tiles;
    int t;
    int off;
    int v;
    int r;

    p = gMapData;
    t = y * 2;
    rows = p + 0x417A;
    off = *(u16 *)(rows + t) + x;
    tiles = p + 0x1432;
    v = tiles[off];

    r = 0;

    if (v != 7 && v != 0xD && v != 0x13)
        r = (v != 2);

    return r;
}

/* The read half of c_08001158.c: same row-offset arithmetic, the HALFWORD
 * plane at +0xA22 (hence the extra lsls #1), and the cell handed to
 * sub_08001704 alongside the coordinates that produced it. r0 and r1 are never
 * clobbered on the way, so the first two arguments cost nothing.
 *
 * It RETURNS what sub_08001704 returns, and the epilogue is the only thing
 * that says so: pop {r1}; bx r1 keeps r0 intact, where a void function would
 * have popped the return address into r0 itself. */
int sub_080016D0(int x, int y)
{
    u8 *p;
    u8 *rows;
    u8 *tiles;
    int t;
    int off;

    p = gMapData;
    t = y * 2;
    rows = p + 0x417A;
    off = (*(u16 *)(rows + t) + x) * 2;
    tiles = p + 0xA22;
    return sub_08001704(x, y, *(u16 *)(tiles + off));
}
