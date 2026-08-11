#include "global.h"

/* Promoted from assembly; each function below is byte-for-byte
 * identical to the original. Order is address order and must
 * stay that way -- the linker places this file's .text as one
 * contiguous block at 0x08039F18.
 * sub_08039F18 @ 0x08039F18
 */

/* One nested expression, which is what loads gUnknown_08610A38's address first
 * into the callee-saved r4 and keeps it live to the end. The `adds r3, #0x38`
 * on the base rather than a folded displacement is the `g[i].member` hoist for
 * a word access on an array global.
 *
 * The result is a NUL-terminated string: the only caller, sub_080396F4, hands
 * it straight to sub_08039544, which copies bytes until the first zero. */
u8 *sub_08039F18(int a)
{
    return gUnknown_08610A38[gCoDataTable[gArmyRecords[a].unk1d]
                                 .unk38[gArmyRecords[a].unk1e]
                                 .unk00];
}
