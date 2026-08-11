#include "global.h"

/* Promoted from assembly; each function below is byte-for-byte
 * identical to the original. Order is address order and must
 * stay that way -- the linker places this file's .text as one
 * contiguous block at 0x080266DC.
 * sub_080266DC @ 0x080266DC
 */

/* Promoted from assembly; each function below is byte-for-byte
 * identical to the original. Order is address order and must
 * stay that way -- the linker places this file's .text as one
 * contiguous block at 0x080266DC.
 * sub_080266DC @ 0x080266DC
 */


/* Promoted from assembly; each function below is byte-for-byte
 * identical to the original. Order is address order and must
 * stay that way -- the linker places this file's .text as one
 * contiguous block at 0x080266DC.
 * sub_080266DC @ 0x080266DC
 */

bool8 sub_080266DC(u8 index)
{
    if (gArmyRecords[index].unk1b != 0 && gArmyRecords[index].unk14 == 0)
        return TRUE;

    return FALSE;
}
