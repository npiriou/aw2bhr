#include "global.h"

/* Promoted from assembly; each function below is byte-for-byte
 * identical to the original. Order is address order and must
 * stay that way -- the linker places this file's .text as one
 * contiguous block at 0x08026520.
 * sub_08026520 @ 0x08026520
 */

/* Refreshes the four per-army summary bytes each turn: unk34/unk35/unk36 are
 * three independent scores, unk38 is sub_08026368's clamped total of them and
 * unk33 is sub_08037D80's grade of that total.
 *
 * The `strh` at +0x38 is immediately followed by an `ldrh` of the same slot:
 * the source stores the field and then passes the FIELD, not the register.
 * Written as `sub_08037D80(sub_08026368(i))` the reload disappears. */
void sub_08026520(void)
{
    u8 i;

    for (i = 1; i <= 4; i++)
    {
        gArmyRecords[i].unk34 = sub_080263A4(i);
        gArmyRecords[i].unk35 = sub_08026424(i);
        gArmyRecords[i].unk36 = sub_080264BC(i);
        gArmyRecords[i].unk38 = sub_08026368(i);
        gArmyRecords[i].unk33 = sub_08037D80(gArmyRecords[i].unk38);
    }
}
