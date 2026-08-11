#include "global.h"

/* Promoted from assembly; each function below is byte-for-byte
 * identical to the original. Order is address order and must
 * stay that way -- the linker places this file's .text as one
 * contiguous block at 0x080430B0.
 * sub_080430B0 @ 0x080430B0, sub_08043120 @ 0x08043120, sub_08043190 @ 0x08043190, sub_08043200 @ 0x08043200
 */

/* Family F049: four copies of one body that differ ONLY in the s16 index K
 * (0, 1, 2, 3 -- `movs rN, #0/2/4/6` at the three `ldrsh` sites), exactly as
 * data/families.json `varies` says.
 *
 * Three things had to be read off the ROM rather than guessed:
 *
 * 1. struct CoModeData.unk24 is an array of POINTERS. The ROM does
 *    `ldr r7,[r3]` on the table word and then `ldrsh r0,[r7,r2]` through the
 *    result. 0x24 + 8*4 == 0x44 fills the record exactly, and the two indices
 *    in use (gUnitTypeData[c].unk18, and sub_080432E0's 5/6/7) span it.
 *
 * 2. Both lookups happen BEFORE the gUnknown_03003FC0.unk08 early-out. That is
 *    not scheduling -- agbcc will not hoist a load above a branch -- so the
 *    two assignments really are the first two statements, unlike the F038
 *    accessors in c_08042E2C.c where the early-out comes first.
 *
 * 3. gUnitTypeData is NOT const, and that is what the second
 *    `ldrb r0,[r6,#0x18]` proves. With the header's old `const` the load is
 *    CSEd across `bl sub_080432E0`, the value rather than the address lives in
 *    r6, and the function is two bytes short. See the note on the declaration
 *    in include/unknown-globals.h.
 *
 * The dead `ldr r7, =gCoDataTable` at the top and the folded pool word
 * `gCoDataTable + 0x5c` (which asm/ symbolises as gUnknown_085D3E2C) both
 * fall straight out of the plain member spelling under -fforce-addr; neither
 * is something to reproduce by hand. */

int sub_080430B0(int a, int b, int c)
{
    s16 *p;
    s16 *q;

    p = gCoDataTable[a].unk38[b].unk24[gUnitTypeData[c].unk18];
    q = gCoDataTable[a].unk38[b].unk24[sub_080432E0(c)];

    if (gUnknown_03003FC0.unk08 == 0)
        return 0;

    if (gUnitTypeData[c].unk18 == 0)
        return p[0];

    return p[0] + q[0];
}

/* Family F049: four copies of one body that differ ONLY in the s16 index K
 * (0, 1, 2, 3 -- `movs rN, #0/2/4/6` at the three `ldrsh` sites), exactly as
 * data/families.json `varies` says.
 *
 * Three things had to be read off the ROM rather than guessed:
 *
 * 1. struct CoModeData.unk24 is an array of POINTERS. The ROM does
 *    `ldr r7,[r3]` on the table word and then `ldrsh r0,[r7,r2]` through the
 *    result. 0x24 + 8*4 == 0x44 fills the record exactly, and the two indices
 *    in use (gUnitTypeData[c].unk18, and sub_080432E0's 5/6/7) span it.
 *
 * 2. Both lookups happen BEFORE the gUnknown_03003FC0.unk08 early-out. That is
 *    not scheduling -- agbcc will not hoist a load above a branch -- so the
 *    two assignments really are the first two statements, unlike the F038
 *    accessors in c_08042E2C.c where the early-out comes first.
 *
 * 3. gUnitTypeData is NOT const, and that is what the second
 *    `ldrb r0,[r6,#0x18]` proves. With the header's old `const` the load is
 *    CSEd across `bl sub_080432E0`, the value rather than the address lives in
 *    r6, and the function is two bytes short. See the note on the declaration
 *    in include/unknown-globals.h.
 *
 * The dead `ldr r7, =gCoDataTable` at the top and the folded pool word
 * `gCoDataTable + 0x5c` (which asm/ symbolises as gUnknown_085D3E2C) both
 * fall straight out of the plain member spelling under -fforce-addr; neither
 * is something to reproduce by hand. */

int sub_08043120(int a, int b, int c)
{
    s16 *p;
    s16 *q;

    p = gCoDataTable[a].unk38[b].unk24[gUnitTypeData[c].unk18];
    q = gCoDataTable[a].unk38[b].unk24[sub_080432E0(c)];

    if (gUnknown_03003FC0.unk08 == 0)
        return 0;

    if (gUnitTypeData[c].unk18 == 0)
        return p[1];

    return p[1] + q[1];
}

