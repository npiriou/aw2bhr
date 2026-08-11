#include "global.h"

/* Promoted from assembly; each function below is byte-for-byte
 * identical to the original. Order is address order and must
 * stay that way -- the linker places this file's .text as one
 * contiguous block at 0x080586CC.
 * sub_080586CC @ 0x080586CC
 */

/* Counts the units in the 0x40-wide window at gUnknown_03003F2C that are alive,
 * belong to class 2 per the gUnknown_0857680F type table, have unk09's low
 * 3-bit field equal to 1, and stand on a map cell whose signed terrain byte is
 * not -1.
 *
 * No store anywhere in the loop, so agbcc hoists the `ldr` of gUnitRecords
 * and the `ldrh` of gUnknown_03003F2C into the preheader and check_dbra_loop
 * turns the exit test into a countdown -- none of which is authored. -1 lands
 * in a register (`movs #1; rsbs #0`) because `cmp` has no negative immediate.
 *
 * `gUnknown_03003340[p->unk03][p->unk02]` is the c_0804151C.c cell idiom: row
 * pointer first (`lsls #2; adds`), column added to the loaded pointer.
 */

int sub_080586CC(void)
{
    struct UnitRecord *p;
    int count;
    int i;

    count = 0;
    for (i = gUnknown_03003F2C; i < gUnknown_03003F2C + 0x40; i++)
    {
        p = &gUnitRecords[i];
        if (p->unk00 != 0 && gUnknown_0857680F[p->unk00] == 2
            && (p->unk09 & 7) == 1
            && (s8)gUnknown_03003340[p->unk03][p->unk02] != -1)
            count++;
    }

    return count;
}
