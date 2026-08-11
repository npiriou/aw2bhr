#include "global.h"

/* Promoted from assembly; each function below is byte-for-byte
 * identical to the original. Order is address order and must
 * stay that way -- the linker places this file's .text as one
 * contiguous block at 0x0804209C.
 * sub_0804209C @ 0x0804209C, sub_08042154 @ 0x08042154, sub_080421D0 @ 0x080421D0, sub_0804223C @ 0x0804223C, sub_080422A8 @ 0x080422A8, sub_0804236C @ 0x0804236C
 */

/* Is there an enemy unit standing on (x, y) whose displayed stats are stale?
 * The bounds pair, the +0x417A row table and the unit-id plane at +0x51A are
 * the c_08001158.c map-cell idiom; gUnknown_03003F2C holds the army bits the
 * cell's top two must equal.
 *
 * gMapData is read through the GLOBAL in the bounds chain rather than
 * through a local bound before it. CSE still emits one `ldr` and reuses it for
 * everything after, but binding a local first creates the pseudo ahead of the
 * `x < 0` test -- which both hoists the load above that branch and pushes the
 * zero-extended y out of r1 into r4.
 *
 * The two tail compares are the stat check: unk0b caps the 4-bit field at bit 7
 * of the unit's unk04 container and unk10 caps the 7-bit unk06, exactly the
 * pairing gUnitTypeData's own comments record for sub_08042998. */
bool8 sub_0804209C(s16 x, s16 y)
{
    u8 *p;
    u8 *rows;
    u8 *units;
    struct UnitRecord *unit;
    struct UnitTypeData *e;
    int y2;
    int off;
    int id;

    if (x < 0 || x >= *(u16 *)gMapData
     || y < 0 || y >= *(u16 *)(gMapData + 2))
        return FALSE;

    p = gMapData;
    y2 = y * 2;
    rows = p + 0x417a;
    off = *(u16 *)(rows + y2) + x;
    units = p + 0x51a;
    id = units[off];

    if (id == 0)
        return FALSE;

    if ((id & 0xc0) != gUnknown_03003F2C)
        return FALSE;

    unit = &gUnitRecords[id];

    if (unit == (struct UnitRecord *)gUnknown_030040D8)
        return FALSE;

    if (gUnknown_084995A8[unit->unk00] == 0)
        return FALSE;

    e = &gUnitTypeData[unit->unk00];

    if (e->unk0b != unit->unk04_7)
        return TRUE;

    if (e->unk10 != unit->unk06_0)
        return TRUE;

    return FALSE;
}

/* The OR of sub_080421D0 and sub_0804223C: passable if EITHER flag byte
 * clears sub_08041F38. The `||` with an explicit `else` is what puts the
 * `movs r0,#1` in the fallthrough slot after the second `beq`; the chained
 * `if (a) return TRUE; if (b) return TRUE;` spelling emits the zero arm
 * there instead, as sub_080421D0 does. */
bool8 sub_08042154(struct Unk030040D8 *a1, s16 a2, s16 a3)
{
    u8 *t;
    u8 *p;
    u8 *rows;
    u8 *cells;
    u8 *costs;
    int y2;
    int off;
    int idx;

    t = gUnitTypeData[a1->unk00].unk14;
    p = gMapData;
    y2 = a3 * 2;
    rows = p + 0x417a;
    off = *(u16 *)(rows + y2) + a2;
    cells = p + 0x1432;
    idx = cells[off] & 0x1f;
    costs = t + 0x1a;

    if (costs[idx] == 0)
        return FALSE;

    if (sub_08041F38(a2, a3, a1->unk07[0]) != 0
     || sub_08041F38(a2, a3, a1->unk07[1]) != 0)
        return TRUE;
    else
        return FALSE;
}

/* The c_0804247C.c map-cell idiom plus a per-terrain movement-cost lookup:
 * gUnitTypeData[type].unk14 points at a blob whose +0x1a is a table of
 * cost bytes indexed by the cell's low 5 bits, and a zero entry means the
 * terrain is impassable for this unit type.
 *
 * `idx` and `costs` are SEPARATE statements and the order between them is
 * load-bearing. Writing the lookup as `t[0x1a + idx]` folds 0x1a into the
 * `ldrb` displacement; binding `costs = t + 0x1a` earlier emits the
 * `adds r4,#0x1a` before the cell load. Only cells -> idx -> costs puts the
 * add between the `ands` and the `ldrb`, where the ROM has it. */
