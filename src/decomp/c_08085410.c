#include "global.h"

/* Promoted from assembly; each function below is byte-for-byte
 * identical to the original. Order is address order and must
 * stay that way -- the linker places this file's .text as one
 * contiguous block at 0x08085410.
 * sub_08085410 @ 0x08085410
 */

/* Maps a terrain/defence modifier to one of thirteen consecutive ids
 * (0x9b..0xa7). The `adds r0, #0x1e; cmp r0, #0x6e; bls` pair in front of the
 * jump table is the signed-range switch idiom -- the table has 111 entries for
 * -30..80 -- and the case bodies come out in SOURCE order, which is what fixes
 * `default:` between case -30 and case 10 rather than at the end. */

int sub_08085410(int a1, int a2)
{
    switch (sub_080430B0(gArmyRecords[a1].unk1d, gArmyRecords[a1].unk1e, a2))
    {
    case -10:
        return 0x9b;
    case -20:
        return 0x9c;
    case -30:
        return 0x9d;
    default:
        return 0x9e;
    case 10:
        return 0x9f;
    case 15:
        return 0xa0;
    case 20:
        return 0xa1;
    case 30:
        return 0xa2;
    case 40:
        return 0xa3;
    case 50:
        return 0xa4;
    case 60:
        return 0xa5;
    case 75:
        return 0xa6;
    case 80:
        return 0xa7;
    }
}
