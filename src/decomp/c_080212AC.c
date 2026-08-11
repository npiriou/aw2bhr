#include "global.h"

/* Promoted from assembly; each function below is byte-for-byte
 * identical to the original. Order is address order and must
 * stay that way -- the linker places this file's .text as one
 * contiguous block at 0x080212AC.
 * sub_080212AC @ 0x080212AC
 */

/* Runs one army's turn-start pass: first re-scores all 0x32 of its unit slots,
 * then walks the map and fires sub_080210C8 on every tile whose top three
 * terrain bits equal the army's gUnknown_084995F4 tag.
 *
 * gUnknown_08090960 in the asm is NOT a global: the ROM word at 0x08090960
 * holds 0x08499598, so it is agbcc's own -fforce-addr address constant for
 * gArmyRecords (the guard names it twice, the inner loop once, and only
 * the guard gets the word).
 *
 * The `ldrb` on gUnknown_084995FE is combine narrowing a halfword load to the
 * byte the `(u8)` cast keeps, not evidence of a byte table -- `faction * 2` is
 * CSE'd with gUnknown_084995F4's index and spilled to [sp,#8], which is what
 * proves both tables have a 2-byte stride.
 *
 * That table's declaration LOST its `const` for this function (see
 * include/unknown-globals.h): with `const`, agbcc proves the sub_080211DC call
 * cannot write it and hoists the element load out of the loop.  All seven
 * existing readers were re-verified and still match.
 *
 * `y << 16` living in r8 across the inner loop and the `asrs #0x10` that
 * unpacks it at the call are loop-optimiser output -- y is a plain int here. */
struct Unk212ACMap
{
    /* 0x0000 */ u16 width;
    /* 0x0002 */ u16 height;
    /* 0x0004 */ u8 filler_0004[0x1432 - 0x04];
    /* 0x1432 */ u8 plane[0x417A - 0x1432];
    /* 0x417A */ u16 rowOffset[1];
};

void sub_080212AC(u16 faction)
{
    int i;
    int x;
    int y;

    if (gArmyRecords[faction].unk1b != 0 && gArmyRecords[faction].unk1c != 0)
    {
        for (i = 0; i <= 0x32; i++)
            sub_080211DC((u8)(gUnknown_084995FE[faction] + i), 1);

        for (y = 0; y < ((struct Unk212ACMap *)gMapData)->height; y++)
        {
            for (x = 0; x < ((struct Unk212ACMap *)gMapData)->width; x++)
            {
                if ((((struct Unk212ACMap *)gMapData)->plane[
                         ((struct Unk212ACMap *)gMapData)->rowOffset[y] + x] & 0xE0)
                    == gUnknown_084995F4[faction])
                {
                    sub_080210C8(x, y, 0, gArmyRecords[faction].unk1c, 1, 0);
                }
            }
        }
    }
}
