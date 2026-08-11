#include "global.h"

/* Promoted from assembly; each function below is byte-for-byte
 * identical to the original. Order is address order and must
 * stay that way -- the linker places this file's .text as one
 * contiguous block at 0x0805C2DC.
 * sub_0805C2DC @ 0x0805C2DC, sub_0805C514 @ 0x0805C514, sub_0805C720 @ 0x0805C720
 */

/* Scores every unit in the armies whose bit is set in
 * gArmyRecords[a1].unk2c and returns the slot number of the best one.
 *
 * struct Map is the local cast-on view of gMapData that
 * include/unknown-globals.h prescribes: the ROM computes every plane address as
 * `(map + K) + idx`, and only a COMPONENT_REF preserves that association.
 * gUnknown_0816D9D8 is NOT a global -- the ROM word there holds 0x08499590, so
 * it is agbcc's own -fforce-addr constant for gMapData; the honest
 * spelling reproduces it and the promotion carries the rodata word.
 *
 * Three things were each worth the whole match:
 *  - `e` is ONE pointer local reused for the outer slot record and the cell's
 *    record. The outer one has to be a pointer, not a subscript, or the base
 *    reload after sub_0801F838 costs three instructions.
 *  - `n` must be DECLARED BEFORE `i`: their reload spill slots are handed out in
 *    declaration order, and the ROM's are 0x18 (n) then 0x1c (i). Same lever
 *    orders best/score/bestN into 0xc/0x10/0x14.
 *  - the score term is `unk04_0 * (call / 10)`, field FIRST. gcc 2.x's
 *    preexpand_calls hoists the sub_08042C9C call out ahead of the whole
 *    expression, so the field load lands between it and __divsi3 exactly as the
 *    ROM has it; writing `call / 10 * unk04_0` puts the load after the divide.
 *
 * MATCHED. */

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
    /* 0x2852 */ u8 unk2852[0x1928];
    /* 0x417A */ u16 unk417A[0x100];
};

u8 sub_0805C2DC(u16 a1, u8 a2)
{
    int n;
    int i;
    int x;
    int y;
    int best;
    int score;
    u8 bestN;
    struct UnitRecord *e;

    best = 0;
    bestN = 0;

    for (i = 0; i <= 3; i++)
    {
        if (((gArmyRecords[a1].unk2c >> i) & 1) == 0)
            continue;

        for (n = i * 64; n < i * 64 + 64; n++)
        {
            e = &gUnitRecords[n];
            if (e->unk00 == 0)
                continue;

            score = 0;
            sub_0801F838(0xff);
            sub_0801F9C0(e->unk02, e->unk03, 2, 0);

            for (y = 0; y < ((struct Map *)gMapData)->unk02; y++)
            {
                for (x = 0; x < ((struct Map *)gMapData)->unk00; x++)
                {
                    if ((s8)gUnknown_03003340[y][x] < 0)
                        continue;
                    if (((struct Map *)gMapData)->unk051A[((struct Map *)gMapData)->unk417A[y] + x] == 0)
                        continue;
                    if (a2 != 0 && !sub_08020DBC(a1, x, y))
                        continue;
                    e = &gUnitRecords[((struct Map *)gMapData)->unk051A[((struct Map *)gMapData)->unk417A[y] + x]];
                    if (e->unk00 == 0x18)
                    {
                        if ((e->unk01 & 0x20) != 0)
                            continue;
                        if (!sub_080257C0(((struct Map *)gMapData)->unk0012[((struct Map *)gMapData)->unk417A[y] + x]))
                            continue;
                    }
                    if (e->unk04_0 <= 10)
                        continue;
                    if (sub_08026F28(a1, (((struct Map *)gMapData)->unk051A[((struct Map *)gMapData)->unk417A[y] + x] >> 6) + 1) == 1)
                        score -= e->unk04_0 * (sub_08042C9C(gCurrentArmyIndex, e->unk00) / 10);
                    else
                        score += e->unk04_0 * (sub_08042C9C(gCurrentArmyIndex, e->unk00) / 10);
                }
            }

            if (score > best)
            {
                bestN = n;
                best = score;
            }
        }
    }

    return bestN;
}

