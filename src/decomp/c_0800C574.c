#include "global.h"

/* Promoted from assembly; each function below is byte-for-byte
 * identical to the original. Order is address order and must
 * stay that way -- the linker places this file's .text as one
 * contiguous block at 0x0800C574.
 * sub_0800C574 @ 0x0800C574, sub_0800C608 @ 0x0800C608, sub_0800C6A8 @ 0x0800C6A8, sub_0800C6E8 @ 0x0800C6E8
 */

void sub_0800C574(int x, int y, int t)
{
    u8 *p;
    u8 *rows;
    u8 *plane;
    int n;
    int off;
    int u;
    int z;

    if ((s8)gUnknown_0200B0B0->unk12 > 0x5B)
        return;

    n = (s8)gUnknown_0200B0B0->unk12;

    p = gMapData;
    u = y * 2;
    rows = p + 0x417A;
    off = *(u16 *)(rows + u) + x;
    plane = p + 0x193A;
    plane[off] = (z = 0, n);

    gPropertyList[n].unk00 = t;
    gPropertyList[n].unk01 = x;
    gPropertyList[n].unk02 = y;
    gPropertyList[n].unk03[0] = z;
    gPropertyList[n + 1].unk00 = 0xFF;

    gUnknown_03003150[n].flags = t;
    gUnknown_03003150[n].x = x;
    gUnknown_03003150[n].y = y;
    n++;
    gUnknown_03003150[n].flags = -1;
    gUnknown_0200B0B0->unk12 = n;
}

/* The REMOVE half of the sub_0800C574 pair: drops the record for cell (x, y)
 * from the two parallel arrays and decrements the count sub_0800C574
 * increments.
 *
 * The cell byte comes off the +0x1432 terrain plane through the +0x417A row
 * table, the c_0800C840 idiom -- but UNMASKED here, where sub_0800C840 hands
 * sub_0800C7E8 `& 0x1f`.  sub_0800C7E8 masks internally, so the two spellings
 * agree; sub_0800C7A4 then switches the same raw byte over 0x28/0x48/0x68/0x88,
 * values outside the low five bits, so the mask genuinely is absent here.
 *
 * The scan has TWO exits and the bottom of the loop gives their order: the
 * `cmp r4,#0x5b; bgt` bound runs BEFORE the 0xFF sentinel re-check, so this is
 * a bounded `for` whose body `break`s and not a `while (flags != 0xFF)`.  The
 * index survives as a real variable rather than becoming a pointer giv only
 * because gPropertyList's 8-byte stride needs it (`lsls r0, r4, #3`); the
 * 4-byte array still gets the giv.
 *
 * `(s8)gUnknown_0200B0B0->unk12` on the u8 member emits `movs r0,#0x12;
 * ldrsb r0,[r1,r0]` -- see the note in src/decomp/c_08010B34.c, which this
 * function corrects: the cast folds to ldrsb whenever the address is
 * base+constant, and only leaves `ldrb; lsl; asr` behind when it is not.
 * The decrement one line above reads the SAME member with a plain `ldrb`
 * because a truncating store back needs no sign, and the reload between them
 * is that store aliasing the pointer global. */
void sub_0800C608(int x, int y)
{
    u8 *p;
    u8 *rows;
    u8 *tiles;
    int off;
    int cell;
    int t;
    int i;

    p = gMapData;
    t = y * 2;
    rows = p + 0x417A;
    off = *(u16 *)(rows + t) + x;
    tiles = p + 0x1432;
    cell = tiles[off];

    if (sub_0800C7E8(cell) == 2)
        sub_0800C7A4(cell);

    for (i = 0; i <= 0x5B; i++)
    {
        if (gUnknown_03003150[i].flags == 0xFF)
            break;
        if (gUnknown_03003150[i].flags != 0 && gUnknown_03003150[i].x == x
         && gUnknown_03003150[i].y == y)
        {
            gPropertyList[i].unk00 = 0;
            gUnknown_03003150[i].flags = 0;
            gUnknown_0200B0B0->unk12--;
            if ((s8)gUnknown_0200B0B0->unk12 < 0)
                gUnknown_0200B0B0->unk12 = 0;
        }
    }
}

/* Totals sub_0800C6E8 over all four 0x28/0x68/0x48/0x88 ids.  The two out
 * parameters are one shared pair of stack ints reused by every call -- the ROM
 * computes `add r5, sp, #4` once in the prologue and passes the same `sp` and
 * `r5` four times -- so the values are thrown away and only the count is kept.
 *
 * An accumulator, not a four-term sum: the ROM folds each result into r4 at
 * its own call (`adds r4, r0, #0` then three `adds r4, r4, r0`).  Written as
 * one `a + b + c + d` expression agbcc keeps all four results live to the end,
 * spending r6 and r8 and an extra push/pop pair.
 *
 * The id order is 0x28, 0x68, 0x48, 0x88 -- not the sorted order the switch in
 * the callee uses. */
int sub_0800C6A8(void)
{
    int x;
    int y;
    int n;

    n = sub_0800C6E8(0x28, &x, &y);
    n += sub_0800C6E8(0x68, &x, &y);
    n += sub_0800C6E8(0x48, &x, &y);
    n += sub_0800C6E8(0x88, &x, &y);

    return n;
}

int sub_0800C6E8(int a, int *b, int *c)
{
    int i;
    int ok;

    ok = 0;
    switch (a)
    {
    case 0x28:
        i = 0;
        break;
    case 0x48:
        i = 1;
        break;
    case 0x68:
        i = 2;
        break;
    case 0x88:
        i = 3;
        break;
    default:
        return ok;
    }

    if ((s8)gUnknown_0200B0B0->unk17[i] >= 0 && (s8)gUnknown_0200B0B0->unk1b[i] >= 0)
    {
        *b = (s8)gUnknown_0200B0B0->unk17[i];
        *c = (s8)gUnknown_0200B0B0->unk1b[i];
        ok = 1;
    }

    return ok;
}
