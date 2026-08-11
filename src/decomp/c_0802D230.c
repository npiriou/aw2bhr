#include "global.h"

/* Promoted from assembly; each function below is byte-for-byte
 * identical to the original. Order is address order and must
 * stay that way -- the linker places this file's .text as one
 * contiguous block at 0x0802D230.
 * sub_0802D230 @ 0x0802D230, sub_0802D2A0 @ 0x0802D2A0
 */

#include "proc.h"

void sub_0802D230(void)
{
    int v;

    sub_08034F48();
    sub_0801A168();

    if (((struct UnitRecord *)gUnknown_030040D8)->unk04_0 != 0)
        v = (Div(((struct UnitRecord *)gUnknown_030040D8)->unk04_0 - 1, 10) + 1)
            * gUnitTypeData[gUnknown_030040D8->unk00].unk06 / 2;
    else
        v = 0;

    sub_08025B58(gCurrentArmyIndex, v);
    sub_0804256C();
    sub_08025D60(gUnknown_03003F38);
    sub_080258CC();
}

void sub_0802D2A0(int a1, int a2, u8 a3)
{
    if (!(a3 & 2))
    {
        sub_0801F838(0xff);
        sub_08020D50(gUnknown_03003100.spos.unk00, gUnknown_03003100.spos.unk02,
            (struct UnitRecord *)gUnknown_030040D8);
        Proc_Start(gUnknown_0849A9F8, PROC_TREE_3);
        sub_0801A168();
        sub_08024500();
        sub_08034F7C();
    }
}
