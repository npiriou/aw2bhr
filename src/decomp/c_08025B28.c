#include "global.h"

/* Promoted from assembly; each function below is byte-for-byte
 * identical to the original. Order is address order and must
 * stay that way -- the linker places this file's .text as one
 * contiguous block at 0x08025B28.
 * sub_08025B28 @ 0x08025B28, sub_08025B58 @ 0x08025B58, sub_08025B80 @ 0x08025B80
 */

/* Binding the deref to a local is what keeps `ldr r3,[r2]` out of the second
 * and third statements: repeating gArmyRecords[a] would reload the
 * pointer after each store (see sub_08044178), and the target loads it once.
 */
void sub_08025B28(u16 a, u32 b)
{
    struct ArmyRecord *arr = gArmyRecords;

    arr[a].unk00 -= b;
    arr[a].unk04 += b;
    if (arr[a].unk04 > 999999)
        arr[a].unk04 = 999999;
}

void sub_08025B58(u16 a, u32 b)
{
    struct ArmyRecord *arr = gArmyRecords;

    arr[a].unk00 += b;
    if (arr[a].unk00 > 999999)
        arr[a].unk00 = 999999;
}

/* unk06_0 is a 7-bit field: the clear path emits `movs #0x80; rsbs` (= ~0x7f)
 * where a bare movs would have done, and the read is the two-instruction
 * `lsls #25; lsrs #25` double shift rather than a shift/mask triple.
 */
void sub_08025B80(struct UnitRecord *s, u8 b)
{
    if (s->unk06_0 >= b)
        s->unk06_0 -= b;
    else
        s->unk06_0 = 0;
}