u8 sub_0805C514(u16 a1, u8 a2)
{
    int n;
    int i;
    int x;
    int y;
    int best;
    int score;
    u8 bestN;
    struct UnitRecord *e;

    best = 0;
    bestN = 0;

    for (i = 0; i <= 3; i++)
    {
        if (((gArmyRecords[a1].unk2c >> i) & 1) == 0)
            continue;

        for (n = i * 64; n < i * 64 + 64; n++)
        {
            e = &gUnitRecords[n];
            if (e->unk00 == 0)
                continue;

            score = 0;
            sub_0801F838(0xff);
            sub_0801F9C0(e->unk02, e->unk03, 2, 0);

            for (y = 0; y < ((struct Map *)gMapData)->unk02; y++)
            {
                for (x = 0; x < ((struct Map *)gMapData)->unk00; x++)
                {
                    if ((s8)gUnknown_03003340[y][x] < 0)
                        continue;
                    if (((struct Map *)gMapData)->unk051A[((struct Map *)gMapData)->unk417A[y] + x] == 0)
                        continue;
                    if (a2 != 0 && !sub_08020DBC(a1, x, y))
                        continue;
                    e = &gUnitRecords[((struct Map *)gMapData)->unk051A[((struct Map *)gMapData)->unk417A[y] + x]];
                    if (e->unk00 == 0x18)
                    {
                        if ((e->unk01 & 0x20) != 0)
                            continue;
                        if (!sub_080257C0(((struct Map *)gMapData)->unk0012[((struct Map *)gMapData)->unk417A[y] + x]))
                            continue;
                    }
                    if (e->unk04_0 <= 10)
                        continue;
                    if (sub_08026F28(a1, (((struct Map *)gMapData)->unk051A[((struct Map *)gMapData)->unk417A[y] + x] >> 6) + 1) == 1)
                        score -= e->unk04_0;
                    else
                        score += e->unk04_0;
                }
            }

            if (score > best)
            {
                bestN = n;
                best = score;
            }
        }
    }

    return bestN;
}

u8 sub_0805C720(u16 a1, u8 a2)
{
    int n;
    int i;
    int x;
    int y;
    int best;
    int score;
    u8 bestN;
    int mul;
    struct UnitRecord *e;

    best = 0;
    bestN = 0;

    for (i = 0; i <= 3; i++)
    {
        if (((gArmyRecords[a1].unk2c >> i) & 1) == 0)
            continue;

        for (n = i * 64; n < i * 64 + 64; n++)
        {
            e = &gUnitRecords[n];
            if (e->unk00 == 0)
                continue;

            score = 0;
            sub_0801F838(0xff);
            sub_0801F9C0(e->unk02, e->unk03, 2, 0);

            for (y = 0; y < ((struct Map *)gMapData)->unk02; y++)
            {
                for (x = 0; x < ((struct Map *)gMapData)->unk00; x++)
                {
                    if ((s8)gUnknown_03003340[y][x] < 0)
                        continue;
                    if (((struct Map *)gMapData)->unk051A[((struct Map *)gMapData)->unk417A[y] + x] == 0)
                        continue;
                    if (a2 != 0 && !sub_08020DBC(a1, x, y))
                        continue;
                    e = &gUnitRecords[((struct Map *)gMapData)->unk051A[((struct Map *)gMapData)->unk417A[y] + x]];
                    if (e->unk00 == 0x18)
                    {
                        if ((e->unk01 & 0x20) != 0)
                            continue;
                        if (!sub_080257C0(((struct Map *)gMapData)->unk0012[((struct Map *)gMapData)->unk417A[y] + x]))
                            continue;
                    }
                    if (e->unk04_0 <= 10)
                        continue;
                    mul = 1;
                    if (gUnitTypeData[e->unk00].unk0e > 1)
                        mul = 2;
                    if (sub_08026F28(a1, (((struct Map *)gMapData)->unk051A[((struct Map *)gMapData)->unk417A[y] + x] >> 6) + 1) == 1)
                        score -= e->unk04_0 * (sub_08042C9C(gCurrentArmyIndex, e->unk00) / 10) * mul;
                    else
                        score += e->unk04_0 * (sub_08042C9C(gCurrentArmyIndex, e->unk00) / 10) * mul;
                }
            }

            if (score > best)
            {
                bestN = n;
                best = score;
            }
        }
    }

    return bestN;
}