/* Family F049: four copies of one body that differ ONLY in the s16 index K
 * (0, 1, 2, 3 -- `movs rN, #0/2/4/6` at the three `ldrsh` sites), exactly as
 * data/families.json `varies` says.
 *
 * Three things had to be read off the ROM rather than guessed:
 *
 * 1. struct CoModeData.unk24 is an array of POINTERS. The ROM does
 *    `ldr r7,[r3]` on the table word and then `ldrsh r0,[r7,r2]` through the
 *    result. 0x24 + 8*4 == 0x44 fills the record exactly, and the two indices
 *    in use (gUnitTypeData[c].unk18, and sub_080432E0's 5/6/7) span it.
 *
 * 2. Both lookups happen BEFORE the gUnknown_03003FC0.unk08 early-out. That is
 *    not scheduling -- agbcc will not hoist a load above a branch -- so the
 *    two assignments really are the first two statements, unlike the F038
 *    accessors in c_08042E2C.c where the early-out comes first.
 *
 * 3. gUnitTypeData is NOT const, and that is what the second
 *    `ldrb r0,[r6,#0x18]` proves. With the header's old `const` the load is
 *    CSEd across `bl sub_080432E0`, the value rather than the address lives in
 *    r6, and the function is two bytes short. See the note on the declaration
 *    in include/unknown-globals.h.
 *
 * The dead `ldr r7, =gCoDataTable` at the top and the folded pool word
 * `gCoDataTable + 0x5c` (which asm/ symbolises as gUnknown_085D3E2C) both
 * fall straight out of the plain member spelling under -fforce-addr; neither
 * is something to reproduce by hand. */

int sub_08043190(int a, int b, int c)
{
    s16 *p;
    s16 *q;

    p = gCoDataTable[a].unk38[b].unk24[gUnitTypeData[c].unk18];
    q = gCoDataTable[a].unk38[b].unk24[sub_080432E0(c)];

    if (gUnknown_03003FC0.unk08 == 0)
        return 0;

    if (gUnitTypeData[c].unk18 == 0)
        return p[2];

    return p[2] + q[2];
}

/* Family F049: four copies of one body that differ ONLY in the s16 index K
 * (0, 1, 2, 3 -- `movs rN, #0/2/4/6` at the three `ldrsh` sites), exactly as
 * data/families.json `varies` says.
 *
 * Three things had to be read off the ROM rather than guessed:
 *
 * 1. struct CoModeData.unk24 is an array of POINTERS. The ROM does
 *    `ldr r7,[r3]` on the table word and then `ldrsh r0,[r7,r2]` through the
 *    result. 0x24 + 8*4 == 0x44 fills the record exactly, and the two indices
 *    in use (gUnitTypeData[c].unk18, and sub_080432E0's 5/6/7) span it.
 *
 * 2. Both lookups happen BEFORE the gUnknown_03003FC0.unk08 early-out. That is
 *    not scheduling -- agbcc will not hoist a load above a branch -- so the
 *    two assignments really are the first two statements, unlike the F038
 *    accessors in c_08042E2C.c where the early-out comes first.
 *
 * 3. gUnitTypeData is NOT const, and that is what the second
 *    `ldrb r0,[r6,#0x18]` proves. With the header's old `const` the load is
 *    CSEd across `bl sub_080432E0`, the value rather than the address lives in
 *    r6, and the function is two bytes short. See the note on the declaration
 *    in include/unknown-globals.h.
 *
 * The dead `ldr r7, =gCoDataTable` at the top and the folded pool word
 * `gCoDataTable + 0x5c` (which asm/ symbolises as gUnknown_085D3E2C) both
 * fall straight out of the plain member spelling under -fforce-addr; neither
 * is something to reproduce by hand. */

int sub_08043200(int a, int b, int c)
{
    s16 *p;
    s16 *q;

    p = gCoDataTable[a].unk38[b].unk24[gUnitTypeData[c].unk18];
    q = gCoDataTable[a].unk38[b].unk24[sub_080432E0(c)];

    if (gUnknown_03003FC0.unk08 == 0)
        return 0;

    if (gUnitTypeData[c].unk18 == 0)
        return p[3];

    return p[3] + q[3];
}
