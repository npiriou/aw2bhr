#include "global.h"

/* Promoted from assembly; each function below is byte-for-byte
 * identical to the original. Order is address order and must
 * stay that way -- the linker places this file's .text as one
 * contiguous block at 0x0803E3D8.
 * sub_0803E3D8 @ 0x0803E3D8
 */

/* MATCHED (wave 49, W49-C), first attempt. 380/380 bytes.
 *
 * PROMOTION NOTE: the only reloc difference is agbcc's own -fforce-addr copy of
 * &gMapData, which asm/ prints as `gUnknown_080912F8` because gen_lds.py
 * invents a symbol for the ROM word. The ROM word at 0x080912F8 contains
 * 0x08499590. Do NOT declare gUnknown_080912F8. This entry needs
 *   "rodata": ["0x080912F8"]
 * in data/promoted.json, then tools/split_rodata.py + tools/gen_lds.py.
 *
 * The map planes are reached as MEMBERS of a struct cast onto gMapData
 * (W34-F's rule): a flat `gMapData[0x1432 + rowOffset[y] + x]`
 * reassociates to `(p + idx) + K` where the ROM has `(p + K) + idx`. Both the
 * 0x417A row table and the 0x1432 terrain plane need it here, and the 0x417A
 * constant is too wide for `adds #imm8`, which is why the ROM spends a register
 * and a pool word on it -- that is a consequence of the offset, not a lever.
 *
 * The switch is the dense `subs #0x15; cmp #0xa; bhi` form, so case N of the
 * 11-entry jump table at _0803E464 is source value N + 0x15. Case BODIES come
 * out in SOURCE order, which is what fixes the odd-looking order below
 * (0x19, then 0x16/0x15/0x17/0x18, then 0x1c/0x1b/0x1a/0x1d/0x1f/0x1e); the
 * four sub_0803E260 arms are cross-jumped onto one shared tail by jump.c, so
 * they are four ordinary cases differing only in the fifth argument, not a
 * grouped case label.
 */

struct Unk3E3D8Map
{
    /* 0x0000 */ u16 unk00;
    /* 0x0002 */ u16 unk02;
    /* 0x0004 */ u8 filler_0004[0x1432 - 0x04];
    /* 0x1432 */ u8 terrain[0x417A - 0x1432];
    /* 0x417A */ u16 rowOffset[1];
};
#define MAP ((struct Unk3E3D8Map *)gMapData)

void sub_0803E3D8(void)
{
    int x;
    int y;
    int a;
    int b;

    a = gUnknown_085C77A0[gUnknown_03003FC0.unk02].unk1c;
    b = gUnknown_085C77A0[gUnknown_03003FC0.unk02].unk1e;
    sub_0803DE14();
    for (y = 0; y < MAP->unk02; y++)
    {
        for (x = 0; x < MAP->unk00; x++)
        {
            if (sub_0803DE94(x, y))
                continue;
            switch (MAP->terrain[MAP->rowOffset[y] + x] & 0x1f)
            {
            case 0x19:
                sub_0803E158(x, y, a, b);
                break;
            case 0x16:
                sub_0803E260(x, y, a, b, 1);
                break;
            case 0x15:
                sub_0803E260(x, y, a, b, 0);
                break;
            case 0x17:
                sub_0803E260(x, y, a, b, 2);
                break;
            case 0x18:
                sub_0803E260(x, y, a, b, 3);
                break;
            case 0x1c:
                sub_0803E1B0(x, y, 4, 4, 2, 1);
                break;
            case 0x1b:
                sub_0803E208(x, y, 3, 3, a, 2, 1);
                break;
            case 0x1a:
                sub_0803E208(x, y, 3, 3, a, b, 0);
                break;
            case 0x1d:
                sub_0803E310(x, y, 3, 4, 1, 1);
                break;
            case 0x1f:
                sub_0803E108(x, y, 4, 4);
                break;
            case 0x1e:
                sub_0803E2B8(x, y, 3, 3, 7, 7);
                break;
            }
        }
    }
}
