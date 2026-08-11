#include "global.h"

/* Promoted from assembly; each function below is byte-for-byte
 * identical to the original. Order is address order and must
 * stay that way -- the linker places this file's .text as one
 * contiguous block at 0x08042E18.
 * sub_08042E18 @ 0x08042E18
 */

/* Promoted from assembly; each function below is byte-for-byte
 * identical to the original. Order is address order and must
 * stay that way -- the linker places this file's .text as one
 * contiguous block at 0x08042E18.
 * sub_08042E18 @ 0x08042E18
 */


/* `int`, not the `u8` this was promoted with. Its own bytes cannot tell --
 * it is a leaf ending in `bx lr` and `ldrb` zero-extends either way -- but
 * sub_08042DFC forwards the result straight out with no `lsls #24; lsrs #24`,
 * and a u8-returning callee makes the caller re-narrow. The call site is the
 * only evidence there is, and it says 32 bits.
 */
int sub_08042E18(int a)
{
    return gCoDataTable[a].unk16;
}
