#include "global.h"

/* Promoted from assembly; each function below is byte-for-byte
 * identical to the original. Order is address order and must
 * stay that way -- the linker places this file's .text as one
 * contiguous block at 0x08010B34.
 * sub_08010B34 @ 0x08010B34, sub_08010D28 @ 0x08010D28, sub_08010D80 @ 0x08010D80, sub_08010DD4 @ 0x08010DD4
 */

/* See the note on struct Map in work/sub_08010604/sub_08010604.c. */
struct Map
{
    /* 0x0000 */ u16 unk00;
    /* 0x0002 */ u16 unk02;
    /* 0x0004 */ u16 unk04;
    /* 0x0006 */ u16 unk06;
    /* 0x0008 */ u8 filler_0008[0x0A];
    /* 0x0012 */ u8 unk0012[0x0508];
    /* 0x051A */ u8 unk051A[0x0508];
    /* 0x0A22 */ u16 unk0A22[0x0508];
    /* 0x1432 */ u8 unk1432[0x0A10];
    /* 0x1E42 */ u8 unk1E42[0x0508];
    /* 0x234A */ u8 unk234A[0x0508];
    /* 0x2852 */ u8 unk2852[0x0A10];
    /* 0x3262 */ u8 unk3262[0x0F18];
    /* 0x417A */ u16 unk417A[0x100];
};

/* Picks the connector tile for a road/bridge cell from which of its four
 * neighbours sub_0800F564 reports as joinable, after re-drawing the cell when
 * it is one of the two bridge ids. -1 means "leave it alone".
 *
 * The fourteen-way `||` chain must be written in EXACTLY this order: agbcc's
 * fold merges only the FIRST adjacent pair (0x142 / 0x143) into the range test
 * `(u16)(t - 0x142) <= 1`, because from the third term on the left operand is
 * a compound OR rather than a bare comparison. 0x140/0x141 are adjacent too
 * and are NOT merged, which is what fixes the order rather than the set.
 * The `adds r0,#1` / `subs r0,#0x1d` chain between compares is agbcc reusing
 * the previous constant's register, not source arithmetic.
 *
 * The cell is READ TWICE: the second read is a separate expression, not the
 * local `t`, because the fourteen-way merge is a control-flow join and agbcc's
 * CSE runs on extended basic blocks, so the ROM reloads.
 *
 * The word at 0x0808D8A4 is agbcc's -fforce-addr .rodata address constant
 * holding 0x08499590 (dereferenced in baserom.gba), not a global; the honest
 * spelling below reproduces it and promotion must carry the rodata entry. */
int sub_08010B34(int x, int y)
{
    u16 t;
    u16 u;

    t = ((struct Map *)gMapData)->unk0A22[
            ((struct Map *)gMapData)->unk417A[y] + x];
    if (t == 0x142 || t == 0x143 || t == 0x140 || t == 0x141 || t == 0x160
        || t == 0x161 || t == 0x162 || t == 0x163 || t == 0x122 || t == 0x123
        || t == 0x121 || t == 0x120 || t == 0x103 || t == 0x102)
    {
        u = ((struct Map *)gMapData)->unk0A22[
                ((struct Map *)gMapData)->unk417A[y] + x];
        if (u == 0x162 || u == 0x163)
        {
            if (sub_0800F8D4(x, y))
                return -1;
            sub_0800C608(x, y);
            sub_080011F4(x, y, 0xf);
            sub_08001158(x, y, sub_0800FD44(x, y, 1));
        }
        if (sub_0800F564(x, y, 0) == 2)
        {
            if (sub_0800F564(x, y, 1) == 2)
                return -1;
            if (sub_0800F564(x, y, 2) == 2)
                return -1;
            if (sub_0800F564(x, y, 3) == 2)
                return -1;
            return 0x121;
        }
        if (sub_0800F564(x, y, 1) == 2)
        {
            if (sub_0800F564(x, y, 0) == 2)
                return -1;
            if (sub_0800F564(x, y, 2) == 2)
                return -1;
            if (sub_0800F564(x, y, 3) == 2)
                return -1;
            return 0x120;
        }
        if (sub_0800F564(x, y, 2) == 2)
        {
            if (sub_0800F564(x, y, 0) == 2)
                return -1;
            if (sub_0800F564(x, y, 1) == 2)
                return -1;
            if (sub_0800F564(x, y, 3) == 2)
                return -1;
            return 0x103;
        }
        if (sub_0800F564(x, y, 3) == 2)
        {
            if (sub_0800F564(x, y, 0) == 2)
                return -1;
            if (sub_0800F564(x, y, 1) == 2)
                return -1;
            if (sub_0800F564(x, y, 2) == 2)
                return -1;
            return 0x102;
        }
    }
    return -1;
}

/* Eight redraw passes over one (x, y) cell, all sharing the same key. The
 * second is `sub_08001158(x, y, sub_0800FD44(x, y, 1))` -- the nested call
 * leaves its result in r0 and agbcc moves it to r2 before reloading r0/r1, so
 * the nesting costs the same `add r2,r0,#0` a temporary would. */
void sub_08010D28(int x, int y)
{
    sub_080011F4(x, y, 0xf);
    sub_08001158(x, y, sub_0800FD44(x, y, 1));
    sub_08010ADC(x, y);
    sub_0800A588(x, y);
    sub_0800ABD0(x, y);
    sub_08007F9C(x, y);
    sub_0800BEE4(x, y);
    sub_0800EC20(x, y);
}

