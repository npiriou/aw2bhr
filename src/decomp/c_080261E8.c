#include "global.h"

/* Promoted from assembly; each function below is byte-for-byte
 * identical to the original. Order is address order and must
 * stay that way -- the linker places this file's .text as one
 * contiguous block at 0x080261E8.
 * sub_080261E8 @ 0x080261E8
 */

/* A per-army precondition check: in mode 1 the army's class selects a
 * sub_0803CBD8 tag to test, class 5 always passes, and anything outside 1..5
 * fails. Outside mode 1 it always passes.
 *
 * THE `else return TRUE` IS LOad-BEARING and is the whole difference between
 * this and a candidate that is otherwise instruction-exact. Written as
 * `... } return TRUE;` -- one trailing return for both paths -- agbcc creates
 * the `movs r0, #0` block before the `movs r0, #1` block and the two come out
 * in the wrong order, with `bne` where the ROM has `beq`. Giving the `!= 1`
 * path its own `return TRUE` inside the `else` and leaving a bare
 * `return FALSE` at the end creates them the other way round.
 *
 * The four tags are set into one variable and the call is shared: written as
 * four separate `if (!sub_0803CBD8(K)) return FALSE;` the cross-jumped result
 * is the same instructions, so this is a spelling choice, not evidence.
 *
 * `int` parameter: the prologue is a bare `adds r2, r0, #0` with no
 * PROMOTE_MODE narrowing at all. */
bool8 sub_080261E8(int a)
{
    int v;

    if (gUnknown_03003FC0.unk01 == 1)
    {
        switch (gArmyRecords[a].unk1a - 1)
        {
        case 0:
            v = 0x65;
            break;
        case 1:
            v = 0x66;
            break;
        case 2:
            v = 0x68;
            break;
        case 3:
            v = 0x67;
            break;
        case 4:
            return TRUE;
        default:
            return FALSE;
        }

        if (sub_0803CBD8(v) != 0)
            return TRUE;
    }
    else
        return TRUE;

    return FALSE;
}
