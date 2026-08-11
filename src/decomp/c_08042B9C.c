#include "global.h"

/* Promoted from assembly; each function below is byte-for-byte
 * identical to the original. Order is address order and must
 * stay that way -- the linker places this file's .text as one
 * contiguous block at 0x08042B9C.
 * sub_08042B9C @ 0x08042B9C
 */

/* gUnknown_030044B0 is declared `u8 []`, so the word store at +8 needs a cast.
 * The BASE must be bound to a local first: writing
 * `*(u32 *)(gUnknown_030044B0 + 8)` folds the 8 into the pool word as
 * `gUnknown_030044B0+0x8` and stores at `[r2]`, where the ROM has a clean
 * `gUnknown_030044B0` pool word and `str r0, [r2, #8]`. Same instruction
 * count -- the difference is entirely in the relocation addend.
 *
 * The element address is recomputed for the second store because the first
 * `strb` kills gArmyRecords's MEM (it is a non-const pointer global). */
void sub_08042B9C(void)
{
    u8 *q;

    gUnknown_03003FC0.unk2e = sub_08035170();
    q = gUnknown_030044B0;
    *(u32 *)(q + 8) = gUnknown_03001FD4;

    if (gUnknown_03003FC0.unk32 == 0)
    {
        gArmyRecords[gCurrentArmyIndex].unk2f = gUnknown_030033E4.unk00;
        gArmyRecords[gCurrentArmyIndex].unk30 = gUnknown_030033E4.unk02;
    }

    gUnknown_030032D8 = 1;
    sub_08025EA0();
}
