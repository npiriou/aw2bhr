#include "global.h"

/* Promoted from assembly; each function below is byte-for-byte
 * identical to the original. Order is address order and must
 * stay that way -- the linker places this file's .text as one
 * contiguous block at 0x080270B8.
 * sub_080270B8 @ 0x080270B8, sub_080270F0 @ 0x080270F0, sub_08027118 @ 0x08027118
 */

bool8 sub_080270B8(void)
{
    int i;

    for (i = 1; i <= 4; i++)
    {
        if (gArmyRecords[i].unk1b != 0 && (gArmyRecords[i].unk13 & 0x20))
            return TRUE;
    }

    return FALSE;
}

u8 sub_080270F0(void)
{
    int i;

    for (i = 1; i <= 4; i++)
    {
        if (gArmyRecords[i].unk1b == 1)
            return i;
    }

    return 0;
}

void sub_08027118(void)
{
    int i;
    int j;

    for (i = 1; i <= 4; i++)
    {
        if (sub_080266DC(i))
        {
            for (j = 1; j <= 4; j++)
            {
                if (j != i && gArmyRecords[j].unk1b != 0
                    && gArmyRecords[j].unk2a == gArmyRecords[i].unk2a)
                    gArmyRecords[j].unk14 = 0;
            }
        }
    }
}
