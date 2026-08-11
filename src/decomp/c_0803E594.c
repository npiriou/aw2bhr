#include "global.h"

/* Promoted from assembly; each function below is byte-for-byte
 * identical to the original. Order is address order and must
 * stay that way -- the linker places this file's .text as one
 * contiguous block at 0x0803E594.
 * sub_0803E594 @ 0x0803E594
 */

/* MATCHED (wave 49, W49-C), third attempt. 304/304 bytes, relocs match.
 *
 * Four rays out of (a1, a2) -- up, right, down, left -- appending every cell of
 * the +0x12 unit plane that is occupied. Two things were load-bearing and both
 * are measured, not stylistic:
 *
 * 1. THE PARAMETERS ARE THE SAVED COPIES AND THE LOCALS ARE THE LOOP VARIABLES,
 *    not the other way round. The ROM's prologue is `str r0,[sp]; str r1,[sp,#4]`
 *    -- the incoming argument registers stored straight to their homes, before
 *    the `adds r5,r0,#0` / `adds r4,r1,#0` copies -- and each loop preheader
 *    reloads exactly the one of the two the previous loop destroyed. Writing it
 *    the other way (locals `x0`/`y0` saved from mutated parameters) is
 *    size-exact at 92.1% with the two stores AFTER the copies and sourced from
 *    r5/r4. Writing four independent locals instead is worse still: a1 and a2
 *    then keep registers of their own, the loop variable takes a copy
 *    (`adds r4, r6, #0`) it does not have in the ROM, and a3 gets spilled where
 *    the ROM keeps it in sl for the whole function.
 *
 * 2. THE PLANE EXPRESSION IS SPELLED INLINE IN ALL FOUR LOOPS, and the ROM's two
 *    different-looking shapes come out of that ONE spelling. See the chapter
 *    added to docs/agbcc-codegen.md: where the loop variable is the ROW the byte
 *    survives in r0 and is copied (`adds r2,r0,#0`), where it is the COLUMN its
 *    own `(u16)` narrowing lands in r0 first and the byte is re-loaded
 *    (`ldrb r2,[r1]`). Binding it to a `t` local instead -- the c_0803E764
 *    spelling -- reproduces neither and costs the address register besides.
 */

struct Unk3E594Map
{
    /* 0x0000 */ u16 unk00;
    /* 0x0002 */ u16 unk02;
    /* 0x0004 */ u8 filler_0004[0x12 - 0x04];
    /* 0x0012 */ u8 unit[0x417A - 0x12];
    /* 0x417A */ u16 rowOffset[1];
};
#define MAP ((struct Unk3E594Map *)gMapData)

void sub_0803E594(int a1, int a2, int a3)
{
    int x;
    int y;

    x = a1;
    y = a2;
    for (; y >= 0; y--)
    {
        if (MAP->unit[MAP->rowOffset[y] + x] != 0)
            sub_0803E560(x, y, MAP->unit[MAP->rowOffset[y] + x], a3);
    }
    y = a2;
    for (; x < MAP->unk00; x++)
    {
        if (MAP->unit[MAP->rowOffset[y] + x] != 0)
            sub_0803E560(x, y, MAP->unit[MAP->rowOffset[y] + x], a3);
    }
    x = a1;
    for (; y < MAP->unk02; y++)
    {
        if (MAP->unit[MAP->rowOffset[y] + x] != 0)
            sub_0803E560(x, y, MAP->unit[MAP->rowOffset[y] + x], a3);
    }
    y = a2;
    for (; x >= 0; x--)
    {
        if (MAP->unit[MAP->rowOffset[y] + x] != 0)
            sub_0803E560(x, y, MAP->unit[MAP->rowOffset[y] + x], a3);
    }
}
