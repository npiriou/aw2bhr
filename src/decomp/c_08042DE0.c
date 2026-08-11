#include "global.h"

/* Promoted from assembly; each function below is byte-for-byte
 * identical to the original. Order is address order and must
 * stay that way -- the linker places this file's .text as one
 * contiguous block at 0x08042DE0.
 * sub_08042DE0 @ 0x08042DE0, sub_08042DFC @ 0x08042DFC
 */

/* Promoted from assembly; each function below is byte-for-byte
 * identical to the original. Order is address order and must
 * stay that way -- the linker places this file's .text as one
 * contiguous block at 0x08042DFC.
 * sub_08042DFC @ 0x08042DFC
 */


/* Promoted from assembly; each function below is byte-for-byte
 * identical to the original. Order is address order and must
 * stay that way -- the linker places this file's .text as one
 * contiguous block at 0x08042DE0.
 * sub_08042DE0 @ 0x08042DE0
 */

int sub_08042DE0(int a1)
{
    return sub_08042DCC(gArmyRecords[a1].unk1d);
}

/* `int`, not `u8`: a u8 return re-narrows the callee's result with
 * `lsls #24; lsrs #24`, which the original does not have. */
int sub_08042DFC(int a1)
{
    return sub_08042E18(gArmyRecords[a1].unk1d);
}
