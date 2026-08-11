#include "global.h"

/* Promoted from assembly; each function below is byte-for-byte
 * identical to the original. Order is address order and must
 * stay that way -- the linker places this file's .text as one
 * contiguous block at 0x08044994.
 * sub_08044994 @ 0x08044994
 */

struct Unk8044994
{
    /* 0x00 */ u8 filler_00[0x64];
    /* 0x64 */ s16 unk64;
    /* 0x66 */ s16 unk66;
};

void sub_08044994(struct Unk8044994 *proc)
{
    struct UnitRecord *e;
    int col;
    int row;

    if (proc->unk66 > 0)
    {
        e = &gUnitRecords[proc->unk66];
        col = e->unk02;
        row = e->unk03;

        sub_08044854(col, row - 2, proc->unk64);

        sub_08044854(col - 1, row - 1, proc->unk64);
        sub_08044854(col, row - 1, proc->unk64);
        sub_08044854(col + 1, row - 1, proc->unk64);

        sub_08044854(col - 2, row, proc->unk64);
        sub_08044854(col - 1, row, proc->unk64);
        sub_08044854(col, row, proc->unk64);
        sub_08044854(col + 1, row, proc->unk64);
        sub_08044854(col + 2, row, proc->unk64);

        sub_08044854(col - 1, row + 1, proc->unk64);
        sub_08044854(col, row + 1, proc->unk64);
        sub_08044854(col + 1, row + 1, proc->unk64);

        sub_08044854(col, row + 2, proc->unk64);
    }

    sub_08039F58();
}
