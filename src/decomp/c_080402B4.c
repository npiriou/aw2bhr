#include "global.h"

/* Promoted from assembly; each function below is byte-for-byte
 * identical to the original. Order is address order and must
 * stay that way -- the linker places this file's .text as one
 * contiguous block at 0x080402B4.
 * sub_080402B4 @ 0x080402B4, sub_08040380 @ 0x08040380
 */

#include "proc.h"
/* Reports the cell to sub_0803FF48 and then, for exactly two tile ids, rewrites
 * both map planes at that cell. The two `if`s are separate statements and not
 * an `else if` chain -- the ROM re-tests the snapshot after the first block
 * falls through rather than branching past it.
 *
 * The snapshot is taken BEFORE the sub_0803FF48 call and held in a
 * callee-saved register: gMapData is not const, so every store and
 * every call kills it, which is why the ROM reloads the base
 * (`ldr r1,[r6]`) for each of the four subsequent plane accesses while keeping
 * `y * 2`, 0x417A and 0xA22 live in r5, r8 and sb. That is the
 * `mov r7,sb; mov r6,r8` five-plus-live-values shape, not a loop.
 *
 * Plane addresses go through a struct laid over gMapData for the usual
 * reason: the ROM computes `(map + K) + idx`, which only a COMPONENT_REF
 * preserves. */
struct Unk402B4Map
{
    /* 0x0000 */ u8 filler_0000[0x0A22];
    /* 0x0A22 */ u16 unkA22[0x508];
    /* 0x1432 */ u8 terrain[0x2D48];
    /* 0x417A */ u16 rowOffset[1];
};
/* The sibling of sub_080402B4 next door: same snapshot-then-rewrite shape on
 * the two map planes, but keyed on tile 0x180 and driving the 0x0849FB8C
 * loader pair (sub_08040430 / sub_0804046C) instead of sub_0803FF48.
 *
 * 0x1CA is written twice and CSEd into r4 across both calls. The snapshot is
 * taken BEFORE them because gMapData is not const and every call kills
 * it -- which is also why the base is reloaded for each plane access afterwards
 * while `y * 2` and the plane constants stay live in sb/r8.
 *
 * Plane addresses go through a struct laid over gMapData: the ROM
 * computes `(map + K) + idx`, which only a COMPONENT_REF preserves. */
struct Unk40380Map
{
    /* 0x0000 */ u8 filler_0000[0x0A22];
    /* 0x0A22 */ u16 unkA22[0x508];
    /* 0x1432 */ u8 terrain[0x2D48];
    /* 0x417A */ u16 rowOffset[1];
};

void sub_080402B4(int x, int y, ProcPtr parent)
{
    u16 v = ((struct Unk402B4Map *)gMapData)->unkA22[
        ((struct Unk402B4Map *)gMapData)->rowOffset[y] + x];

    sub_0803FF48(x, y, -3, parent);

    if (v == 0x162)
    {
        ((struct Unk402B4Map *)gMapData)->terrain[
            ((struct Unk402B4Map *)gMapData)->rowOffset[y] + x] = 1;
        ((struct Unk402B4Map *)gMapData)->unkA22[
            ((struct Unk402B4Map *)gMapData)->rowOffset[y] + x] = 0x122;
    }

    if (v == 0x163)
    {
        ((struct Unk402B4Map *)gMapData)->terrain[
            ((struct Unk402B4Map *)gMapData)->rowOffset[y] + x] = 1;
        ((struct Unk402B4Map *)gMapData)->unkA22[
            ((struct Unk402B4Map *)gMapData)->rowOffset[y] + x] = 0x123;
    }

    sub_08024268();
    sub_080219AC();
}

void sub_08040380(int x, int y, ProcPtr parent)
{
    u16 v = ((struct Unk40380Map *)gMapData)->unkA22[
        ((struct Unk40380Map *)gMapData)->rowOffset[y] + x];

    sub_08040430(0x1CA, 5);
    sub_0804046C(x, y, 0x1CA, 5, parent);

    if (v == 0x180)
    {
        ((struct Unk40380Map *)gMapData)->terrain[
            ((struct Unk40380Map *)gMapData)->rowOffset[y] + x] = 0x12;
        ((struct Unk40380Map *)gMapData)->unkA22[
            ((struct Unk40380Map *)gMapData)->rowOffset[y] + x] = 0x1A0;
    }

    sub_08021CB4();
    sub_08024268();
}
