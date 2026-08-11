#include "global.h"

/* Promoted from assembly; each function below is byte-for-byte
 * identical to the original. Order is address order and must
 * stay that way -- the linker places this file's .text as one
 * contiguous block at 0x080433B8.
 * sub_080433B8 @ 0x080433B8, sub_080433C8 @ 0x080433C8, sub_080433D8 @ 0x080433D8, sub_080433E8 @ 0x080433E8
 */

/* All four return `int`, not the u8/u16 they were first promoted with. The
 * bodies are bare ldrb/ldrh member loads and are byte-identical either way, so
 * the width was a body-side guess with no oracle. Wave 26 produced the first
 * promoted callers and they settle it: sub_08042C9C does `bl sub_080433D8;
 * adds r6, r0, #0` and multiplies with r6 later -- no re-narrowing after the
 * bl with the value used, which a u16 return cannot produce (agbcc re-narrows
 * a narrow-returning callee's result at every call site). sub_08042D1C,
 * sub_08042D50 and sub_08042D84 show the same for the other three. Settled
 * from the callers; all four re-verified byte-exact after the change. */

int sub_080433B8(int a)
{
    return gUnitTypeData[a].unk0a;
}

int sub_080433C8(int a)
{
    return gUnitTypeData[a].unk0f;
}

int sub_080433D8(int a)
{
    return gUnitTypeData[a].unk06;
}

int sub_080433E8(int a)
{
    return gUnitTypeData[a].unk0c;
}
