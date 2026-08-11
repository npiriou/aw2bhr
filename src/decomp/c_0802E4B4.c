#include "global.h"

/* Promoted from assembly; each function below is byte-for-byte
 * identical to the original. Order is address order and must
 * stay that way -- the linker places this file's .text as one
 * contiguous block at 0x0802E4B4.
 * sub_0802E4B4 @ 0x0802E4B4
 */

/* The cell fetch is the c_08008B70 / c_080094EC family expression: the screen
 * descriptor's u16 rowOffset[] lives at +0x417A and the byte cell array at
 * +0x12. Two things are load-bearing here and neither is guessable from the
 * listing:
 *
 *   - `sel = &gUnknown_03003F38` up front. Under -fforce-addr the destination
 *     of `gUnknown_03003F38 = ...` has its address forced into a register
 *     BEFORE the right-hand side is expanded, so the pool word for it lands
 *     ahead of gMapData's and the address survives the whole index
 *     computation in r8. Spelling the two later reads through the global
 *     instead puts that pool word after gMapData's and costs 45 bytes.
 *   - `p += idx` rather than `p[idx]`. Both are one `adds`, but the compound
 *     assignment accumulates into the pointer's own register (`adds r1,r1,r0;
 *     ldrb r0,[r1]`) while the subscript picks the index's (`adds r0,r1,r0;
 *     ldrb r0,[r0]`). Four bytes, twice.
 *
 * The whole address chain is recomputed after the sub_080242B0 call because
 * the call clobbers memory; only `sy * 2` and the two s16 casts survive as
 * common subexpressions, which is why they read as locals and p/rows do not.
 *
 * gUnknown_030040D8 is the same object as gUnitRecords[i] -- see the note
 * on struct Unk030040D8 in unknown-globals.h for why the cast is here rather
 * than in the global's type. */
void sub_0802E4B4(s16 x, s16 y)
{
    u8 *p;
    u8 *rows;
    u8 *sel;
    int t;
    int idx;
    s16 sx;
    s16 sy;

    sub_0801B780(0);
    gUnknown_030040DC = 0;
    gUnknown_030033E8[0] = 0;
    gUnknown_030033E8[1] = 0;
    gUnknown_03000558 = 0;

    sel = &gUnknown_03003F38;
    sx = x;
    sy = y;
    p = gMapData;
    t = sy * 2;
    rows = p + 0x417A;
    idx = *(u16 *)(rows + t) + sx;
    p += 0x12;
    p += idx;
    gUnknown_03003F38 = *p;
    gUnknown_030040D8 = (struct Unk030040D8 *)&gUnitRecords[*sel];

    if (sub_080242B0(sx, sy))
    {
        sub_0802D5E8(sx, sy);
        return;
    }

    p = gMapData;
    t = sy * 2;
    rows = p + 0x417A;
    idx = *(u16 *)(rows + t) + sx;
    p += 0x12;
    p += idx;
    if (*p == 0 || (gUnknown_030040D8->unk01 & 1))
    {
        sub_0802D458();
        return;
    }

    sub_08074320(gUnknown_030040D8);
    gUnknown_03003110[0] = 4;
    sub_08035584(gUnknown_030040D8);
    sub_08024454();
    sub_080258CC();
    gUnknown_03004480 = (*sel >> 6) + 1;
    sub_0801F92C(gMapData + 0x2852);
    sub_080202A4(gUnknown_030040D8);
    gUnknown_03004480 = gCurrentArmyIndex;
    sub_08022990((u16)sx, (u16)sy, 0);
    gUnknown_03003334 = 1;
    sub_08038C98();
    sub_0803B4DC(0x69);
}
