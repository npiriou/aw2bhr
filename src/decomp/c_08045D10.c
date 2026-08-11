#include "global.h"

/* Promoted from assembly; each function below is byte-for-byte
 * identical to the original. Order is address order and must
 * stay that way -- the linker places this file's .text as one
 * contiguous block at 0x08045D10.
 * sub_08045D10 @ 0x08045D10
 */

/* "Is army 2 free of any unit whose unk00 is 0x15?"  The same 0x32-slot scan the
 * rest of this block uses, with a constant army index -- `ldrh [.,#4]` is
 * gUnknown_084995FE[2], and the `(u16)` cast is what keeps it a plain `ldrh`
 * rather than `movs r1,#0; ldrsh` (that table is declared `const s16 []` but is
 * really u16; see include/unknown-globals.h).
 *
 * The two tests MUST be separate `if` statements.  Written as
 * `unk00 != 0 && unk00 == 0x15` the C front end's fold_truthop collapses the
 * pair to the single `cmp #0x15`, losing the ROM's `cmp r1,#0; beq` and
 * flipping the loop's register assignment as well.  Two statements are out of
 * fold's reach.
 *
 * Nothing stores in the loop, so the bound and the array deref are hoisted and
 * strength_reduce gives the r0 pointer giv; the ascending `blt` at the bottom is
 * the source's own direction (no dbra reversal, because the giv keeps i live). */
bool8 sub_08045D10(void)
{
    int i;

    for (i = (u16)gUnknown_084995FE[2] + 1; i < (u16)gUnknown_084995FE[2] + 0x33; i++)
    {
        if (gUnitRecords[i].unk00 == 0)
            continue;

        if (gUnitRecords[i].unk00 == 0x15)
            return FALSE;
    }

    return TRUE;
}
