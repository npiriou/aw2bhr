#include "global.h"

/* Promoted from assembly; each function below is byte-for-byte
 * identical to the original. Order is address order and must
 * stay that way -- the linker places this file's .text as one
 * contiguous block at 0x0803D238.
 * sub_0803D238 @ 0x0803D238, sub_0803D2F8 @ 0x0803D2F8
 */

struct Rec
{
    /* 0x0000 */ u8 width;
    /* 0x0001 */ u8 height;
    /* 0x0002 */ u16 tile[(0x04B2 - 0x0002) / 2];
    /* 0x04B2 */ u8 name[0x11];
    /* 0x04C3 */ u8 unk4C3;
    /* 0x04C4 */ u8 unk4C4[5];
    /* 0x04C9 */ u8 unk4C9;
    /* 0x04CA */ u8 unk4CA;
    /* 0x04CB */ u8 cell[1];
};
struct Map
{
    /* 0x0000 */ u16 width;
    /* 0x0002 */ u16 height;
    /* 0x0004 */ u8 filler_04[0x0A22 - 0x0004];
    /* 0x0A22 */ u16 tile[(0x1432 - 0x0A22) / 2];
    /* 0x1432 */ u8 terrain[0x417A - 0x1432];
    /* 0x417A */ u16 rowOffset[(0x4232 - 0x417A) / 2];
    /* 0x4232 */ u8 filler_4232[1];
    /* 0x4233 */ u8 unk4233;
};

/* The cell byte is read from the array at each use rather than bound to a
 * `u8` local, and that is what produces the ROM's `lsls #0x18` / `lsrs #0x1e`
 * pair. A `u8` local is PROMOTE_MODE'd to an SImode pseudo whose top bits
 * agbcc knows are zero, so `v >> 6` collapses to a single `lsr #6` and the
 * `!= 0` test compares the raw register. Reading through the record keeps the
 * value in QImode: the test needs `lsl #24` first, and the extract of bits
 * 6..7 then REUSES that shift (`lsr #30`). `& 0x3f` needs no extension in
 * either mode, which is why the low field stays a plain `and` in both.
 *
 * gCurrentArmyIndex and gUnknown_03003F2C are saved and restored around the
 * call loop -- both spelled honestly. Their initial reads go through this
 * unit's own -fforce-addr `.rodata` words (the ROM words at 0x08091148 and
 * 0x0809114C, which hold &gCurrentArmyIndex and &gUnknown_03003F2C -- checked
 * against baserom.gba, they are pool words and not globals), while the writes
 * get ordinary inline pool words. agbcc places both from the honest spelling;
 * nothing needs to name the 0x0809 addresses. The save locals are `int`, not
 * `u16`: the ROM stores them with `str` and reads them back with `ldrh`,
 * which is a word-wide local narrowed by the u16 store at the far end. */
void sub_0803D238(u8 *a1)
{
    int save0;
    int save1;
    int x, y, k;

    save0 = gCurrentArmyIndex;
    save1 = gUnknown_03003F2C;
    sub_08025E74();
    k = 0;
    for (y = 0; y < ((struct Map *)gMapData)->height; y++) {
        for (x = 0; x < ((struct Map *)gMapData)->width; x++) {
            if (((struct Rec *)a1)->cell[k] != 0) {
                gCurrentArmyIndex = (((struct Rec *)a1)->cell[k] >> 6) + 1;
                gUnknown_03003F2C = (((struct Rec *)a1)->cell[k] >> 6) << 6;
                sub_08025CC8(x, y, ((struct Rec *)a1)->cell[k] & 0x3f);
            }
            k++;
        }
    }
    gCurrentArmyIndex = save0;
    gUnknown_03003F2C = save1;
}

/* The 5-byte copy loop REUSES `x` rather than taking a fresh `i`, and that is
 * a real byte difference, not a style choice: a fresh counter is a separate
 * pseudo and lands in r3 with the source base in r4, where the ROM has them
 * the other way round. Sharing the pseudo with the tile loop's column counter
 * puts it in r4 in both. Declaration order does not reach this -- moving `i`
 * to the end of the declaration list, into its own block, or binding the
 * source base to a named pointer were all measured and none moved it. */
void sub_0803D2F8(int a1, u8 *a2)
{
    int x, y, k;

    sub_0803CC84((u8 *)a1, ((struct Rec *)a2)->name);
    ((struct Map *)gMapData)->unk4233 = ((struct Rec *)a2)->unk4C3;
    for (x = 0; x <= 4; x++)
        gUnknown_03003FF3[x] = ((struct Rec *)a2)->unk4C4[x];
    ((struct Map *)gMapData)->width = ((struct Rec *)a2)->width;
    ((struct Map *)gMapData)->height = ((struct Rec *)a2)->height;
    sub_080215FC();
    k = 0;
    for (y = 0; y < ((struct Map *)gMapData)->height; y++) {
        for (x = 0; x < ((struct Map *)gMapData)->width; x++) {
            ((struct Map *)gMapData)
                ->tile[((struct Map *)gMapData)->rowOffset[y] + x] =
                ((struct Rec *)a2)->tile[k];
            k++;
        }
    }
}
