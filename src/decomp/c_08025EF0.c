#include "global.h"

/* Promoted from assembly; each function below is byte-for-byte
 * identical to the original. Order is address order and must
 * stay that way -- the linker places this file's .text as one
 * contiguous block at 0x08025EF0.
 * sub_08025EF0 @ 0x08025EF0, sub_08025F74 @ 0x08025F74, sub_08025FC0 @ 0x08025FC0
 */

bool8 sub_08025EF0(int a1, int a2)
{
    u8 *t;
    u8 *u;

    if ((a1 & 0xc0) != (a2 & 0xc0))
        return FALSE;

    t = gUnitTypeData[gUnitRecords[a1].unk00].unk14;

    if (t == NULL)
        return FALSE;

    u = t + 1;

    if (u[gUnitRecords[a2].unk00] == 0)
        return FALSE;

    if (t[0] == 1 && gUnitRecords[a1].unk07 != 0)
        return FALSE;

    if (t[0] == 2 && gUnitRecords[a1].unk08 != 0)
        return FALSE;

    return TRUE;
}

bool8 sub_08025F74(struct UnitRecord *a1, u8 a2)
{
    u8 *t;
    u8 *u;

    t = gUnitTypeData[a1->unk00].unk14;

    if (t == NULL)
        return FALSE;

    u = t + 1;

    if (u[a2] == 0)
        return FALSE;

    if (t[0] == 2 && a1->unk08 != 0)
        return FALSE;

    if (t[0] == 1 && a1->unk07 != 0)
        return FALSE;

    return TRUE;
}

bool8 sub_08025FC0(struct UnitRecord *a1, struct UnitRecord *a2)
{
    if (a1->unk00 != a2->unk00)
        return FALSE;

    if (((a1 - gUnitRecords) & 0xc0) != ((a2 - gUnitRecords) & 0xc0))
        return FALSE;

    if (a1->unk07 != 0)
        return FALSE;

    if (a2->unk07 != 0)
        return FALSE;

    if (a2->unk04_0 != 0 && Div(a2->unk04_0 - 1, 10) == 9)
        return FALSE;

    return TRUE;
}
