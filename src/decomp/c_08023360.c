#include "global.h"

/* Promoted from assembly; each function below is byte-for-byte
 * identical to the original. Order is address order and must
 * stay that way -- the linker places this file's .text as one
 * contiguous block at 0x08023360.
 * sub_08023360 @ 0x08023360
 */

#include "hardware.h"

/* The `(u16)` cast on sub_080261A0's result is load-bearing and is NOT the
 * same thing as declaring the callee `u16`: agbcc trusts a narrow RETURN
 * TYPE to have been narrowed by the callee and emits nothing, while an
 * explicit cast leaves both operands of the `&` in their own pseudos --
 * `adds r2,r0,#0; ldr r1,=0x3ff; adds r0,r1,#0; ands r2,r0` against the
 * two-instruction `ldr r2,=0x3ff; ands r2,r0`. Four bytes, and it is the
 * whole difference on this function. (s16) is byte-identical here, so the
 * signedness is not settled -- only the presence of the cast is.
 */
void sub_08023360(int a)
{
    sub_08011B18();

    if (a == 1)
        sub_08010FE0();

    if (a == 0)
        sub_08011018();

    sub_08012C58(gUnknown_0849D16C);
    sub_08023860();
    sub_080128D0();

    sub_08011C68(gUnknown_0809175C, (void *)0x06003600, 0xa0);
    sub_08011C68(sub_08026190(), (void *)0x060046A0, ((u16)sub_080261A0() & 0x3ff) * 0x20);
    sub_08011C68(sub_08026198(), (void *)0x06005440, 0x200);

    Decompress(gUnknown_080BD1EC, (void *)0x06008000);

    sub_08011C68(gUnknown_0809175C + 0xa0, (void *)0x0600E780, 0x20);
    sub_08011C68(gUnknown_08499578, (void *)0x06007000, 0x800);
    sub_08011C68(gUnknown_0849957C, (void *)0x0600F000, 0x800);
    sub_08011C68(gUnknown_08499580, (void *)0x06007800, 0x800);

    ApplyPalette((u16 *)(gUnknown_0810E6E0 + (gArmyRecords[1].unk1a - 1) * 0x20), 12);
    ApplyPalette((u16 *)(gUnknown_0810E6E0 + (gArmyRecords[2].unk1a - 1) * 0x20), 13);
    ApplyPalette((u16 *)(gUnknown_0810E6E0 + (gArmyRecords[3].unk1a - 1) * 0x20), 14);
    ApplyPalette((u16 *)(gUnknown_0810E6E0 + (gArmyRecords[4].unk1a - 1) * 0x20), 15);

    sub_0803F80C(8);
    sub_0802D2EC();

    ApplyPalette(gUnknown_0809163C, 18);

    sub_08037150(0x1a6);
    sub_08024268();
    sub_08022A08();

    sub_08011C68(gUnknown_08499584, (void *)0x0600F800, 0x800);

    sub_08035568();
    sub_080116E8();
    sub_080354FC();
    sub_08035020(gUnknown_03003FC0.unk2c);
    sub_08022A34();

    sub_0801A5B0(gCurrentArmyIndex);
    sub_08043834(gCurrentArmyIndex);
    sub_0801A57C(gCurrentArmyIndex);
}
