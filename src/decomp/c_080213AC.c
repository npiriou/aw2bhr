#include "global.h"

/* Promoted from assembly; each function below is byte-for-byte
 * identical to the original. Order is address order and must
 * stay that way -- the linker places this file's .text as one
 * contiguous block at 0x080213AC.
 * sub_080213AC @ 0x080213AC
 */

/* The per-turn refresh: repaint the two 0x508-byte overlay planes at
 * gMapData + 0x1E42 for the two cursor slots gUnknown_03004070 and
 * gUnknown_03004088, then -- only in the gUnknown_03003FC0.unk0d mode -- re-run
 * every army's turn-start pass and stamp sub_080210C8 over each record
 * sub_0803F5C8 hands back.
 *
 * gUnknown_08090964 and gUnknown_08090968 in the asm are NOT globals: the ROM
 * words there hold 0x03003FC0 and 0x08499590, so they are agbcc's own
 * -fforce-addr address constants for gUnknown_03003FC0 and gMapData.
 * Both are named several times across a control-flow merge here, which is the
 * documented trigger; the honest spelling reproduces them.
 *
 * The record walk reads offset 2 of struct Unk02028360 two different widths --
 * `ldrh` masked with 0x3C0 for the loop test and `ldrb` with shift pairs in the
 * body -- and that is exactly the already-declared bitfield triple
 * unk02_0:3 / unk02_3:3 / unk02_6:4.  The `ldrh; ands #0x3C0; cmp #0` is
 * fold's optimize_bit_field_compare rewriting `unk02_6 != 0` into a test of the
 * containing halfword; the two body reads are ordinary extract_bit_field shift
 * pairs (`lsls #0x1d/lsrs #0x1d` and `lsls #0x1a/lsrs #0x1d`), which a plain
 * `& 7` on a byte member could not produce -- that spells `movs #7; ands`.
 *
 * `lsls r7,r2,#0x10` in the inner preheader is the loop optimiser hoisting the
 * (s16) cast of x; x and y are plain ints. */

struct Unk213ACMap
{
    /* 0x0000 */ u8 filler_0000[0x1E42];
    /* 0x1E42 */ u8 unk1E42[1];
};

void sub_080213AC(void)
{
    struct Unk02028360 *p;
    int x;
    int y;
    int xend;
    int yend;

    if (gUnknown_03003FC0.unk0d != 0
        && gArmyRecords[gCurrentArmyIndex].unk1b == 2)
    {
        sub_08020754(&((struct Unk213ACMap *)gMapData)->unk1E42[
                         gUnknown_03004070 * 0x508]);
    }
    else
    {
        sub_080206E4(&((struct Unk213ACMap *)gMapData)->unk1E42[
                         gUnknown_03004070 * 0x508],
                     1 - gUnknown_03003FC0.unk0d);
    }

    sub_080206E4(&((struct Unk213ACMap *)gMapData)->unk1E42[
                     gUnknown_03004088 * 0x508],
                 1 - gUnknown_03003FC0.unk0d);

    if (gUnknown_03003FC0.unk0d != 0)
    {
        sub_080212AC(1);
        sub_080212AC(2);
        sub_080212AC(3);
        sub_080212AC(4);

        p = sub_0803F5C8(0);

        while (p->unk02_6 != 0)
        {
            xend = p->unk00 + p->unk02_0;
            yend = p->unk01 + p->unk02_3;

            for (x = p->unk00; x < xend; x++)
            {
                for (y = p->unk01; y < yend; y++)
                    sub_080210C8(x, y, 0, 3, 1, 0);
            }

            p++;
        }
    }
}
