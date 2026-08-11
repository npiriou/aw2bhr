#include "global.h"

/* Promoted from assembly; each function below is byte-for-byte
 * identical to the original. Order is address order and must
 * stay that way -- the linker places this file's .text as one
 * contiguous block at 0x08000DF8.
 * sub_08000DF8 @ 0x08000DF8
 */

/* Wave 37 (W37-E). Matched on the second attempt.
 * The six halfword clears share ONE base load. Written as
 * `((struct Map *)gMapData)->unkNN = 0` six times agbcc reloads the
 * pointer global before every store (it must assume the store aliases the
 * pointer variable), which is +8 bytes; binding `u16 *map` once fixes it.
 * The stores use r4 -- the parameter -- because cse knows a1 == 0 on this side
 * of `cmp r4, #0`, so the source really does say 0. */

void sub_08000DF8(int a1)
{
    u16 *map;

    gUnknown_030032D8 = 0;
    sub_080215D0();

    if (a1 == 0)
    {
        map = (u16 *)gMapData;
        map[2] = 0;
        map[3] = 0;
        map[4] = 0;
        map[5] = 0;
        map[6] = 0;
        map[7] = 0;
        sub_08003B8C();
    }

    gUnknown_030033E4.unk00 = 0;
    gUnknown_030033E4.unk02 = 0;
    gUnknown_030033E0.unk00 = 0;
    gUnknown_030033E0.unk02 = 0;
    sub_0802BB98();
}
