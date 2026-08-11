#include "global.h"

/* Promoted from assembly; each function below is byte-for-byte
 * identical to the original. Order is address order and must
 * stay that way -- the linker places this file's .text as one
 * contiguous block at 0x08026F04.
 * sub_08026F04 @ 0x08026F04
 */

/* One statement. gCurrentArmyIndex is loaded once and serves as both
 * sub_08025B58's `u16` first argument and the index of the element whose unk08
 * supplies the second -- which is why the `ldrh` sits before the array's own
 * pool word. unk08 was filler until wave 29; the whole-word `ldr` at +8 and
 * sub_08025B58's declared `u32` parameter are what name it. */

void sub_08026F04(void)
{
    sub_08025B58(gCurrentArmyIndex, gArmyRecords[gCurrentArmyIndex].unk08);
}
