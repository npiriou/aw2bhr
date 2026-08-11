#include "global.h"

/* Promoted from assembly; each function below is byte-for-byte
 * identical to the original. Order is address order and must
 * stay that way -- the linker places this file's .text as one
 * contiguous block at 0x0802A6B0.
 * sub_0802A6B0 @ 0x0802A6B0
 */

#include "proc.h"

/* struct Unk03001470's unk38 is declared u16 in the shared header; every read
 * here is `ldrsh`, which only a SIGNED halfword produces, so the member is
 * very likely s16. The header is not changed for that -- other promoted
 * functions read the same member -- and the explicit `(s16)` cast below is
 * byte-exact: sign_extend(subreg(zero_extend(mem:HI))) folds straight back to
 * `ldrsh`. The `++` keeps the plain `ldrh` because only the store consumes it.
 *
 * gUnknown_03001470 and gUnknown_03001FBC are named honestly; both pick up
 * agbcc -fforce-addr `.rodata` words (what the ROM prints as
 * gUnknown_08090B88 / gUnknown_08090B8C) from the reference count alone.
 *
 * unk04_0 is the 7-bit bitfield: the truth test folds to `movs #0x7f; ands`
 * while the VALUE use expands to the `lsls #0x19; lsrs #0x19` pair, which is
 * the pair of spellings only a bitfield gives. */

void sub_0802A6B0(void)
{
    struct UnitRecord *p;

    if (Proc_Find(gUnknown_0849FB44))
        return;

    while ((s16)gUnknown_03001470[gUnknown_03001FBC].unk38 <= 0x32)
    {
        p = &gUnitRecords[(s16)gUnknown_03001470[gUnknown_03001FBC].unk38
                               + gUnknown_03003F2C];

        if (p->unk00 != 0 && sub_080253B0(p))
        {
            sub_08025B24(p, p->unk04_0 ? Div(p->unk04_0 - 1, 10) + 1 : 0);
            sub_0804018C(p);
            return;
        }

        gUnknown_03001470[gUnknown_03001FBC].unk38++;
    }

    if ((s16)gUnknown_03001470[gUnknown_03001FBC].unk38 > 0x32)
        sub_08015C30(gUnknown_03001FBC);
}
