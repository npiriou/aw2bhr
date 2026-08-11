#include "global.h"

/* Promoted from assembly; each function below is byte-for-byte
 * identical to the original. Order is address order and must
 * stay that way -- the linker places this file's .text as one
 * contiguous block at 0x08025BB4.
 * sub_08025BB4 @ 0x08025BB4
 */

/* This function HAS a parameter, and that is refutable rather than merely
 * unproved: it opens with a bare `bl sub_08035740`, and sub_08035740 --
 * already matched in src/decomp/c_08035740.c as `void sub_08035740(void *a)` --
 * reads r0. Any literal argument would cost a `movs r0,#N` the ROM does not
 * have, so the value has to be arriving in r0 already.
 *
 * gUnknown_030040D8 is handed straight to sub_08025B80's
 * `struct UnitRecord *` with no arithmetic. That is a SECOND, independent
 * confirmation of the note on struct Unk030040D8.unk01 in unknown-globals.h --
 * the pointed-to object really is a struct UnitRecord. The cast is here rather
 * than in the header because merging the two struct names would change
 * c_080424BC.c's `->unk05 &= 7` into the bitfield spelling. */

void sub_08025BB4(void *a1)
{
    sub_08035740(a1);

    if (gUnknown_03003FC0.unk0d != 0)
        sub_08025B80((struct UnitRecord *)gUnknown_030040D8, gUnknown_03004074);
}
