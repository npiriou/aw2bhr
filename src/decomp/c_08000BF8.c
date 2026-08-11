#include "global.h"

/* Promoted from assembly; each function below is byte-for-byte
 * identical to the original. Order is address order and must
 * stay that way -- the linker places this file's .text as one
 * contiguous block at 0x08000BF8.
 * sub_08000BF8 @ 0x08000BF8, sub_08000C68 @ 0x08000C68
 */

/* Wave 37 (W37-E). Matched first attempt.
 * The terrain lookup is written through the shared local `struct Map` cast
 * (see include/unknown-globals.h on gMapData and the W34-F note):
 * `map->unk1432[x + map->unk417A[y]]`. Plain byte arithmetic on the `u8 *`
 * reassociates the 0x1432 / 0x417A constants past the index and does not match.
 * unk65 is the "cursor moved" flag this sets and sub_08001DAC consumes. */

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

void sub_08000BF8(void)
{
    if (gUnknown_0200B0B0->unk07 == 0)
    {
        if (gUnknown_0200B0B0->unk2a
            != ((struct Map *)gMapData)->unk1432[
                   gUnknown_0200B0B0->unk08
                   + ((struct Map *)gMapData)->unk417A[gUnknown_0200B0B0->unk0a]])
            gUnknown_0200B0B0->unk65 = 1;
    }
    else
    {
        if (gUnknown_0200B0B0->unk24
            != sub_08008B70(gUnknown_0200B0B0->unk08, gUnknown_0200B0B0->unk0a))
            gUnknown_0200B0B0->unk65 = 1;
    }
}

void sub_08000C68(void)
{
    sub_0803B4DC(0x65);

    if (gUnknown_0200B0B0->unk07 == 0)
        sub_08000CCC(((struct Map *)gMapData)->unk1432[
            gUnknown_0200B0B0->unk08
            + ((struct Map *)gMapData)->unk417A[gUnknown_0200B0B0->unk0a]]);
    else
        sub_08000CCC(sub_08008B70(gUnknown_0200B0B0->unk08, gUnknown_0200B0B0->unk0a));
}
