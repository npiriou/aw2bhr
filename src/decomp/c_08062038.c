#include "global.h"

/* Promoted from assembly; each function below is byte-for-byte
 * identical to the original. Order is address order and must
 * stay that way -- the linker places this file's .text as one
 * contiguous block at 0x08062038.
 * sub_08062038 @ 0x08062038
 */

void sub_08062038(void)
{
    u16 saved;
    u16 *p;
    int i;

    i = gCurrentArmyIndex;
    saved = i;
    if (gUnknown_030046B8 & 2)
    {
        for (i = 1; i <= 4; i++)
        {
            p = &gUnknown_03004480;
            gCurrentArmyIndex = i;
            *p = i;
            if (sub_080266DC(i))
                sub_0806209C();
        }
        gUnknown_03004480 = gCurrentArmyIndex = saved;
    }
}
