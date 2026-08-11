#include "global.h"

/* Promoted from assembly; each function below is byte-for-byte
 * identical to the original. Order is address order and must
 * stay that way -- the linker places this file's .text as one
 * contiguous block at 0x08025308.
 * sub_08025308 @ 0x08025308
 */

int sub_08025308(int a)
{
    int count = 0;
    int i;

    for (i = 1; i <= 50; i++)
    {
        if (gUnitRecords[(a - 1) * 64 + i].unk00 != 0)
            count++;
    }

    return count;
}
