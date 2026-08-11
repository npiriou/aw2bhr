#include "global.h"

/* Promoted from assembly; each function below is byte-for-byte
 * identical to the original. Order is address order and must
 * stay that way -- the linker places this file's .text as one
 * contiguous block at 0x0802CE04.
 * sub_0802CE04 @ 0x0802CE04
 */

void sub_0802CE04(void)
{
    gUnknown_03003FC0.unk0c = 1 - gUnknown_03003FC0.unk0c;

    switch (gUnknown_03003FC0.unk0c)
    {
    case 0:
        sub_0803B5E8();
        break;

    case 1:
        sub_08043DAC(gCurrentArmyIndex);
        break;
    }

    sub_08019E68();
    gUnknown_0200C420.unk14 = (gUnknown_03003FC0.unk0c == 0);
}
