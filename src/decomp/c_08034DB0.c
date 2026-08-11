#include "global.h"

/* Promoted from assembly; each function below is byte-for-byte
 * identical to the original. Order is address order and must
 * stay that way -- the linker places this file's .text as one
 * contiguous block at 0x08034DB0.
 * sub_08034DB0 @ 0x08034DB0, sub_08034DCC @ 0x08034DCC
 */

void sub_08034DB0(void)
{
    if (sub_0803B628() == 0)
        gUnknown_030032D8 = 5;
}

/* gCurrentArmyIndex is a u16 read here with a bare `ldrb` -- the truncation
 * folded into the load that sub_08043DAC's u8 parameter forces. */
void sub_08034DCC(void)
{
    if (gUnknown_03004080 != 1)
        sub_08043DAC(gCurrentArmyIndex);

    sub_08074460();
    gUnknown_030032D8 = 6;
}
