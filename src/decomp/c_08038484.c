#include "global.h"

/* Promoted from assembly; each function below is byte-for-byte
 * identical to the original. Order is address order and must
 * stay that way -- the linker places this file's .text as one
 * contiguous block at 0x08038484.
 * sub_08038484 @ 0x08038484
 */

/* THE 0x10 TABLE IS AN ARRAY, AND THE SHARED STRUCT SAYS IT IS TWO SCALARS.
 * gUnknown_0200C420's +0x10 and +0x12 are declared `u16 unk10; u16 unk12;` in
 * unknown-globals.h, but this function subscripts that pair with the runtime
 * value sub_0803866C() returns. The difference is visible in the pool word and
 * is worth 3 instructions: `&gUnknown_0200C420.unk10` const-folds to the single
 * address constant `gUnknown_0200C420+0x10`, while the ROM's word is a BARE
 * `gUnknown_0200C420` with `adds r5,r4,#0; adds r5,#0x10` computing the member
 * offset at runtime -- which is what an array member emits, because the
 * variable subscript stops the fold.
 *   Reshaping unk10/unk12 into `u16 unk10[2]` is the reading the codegen
 * actually supports, but it is a SHARED member (sub_0803C52C reads +0x10 twice,
 * sub_08016A54 zeroes both) and the brief forbids reshaping one to suit a
 * single function, so the layout is spelled locally instead -- the same device
 * src/decomp/c_08038848.c uses for gMapData. Recorded rather than
 * changed; if a second function turns up subscripting the same pair, that is
 * the evidence to promote it to an array in the header.
 *
 * `ok` is `int`: the `lsls #0x18; lsrs #0x18` after sub_0807821C is agbcc
 * re-narrowing that function's bool8 return, not a narrow local. The else arm
 * stores `ok` rather than a literal 0 -- the ROM reuses r4 there instead of
 * emitting a fresh `movs r0,#0`, which is what naming the variable produces. */
struct Unk38484Tbl
{
    /* 0x00 */ u8 filler_00[0x10];
    /* 0x10 */ u16 unk10[2];
};

void sub_08038484(void)
{
    struct Unk38484Tbl *tbl;
    int ok;

    ok = 0;

    if (sub_0803861C())
    {
        sub_08038368(gUnknown_03003FC0.unk02 - 0x8a, gUnknown_03004080,
                     gArmyRecords[sub_0807A908()].unk38);
        sub_0807823C(gUnknown_03003FC0.unk02 - 0x8a);
        gUnknown_0202FDFC.unk0c = gUnknown_03003FC0.unk02 - 0x8a;
        gUnknown_0202FDFC.unk11 = 1;
        ok = sub_0807821C(gUnknown_03003FC0.unk02 - 0x8a);
    }
    else
    {
        gUnknown_0202FDFC.unk0c = gUnknown_03003FC0.unk02 - 0x8a;
        gUnknown_0202FDFC.unk11 = ok;
    }

    sub_080346FC();

    if (ok)
    {
        tbl = (struct Unk38484Tbl *)&gUnknown_0200C420;

        if (tbl->unk10[sub_0803866C()] < sub_08038434())
            tbl->unk10[sub_0803866C()] = sub_08038434();

        sub_08045790();
    }
    else
    {
        sub_0803BADC();
    }
}
