#include "global.h"

/* Promoted from assembly; each function below is byte-for-byte
 * identical to the original. Order is address order and must
 * stay that way -- the linker places this file's .text as one
 * contiguous block at 0x08035B80.
 * sub_08035B80 @ 0x08035B80
 */

/* Draws the current gUnknown_03001470 slot's position relative to the
 * gMapData viewport.
 *
 * The index local must be `int`, not `s16`, and that is 6 bytes: with `s16 i`
 * agbcc groups the two pool `ldr`s back to back and the `ldrsh` scratch moves
 * from r1 to r2, which also swaps the first two pool words. With `int i` the
 * `ldrsh` completes before the table's address is materialised, which is what
 * the ROM has. Naming both globals inline with no locals at all is
 * byte-identical to the `int` spelling -- what matters is only that the index's
 * load is not scheduled alongside the table's.
 *
 * unk1e / unk20 are declared `s16` and read `ldrh` here: the subtraction is
 * computed in HImode because sub_080155C0's parameters are `s16`, so both
 * operands load unsigned and the single `lsls #0x10; asrs #0x10` at the end is
 * the whole conversion. No cast is needed or wanted. */
void sub_08035B80(void)
{
    struct Unk03001470 *p;
    u8 *q;
    int i;

    i = gUnknown_03001FBC;
    p = &gUnknown_03001470[i];
    q = gMapData;

    sub_080155C0(i,
                 p->unk1e - *(u16 *)(q + 4),
                 p->unk20 - *(u16 *)(q + 6) - 2);
}
