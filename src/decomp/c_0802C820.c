#include "global.h"

/* Promoted from assembly; each function below is byte-for-byte
 * identical to the original. Order is address order and must
 * stay that way -- the linker places this file's .text as one
 * contiguous block at 0x0802C820.
 * sub_0802C820 @ 0x0802C820, sub_0802C848 @ 0x0802C848
 */

/* "Is this army NOT blocked", where the block reason is a mask.
 *
 * sub_080442AC returns `int`, not bool8: its result goes straight into the
 * `ands` with no `lsls #0x18; lsrs #0x18` in front, and agbcc re-narrows a
 * narrow-returning callee at every call site. Its parameter is `int` on the
 * bare-prologue rule -- `adds r5, r0, #0` with no masking -- so the u16
 * gCurrentArmyIndex arrives as a plain `ldrh`.
 *
 * The test reads FALSE-first: the branch jumps to `movs r0, #0` and that block
 * sits after the pool, so `return FALSE` is the THEN arm and `return TRUE` the
 * fallthrough. That is the sub_0802C5F0 / sub_0802C684 spelling, not the
 * sub_0802C69C one.
 */

bool8 sub_0802C820(void)
{
    if (gUnknown_03003FC0.unk07 & sub_080442AC(gCurrentArmyIndex))
        return FALSE;

    return TRUE;
}

/* sub_0802C820's twin: byte-identical apart from the callee, sub_08044280
 * instead of sub_080442AC. Both callees open `adds r5, r0, #0` with no masking
 * (so `int` parameter) and both results feed the `ands` with no re-narrowing
 * (so `int` return). See sub_0802C820 for the branch-sense reading.
 */

bool8 sub_0802C848(void)
{
    if (gUnknown_03003FC0.unk07 & sub_08044280(gCurrentArmyIndex))
        return FALSE;

    return TRUE;
}
