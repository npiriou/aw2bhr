#include "global.h"

/* Promoted from assembly; each function below is byte-for-byte
 * identical to the original. Order is address order and must
 * stay that way -- the linker places this file's .text as one
 * contiguous block at 0x08035080.
 * sub_08035080 @ 0x08035080
 */

struct Unk03004490
{
    u8 filler_00[3];
    u8 values[5];
};

/* Increment the current army's selector byte, then report whether every other
 * active army either shares that byte or is ineligible. The wrapper struct is
 * required for agbcc to hoist the +3 member offset onto the pointer base, as in
 * the ROM. The single-iteration do block preserves the original goto-loop
 * layout and register allocation. */
u8 sub_08035080(void)
{
    u8 value;
    u8 i;

    value = ++((struct Unk03004490 *)gUnknown_03004490)->values[gCurrentArmyIndex];
    do
    {
        i = 1;
        goto check;
    next:
        i++;
    check:
        if (i > 4)
            return 1;
        if (i == gCurrentArmyIndex)
            goto next;
        if (!sub_080266DC(i))
            goto next;
    } while (0);

    if (value == ((struct Unk03004490 *)gUnknown_03004490)->values[i])
        goto next;

    return 0;
    return 1;
}
