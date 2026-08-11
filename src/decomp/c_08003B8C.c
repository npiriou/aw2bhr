#include "global.h"

/* Promoted from assembly; each function below is byte-for-byte
 * identical to the original. Order is address order and must
 * stay that way -- the linker places this file's .text as one
 * contiguous block at 0x08003B8C.
 * sub_08003B8C @ 0x08003B8C
 */

#include "hardware.h"
/* MATCHED, wave 40 (W40-A). Needs its .rodata pool word placed:
 *   "rodata": ["0x0808D788"]
 * -- the ROM word there holds &gUnknown_0200B0B0, so writing the global's name
 * honestly is correct and trymatch reports only the symbol-naming artefact.
 *
 * Resets the map header to 30 x 20, rebuilds the +0x417A row-offset table, and
 * then picks a fill routine from the L/R key pair. Two notes worth keeping:
 *   - the row-offset loop has NO entry guard because the `strh #0x14` two
 *     statements earlier is CSE'd into the guard's load, folding `0 < 20` away.
 *     It is an ordinary `for`, not a do/while.
 *   - the key word is gpKeySt->unk00 (offset 0), NOT ->held (offset 4). unk00
 *     is the mask hardware.h already documents as carrying 0x200 (L) and
 *     0x100 (R), which is exactly the 0x300 tested here. */

struct Unk3F44Map
{
    /* 0x0000 */ u16 width;
    /* 0x0002 */ u16 height;
    /* 0x0004 */ u8 filler_0004[0x10 - 0x04];
    /* 0x0010 */ u16 unk10;
    /* 0x0012 */ u8 filler_0012[0x417A - 0x12];
    /* 0x417A */ u16 rowOffset[1];
};
#define MAP ((struct Unk3F44Map *)gMapData)

void sub_08003B8C(void)
{
    int i;
    int keys;

    sub_08025E74();
    gUnknown_0200B0B0->unk13 = 0;
    MAP->width = 0x1E;
    MAP->height = 0x14;
    MAP->unk10 = 0;
    for (i = 0; i < MAP->height; i++)
        MAP->rowOffset[i] = i * MAP->width;

    keys = gpKeySt->unk00 & 0x300;
    if (keys == 0x300)
    {
        sub_08004724();
        sub_08003ED0();
    }
    else if (keys == 0x200)
    {
        sub_080040C8();
        sub_08003ED0();
    }
    else if (keys == 0x100)
    {
        sub_0800449C();
        sub_08003ED0();
    }
    else if (DivRem(sub_080129E0(), 100) > 0x1D)
    {
        sub_080040C8();
        sub_08003ED0();
    }
    else
    {
        sub_0800449C();
        sub_08003ED0();
    }

    sub_080219AC();
    sub_0800C8D8();
    gUnknown_0200B0B0->unk12 = sub_0800C874();
}
