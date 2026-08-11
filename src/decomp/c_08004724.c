#include "global.h"

/* Promoted from assembly; each function below is byte-for-byte
 * identical to the original. Order is address order and must
 * stay that way -- the linker places this file's .text as one
 * contiguous block at 0x08004724.
 * sub_08004724 @ 0x08004724
 */

/* MATCHED, wave 42 (W42-B2), via decomp-permuter from a 57.4% size-exact
 * hand draft. PROMOTION NEEDS THE .rodata POOL WORD PLACED -- add
 *     "rodata": ["0x0808D7A8"]
 * to this function's data/promoted.json entry, then re-run
 * tools/split_rodata.py and tools/gen_lds.py before building. trymatch
 * reports `relocs: name different symbols that resolve to the same address`,
 * which is the false-mismatch case, not a difference.
 *
 * The two levers, both of which the permuter found and neither of which was
 * on the wave-40 draft's list of things to try:
 *   - `m = MAP;` INSIDE the inner loop body, used for the two stores' bases
 *     while the rowOffset reads still go through the global expression. The
 *     residual was that agbcc kept the -fforce-addr .rodata word's ADDRESS
 *     live in sl for the whole function so pass 2's guard could re-read it;
 *     splitting the address's uses this way lets pass 2 re-materialise the
 *     plain .text pool word as the ROM does, and the freed register pays for
 *     hoisting 0x417A.
 *   - `c = 7;` before the loops rather than the literal at the store. A named
 *     constant local lands in a different register (`movs r5,#7` and not
 *     `movs r0,#7`) -- the same family as the wave-23 named-constant rule.
 *
 * What was already right and is worth keeping: the ROM materialises
 * &gMapData TWICE by two different routes and BOTH belong. -fforce-addr
 * parks the address in this unit's own .rodata (the word the ROM has at
 * 0x0808D7A8, immediately behind gUnknown_0808D7A0's 8 bytes) and pass 1's
 * outer guard reads through it, while pass 1's body and all of pass 2 use a
 * plain .text pool word holding the symbol. The honest spelling below produces
 * both; no c_local workaround is needed.
 */

struct Unk3F44Map
{
    /* 0x0000 */ u16 width;
    /* 0x0002 */ u16 height;
    /* 0x0004 */ u8 filler_0004[0x0A22 - 0x0004];
    /* 0x0A22 */ u16 plane[(0x1432 - 0x0A22) / 2];
    /* 0x1432 */ u8 cell[0x417A - 0x1432];
    /* 0x417A */ u16 rowOffset[1];
};
#define MAP ((struct Unk3F44Map *)gMapData)

void sub_08004724(void)
{
    u16 tiles[4];
    const u8 *p;
    struct Unk3F44Map *m;
    int x;
    int y;
    int c;

    sub_0808B6E8(tiles, gUnknown_0808D7A0, 8);
    c = 7;
    p = gUnknown_08486FC4;

    for (y = 0; y < MAP->height; y++)
        for (x = 0; x < MAP->width; x++)
        {
            m = MAP;
            m->plane[MAP->rowOffset[y] + x] = 0x2A;
            m->cell[MAP->rowOffset[y] + x] = c;
        }

    for (y = 0; y < MAP->height; y++)
        for (x = 0; x < MAP->width; x++)
            sub_08003F44(x, y, tiles[*p++]);
}
