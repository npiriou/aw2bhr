#include "global.h"

/* Promoted from assembly; each function below is byte-for-byte
 * identical to the original. Order is address order and must
 * stay that way -- the linker places this file's .text as one
 * contiguous block at 0x08084BD4.
 * sub_08084BD4 @ 0x08084BD4
 */

#include "proc.h"
/* The proc's own fields. unk66 is SIGNED: it is written from the u16
 * gCurrentArmyIndex but read back `movs r1,#0; ldrsh r0,[r5,r1]` for
 * sub_08043834. The `adds rN,#0x64` / `#0x66` / `#0x4e` base bumps are not a
 * spelling choice -- `strh`'s immediate offset stops at 62. */
struct UnkBD4Proc
{
    /* 00 */ PROC_HEADER;
    /* 29 */ STRUCT_PAD(0x29, 0x4c);
    /* 4c */ u16 unk4c;
    /* 4e */ u16 unk4e;
    /* 50 */ STRUCT_PAD(0x50, 0x58);
    /* 58 */ u32 unk58;
    /* 5c */ STRUCT_PAD(0x5c, 0x64);
    /* 64 */ u16 unk64;
    /* 66 */ s16 unk66;
};

void sub_08084BD4(struct UnkBD4Proc *proc)
{
    proc->unk64 = 0;
    proc->unk66 = gCurrentArmyIndex;
    proc->unk4e = 0;
    proc->unk58 = 0;
    proc->unk4c = 0;
    sub_08085298(proc);
    sub_080852A8((struct Unk080852A8 *)proc);
    sub_08043834(proc->unk66);
}
