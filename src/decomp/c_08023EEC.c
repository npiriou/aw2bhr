#include "global.h"

/* Promoted from assembly; each function below is byte-for-byte
 * identical to the original. Order is address order and must
 * stay that way -- the linker places this file's .text as one
 * contiguous block at 0x08023EEC.
 * sub_08023EEC @ 0x08023EEC
 */

void sub_08023EEC(void)
{
    if (((s16)*(u16 *)(gMapData + 4) < (s16)*(u16 *)(gMapData + 8)
      && ((s16)*(u16 *)(gMapData + 4) >> 4) != ((s16)*(u16 *)(gMapData + 8) >> 4))
     || ((s16)*(u16 *)(gMapData + 4) > (s16)*(u16 *)(gMapData + 8)
      && ((s16)*(u16 *)(gMapData + 4) >> 4) != ((*(s16 *)(gMapData + 8) - 1) >> 4)))
    {
        if ((s16)*(u16 *)(gMapData + 4) < *(s16 *)(gMapData + 8))
            sub_08023DCC(((s16)*(u16 *)(gMapData + 4) >> 4) - *(u16 *)(gMapData + 0xc),
                         ((s16)*(u16 *)(gMapData + 6) >> 4) - *(u16 *)(gMapData + 0xe),
                         (s16)*(u16 *)(gMapData + 4) >> 4,
                         (s16)*(u16 *)(gMapData + 6) >> 4);
        else
            sub_08023E14((((s16)*(u16 *)(gMapData + 4) >> 4) + 0xf) - *(u16 *)(gMapData + 0xc),
                         ((s16)*(u16 *)(gMapData + 6) >> 4) - *(u16 *)(gMapData + 0xe),
                         ((s16)*(u16 *)(gMapData + 4) >> 4) + 0xf,
                         (s16)*(u16 *)(gMapData + 6) >> 4);

        sub_08013AFC();
        sub_08013B0C();
        sub_08013B1C();

        if (gUnknown_03000559 == 1)
            sub_08013AEC();
    }

    if (((s16)*(u16 *)(gMapData + 6) < (s16)*(u16 *)(gMapData + 0xa)
      && ((s16)*(u16 *)(gMapData + 6) >> 4) != ((s16)*(u16 *)(gMapData + 0xa) >> 4))
     || ((s16)*(u16 *)(gMapData + 6) > (s16)*(u16 *)(gMapData + 0xa)
      && ((s16)*(u16 *)(gMapData + 6) >> 4) != ((*(s16 *)(gMapData + 0xa) - 1) >> 4)))
    {
        if ((s16)*(u16 *)(gMapData + 6) < *(s16 *)(gMapData + 0xa))
            sub_08023E5C(((s16)*(u16 *)(gMapData + 4) >> 4) - *(u16 *)(gMapData + 0xc),
                         ((s16)*(u16 *)(gMapData + 6) >> 4) - *(u16 *)(gMapData + 0xe),
                         (s16)*(u16 *)(gMapData + 4) >> 4,
                         (s16)*(u16 *)(gMapData + 6) >> 4);
        else
            sub_08023EA4(((s16)*(u16 *)(gMapData + 4) >> 4) - *(u16 *)(gMapData + 0xc),
                         (((s16)*(u16 *)(gMapData + 6) >> 4) + 0xa) - *(u16 *)(gMapData + 0xe),
                         (s16)*(u16 *)(gMapData + 4) >> 4,
                         ((s16)*(u16 *)(gMapData + 6) >> 4) + 0xa);

        sub_08013AFC();
        sub_08013B0C();
        sub_08013B1C();

        if (gUnknown_03000559 == 1)
            sub_08013AEC();
    }

    {
        u8 *map;

        map = gMapData;
        *(u16 *)(map + 8) = *(u16 *)(map + 4);
        *(u16 *)(map + 0xa) = *(u16 *)(map + 6);
    }
}
