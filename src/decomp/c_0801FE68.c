#include "global.h"

/* Promoted from assembly; each function below is byte-for-byte
 * identical to the original. Order is address order and must
 * stay that way -- the linker places this file's .text as one
 * contiguous block at 0x0801FE68.
 * sub_0801FE68 @ 0x0801FE68
 */

/* Grows the gUnknown_03003340 reachability plane by one cell in each of the
 * four directions: every cell that is set (>= 0), is not already flagged 0x40
 * and -- when it is non-zero -- sits on terrain byte 0 of the map's +0x12
 * plane, bumps each orthogonal neighbour's counter and stamps 0x40 on it. The
 * second pass then clears every cell that did NOT get the 0x40 flag to -1.
 *
 * gUnknown_0809093C is NOT a global: the ROM word there holds 0x08499590, i.e.
 * it is agbcc's own -fforce-addr copy of &gMapData (same class as
 * 0x08090934 in c_0801F888.c). The honest spelling reproduces it -- declaring a
 * symbol for it would add a fourth load level. The promotion must carry
 * "rodata": ["0x0809093C"].
 *
 * x and y are u8. That is what produces the UNSIGNED `blo`/`bhs` loop tests
 * against the u16 map bounds: shorten_compare narrows `(int)(u8)x <
 * (int)(u16)w` back to a common unsigned type, whereas the `int` counters of
 * c_0801F888.c's sub_0801F98C over the same two bounds give signed `blt`.
 *
 * The second pass's fill value is written through an (s8 *) view. Spelled
 * `gUnknown_03003340[y][x] = -1;` against the u8 element type the C front end
 * folds the constant to 255 and agbcc emits `movs rN, #0xff`; the ROM has
 * `movs #1; rsbs` (SImode -1), which only survives when the store's tree type
 * is signed.
 *
 * THE `do { } while (0)` IS A REGISTER-ALLOCATION LEVER, exactly as in
 * src/decomp/c_0805AD90.c, and it is worth 4 bytes across BOTH halves even
 * though it wraps only the second one. Without it the function is 444 bytes
 * with instruction order, control flow and the literal pool already exact and
 * one allocation fact wrong throughout: the ROM parks &gMapData in r7
 * and the loop-hoisted `y + 1` in ip, the plain candidate does the reverse, so
 * each of the four `ldr rN,[r7]` sites costs an extra `mov r7, ip` (+8), the
 * three `lsls` on y+1 save one `mov r1, ip` each (-6), and the hoisted 0x40 of
 * the second pass lands in ip instead of r7 (+2). Bracketing the second pass's
 * inner loop reorders the allocnos in the FIRST pass too.
 *
 * MATCHED (decomp-permuter, 426 iterations from the 444-byte hand draft --
 * the same lever the permuter found for sub_0805AD90, found again). */
void sub_0801FE68(void)
{
    u8 x;
    u8 y;
    int c;
    u8 *p;
    u8 *rows;
    u8 *cells;
    int t;
    int off;

    for (y = 0; y < *(u16 *)(gMapData + 2); y++) {
        for (x = 0; x < *(u16 *)gMapData; x++) {
            c = (s8)gUnknown_03003340[y][x];
            if (c < 0 || c == 0x40)
                continue;
            if (c != 0) {
                p = gMapData;
                t = y * 2;
                rows = p + 0x417a;
                off = *(u16 *)(rows + t) + x;
                cells = p + 0x12;
                if (cells[off] != 0)
                    continue;
            }
            if (x != 0 && (s8)gUnknown_03003340[y][x - 1] != 0x40) {
                gUnknown_03003340[y][x - 1]++;
                gUnknown_03003340[y][x - 1] |= 0x40;
            }
            if (x != *(u16 *)gMapData - 1
                && (s8)gUnknown_03003340[y][x + 1] != 0x40) {
                gUnknown_03003340[y][x + 1]++;
                gUnknown_03003340[y][x + 1] |= 0x40;
            }
            if (y != 0 && (s8)gUnknown_03003340[y - 1][x] != 0x40) {
                gUnknown_03003340[y - 1][x]++;
                gUnknown_03003340[y - 1][x] |= 0x40;
            }
            if (y != *(u16 *)(gMapData + 2) - 1
                && (s8)gUnknown_03003340[y + 1][x] != 0x40) {
                gUnknown_03003340[y + 1][x]++;
                gUnknown_03003340[y + 1][x] |= 0x40;
            }
        }
    }

    for (y = 0; y < *(u16 *)(gMapData + 2); y++) {
        do {
            for (x = 0; x < *(u16 *)gMapData; x++) {
                if ((gUnknown_03003340[y][x] & 0x40) == 0)
                    ((s8 *)gUnknown_03003340[y])[x] = -1;
            }
        } while (0);
    }
}
