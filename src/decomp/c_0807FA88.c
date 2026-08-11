#include "global.h"

/* Promoted from assembly; each function below is byte-for-byte
 * identical to the original. Order is address order and must
 * stay that way -- the linker places this file's .text as one
 * contiguous block at 0x0807FA88.
 * sub_0807FA88 @ 0x0807FA88
 */

#include "proc.h"
struct Unk0807FA88
{
    /* 0x00 */ u8 filler_00[0x4c];
    /* 0x4c */ u16 unk4c;
    /* 0x4e */ u8 filler_4e[0x58 - 0x4e];
    /* 0x58 */ int unk58;
};

void sub_0807FA88(struct Unk0807FA88 *proc)
{
    proc->unk58 = sub_0807F8FC(
        gUnknown_08610A38[gCoDataTable[gUnknown_03005970].unk38[gUnknown_03005904].unk00],
        gUnknown_08616750, proc);
    proc->unk4c = 0;
}
