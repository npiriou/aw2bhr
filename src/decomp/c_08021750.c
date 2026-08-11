#include "global.h"

/* Promoted from assembly; each function below is byte-for-byte
 * identical to the original. Order is address order and must
 * stay that way -- the linker places this file's .text as one
 * contiguous block at 0x08021750.
 * sub_08021750 @ 0x08021750
 */

/* Rebuilds the map's terrain plane from its tile-id table: for every cell,
 * plane[idx] = gUnknown_0849959C[tile[idx]], where idx = rowOffset[y] + x.
 *
 * Two authoring points the diff insisted on:
 *  - The RHS lookup is its own statement, evaluated BEFORE the store address.
 *    Spelled as one assignment, agbcc forms &plane[idx] first, which makes
 *    0x1432's pseudo the older one; it then wins the callee-saved register and
 *    0x0A22 is rematerialised inside the loop.  The ROM has it the other way
 *    round (0x0A22 in ip, `ldr r0,=0x1432` in the loop body), which is the
 *    tile[] reference having been created first.
 *  - The seven zeroed fields at +4..+0x10 are seven separate assignments, one
 *    `strh` each -- an array plus a loop would not unroll.
 *
 * gMapData is named nine times here and still gets an ordinary inline
 * pool word rather than a -fforce-addr .rodata word, so reference count alone
 * does not decide that; contrast sub_08022580 in this same block, which names
 * it twice and does get one.
 *
 * The parameter is an int, not a pointer: `adds r0,#0x4c` runs on it before the
 * `lsls #0x18; lsrs #0x18`, and that truncation is the conversion to
 * sub_0803CF3C's already-promoted `u8` first parameter. */
struct Unk21750Map
{
    /* 0x0000 */ u16 width;
    /* 0x0002 */ u16 height;
    /* 0x0004 */ u16 unk04;
    /* 0x0006 */ u16 unk06;
    /* 0x0008 */ u16 unk08;
    /* 0x000a */ u16 unk0a;
    /* 0x000c */ u16 unk0c;
    /* 0x000e */ u16 unk0e;
    /* 0x0010 */ u16 unk10;
    /* 0x0012 */ u8 filler_0012[0x0A22 - 0x12];
    /* 0x0A22 */ u16 tile[(0x1432 - 0x0A22) / 2];
    /* 0x1432 */ u8 plane[0x417A - 0x1432];
    /* 0x417A */ u16 rowOffset[(0x421A - 0x417A) / 2];
    /* 0x421A */ u8 buf[1];
};

void sub_08021750(int a)
{
    int x;
    int y;
    u16 t;

    sub_0803CF3C((u8)(a + 0x4c), (int)(gMapData + 0x421A));

    ((struct Unk21750Map *)gMapData)->unk04 = 0;
    ((struct Unk21750Map *)gMapData)->unk06 = 0;
    ((struct Unk21750Map *)gMapData)->unk08 = 0;
    ((struct Unk21750Map *)gMapData)->unk0a = 0;
    ((struct Unk21750Map *)gMapData)->unk0c = 0;
    ((struct Unk21750Map *)gMapData)->unk0e = 0;
    ((struct Unk21750Map *)gMapData)->unk10 = 0;

    for (y = 0; y < ((struct Unk21750Map *)gMapData)->height; y++)
    {
        for (x = 0; x < ((struct Unk21750Map *)gMapData)->width; x++)
        {
            t = ((struct Unk21750Map *)gMapData)->tile[
                    ((struct Unk21750Map *)gMapData)->rowOffset[y] + x];
            ((struct Unk21750Map *)gMapData)->plane[
                ((struct Unk21750Map *)gMapData)->rowOffset[y] + x] =
                gUnknown_0849959C[t];
        }
    }
}
