#include "global.h"

/* Promoted from assembly; each function below is byte-for-byte
 * identical to the original. Order is address order and must
 * stay that way -- the linker places this file's .text as one
 * contiguous block at 0x08057D90.
 * sub_08057D90 @ 0x08057D90
 */

/* The 0x08499590 map header, cut where this function reads it. Same shape as
 * c_08058BB4.c's `struct Map`, but the plane boundary carved here is +0x2D5A
 * (which sits inside that file's unk2852) because that is the constant this
 * function's pool word carries. Only a member's START offset enters the address
 * arithmetic, so the split is byte-neutral. */
struct Map
{
    /* 0x0000 */ u16 unk00;
    /* 0x0002 */ u16 unk02;
    /* 0x0004 */ u8 filler_04[0x0e];
    /* 0x0012 */ u8 unk0012[0x2d48];
    /* 0x2d5a */ u8 unk2D5A[0x1420];
    /* 0x417a */ u16 unk417A[0x100];
};

/* Scans the whole map for the reachable cell with the lowest cost byte and
 * writes its (x, y) back through the two out-parameters, after running two
 * gMapData-plane setups and one indirect draw call.
 *
 * Three things were the whole difference and all three are gMapData
 * addressing, i.e. the idiom c_0805AD90.c and the header note at
 * include/unknown-globals.h describe:
 *
 *  - the planes must be STRUCT MEMBERS. `gMapData[0x12 + idx]` on the
 *    bare `u8 *` folds 0x12 into ldrb's displacement (`ldrb r0,[r1,#18]`),
 *    where the ROM computes `(map + 0x12) + idx`; `map->unk0012[idx]` is the
 *    spelling that keeps the constant on the base. Same for +0x2D5A.
 *  - `map->unk417A[y]` likewise. Written as
 *    `*(u16 *)(gMapData + 0x417A + y * 2)` it reassociates to
 *    `(map + y * 2) + 0x417A`, the wrong way round.
 *  - `map` is bound INSIDE the innermost `if`, not before the loops: the ROM
 *    reloads gMapData once per accepted cell and CSEs the three uses
 *    within it. A binding outside the loops loads it once for the function.
 *
 * The loop counters are `s16` (the `lsl #16 / asr #16` biv) and the two winners
 * are `u16` locals in promoted SImode stack slots -- which is what makes
 * `bx = x` a zero-extending `lsrs` rather than a sign-extending `asrs`.
 *
 * MATCHED. */
void sub_08057D90(s16 *px, s16 *py)
{
    struct Map *map;
    s16 x;
    s16 y;
    u16 bx;
    u16 by;
    int best;
    int idx;
    int v;

    bx = 0;
    by = 0;

    sub_0801F92C(gMapData + 0x2D5A);
    gUnknown_030013EC(*px, *py, gUnknown_030040D8->unk00, 0x78, by);
    sub_0801F92C(gMapData + 0x2852);
    sub_080202A4(gUnknown_030040D8);

    best = 0x7FFF;

    for (y = 0; y < ((struct Map *)gMapData)->unk02; y++)
    {
        for (x = 0; x < ((struct Map *)gMapData)->unk00; x++)
        {
            if ((s8)gUnknown_03003340[y][x] >= 0)
            {
                map = (struct Map *)gMapData;
                idx = map->unk417A[y] + x;
                if (map->unk0012[idx] == 0)
                {
                    v = map->unk2D5A[idx];
                    if (v <= best)
                    {
                        best = v;
                        bx = x;
                        by = y;
                    }
                }
            }
        }
    }

    *px = bx;
    *py = by;
}
