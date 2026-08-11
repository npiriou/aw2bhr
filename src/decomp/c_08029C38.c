#include "global.h"

/* Promoted from assembly; each function below is byte-for-byte
 * identical to the original. Order is address order and must
 * stay that way -- the linker places this file's .text as one
 * contiguous block at 0x08029C38.
 * sub_08029C38 @ 0x08029C38, sub_08029CB8 @ 0x08029CB8
 */

/* `proc->unk2c` must NOT be bound to a local first: as `d = proc->unk2c;` its
 * `ldr` is emitted ahead of the unk24 load, where the ROM loads unk24 first.
 * Written twice and left to CSE it lands in the ROM's order and still shares
 * the one register across both arms. */
struct Unk29C38Proc
{
    /* 0x00 */ u8 filler_00[0x24];
    /* 0x24 */ int unk24;
    /* 0x28 */ int unk28;
    /* 0x2c */ int unk2c;
    /* 0x30 */ u8 unk30;
};
/* The proc slot sub_080152EC returns is NOT struct Unk03001470: that type has
 * u16 at +0x24, and this function writes +0x24 and +0x28 with full-word `str`.
 * Declared locally and cast onto the result, the same shape c_0803F2F4.c uses.
 *
 * a2 is never read -- r1 is overwritten with the literal 0 for sub_080152EC
 * before anything touches it -- so its WIDTH is invisible here and is settled
 * at the only call site: sub_0802A38C loads gCurrentArmyIndex (a u16 global)
 * with `ldrb`, which is the u8 conversion of a halfword on a little-endian
 * target. It cannot be dropped either, since a3 arrives in r2 and a4 in r3. */
struct Unk29CB8Proc
{
    /* 0x00 */ u8 filler_00[0x20];
    /* 0x20 */ u16 unk20;
    /* 0x22 */ u16 unk22;
    /* 0x24 */ int unk24;
    /* 0x28 */ int unk28;
    /* 0x2c */ u8 filler_2c[0x04];
    /* 0x30 */ u8 unk30;
};

void sub_08029C38(struct Unk29C38Proc *proc)
{
    proc->unk24 -= proc->unk2c;

    if (proc->unk24 > 0)
    {
        if (proc->unk30 == 1)
            gArmyRecords[gCurrentArmyIndex].unk00 -= proc->unk2c;

        return;
    }

    if (proc->unk30 == 1)
        gArmyRecords[gCurrentArmyIndex].unk00 = proc->unk28;

    sub_080272B4();
    sub_08015328(gUnknown_03001FBC);
    sub_0803B4DC(0x6c);
}

void sub_08029CB8(struct Unk802C57C *a1, u8 a2, int a3, u8 a4)
{
    struct Unk29CB8Proc *proc;

    proc = (struct Unk29CB8Proc *)sub_080152EC(gUnknown_0849A0A8, 0);
    proc->unk24 = a3;
    proc->unk28 = gArmyRecords[gCurrentArmyIndex].unk00 - a3;
    proc->unk20 = a1->unk00;
    proc->unk22 = a1->unk02;

    if (gUnknown_03003FC0.unk47 != 0)
        proc->unk30 = 0;
    else
        proc->unk30 = a4;
}
