#include "global.h"

/* Promoted from assembly; each function below is byte-for-byte
 * identical to the original. Order is address order and must
 * stay that way -- the linker places this file's .text as one
 * contiguous block at 0x080440A8.
 * sub_080440A8 @ 0x080440A8, sub_080440E0 @ 0x080440E0
 */

void sub_080440A8(int a1, int a2)
{
    int v;

    switch (a2) {
    default:
    case 1:
        v = sub_080441D4(a1);
        break;
    case 2:
        v = sub_08044208(a1);
        break;
    }
    v = sub_08044094(a1) - v;
    if (v < 0)
        v = 0;
    sub_08044080(a1, v);
}

void sub_080440E0(int a1, int a2)
{
    int v;

    if (gUnknown_03003FC0.unk07 == 0)
        return;
    if (sub_0804415C(a1))
        return;
    v = gArmyRecords[a1].unk20;
    if (v < 0)
        return;
    if (v + a2 > sub_08044208(a1))
        gArmyRecords[a1].unk20 = sub_08044208(a1);
    else
        gArmyRecords[a1].unk20 = gArmyRecords[a1].unk20 + a2;
}
