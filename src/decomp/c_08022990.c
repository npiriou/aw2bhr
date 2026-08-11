#include "global.h"

/* Promoted from assembly; each function below is byte-for-byte
 * identical to the original. Order is address order and must
 * stay that way -- the linker places this file's .text as one
 * contiguous block at 0x08022990.
 * sub_08022990 @ 0x08022990
 */

/* The sub_080227F4 member of the 76-byte twin family (sub_08022580 /
 * sub_080227A8), wrapped in a setup/teardown: the sweep is 11 rows of 16
 * instead of 16 of 16 (`cmp r5,#0xa` at the outer bottom against `#0xf` at the
 * inner).
 *
 * gUnknown_080909B4 in the asm is NOT a global: the ROM word at 0x080909B4
 * holds 0x08499590, agbcc's own -fforce-addr address constant for
 * gMapData -- the fifth of that set (see include/unknown-globals.h).
 *
 * The first two parameters are dead here and cost no instruction: they are
 * already in r0/r1 at entry and sub_08013C00 is declared nullary, so the
 * forwarding is invisible either way.  Only the third is narrowed at entry
 * (`lsls #0x10; lsrs #0x10`) and parked in r8 across the loop for the final
 * `strh` into the slot sub_080152EC hands back. */
void sub_08022990(int a1, int a2, u16 a3)
{
    u16 x;
    u16 y;

    sub_08013C00();

    for (y = 0; y <= 0xa; y++)
    {
        for (x = 0; x <= 0xf; x++)
        {
            sub_080227F4((u16)(x + ((s16)*(u16 *)(gMapData + 4) >> 4)),
                         (u16)(y + ((s16)*(u16 *)(gMapData + 6) >> 4)));
        }
    }

    sub_08013AEC();
    gUnknown_03000559 = 1;
    sub_080152EC(gUnknown_08499B4C, 0xff)->unk1e = a3;
}
