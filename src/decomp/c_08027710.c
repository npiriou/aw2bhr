#include "global.h"

/* Promoted from assembly; each function below is byte-for-byte
 * identical to the original. Order is address order and must
 * stay that way -- the linker places this file's .text as one
 * contiguous block at 0x08027710.
 * sub_08027710 @ 0x08027710, sub_0802776C @ 0x0802776C, sub_080277BC @ 0x080277BC
 */

void sub_08027710(void)
{
    u16 x;
    u16 y;

    x = gUnknown_030033E4.unk00 * 16 - *(u16 *)(gMapData + 4);
    y = gUnknown_030033E4.unk02 * 16 - *(u16 *)(gMapData + 6);

    if ((s16)y <= 0x4f)
    {
        if ((s16)x <= 0x7f)
            sub_080276F0();
        else
            sub_080276D0();
    }
    else
    {
        if (gUnknown_03003130.unk00 == 0)
            sub_080276D0();
        else
            sub_080276F0();
    }
}

void sub_0802776C(u8 a1)
{
    switch (a1)
    {
    case 0:
        sub_0801F024(sub_08027658, 0);
        break;

    case 1:
        sub_0801F024(sub_080276D0, 0);
        break;

    case 2:
        sub_0801F024(sub_080276F0, 0);
        break;

    case 3:
        sub_0801F024(sub_08027710, 0);
        break;
    }
}

void sub_080277BC(void)
{
    gUnknown_03001470[gUnknown_03001FBC].unk24++;

    sub_080157A4(gUnknown_03001FBC, gUnknown_08090AA8[gUnknown_03001470[gUnknown_03001FBC].unk24]);
    sub_080157F4(gUnknown_03001FBC, gUnknown_08090AA8[gUnknown_03001470[gUnknown_03001FBC].unk24]);

    if (gUnknown_08090AA8[gUnknown_03001470[gUnknown_03001FBC].unk24] == 0x100)
        sub_08015C30(gUnknown_03001FBC);
}
