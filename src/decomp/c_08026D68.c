#include "global.h"

/* Promoted from assembly; each function below is byte-for-byte
 * identical to the original. Order is address order and must
 * stay that way -- the linker places this file's .text as one
 * contiguous block at 0x08026D68.
 * sub_08026D68 @ 0x08026D68
 */

/* The per-army terrain census. Clears each army's tallies, then walks every
 * cell of the gMapData map: the top three bits of the +0x1432 terrain
 * byte select the owning army and the low five are the terrain id, so an owned
 * property adds sub_08026C6C(id) to that army's income and bumps one counter
 * per kind. Terrain 8 (the HQ) records its cell instead of counting.
 *
 * The map is reached through a local overlay because gMapData is
 * declared `u8 *`: unk00/unk02 are the width and height, +0x1432 is the cell
 * array and +0x417a the per-row start table, which is the same +0x1432 /
 * rowOffset pair sub_080253B0 and sub_08058A2C use.
 *
 * The case bodies are emitted in SOURCE order, and the ROM's block order is
 * 8, 14, 20, 6, 10, 11 -- which is why the switch is written that way rather
 * than in ascending case order. The jump table itself is ascending regardless.
 *
 * The `lsls #0x18` on `b & 0xe0` before the zero test is the u8 local `t`, and
 * `lsrs #0x1d` on that same shifted value is `t >> 5`: read the pair as
 * `(u32)x << 24 >> 29`, a net right shift of 5, not as a mask plus a shift. */
struct Unk26D68Map
{
    /* 0x0000 */ u16 width;
    /* 0x0002 */ u16 height;
    /* 0x0004 */ u8 filler_04[0x142e];
    /* 0x1432 */ u8 cells[0x2d48];
    /* 0x417a */ u16 rowOffset[1];
};

void sub_08026D68(void)
{
    int i;
    int j;
    int k;
    u8 b;
    u8 t;
    int kind;
    struct ArmyRecord *army;

    for (i = 0; i <= 4; i++)
    {
        gArmyRecords[i].unk08 = 0;
        gArmyRecords[i].unk0c = 0;
        gArmyRecords[i].unk0d = 0;
        gArmyRecords[i].unk0e = 0;
        gArmyRecords[i].unk0f = 0;
        gArmyRecords[i].unk2d |= 0x80;
    }

    for (j = 0; j < ((struct Unk26D68Map *)gMapData)->height; j++)
    {
        for (k = 0; k < ((struct Unk26D68Map *)gMapData)->width; k++)
        {
            b = ((struct Unk26D68Map *)gMapData)->cells[
                    ((struct Unk26D68Map *)gMapData)->rowOffset[j] + k];

            t = b & 0xe0;
            if (t != 0)
            {
                army = &gArmyRecords[t >> 5];
                kind = b & 0x1f;

                switch (kind)
                {
                case 8:
                    army->unk08 += sub_08026C6C(kind);
                    army->unk2d = k;
                    army->unk2e = j;
                    break;
                case 14:
                    army->unk08 += sub_08026C6C(kind);
                    army->unk0c++;
                    break;
                case 20:
                    army->unk08 += sub_08026C6C(kind);
                    army->unk10++;
                    break;
                case 6:
                    army->unk08 += sub_08026C6C(kind);
                    army->unk0d++;
                    break;
                case 10:
                    army->unk08 += sub_08026C6C(kind);
                    army->unk0e++;
                    break;
                case 11:
                    army->unk08 += sub_08026C6C(kind);
                    army->unk0f++;
                    break;
                }
            }
        }
    }

    sub_08026CD0();
}
