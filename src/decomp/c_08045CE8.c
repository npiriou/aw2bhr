#include "global.h"

/* Promoted from assembly; each function below is byte-for-byte
 * identical to the original. Order is address order and must
 * stay that way -- the linker places this file's .text as one
 * contiguous block at 0x08045CE8.
 * sub_08045CE8 @ 0x08045CE8
 */

/* sub_08045BF0's twin: a different row-pointer slot (+0x417C), a different
 * byte in the record (+0x1E), and it asks for top-two-bits == 1 instead of 0.
 * See sub_08045BF0 for why the cell is held in an `int`. */
int sub_08045CE8(void)
{
    u8 *p;
    int v;

    p = gMapData;
    v = (p + *(u16 *)(p + 0x417C))[0x1E];

    if (v == 0)
        return 0;
    if ((v >> 6) == 1)
        return 1;

    return 0;
}
