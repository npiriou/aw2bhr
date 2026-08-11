#include "global.h"

/* Promoted from assembly; each function below is byte-for-byte
 * identical to the original. Order is address order and must
 * stay that way -- the linker places this file's .text as one
 * contiguous block at 0x080293C8.
 * sub_080293C8 @ 0x080293C8, sub_08029490 @ 0x08029490
 */

#include "hardware.h"
#include "proc.h"
/* gUnknown_08090B68 is an agbcc -fforce-addr address word: the ROM holds
 * 0x03002EE0 there, which aw2bhr.map names `gpKeySt`. Naming gpKeySt directly
 * reproduces the three-level read. */
/* WAVE 35: CANONICAL `struct Map`. Eight drafts across blocks 0x08029-0x0802B
 * each invented their own body for this tag, with 2 to 7 named fields. Every
 * one compiles and byte-matches ALONE, so trymatch cannot see the problem;
 * merged into a promoted unit it is a hard `redefinition of struct Map`.
 * All seven distinct bodies describe the SAME layout at different
 * resolutions -- the offsets reconcile exactly -- so this is their union, the
 * most refined reading of each region. Byte-neutral: only a field's START
 * OFFSET enters the address arithmetic, never its declared length, and no
 * draft referenced any filler. Keep the drafts in sync; sync_work.py
 * reintroduces whatever the drafts say. */
struct Map
{
    /* 0x0000 */ u16 unk00;
    /* 0x0002 */ u16 unk02;
    /* 0x0004 */ u16 unk04;
    /* 0x0006 */ u16 unk06;
    /* 0x0008 */ u8 filler_0008[0x0A];
    /* 0x0012 */ u8 unk0012[0x0508];
    /* 0x051A */ u8 unk051A[0x0F18];
    /* 0x1432 */ u8 unk1432[0x0A10];
    /* 0x1E42 */ u8 unk1E42[0x0508];
    /* 0x234A */ u8 unk234A[0x0508];
    /* 0x2852 */ u8 unk2852[0x1928];
    /* 0x417A */ u16 unk417A[0x100];
};

void sub_080293C8(ProcPtr proc)
{
    sub_08023824();
    sub_080236E8();
    sub_08023908(4);

    if (sub_08029490(proc))
    {
        sub_08023274(2);

        if (gpKeySt->held & 1)
        {
            Proc_End(proc);
            gUnknown_03003F40 = ((struct Map *)gMapData)->unk051A[
                ((struct Map *)gMapData)->unk417A[gUnknown_030033E4.unk02]
                + gUnknown_030033E4.unk00];
            sub_08028EE4();
            return;
        }
    }
    else
    {
        sub_08023274(1);
    }

    if (gpKeySt->held & 2)
    {
        gUnknown_030033E4.unk00 = gUnknown_03003100.pos.unk00;
        gUnknown_030033E4.unk02 = gUnknown_03003100.pos.unk02;
        gUnknown_03003F40 = -1;
        Proc_End(proc);
        sub_08028EE4();
    }
}

bool8 sub_08029490(ProcPtr proc)
{
    int idx;

    idx = ((struct Map *)gMapData)->unk417A[gUnknown_030033E4.unk02]
        + gUnknown_030033E4.unk00;

    if ((s8)((struct Map *)gMapData)->unk2852[idx] >= 0)
    {
        if (((struct Map *)gMapData)->unk1E42[idx] == 0)
            return 1;

        if (((struct Map *)gMapData)->unk0012[idx] != 0
            && sub_08026F5C(((struct Map *)gMapData)->unk0012[idx]))
            return 1;
    }

    return 0;
}
