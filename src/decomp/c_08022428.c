#include "global.h"

/* Promoted from assembly; each function below is byte-for-byte
 * identical to the original. Order is address order and must
 * stay that way -- the linker places this file's .text as one
 * contiguous block at 0x08022428.
 * sub_08022428 @ 0x08022428
 */

struct Unk22428Map
{
    /* 0x0000 */ u16 width;
    /* 0x0002 */ u16 height;
    /* 0x0004 */ u8 filler_0004[0x000C - 0x0004];
    /* 0x000C */ u16 camX;
    /* 0x000E */ u16 camY;
    /* 0x0010 */ u8 filler_0010[0x051A - 0x0010];
    /* 0x051A */ u8 unitId[0x234A - 0x051A];
    /* 0x234A */ u8 flag234A[0x417A - 0x234A];
    /* 0x417A */ u16 rowOffset[1];
};

/* MATCHED (wave 52, W52-E), by decomp-permuter at iteration 553 from the 98.5%
 * hand draft. PROMOTION MUST CARRY THE POOL WORDS:
 *
 *     "rodata": ["0x0809099C", "0x080909A0"]
 *
 * then re-run tools/split_rodata.py and tools/gen_lds.py. gUnknown_0809099C
 * and gUnknown_080909A0 are agbcc's own -fforce-addr words for
 * gMapData and gUnitRecords; the honest spelling names the
 * objects and the build places the two words (addends 0 and 4).
 *
 * THE WHOLE RESIDUAL WAS ONE REGISTER SWAP, and `(new_var = cx)` is what
 * closes it -- do not "clean it up". The 98.5% draft had
 *
 *     original            candidate
 *     mov r9, r0          mov sl, r0     <- cx
 *     mov sl, r0          mov r9, r0     <- cy
 *
 * plus the matching pair at the two uses. Writing the embedded assignment into
 * the first addend creates cx's reference at a point no statement boundary can
 * reach, which flips the two allocnos' priority order without moving either
 * definition. Measured alternatives, wave 52:
 *   - `gUnknown_0849957C + cy * 32 + cx` -- keeps the same registers and
 *     reorders the two uses instead. Worse.
 *   - swapping the `cx =` and `cy =` statements -- DOES flip r9/sl correctly,
 *     but then loads camY before camX. Trades 4 wrong bytes for 2.
 *   - declaration order of the two locals -- byte-identical either way. A
 *     local pseudo's allocno number comes from FIRST USE, not from
 *     expand_decl walking the declaration list.
 *
 * Two things were load-bearing before the permuter ever ran:
 *   - `cx`/`cy` hold the ALREADY-DOUBLED offsets and the address is
 *     `p + cx + cy * 32`. The exemplar's spelling (`cx = ... & 0xf` with
 *     `p + cx * 2 + cy * 64`, c_080223E0.c) sinks the doubling to the use as
 *     `lsls #2` / `lsls #7` and costs 4 bytes.
 *   - the guards must be ONE `||` chain with the drawing arm as the `else`;
 *     five separate `if (...) { blank; return; }` blocks lay the blank arm out
 *     after the drawing arm. */
void sub_08022428(u16 x, u16 y)
{
    struct UnitRecord *e;
    u8 id;
    int cx;
    int cy;
    u16 v;
    u16 hp;
    unsigned char new_var;

    if (y >= ((struct Unk22428Map *)gMapData)->height
        || (id = ((struct Unk22428Map *)gMapData)
                     ->unitId[((struct Unk22428Map *)gMapData)->rowOffset[y] + x]) == 0
        || ((struct Unk22428Map *)gMapData)
               ->flag234A[((struct Unk22428Map *)gMapData)->rowOffset[y] + x] == 0
        || !sub_0802571C(id)
        || (gUnitRecords[id].unk01 & 4) != 0)
    {
        sub_080223E0(x, y);
    }
    else
    {
        if ((gUnitRecords[id].unk01 & 1) != 0)
            v = 0x100;
        else
            v = id & 0xc0;

        cx = ((x - ((struct Unk22428Map *)gMapData)->camX) & 0xf) * 2;
        cy = ((y - ((struct Unk22428Map *)gMapData)->camY) & 0xf) * 2;

        e = &gUnitRecords[id];

        if (e->unk04_0 != 0)
            hp = Div(e->unk04_0 - 1, 10) + 1;
        else
            hp = 0;

        sub_0802216C((gUnknown_0849957C + (new_var = cx)) + cy * 32,
                     gUnitRecords[id].unk00,
                     v,
                     e->unk07 | e->unk08,
                     e->unk05_3,
                     hp,
                     (u8)(e->unk01 & 0x20),
                     sub_08043050((id >> 6) + 1) & 1);
    }
}
