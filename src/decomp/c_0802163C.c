#include "global.h"

/* Promoted from assembly; each function below is byte-for-byte
 * identical to the original. Order is address order and must
 * stay that way -- the linker places this file's .text as one
 * contiguous block at 0x0802163C.
 * sub_0802163C @ 0x0802163C
 */

/* Loads map <a> into gMapData: for the 0xB4..0xBF range it forwards to
 * sub_08021750 (the "rebuild from the tile table already in the map" path), and
 * otherwise it seeds the header from the gUnknown_03003F68 blob, copies the
 * name, and fills tile[]/plane[] from the blob's halfword tile stream.
 *
 * The guard is agbcc's own range test: `a >= 0xb4 && a <= 0xbf` goes through
 * fold's build_range_check and comes out as `(unsigned)(a - 0xb4) <= 0xb`,
 * which is the ROM's `subs r0,#0xb4 / cmp r0,#0xb / bhi`.  The parameter is an
 * `int` (unknown-functions.h already says so), so the subtraction happens in
 * SImode and there is no truncation -- contrast sub_08021810, whose copy of the
 * same test runs on a u8 member and therefore carries `lsls #0x18; lsrs #0x18`.
 *
 * gUnknown_03003F68's blob is u8 width, u8 height, then the halfword tile
 * stream at +2; the `ldrh r4,[r0,#2]` with the index already scaled is the +2
 * folded into the load displacement, i.e. a member array and not `+ 1` on a
 * bare u16 pointer. */

struct Unk2163CMap
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
    /* 0x421A */ u8 name[0x4233 - 0x421A];
    /* 0x4233 */ u8 unk4233;
};
struct Unk2163CSrc
{
    /* 0x00 */ u8 width;
    /* 0x01 */ u8 height;
    /* 0x02 */ u16 tile[1];
};

void sub_0802163C(int a)
{
    int x;
    int y;
    u16 t;

    if (a >= 0xb4 && a <= 0xbf)
    {
        sub_08021750(a);
        return;
    }

    sub_080247A4(a);

    ((struct Unk2163CMap *)gMapData)->width =
        ((struct Unk2163CSrc *)gUnknown_03003F68)->width;
    ((struct Unk2163CMap *)gMapData)->height =
        ((struct Unk2163CSrc *)gUnknown_03003F68)->height;
    ((struct Unk2163CMap *)gMapData)->unk04 = 0;
    ((struct Unk2163CMap *)gMapData)->unk06 = 0;
    ((struct Unk2163CMap *)gMapData)->unk08 = 0;
    ((struct Unk2163CMap *)gMapData)->unk0a = 0;
    ((struct Unk2163CMap *)gMapData)->unk0c = 0;
    ((struct Unk2163CMap *)gMapData)->unk0e = 0;
    ((struct Unk2163CMap *)gMapData)->unk10 = 0;

    sub_0803CC84(((struct Unk2163CMap *)gMapData)->name,
                 sub_08024944(a));
    ((struct Unk2163CMap *)gMapData)->unk4233 = sub_0802490C(a);

    sub_080215FC();

    for (y = 0; y < ((struct Unk2163CMap *)gMapData)->height; y++)
    {
        for (x = 0; x < ((struct Unk2163CMap *)gMapData)->width; x++)
        {
            t = ((struct Unk2163CSrc *)gUnknown_03003F68)->tile[
                    ((struct Unk2163CMap *)gMapData)->rowOffset[y] + x];
            ((struct Unk2163CMap *)gMapData)->tile[
                ((struct Unk2163CMap *)gMapData)->rowOffset[y] + x] = t;
            ((struct Unk2163CMap *)gMapData)->plane[
                ((struct Unk2163CMap *)gMapData)->rowOffset[y] + x] =
                gUnknown_0849959C[t];
        }
    }

    sub_0802481C();
}
