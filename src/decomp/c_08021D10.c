#include "global.h"

/* Promoted from assembly; each function below is byte-for-byte
 * identical to the original. Order is address order and must
 * stay that way -- the linker places this file's .text as one
 * contiguous block at 0x08021D10.
 * sub_08021D10 @ 0x08021D10
 */

/* gUnknown_08090978 in the asm is NOT a global: the ROM word at 0x08090978
 * holds 0x08499590, i.e. it is agbcc's own -fforce-addr address constant for
 * gMapData (same case as gUnknown_080909A4 / gUnknown_080909B0 in
 * sub_08022580 / sub_080227A8).  Naming the global honestly reproduces
 * `ldr r6,=.LC; ldr r0,[r6]; ldr r1,[r0]`.
 *
 * +4 / +6 are the camera's pixel scroll, unwrapped to tile units by
 * `(s16)v >> 4`; +0xc / +0xe are the tile-granular camera origin.  Both camera
 * terms are written INLINE at every use rather than bound to a local: a local
 * assigned before the call emits the +4 and +6 loads back to back, while the
 * ROM interleaves them (+4, +0xc, +6, +0xe), which is argument order with CSE
 * picking each one up at its first use.
 *
 * The counter is s16 (`lsls #0x10; asrs #0x10` at the head, `ble` at the
 * bottom), unlike the u16 counters of the sub_08022580 twins. */
void sub_08021D10(void)
{
    s16 y;

    for (y = 0; y <= 0xf; y++)
    {
        sub_08023BAC((u16)(((s16)*(u16 *)(gMapData + 4) >> 4)
                           - *(u16 *)(gMapData + 0xc)),
                     (u16)(y + (((s16)*(u16 *)(gMapData + 6) >> 4)
                                - *(u16 *)(gMapData + 0xe))),
                     (u16)((s16)*(u16 *)(gMapData + 4) >> 4),
                     (u16)(y + ((s16)*(u16 *)(gMapData + 6) >> 4)));
    }

    sub_08013B1C();
}
