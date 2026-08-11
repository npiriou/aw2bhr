#include "global.h"

/* Promoted from assembly; each function below is byte-for-byte
 * identical to the original. Order is address order and must
 * stay that way -- the linker places this file's .text as one
 * contiguous block at 0x0805ACA8.
 * sub_0805ACA8 @ 0x0805ACA8, sub_0805ACFC @ 0x0805ACFC
 */

/* The 0x0805ACA8 driver's per-cell tester, and the near-twin of sub_08058E88
 * in src/decomp/c_08058BB4.c: same guards, same `rowOffset[y] + x` key, same
 * (s8) read of gUnknown_03003340. It differs only in the tail -- 0x78 is
 * rejected as well as 0x79, and the terrain gate accepts exactly the two
 * codes 0xb and 0xd instead of consulting the move table.
 *
 * sub_0805C128 is the same shape again with a different tail.
 *
 * struct Map is the wave-35 canonical layout, copied verbatim from
 * c_08058BB4.c -- gMapData stays `u8 *` and the byte arithmetic is
 * written through a locally declared struct cast onto it, which is what keeps
 * `(map + K) + idx` from folding K into the load displacement.
 *
 * The single `lsls #0x18; asrs #0x18` serves all three compares, so it is an
 * int local carrying one explicit (s8) cast, not an s8 local -- the same
 * reading c_08058BB4.c records for sub_08058E88. */
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

/* Offers the four cells orthogonally adjacent to (x, y) to sub_0805ACFC and
 * reports whether any of them was accepted. The exact shape of sub_08058C54
 * in src/decomp/c_08058BB4.c, with a 1/0 result instead of best/-1.
 *
 * BINDING THE SENTINEL IS LOAD-BEARING, for the reason c_08058BB4.c records:
 * the literal 0x270F written at both the seed store and the final test stays a
 * plain constant, loaded into r0 and stored from r0, so the ROM's redundant
 * `ldr r0,=0x270F; mov r8,r0; mov r0,r8; strh` has nothing to come from. Its
 * own statement makes it a pseudo whose live range starts before the store, so
 * the store must read it back out of the callee-saved register.
 *
 * The tail is written `== sentinel -> 0` and not `!= sentinel -> 1`: agbcc
 * branches to the `if` BODY and falls through to what follows, so the arm
 * written as the body is the one laid out after the literal pool. The ROM's
 * `beq` reaches `movs r0,#0`, so `return 0` is the body. The opposite spelling
 * compiles to the same instructions in the opposite order.
 *
 * The fourth call's `adds r4,#1` clobbers y because y is dead after it; that
 * falls out of writing `y + 1` last. */
u8 sub_0805ACA8(int x, int y, u16 * out)
{
    int sentinel;

    sentinel = 0x270F;
    out[0] = sentinel;

    sub_0805ACFC(x - 1, y, out);
    sub_0805ACFC(x + 1, y, out);
    sub_0805ACFC(x, y - 1, out);
    sub_0805ACFC(x, y + 1, out);

    if (out[0] == sentinel)
        return 0;

    return 1;
}

void sub_0805ACFC(int x, int y, u16 * out)
{
    struct Map * map;
    int idx;
    int t;
    int terrain;

    if (x < 0)
        return;
    if (y < 0)
        return;

    map = (struct Map *)gMapData;

    if (x >= map->unk00)
        return;
    if (y >= map->unk02)
        return;

    idx = map->unk417A[y] + x;

    if (map->unk0012[idx] != gUnknown_03003F38 && map->unk0012[idx] != 0)
        return;

    t = (s8)gUnknown_03003340[y][x];

    if (t < 0)
        return;
    if (t == 0x78)
        return;
    if (t == 0x79)
        return;

    terrain = map->unk1432[idx] & 0x1f;

    if (terrain != 0xd && terrain != 0xb)
        return;

    out[0] = x;
    out[1] = y;
}
