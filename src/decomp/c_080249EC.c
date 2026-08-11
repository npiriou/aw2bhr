#include "global.h"

/* Promoted from assembly; each function below is byte-for-byte
 * identical to the original. Order is address order and must
 * stay that way -- the linker places this file's .text as one
 * contiguous block at 0x080249EC.
 * sub_080249EC @ 0x080249EC
 */

/* THREE parameters and the FIRST is unused: the body opens on r1 and never
 * reads r0, so a1 exists only to put a2 and a3 in r1 and r2.
 *
 * a2 is `s8`, and the two shift pairs are what say so: PROMOTE_MODE narrows it
 * into r3 with `lsls #0x18; lsrs #0x18` (zero-extending, which says only
 * "narrow") and the USE re-reads it as `lsls #0x18; asrs #0x18` -- the second
 * pair is where the sign lives. a3 is the u8 index into gUnitTypeData.
 *
 * `lsls #0x19; asrs #0x18` is a NET LEFT SHIFT OF ONE under an (s8) cast, not a
 * mask: count it as `(s8)(x * 2)` and fold it into the x5 above it, giving
 * `(s8)(unk10 * 10)`. Written as a shift it is a different instruction
 * sequence -- shorten_binary_op applies to MULT_EXPR and not to shifts. */

int sub_080249EC(int a1, s8 a2, u8 a3)
{
    if (gUnitTypeData[a3].unk1a == 0x10)
        return 0;

    return (s8)(gUnknown_085D583C[a2].unk10 * 10);
}
