#include "global.h"

/* Promoted from assembly; each function below is byte-for-byte
 * identical to the original. Order is address order and must
 * stay that way -- the linker places this file's .text as one
 * contiguous block at 0x0802032C.
 * sub_0802032C @ 0x0802032C
 */

/* The same dispatch as sub_0802026C, with the arguments taken from one unit
 * record instead of the caller: column, row, unit-type id, then the literal
 * pair (0x78, 0).
 *
 * The three `ldrb` at +0, +2 and +3 are what identifies the parameter as
 * `struct UnitRecord *` -- that struct's unk02 and unk03 are documented as the
 * cell column and row, and unk00 as the unit-type id, which is exactly the
 * order they arrive in. */
void sub_0802032C(struct UnitRecord *unit)
{
    gUnknown_030013EC(unit->unk02, unit->unk03, unit->unk00, 0x78, 0);
}
