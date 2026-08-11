#include "global.h"

/* Promoted from assembly; each function below is byte-for-byte
 * identical to the original. Order is address order and must
 * stay that way -- the linker places this file's .text as one
 * contiguous block at 0x08021810.
 * sub_08021810 @ 0x08021810, sub_080219AC @ 0x080219AC
 */

/* Reports two byte counts through out-pointers.  For the 0xB4..0xBF range of
 * gUnknown_03003FC0.unk02 the answers are canned in gUnknown_020280C0's record;
 * otherwise it loads the map, tallies the properties on it per owner into a
 * six-byte scratch, and reports (largest single army's tally, total).
 *
 * The guard is the same fold range test sub_0802163C carries, but on a u8
 * member: build_range_check keeps unsigned char as the working type, so the
 * subtraction truncates -- `adds #0x4c; lsls #0x18; lsrs #0x18; cmp #0xb; bhi`
 * is `unk02 >= 0xb4 && unk02 <= 0xbf` and nothing else.  (+0x4c is -0xb4 mod
 * 256; the ROM's own `subs #0xb4` in the index below is the same constant.)
 *
 * The two canned bytes are gUnknown_020280C0[i].filler_14[5] and [6], i.e.
 * +0x19 and +0x1a.  They are left inside filler_14 deliberately: the only other
 * reader/writer, src/decomp/c_0803D3F0.c, already writes exactly those two
 * indices from a save record, so naming them would strand that file for no
 * byte-level gain.
 *
 * The clear loop, the inner map loop and the max loop all run on ONE local `x`.
 * Declaring a separate `i` for the two short loops costs 4 bytes: x and the
 * `y * 2` temp swap r3/r4 and the pool address then needs an extra `mov r0, ip`
 * to load through.  sub_080219AC in this same block needed the identical fix;
 * see docs/agbcc-codegen.md.
 *
 * gUnknown_0809096C and gUnknown_08090970 in the asm are NOT globals -- the ROM
 * words there hold 0x030032D0 and 0x08499590, agbcc's own -fforce-addr address
 * constants.  0x030032D0 had no linker symbol at all (gen_lds.py only invents
 * one for addresses `asm/` names directly, and nothing names this one outside a
 * pool word), so this function is what proves the object exists; see
 * include/unknown-globals.h and the new aw2bhr.lds line. */

struct Unk21810Map
{
    /* 0x0000 */ u16 width;
    /* 0x0002 */ u16 height;
    /* 0x0004 */ u8 filler_0004[0x1432 - 0x04];
    /* 0x1432 */ u8 plane[0x417A - 0x1432];
    /* 0x417A */ u16 rowOffset[1];
};
/* Rebuilds the two property lists from the map plane, in two passes over every
 * cell.  Pass one counts each army's properties into gArmyRecords[].unk11,
 * stamps the list index into the map's second 0x508-byte plane at +0x193A, and
 * fills gPropertyList[] with (terrain, x, y); pass two fills
 * gUnknown_03003150[] with (raw cell, x, y).  Both lists are 0xFF-terminated,
 * and the second is capped at 0x5C entries -- the counter still advances past
 * the cap, so the terminator can land exactly at [0x5b].
 *
 * gUnknown_08090974 in the asm is NOT a global: the ROM word there holds
 * 0x08499590, i.e. &gMapData, and it is one of a run of three
 * consecutive words (0x08090970/74/78) all holding that same address --
 * -fforce-addr's one-word-per-(function, symbol) private copies. Spelling
 * gMapData honestly reproduces the ROM's three-level
 * `ldr =word / ldr [r] / ldr [r]` for the loop-carried references while still
 * emitting the ordinary two-level inline pool word at the sites that get one.
 *
 * The reset loop really is ascending: `subs r6,#1 / cmp r6,#0 / bge` at the
 * bottom is check_dbra_loop reversing an `x <= 4` counter whose only use is the
 * strength-reduced 0x3c byte stride.
 *
 * That reset loop runs on `x`, the SAME local as the inner map loop, and that
 * is worth 4 bytes: with a separate `i` declared, x lands in ip and
 * &gMapData's pool address in r6, which is the reverse of the ROM.
 * Merging them adds the reset loop's references to x's allocno and moves it
 * ahead of the address constant they were tying with.  sub_08021810 in this
 * same block needed the identical fix; see docs/agbcc-codegen.md.
 *
 * Case 16 in pass two falls THROUGH into the main group when
 * gUnknown_03003FC0.unk01 == 5; the ROM's `bne` to the skip label plus a
 * fall-in to the shared block is exactly a case label with no break. */

struct Unk219ACMap
{
    /* 0x0000 */ u16 width;
    /* 0x0002 */ u16 height;
    /* 0x0004 */ u8 filler_0004[0x1432 - 0x04];
    /* 0x1432 */ u8 plane[0x193A - 0x1432];
    /* 0x193A */ u8 owner[0x417A - 0x193A];
    /* 0x417A */ u16 rowOffset[(0x421A - 0x417A) / 2];
};

