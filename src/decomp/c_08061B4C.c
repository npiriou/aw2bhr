#include "global.h"

/* Promoted from assembly; each function below is byte-for-byte
 * identical to the original. Order is address order and must
 * stay that way -- the linker places this file's .text as one
 * contiguous block at 0x08061B4C.
 * sub_08061B4C @ 0x08061B4C
 */

/* MATCHED.
 *
 * The map planes go through the `struct Map` cast idiom of c_08000BF8.c. Plain
 * `u8 *` arithmetic on gMapData reassociates the 0x1432 / 0x417A /
 * 0x12 constants PAST the index -- the ROM adds the plane constant to the base
 * first, then the index -- and costs far more than the tail below.
 * gMapData is named directly at every use rather than bound to a local:
 * the ROM keeps only its ADDRESS in sl and re-loads the pointer each time.
 *
 * gUnitRecords is a REAL declared global, not a pool word, and so is
 * gMapData -- both trip the "named pointer whose target is also named"
 * false positive in the pool-word screen.
 *
 * THE LAST 8 BYTES, and the reason this is worth reading: hand-written C got to
 * 98.0% with the size exact, the relocations exact, the pool exact and all ten
 * words in order -- everything but four instructions:
 *
 *   original            plain `gUnitRecords[c].unk00`
 *   ldr  r0, [pc,#208]  ldr  r0, [pc,#208]
 *   lsls r1, r4, #1     ldr  r1, [r0]
 *   adds r1, r1, r4     lsls r0, r4, #1
 *   lsls r1, r1, #2     adds r0, r0, r4
 *   ldr  r0, [r0]       lsls r0, r0, #2
 *   adds r0, r0, r1     adds r0, r0, r1
 *
 * The ROM computes `c * 12` BETWEEN the pool load of &gUnitRecords and the
 * `ldr r0,[r0]` that dereferences it, hiding the load in the multiply, and gives
 * the multiply r1 rather than r0. No statement boundary reaches inside that
 * address expression, so no rewriting of the `&&` chain moves it.
 *
 * decomp-permuter closed it in 9,826 iterations, and the mutation it found is
 * the ASSIGNMENT-INSIDE-THE-EXPRESSION below: binding the element address to its
 * own local *within* the comparison creates that pseudo at a point ordinary
 * source cannot name, which is the same lever the comma-operator note in
 * docs/agbcc-codegen.md describes for pool ordering. This is the residual class
 * that doc's tail calls the permuter's own -- instruction ORDER, not stack slot
 * -- and it behaved exactly as advertised.
 */

struct Map
{
    /* 0x0000 */ u16 unk00;
    /* 0x0002 */ u16 unk02;
    /* 0x0004 */ u16 unk04;
    /* 0x0006 */ u16 unk06;
    /* 0x0008 */ u8 filler_0008[0x0A];
    /* 0x0012 */ u8 unk0012[0x0508];
    /* 0x051A */ u8 unk051A[0x0F18];
    /* 0x1432 */ u8 unk1432[0x0A10];
    /* 0x1E42 */ u8 unk1E42[0x0508];
    /* 0x234A */ u8 unk234A[0x0508];
    /* 0x2852 */ u8 unk2852[0x0A10];
    /* 0x3262 */ u8 unk3262[0x0508];
    /* 0x376A */ u8 unk376A[0x0508];
    /* 0x3C72 */ u8 unk3C72[0x0508];
    /* 0x417A */ u16 unk417A[0x100];
};
#define MAP ((struct Map *)gMapData)

void sub_08061B4C(void)
{
    int i;
    struct UnitRecord *u;
    int j;
    int k;
    u8 c;

    sub_0801F92C(gMapData + 0x2852);
    sub_080581A4(gMapData + 0x376A, 0);

    for (i = 0; gPropertyList[i].unk00 != 0xFF; i++)
    {
        if ((MAP->unk1432[MAP->unk417A[gPropertyList[i].unk02]
                          + gPropertyList[i].unk01] & 0x1F) == 0x13)
            continue;

        if (sub_08026FD0(gUnknown_03003F2C,
                         MAP->unk1432[MAP->unk417A[gPropertyList[i].unk02]
                                      + gPropertyList[i].unk01]) == 1)
            continue;

        c = MAP->unk0012[MAP->unk417A[gPropertyList[i].unk02]
                         + gPropertyList[i].unk01];

        if (c != 0 && sub_08026F9C(gUnknown_03003F2C, c) == 1
            && (*(u = &gUnitRecords[c])).unk00 <= 2)
            continue;

        gUnknown_030013EC(gPropertyList[i].unk01, gPropertyList[i].unk02, 1, 3, 0);

        for (j = 0; j < MAP->unk02; j++)
        {
            for (k = 0; k < MAP->unk00; k++)
            {
                if ((s8)gUnknown_03003340[j][k] >= 0)
                    MAP->unk376A[MAP->unk417A[j] + k]++;
            }
        }
    }
}
