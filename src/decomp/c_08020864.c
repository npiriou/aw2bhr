#include "global.h"

/* Promoted from assembly; each function below is byte-for-byte
 * identical to the original. Order is address order and must
 * stay that way -- the linker places this file's .text as one
 * contiguous block at 0x08020864.
 * sub_08020864 @ 0x08020864, sub_080208C8 @ 0x080208C8
 */

/* Walks the other three factions in turn order, wrapping against 4 so slot 0 is
 * never visited, and reports whether any is both "agreeing" under sub_08020824
 * and active. The wrap is spelled out twice because the ROM computes it twice.
 *
 * `a + k` MUST be written out rather than bound to a local. With
 * `t = a + k; ... t <= 4 ? t : t - 4 ...` the ternary matches fold's
 * `A op B ? A : C` pattern -- the then-arm is literally the comparison's left
 * operand -- and fold inverts the condition and swaps the arms, so agbcc emits
 * the `t - 4` arm inline and the `t` arm past the branch. That is the ROM's two
 * ternaries reversed, and it costs 4 bytes. Written as `a + k` the condition's
 * operand and the arm are different trees, the pattern does not fire, and the
 * source order survives. `t > 4 ? t - 4 : t` is the SAME wrong output -- both
 * spellings normalise to it, so the arm order is not selectable by rewriting
 * the condition.
 *
 * The `lsls #0x10; lsrs #0x10` on sub_08020824's result is a u16 local at this
 * call site, not a narrow return type -- c_08020824.c returns `int`. */
int sub_08020864(u16 a)
{
    u16 k;
    u16 r;

    for (k = 1; k <= 3; k++)
    {
        r = sub_08020824(a, a + k <= 4 ? a + k : a + k - 4);
        if (r == 2 && gArmyRecords[a + k <= 4 ? a + k : a + k - 4].unk1b == 1)
            return 1;
    }
    return 0;
}

/* True when some OTHER live faction's allegiance byte differs from both the
 * subject's and the first such byte already seen -- i.e. when at least two
 * distinct foreign allegiances are on the board. `seen` and `seenValue` are the
 * two-state scan; the ROM's two blocks are the two arms of one `if (seen)`, and
 * the `unk1b == 1` guard is inside BOTH of them rather than hoisted above the
 * `if`, which is the order the branches appear in.
 *
 * gArmyRecords is named honestly here and the three-level chain
 * (`ldr rA,=<pool>; ldr rB,[rA]; ldr rC,[rB]`) reproduces on its own: agbcc
 * parks &gArmyRecords in this unit's .rodata, which is the ROM word at
 * 0x08090940 that gen_lds.py calls gUnknown_08090940. Declaring that word as a
 * `struct ArmyRecord **` global is NOT needed and would be wrong -- it is
 * agbcc's own address-constant pool entry. */
int sub_080208C8(int a)
{
    int k;
    int seen;
    int seenValue;

    if (gArmyRecords[gCurrentArmyIndex].unk1b == 1)
        return 0;

    seenValue = 0;
    seen = 0;
    for (k = 1; k <= 4; k++)
    {
        if (k == a)
            continue;
        if (!sub_080266DC(k))
            continue;
        if (seen)
        {
            if (gArmyRecords[k].unk1b == 1
             && seenValue != gArmyRecords[k].unk2a
             && gArmyRecords[a].unk2a != gArmyRecords[k].unk2a)
                return 1;
        }
        else
        {
            if (gArmyRecords[k].unk1b == 1
             && gArmyRecords[k].unk2a != gArmyRecords[a].unk2a)
            {
                seenValue = gArmyRecords[k].unk2a;
                seen = 1;
            }
        }
    }
    return 0;
}
