#include "global.h"

/* Promoted from assembly; each function below is byte-for-byte
 * identical to the original. Order is address order and must
 * stay that way -- the linker places this file's .text as one
 * contiguous block at 0x08008928.
 * sub_08008928 @ 0x08008928, sub_08008A8C @ 0x08008A8C
 */

#include "hardware.h"

int sub_08008928(void)
{
    int x;
    int y;
    int v;
    int q;
    int f;
    int result;

    result = 0;

    gCurrentArmyIndex = f = gUnknown_0200B0B0->unk2f;
    gUnknown_03003F2C = (f - 1) << 6;

    x = gUnknown_0200B0B0->unk08;
    y = gUnknown_0200B0B0->unk0a;

    if (gUnknown_0200B0B0->unk24 == 0x19)
    {
        if (sub_08008A8C(1, x, y))
        {
            sub_08024268();
            result = 2;
        }
    }
    else
    {
        v = sub_08008B70(x, y);

        if (v != gUnknown_0200B0B0->unk24)
        {
            s8 *costs;
            u8 *m;
            u8 *rows;
            u8 *cells;
            int t;
            int idx;
            int c;

            if (v == 0x19)
            {
                v = -1;
            }
            else if (sub_08025308(gUnknown_0200B0B0->unk2f) > 0x31
                     && (v >> 6) + 1 != gUnknown_0200B0B0->unk2f)
            {
                return -1;
            }

            /* WAVE 37 final sweep: `.unk18` was `s8 *` when this function was
             * verified; W37-H later widened it to `s8 *[3]`. Element 0 is at
             * the same offset, so `[0]` is byte-identical to the spelling that
             * matched -- this is a declaration change, not a behaviour one. */
            costs = gCoDataTable[1].unk38[0].unk18[0];

            m = gMapData;
            t = y * 2;
            rows = m + 0x417A;
            idx = *(u16 *)(rows + t) + x;
            cells = m + 0x1432;
            c = (*(cells + idx) & 0x1f)
                + gUnitTypeData[gUnknown_0200B0B0->unk24 & 0x3f].unk19 * 32;

            q = costs[c];

            if (q != -1)
            {
                if (v > 0)
                    sub_08008A8C(0, x, y);

                if (sub_08025308(gUnknown_0200B0B0->unk2f) <= 0x31
                    && sub_08025CC8(x, y, gUnknown_0200B0B0->unk24 & 0x3f))
                    result = 1;
            }
            else
            {
                result = q;
            }
        }
    }

    if (result > 0)
        sub_080088F0();

    return result;
}

int sub_08008A8C(int mode, int x, int y)
{
    u8 *p;
    u8 *rows;
    u8 *cells;
    struct UnitRecord *e;
    int t;
    int idx;
    int result;

    result = 0;

    p = gMapData;
    t = y * 2;
    rows = p + 0x417A;
    idx = *(u16 *)(rows + t) + x;
    cells = p + 0x12;

    if (*(cells + idx) != 0)
    {
        e = &gUnitRecords[*(cells + idx)];

        if (mode != 0)
        {
            SetWinEnable(1, 0, 0);

            gUnknown_03002B40 = 0;
            gUnknown_03002EFC = 0;
            gUnknown_03002B4C = 0;
            gUnknown_03002B44 = 0;

            gUnknown_030030A4.bits.win0_enable_blend = 0;
            gUnknown_030030DC.bits.win1_enable_blend = 1;

            gUnknown_030030A4.bits.win0_enable_bg0 = 1;
            gUnknown_030030A4.bits.win0_enable_bg1 = 1;
            gUnknown_030030A4.bits.win0_enable_bg2 = 1;
            gUnknown_030030A4.bits.win0_enable_bg3 = 1;
            gUnknown_030030A4.bits.win0_enable_obj = 1;

            gUnknown_030030DC.bits.win0_enable_bg0 = 1;
            gUnknown_030030DC.bits.win0_enable_bg1 = 1;
            gUnknown_030030DC.bits.win0_enable_bg2 = 1;
            gUnknown_030030DC.bits.win0_enable_bg3 = 1;
            gUnknown_030030DC.bits.win0_enable_obj = 1;

            sub_0804018C(e);
        }
        else
        {
            e->unk00 = 0;
        }

        sub_080088F0();

        result = 1;
    }

    return result;
}
