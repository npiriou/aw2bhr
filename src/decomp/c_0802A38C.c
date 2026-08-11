#include "global.h"

/* Promoted from assembly; each function below is byte-for-byte
 * identical to the original. Order is address order and must
 * stay that way -- the linker places this file's .text as one
 * contiguous block at 0x0802A38C.
 * sub_0802A38C @ 0x0802A38C
 */

/* `bl _call_via_r3` is an ordinary indirect THUMB call, not an interworking
 * veneer. The register index does NOT count arguments here: the pointer
 * arrives as parameter 2 in r1 and is moved to r3 because r0 carries the one
 * argument.
 *
 * TWO binding locals, each worth a couple of bytes and neither obvious:
 *
 * (1) `t` -- the shifted half of the packed word must be its own statement.
 *     Written `p->unk02 | p->unk03 << 16` the instructions are identical but
 *     the unk02 load is scheduled ahead of the unk03 load and shift; written
 *     the other way round (`p->unk03 << 16 | p->unk02`) the order is right but
 *     the OR's destination moves to the shift's register. Only splitting the
 *     shift out gives the ROM's `ldrb; lsls; ldrb; orrs r1, r0`.
 *
 * (2) `q` -- binding gCurrentArmyIndex's ADDRESS before the indirect call is
 *     what makes it live ACROSS that call, so it takes a callee-saved register
 *     and its pool `ldr` is emitted at the top of the block, as in the ROM.
 *     Dereferenced in place the address is rematerialised into a scratch after
 *     the call instead. The load itself stays after the call either way.
 *
 * `ldrb r1, [r5]` on a u16 global is the u8 conversion of a halfword on a
 * little-endian target -- sub_08029CB8's unused second parameter is u8, and
 * this call site is the only thing in the tree that shows its width. */
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
struct Unk2A38C
{
    /* 0x00 */ u8 filler_00[0x02];
    /* 0x02 */ u8 unk02;
    /* 0x03 */ u8 unk03;
};

bool8 sub_0802A38C(struct Unk2A38C *p, int (*fn)(struct Unk2A38C *))
{
    const u16 *q;
    int r;
    int v;
    int t;

    if (((struct Map *)gMapData)->unk234A[
            ((struct Map *)gMapData)->unk417A[p->unk03] + p->unk02] == 0)
    {
        fn(p);
        return 0;
    }

    q = &gCurrentArmyIndex;
    r = fn(p);

    if (r == 0)
        return 0;

    t = p->unk03 << 16;
    v = p->unk02 | t;
    sub_08029CB8((struct Unk802C57C *)&v, *q, r, 0);
    return 1;
}
