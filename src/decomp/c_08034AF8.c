#include "global.h"

/* Promoted from assembly; each function below is byte-for-byte
 * identical to the original. Order is address order and must
 * stay that way -- the linker places this file's .text as one
 * contiguous block at 0x08034AF8.
 * sub_08034AF8 @ 0x08034AF8
 */

#include "hardware.h"

/* gUnknown_08090E24 is a `-fforce-addr` .rodata word holding &gCurrentArmyIndex
 * (dumped from baserom.gba), not a global -- it is read on both sides of the
 * sub_080348B4 test. gUnknown_08090D90 immediately before it IS a real u16
 * table (0,1,2,3,3,3,2,1,0,0), reached with `lsls #1; adds; ldrh`.
 *
 * gUnknown_03004008 is declared s32 but the division is `__udivsi3`/`__umodsi3`,
 * so the source read it unsigned; the cast is on the read rather than a retype
 * of the global, which a dozen other files share. */
void sub_08034AF8(void)
{
    if (sub_080348B4())
    {
        sub_08034A7C(0x4e, gArmyRecords[sub_08026704(gCurrentArmyIndex)].unk1a);
        switch (gUnknown_02028E40)
        {
        case 0:
            sub_08034A58(0x38, gUnknown_08090DA4);
            sub_08034A58(gUnknown_08090D90[(u32)gUnknown_03004008 / 3 % 10] + 0x68,
                         gUnknown_08090DB0);
            break;
        case 1:
            sub_08034A58(0x38, gUnknown_08090DC0);
            sub_08034A58(gUnknown_08090D90[(u32)gUnknown_03004008 / 3 % 10] + 0x68,
                         gUnknown_08090DD0);
            break;
        case 2:
            sub_08034A58(0x38, gUnknown_08090DE0);
            sub_08034A58(gUnknown_08090D90[(u32)gUnknown_03004008 / 3 % 10] + 0x68,
                         gUnknown_08090DF0);
            break;
        case 3:
            sub_08034A58(0x30, gUnknown_08090E04);
            sub_08034A58(gUnknown_08090D90[(u32)gUnknown_03004008 / 3 % 10] + 0x68,
                         gUnknown_08090E14);
            break;
        }
        if ((gpKeySt->held & 1) == 0)
            return;
    }
    sub_08026768();
    sub_080268F4();
    sub_08044178(gCurrentArmyIndex);
    sub_08024268();
    sub_08062038();
    if (sub_080348B4())
        sub_0802BFBC();
    sub_08034C8C();
    sub_0803B5E8();
    gUnknown_030032D8 = 4;
}
