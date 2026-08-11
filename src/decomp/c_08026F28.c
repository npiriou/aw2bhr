#include "global.h"

/* Promoted from assembly; each function below is byte-for-byte
 * identical to the original. Order is address order and must
 * stay that way -- the linker places this file's .text as one
 * contiguous block at 0x08026F28.
 * sub_08026F28 @ 0x08026F28
 */

/* The direct-index sibling of sub_08026F9C: same unk2a comparison, but the
 * indices arrive already resolved rather than as `(a >> 6) + 1`. */
bool8 sub_08026F28(u16 a, u16 b)
{
    if (gArmyRecords[a].unk2a == gArmyRecords[b].unk2a)
        return TRUE;

    return FALSE;
}
