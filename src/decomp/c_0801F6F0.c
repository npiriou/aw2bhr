#include "global.h"

/* Promoted from assembly; each function below is byte-for-byte
 * identical to the original. Order is address order and must
 * stay that way -- the linker places this file's .text as one
 * contiguous block at 0x0801F6F0.
 * sub_0801F6F0 @ 0x0801F6F0
 */

/* One neighbour step of the movement-range flood fill. Adds the cursor origin
 * gUnknown_0300409C to the (dx, dy) it is given, costs the destination cell,
 * and -- if the new cost beats what gUnknown_03003340 already holds and the
 * cell passes the four bounds/blocking tests -- writes a 4-byte {x, y, tag,
 * cost} record through the gUnknown_03003F64 queue cursor and records the cost.
 *
 * `idx` IS ASSIGNED INSIDE THE EXPRESSION, and that is the whole ordering fact.
 * As its own preceding statement it loads gMapData first; the ROM
 * loads gUnknown_084999C8 first, because that pointer is the OUTERMOST base of
 * the cost term and the map is only reached inside its index. Written as a
 * separate statement everything else is identical and the two pool words come
 * out swapped.
 *
 * The `&` operands are `unk2a & unk24[...]`, not the other way round: the ROM
 * materialises `&p->unk2a` (three instructions, since 0x2a is past the `ldrb`
 * displacement range) BEFORE the `lsrs #0x1e` that indexes unk24.
 *
 * `x` and `y` are u8 LOCALS -- shorten_compare narrows both bound tests to
 * unsigned char, which is where the `bhs` comes from; an int local gives the
 * signed `bge`. `v` is u16 and the two views of the same sum (`lsrs #0x10` for
 * the stored byte, `asrs #0x10` for the two signed compares) fall out of that
 * one declaration. */
struct Map
{
    /* 0x0000 */ u16 unk00;
    /* 0x0002 */ u16 unk02;
    /* 0x0004 */ u16 unk04;
    /* 0x0006 */ u16 unk06;
    /* 0x0008 */ u8 filler_0008[0x0A];
    /* 0x0012 */ u8 unk0012[0x0508];
    /* 0x051A */ u8 unk051A[0x0F18];
    /* 0x1432 */ u8 unk1432[0x0A10];
    /* 0x1E42 */ u8 unk1E42[0x0508];
    /* 0x234A */ u8 unk234A[0x0508];
    /* 0x2852 */ u8 unk2852[0x0A10];
    /* 0x3262 */ u8 unk3262[0x0F18];
    /* 0x417A */ u16 unk417A[0x100];
};

void sub_0801F6F0(u8 a1, u8 a2, u8 a3)
{
    u8 x;
    u8 y;
    int idx;
    u16 v;

    x = a2 + gUnknown_0300409C->unk00;
    y = a3 + gUnknown_0300409C->unk01;

    v = gUnknown_084999C8->unk00[((struct Map *)gMapData)->unk1432[
            idx = ((struct Map *)gMapData)->unk417A[y] + x] & 0x1F]
        + (s8)gUnknown_03003340[gUnknown_0300409C->unk01][gUnknown_0300409C->unk00];

    if ((s16)v >= gUnknown_03003340[y][x])
        return;

    if (gUnknown_084999C8->unk22 != 0
        && ((struct Map *)gMapData)->unk0012[idx] != 0
        && (gUnknown_084999C8->unk2a
            & gUnknown_084999C8->unk24[((struct Map *)gMapData)->unk0012[idx] >> 6]))
        return;

    if ((s16)v > gUnknown_084999C8->unk20)
        return;
    if (x >= gUnknown_084999C8->unk28)
        return;
    if (y >= gUnknown_084999C8->unk29)
        return;
    if (gUnknown_030040E0 > 0x15C)
        return;

    gUnknown_030040E0++;
    gUnknown_03003F64[0] = x;
    gUnknown_03003F64[1] = y;
    gUnknown_03003F64[2] = a1;
    gUnknown_03003F64[3] = v;
    gUnknown_03003F64 += 4;
    gUnknown_03003340[y][x] = v;
}
