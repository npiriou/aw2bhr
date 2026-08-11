#include "global.h"

/* Promoted from assembly; each function below is byte-for-byte
 * identical to the original. Order is address order and must
 * stay that way -- the linker places this file's .text as one
 * contiguous block at 0x080348B4.
 * sub_080348B4 @ 0x080348B4
 */

/* MATCHED byte-for-byte, wave 43 (W43-C), first attempt.
 *
 * A predicate over armies 1..4: it finds the first occupied slot
 * (unk1b == 1), remembers that army's unk2a, and returns 1 as soon as a later
 * occupied slot disagrees with it. `v = 0` is in the source -- the
 * `movs r7,#0` is not a dead store agbcc invented, because the read in the
 * `found != 0` arm is reachable from function entry as far as its liveness is
 * concerned.
 *
 * `if (found != 0) { ... } else { ... }` with the CHECK arm written FIRST is
 * what the layout pins: `cmp r5,#0; beq` falls through into the check and
 * jumps forward to the record block, so the record block sits at the higher
 * address. The other spelling emits them the other way round.
 *
 * The three guards are one `&&` chain rather than three early returns; both
 * spellings cross-jump onto the same `movs r0,#0`, so that is a readability
 * choice and not a measured one. `gArmyRecords` is the pointer-to-array
 * global, so a plain `[i]` subscript is what produces the
 * `ldr rN,=sym; ldr rN,[rN]` pair -- see its note in unknown-globals.h. The
 * `(x*16 - x)*4` synthesis is agbcc's 60-byte stride multiply. */

bool8 sub_080348B4(void)
{
    u8 i;
    u8 v;
    int found;

    if (gUnknown_03003FC0.unk32 == 0 && gUnknown_03003FC0.unk0d != 0
        && gUnknown_03003FC0.unk01 == 3)
    {
        v = 0;
        found = 0;

        for (i = 1; i <= 4; i++)
        {
            if (sub_080266DC(i))
            {
                if (found != 0)
                {
                    if (gArmyRecords[i].unk1b == 1
                        && v != gArmyRecords[i].unk2a)
                        return 1;
                }
                else if (gArmyRecords[i].unk1b == 1)
                {
                    v = gArmyRecords[i].unk2a;
                    found = 1;
                }
            }
        }
    }

    return 0;
}
