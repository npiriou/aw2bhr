#include "global.h"

/* Promoted from assembly; each function below is byte-for-byte
 * identical to the original. Order is address order and must
 * stay that way -- the linker places this file's .text as one
 * contiguous block at 0x080413E8.
 * sub_080413E8 @ 0x080413E8
 */

/* MATCHED byte-for-byte, wave 50 (W50-J). Hand-derived to 98.1% size-exact,
 * closed by decomp-permuter on the last 6 bytes.
 *
 * THE PERMUTER'S ONLY CHANGE WAS `(u8)` ON sub_080413B4's FOURTH ARGUMENT, and
 * it emits NO instruction. `v` is either `lsrs #28` of a halfword (4 bits) or
 * the constant 6, so gcc's `nonzero_bits` proves `v & 0xff` is a no-op and
 * combine deletes it -- but the cast still makes the argument a NOP_EXPR
 * rather than a bare pseudo, so it goes through precompute and lands in r3
 * BEFORE the plain register copies. That reorders the whole argument setup
 * from `r0, r1, r2, r3` to the ROM's `r3, r0, r1, r2`. Without the cast the
 * function is 6 bytes off in exactly this one spot. See the wave-50 entry in
 * docs/agbcc-codegen.md.
 *   The cast is at THIS USE. It is a hint that sub_080413B4's fourth parameter
 * is really `u8`, but the promoted src/decomp/c_080413B4.c defines it `int`
 * and that definition wins; retyping it would touch a byte-exact file and its
 * other callers, so it is recorded here and NOT acted on.
 *
 * ONE local for both byte reads, not two. The ROM keeps the gUnknown_020288B4
 * cell and the map's unit-id cell in the SAME register (r4) -- two locals put
 * them in r3 and r4 and cost the map its register, flipping r3/r4 across the
 * whole body. This is wave 49's "two disjoint values in one function are
 * usually ONE local" rule, and it was worth the entire register assignment.
 *
 * The map is reached through a LOCALLY DECLARED struct cast onto the `u8 *`
 * symbol, per gMapData's note in unknown-globals.h: only a
 * COMPONENT_REF keeps the ROM's `(map + K) + idx` association. Written as
 * pointer arithmetic, `gMapData[0x12 + idx]` folds K into the load
 * displacement and `*(u16 *)(gMapData + 0x417A + y * 2)` reassociates
 * the constant outward. Same for gUnknown_030013D0: spelled
 * `*(s16 *)(gUnknown_030013D0 + 0x18)` agbcc folds +0x18 into the POOL WORD
 * (`.word gUnknown_030013D0+0x18`), where the ROM has a clean
 * `.word gUnknown_030013D0` and `movs r1,#0x18`. The struct-cast spelling that
 * c_08020984.c already uses is what produces the ROM's form.
 *
 * `(s8)` on gUnknown_03003340[y][x] is a cast at the use, not evidence the
 * table should be retyped: the array stays `u8 *[]`. agbcc would have used
 * `ldrsb rD,[rB,rI]` for a genuine `s8 *`, and the ROM has
 * `adds rD,rB,rI; ldrb; lsls #24; asrs #24`.
 *
 * c_08020984.c is this function's near-twin -- same sub_0803DF54 /
 * gUnknown_020288B4 / unk02_6 / sub_08026F9C / sub_080251BC sequence over the
 * same map -- and settled the unk18 and unk02_6 spellings. */

struct Unk413E8Map
{
    /* 0x0000 */ u16 width;
    /* 0x0002 */ u16 height;
    /* 0x0004 */ u8 filler_0004[0x0012 - 0x0004];
    /* 0x0012 */ u8 unit[0x417A - 0x0012];
    /* 0x417A */ u16 rowOffset[1];
};
#define MAP ((struct Unk413E8Map *)gMapData)

int sub_080413E8(void)
{
    struct Unk02028360 *unit;
    int v;
    u8 t;
    int x;
    int y;

    sub_0804138C();

    for (y = 0; y < MAP->height; y++)
    {
        for (x = 0; x < MAP->width; x++)
        {
            if ((s8)gUnknown_03003340[y][x] < 0)
                continue;

            unit = sub_0803DF54(x, y);
            t = gUnknown_020288B4[MAP->rowOffset[y] + x];

            v = 0;

            if (unit != NULL)
                v = unit->unk02_6;

            if (t)
                v = 6;

            if (v)
            {
                sub_080251BC(gUnknown_03003F38, 0, &gUnknown_03003100.pos);

                if (((struct Unk030013D0 *)gUnknown_030013D0)->unk18 == 0)
                    continue;

                sub_080413B4(x, y, 0, (u8)v);
            }
            else
            {
                t = MAP->unit[MAP->rowOffset[y] + x];

                if (t == 0)
                    continue;

                if (sub_08026F9C(gUnknown_03003F38, t) == 1)
                    continue;

                sub_080251BC(gUnknown_03003F38, t, &gUnknown_03003100.pos);

                if (((struct Unk030013D0 *)gUnknown_030013D0)->unk18 == 0)
                    continue;

                sub_080413B4(x, y, t, 0);
            }
        }
    }

    return gUnknown_030040A8;
}
