#include "global.h"

/* Promoted from assembly; each function below is byte-for-byte
 * identical to the original. Order is address order and must
 * stay that way -- the linker places this file's .text as one
 * contiguous block at 0x08061E54.
 * sub_08061E54 @ 0x08061E54
 */

/* A pointer-parameter type. +0x00 is the gUnitTypeData row index (`ldrb`,
 * then `* 0x5c`), +0x06 carries a seven-bit field at bit 0 (`ldrb; lsls #0x19;
 * lsrs #0x19`) and +0x09 has three bits at bit 0 cleared (`mov #8; neg; and`,
 * mask ~0x07) -- the same +0x09 field sub_08061E80 clears.
 */
struct Unk61E54
{
    /* 00 */ u8 unk00;
    /* 01 */ u8 filler_01[0x06 - 0x01];
    /* 06 */ u8 unk06_0 : 7;
    /* 06 */ u8 unk06_7 : 1;
    /* 07 */ u8 filler_07[0x09 - 0x07];
    /* 09 */ u8 unk09_0 : 3;
    /* 09 */ u8 unk09_3 : 5;
};

void sub_08061E54(struct Unk61E54 *p)
{
    if (gUnitTypeData[p->unk00].unk10 - 5 < p->unk06_0)
        p->unk09_0 = 0;
}
