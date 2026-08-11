#include "global.h"

/* Promoted from assembly; each function below is byte-for-byte
 * identical to the original. Order is address order and must
 * stay that way -- the linker places this file's .text as one
 * contiguous block at 0x0803861C.
 * sub_0803861C @ 0x0803861C
 */

/* MATCHED byte-for-byte (wave 43, W43-E). relocs: match.
 *
 * `r1 + 0x66` is not a member of the outer object: gArmyRecords's record
 * stride is 0x3c and 0x3c + 0x2a = 0x66, so it is gArmyRecords[1].unk2a,
 * the same member the loop reads at index i. The function counts the armies
 * whose unk2a agrees with army 1's.
 *
 * THE TAIL'S SPELLING IS THE WHOLE DIFFICULTY, and it cost one attempt (96.2%,
 * 3 bytes, size already exact). agbcc lays out a `return 0` / `return 1` pair
 * by putting the FALL-THROUGH return inline and the written EARLY return
 * out-of-line past the literal pool, inverting the branch you wrote. So
 *
 *     if (count == 0) return 0; return 1;     ->  beq <ret 0>; movs r0,#1
 *     if (count != 0) return 1; return 0;     ->  bne <ret 1>; movs r0,#0
 *
 * and it is the SECOND that produces the ROM's `bne _08038664` over an inline
 * `movs r0,#0`. Read the ROM backwards: whichever constant is INLINE is the
 * one the source falls through to. Verified by compiling both spellings side
 * by side; see docs/agbcc-codegen.md.
 */

bool8 sub_0803861C(void)
{
    u8 count;
    u8 i;

    count = 0;
    for (i = 1; i <= 4; i++)
    {
        if (sub_080266DC(i) && gArmyRecords[1].unk2a == gArmyRecords[i].unk2a)
            count++;
    }
    if (count != 0)
        return 1;
    return 0;
}
