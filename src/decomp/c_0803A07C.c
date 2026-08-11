#include "global.h"

/* Promoted from assembly; each function below is byte-for-byte
 * identical to the original. Order is address order and must
 * stay that way -- the linker places this file's .text as one
 * contiguous block at 0x0803A07C.
 * sub_0803A07C @ 0x0803A07C
 */

void sub_0803A07C(void)
{
    struct UnitRecord *u;
    void **tbl;
    int x;
    int n;
    s8 d;

    u = gUnknown_0849D89C->unk04;
    x = gUnknown_0849D89C->unk00 + 0x30;
    tbl = gUnknown_0849DC18;
    n = sub_08042DE0(gUnknown_0849D89C->unk08) - 1;
    sub_0801BD00(x, 0x39, tbl[u->unk00 * 15 + n], 0x32E8);
    sub_0801F34C(gUnknown_0849E224[u->unk00], gUnknown_0849D89C->unk00 + 0x3a, 8, 0, 0);
    sub_0801F34C(0x23, gUnknown_0849D89C->unk00 + 0x3a, 0x18, 0, 0);
    sub_0801F34C(6, gUnknown_0849D89C->unk00 + 0x44, 0x28, 0, 0);
    d = (u->unk06_0 <= 9) ? -4 : 0;
    sub_0802BCF0(d + gUnknown_0849D89C->unk00 + 0x58, 0x28, u->unk06_0);
    d = (gUnitTypeData[u->unk00].unk10 <= 9) ? -4 : 0;
    sub_0802BCF0(d + gUnknown_0849D89C->unk00 + 0x64, 0x30, gUnitTypeData[u->unk00].unk10);
    sub_0801BD00(gUnknown_0849D89C->unk00 + 0x5b, 0x2c, gUnknown_0849D8A0, 0x13CA);
    sub_0803AB3C();
}
