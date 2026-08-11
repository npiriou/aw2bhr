#include "global.h"

/* Promoted from assembly; each function below is byte-for-byte
 * identical to the original. Order is address order and must
 * stay that way -- the linker places this file's .text as one
 * contiguous block at 0x08026924.
 * sub_08026924 @ 0x08026924
 */

/* Chapter setup: clears the unused army slots' gUnknown_03003FF8 entries, then
 * copies the four per-slot arrays out of gUnknown_03003FC0 into the army
 * records, and finally walks the unit table handing every live unit to
 * sub_08025D20.
 *
 * TWO READ-OUTS THAT ARE NOT FREE:
 *
 * 1. The first loop really is DESCENDING in the source. Its counter runs down
 *    from `4 - sub_080248F8()` to 1 and the index is written `5 - k`, which is
 *    why the constant 5 is a live register across it. Rewriting it as the
 *    equivalent ascending `for (i = n + 1; i <= 4; i++)` does NOT get rewritten
 *    back into this by check_dbra_loop -- it produces an ascending loop with a
 *    direct index and a `cmp #4` bottom test, 30 bytes different. So this is a
 *    case where the down-counter is the source and not the optimiser, which is
 *    the opposite of the usual warning.
 *
 * 2. `j = 5 - k;` MUST be its own statement. Folded into the subscript as
 *    `gUnknown_03003FF8[5 - k]` everything matches except that agbcc hoists the
 *    array's address into the preheader BEFORE the constant 5, swapping the two
 *    registers (`ldr r4, =base; movs r3, #5` against the ROM's `movs r4, #5;
 *    ldr r3, =base`) -- 7 bytes on an otherwise size-exact 97.6% candidate.
 *    Splitting the index out creates the constant's pseudo first and the
 *    allocation order follows. Same lever as the wave-17 allocno rule, reached
 *    from the statement side.
 *
 * `i` is s16 throughout: every loop increment is `lsls #0x10 / lsrs #0x10 /
 * asrs #0x10` with the SIGN-extended copy feeding the compare. The last loop's
 * argument `(i >> 6) + 1` is a real arithmetic shift (`asrs r0, r2, #0x16` on
 * the already-shifted value), not a division -- a signed /64 would carry a
 * rounding correction. */
void sub_08026924(void)
{
    s16 i;
    s16 k;
    int j;

    for (k = 4 - sub_080248F8(); k > 0; k--)
    {
        j = 5 - k;
        gUnknown_03003FF8[j] = 0;
    }

    gArmyRecords[1].unk1b = gUnknown_03003FC0.unk38[1];
    gArmyRecords[2].unk1b = gUnknown_03003FC0.unk38[2];
    gArmyRecords[3].unk1b = gUnknown_03003FC0.unk38[3];
    gArmyRecords[4].unk1b = gUnknown_03003FC0.unk38[4];

    for (i = 1; i <= 4; i++)
    {
        gArmyRecords[i].unk1a = gUnknown_03003FC0.unk33[i];
        gArmyRecords[i].unk1d = gUnknown_03003FC0.unk3d[i];
        gArmyRecords[i].unk2a = gUnknown_03003FC0.unk42[i];
        gArmyRecords[i].unk2c = 0;

        if (gArmyRecords[i].unk1b == 0)
            gArmyRecords[i].unk1d = 1;
    }

    sub_08026A48();

    if (gUnknown_03003FC0.unk08 == 0)
        gUnknown_03003FC0.unk07 = 0;

    sub_08026B28();

    for (i = 1; i <= 0xff; i++)
    {
        if (gUnitRecords[i].unk00 != 0)
            sub_08025D20((i >> 6) + 1);
    }
}
