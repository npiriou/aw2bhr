#include "global.h"

/* Promoted from assembly; each function below is byte-for-byte
 * identical to the original. Order is address order and must
 * stay that way -- the linker places this file's .text as one
 * contiguous block at 0x08026BAC.
 * sub_08026BAC @ 0x08026BAC
 */

/* Clears the five army records and stamps each one's identity: unk2a gets the
 * slot index inside the loop, and unk2b gets that slot's single mask bit
 * (1, 2, 4, 8 for slots 1..4) in four statements afterwards. Slot 0 is reset
 * like the rest but gets no bit.
 *
 * `i` is an `int` -- the exit test is `cmp r4, #4; ble`, SIGNED, and the value
 * is stored into the u8 unk2a with no narrowing at all.
 *
 * The array pointer is reloaded before nearly every store because each store
 * may alias it; where consecutive fields DO share one `ldr` (unk00/unk04/
 * unk11, the unk14..unk1a run, unk20/unk24, unk26/unk28/unk2a) that is agbcc's
 * own CSE and not a different source spelling -- one plain assignment
 * statement per field reproduces the grouping exactly.
 *
 * gUnknown_08090A8C is the -fforce-addr word holding &gArmyRecords, which
 * is why the chain is three `ldr`s deep. Named honestly; the build places the
 * word. */
void sub_08026BAC(void)
{
    int i;

    for (i = 0; i <= 4; i++)
    {
        gArmyRecords[i].unk00 = 0;
        gArmyRecords[i].unk04 = 0;
        gArmyRecords[i].unk11 = 0;
        gArmyRecords[i].unk12 = 0;
        gArmyRecords[i].unk13 = 0;
        gArmyRecords[i].unk14 = 0;
        gArmyRecords[i].unk16 = 0;
        gArmyRecords[i].unk18 = 0;
        gArmyRecords[i].unk1a = 0;
        gArmyRecords[i].unk1b = 0;
        gArmyRecords[i].unk1c = 0;
        gArmyRecords[i].unk1e = 0;
        gArmyRecords[i].unk1f = 0;
        gArmyRecords[i].unk20 = 0;
        gArmyRecords[i].unk24 = 0;
        gArmyRecords[i].unk25 = 0;
        gArmyRecords[i].unk26 = 0;
        gArmyRecords[i].unk28 = 0;
        gArmyRecords[i].unk2a = i;
        gArmyRecords[i].unk31 = 0;
        gArmyRecords[i].unk32 = 0;
        gArmyRecords[i].unk3a = 0;
        gArmyRecords[i].unk3b = 0;
    }

    gArmyRecords[1].unk2b = 1;
    gArmyRecords[2].unk2b = 2;
    gArmyRecords[3].unk2b = 4;
    gArmyRecords[4].unk2b = 8;
}
