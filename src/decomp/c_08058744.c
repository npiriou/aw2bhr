#include "global.h"

/* Promoted from assembly; each function below is byte-for-byte
 * identical to the original. Order is address order and must
 * stay that way -- the linker places this file's .text as one
 * contiguous block at 0x08058744.
 * sub_08058744 @ 0x08058744
 */

/* PARKED at 95.8%, wave 31 (W31-C). 3 bytes of 72, first difference at +0x32.
 *
 * The whole function is right except for ONE register in the else arm. The ROM
 * reloads the pointer into r2:
 *
 *     ldr r2, [r4]
 *     ldrb r0, [r2, #2]
 *     ldrb r1, [r2, #3]
 *
 * every candidate reloads it into r1 and then overwrites r1 with the second
 * `ldrb`. Size and every other byte match; the branch lines in trymatch's diff
 * are label-name noise, not code.
 *
 * RULED OUT: naming gUnknown_030040D8 twice in the call; binding the pointer to
 * a local first; binding the two byte values to `u8` locals first. All three
 * reuse r1.
 *
 * Next thing to try: sub_08020354's parameter widths. It is declared from this
 * call site alone, and a wider first parameter -- or a third parameter -- would
 * make the pointer conflict with r1 and push it out to r2.
 */

int sub_08058744(void)
{
    int r = 0;

    if (sub_08042D50(gCurrentArmyIndex, gUnknown_030040D8->unk00) == 1)
    {
        sub_080202A4(gUnknown_030040D8);
        sub_0801FD9C(0x79);
    }
    else
    {
        sub_0801F838(0xff);
        /* WAVE 36: third argument added. W35-H retyped sub_08020354 from
         * (int, int) to (u16, u16, struct UnitRecord *) on body-side
         * evidence; this call site confirms it independently -- the ROM does
         * `ldr r2,[r4]` then reads r0/r1 through r2 and leaves r2 holding the
         * pointer, so the third argument IS the struct pointer and costs no
         * instruction to pass. */
        sub_08020354(gUnknown_030040D8->unk02, gUnknown_030040D8->unk03,
                     (struct UnitRecord *)gUnknown_030040D8);
        r = 1;
    }
    return r;
}
