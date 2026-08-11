#include "global.h"

/* Promoted from assembly; each function below is byte-for-byte
 * identical to the original. Order is address order and must
 * stay that way -- the linker places this file's .text as one
 * contiguous block at 0x0804440C.
 * sub_0804440C @ 0x0804440C, sub_0804443C @ 0x0804443C, sub_08044460 @ 0x08044460, sub_08044488 @ 0x08044488, sub_080444B4 @ 0x080444B4
 */

int sub_0804440C(struct Unk030040D8 *p)
{
    if ((p->unk01 & 8) == 0 && p->unk00 != 1 && p->unk00 != 2 && p->unk00 != 0xa
     && p->unk00 != 0xb && p->unk00 != 0xf && p->unk00 != 0x15)
        return 1;
    else
        return 0;
}

int sub_0804443C(struct Unk030040D8 *p)
{
    if ((p->unk01 & 8) || sub_080432E0(p->unk00) != 6)
        return 0;
    else
        return 1;
}

int sub_08044460(struct Unk030040D8 *p)
{
    if ((p->unk01 & 8) || gUnitTypeData[p->unk00].unk18 != 0)
        return 0;
    else
        return 1;
}

int sub_08044488(struct Unk030040D8 *p)
{
    int t;

    if (p->unk01 & 8)
        return 0;
    t = gUnitTypeData[p->unk00].unk18;
    if (t == 2)
        return 1;
    if (t == 3)
        return 1;
    return 0;
}

int sub_080444B4(struct Unk030040D8 *p)
{
    int t;

    if (p->unk01 & 8)
        return 0;
    t = gUnitTypeData[p->unk00].unk18;
    if (t == 2)
        return 1;
    if (t == 3)
        return 1;
    if (t == 0)
        return 0;
    if ((p->unk01 & 1) == 0)
        return 0;
    return 1;
}
