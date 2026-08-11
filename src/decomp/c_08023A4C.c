#include "global.h"

/* Promoted from assembly; each function below is byte-for-byte
 * identical to the original. Order is address order and must
 * stay that way -- the linker places this file's .text as one
 * contiguous block at 0x08023A4C.
 * sub_08023A4C @ 0x08023A4C
 */

void sub_08023A4C(u16 a1, u16 a2, u16 a3, u16 a4)
{
    struct MapData *m;
    u16 *dst;
    u16 off;
    u16 i;

    off = (a2 & 0xF) * 64;
    dst = gUnknown_08499584 + (a1 & 0xF) * 2;

    for (i = 0; i <= 10; i++)
    {
        m = (struct MapData *)gMapData;
        if (m->unk234a[m->unk417a[a4 + i] + a3] == 0)
        {
            dst[off] = gUnknown_080BFBC4[m->unk0a22[m->unk417a[a4 + i] + a3]][0] + 0x4000;
            dst[off + 1] = gUnknown_080BFBC4[m->unk0a22[m->unk417a[a4 + i] + a3]][1] + 0x4000;
            dst[off + 32] = gUnknown_080BFBC4[m->unk0a22[m->unk417a[a4 + i] + a3]][2] + 0x4000;
            dst[off + 33] = gUnknown_080BFBC4[m->unk0a22[m->unk417a[a4 + i] + a3]][3] + 0x4000;
        }
        else
        {
            dst[off] = gUnknown_080BFBC4[m->unk0a22[m->unk417a[a4 + i] + a3]][0];
            dst[off + 1] = gUnknown_080BFBC4[m->unk0a22[m->unk417a[a4 + i] + a3]][1];
            dst[off + 32] = gUnknown_080BFBC4[m->unk0a22[m->unk417a[a4 + i] + a3]][2];
            dst[off + 33] = gUnknown_080BFBC4[m->unk0a22[m->unk417a[a4 + i] + a3]][3];
        }
        off = ((u32)(off + 0x40) << 22) >> 22;
    }
}
