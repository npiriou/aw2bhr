#include "global.h"

/* Promoted from assembly; each function below is byte-for-byte
 * identical to the original. Order is address order and must
 * stay that way -- the linker places this file's .text as one
 * contiguous block at 0x080432E0.
 * sub_080432E0 @ 0x080432E0
 */

/* Maps the 3-valued tag at +0x0e onto a column index. `int`, not u8: its
 * caller sub_080430B0 does `lsls r0, r0, #2` straight off the `bl` with no
 * re-narrowing.
 *
 * The local has to be `u32`. The whole shape hangs on it: an UNSIGNED
 * comparison gives `cmp #1; bls` for the first test, a signed one gives `ble`
 * -- one bit of encoding, no size difference. And a `switch` statement does
 * not reach this at all in any case order (four spellings probed): it expands
 * to two linear `cmp/beq` equality tests, never the range test plus a single
 * equality test the ROM has, even though the two come out the same 36 bytes.
 */
int sub_080432E0(int a)
{
    u32 v = gUnitTypeData[a].unk0e;

    if (v > 1)
        return 6;

    if (v == 1)
        return 5;

    return 7;
}
