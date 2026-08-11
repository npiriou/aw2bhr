#include "global.h"

/* Promoted from assembly; each function below is byte-for-byte
 * identical to the original. Order is address order and must
 * stay that way -- the linker places this file's .text as one
 * contiguous block at 0x0801759C.
 * sub_0801759C @ 0x0801759C
 */

/* Rebuilds the whole terrain plane of the gMapData map from its tile
 * plane, then kicks the three subsystems that cache it and re-selects the
 * current cursor target.
 *
 * Per cell: `idx = rowOffset[y] + x` -- the same 0x417A row-offset table the
 * rest of this map's readers use -- then
 * `terrain[idx] = gUnknown_0849959C[tile[idx]]`, i.e. the u16 tile id at
 * +0x0A22 indexes a global byte table and the result lands in the +0x1432
 * byte plane at the SAME index.
 *
 * The local struct cast is the wave-34 spelling recorded on gMapData
 * in include/unknown-globals.h: only a COMPONENT_REF keeps the ROM's
 * `(map + K) + idx` association: plain `gMapData[0x1432 + idx]` folds
 * K into the load displacement, and `*(u16 *)(gMapData + 0x417A +
 * y * 2)` reassociates the constant outward past the variable. The tag is
 * file-local and its 0x0A22 member splits what c_08041EA8.c's layout calls
 * `unit`; both spellings describe the same bytes and rescale nothing.
 *
 * Both counters are plain `int`: neither the increment nor the exit test
 * carries a narrowing, and both bounds are `ldrh`ed and compared SIGNED. */

struct Unk1759CMap
{
    /* 0x0000 */ u16 width;
    /* 0x0002 */ u16 height;
    /* 0x0004 */ u8 filler_0004[0xa1e];
    /* 0x0a22 */ u16 tile[0x508];
    /* 0x1432 */ u8 terrain[0x2d48];
    /* 0x417a */ u16 rowOffset[1];
};

void sub_0801759C(void)
{
    int x;
    int y;

    for (y = 0; y < ((struct Unk1759CMap *)gMapData)->height; y++)
    {
        for (x = 0; x < ((struct Unk1759CMap *)gMapData)->width; x++)
        {
            int idx;

            idx = ((struct Unk1759CMap *)gMapData)->rowOffset[y] + x;
            ((struct Unk1759CMap *)gMapData)->terrain[idx] =
                gUnknown_0849959C[((struct Unk1759CMap *)gMapData)->tile[idx]];
        }
    }

    sub_080215B8();
    sub_08023348();
    sub_08024268();
    sub_0801A548(gCurrentArmyIndex);
}
