#include "global.h"

/* Promoted from assembly; each function below is byte-for-byte
 * identical to the original. Order is address order and must
 * stay that way -- the linker places this file's .text as one
 * contiguous block at 0x0802DCB4.
 * sub_0802DCB4 @ 0x0802DCB4
 */

#include "hardware.h"
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
    /* 0x2852 */ u8 unk2852[0x1928];
    /* 0x417A */ u16 unk417A[0x100];
};

void sub_0802DCB4(void)
{
    struct UnitRecord *unit;

    sub_08023824();
    sub_0802361C();
    sub_08023908(4);
    sub_08023274(0);

    if (sub_0802DBF8())
    {
        if (gpKeySt->held & 8)
        {
            sub_0802E250();
            return;
        }

        if (gpKeySt->held & 4)
        {
            sub_0801B780(0);
            sub_0802D458();
            return;
        }

        if (gpKeySt->held & 0x100)
        {
            if (((struct Map *)gMapData)->unk0012[
                    ((struct Map *)gMapData)->unk417A[gUnknown_030033E4.unk02]
                    + gUnknown_030033E4.unk00] != 0)
            {
                sub_0801B780(0);
                sub_0803A8F0(&gUnitRecords[
                    ((struct Map *)gMapData)->unk0012[
                        ((struct Map *)gMapData)->unk417A[gUnknown_030033E4.unk02]
                        + gUnknown_030033E4.unk00]]);
                return;
            }

            sub_0801B780(0);
            sub_080470F8(sub_0803EED4(gUnknown_030033E4.unk00, gUnknown_030033E4.unk02));
            return;
        }

        if (gpKeySt->held & 0x200)
        {
            sub_08025580();
            unit = sub_080254AC();

            if (unit != NULL)
            {
                sub_08029088(unit->unk02, unit->unk03);

                if (sub_08015BD0((s32)gUnknown_0849A00C) != -1)
                {
                    sub_0802DCA4();
                    return;
                }
            }
        }

        if (gpKeySt->held & 1)
        {
            sub_0802E4B4(((struct Unk802C57CS *)&gUnknown_030033E4)->unk00,
                         ((struct Unk802C57CS *)&gUnknown_030033E4)->unk02);
            return;
        }

        if (gpKeySt->held & 2)
        {
            if (sub_0802E2D0(((struct Unk802C57CS *)&gUnknown_030033E4)->unk00,
                             ((struct Unk802C57CS *)&gUnknown_030033E4)->unk02))
                return;
        }
    }

    sub_0802A7C4();
    sub_0802776C(0);
}
