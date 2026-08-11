#include "global.h"

/* Promoted from assembly; each function below is byte-for-byte
 * identical to the original. Order is address order and must
 * stay that way -- the linker places this file's .text as one
 * contiguous block at 0x080401B4.
 * sub_080401B4 @ 0x080401B4, sub_08040200 @ 0x08040200
 */

#include "proc.h"
/* Hands the proc's entry to sub_0803FF48 with the entry's cell pair and its
 * class tag, then reports the entry's index and refreshes the list.
 *
 * sub_0803FF48's FOURTH argument is invisible at the call: r3 still holds the
 * proc from the `adds r3, r0, #0` in the prologue, so no instruction sets it
 * up -- the declared `ProcPtr` fourth parameter is what proves it is there.
 *
 * The magic-number chain ends `asr #2` and NOT `asr #8`, so this is the bare
 * pointer subtraction `ent - gUnitRecords` with no `>> 6`: it is the unit
 * index, not the army number that sub_0804203C and sub_08041FE0 derive. */
struct Unk401B4Proc
{
    /* 00 */ u8 filler_00[0x4c];
    /* 4c */ struct UnitRecord *unk4c;
};
/* Marks the entry's cell as occupied on two planes of the gMapData
 * map, then refreshes. The map header is modelled as a struct for the same
 * reason sub_08041EA8 and sub_08040790 need it: the ROM computes every plane
 * address as `(map + K) + idx`, an association that only survives through a
 * COMPONENT_REF.
 *
 * gMapData is RE-READ for the second store (`ldr r2,[r5]` a second
 * time off the same pool register): the intervening `strb` goes through a
 * `u8 *` and kills the non-const pointer global's MEM, so writing the global
 * honestly twice is what reproduces the reload.
 *
 * sub_0803FECC's third argument is invisible at the call -- r1 still holds this
 * function's own second parameter -- which its declared `ProcPtr` third
 * parameter is what proves. */
struct Unk40200Map
{
    /* 0x0000 */ u8 filler_0000[0x0A22];
    /* 0x0A22 */ u16 unkA22[0x508];
    /* 0x1432 */ u8 terrain[0x2D48];
    /* 0x417A */ u16 rowOffset[1];
};

void sub_080401B4(struct Unk401B4Proc *proc)
{
    struct UnitRecord *ent = proc->unk4c;

    sub_0803FF48(ent->unk02, ent->unk03, gUnitTypeData[ent->unk00].unk18, proc);
    sub_08025D60(ent - gUnitRecords);
    sub_080258CC();
}

void sub_08040200(struct Unk02028360 *ent, ProcPtr a2)
{
    sub_0803FECC(ent->unk00, ent->unk01, a2);

    ((struct Unk40200Map *)gMapData)->terrain[
        ((struct Unk40200Map *)gMapData)->rowOffset[ent->unk01]
        + ent->unk00] = 1;

    ((struct Unk40200Map *)gMapData)->unkA22[
        ((struct Unk40200Map *)gMapData)->rowOffset[ent->unk01]
        + ent->unk00] = 4;

    sub_08024268();
    sub_0803E0D0((struct Unk3E0D0 *)ent);
    sub_080219AC();
}
