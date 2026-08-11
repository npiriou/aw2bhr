#include "global.h"

/* Promoted from assembly; each function below is byte-for-byte
 * identical to the original. Order is address order and must
 * stay that way -- the linker places this file's .text as one
 * contiguous block at 0x08025E74.
 * sub_08025E74 @ 0x08025E74
 */

/* Promoted from assembly; each function below is byte-for-byte
 * identical to the original. Order is address order and must
 * stay that way -- the linker places this file's .text as one
 * contiguous block at 0x08025E74.
 * sub_08025E74 @ 0x08025E74
 */

void sub_08025E74(void)
{
    int i;

    for (i = 0; i < 256; i++)
        gUnitRecords[i].unk00 = 0;
    gUnknown_030032C0 = 0;
}
