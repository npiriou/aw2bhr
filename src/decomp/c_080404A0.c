#include "global.h"

/* Promoted from assembly; each function below is byte-for-byte
 * identical to the original. Order is address order and must
 * stay that way -- the linker places this file's .text as one
 * contiguous block at 0x080404A0.
 * sub_080404A0 @ 0x080404A0
 */

#include "proc.h"
/* The 0x0849FB8C proc's init: it converts the cell pair at unk2c/unk30 IN PLACE
 * into the pixel pair `cell * 16 + halfTile` -- the same conversion the matched
 * sub_08040590 writes into the separate unk54/unk58 -- then installs the
 * animation and plays a sound.
 *
 * Because the `+ 8` and `+ 0x10` are stored back into unk2c/unk30 BEFORE the
 * screen origin is subtracted, no `A - (B - 8)` association arises here and the
 * ROM simply reuses the stored values in r1/r2 rather than reloading them.
 * That is the same reason the parked sub_08040150 next door is hard: it does
 * the same arithmetic without the intervening store.
 *
 * sub_0801C70C takes SIX arguments, the last two on the stack (`str` to sp+0
 * and sp+4 with 0 and 1); unk4a is the packed `tile | pal << 12` halfword that
 * sub_0804046C builds and that is copied into the sprite's +0x22 below. */
struct Unk404A0Proc
{
    /* 00 */ u8 filler_00[0x2c];
    /* 2c */ int unk2c;
    /* 30 */ int unk30;
    /* 34 */ u8 filler_34[0x16];
    /* 4a */ u16 unk4a;
    /* 4c */ u8 filler_4c[0x04];
    /* 50 */ struct Unk0801C210 *unk50;
};

void sub_080404A0(struct Unk404A0Proc *proc)
{
    proc->unk2c = (proc->unk2c << 4) + 8;
    proc->unk30 = (proc->unk30 << 4) + 0x10;

    sub_0801C70C((void *)gUnknown_08111D94,
                 proc->unk2c - *(s16 *)(gMapData + 4),
                 proc->unk30 - *(s16 *)(gMapData + 6),
                 proc->unk4a, 0, 1);

    proc->unk50 = sub_0801C210((void *)gUnknown_08111D94, 1, 1);
    sub_0801C4D4(proc->unk50, 1);
    proc->unk50->unk22 = proc->unk4a;

    sub_0803B4DC(0x1C6);
}
