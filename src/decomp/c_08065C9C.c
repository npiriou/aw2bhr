#include "global.h"

/* Promoted from assembly; each function below is byte-for-byte
 * identical to the original. Order is address order and must
 * stay that way -- the linker places this file's .text as one
 * contiguous block at 0x08065C9C.
 * sub_08065C9C @ 0x08065C9C
 */

#include "hardware.h"

/* gUnknown_0816E114 and gUnknown_0816E118 are agbcc's own -fforce-addr words
 * holding &gUnknown_08580934 and &gpKeySt (the ROM words at those addresses are
 * 0x08580934 and 0x03002EE0). Naming the globals honestly reproduces the ROM's
 * three-level `ldr rN,=<word>; ldr r0,[rN]; ldr r1,[r0]` exactly -- see the
 * gMapData note in unknown-globals.h.
 *
 * The selector is `int`, not `s8`: the ROM never re-narrows it between the
 * decrement, the increment and the final `strb`. Both arms are spelled with the
 * ZERO test as the `if` and the step as the `else` -- gcc puts the else arm
 * after the short-circuit failures, which is the only order in which the
 * `&&` chain can branch past the step the way the ROM does. */
void sub_08065C9C(int a1)
{
    int v = gUnknown_08580934->unk32;

    if (gpKeySt->unk02 & 0x20)
    {
        if (v == 0)
        {
            if (gpKeySt->unk02 != 0 && gpKeySt->held != 0)
                v = a1 - 1;
        }
        else
        {
            v--;
        }
    }

    if (gpKeySt->unk02 & 0x10)
    {
        if (v == a1 - 1)
        {
            if (gpKeySt->unk02 != 0 && gpKeySt->held != 0)
                v = 0;
        }
        else
        {
            v++;
        }
    }

    if (gUnknown_08580934->unk32 != v)
        sub_0803B4DC(0x64);

    gUnknown_08580934->unk32 = v;
}