void sub_08021810(u8 *a, u8 *b)
{
    u8 x;
    u8 y;

    if (gUnknown_03003FC0.unk02 >= 0xb4 && gUnknown_03003FC0.unk02 <= 0xbf)
    {
        *b = gUnknown_020280C0[gUnknown_03003FC0.unk02 - 0xb4].filler_14[5];
        *a = gUnknown_020280C0[gUnknown_03003FC0.unk02 - 0xb4].filler_14[6];
        return;
    }

    sub_0802163C(gUnknown_03003FC0.unk02);

    for (x = 0; x <= 5; x++)
        gUnknown_030032D0[x] = 0;

    for (y = 0; y < ((struct Unk21810Map *)gMapData)->height; y++)
    {
        for (x = 0; x < ((struct Unk21810Map *)gMapData)->width; x++)
        {
            switch (((struct Unk21810Map *)gMapData)->plane[
                        ((struct Unk21810Map *)gMapData)->rowOffset[y] + x] & 0x1f)
            {
            case 6:
            case 8:
            case 10:
            case 11:
            case 14:
            case 20:
                gUnknown_030032D0[5]++;
                gUnknown_030032D0[
                    ((struct Unk21810Map *)gMapData)->plane[
                        ((struct Unk21810Map *)gMapData)->rowOffset[y] + x] >> 5]++;
                break;
            }
        }
    }

    gUnknown_030032D0[0] = 0;

    for (x = 0; x <= 4; x++)
    {
        if (gUnknown_030032D0[0] < gUnknown_030032D0[x])
            gUnknown_030032D0[0] = gUnknown_030032D0[x];
    }

    *b = gUnknown_030032D0[5];

    if (gUnknown_030032D0[0] < gUnknown_030032D0[5])
        gUnknown_030032D0[0]++;

    *a = gUnknown_030032D0[0];
}

void sub_080219AC(void)
{
    int n;
    int x;
    int y;

    n = 0;

    sub_0801F92C(gMapData + 0x193A);
    sub_0801F838(0xff);

    for (x = 0; x <= 4; x++)
        gArmyRecords[x].unk11 = 0;

    for (y = 0; y < ((struct Unk219ACMap *)gMapData)->height; y++)
    {
        for (x = 0; x < ((struct Unk219ACMap *)gMapData)->width; x++)
        {
            switch (((struct Unk219ACMap *)gMapData)->plane[
                        ((struct Unk219ACMap *)gMapData)->rowOffset[y] + x] & 0x1f)
            {
            case 6:
            case 8:
            case 10:
            case 11:
            case 14:
            case 17:
            case 20:
                gArmyRecords[
                    ((struct Unk219ACMap *)gMapData)->plane[
                        ((struct Unk219ACMap *)gMapData)->rowOffset[y] + x] >> 5].unk11++;
                ((struct Unk219ACMap *)gMapData)->owner[
                    ((struct Unk219ACMap *)gMapData)->rowOffset[y] + x] = n;
                gPropertyList[n].unk00 =
                    ((struct Unk219ACMap *)gMapData)->plane[
                        ((struct Unk219ACMap *)gMapData)->rowOffset[y] + x] & 0x1f;
                gPropertyList[n].unk01 = x;
                gPropertyList[n].unk02 = y;
                gPropertyList[n].unk03[0] = 0;
                n++;
                break;
            }
        }
    }

    gPropertyList[n].unk00 = 0xff;
    gUnknown_03003FC0.unk47 = 0;

    n = 0;

    for (y = 0; y < ((struct Unk219ACMap *)gMapData)->height; y++)
    {
        for (x = 0; x < ((struct Unk219ACMap *)gMapData)->width; x++)
        {
            switch (((struct Unk219ACMap *)gMapData)->plane[
                        ((struct Unk219ACMap *)gMapData)->rowOffset[y] + x] & 0x1f)
            {
            case 16:
                if (gUnknown_03003FC0.unk01 != 5)
                    break;
            case 6:
            case 8:
            case 10:
            case 11:
            case 14:
            case 17:
            case 18:
            case 20:
            case 21:
            case 22:
            case 23:
            case 24:
            case 25:
                if (n <= 0x5b)
                {
                    gUnknown_03003150[n].flags =
                        ((struct Unk219ACMap *)gMapData)->plane[
                            ((struct Unk219ACMap *)gMapData)->rowOffset[y] + x];
                    gUnknown_03003150[n].x = x;
                    gUnknown_03003150[n].y = y;
                }

                n++;
                break;
            }
        }
    }

    if (n <= 0x5b)
        gUnknown_03003150[n].flags = 0xff;
}
