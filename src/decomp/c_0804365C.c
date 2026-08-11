#include "global.h"

/* Promoted from assembly; each function below is byte-for-byte
 * identical to the original. Order is address order and must
 * stay that way -- the linker places this file's .text as one
 * contiguous block at 0x0804365C.
 * sub_0804365C @ 0x0804365C, sub_080436DC @ 0x080436DC
 */

/* `t -= gUnknown_03004080 - 1;` written in ONE expression reassociates to
 * `(t + 1) - g`, which is not the ROM; the statement boundary is what stops
 * agbcc's fold() (W35-A). The two `ldr rA,=0x1FF; adds rB,rA,#0; ands rC,rB`
 * triplets that used to be one instruction short in each arm are sub_0802BD54's
 * u16 parameters -- see the retyped declaration in unknown-functions.h. */
void sub_0804365C(int x, int y)
{
    int t = sub_08043630();
    int n;

    if (t == 0)
        return;

    n = gUnknown_03004080 - 1;
    t -= n;
    if (t > 0x63)
        return;

    if (x <= 0x77)
        x += 0x55;

    PutSprite(0, x, y, gUnknown_084A0024, 0x1042);

    if (t > 9)
        sub_0802BD54((x - 0xa) & 0x1ff, y, t);
    else
        sub_0802BD54((x - 0xf) & 0x1ff, y, t);
}

void sub_080436DC(int x, int y, int pid)
{
    int off;

    gUnknown_030005D0 = pid;

    if (gUnknown_03003FC0.unk0d != 0 && (gArmyRecords[pid].unk1c & 2) == 0)
        sub_080119A0((x + 0x34) & 0x1ff, y + 3, gUnknown_0809136C);
    else
        sub_0802BD54((x + 0x34) & 0x1ff, y + 3, gArmyRecords[pid].unk00);

    PutSprite(0, x, y, gUnknown_084A0032, 0x7000);
    PutSprite(0, x, y, gUnknown_084A003A, 0xe03a);

    sub_08043AA0(gArmyRecords[pid].unk1d, 0x1e);
    off = ((gArmyRecords[pid].unk1d * 8) & 0x3ff) * 0x20;
    sub_08011E54(gUnknown_08102F64 + off, (void *)0x06010740, 0x100);

    if (gUnknown_03003FC0.unk07 != 0)
    {
        if (gArmyRecords[pid].unk1e != 0)
            sub_08043898(x, y, pid);
        else
            sub_080438FC(x, y, pid);
    }

    if (sub_080442E4(pid))
    {
        if ((u8)sub_08044280(pid))
            sub_0803B4DC(0x1e0);
        else
            sub_0803B4DC(0x75);
    }
}
