#include "global.h"

/* Promoted from assembly; each function below is byte-for-byte
 * identical to the original. Order is address order and must
 * stay that way -- the linker places this file's .text as one
 * contiguous block at 0x08020984.
 * sub_08020984 @ 0x08020984, sub_08020B88 @ 0x08020B88
 */

/* Recomputes every army's gArmyRecords unk1c "stance" byte for the current
 * army gCurrentArmyIndex: clear all five, seed the current one from the
 * gUnknown_08090944 table (upgraded to 3 when sub_08020864 agrees), propagate it
 * to every army sub_08020824 rates 2, then -- unless sub_080208C8 vetoes -- paint
 * the complement (v ^ 3) onto the rated-1 live armies and their own rated-2
 * neighbours (`v ^ 3` written at both uses -- the ROM's copy of it sits AFTER
 * the loop's `i = 1` init in the preheader, which is where LICM puts a hoist and
 * not where a source statement would land). Finally flag the current army with bit 1 if no live army is
 * already occupied or flagged.
 *
 * The `cur + k <= 4 ? cur + k : cur + k - 4` wrap is bound to a local in the
 * outer loops (the ROM computes it once into r5) and written out TWICE in the
 * innermost one, which is c_08020864.c's documented case. */
void sub_08020984(void)
{
    u16 i;
    u16 j;
    u16 t;
    u16 v;

    gArmyRecords[0].unk1c = 0;
    for (i = 1; i <= 4; i++)
        gArmyRecords[i].unk1c = 0;

    v = gUnknown_08090944[gArmyRecords[gCurrentArmyIndex].unk1b];
    if (v == 1 && (u8)sub_08020864(gCurrentArmyIndex))
        v = 3;
    gArmyRecords[gCurrentArmyIndex].unk1c = v;

    for (i = 1; i <= 3; i++)
    {
        t = gCurrentArmyIndex + i <= 4
          ? gCurrentArmyIndex + i
          : gCurrentArmyIndex + i - 4;
        if ((u16)sub_08020824(gCurrentArmyIndex, t) == 2)
            gArmyRecords[t].unk1c = v;
    }

    if (!(u8)sub_080208C8(gCurrentArmyIndex))
    {
        for (i = 1; i <= 3; i++)
        {
            t = gCurrentArmyIndex + i <= 4
              ? gCurrentArmyIndex + i
              : gCurrentArmyIndex + i - 4;
            if ((u16)sub_08020824(gCurrentArmyIndex, t) == 1
             && gArmyRecords[t].unk1b == 1)
            {
                gArmyRecords[t].unk1c = v ^ 3;
                for (j = 1; j <= 3; j++)
                {
                    if ((u16)sub_08020824(t, t + j <= 4 ? t + j : t + j - 4) == 2)
                        gArmyRecords[t + j <= 4 ? t + j : t + j - 4].unk1c = v ^ 3;
                }
            }
        }
    }

    for (i = 1; i <= 5; i++)
    {
        if (i > 4)
        {
            gArmyRecords[gCurrentArmyIndex].unk1c |= 2;
            break;
        }
        if (sub_080266DC(i) && gArmyRecords[i].unk1b == 1)
            break;
        if (sub_080266DC(i) && (gArmyRecords[i].unk1c & 2))
            break;
    }

    gUnknown_03004070 = 0;
    gUnknown_03004088 = 1;
}

/* Overlay writer over the Manhattan disk of radius `r` around (x, y): the outer
 * loop walks yy from y-r to y+r with a parallel s16 `dy` running -r..+r, and the
 * inner loop walks xx over x-r+|dy| .. x+r-|dy|.
 *
 * `dy` is a real source variable, not a strength-reduced giv, and its `-r` init
 * is the function's FIRST statement: the ROM negates the raw `r << 16` in place
 * (`negs r0, r5; lsrs r6, r0, #0x10`, two instructions off the same shift the
 * sign extension of `r` needs), which only happens while `r` has no
 * sign-extended form yet. Written after `yy = y - r` it costs a third
 * instruction and negates the sign-extended copy instead.
 *
 * `rows` must be its own local or agbcc reassociates the 0x417A constant to
 * last -- c_0805AD90.c's rule, and it applies to the `p + 0x12` unit-id plane
 * here for the same reason. */
void sub_08020B88(s16 x, s16 y, s16 r, s16 v)
{
    s16 xx;
    s16 yy;
    s16 dy;
    struct Unk02028360 *u;
    u8 *p;
    u8 *rows;
    u8 *ids;
    int ty;
    int m;
    int n;
    int t;

    dy = -r;

    for (yy = y - r; yy <= y + r; yy++, dy++)
    {
        if (yy < 0)
            continue;
        if (yy >= *(u16 *)(gMapData + 2))
            continue;

        for (xx = x - r + (dy < 0 ? -dy : dy);
             xx <= x + r - (dy < 0 ? -dy : dy);
             xx++)
        {
            if (xx < 0)
                continue;
            if (xx >= *(u16 *)gMapData)
                continue;

            u = sub_0803DF54(xx, yy);
            m = gUnknown_020288B4[(p = gMapData,
                                   ty = yy * 2,
                                   rows = p + 0x417a,
                                   *(u16 *)(rows + ty) + xx)];
            n = 0;
            if (u != NULL)
                n = u->unk02_6;
            if (m)
                n = 6;

            if (n)
            {
                sub_080251BC(gUnknown_03003F38, 0, &gUnknown_03003100.pos);
                if (((struct Unk030013D0 *)gUnknown_030013D0)->unk18 != 0)
                    gUnknown_03003340[yy][xx] = v;
            }
            else
            {
                ids = p + 0x12;
                t = ids[*(u16 *)(rows + ty) + xx];
                if (t == 0)
                    continue;
                if (sub_08026F9C(gUnknown_03003F38, t) == 1)
                    continue;
                sub_080251BC(gUnknown_03003F38, t, &gUnknown_03003100.pos);
                if (((struct Unk030013D0 *)gUnknown_030013D0)->unk18 != 0)
                    gUnknown_03003340[yy][xx] = v;
            }
        }
    }
}
