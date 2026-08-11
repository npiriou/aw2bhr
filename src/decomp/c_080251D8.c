#include "global.h"

/* Promoted from assembly; each function below is byte-for-byte
 * identical to the original. Order is address order and must
 * stay that way -- the linker places this file's .text as one
 * contiguous block at 0x080251D8.
 * sub_080251D8 @ 0x080251D8
 */

void sub_080251D8(int a1)
{
    struct UnitRecord *e;
    int a;
    int b;
    int ok;
    int t;

    a = 0;
    b = 0;
    ok = 0;

    e = &gUnitRecords[a1];

    sub_08024A2C((struct Unk030013D0 *)gUnknown_030013D0, a1);

    if (e->unk04_7 != 0)
    {
        a = sub_08043070(gArmyRecords[gCurrentArmyIndex].unk1d,
                         gArmyRecords[gCurrentArmyIndex].unk1e,
                         e->unk00, 3, 0);
    }
    else
    {
        ok = sub_080433F8(e->unk00, 3, 1);
        if (ok != 0)
            b = sub_08043070(gArmyRecords[gCurrentArmyIndex].unk1d,
                             gArmyRecords[gCurrentArmyIndex].unk1e,
                             e->unk00, 3, 1);
    }

    if (a <= b)
    {
        if (ok != 0)
        {
            ((struct Unk030013D0 *)gUnknown_030013D0)->unk18 = 5;
            ((struct Unk030013D0 *)gUnknown_030013D0)->unk10 = b;
        }
    }
    else
    {
        ((struct Unk030013D0 *)gUnknown_030013D0)->unk18 = 1;
        ((struct Unk030013D0 *)gUnknown_030013D0)->unk10 = a;
        ((struct Unk030013D0 *)gUnknown_030013D0)->unk0a--;
    }

    sub_08024C58((struct Unk030013D0 *)gUnknown_030013D0, 0, 0);

    if (((struct Unk030013D0 *)gUnknown_030013D0)->unk00->unk04_0 != 0)
        t = ((struct Unk030013D0 *)gUnknown_030013D0)->unk14
          * (Div(((struct Unk030013D0 *)gUnknown_030013D0)->unk00->unk04_0 - 1, 10) + 1);
    else
        t = 0;

    ((struct Unk030013D0 *)gUnknown_030013D0)->unk14 =
        ((struct Unk030013D0 *)gUnknown_030013D0)->unk0c = Div(t, 10);
}
