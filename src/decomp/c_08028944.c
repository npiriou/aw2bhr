#include "global.h"

/* Promoted from assembly; each function below is byte-for-byte
 * identical to the original. Order is address order and must
 * stay that way -- the linker places this file's .text as one
 * contiguous block at 0x08028944.
 * sub_08028944 @ 0x08028944
 */

/* "Is army `a` still playable?" -- only asked at all when gUnknown_03003FC0's
 * +0x04 flag byte has bit 1 set, and then it is `sub_080266DC(a) && !unk32`.
 *
 * The RETURN-BLOCK ORDER is a readout: the ROM's `movs r0,#1; b` sits inline
 * where the tests fall through and the `movs r0,#0` is at the very end, which
 * is the early-`return TRUE` spelling. Nesting the two inner tests inside the
 * flag test and returning TRUE at the bottom emits them the other way round --
 * same size, different bytes. */
bool8 sub_08028944(u16 a)
{
    if ((gUnknown_03003FC0.unk04 & 2) == 0)
        return TRUE;

    if (sub_080266DC(a) == 0)
        return FALSE;

    if (gArmyRecords[a].unk32 != 0)
        return FALSE;

    return TRUE;
}
