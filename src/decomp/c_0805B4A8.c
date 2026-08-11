#include "global.h"

/* Promoted from assembly; each function below is byte-for-byte
 * identical to the original. Order is address order and must
 * stay that way -- the linker places this file's .text as one
 * contiguous block at 0x0805B4A8.
 * sub_0805B4A8 @ 0x0805B4A8
 */

int sub_0805B4A8(void)
{
    int v;

    v = gUnitTypeData[gUnknown_030046D4].unk1a;

    if (v == 2)
        goto one;
    if (v <= 2)
        goto zero;
    if (v != 4)
        goto zero;
    return 2;
one:
    return 1;
zero:
    return 0;
}
