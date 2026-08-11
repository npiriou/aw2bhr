#include "global.h"

/* Promoted from assembly; each function below is byte-for-byte
 * identical to the original. Order is address order and must
 * stay that way -- the linker places this file's .text as one
 * contiguous block at 0x08024C58.
 * sub_08024C58 @ 0x08024C58
 */

void sub_08024C58(struct Unk030013D0 *a1, int a2, u8 a3)
{
    int idx;
    int x;
    int y;
    int acc;
    int hp;
    int r;
    int q;

    idx = (a1->unk00 - gUnitRecords) >> 6;
    x = *(s16 *)((u8 *)&gArmyRecords[idx] + 0x62);
    y = *(s16 *)((u8 *)&gArmyRecords[idx] + 0x64);
    acc = 100;

    if (a1->unk00->unk04_0 != 0)
        hp = Div(a1->unk00->unk04_0 - 1, 10) + 1;
    else
        hp = 0;

    a1->unk06 = sub_08043304((struct Unk43304 *)a1);
    a1->unk0e = Div(hp * a1->unk06, 10);
    a1->unk0e = a1->unk0e
              + sub_08042CF8(((a1->unk00 - gUnitRecords) >> 6) + 1, a1->unk00->unk00)
              + y;
    a1->unk0c = a1->unk10;

    if (a2 == 1)
        acc += sub_0804338C((struct Unk43304 *)a1);

    acc += sub_0804334C((struct Unk43304 *)a1);
    acc += x;

    a1->unk0c = Div(acc * a1->unk0c, 100);
    a1->unk14 = a1->unk0c;

    if (a1->unk0c != 0)
    {
        if (gUnknown_03003FC0.unk06 == 0 || a3 != 0)
        {
            r = DivRem(sub_080129E0(),
                       sub_08042E64(((a1->unk00 - gUnitRecords) >> 6) + 1));
            q = sub_08042EBC(((a1->unk00 - gUnitRecords) >> 6) + 1);
            if (q != 0)
                q = DivRem(sub_080129E0(), q);

            a1->unk0c = a1->unk0c + r - q;
            if (a1->unk0c < 0)
                a1->unk0c = 0;
        }
    }
}
