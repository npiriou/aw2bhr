#include "global.h"

/* Promoted from assembly; each function below is byte-for-byte
 * identical to the original. Order is address order and must
 * stay that way -- the linker places this file's .text as one
 * contiguous block at 0x08038848.
 * sub_08038848 @ 0x08038848
 */

/* Promoted from assembly; each function below is byte-for-byte
 * identical to the original. Order is address order and must
 * stay that way -- the linker places this file's .text as one
 * contiguous block at 0x08038848.
 * sub_08038848 @ 0x08038848
 */

/* Pushes one (x, y) step onto the gUnknown_0849D5F8 move stack and records the
 * fuel left after it: unk20/unk2c are the parallel coordinate tables the
 * wave-32 split of that struct predicted, unk38 is the running cost, and the
 * new entry is the previous one minus the terrain cost of the cell stepped on.
 *
 * The cost table is
 * `gCoDataTable[row].unk38[e->unk1e].unk18[gUnknown_03003FC0.unk2c]`,
 * where `e` is gArmyRecords[(gUnknown_03003F38 >> 6) + 1] -- the +1 rides
 * in the 0x59 / 0x5a displacements (0x3c + 0x1d and 0x3c + 0x1e), the same
 * one-based indexing src/decomp/c_080211DC.c uses on that array. `row` is
 * e->unk1d normally and the literal 1 when gUnknown_03003FC0.unk08 is clear,
 * which is why the else arm is the bare constant 0x104 == 1 * sizeof(entry).
 * CoModeData.unk18 was a scalar `s8 *` until this wave; the
 * `(unk1e * 17 + unk2c) << 2` off the +0x50 member base is a 4-byte stride
 * INSIDE the 0x44 entry stride, so it is `s8 *[3]`.
 *
 * THE LOCALS ARE LOAD-BEARING, and they are worth 8 bytes plus a literal-pool
 * level:
 *   - `i` then `stack`, IN THAT ORDER, before the two address bindings. The
 *     ROM reads unk45 and only then does `adds rB,#0x38` on the struct base.
 *     Binding `stack` first swaps those two instructions; dropping `stack` and
 *     writing `&p->unk38[p->unk45]` reassociates to `(i + 0x38) + p`; dropping
 *     the address locals entirely computes both addresses AFTER the table
 *     lookup, needs r8/ip for the two parameters, and pushes agbcc into
 *     force-addr'ing gUnknown_0849D5F8 into this unit's .rodata -- the ROM
 *     reaches that symbol with a plain pool word.
 *   - `c` for the whole cell index, exactly as src/decomp/c_08041EA8.c writes
 *     the same expression. Inlining it into the subscript swaps r1 and r2 in
 *     the last block (6 bytes), and reversing the two addends reschedules the
 *     gUnitTypeData lookup ahead of the cell read. */
struct Unk38848Map
{
    /* 0x0000 */ u8 filler_0000[0x1432];
    /* 0x1432 */ u8 plane[0x417A - 0x1432];
    /* 0x417A */ u16 rowOffset[1];
};

void sub_08038848(s8 a, s8 b)
{
    struct Unk38848Map *map;
    s8 *costs;
    s8 *stack;
    s8 *cur;
    s8 *prev;
    int i;
    int c;

    gUnknown_0849D5F8->unk45++;
    gUnknown_0849D5F8->unk20[gUnknown_0849D5F8->unk45] = a;
    gUnknown_0849D5F8->unk2c[gUnknown_0849D5F8->unk45] = b;

    i = gUnknown_0849D5F8->unk45;
    stack = gUnknown_0849D5F8->unk38;
    cur = &stack[i];
    prev = &stack[i - 1];

    costs = gCoDataTable[gUnknown_03003FC0.unk08
                ? gArmyRecords[(gUnknown_03003F38 >> 6) + 1].unk1d
                : 1]
            .unk38[gArmyRecords[(gUnknown_03003F38 >> 6) + 1].unk1e]
            .unk18[gUnknown_03003FC0.unk2c];

    map = (struct Unk38848Map *)gMapData;

    c = (map->plane[map->rowOffset[b] + a] & 0x1f)
        + gUnitTypeData[gUnknown_030040D8->unk00].unk19 * 32;

    *cur = *prev - costs[c];
}
