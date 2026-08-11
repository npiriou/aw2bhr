#include "global.h"

/* Promoted from assembly; each function below is byte-for-byte
 * identical to the original. Order is address order and must
 * stay that way -- the linker places this file's .text as one
 * contiguous block at 0x0800CF28.
 * sub_0800CF28 @ 0x0800CF28
 */

/* The full "a cell changed" refresh, and the widest of the family: it is
 * sub_0800AF74's list (sub_0800A588 / sub_0800ABD0 / sub_08007F9C /
 * sub_0800BEE4) with sub_0800CFDC in front of it, wrapped in two guards.
 *
 * The struct is c_0800C22C.c's / c_0800AF24.c's, copied verbatim.
 *
 * `(u16)(cells[off] - 0x86) <= 1` in the ROM is fold_range_test's output for
 * the source's `cells[off] == 0x86 || cells[off] == 0x87` -- do not author the
 * subtract.  The whole guard is therefore
 * `86 || 87 || terrain != 4`, and the `beq` on terrain == 4 is the only path
 * that skips the re-tile block.
 *
 * `off` is one binding local and stays one: the ROM keeps rowOffset[y] + x in
 * r3 across the u16 cells load and reuses it for the u8 terrain load, so the
 * two accesses share the index rather than recomputing it. */

struct MapScreen
{
    /* 0x0000 */ u16 width;
    /* 0x0002 */ u16 height;
    /* 0x0004 */ u8 filler_0004[0x0A22 - 4];
    /* 0x0A22 */ u16 cells[(0x1432 - 0x0A22) / 2];
    /* 0x1432 */ u8 terrain[0x417A - 0x1432];
    /* 0x417A */ u16 rowOffset[1];
};
#define MAP ((struct MapScreen *)gMapData)

void sub_0800CF28(int x, int y)
{
    int off;

    if (sub_0800C840(x, y))
    {
        sub_0800C608(x, y);
        sub_080011F4(x, y, 1);
    }

    off = MAP->rowOffset[y] + x;

    if (MAP->cells[off] == 0x86 || MAP->cells[off] == 0x87 || MAP->terrain[off] != 4)
    {
        sub_0800CEF8(x, y);
        sub_080011F4(x, y, 4);
        sub_08007F14(x, y, 0x87);
        sub_08007D70(x, y);
    }

    sub_0800CFDC(x, y);
    sub_0800A588(x, y);
    sub_0800ABD0(x, y);
    sub_08007F9C(x, y);
    sub_0800BEE4(x, y);
}
