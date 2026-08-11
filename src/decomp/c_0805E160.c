#include "global.h"

/* Promoted from assembly; each function below is byte-for-byte
 * identical to the original. Order is address order and must
 * stay that way -- the linker places this file's .text as one
 * contiguous block at 0x0805E160.
 * sub_0805E160 @ 0x0805E160
 */

/* Wave 52, W52-D.  MATCHED (332/332, relocs resolve to the same addresses).
 *
 * gUnknown_0816DA5C / _0816DA60 / _0816DA64 are NOT globals -- the ROM words
 * there hold 0x08499590, 0x030040D8 and 0x030013EC, so they are agbcc's own
 * -fforce-addr constants for gMapData, gUnknown_030040D8 and
 * gUnknown_030013EC.  The honest spelling reproduces all three and the
 * promotion must carry the three rodata words:
 *   "rodata": ["0x0816DA5C", "0x0816DA60", "0x0816DA64"]
 *
 * THE LOOP IS A GOTO LOOP, and that is the whole residual.  Written as a
 * `do { } while (r != 1)` the shape is otherwise byte-exact but LICM builds a
 * preheader out of three invariants -- the pooled 0x270F, `add rN,sp,#4` and
 * gUnknown_03004730's base -- which costs a third high callee-saved register
 * (the ROM pushes only r6/r7 for r8/sb) and pushes gUnknown_030013EC's
 * -fforce-addr pointer out of r5.  A goto loop has no NOTE_INSN_LOOP_BEG, LICM
 * never runs, and all of it comes back at once.  This is exactly the
 * measurement recorded on sub_0805B778 (wave 51, W51-E), which is this
 * function's twin: same scratch cell, same 0x270F seed, same
 * gUnknown_03003340 / sub_08058224 / sub_080591E4 tail.
 *
 * _call_via_r4 here is a FIVE-argument indirect call, not a four-argument one:
 * r4 is callee-saved, so the register index is not an arity readout (W46-D).
 *
 * The `(x & 0xc0) != 0` arm is written as the ELSE so that sub_0805B3F4 lands
 * after the body; an early `return` puts that block before the body instead.
 *
 * struct Unk5A514Cell is repeated here rather than declared in a shared header
 * because src/decomp/c_0805A268.c, c_0805A514.c and c_0805A744.c all define it
 * file-locally; sub_0805A514's promoted definition takes it, so the cast on
 * gUnknown_03003F20 is what keeps this unit in agreement with that definition.
 * See the note at sub_0805A5E0 in include/unknown-functions.h. */

struct Unk5A514Cell
{
    /* 0x00 */ u8 x;
    /* 0x01 */ u8 y;
    /* 0x02 */ s16 v;
};
void sub_0805A514(struct Unk5A514Cell *);
struct Map
{
    /* 0x0000 */ u16 unk00;
    /* 0x0002 */ u16 unk02;
    /* 0x0004 */ u16 unk04;
    /* 0x0006 */ u16 unk06;
    /* 0x0008 */ u8 filler_0008[0x0A];
    /* 0x0012 */ u8 unk0012[0x0508];
    /* 0x051A */ u8 unk051A[0x0F18];
    /* 0x1432 */ u8 unk1432[0x0A10];
    /* 0x1E42 */ u8 unk1E42[0x0508];
    /* 0x234A */ u8 unk234A[0x0508];
    /* 0x2852 */ u8 unk2852[0x1928];
    /* 0x417A */ u16 unk417A[0x100];
};
struct CellXY
{
    /* 0x00 */ u16 x;
    /* 0x02 */ u16 y;
};

void sub_0805E160(void)
{
    struct CellXY pos;
    struct Unk03003338 *list;
    u8 r;

    list = gUnknown_03003F20;
    sub_0801F92C(gMapData + 0x2852);

    if ((gUnknown_030040D8->unk07[2] & 0xc0) == 0)
    {
        gUnknown_030013EC(gUnknown_030040D8->unk02, gUnknown_030040D8->unk03,
                          gUnknown_030040D8->unk00, 0x78, 1);
        sub_0801FD9C(0x79);
        sub_0805A514((struct Unk5A514Cell *)list);
    loop:
        pos.x = 0x270F;
        sub_08059C00(list, (u16 *)&pos);
        if (pos.x == 0x270F)
        {
            sub_0805E2AC();
            return;
        }
        gUnknown_03004730[((struct Map *)gMapData)->unk0012[((struct Map *)gMapData)->unk417A[pos.y] + pos.x] & 0x3f]++;
        r = sub_0805ACA8(pos.x, pos.y, (u16 *)&pos);
        if (r != 1)
            goto loop;

        gUnknown_030013EC(pos.x, pos.y, gUnknown_030040D8->unk00, 0x78, 1);

        if ((s8)gUnknown_03003340[gUnknown_030040D8->unk03][gUnknown_030040D8->unk02] > sub_08058224((struct UnitRecord *)gUnknown_030040D8))
            sub_080591E4(&pos);
        else
            sub_0805D648(pos.x, pos.y, 2, 0, 0);
    }
    else
    {
        sub_0805B3F4();
    }
}
