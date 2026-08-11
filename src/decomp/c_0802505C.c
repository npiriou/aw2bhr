#include "global.h"

/* Promoted from assembly; each function below is byte-for-byte
 * identical to the original. Order is address order and must
 * stay that way -- the linker places this file's .text as one
 * contiguous block at 0x0802505C.
 * sub_0802505C @ 0x0802505C, sub_080250E8 @ 0x080250E8
 */

struct Unk30013D0
{
    /* 0x00 */ struct UnitRecord *unk00;
    /* 0x04 */ u8 filler_04[0x04];
    /* 0x08 */ s16 unk08;
    /* 0x0a */ u16 unk0a;
};

void sub_0802505C(void *a1)
{
    struct Unk30013D0 *p = a1;
    int v;
    u16 w;

    if (p->unk00->unk04_0 != 0)
        v = Div(p->unk00->unk04_0 - 1, 10) + 1;
    else
        v = 0;

    if (p->unk08 != 0)
        w = v - 1 - Div(p->unk08 - 1, 10);
    else
        w = v;

    sub_08025B24(p->unk00, w);

    p->unk00->unk04_0 = p->unk08;
    p->unk00->unk04_7 = p->unk0a;

    if (p->unk00->unk04_0 == 0)
        sub_0804018C(p->unk00);
}

void sub_080250E8(void)
{
    if (((struct Unk30013D0 *)gUnknown_030013D0)->unk08 <= 0)
        sub_08026588(
            ((((struct Unk30013D0 *)gUnknown_030013B0)->unk00 - gUnitRecords) >> 6) + 1,
            ((((struct Unk30013D0 *)gUnknown_030013D0)->unk00 - gUnitRecords) >> 6) + 1,
            ((struct Unk30013D0 *)gUnknown_030013D0)->unk00->unk00);

    if (((struct Unk30013D0 *)gUnknown_030013B0)->unk08 <= 0)
        sub_08026588(
            ((((struct Unk30013D0 *)gUnknown_030013D0)->unk00 - gUnitRecords) >> 6) + 1,
            ((((struct Unk30013D0 *)gUnknown_030013B0)->unk00 - gUnitRecords) >> 6) + 1,
            ((struct Unk30013D0 *)gUnknown_030013B0)->unk00->unk00);

    sub_0802505C(gUnknown_030013D0);
    sub_0802505C(gUnknown_030013B0);
}
