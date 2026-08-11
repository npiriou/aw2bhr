#include "global.h"

/* Promoted from assembly; each function below is byte-for-byte
 * identical to the original. Order is address order and must
 * stay that way -- the linker places this file's .text as one
 * contiguous block at 0x08024830.
 * sub_08024830 @ 0x08024830
 */

#include "hardware.h"

/* The palette half of sub_08023360 (src/decomp/c_08023360.c) lifted out as its
 * own routine: the same four `gUnknown_0810E6E0 + (gArmyRecords[i].unk1a
 * - 1) * 0x20` palette rows, on ApplyPaletteExt with a BYTE offset where the
 * exemplar uses ApplyPalette with a slot index -- 0x180/0x1A0/0x1C0/0x1E0 are
 * the exemplar's 12/13/14/15 times 0x20, and 0x240 is its 18.
 *
 * A data_refs-subset target (42% of sub_08023360) and it behaved exactly as
 * that axis claims: all five globals and all eight callees already declared,
 * first draft, one attempt, nothing derived but statement order. The fourth
 * ApplyPaletteExt reuses the struct offset 0x10A as the base of its palette
 * constant (`movs r1,#0x85; lsls r1,#1` ... `adds r1,#0xd6` = 0x1E0) -- that is
 * gcc CSEing two unrelated integer constants, not a source feature. */

void sub_08024830(void)
{
    ApplyPaletteExt((u16 *)(gUnknown_0810E6E0 + (gArmyRecords[1].unk1a - 1) * 0x20),
                    0x180, 0x20);
    ApplyPaletteExt((u16 *)(gUnknown_0810E6E0 + (gArmyRecords[2].unk1a - 1) * 0x20),
                    0x1A0, 0x20);
    ApplyPaletteExt((u16 *)(gUnknown_0810E6E0 + (gArmyRecords[3].unk1a - 1) * 0x20),
                    0x1C0, 0x20);
    ApplyPaletteExt((u16 *)(gUnknown_0810E6E0 + (gArmyRecords[4].unk1a - 1) * 0x20),
                    0x1E0, 0x20);

    sub_0803F80C(8);
    sub_0802D2EC();

    ApplyPaletteExt(gUnknown_0809163C, 0x240, 0x20);

    sub_08035020(gUnknown_03003FC0.unk2c);
    sub_08022A34();

    sub_0801A5B0(gCurrentArmyIndex);
    sub_08043834(gCurrentArmyIndex);
    sub_0801A57C(gCurrentArmyIndex);
}
