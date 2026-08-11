#include "global.h"

/* Promoted from assembly; each function below is byte-for-byte
 * identical to the original. Order is address order and must
 * stay that way -- the linker places this file's .text as one
 * contiguous block at 0x08079024.
 * sub_08079024 @ 0x08079024
 */

struct Unk8079024
{
    /* 0x00 */ u8 filler_00[0x2c];
    /* 0x2c */ int unk2c;
    /* 0x30 */ int unk30;
    /* 0x34 */ u8 filler_34[0x0c];
    /* 0x40 */ int unk40;
    /* 0x44 */ u8 filler_44[8];
    /* 0x4c */ s16 unk4c;
    /* 0x4e */ u8 filler_4e[4];
    /* 0x52 */ u16 unk52;
    /* 0x54 */ u8 filler_54[4];
    /* 0x58 */ int unk58;
    /* 0x5c */ int unk5c;
    /* 0x60 */ int unk60;
    /* 0x64 */ s16 unk64;
    /* 0x66 */ s16 unk66;
    /* 0x68 */ s16 unk68;
    /* 0x6a */ s16 unk6a;
};

void sub_08079024(struct Unk8079024 *p)
{
    int n;
    u16 *q;

    p->unk58 = gArmyRecords[sub_0807A908()].unk34;
    p->unk5c = gArmyRecords[sub_0807A908()].unk35;
    p->unk60 = gArmyRecords[sub_0807A908()].unk36;
    p->unk4c = 0;
    p->unk64 = 0;
    p->unk66 = 0;
    p->unk68 = 0;
    p->unk6a = p->unk58 + p->unk5c + p->unk60;
    p->unk2c = 0;
    p->unk30 = 0;
    n = gArmyRecords[sub_0807A908()].unk33;
    q = &p->unk52;
    *q = 5 - n;
    if (*q > 3)
        *q = 3;
    if (*q == 3)
        p->unk40 = 0x10;
    else
        p->unk40 = 0;
}
