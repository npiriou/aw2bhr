#include "global.h"

/* Promoted from assembly; each function below is byte-for-byte
 * identical to the original. Order is address order and must
 * stay that way -- the linker places this file's .text as one
 * contiguous block at 0x08058224.
 * sub_08058224 @ 0x08058224
 */

/* +0x06 is a 7-BIT BITFIELD, not a byte masked at the use: `ldrb; lsls #0x19;
 * lsrs #0x19` is extract_bit_field's shape for bitsize 7 at bit 0, and the
 * shifted intermediate is CSEd into the else arm's `lsrs r0, r2, #0x19`.
 * sub_08042D1C is called twice on purpose -- the ROM recomputes it rather than
 * keeping it live across the compare. */
/* Wave 32 (W32-B) UNIFIES the local struct with struct UnitRecord: the two
 * members this body reads -- unk00 at +0x00 and the 7-bit bitfield at +0x06 --
 * are that type's unk00 and unk06_0 at the identical offsets and widths, and
 * sub_08062730 (promoted this wave) passes gUnitRecords entries straight
 * in. Byte-neutral; re-verified. The local model is removed rather than kept
 * so the shared type is the only one a future caller can see. */
int sub_08058224(struct UnitRecord *p)
{
    if (sub_08042D1C(gCurrentArmyIndex, p->unk00) > p->unk06_0)
        return p->unk06_0;
    return sub_08042D1C(gCurrentArmyIndex, p->unk00);
}
