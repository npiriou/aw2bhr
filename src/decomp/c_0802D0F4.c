#include "global.h"

/* Promoted from assembly; each function below is byte-for-byte
 * identical to the original. Order is address order and must
 * stay that way -- the linker places this file's .text as one
 * contiguous block at 0x0802D0F4.
 * sub_0802D0F4 @ 0x0802D0F4
 */

/* The same frame-entry shape as c_0802CFFC.c -- open with sub_08034F48 and
 * sub_0801A168, close with the sub_0802C57C / sub_080424FC / sub_0802C594
 * bracket -- with a different middle and one extra call (sub_08042998) in the
 * tail.
 *
 * The middle is the block unknown-globals.h already records for sub_08042998
 * on gUnknown_03003FC0.unk0d: the zero loaded for the `unk0d == 0` test is
 * REUSED to clear gUnknown_03004074, which is why the store is `strb r5, [r4]`
 * with r5 the byte just compared. `gUnknown_03004074 = 0;` is the source --
 * agbcc's cse records the equivalence from the conditional jump, so no
 * `movs rN, #0` is emitted.
 *
 * unk32 is tested TWICE and the tests are NESTED, not sequential: the first
 * `beq` jumps past the second test to the tail, which an `if (unk32 && ...)`
 * pair could not produce. sub_08025B80 wants a `struct UnitRecord *` while
 * gUnknown_030040D8 is declared `struct Unk030040D8 *`; the cast is
 * byte-neutral and the two struct names are already flagged in
 * unknown-globals.h as the same object (gUnknown_08091364 is
 * `struct UnitRecord **` holding &gUnknown_030040D8).
 *
 * MATCHED first draft. */
void sub_0802D0F4(void)
{
    sub_08034F48();
    sub_0801A168();

    if (gUnknown_03003FC0.unk32 != 0)
    {
        if (gUnknown_03003FC0.unk0d == 0)
        {
            sub_08025B80((struct UnitRecord *)gUnknown_030040D8, gUnknown_03004074);
            gUnknown_03004074 = 0;
        }

        if (gUnknown_03003FC0.unk32 != 0)
            sub_08034534(0xa, gUnknown_03003F38, 0, 0);
    }

    sub_0802C57C();
    sub_08042998();
    sub_080424FC();
    sub_0802C594();
}