bool8 sub_080421D0(struct Unk030040D8 *a1, s16 a2, s16 a3)
{
    u8 *t;
    u8 *p;
    u8 *rows;
    u8 *cells;
    u8 *costs;
    int y2;
    int off;
    int idx;

    t = gUnitTypeData[a1->unk00].unk14;
    p = gMapData;
    y2 = a3 * 2;
    rows = p + 0x417a;
    off = *(u16 *)(rows + y2) + a2;
    cells = p + 0x1432;
    idx = cells[off] & 0x1f;
    costs = t + 0x1a;

    if (costs[idx] == 0)
        return FALSE;

    if (sub_08041F38(a2, a3, a1->unk07[0]) != 0)
        return TRUE;

    return FALSE;
}

/* sub_080421D0's twin, differing only in which of the two adjacent flag bytes
 * at +7/+8 it hands to sub_08041F38. See c_080421D0.c for why `idx` and
 * `costs` are separate statements in that order. */
bool8 sub_0804223C(struct Unk030040D8 *a1, s16 a2, s16 a3)
{
    u8 *t;
    u8 *p;
    u8 *rows;
    u8 *cells;
    u8 *costs;
    int y2;
    int off;
    int idx;

    t = gUnitTypeData[a1->unk00].unk14;
    p = gMapData;
    y2 = a3 * 2;
    rows = p + 0x417a;
    off = *(u16 *)(rows + y2) + a2;
    cells = p + 0x1432;
    idx = cells[off] & 0x1f;
    costs = t + 0x1a;

    if (costs[idx] == 0)
        return FALSE;

    if (sub_08041F38(a2, a3, a1->unk07[1]) != 0)
        return TRUE;

    return FALSE;
}

/* Five values stay live across the two calls -- the address of
 * gMapData, the 0x417A constant, y * 2, the narrowed x and the unit
 * pointer -- which is what the sl/sb/r8 saves are for, not a loop.
 *
 * The post-call half needs its OWN p/rows/off locals. Sharing them with the
 * pre-call half keeps p live past `ids`, and the ROM's single `adds r2,#0x12`
 * (which consumes p in place) then comes out as a copy plus an add.
 *
 * unit is bound BEFORE the `id == 0` test, the same ordering sub_08041F38
 * uses, and the sub_08025F74 argument is respelled &gUnitRecords[id]
 * rather than reusing unit: that global is a non-const pointer, so the first
 * `bl` kills its MEM and the ROM reloads it. */
bool8 sub_080422A8(s16 x, s16 y)
{
    u8 *p;
    u8 *rows;
    u8 *ids;
    u8 *p2;
    u8 *rows2;
    u8 *cells;
    u8 *costs;
    u8 *t;
    struct UnitRecord *unit;
    int y2;
    int off;
    int off2;
    int id;
    int idx;

    p = gMapData;
    y2 = y * 2;
    rows = p + 0x417a;
    off = *(u16 *)(rows + y2) + x;
    ids = p + 0x12;
    id = ids[off];
    unit = &gUnitRecords[id];

    if (id == 0)
        return FALSE;

    if (sub_08026F9C(id, gUnknown_03003F38) == 0)
        return FALSE;

    if (sub_08025F74(&gUnitRecords[id], gUnknown_030040D8->unk00) == 0)
        return FALSE;

    t = gUnitTypeData[unit->unk00].unk14;

    p2 = gMapData;
    rows2 = p2 + 0x417a;
    off2 = *(u16 *)(rows2 + y2) + x;
    cells = p2 + 0x1432;
    idx = cells[off2] & 0x1f;
    costs = t + 0x1a;

    if (costs[idx] == 0)
        return FALSE;

    return TRUE;
}

/* Reads the same cell TWICE around the sub_08026FD0 call, and the second read
 * needs its own set of locals. Reusing p/rows/off/cells for both halves makes
 * each one pseudo whose live range spans the `bl`, which pushes the 0x1432
 * constant into a callee-saved register and adds r7 to the push list; the ROM
 * keeps it in scratch r0. Only y * 2 and the narrowed x survive the call, in
 * r5 and r4. */
bool8 sub_0804236C(s16 x, s16 y)
{
    u8 army;
    u8 *p;
    u8 *rows;
    u8 *cells;
    u8 *p2;
    u8 *rows2;
    u8 *cells2;
    int y2;
    int off;
    int off2;

    army = gUnknown_03003F38;
    p = gMapData;
    y2 = y * 2;
    rows = p + 0x417a;
    off = *(u16 *)(rows + y2) + x;
    cells = p + 0x1432;

    if (sub_08026FD0(army, cells[off]) == 1)
        return FALSE;

    p2 = gMapData;
    rows2 = p2 + 0x417a;
    off2 = *(u16 *)(rows2 + y2) + x;
    cells2 = p2 + 0x1432;

    switch (cells2[off2] & 0x1f)
    {
    case 6:
    case 8:
    case 10:
    case 11:
    case 14:
    case 20:
        return TRUE;
    default:
        return FALSE;
    }
}
