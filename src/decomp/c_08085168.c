#include "global.h"

/* Promoted from assembly; each function below is byte-for-byte
 * identical to the original. Order is address order and must
 * stay that way -- the linker places this file's .text as one
 * contiguous block at 0x08085168.
 * sub_08085168 @ 0x08085168
 */

void sub_08085168(s16 *p)
{
    sub_080853B0();
    sub_08043B60(0x20, 0x28, 0x82AC, 3);
    sub_0801F34C(gArmyRecords[p[0x33]].unk1a + 0x3D, 8, 0x28, 0, 1);

    if (sub_080266DC(p[0x33]) != 0)
        sub_080436DC(0x98, 0x70, p[0x33]);
}
