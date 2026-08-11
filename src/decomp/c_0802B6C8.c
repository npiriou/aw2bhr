#include "global.h"

/* Promoted from assembly; each function below is byte-for-byte
 * identical to the original. Order is address order and must
 * stay that way -- the linker places this file's .text as one
 * contiguous block at 0x0802B6C8.
 * sub_0802B6C8 @ 0x0802B6C8
 */

/* Classifies the cell at (x, y). The `* 0x55555555; rsbs; asrs #8` run is ONE
 * expression, not two: agbcc folds the exact division by the 0x0c element
 * stride that `unit - gUnitRecords` needs into the `>> 6` that recovers
 * the army, so the whole thing lands as a single shift-and-add chain.
 *
 * `army` has to be a BINDING LOCAL. Written inline as the third argument the
 * instructions are identical but agbcc emits it AFTER the two simple
 * arguments, where the ROM computes it first; the local also moves the
 * gUnitRecords base from r3 to the r1 the ROM uses.
 *
 * The (u8) is real, not a tidy-up: sub_0802706C's third parameter is u16 and
 * the ROM truncates with `lsls #0x18; lsrs #0x18`. The prototype is right --
 * the promoted sub_0802B91C passes an s16 there with no truncation at all. */
/* WAVE 35: CANONICAL `struct Map`. Eight drafts across blocks 0x08029-0x0802B
 * each invented their own body for this tag, with 2 to 7 named fields. Every
 * one compiles and byte-matches ALONE, so trymatch cannot see the problem;
 * merged into a promoted unit it is a hard `redefinition of struct Map`.
 * All seven distinct bodies describe the SAME layout at different
 * resolutions -- the offsets reconcile exactly -- so this is their union, the
 * most refined reading of each region. Byte-neutral: only a field's START
 * OFFSET enters the address arithmetic, never its declared length, and no
 * draft referenced any filler. Keep the drafts in sync; sync_work.py
 * reintroduces whatever the drafts say. */
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
    /* 0x2852 */ u8 unk2852[0x1928];
    /* 0x417A */ u16 unk417A[0x100];
};

u8 sub_0802B6C8(u8 x, u8 y)
{
    struct UnitRecord *unit;
    u8 army;
    int idx;

    idx = x + ((struct Map *)gMapData)->unk417A[y];

    if (((struct Map *)gMapData)->unk0012[idx] == 0)
        return 0;

    unit = &gUnitRecords[((struct Map *)gMapData)->unk0012[idx]];
    army = ((unit - gUnitRecords) >> 6) + 1;

    if (sub_0802706C(unit->unk00, gCurrentArmyIndex, army))
        return 2;

    if (unit->unk07 | unit->unk08)
        return 2;

    return 1;
}
