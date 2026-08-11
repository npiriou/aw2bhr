#include "global.h"

/* Promoted from assembly; each function below is byte-for-byte
 * identical to the original. Order is address order and must
 * stay that way -- the linker places this file's .text as one
 * contiguous block at 0x080267AC.
 * sub_080267AC @ 0x080267AC
 */

/* gUnknown_08090A74 and gUnknown_08090A78 are NOT globals: they are
 * `-fforce-addr` .rodata address-constant words holding &gCurrentArmyIndex and
 * &gArmyRecords (dumped from baserom.gba), so both globals are named
 * directly here and agbcc rebuilds the pool. Each is read on both sides of the
 * gUnknown_03004080 test, which is the control-flow merge that forces the
 * words in the first place.
 *
 * The embedded `(n = ...)` is load-bearing on BOTH counts and neither is
 * cosmetic. The ROM holds one loaded halfword in r2 across the first and THIRD
 * store and re-`ldrh`s only for the middle one, so all three spelled as the
 * global is +4 bytes (three loads) and all three spelled as `n` is -2
 * (`subs r0,r2,#1`). And the assignment has to happen INSIDE the first
 * statement rather than in one of its own: a leading `n = gCurrentArmyIndex;`
 * creates the force-addr pseudo before gUnknown_03004084's, which swaps the
 * first two pool words. */
void sub_080267AC(void)
{
    u16 n;

    gUnknown_03004084 = (n = gCurrentArmyIndex) * 0x20;
    gUnknown_03003F2C = (gCurrentArmyIndex - 1) * 0x40;
    gUnknown_03004480 = n;
    gUnknown_030032C0 = 0;
    gArmyRecords[gCurrentArmyIndex].unk16 = 0;
    gArmyRecords[gCurrentArmyIndex].unk24 = 0;
    sub_08020984();
    sub_08026F04();
    sub_0801A548(gCurrentArmyIndex);
    sub_0801A57C(gCurrentArmyIndex);
    sub_08043834(gCurrentArmyIndex);
    if (gUnknown_03004080 == 1)
    {
        if (sub_08026D44(gCurrentArmyIndex))
            sub_08022AAC(gArmyRecords[gCurrentArmyIndex].unk2d & 0x7f,
                         gArmyRecords[gCurrentArmyIndex].unk2e & 0x7f);
        else
            sub_08022AAC(*(s16 *)(gMapData + 4) / 16 + 7,
                         *(s16 *)(gMapData + 6) / 16 + 4);
        gArmyRecords[gCurrentArmyIndex].unk2f = gUnknown_030033E4.unk00;
        gArmyRecords[gCurrentArmyIndex].unk30 = gUnknown_030033E4.unk02;
    }
    sub_08022AAC(gArmyRecords[gCurrentArmyIndex].unk2f,
                 gArmyRecords[gCurrentArmyIndex].unk30);
}
