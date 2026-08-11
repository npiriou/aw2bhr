#include "global.h"

/* Promoted from assembly; each function below is byte-for-byte
 * identical to the original. Order is address order and must
 * stay that way -- the linker places this file's .text as one
 * contiguous block at 0x0805C208.
 * sub_0805C208 @ 0x0805C208
 */

/* Next door to src/decomp/c_0805C1D8.c and guarded the same way: the
 * `cmp #1; bls` on gUnknown_03004770 is the UNSIGNED compare that pins that
 * global u32, which is the evidence its declaration already cites.
 *
 * `unk04_0 <= 0x5a` comes out with `bhi`, an UNSIGNED compare, even though a
 * 7-bit field of a u16 container would promote to int under the usual rules.
 * That is agbcc's behaviour, not a cast in the source: the value is the result
 * of `lsls #0x19; lsrs #0x19`, so the compare is emitted against the
 * zero-extended temporary. Probed -- no cast is needed or wanted.
 *
 * The tail is the 24-byte `if (...) return 1; ... return 0;` split across a `b`
 * with the two `movs` in different blocks, not a returned comparison. */

int sub_0805C208(void)
{
    int count;
    int i;

    count = 0;

    if (gUnknown_03004770 <= 1)
    {
        for (i = gUnknown_03003F2C + 1; i < gUnknown_03003F2C + 0x40; i++)
        {
            if (gUnitRecords[i].unk00 != 0
                && gUnitRecords[i].unk04_0 <= 0x5a)
                count++;
        }

        if (count != 0)
            return 1;
    }

    return 0;
}
