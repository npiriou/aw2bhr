#include "global.h"

/* Promoted from assembly; each function below is byte-for-byte
 * identical to the original. Order is address order and must
 * stay that way -- the linker places this file's .text as one
 * contiguous block at 0x0805AE88.
 * sub_0805AE88 @ 0x0805AE88, sub_0805AF90 @ 0x0805AF90, sub_0805B0AC @ 0x0805B0AC, sub_0805B1CC @ 0x0805B1CC, sub_0805B2EC @ 0x0805B2EC
 */

/* Map-redraw family, 0x0805A/0x0805B. Same shape as sub_0805AF90 (see
 * work/sub_0805AF90) with exactly four measured differences, and this file is
 * the one that never returns: it has NO epilogue at all, its bottom being
 * `bl sub_0805B744; b <top>`.
 *
 * The four, against sub_0805AF90:
 *   1. sub_0805B5BC returning false calls sub_0805AF90() and falls through,
 *      where sub_0805AF90 itself returns.
 *   2. no epilogue -- consequence of (1) leaving the loop no exit.
 *   3. the indirect hook's third argument is the literal 1, not
 *      gUnknown_030046D4, which is why this function's data_refs lack it. The
 *      `movs r2,#1` lands AFTER the two pool loads because argument setup is
 *      grouped by operand class (copies, pool ldrs, then mov #imm8), not by
 *      argument order.
 *   4. the loop-continue test is sub_08058318() < sub_0805848C(), with no `+ 5`.
 * Everything else that differs in the listings is register choice and is not
 * reachable from the source.
 *
 * The double loop is c_0805AD90.c's sub_0805AD90 verbatim -- see that file's
 * header for why `rows` must be its own local and why the `do { } while (0)`
 * around the two pointer bumps is a register-allocation lever, not control
 * flow. MATCHED first attempt. */
void sub_0805AE88(void)
{
    int it;
    int i;
    int a;
    int b;
    int x;
    int y;
    u8 *p;
    u8 *rows;
    u8 *cells;
    int t;
    int off;
    int terrain;

    i = -1;
    it = sub_0805B4A8();

    for (;;)
    {
        i++;
        if ((u8)sub_0805B5BC(&it, &i, &a, &b) == 0)
            sub_0805AF90();

        gUnknown_030013EC(a, b, 1, gUnknown_085766E0->unk0f, 0);

        if (sub_08058318() < sub_0805848C())
        {
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
                        cells = p + 0x1432;
                        terrain = cells[off] & 0x1f;
                        if (terrain == 0xd || terrain == 0xb)
                        {
                            do
                            {
                                p += 0x3c72;
                                p += off;
                            } while (0);
                            (*p)++;
                        }
                    }
                }
            }

            sub_0805B744();
        }
    }
}

/* Map-redraw family, 0x0805A/0x0805B, and the representative the other four
 * were derived from. Loops on sub_0805B5BC (the sub_0805B4D8 twin that walks a
 * list, seeded from sub_0805B4A8()), hands its two out-params to the
 * gUnknown_030013EC indirect hook, and -- while the frame budget allows --
 * sweeps every passable cell and bumps the per-cell counter in the map's 0x3C72
 * plane wherever the terrain code is 0xd or 0xb.
 *
 * `i` is preset to -1 and bumped at the top of the loop, so the first
 * sub_0805B5BC call sees 0. The three later argument addresses (&i, &a, &b) are
 * LICM-hoisted into sp+0x14, sp+0x18 and sl -- compiler output, not source.
 * sub_0805B5BC's result is truth-tested one byte wide; see the note added at
 * its declaration in unknown-functions.h for why the `(u8)` is written as a
 * cast at the call rather than by retyping the shared prototype.
 *
 * The double loop is c_0805AD90.c's sub_0805AD90 verbatim -- see that file's
 * header for why `rows` must be its own local (else agbcc reassociates the
 * 0x417A pool constant to last) and why the `do { } while (0)` around the two
 * pointer bumps is a register-allocation lever, not control flow.
 *
 * MATCHED first attempt, straight off the exemplar with no probing. */
void sub_0805AF90(void)
{
    int it;
    int i;
    int a;
    int b;
    int x;
    int y;
    u8 *p;
    u8 *rows;
    u8 *cells;
    int t;
    int off;
    int terrain;

    i = -1;
    it = sub_0805B4A8();

    for (;;)
    {
        i++;
        if ((u8)sub_0805B5BC(&it, &i, &a, &b) == 0)
            return;

        gUnknown_030013EC(a, b, gUnknown_030046D4, gUnknown_085766E0->unk0f, 0);

        if (sub_08058318() < sub_080585D4() + 5)
        {
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
                        cells = p + 0x1432;
                        terrain = cells[off] & 0x1f;
                        if (terrain == 0xd || terrain == 0xb)
                        {
                            do
                            {
                                p += 0x3c72;
                                p += off;
                            } while (0);
                            (*p)++;
                        }
                    }
                }
            }

            sub_0805B744();
        }
    }
}

