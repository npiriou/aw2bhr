#include "global.h"

/* Promoted from assembly; each function below is byte-for-byte
 * identical to the original. Order is address order and must
 * stay that way -- the linker places this file's .text as one
 * contiguous block at 0x080260D0.
 * sub_080260D0 @ 0x080260D0
 */

/* Copies 64 records into slot `n - 1` of gUnitRecords, 64 records per
 * slot. Each `*dst = *src` on the 0x0c-byte struct is agbcc's block move --
 * one `ldmia`/`stmia` of three registers through scratch copies, which is
 * where the r5/r6/r7 push comes from. `(n - 1) * 64` elements is 0x300 bytes,
 * emitted as `((n-1) * 3) << 8`. */
void sub_080260D0(struct UnitRecord *src, int n)
{
    struct UnitRecord *dst;
    int i;

    dst = &gUnitRecords[(n - 1) * 64];

    for (i = 0; i < 64; i++)
        *dst++ = *src++;
}
