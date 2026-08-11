#include "global.h"

/* Promoted from assembly; each function below is byte-for-byte
 * identical to the original. Order is address order and must
 * stay that way -- the linker places this file's .text as one
 * contiguous block at 0x08045564.
 * sub_08045564 @ 0x08045564
 */

/* unk2c is an `int` read three different ways and all three are the same
 * member: `ldrsh` for sub_08025CC8's s16 parameter (agbcc narrows the load for
 * a signed-halfword conversion), `ldr` plus `lsls #0x18; lsrs #0x18` for the
 * u8 one (it does NOT narrow for a byte conversion), and `ldr`/`str` for the
 * increment. Reading them as three fields is the trap -- the `str` at the tail
 * proves the member is a word. */
struct Unk45564Proc
{
    /* 0x00 */ u8 filler_00[0x2c];
    /* 0x2c */ int unk2c;
    /* 0x30 */ int unk30;
    /* 0x34 */ u8 filler_34[0x20];
    /* 0x54 */ int unk54;
};

void sub_08045564(struct Unk45564Proc *proc)
{
    u8 x;
    u8 y;
    u8 flag;

    sub_08025CC8(proc->unk2c, proc->unk30, proc->unk54)->unk04_0 = 0x5a;

    x = proc->unk2c;
    y = proc->unk30;
    flag = 0;

    if (gArmyRecords[gCurrentArmyIndex].unk1f == 2)
        flag = 1;

    sub_08044B08(x, y, flag);
    proc->unk2c++;
}
