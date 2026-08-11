#include "global.h"

/* Promoted from assembly; each function below is byte-for-byte
 * identical to the original. Order is address order and must
 * stay that way -- the linker places this file's .text as one
 * contiguous block at 0x0804360C.
 * sub_0804360C @ 0x0804360C
 */

void sub_0804360C(int a)
{
    sub_080436DC(a, 3, gCurrentArmyIndex);
    sub_0804365C(a, 6);
}
