#include "global.h"

/* Promoted from assembly; each function below is byte-for-byte
 * identical to the original. Order is address order and must
 * stay that way -- the linker places this file's .text as one
 * contiguous block at 0x080198D0.
 * sub_080198D0 @ 0x080198D0
 */

/* `bl _call_via_r0` is the ordinary indirect call, not an interworking veneer,
 * and the register index says it takes no arguments -- which agrees with
 * gUnknown_03001FF0's declared `bool8 (*)(void)`. The `lsls #0x18` with no
 * `lsrs` after it is the truth test on that bool8 result. */
void sub_080198D0(void)
{
    if (gArmyRecords[gCurrentArmyIndex].unk1b != 2
     && gUnknown_03001FF0 != NULL
     && gUnknown_03001FF0())
        sub_08034F7C();
}
