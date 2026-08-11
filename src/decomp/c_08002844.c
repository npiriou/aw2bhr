#include "global.h"

/* Promoted from assembly; each function below is byte-for-byte
 * identical to the original. Order is address order and must
 * stay that way -- the linker places this file's .text as one
 * contiguous block at 0x08002844.
 * sub_08002844 @ 0x08002844
 */

#include "hardware.h"

/* The sibling of sub_0800272C one table along: loads a unit's OBJ graphics and
 * palette and hands the sprite to sub_0801BD00. a4's low six bits are the unit
 * kind; 0x19 takes a wholly different loader and a different sprite mode.
 *
 * `pal` must be a single-assignment local, NOT a folded 0x6000 in the ternary.
 * agbcc synthesises the constant 0x6000 as `mov #0xC0; lsl #7` (its split takes
 * the SMALLEST shift), but the ROM has `mov r0,#6; lsl r3,r0,#0xc` -- a real
 * ashift whose input is rematerialised into a scratch. That is what a
 * single-def constant local gets: its def is in another basic block so combine
 * never folds the shift, local-alloc gives it reg_equiv_constant instead of a
 * hard register, and reload reloads the 6 into r0 at the use. */
void sub_08002844(int a1, int a2, int a3, int a4, int a5, int a6, int a7)
{
    int cls;
    int attr0, attr1;
    int pal;

    pal = 6;
    cls = (s8)gUnknown_0200B0B0->unk2f;
    if (cls == 0)
        cls = 1;

    a4 &= 0x3F;
    if (a4 != 0x19)
    {
        if (a7)
        {
            sub_08011E54(sub_08026190() + ((sub_080261A4(cls, a4) & 0x3FF) << 5),
                         (void *)(0x06010000 + ((gUnknown_08485D20[a1] & 0x3FF) << 5)),
                         0x80);
        }
        ApplyPalette((u16 *)(gUnknown_0810E6E0 + (gArmyRecords[cls].unk1a - 1) * 0x20), 22);
    }
    else
    {
        sub_0801F19C(0x12, (void *)0x06010000, gUnknown_08485D20[a1]);
        ApplyPalette(gUnknown_081268D8, 30);
    }

    a3 += 8;

    attr1 = a2 & 0x1FF;
    if (a6)
        attr1 |= 0x1000;
    attr0 = ((a3 + 8) & 0xFF) | 0x400;
    if (a5)
        attr0 |= 0x100;
    sub_0801BD00(attr1, attr0, gUnknown_08485D44[a1], a4 != 0x19 ? pal << 12 : 0xE000);
}
