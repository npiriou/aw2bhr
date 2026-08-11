#include "global.h"

/* Promoted from assembly; each function below is byte-for-byte
 * identical to the original. Order is address order and must
 * stay that way -- the linker places this file's .text as one
 * contiguous block at 0x08026CD0.
 * sub_08026CD0 @ 0x08026CD0
 */

/* Applies the chapter record's five preset flag pairs to the five armies: for
 * each slot whose unk00 is not 0xff, OR unk00 into that army's unk2d and unk01
 * into its unk2e.
 *
 * The two subscripts have DIFFERENT origins and that is the whole difficulty of
 * this function -- it sat parked size-exact from wave 41. The army offset giv
 * starts at 0 with `+0x2d`, so its index is 0-based; the slot giv starts at -4
 * with `+0x48`, so its index is -1-based off a member at 0x48. Both must be
 * bound to their own locals: written as `[i - 1]` inline, fold reassociates the
 * -1 into the member constant and gcc emits the giv at its natural origin
 * instead (see include/unknown-globals.h at unk48 for the full measurement).
 *
 * The `movs r5,#4` counting down under `subs; cmp #0; bge` is check_dbra_loop
 * on this ascending loop, not a descending source loop -- it only fires because
 * both subscripts became givs and left the biv dead. */
void sub_08026CD0(void)
{
    int i;
    int j;
    int k;

    for (i = 1; i <= 5; i++)
    {
        j = i - 1;
        k = i - 2;
        if (gUnknown_085C77A0[gUnknown_03003FC0.unk02].unk48[k].unk00 != 0xff)
        {
            gArmyRecords[j].unk2d |=
                gUnknown_085C77A0[gUnknown_03003FC0.unk02].unk48[k].unk00;
            gArmyRecords[j].unk2e |=
                gUnknown_085C77A0[gUnknown_03003FC0.unk02].unk48[k].unk01;
        }
    }
}
