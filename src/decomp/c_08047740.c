#include "global.h"

/* Promoted from assembly; each function below is byte-for-byte
 * identical to the original. Order is address order and must
 * stay that way -- the linker places this file's .text as one
 * contiguous block at 0x08047740.
 * sub_08047740 @ 0x08047740
 */

/* Counts the loaded-cargo links that a slot swap would break, weighted 1 for
 * the direct case and 2 for the transitive one.
 *
 * Same vocabulary as sub_0804769C (gUnitRecords is a POINTER, stride 12;
 * gUnknown_03003F2C is the army base for both the subscript and the two
 * `unk07 - base` / `unk08 - base` subtractions; the bound p->unk21 is an
 * unsigned u8), and the same ONE COUNTER, ONE RETURN POINT shape.
 *
 * The a3 == 1 arm ends the function the moment its guard fires -- every path
 * out of the inner scan branches to the epilogue -- so its outer loop breaks.
 * The other arm's inner scan falls back into the outer i++ instead, so there
 * the break is a plain one. That asymmetry is the whole difference between the
 * two halves apart from the += 1 / += 2.
 *
 * a2 is REASSIGNED in the second arm (`adds r6, r4, #0` overwrites the
 * parameter register with gUnknown_02028DD8[i]); it is not a fresh local, which
 * is why the outer test that follows compares against the unnarrowed
 * `unk07 - base` while the inner one binds u16 temporaries.
 *
 * The word at 0x0812A128 holds 0x02028DD8: it is an agbcc -fforce-addr address
 * constant for gUnknown_02028DD8, not an object, so it is spelled honestly and
 * not declared. */

u16 sub_08047740(struct Unk0804769C *p, u16 a2, u16 a3, u16 a4)
{
    u16 count;
    u16 i;
    u16 u;
    u16 v;
    struct UnitRecord *r;

    count = 0;

    if (a3 == 1)
    {
        for (i = 0; i < p->unk21; i++)
        {
            r = &gUnitRecords[gUnknown_03003F2C + gUnknown_02028DD8[i]];
            if (r->unk01 & 0x10)
            {
                u = r->unk07 - gUnknown_03003F2C;
                v = r->unk08 - gUnknown_03003F2C;
                if (a2 == u || a2 == v)
                {
                    for (i = a4 + 1; i < p->unk21; i++)
                    {
                        if (gUnknown_02028DD8[i] == u
                         || gUnknown_02028DD8[i] == v)
                        {
                            count = count + 1;
                            break;
                        }
                    }
                    break;
                }
            }
        }
    }
    else
    {
        for (i = 0; i < p->unk21; i++)
        {
            r = &gUnitRecords[gUnknown_03003F2C + gUnknown_02028DD8[i]];
            if (r->unk01 & 0x10)
            {
                if (a2 == r->unk07 - gUnknown_03003F2C
                 || a2 == r->unk08 - gUnknown_03003F2C)
                {
                    a2 = gUnknown_02028DD8[i];
                    for (i = 0; i < p->unk21; i++)
                    {
                        r = &gUnitRecords[gUnknown_03003F2C
                                               + gUnknown_02028DD8[i]];
                        if (r->unk01 & 0x10)
                        {
                            u = r->unk07 - gUnknown_03003F2C;
                            v = r->unk08 - gUnknown_03003F2C;
                            if (a2 == u || a2 == v)
                            {
                                for (i = a4 + 1; i < p->unk21; i++)
                                {
                                    if (gUnknown_02028DD8[i] == u
                                     || gUnknown_02028DD8[i] == v)
                                    {
                                        count = count + 2;
                                        break;
                                    }
                                }
                                break;
                            }
                        }
                    }
                }
            }
        }
    }

    return count;
}
