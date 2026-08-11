#include "global.h"

/* Promoted from assembly; each function below is byte-for-byte
 * identical to the original. Order is address order and must
 * stay that way -- the linker places this file's .text as one
 * contiguous block at 0x08061CF8.
 * sub_08061CF8 @ 0x08061CF8
 */

/* Four independent `if`s over the four low bits of one flag byte, each adding
 * sub_08061DA8(n) into the accumulator. The element expression is repeated
 * rather than bound to a local: a local would park the element address in a
 * callee-saved register, while the ROM recomputes `gCurrentArmyIndex * 0x3c`
 * and the `adds r0,#0x2c` (past ldrb's imm5) inside every arm. */
void sub_08061CF8(void)
{
    gUnknown_03004788 = 0;

    if (gArmyRecords[gCurrentArmyIndex].unk2c & 1)
        gUnknown_03004788 += sub_08061DA8(1);

    if (gArmyRecords[gCurrentArmyIndex].unk2c & 2)
        gUnknown_03004788 += sub_08061DA8(2);

    if (gArmyRecords[gCurrentArmyIndex].unk2c & 4)
        gUnknown_03004788 += sub_08061DA8(3);

    if (gArmyRecords[gCurrentArmyIndex].unk2c & 8)
        gUnknown_03004788 += sub_08061DA8(4);
}
