#include "global.h"

/* Promoted from assembly; each function below is byte-for-byte
 * identical to the original. Order is address order and must
 * stay that way -- the linker places this file's .text as one
 * contiguous block at 0x08045848.
 * sub_08045848 @ 0x08045848, sub_08045874 @ 0x08045874, sub_080458A0 @ 0x080458A0, sub_080458CC @ 0x080458CC, sub_080458F8 @ 0x080458F8
 */

/* One of five near-identical predicates over the gMapData screen
 * struct: read the u16 row offset at +0x4184, index the byte array that starts
 * 0x143f bytes into the struct with it, and test the top three bits of that
 * byte for the value 1. All five use `lsrs #5` and `cmp #1` -- diffed one by
 * one, the shift kind, the shift amount and the compare constant are the same
 * across the whole family and only the two pool constants move.
 *
 * The byte is read with an explicit `*(p + ... )` and NOT with `p[...]`: the
 * subscript spelling puts the deref'd pointer in r1 and the running address in
 * r0, which is the ROM's allocation mirrored. See docs/agbcc-codegen.md.
 *
 * `if (c) return 1; return 0;` and not `return c;` -- the ROM materialises
 * BOTH constants and puts its literal pool between the two return blocks,
 * which the short boolean form (16 bytes, no `movs r0,#0`) cannot produce.
 * A leaf: no push, and the epilogue is a bare `bx lr`.
 */
int sub_08045848(void)
{
    u8 *p;

    p = gMapData;

    if (*(p + *(u16 *)(p + 0x4184) + 0x143f) >> 5 == 1)
        return 1;

    return 0;
}

/* One of five near-identical predicates over the gMapData screen
 * struct: read the u16 row offset at +0x417e, index the byte array that starts
 * 0x1441 bytes into the struct with it, and test the top three bits of that
 * byte for the value 1. All five use `lsrs #5` and `cmp #1` -- diffed one by
 * one, the shift kind, the shift amount and the compare constant are the same
 * across the whole family and only the two pool constants move.
 *
 * The byte is read with an explicit `*(p + ... )` and NOT with `p[...]`: the
 * subscript spelling puts the deref'd pointer in r1 and the running address in
 * r0, which is the ROM's allocation mirrored. See docs/agbcc-codegen.md.
 *
 * `if (c) return 1; return 0;` and not `return c;` -- the ROM materialises
 * BOTH constants and puts its literal pool between the two return blocks,
 * which the short boolean form (16 bytes, no `movs r0,#0`) cannot produce.
 * A leaf: no push, and the epilogue is a bare `bx lr`.
 */
int sub_08045874(void)
{
    u8 *p;

    p = gMapData;

    if (*(p + *(u16 *)(p + 0x417e) + 0x1441) >> 5 == 1)
        return 1;

    return 0;
}

/* One of five near-identical predicates over the gMapData screen
 * struct: read the u16 row offset at +0x418a, index the byte array that starts
 * 0x1435 bytes into the struct with it, and test the top three bits of that
 * byte for the value 1. All five use `lsrs #5` and `cmp #1` -- diffed one by
 * one, the shift kind, the shift amount and the compare constant are the same
 * across the whole family and only the two pool constants move.
 *
 * The byte is read with an explicit `*(p + ... )` and NOT with `p[...]`: the
 * subscript spelling puts the deref'd pointer in r1 and the running address in
 * r0, which is the ROM's allocation mirrored. See docs/agbcc-codegen.md.
 *
 * `if (c) return 1; return 0;` and not `return c;` -- the ROM materialises
 * BOTH constants and puts its literal pool between the two return blocks,
 * which the short boolean form (16 bytes, no `movs r0,#0`) cannot produce.
 * A leaf: no push, and the epilogue is a bare `bx lr`.
 */
int sub_080458A0(void)
{
    u8 *p;

    p = gMapData;

    if (*(p + *(u16 *)(p + 0x418a) + 0x1435) >> 5 == 1)
        return 1;

    return 0;
}

/* One of five near-identical predicates over the gMapData screen
 * struct: read the u16 row offset at +0x418a, index the byte array that starts
 * 0x1438 bytes into the struct with it, and test the top three bits of that
 * byte for the value 1. All five use `lsrs #5` and `cmp #1` -- diffed one by
 * one, the shift kind, the shift amount and the compare constant are the same
 * across the whole family and only the two pool constants move.
 *
 * The byte is read with an explicit `*(p + ... )` and NOT with `p[...]`: the
 * subscript spelling puts the deref'd pointer in r1 and the running address in
 * r0, which is the ROM's allocation mirrored. See docs/agbcc-codegen.md.
 *
 * `if (c) return 1; return 0;` and not `return c;` -- the ROM materialises
 * BOTH constants and puts its literal pool between the two return blocks,
 * which the short boolean form (16 bytes, no `movs r0,#0`) cannot produce.
 * A leaf: no push, and the epilogue is a bare `bx lr`.
 */
int sub_080458CC(void)
{
    u8 *p;

    p = gMapData;

    if (*(p + *(u16 *)(p + 0x418a) + 0x1438) >> 5 == 1)
        return 1;

    return 0;
}

/* The fifth member of the sub_08045848 family, and the only one whose address
 * arithmetic is associated the other way round: the ROM adds 0x1432 to the
 * base BEFORE the `ldrh` of the row offset and ends `(p + 0x1432) + row`,
 * where its four siblings end `(p + row) + K` with the constant's pool load
 * last. Same instruction multiset, same 44 bytes -- only the order moves, so
 * there is no size signal.
 *
 * Binding the biased base to a second pointer local is what reaches it. Ruled
 * out on one probe, all three producing the siblings' order or the r0/r1 swap:
 * `*(p + 0x1432 + row)` with the constant written first, `(p + 0x1432)[row]`,
 * and the same two with no local at all.
 */
int sub_080458F8(void)
{
    u8 *p;
    u8 *q;

    p = gMapData;
    q = p + 0x1432;

    if (*(q + *(u16 *)(p + 0x418c)) >> 5 == 1)
        return 1;

    return 0;
}
