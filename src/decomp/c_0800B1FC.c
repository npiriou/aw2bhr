#include "global.h"

/* Promoted from assembly; each function below is byte-for-byte
 * identical to the original. Order is address order and must
 * stay that way -- the linker places this file's .text as one
 * contiguous block at 0x0800B1FC.
 * sub_0800B1FC @ 0x0800B1FC
 */

/* Classifies the map cell at (x, y): 0 for the two "blank" tiles, 1 for the
 * 0x22/0x23 pair, -1 otherwise.
 *
 * The screen is reached through a STRUCT, and that is what makes it match --
 * the wave-18 rule that a table declared flat may need a struct. Written as
 * `*(u16 *)(gMapData + 0x417A + y * 2)` agbcc reassociates to
 * `(y * 2 + base) + 0x417A`, which needs one register fewer and drops the r4
 * push the ROM has. A member-array access computes `base + offsetof` first and
 * then adds the scaled index, which is the ROM's order and its register
 * pressure. gMapData itself stays `u8 *` -- a dozen promoted files
 * share that declaration -- so the struct is local to this file and applied by
 * a cast.
 *
 * `cell` is `int`, not `u16`. The 0x22/0x23 pair folds to the unsigned range
 * test `cell - 0x22 <= 1`, and on a u16 that test carries an extra
 * `lsls #0x10; lsrs #0x10` truncation the ROM does not have.
 *
 * The struct extents are inferred from the adjacent offsets this block reaches
 * and are NOT proved; only the three member positions are. */
struct MapScreen
{
    /* 0x0000 */ u8 filler_0000[0x0A22];
    /* 0x0A22 */ u16 cells[(0x1432 - 0x0A22) / 2];
    /* 0x1432 */ u8 terrain[0x417A - 0x1432];
    /* 0x417A */ u16 rowOffset[1];
};

int sub_0800B1FC(int x, int y)
{
    struct MapScreen *s = (struct MapScreen *)gMapData;
    int cell = s->cells[s->rowOffset[y] + x];

    if (cell == 2 || cell == 0x20)
        return 0;

    if (cell == 0x22 || cell == 0x23)
        return 1;

    return -1;
}
