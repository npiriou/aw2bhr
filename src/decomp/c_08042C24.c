#include "global.h"

/* Promoted from assembly; each function below is byte-for-byte
 * identical to the original. Order is address order and must
 * stay that way -- the linker places this file's .text as one
 * contiguous block at 0x08042C24.
 * sub_08042C24 @ 0x08042C24, sub_08042C68 @ 0x08042C68, sub_08042C9C @ 0x08042C9C, sub_08042CD4 @ 0x08042CD4, sub_08042CF8 @ 0x08042CF8, sub_08042D1C @ 0x08042D1C, sub_08042D50 @ 0x08042D50, sub_08042D84 @ 0x08042D84
 */

#include "proc.h"
/* The parent argument doubles as a tree NUMBER and a real ProcPtr, and the
 * signed `cmp #7` is what tells them apart -- hence the `(int)` cast, which a
 * pointer comparison would not give.
 *
 * The arms are written `<= 7` with Proc_Start first, so the ROM's `bgt` reaches
 * the else. That is the ORDINARY sense: these two arms REJOIN, and for
 * rejoining arms agbcc branches on the NEGATED condition to the else and leaves
 * the then inline. It is the opposite of what the same source shape does when
 * both arms `return` (see sub_08017CF0), where the else goes inline instead.
 *
 * unk64/unk66 are reached by advancing the base (`adds r0,#0x64` then
 * `adds r0,#2`) because 0x64 is past `strh`'s 5-bit displacement; unk2c/unk30
 * stay in the displacement, which is why only the halfword pair walks. */
struct Unk08042C24Proc
{
    /* 00 */ STRUCT_PAD(0x00, 0x2c);
    /* 2c */ int unk2c;
    /* 30 */ int unk30;
    /* 34 */ STRUCT_PAD(0x34, 0x4a);
    /* 4a */ u16 unk4a;
    /* 4c */ STRUCT_PAD(0x4c, 0x64);
    /* 64 */ u16 unk64;
    /* 66 */ u16 unk66;
};

void sub_08042C24(int a, int b, int c, int d, ProcPtr parent)
{
    struct Unk08042C24Proc *proc;

    if ((int)parent <= 7)
        proc = Proc_Start(gUnknown_0849FCA4, parent);
    else
        proc = Proc_StartBlocking(gUnknown_0849FCA4, parent);

    proc->unk64 = a;
    proc->unk66 = b;
    proc->unk2c = c;
    proc->unk30 = d;
    proc->unk4a = 0x1e;
}

/* The two bounds are bound to LOCALS, and that is what the ROM's compares need.
 * Written as literals, agbcc's tree-level `fold` canonicalises `>= 2` to `> 1`
 * and, knowing unk18 is a u8, picks unsigned branches -- `cmp #3; bhi` /
 * `cmp #1; bls`. Via int locals the comparison is typed `int` at tree level, so
 * neither the fold nor the range-narrowing fires, and constant propagation
 * still puts both values back in the `cmp` immediates. Exactly the lever
 * docs/agbcc-codegen.md records for sub_08044BA0 (wave 26, W26-B).
 *
 * Binding both is necessary: `hi` alone leaves the second compare unsigned. */
int sub_08042C68(int a, int b)
{
    int lo;
    int hi;

    hi = 3;
    lo = 2;

    if ((sub_08043050(a) & 0x80) != 0
        && gUnitTypeData[b].unk18 <= hi
        && gUnitTypeData[b].unk18 >= lo)
        return -2;

    return 0;
}

/* Both parameters are `int`, which corrects the `(u16, u8)` this function was
 * declared with -- see the note in unknown-functions.h. The prologue is a bare
 * `adds r4, r0, #0; adds r5, r1, #0` and both values live across two calls; the
 * narrow declaration adds four PROMOTE_MODE narrowing instructions. */
int sub_08042C9C(int a, int b)
{
    return Div(sub_080433D8(b)
        * (sub_08043270(gArmyRecords[a].unk1d, gArmyRecords[a].unk1e, b)
            + 0x64), 100);
}

/* The `lsls #4; subs; lsls #2` triple is the 0x3c stride of
 * struct ArmyRecord, and gArmyRecords is a pointer to it, so the pool
 * word is dereferenced once before the index is added. */
int sub_08042CD4(int a, int b)
{
    return sub_080430B0(gArmyRecords[a].unk1d, gArmyRecords[a].unk1e, b)
        + 0x64;
}

/* sub_08042CD4's twin over sub_08043120. */
int sub_08042CF8(int a, int b)
{
    return sub_08043120(gArmyRecords[a].unk1d, gArmyRecords[a].unk1e, b)
        + 0x64;
}

/* The one-argument call is the LEFT operand of the `+`: agbcc evaluates it
 * first and parks the result in r4, then builds the three-argument call. */
int sub_08042D1C(int a, int b)
{
    return sub_080433B8(b)
        + sub_08043190(gArmyRecords[a].unk1d, gArmyRecords[a].unk1e, b);
}

/* sub_08042D1C's twin over sub_080433C8 / sub_08043200. */
int sub_08042D50(int a, int b)
{
    return sub_080433C8(b)
        + sub_08043200(gArmyRecords[a].unk1d, gArmyRecords[a].unk1e, b);
}

/* The floor is written `if (n > 1) return n; else return 1;` and not the other
 * way round: both arms return, so agbcc emits the ELSE inline and branches to
 * the THEN on the condition being true, which is the ROM's `bgt` over an inline
 * `movs r0, #1`.
 *
 * gUnknown_03003FC0.unk2c is reached with `adds r0, #0x2c` on the base rather
 * than an `ldrb` displacement because 0x2c is past `ldrb`'s 5-bit field -- that
 * is addressing, not a member-array tell. */
int sub_08042D84(int a, int b)
{
    int n;

    n = sub_080433E8(b)
        + sub_080432A8(gArmyRecords[a].unk1d, gArmyRecords[a].unk1e);

    if (gUnknown_03003FC0.unk2c == 2)
        n--;

    if (n > 1)
        return n;
    else
        return 1;
}
