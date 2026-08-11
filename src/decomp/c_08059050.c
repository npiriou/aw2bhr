#include "global.h"

/* Promoted from assembly; each function below is byte-for-byte
 * identical to the original. Order is address order and must
 * stay that way -- the linker places this file's .text as one
 * contiguous block at 0x08059050.
 * sub_08059050 @ 0x08059050
 */

void sub_08059050(int a1, s16 *best, void *out)
{
    u8 army;
    s8 v;

    army = a1;

    if (sub_080266DC(army) && sub_08026D44(army))
    {
        if ((s8)gUnknown_03003340[gArmyRecords[army].unk2e & 0x7f]
                                 [gArmyRecords[army].unk2d & 0x7f] >= 0)
        {
            v = gUnknown_03003340[gArmyRecords[army].unk2e & 0x7f]
                                 [gArmyRecords[army].unk2d & 0x7f];

            if (v < *best)
            {
                struct ArmyRecord *base;

                *best = v;
                base = gArmyRecords;
                ((u16 *)out)[0] = base[army].unk2d & 0x7f;
                ((u16 *)out)[1] = base[army].unk2e & 0x7f;
            }
        }
    }
}
