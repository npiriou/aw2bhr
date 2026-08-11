#include "global.h"

/* Promoted from assembly; each function below is byte-for-byte
 * identical to the original. Order is address order and must
 * stay that way -- the linker places this file's .text as one
 * contiguous block at 0x080852A8.
 * sub_080852A8 @ 0x080852A8
 */

struct Unk080852A8
{
    /* 0x00 */ u8 filler_00[0x66];
    /* 0x66 */ s16 unk66;
};

void sub_080852A8(struct Unk080852A8 *proc)
{
    if (gUnknown_03005940 <= 3)
        sub_08014668(1, 7, gUnknown_08499578,
                     gUnknown_08616F0C[gArmyRecords[proc->unk66].unk1d * 4
                                       + gUnknown_03005940],
                     0x8000, 0x3F);
    if (gUnknown_03005940 == 2)
    {
        sub_0801B780(0);
        sub_080149C0(3, 5, gUnknown_08499578,
                     gUnknown_08610A38[gCoDataTable[gArmyRecords[proc->unk66].unk1d]
                                           .unk38[1].unk00],
                     0x8000, 0);
    }
    if (gUnknown_03005940 == 3)
    {
        sub_0801B780(0);
        sub_080149C0(3, 5, gUnknown_08499578,
                     gUnknown_08610A38[gCoDataTable[gArmyRecords[proc->unk66].unk1d]
                                           .unk38[2].unk00],
                     0x8000, 0);
    }
    sub_08013AEC();
}
