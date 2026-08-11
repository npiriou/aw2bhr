#include "global.h"

/* Promoted from assembly; each function below is byte-for-byte
 * identical to the original. Order is address order and must
 * stay that way -- the linker places this file's .text as one
 * contiguous block at 0x08045448.
 * sub_08045448 @ 0x08045448, sub_08045460 @ 0x08045460, sub_08045478 @ 0x08045478
 */

#include "proc.h"

struct Unk45448Proc
{
    /* 00 */ PROC_HEADER;
    /* 29 */ STRUCT_PAD(0x29, 0x54);
    /* 54 */ int unk54;
};
#include "proc.h"

struct Unk45460Proc
{
    /* 00 */ PROC_HEADER;
    /* 29 */ STRUCT_PAD(0x29, 0x54);
    /* 54 */ int unk54;
};
struct Unk45478
{
    /* 0x00 */ u8 filler_00[0x2c];
    /* 0x2c */ int unk2c;
    /* 0x30 */ int unk30;
};

void sub_08045448(ProcPtr parent)
{
    struct Unk45448Proc *proc = Proc_StartBlocking(gUnknown_084A09CC, parent);

    proc->unk54 = 1;
}

void sub_08045460(ProcPtr parent)
{
    struct Unk45460Proc *proc = Proc_StartBlocking(gUnknown_084A09CC, parent);

    proc->unk54 = 2;
}

void sub_08045478(struct Unk45478 *p)
{
    p->unk2c = 0;
    p->unk30 = 0;
    sub_08044AB8(gCurrentArmyIndex);
}
