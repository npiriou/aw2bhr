#include "global.h"

/* Promoted from assembly; each function below is byte-for-byte
 * identical to the original. Order is address order and must
 * stay that way -- the linker places this file's .text as one
 * contiguous block at 0x0804151C.
 * sub_0804151C @ 0x0804151C, sub_080415E4 @ 0x080415E4, sub_080416A4 @ 0x080416A4, sub_08041758 @ 0x08041758
 */

/* Scans the whole map for cells the sub_0804236C predicate accepts, and writes
 * them into the gUnknown_03003F20 list as {x, y, value} triples terminated by a
 * 0xFFFF value word. Returns how many were written.
 *
 * Byte-identical twin of sub_08041758 apart from the predicate: the two differ
 * in exactly one instruction, the `bl` target (sub_0804236C vs sub_0804247C).
 *
 * gUnknown_03003F20 is declared `struct Unk03003338 *` -- a type inherited from
 * its only writer, sub_0803486C. This function reads it as a 4-byte
 * {u8 x; u8 y; s16 v;} record (`strb`, `strb #1`, `strh #2`, `adds r5,#4`), so
 * the cast is deliberate; see include/unknown-globals.h. The declaration is
 * left alone rather than retyped because the writer's value genuinely is the
 * Unk03003338 record pointer -- one scratch buffer, two record layouts.
 *
 * The outer guard compares against sb (the count) rather than y because both
 * are 0 at that point and CSE shares the register -- nothing to author. */

struct Unk4151CCell
{
    /* 00 */ u8 x;
    /* 01 */ u8 y;
    /* 02 */ s16 v;
};
/* Byte-identical twin of sub_0804151C -- 200 bytes, 87 instructions, the same
 * five pool words in the same order, differing in exactly ONE instruction: the
 * predicate called is sub_0804247C here and sub_0804236C there. See
 * src/decomp/c_0804151C.c for the read-out of the shape.
 *
 * That one-instruction equality is also what proves sub_0804247C returns bool8
 * and not the `int` it was promoted as: the `lsls #0x18; lsrs #0x18` after the
 * `bl` is agbcc's re-narrowing of a narrow-returning callee, and an `int`
 * callee would not produce it. See include/unknown-functions.h. */

struct Unk41758Cell
{
    /* 00 */ u8 x;
    /* 01 */ u8 y;
    /* 02 */ s16 v;
};

int sub_0804151C(void)
{
    struct Unk4151CCell *out;
    int count;
    int x;
    int y;

    count = 0;
    out = (struct Unk4151CCell *)gUnknown_03003F20;
    if ((u8)(gUnknown_030040D8->unk00 - 1) > 1)
        return 0;

    for (y = 0; y < *(u16 *)(gMapData + 2); y++)
    {
        for (x = 0; x < *(u16 *)gMapData; x++)
        {
            if ((s8)gUnknown_03003340[y][x] >= 0 && sub_0804236C(x, y) == 1)
            {
                count++;
                out->x = x;
                out->y = y;
                out->v = (s8)gUnknown_03003340[y][x];
                out++;
            }
        }
    }
    out->v = 0xFFFF;
    return count;
}

/* Builds the gUnknown_03003338 list from every map cell whose unit id passes
 * sub_08025EF0 against gUnknown_03003F38, terminates it with a zero unk00, and
 * returns the element count as a pointer difference.
 *
 * The splitter's `gUnknown_08091310` is NOT an object: the ROM word at
 * 0x08091310 holds 0x03003338, so it is agbcc's own -fforce-addr word for
 * gUnknown_03003338 (its neighbour 0x08091314 holds the same address again --
 * the documented one-word-per-(function, symbol) run). Naming the global
 * honestly reproduces the ROM's three-level `ldr rN,=<word>; ldr rM,[rN];
 * ldr rK,[rM]` chain, including the reload at the end for the pointer
 * subtraction.
 *
 * Cell addressing is the c_0804247C.c idiom -- p, then t, then rows, then off,
 * then cells, each its own local -- which is what keeps 0x417A in a pool word
 * and 0x12 on an `adds` rather than folding either into a load displacement.
 *
 * NOT a twin of sub_080416A4 despite the adjacency and the similar size. */
int sub_080415E4(void)
{
    struct Unk03003338 *out;
    u8 *p;
    u8 *rows;
    u8 *cells;
    int t;
    int off;
    u8 cell;
    int x;
    int y;

    out = gUnknown_03003338;
    for (y = 0; y < *(u16 *)(gMapData + 2); y++)
    {
        for (x = 0; x < *(u16 *)gMapData; x++)
        {
            if ((s8)gUnknown_03003340[y][x] >= 0)
            {
                p = gMapData;
                t = y * 2;
                rows = p + 0x417a;
                off = *(u16 *)(rows + t) + x;
                cells = p + 0x12;
                cell = cells[off];
                if (cell != 0 && sub_08025EF0(cell, gUnknown_03003F38))
                {
                    out->unk00 = cell;
                    out++;
                }
            }
        }
    }
    out->unk00 = 0;
    return out - gUnknown_03003338;
}

/* The sub_080415E4 shape with a different predicate and a different order: here
 * sub_0804209C is asked BEFORE the map cell is addressed, and the cell is read
 * only to be stored, so there is no `cell` local and no `!= 0` test.
 * Not a twin of sub_080415E4 -- 81 instructions against 84, four pool words
 * against five.
 *
 * `gUnknown_08091314` is not an object either: the ROM word there holds
 * 0x03003338, the second of the two consecutive -fforce-addr words for
 * gUnknown_03003338 (0x08091310 is sub_080415E4's). One word per
 * (function, symbol), as documented in include/unknown-globals.h. */
int sub_080416A4(void)
{
    struct Unk03003338 *out;
    u8 *p;
    u8 *rows;
    u8 *cells;
    int t;
    int off;
    int x;
    int y;

    out = gUnknown_03003338;
    for (y = 0; y < *(u16 *)(gMapData + 2); y++)
    {
        for (x = 0; x < *(u16 *)gMapData; x++)
        {
            if ((s8)gUnknown_03003340[y][x] >= 0 && sub_0804209C(x, y))
            {
                p = gMapData;
                t = y * 2;
                rows = p + 0x417a;
                off = *(u16 *)(rows + t) + x;
                cells = p + 0x12;
                out->unk00 = cells[off];
                out++;
            }
        }
    }
    out->unk00 = 0;
    return out - gUnknown_03003338;
}

int sub_08041758(void)
{
    struct Unk41758Cell *out;
    int count;
    int x;
    int y;

    count = 0;
    out = (struct Unk41758Cell *)gUnknown_03003F20;
    if ((u8)(gUnknown_030040D8->unk00 - 1) > 1)
        return 0;

    for (y = 0; y < *(u16 *)(gMapData + 2); y++)
    {
        for (x = 0; x < *(u16 *)gMapData; x++)
        {
            if ((s8)gUnknown_03003340[y][x] >= 0 && sub_0804247C(x, y) == 1)
            {
                count++;
                out->x = x;
                out->y = y;
                out->v = (s8)gUnknown_03003340[y][x];
                out++;
            }
        }
    }
    out->v = 0xFFFF;
    return count;
}
