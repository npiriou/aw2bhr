#include "global.h"

/* Promoted from assembly; each function below is byte-for-byte
 * identical to the original. Order is address order and must
 * stay that way -- the linker places this file's .text as one
 * contiguous block at 0x0805BC7C.
 * sub_0805BC7C @ 0x0805BC7C
 */

/* sub_0805BA34's shape (src/decomp/c_0805B980.c) with a fifth stack argument on
 * each probe and one extra local. `cmn r5, r0` is an ADD-and-test-zero, so the
 * four results are SUMMED and the sum compared with 0 -- not r5 compared with
 * -r0. 0x270F is the 9999 sentinel: stored through the out pointer before the
 * call and compared against it afterwards.
 *
 * The frame is 0xc: the outgoing fifth-argument slot at sp+0, the 4-byte cell
 * pair at sp+4, and a POINTER to that pair spilled at sp+8. Three things had to
 * be true together to reproduce that pointer, and each was measured with
 * compile_probe:
 *   - The pair is a STRUCT, not `s16 loc[2]`. The two reads at the end are
 *     `add r0,sp,#4; ldrh r0,[r0]` and `ldr r2,[sp,#8]; ldrh r1,[r2,#2]` -- the
 *     first through the frame address, the second through the spilled pointer
 *     with the +2 in the load displacement. An array subscript `p[1]` folds the
 *     +2 into the address instead (`mov r1,sp; adds r1,#6`), which is two bytes
 *     longer and is what every array spelling produced.
 *   - The pointer needs a SECOND reference to survive at all. With `p = &loc`
 *     as its own statement and one dereference, cse propagates the frame
 *     address into the use and the slot disappears (frame 0x8). Assigning it
 *     inside the first call's fifth argument is what keeps it.
 *   - That placement is also what puts the three-instruction `mov r1,sp;
 *     adds r1,#4; str r1,[sp,#8]` BETWEEN the third and fifth argument setup
 *     rather than ahead of the whole call. */

struct UnkPos5BC7C
{
    /* 0x00 */ u16 unk00;
    /* 0x02 */ u16 unk02;
};

u8 sub_0805BC7C(int x, int y, u16 *out)
{
    struct UnitRecord *e;
    struct UnkPos5BC7C loc;
    struct UnkPos5BC7C *p;
    int best;

    if (gUnknown_030040D8->unk07[1] == 0)
        return 0;

    e = &gUnitRecords[gUnknown_030040D8->unk07[1]];

    if (sub_0805BD40(x - 1, y, e->unk00, 4, (s16 *)(p = &loc))
        + sub_0805BD40(x + 1, y, e->unk00, 2, (s16 *)&loc)
        + sub_0805BD40(x, y - 1, e->unk00, 1, (s16 *)&loc)
        + sub_0805BD40(x, y + 1, e->unk00, 3, (s16 *)&loc) == 0)
        return 0;

    best = 9999;
    out[0] = best;

    sub_0805BF3C(loc.unk00, p->unk02, out);

    if (out[0] == best)
        return 0;

    return 1;
}
