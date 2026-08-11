#include "global.h"

/* Promoted from assembly; each function below is byte-for-byte
 * identical to the original. Order is address order and must
 * stay that way -- the linker places this file's .text as one
 * contiguous block at 0x08062730.
 * sub_08062730 @ 0x08062730
 */

/* `dist` is a BOUND local, not `dx + dy` at the compare: the ROM keeps the sum
 * in r7 across both calls, and leaving the two halves separate costs an r8
 * spill for gCurrentArmyIndex (+4 bytes, measured).
 *
 * The tail is `if (dist > cost) return 0; else return 1;` and not the `<=`
 * form. Both are the same 24-byte `movs`/`b`/`movs` shape, but agbcc emits the
 * `<=` spelling with `ble` reaching the 1-arm, while the ROM has `bgt` reaching
 * the 0-arm -- i.e. the source's if-body is the ZERO. */
int sub_08062730(struct UnitRecord *a, struct UnitRecord *b)
{
    int dx;
    int dy;
    int dist;
    int cost;

    dx = a->unk02 - b->unk02;
    if (dx < 0)
        dx = -dx;

    dy = a->unk03 - b->unk03;
    if (dy < 0)
        dy = -dy;

    dist = dx + dy;

    if (sub_08042D50(gCurrentArmyIndex, b->unk00) == 1)
        cost = sub_08058224(a) + sub_08058224(b) + 1;
    else
        cost = sub_08058224(a) + sub_08042D50(gCurrentArmyIndex, b->unk00);

    if (dist > cost)
        return 0;
    else
        return 1;
}
