#include "global.h"

/* Promoted from assembly; each function below is byte-for-byte
 * identical to the original. Order is address order and must
 * stay that way -- the linker places this file's .text as one
 * contiguous block at 0x0802571C.
 * sub_0802571C @ 0x0802571C
 */

/* The id form of sub_080255F4's "is this unit boxed in?" -- it looks the unit
 * up and passes the record together with its own unk02/unk03 cell coordinates.
 * The element must be bound to a local: the pointer is used three times, and
 * that is also what keeps the pool word undereferenced until after the x12
 * index is computed (see src/decomp/c_08025340.c).
 *
 * The `lsls #0x18; lsrs #0x18` on the way out is agbcc re-narrowing
 * sub_080255F4's `u8` return at the call site, not a cast in the source. */

bool8 sub_0802571C(u16 a1)
{
    struct UnitRecord *unit = &gUnitRecords[a1];

    return sub_080255F4(unit, unit->unk02, unit->unk03);
}
