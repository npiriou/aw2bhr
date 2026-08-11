#include "global.h"

/* Promoted from assembly; each function below is byte-for-byte
 * identical to the original. Order is address order and must
 * stay that way -- the linker places this file's .text as one
 * contiguous block at 0x08025C5C.
 * sub_08025C5C @ 0x08025C5C, sub_08025C98 @ 0x08025C98, sub_08025CC8 @ 0x08025CC8
 */

/* An early `return NULL` and not `if (u != NULL) { ...; return u; }`: the
 * inverted spelling puts the `movs r0,#0` in the FALL-THROUGH and the body past
 * the branch, which is the mirror image of the ROM. Written as a guard clause
 * the body falls through and the zero sits after the literal pool, as here.
 *
 * The three parameters are `s16`. Nothing in this body can tell s16 from u16 --
 * PROMOTE_MODE emits the same `lsls #0x10; lsrs #0x10` for both (probed) and
 * the values only ever reach a `strb` -- so the evidence is entirely at the two
 * callers, sub_08025C98 and sub_08025CC8, which SIGN-extend all three before
 * the `bl`. `u16` here makes both of them emit `lsrs` instead.
 *
 * The `lsls #0x18; lsrs #0x18` on a3 is the conversion to sub_08025BE0's u8. */

struct UnitRecord *sub_08025C5C(s16 a1, s16 a2, s16 a3)
{
    struct UnitRecord *u = sub_08025AEC();

    if (u == NULL)
        return NULL;

    sub_08025BE0(u, a3);

    u->unk02 = a1;
    u->unk03 = a2;

    sub_08025D20(gCurrentArmyIndex);

    return u;
}

/* Same guard-clause shape as sub_08025C5C: `if (u == NULL) return NULL;` puts
 * the body in the fall-through and the `movs r0,#0` at the end. The inverted
 * `if (u != NULL) { ...; return u; } return NULL;` swaps the two blocks and
 * misses by 20 bytes -- measured, not assumed.
 *
 * `orrs` on a bare `movs r0,#1`, so unk01 bit 0 is a plain mask and not a
 * bitfield. */

void *sub_08025C98(s16 a1, s16 a2, s16 a3)
{
    struct UnitRecord *u = sub_08025C5C(a1, a2, a3);

    if (u == NULL)
        return NULL;

    u->unk01 |= 1;
    sub_080258CC();

    return u;
}

/* sub_08025C98 without the `unk01 |= 1` -- see there for the guard-clause
 * shape. */

/* Wave 32 (W32-B) RETYPES the return `void *` -> `struct UnitRecord *`. It
 * returns sub_08025C5C's result unchanged, and that function is already
 * declared `struct UnitRecord *` right here -- the `void *` was the weakest
 * type that fit when nothing read the result. sub_08045564, promoted this
 * wave, writes `->unk04_0 = 0x5a` through it, which is the discriminating use.
 * Byte-neutral; re-verified. */
struct UnitRecord *sub_08025CC8(s16 a1, s16 a2, s16 a3)
{
    struct UnitRecord *u = sub_08025C5C(a1, a2, a3);

    if (u == NULL)
        return NULL;

    sub_080258CC();

    return u;
}