/* Redraws one cell as a road/bridge, gated on either the campaign counter being
 * low or sub_0800C840 accepting the cell. sub_08010604 supplies the tile id
 * that both sub_08001158 and sub_0800C574 are handed.
 *
 * `(s8)gUnknown_0200B0B0->unk12` is a CAST on a u8 member, not an s8 member.
 *
 * Wave 48 (W48-A) corrects the REASON this comment used to give.  It read the
 * `ldrb; lsl #24; asr #24` here as proof of the u8 declaration, on the grounds
 * that an s8 member would have needed `movs rN,#0x12; ldrsb r0,[r1,rN]`
 * (ldrsb has no immediate form) -- and noted that sub_0800C574 does exactly
 * that with the same member, concluding "the two readers disagree, and this one
 * is the matched evidence".  They do not disagree: sub_0800C574 and
 * sub_0800C608 are both matched now, both read this same u8 member with the
 * same `(s8)` cast, and both get `ldrsb`.  One member, one spelling, two
 * outputs, three matches.
 *
 * So the choice between `ldrsb` and the shift pair is context, not signedness,
 * and neither form is evidence about the type.  That holds at a REGISTER offset
 * too: sub_0800C6E8 reads two u8 members four instructions apart at the same
 * `base + K + i` addressing under the same test and gets one of each.  What
 * does settle a byte member is an operation that folds under only one
 * signedness -- sub_0800C8D8's `|= 0xFF` -- see the
 * "`ldrsb` vs `ldrb; lsl #24; asr #24`" chapter of docs/agbcc-codegen.md.
 *
 * The u8 declaration itself is unaffected -- this file still matches. */
void sub_08010D80(int x, int y)
{
    int t;

    if ((s8)gUnknown_0200B0B0->unk12 <= 0x3b || sub_0800C840(x, y) != 0)
    {
        sub_080011F4(x, y, 0x10);
        t = sub_08010604(x, y);
        sub_08001158(x, y, t);
        sub_0800C574(x, y, t);
        sub_080219AC();
    }
}

/* "Is this bridge cell connected?" -- a horizontal bridge (0x142) is checked
 * against its left and right neighbours, a vertical one (0x143) against the
 * cells above and below, and anything else is accepted outright. A neighbour
 * counts when it is 0x162 or 0x163, which agbcc's range-test fold turns into
 * the `+ -0x162; lsl/lsr #16; cmp #1; bls` in the ROM.
 *
 * The `x + 1` neighbour is emitted as `(rowOffset[y] + 1) + x` and the `y + 1`
 * one as `rowOffset[y + 1] + x`; both are the plain inline spelling below --
 * binding the incremented coordinate to its own local blocks the
 * reassociation and costs a multiply. The map pointer is re-read after each
 * bounds test because those are control-flow merges and agbcc's CSE runs on
 * extended basic blocks, so nothing here needs a temporary to reproduce.
 *
 * `return 0` is the FUNCTION'S LAST STATEMENT and the "accept" arm is an
 * explicit `else return 1`. Written the other way round -- `if (!ok) return 0`
 * inside each arm with a shared trailing `return 1` -- agbcc emits exactly the
 * same 232 bytes with the two tail blocks SWAPPED (`bls`+`movs #0` where the
 * ROM has `bhi`+`movs #1`), worth 8 bytes across four branches. Measured both
 * ways; the source-level polarity of the last `if` alone does nothing, it is
 * which return is the fall-through at the end of the function that decides it.
 *
 * The word at 0x0808D8A8 is NOT a global: it is agbcc's own -fforce-addr
 * .rodata address constant and holds 0x08499590 in baserom.gba, so the honest
 * `gMapData` spelling below reproduces the two-level load and
 * promotion must carry a rodata entry for it (0x0808D8A4, used by
 * sub_08010B34, is a second private copy of the same address). */
int sub_08010DD4(int x, int y)
{
    u16 t;
    u16 u;

    t = ((struct Map *)gMapData)->unk0A22[
            ((struct Map *)gMapData)->unk417A[y] + x];
    if (t == 0x142)
    {
        if (x - 1 >= 0)
        {
            u = ((struct Map *)gMapData)->unk0A22[
                    ((struct Map *)gMapData)->unk417A[y] + (x - 1)];
            if (u == 0x162 || u == 0x163)
                return 1;
        }
        if (x + 1 >= ((struct Map *)gMapData)->unk00)
            return 0;
        u = ((struct Map *)gMapData)->unk0A22[
                ((struct Map *)gMapData)->unk417A[y] + (x + 1)];
        if (u == 0x162 || u == 0x163)
            return 1;
    }
    else if (t == 0x143)
    {
        if (y - 1 >= 0)
        {
            u = ((struct Map *)gMapData)->unk0A22[
                    ((struct Map *)gMapData)->unk417A[y - 1] + x];
            if (u == 0x162 || u == 0x163)
                return 1;
        }
        if (y + 1 >= ((struct Map *)gMapData)->unk02)
            return 0;
        u = ((struct Map *)gMapData)->unk0A22[
                ((struct Map *)gMapData)->unk417A[y + 1] + x];
        if (u == 0x162 || u == 0x163)
            return 1;
    }
    else
    {
        return 1;
    }
    return 0;
}
