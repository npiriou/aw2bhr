#include "global.h"

/* Promoted from assembly; each function below is byte-for-byte
 * identical to the original. Order is address order and must
 * stay that way -- the linker places this file's .text as one
 * contiguous block at 0x0802CEB0.
 * sub_0802CEB0 @ 0x0802CEB0, sub_0802CEFC @ 0x0802CEFC
 */

void sub_0802CEB0(void)
{
    u8 *p;

    sub_0801A168();

    p = gUnknown_030044B0;
    *(u32 *)(p + 8) = gUnknown_03001FD4;

    if (gUnknown_03003FC0.unk32 != 0)
        sub_08034534(0xf, 0, gCurrentArmyIndex, 0);

    sub_0804438C(gCurrentArmyIndex, 1);
    sub_08024268();
}

void sub_0802CEFC(void)
{
    u8 *p;

    sub_0801A168();

    p = gUnknown_030044B0;
    *(u32 *)(p + 8) = gUnknown_03001FD4;

    if (gUnknown_03003FC0.unk32 != 0)
        sub_08034534(0x10, 0, gCurrentArmyIndex, 0);

    sub_0804438C(gCurrentArmyIndex, 2);
    sub_08024268();
}
