#include "global.h"

/* Promoted from assembly; each function below is byte-for-byte
 * identical to the original. Order is address order and must
 * stay that way -- the linker places this file's .text as one
 * contiguous block at 0x08026100.
 * sub_08026100 @ 0x08026100
 */

/* MATCHED, and the first of a byte-identical pair with sub_08044854. Applies
 * `c` points of damage to the unit standing on cell (x, y), flooring at 1:
 * bounds-check the cell against the gMapData screen header, look the
 * unit id up through the +0x51A plane, and clamp.
 *
 * The screen indexing is c_08001158.c's idiom exactly -- `rows = p + 0x417A`,
 * `t = y * 2`, `*(u16 *)(rows + t) + x` -- and the two pointer bindings are
 * load-bearing for the reason docs/agbcc-codegen.md gives under `p[X + C]` vs
 * `*(p + X + C)`: without `cells = p + 0x51A` agbcc reassociates the constant
 * to the outside and emits `((idx + p) + 0x51A)` where the ROM has
 * `idx + (p + 0x51A)`.
 *
 * THE COMMA CHAIN IS NOT DECORATION and it is the whole cost of this function.
 * Three separate facts are read off the ROM and every one of them needs a
 * reference at a position no statement boundary can reach:
 *
 *  - `pp = &gUnitRecords` is the wave-17 anchor. gUnitRecords is a
 *    POINTER global whose address is materialised right after the height test
 *    and whose `ldr` is issued last; `pp` is never read, and what matters is
 *    only that the reference precedes `t = y * 2`. Without it the address load
 *    lands five instructions late, the height goes to r2 instead of r6 and the
 *    frame loses r6. An anchor spelled `u = gUnitRecords;` does NOT work
 *    -- that is a pointer LOAD (`ldr addr; ldr [addr]`), and the ROM's deref is
 *    at the end.
 *  - `t = y * 2` must sit after the anchor and before the 0x417A load, which
 *    only an embedded assignment reaches; as its own statement it is one slot
 *    early.
 *  - `idx = ...` before `(cells = p + 0x51A)[idx]` orders the two adds the way
 *    the ROM has them. Written the other way round the pool order flips to
 *    0x51A before 0x417A.
 *
 * Every neighbouring spelling misses by exactly one slot, which is what makes
 * this a readout rather than a lucky arrangement -- see the wave-20 section in
 * docs/agbcc-codegen.md for the table. */
void sub_08026100(int x, int y, int c)
{
    u8 *p;
    u8 *rows;
    u8 *cells;
    int t;
    int idx;
    struct UnitRecord *u;
    struct UnitRecord **pp;

    if (x < 0)
        return;
    if (y < 0)
        return;

    p = gMapData;

    if (x >= *(u16 *)p)
        return;
    if (y >= *(u16 *)(p + 2))
        return;

    u = &gUnitRecords[(pp = &gUnitRecords, t = y * 2,
        idx = *(u16 *)((rows = p + 0x417A) + t) + x,
        (cells = p + 0x51A)[idx])];

    if (u->unk00 == 0)
        return;
    if ((u->unk01 & 8) != 0)
        return;

    if (u->unk04_0 < c + 1)
        u->unk04_0 = 1;
    else
        u->unk04_0 = u->unk04_0 - c;
}
