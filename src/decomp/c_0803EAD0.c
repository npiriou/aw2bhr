#include "global.h"

/* Promoted from assembly; each function below is byte-for-byte
 * identical to the original. Order is address order and must
 * stay that way -- the linker places this file's .text as one
 * contiguous block at 0x0803EAD0.
 * sub_0803EAD0 @ 0x0803EAD0, sub_0803EB40 @ 0x0803EB40
 */

void sub_0803EAD0(void)
{
    struct Unk02028360 *p;

    for (p = gUnknown_02028360; p->unk02_6 != 0; p++)
    {
        switch (p->unk02_6)
        {
        case 2:
            if (gCurrentArmyIndex == 1)
                p->unk06--;
            break;
        case 1:
        case 3:
        case 4:
        case 5:
            if (gArmyRecords[gCurrentArmyIndex].unk1a == 5)
                p->unk06--;
            break;
        }
    }
}

void sub_0803EB40(void)
{
    struct Unk02028360 *p;

    for (p = gUnknown_02028360; p->unk02_6 != 0; p++)
    {
        switch (p->unk02_6)
        {
        case 1:
        case 2:
        case 3:
        case 4:
        case 5:
            if (p->unk06 == 0)
                p->unk06 = p->unk02_a;
            break;
        }
    }
}
