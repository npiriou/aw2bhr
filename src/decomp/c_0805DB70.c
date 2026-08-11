#include "global.h"

/* Promoted from assembly; each function below is byte-for-byte
 * identical to the original. Order is address order and must
 * stay that way -- the linker places this file's .text as one
 * contiguous block at 0x0805DB70.
 * sub_0805DB70 @ 0x0805DB70
 */

/* sub_0805DB70 @ 0x0805DB70, 308 bytes.
 *
 * gUnknown_0816DA4C is not an object: the ROM word at 0x0816DA4C holds
 * 0x030033EC, agbcc's own -fforce-addr address constant for
 * gCurrentArmyIndex. Naming the global honestly reproduces the double load.
 *
 * `n` is ONE local assigned twice -- the terrain difference and then Div's
 * quotient. That is what the `adds r3, r0, #0` after the `bl Div` records: a
 * pseudo whose live range already covers the whole address chain, so the chain
 * gets r0-r2 instead of r1-r3. Two separate locals coalesce the copy away and
 * shift every register in the chain by one.
 *
 * gCoDataTable[..].unk1c is a function pointer called through
 * `bl _call_via_r1`, and the veneer's register index is what fixes its arity at
 * one argument.
 */
void sub_0805DB70(void)
{
    u8 (*fn)(int);
    int v;
    int n;

    if ((u8)sub_08044280(gCurrentArmyIndex))
    {
        fn = gCoDataTable[gArmyRecords[gCurrentArmyIndex].unk1d].unk1c;
        if (fn != 0 && fn(2) == 1)
        {
            if (gUnknown_03003FC0.unk32 != 0)
                sub_08034534(0x10, 0, gCurrentArmyIndex, 0);
            sub_0804438C(gCurrentArmyIndex, 2);
            return;
        }
    }

    if (sub_0804423C(gCurrentArmyIndex))
    {
        v = sub_080441D4(gCurrentArmyIndex);
        n = sub_08044208(gCurrentArmyIndex) - v;
        n = Div(n * gCoDataTable[gArmyRecords[gCurrentArmyIndex].unk1d].unk17,
                100);
        v += n;
        if (v >= (int)sub_08044094(gCurrentArmyIndex)
            && sub_080129E0() % 100
                   < gCoDataTable[gArmyRecords[gCurrentArmyIndex].unk1d].unk18)
        {
            fn = gCoDataTable[gArmyRecords[gCurrentArmyIndex].unk1d].unk1c;
            if (fn != 0 && fn(1) == 1)
            {
                if (gUnknown_03003FC0.unk32 != 0)
                    sub_08034534(0xf, 0, gCurrentArmyIndex, 0);
                sub_0804438C(gCurrentArmyIndex, 1);
            }
        }
    }
}
