#include "global.h"

/* Promoted from assembly; each function below is byte-for-byte
 * identical to the original. Order is address order and must
 * stay that way -- the linker places this file's .text as one
 * contiguous block at 0x08000E48.
 * sub_08000E48 @ 0x08000E48
 */

/* The map-screen bring-up, entered as a Proc callback: everything it does is
 * keyed off proc->unk1e, which is 0 on a fresh start and non-zero on a resume,
 * and the two `if (a == 0)` blocks are the fresh-start-only halves.
 *
 * gUnknown_0200B0B0 is reached TWO ways in one function and that is agbcc,
 * not two source spellings: -fforce-addr parks &gUnknown_0200B0B0 in this
 * unit's own pool (0x0808D6EC in the ROM) for the reads inside the loop, while
 * the later statements get an ordinary inline pool word. Likewise 0x0808D6F0 /
 * 0x0808D6F4 hold &sub_08036944 / &sub_080369BC with the THUMB bit set, and
 * 0x0808D6F8 / 0x0808D6FC hold &gUnknown_03003F3C and a script blob at
 * 0x084858AC -- all five are written honestly here and agbcc rebuilds the pool.
 *
 * 0x084858AC has no symbol of its own: data/data.s covers 0x084857AC..0x084858DC
 * as one incbin, so the script is named as an offset into it. */
void sub_08000E48(struct Unk03001470 *proc)
{
    int a;
    int i;

    a = proc->unk1e;

    gUnknown_0200B0B0 = &gUnknown_0200B000;
    gUnknown_0200B000.unk9c = 0;

    if (a == 0)
    {
        sub_08001124((u8 *)&gUnknown_0200B000, 0xB0);
        sub_08001124((u8 *)gUnknown_0200B0D0, 0x134);
        sub_080078E4(0, 1);
    }

    gUnknown_030040E4 = 0;
    gUnknown_030040E8 = 0;
    gMapData[0x4233] = 4;
    sub_08026BAC();
    sub_08034780();
    sub_08026924();
    gArmyRecords[0].unk1d = 0;
    gArmyRecords[1].unk1d = 1;
    gArmyRecords[2].unk1d = 3;
    gArmyRecords[3].unk1d = 8;
    gArmyRecords[4].unk1d = 6;
    gUnknown_03003FC0.unk04 = 0;
    gCurrentArmyIndex = 0;
    gUnknown_03004080 = 1;
    sub_08026768();
    sub_08000DF8(a);
    sub_08025E74();
    sub_0803DE68();
    sub_080268F4();
    sub_080258CC();
    sub_08023348();

    if (a == 0)
    {
        gUnknown_0200B0B0->unk00 |= 0x1000;
        gUnknown_0200B0B0->unk28 = 0;
        gUnknown_0200B0B0->unk12 = 0;

        for (i = 0; i <= 3; i++)
        {
            gUnknown_0200B0B0->unk17[i] |= 0xFF;
            gUnknown_0200B0B0->unk1b[i] |= 0xFF;
        }

        gUnknown_0200B0B0->unk6b = 0xFF;
        gUnknown_0200B0B0->unk60 = 0xFC00;
        gUnknown_0200B0B0->unk5c = 0x1220;
        gUnknown_0200B0B0->unk3e = 1;
    }

    sub_0803662C();
    sub_080366D0(sub_08036944);
    sub_080366C4(sub_080369BC);
    gUnknown_03003F3C = 0;
    sub_0801B780(0);
    sub_08002EB4();
    sub_08022AAC(7, 4);
    gUnknown_0200B0B0->unk10 = 0xFF;
    gUnknown_0200B0B0->unk4e = 0xFD80;
    gUnknown_0200B0B0->unk50 = 0xB8;
    gUnknown_0200B0B0->unk4a = 0x64;
    gUnknown_0200B0B0->unk2a = 1;
    gUnknown_0200B0B0->unk2e = 1;
    gUnknown_0200B0B0->unk2f = 1;
    sub_08007328();
    sub_0800056C(0);
    sub_080152C0((s32)&gUnknown_084857AC[0x100], 0);
    sub_08000CCC(0x28);
    sub_08003910();
    sub_0803B524(0xD8);
}
