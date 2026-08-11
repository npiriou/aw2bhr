#include "global.h"

/* Promoted from assembly; each function below is byte-for-byte
 * identical to the original. Order is address order and must
 * stay that way -- the linker places this file's .text as one
 * contiguous block at 0x08023274.
 * sub_08023274 @ 0x08023274, sub_080232CC @ 0x080232CC
 */

void sub_08023274(int a1)
{
    s16 x;
    s16 y;

    sub_08023168(gUnknown_030033E4.unk00 << 4, gUnknown_030033E4.unk02 << 4, a1, &x, &y);
    sub_08043418(x - *(s16 *)(gMapData + 4), y - *(s16 *)(gMapData + 6), (s16)a1);
}

void sub_080232CC(int a1, int a2)
{
    s16 x;
    s16 y;

    sub_08023168(gUnknown_030033E4.unk00 << 4, gUnknown_030033E4.unk02 << 4, a1, &x, &y);
    sub_08043418(x - *(s16 *)(gMapData + 4), y - *(s16 *)(gMapData + 6), a1);
    sub_08043418(x - *(s16 *)(gMapData + 4), y - *(s16 *)(gMapData + 6), a2);
}
