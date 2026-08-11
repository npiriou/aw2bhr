#include "global.h"

/* Promoted from assembly; each function below is byte-for-byte
 * identical to the original. Order is address order and must
 * stay that way -- the linker places this file's .text as one
 * contiguous block at 0x080424FC.
 * sub_080424FC @ 0x080424FC, sub_0804256C @ 0x0804256C
 */

/* sub_0804256C with three extra calls and the gUnknown_03003FC0.unk0d gate.
 * sub_08025B80's parameter is declared `struct UnitRecord *` and the argument
 * is gUnknown_030040D8, so the cast is unavoidable here -- the two struct tags
 * describe the same object and unknown-globals.h records why they are kept
 * apart (retyping the global would turn c_080424BC.c's `->unk05 &= 7` into the
 * bitfield spelling). */
void sub_080424FC(void)
{
    sub_080424BC();
    sub_080176A4();
    gUnknown_030040D8->unk02 = gUnknown_03003100.pos.unk00;
    gUnknown_030040D8->unk03 = gUnknown_03003100.pos.unk02;

    if (gUnknown_03003FC0.unk0d == 0)
        sub_08025B80((struct UnitRecord *)gUnknown_030040D8, gUnknown_03004074);

    sub_08035810();
    gUnknown_030040D8->unk01 |= 1;

    if (gUnknown_030040D8->unk01 & 8)
        gUnknown_030040D8->unk01 |= 2;

    sub_080258CC();
    sub_080743E8(gUnknown_030040D8);
    sub_080198D0();
}

/* The ROM's `movs r5, #0` here is DEAD -- nothing reads r5, and r5 is pushed
 * only to hold it. It is not a tell for a missing statement: this straight
 * transcription reproduces it, so it is just agbcc materialising a constant
 * whose consumer was folded away.
 *
 * gUnknown_030040D8 is re-loaded before each store because the `strb` through
 * it can alias the pointer global itself -- the same reason c_080425B8.c
 * records. Within the final `if` the two reads share one `ldrb`, since no
 * store separates the test from the OR. */
void sub_0804256C(void)
{
    sub_080424BC();
    sub_080176A4();
    gUnknown_030040D8->unk02 = gUnknown_03003100.pos.unk00;
    gUnknown_030040D8->unk03 = gUnknown_03003100.pos.unk02;
    sub_08035810();
    gUnknown_030040D8->unk01 |= 1;

    if (gUnknown_030040D8->unk01 & 8)
        gUnknown_030040D8->unk01 |= 2;
}