/* Map-redraw family, 0x0805A/0x0805B. Returning variant, like sub_0805AF90:
 * the driver returning false is an early `return` and the function has a real
 * epilogue. It drives on sub_0805B6A0 and its loop-continue test is the
 * TWO-PART form that distinguishes this pair from sub_0805AF90's single
 * comparison:
 *
 *     n = sub_080583DC();
 *     if (n != 0 && sub_08058254() < n + 5)
 *
 * -- the `+ K` operand is evaluated FIRST, held in r4, zero-tested, and only
 * then compared against the second query. That forces `n` to be a real local;
 * sub_0805AF90's one-comparison form needs none. Note this is the same pair of
 * counters and the same `+ 5` that unknown-functions.h records at sub_0805BEA0,
 * with the operands the other way round.
 *
 * The two LICM-hoisted argument addresses land in sp+0x18 and sp+0x14 here,
 * swapped against sub_0805AF90's sp+0x14 and sp+0x18. That is the loop
 * optimiser reacting to `n` being live across the preheader -- compiler output,
 * not source, and it falls out on its own.
 *
 * The double loop is c_0805AD90.c's sub_0805AD90 verbatim -- see that file's
 * header for why `rows` must be its own local and why the `do { } while (0)`
 * around the two pointer bumps is a register-allocation lever, not control
 * flow. MATCHED first attempt. */
void sub_0805B0AC(void)
{
    int it;
    int i;
    int a;
    int b;
    int n;
    int x;
    int y;
    u8 *p;
    u8 *rows;
    u8 *cells;
    int t;
    int off;
    int terrain;

    i = -1;
    it = sub_0805B4A8();

    for (;;)
    {
        i++;
        if ((u8)sub_0805B6A0(&it, &i, &a, &b) == 0)
            return;

        gUnknown_030013EC(a, b, gUnknown_030046D4, gUnknown_085766E0->unk0f, 0);

        n = sub_080583DC();
        if (n != 0 && sub_08058254() < n + 5)
        {
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
                        cells = p + 0x1432;
                        terrain = cells[off] & 0x1f;
                        if (terrain == 0xd || terrain == 0xb)
                        {
                            do
                            {
                                p += 0x3c72;
                                p += off;
                            } while (0);
                            (*p)++;
                        }
                    }
                }
            }

            sub_0805B744();
        }
    }
}

/* Map-redraw family, 0x0805A/0x0805B. sub_0805B0AC's twin, instruction for
 * instruction: same returning shape with a real epilogue, same two-part
 * loop-continue test with the `+ K` operand evaluated first into r4 and
 * zero-tested before the second query. It differs from sub_0805B0AC in exactly
 * three tokens -- it drives on sub_0805B5BC rather than sub_0805B6A0, its two
 * counters are sub_080585D4 and sub_0805848C rather than sub_080583DC and
 * sub_08058254, and the margin is `+ 2` rather than `+ 5`.
 *
 * The double loop is c_0805AD90.c's sub_0805AD90 verbatim -- see that file's
 * header for why `rows` must be its own local and why the `do { } while (0)`
 * around the two pointer bumps is a register-allocation lever, not control
 * flow. MATCHED first attempt. */
void sub_0805B1CC(void)
{
    int it;
    int i;
    int a;
    int b;
    int n;
    int x;
    int y;
    u8 *p;
    u8 *rows;
    u8 *cells;
    int t;
    int off;
    int terrain;

    i = -1;
    it = sub_0805B4A8();

    for (;;)
    {
        i++;
        if ((u8)sub_0805B5BC(&it, &i, &a, &b) == 0)
            return;

        gUnknown_030013EC(a, b, gUnknown_030046D4, gUnknown_085766E0->unk0f, 0);

        n = sub_080585D4();
        if (n != 0 && sub_0805848C() < n + 2)
        {
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
                        cells = p + 0x1432;
                        terrain = cells[off] & 0x1f;
                        if (terrain == 0xd || terrain == 0xb)
                        {
                            do
                            {
                                p += 0x3c72;
                                p += off;
                            } while (0);
                            (*p)++;
                        }
                    }
                }
            }

            sub_0805B744();
        }
    }
}

/* Map-redraw family, 0x0805A/0x0805B. sub_0805AE88's twin: same no-epilogue
 * shape, same `if (!driver(...)) sub_0805AF90();` fall-through. It differs from
 * sub_0805AE88 in only two things -- it drives on sub_0805B6A0 rather than
 * sub_0805B5BC (the same signature, minus the terrain predicate), and its
 * loop-continue test is a bare truth test, `sub_080586CC() != 0`, with no
 * second query and no `+ K`. Its indirect hook takes gUnknown_030046D4 as the
 * third argument the way sub_0805AF90 does, not sub_0805AE88's literal 1.
 *
 * The double loop is c_0805AD90.c's sub_0805AD90 verbatim -- see that file's
 * header for why `rows` must be its own local and why the `do { } while (0)`
 * around the two pointer bumps is a register-allocation lever, not control
 * flow. MATCHED first attempt (the one failed run was a missing prototype for
 * sub_080586CC, now published in unknown-functions.h from its promoted
 * definition in src/decomp/c_080586CC.c). */
void sub_0805B2EC(void)
{
    int it;
    int i;
    int a;
    int b;
    int x;
    int y;
    u8 *p;
    u8 *rows;
    u8 *cells;
    int t;
    int off;
    int terrain;

    i = -1;
    it = sub_0805B4A8();

    for (;;)
    {
        i++;
        if ((u8)sub_0805B6A0(&it, &i, &a, &b) == 0)
            sub_0805AF90();

        gUnknown_030013EC(a, b, gUnknown_030046D4, gUnknown_085766E0->unk0f, 0);

        if (sub_080586CC() != 0)
        {
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
                        cells = p + 0x1432;
                        terrain = cells[off] & 0x1f;
                        if (terrain == 0xd || terrain == 0xb)
                        {
                            do
                            {
                                p += 0x3c72;
                                p += off;
                            } while (0);
                            (*p)++;
                        }
                    }
                }
            }

            sub_0805B744();
        }
    }
}
