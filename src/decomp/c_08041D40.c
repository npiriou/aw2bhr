#include "global.h"

/* Promoted from assembly; each function below is byte-for-byte
 * identical to the original. Order is address order and must
 * stay that way -- the linker places this file's .text as one
 * contiguous block at 0x08041D40.
 * sub_08041D40 @ 0x08041D40
 */

/* Orders two units for a queue: by the cell column unk02 first, then by the
 * low seven bits of their army's gArmyRecords record.
 *
 * `(p - gUnitRecords) >> 6` is the matched sub_0804203C's army idiom --
 * exact division by the 0x0c stride (`mul 0x55555555; neg; asr #2`) with the
 * `>> 6` merged into the single `asr #8`. The `+ 1` then indexes
 * gArmyRecords ONE-BASED, which is what puts the member at a runtime
 * 0x3c + 0x2d == 0x69: exactly the reach struct ArmyRecord's own unk2a note
 * records for sub_08026F9C/sub_08026FD0, which read element [n + 1] the same
 * way.
 *
 * The last test is written `<= ... return 0` and NOT `> ... return 1`. Both are
 * the same predicate, but only this polarity leaves `return 1` inline ahead of
 * the literal pool with `return 0` after it; the other spelling swaps the two
 * blocks. Same lever c_0804203C.c documents for its own pair of returns. */
int sub_08041D40(struct UnitRecord *a, struct UnitRecord *b)
{
    if (a->unk02 > b->unk02)
        return 1;

    if (a->unk02 != b->unk02)
        return 0;

    if ((gArmyRecords[((a - gUnitRecords) >> 6) + 1].unk2d & 0x7f)
     <= (gArmyRecords[((b - gUnitRecords) >> 6) + 1].unk2d & 0x7f))
        return 0;

    return 1;
}
