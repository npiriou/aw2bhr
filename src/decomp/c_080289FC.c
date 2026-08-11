#include "global.h"

/* Promoted from assembly; each function below is byte-for-byte
 * identical to the original. Order is address order and must
 * stay that way -- the linker places this file's .text as one
 * contiguous block at 0x080289FC.
 * sub_080289FC @ 0x080289FC, sub_08028A68 @ 0x08028A68, sub_08028AEC @ 0x08028AEC, sub_08028B70 @ 0x08028B70
 */

/* "Is army a1 the last one standing among the armies sharing its +0x2a team
 * byte?" -- gated on gUnknown_03003FC0.unk04 bit 2, and answering TRUE (the
 * fall-through) whenever the gate is closed.
 *
 * The `&&` chain is what puts the gArmyRecords pool word INSIDE the loop:
 * the deref is written in place, once per iteration, and nothing hoists it
 * because the `bl sub_080266DC` in the same body clobbers it. r7 and r6 are
 * strength_reduce's two givs for `a1 * 0x3c` and `i * 0x3c`; only r6 is
 * initialised in the preheader (`movs r6,#0x3c`, i.e. i = 1) and neither is
 * source.
 *
 * `i` is `int`: the increment is a bare `adds r4,#1` and the exit test a signed
 * `ble` with no narrowing anywhere, while the two sub_080266DC arguments carry
 * the `lsls #0x18; lsrs #0x18` that function's u8 parameter demands. */
bool8 sub_080289FC(int a1)
{
    int i;

    if (gUnknown_03003FC0.unk04 & 4)
    {
        if (!sub_080266DC(a1))
            return FALSE;

        for (i = 1; i <= 4; i++)
        {
            if (a1 != i
             && gArmyRecords[a1].unk2a == gArmyRecords[i].unk2a
             && !sub_080266DC(i))
                return FALSE;
        }
    }

    return TRUE;
}

/* Runs the four per-army end-of-turn predicates over slots 1..4 and reports the
 * first one that fails to sub_08028894, with a reason code (8, 0x10, 1, 2).
 * sub_08028AEC is the same function against sub_08028874 -- the two instruction
 * streams differ in nothing but that callee, at all four call sites.
 *
 * `i` is s16 and that is what the whole body's shift pattern is: agbcc keeps the
 * induction variable as `i << 16` in r6 and re-derives every argument from it
 * per the callee's own parameter type -- `lsls/lsrs #0x18` for sub_080266DC's
 * u8, `lsrs #0x10` for the three u16 predicates, `asrs #0x10` for sub_080289BC's
 * and sub_08028894's `int`. Nothing here is a mask and nothing is authored: the
 * `movs r1,#0x80; lsls #9` at the bottom is the loop optimiser adding 0x10000 to
 * the shifted biv, i.e. `i++`.
 *
 * r4 is a COUNTER, not a walking bitmask -- it holds 1,2,3,4,5 and the bottom
 * adds one, not doubles. Its reuse for the sign-extended copy inside the body is
 * register allocation reclaiming a dead range, not a second variable. */
void sub_08028A68(void)
{
    s16 i;

    for (i = 1; i <= 4; i++)
    {
        if (sub_080266DC(i))
        {
            if (!sub_08028904(i))
                sub_08028894(i, 8);
            else if (!sub_080289BC(i))
                sub_08028894(i, 0x10);
            else if (!sub_08028990(i))
                sub_08028894(i, 1);
            else if (!sub_08028944(i))
                sub_08028894(i, 2);
        }
    }
}

/* Byte-for-byte the same function as sub_08028A68 with sub_08028874 in place of
 * sub_08028894 at all four call sites -- 132 bytes and 56 instructions each,
 * differing in nothing else, not a register, an immediate or a branch. See the
 * comment on sub_08028A68 for the readout of the s16 induction variable. */
void sub_08028AEC(void)
{
    s16 i;

    for (i = 1; i <= 4; i++)
    {
        if (sub_080266DC(i))
        {
            if (!sub_08028904(i))
                sub_08028874(i, 8);
            else if (!sub_080289BC(i))
                sub_08028874(i, 0x10);
            else if (!sub_08028990(i))
                sub_08028874(i, 1);
            else if (!sub_08028944(i))
                sub_08028874(i, 2);
        }
    }
}

/* "Which army slot is first at or past gUnknown_03003FC0.unk31?" -- a linear
 * scan of slots 1..4 returning the slot number, or 0 both when unk31 is zero
 * and when nothing reaches it.
 *
 * `i` is u8, not int: the increment is `adds #1; lsls #0x18; lsrs #0x18` and the
 * exit test is an UNSIGNED `bls`, and the returned value needs no widening
 * because the counter already lives zero-extended. The compare against unk31 is
 * `bhs`, unsigned on both sides, which is what types the +0x11 member u8.
 *
 * The `ldr r3, [gArmyRecords]` in the preheader is the loop optimiser's
 * LICM hoist of the pointer deref, not source -- the body is written with the
 * ordinary `gArmyRecords[i]` subscript. */
int sub_08028B70(void)
{
    u8 i;

    if (gUnknown_03003FC0.unk31 == 0)
        return 0;

    for (i = 1; i <= 4; i++)
    {
        if (gArmyRecords[i].unk11 >= gUnknown_03003FC0.unk31)
            return i;
    }

    return 0;
}
