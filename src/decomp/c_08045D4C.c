#include "global.h"

/* Promoted from assembly; each function below is byte-for-byte
 * identical to the original. Order is address order and must
 * stay that way -- the linker places this file's .text as one
 * contiguous block at 0x08045D4C.
 * sub_08045D4C @ 0x08045D4C
 */

struct Map
{
    /* 0x0000 */ u16 unk00;
    /* 0x0002 */ u16 unk02;
    /* 0x0004 */ u16 unk04;
    /* 0x0006 */ u16 unk06;
    /* 0x0008 */ u8 filler_0008[0x0A];
    /* 0x0012 */ u8 unk0012[0x0508];
    /* 0x051A */ u8 unk051A[0x0F18];
    /* 0x1432 */ u8 unk1432[0x0A10];
    /* 0x1E42 */ u8 unk1E42[0x0508];
    /* 0x234A */ u8 unk234A[0x0508];
    /* 0x2852 */ u8 unk2852[0x0A10];
    /* 0x3262 */ u8 unk3262[0x0F18];
    /* 0x417A */ u16 unk417A[0x100];
};

int sub_08045D4C(void)
{
    int x;
    int y;
    int side;
    int c;
    int oleft;
    int oright;

    x = gUnknown_030040D8->unk02;
    y = gUnknown_030040D8->unk03;

    switch (gArmyRecords[(((struct UnitRecord *)gUnknown_030040D8
                                - gUnitRecords) >> 6) + 1].unk1a)
    {
    case 3:
        side = 1;
        break;
    case 1:
        side = 3;
        break;
    default:
        return 0;
    }

    if (y > 0)
    {
        c = ((struct Map *)gMapData)->unk051A[
                ((struct Map *)gMapData)->unk417A[y - 1] + x];
        if (c != 0 && gArmyRecords[(c >> 6) + 1].unk1a == side)
            return 1;
    }

    if (y < ((struct Map *)gMapData)->unk02 - 1)
    {
        c = ((struct Map *)gMapData)->unk051A[
                ((struct Map *)gMapData)->unk417A[y + 1] + x];
        if (c != 0 && gArmyRecords[(c >> 6) + 1].unk1a == side)
            return 1;
    }

    if (x > 0)
    {
        oleft = ((struct Map *)gMapData)->unk417A[y] - 1;
        c = ((struct Map *)gMapData)->unk051A[oleft + x];
        if (c != 0 && gArmyRecords[(c >> 6) + 1].unk1a == side)
            return 1;
    }

    if (x < ((struct Map *)gMapData)->unk00 - 1)
    {
        oright = ((struct Map *)gMapData)->unk417A[y] + 1;
        c = ((struct Map *)gMapData)->unk051A[oright + x];
        if (c != 0 && gArmyRecords[(c >> 6) + 1].unk1a == side)
            return 1;
    }

    return 0;
}
