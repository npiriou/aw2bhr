#include "global.h"

/* Promoted from assembly; each function below is byte-for-byte
 * identical to the original. Order is address order and must
 * stay that way -- the linker places this file's .text as one
 * contiguous block at 0x08041128.
 * sub_08041128 @ 0x08041128
 */

/* Loads one army's unit-sprite sheet and its palette. The Decompress source is
 * picked by sub_08042DE0's mapping of the army index; the palette bank is that
 * army's gArmyRecords record's unk1a.
 *
 * The palette index is bound BEFORE the Decompress call, which is what puts the
 * gArmyRecords lookup between the two `bl`s: the u8 army sits in r4 and
 * unk1a takes the register over once the last use of the index has happened.
 *
 * `(u16)((a3 + 0x10) * 0x20)` is a MULTIPLY and not a mask plus a shift --
 * `adds #0x10; lsls #0x15; lsrs #0x10` is `(u32)x << 21 >> 16`, a net left shift
 * of five, and the apparent 11-bit mask is only the u16 truncation of the
 * product. Same idiom as the matched sub_08040430. a2 is genuinely unused:
 * sub_08042DE0 takes one argument. */
void sub_08041128(int a1, int a2, int a3)
{
    u8 army = a1;
    int i = sub_08042DE0(army);
    u8 pal = gArmyRecords[army].unk1a;

    Decompress(gUnknown_0849FD6C[i], (void *)0x06014940);
    ApplyPaletteExt(&gUnknown_081213F4[pal * 0x10], (u16)((a3 + 0x10) * 0x20), 0x20);
}
