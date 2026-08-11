#include "global.h"

/* Promoted from assembly; each function below is byte-for-byte
 * identical to the original. Order is address order and must
 * stay that way -- the linker places this file's .text as one
 * contiguous block at 0x08044B28.
 * sub_08044B28 @ 0x08044B28
 */

#include "proc.h"

/* Sets the army's step index and then runs the two callbacks that step names:
 * one out of gCoDataTable[unk1d].unk38[unk1f] and one out of
 * gUnknown_084A0090[unk1d].unk1c[unk1f - 1]. Both are `bl _call_via_r1`, i.e.
 * one-argument indirect calls -- the first gets this function's own ProcPtr,
 * the second a literal 0.
 *
 * The SECOND index comes from gCurrentArmyIndex, not from a1, so the two
 * lookups are on different armies and the store between them is why unk1f is
 * re-read rather than reused. */
void sub_08044B28(int a1, int a2, ProcPtr a3)
{
    gArmyRecords[a1].unk1f = a2;

    gCoDataTable[gArmyRecords[a1].unk1d]
        .unk38[gArmyRecords[a1].unk1f].unk04(a3);

    gUnknown_084A0090[gArmyRecords[gCurrentArmyIndex].unk1d]
        .unk1c[gArmyRecords[gCurrentArmyIndex].unk1f - 1].unk0c(0);
}
