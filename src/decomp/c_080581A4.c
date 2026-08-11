#include "global.h"

/* Promoted from assembly; each function below is byte-for-byte
 * identical to the original. Order is address order and must
 * stay that way -- the linker places this file's .text as one
 * contiguous block at 0x080581A4.
 * sub_080581A4 @ 0x080581A4
 */

/* Wave 52, W52-B.  MATCHED, 2 attempts.
 *
 * Seeds a whole map plane to one byte value through a stack table of 40 row
 * pointers (`sub sp,#0xa0`), built from gMapData's +0x417A rowOffset
 * table and then walked row by row.
 *
 * PROMOTION MUST CARRY  "rodata": ["0x0816D938"] .  That ROM word holds
 * 0x08499590, i.e. &gMapData -- agbcc's own -fforce-addr address
 * constant for this function, which is why the symbol is reached with a
 * three-level `ldr rA,=<rodata slot>; ldr rB,[rA]; ldr rC,[rB]`.  The honest
 * `gMapData` spelling reproduces it and trymatch reports `relocs:
 * name different symbols that resolve to the same address`.
 *
 * The map header MUST be reached through a locally declared struct cast onto
 * the `u8 *` symbol, not as pointer arithmetic -- wave 34 (W34-F)'s rule,
 * recorded on gMapData in include/unknown-globals.h.  Written as
 * `*(u16 *)(gMapData + 2)` the height reassociates, becomes
 * loop-invariant, `check_dbra_loop` reverses the first loop into a countdown,
 * and the ROM's re-read of `->unk02` at the loop bottom disappears (-4 bytes).
 */

struct Unk581A4Map
{
    /* 0x0000 */ u16 unk00;
    /* 0x0002 */ u16 unk02;
    /* 0x0004 */ u8 filler_04[0x417a - 0x04];
    /* 0x417a */ u16 unk417a[1];
};

void sub_080581A4(u8 *dst, int a2)
{
    u8 *rows[40];
    u8 v;
    int i;
    int j;

    v = a2;

    for (i = 0; i < ((struct Unk581A4Map *)gMapData)->unk02; i++)
        rows[i] = dst + ((struct Unk581A4Map *)gMapData)->unk417a[i];

    for (i = 0; i < ((struct Unk581A4Map *)gMapData)->unk02; i++)
    {
        for (j = 0; j < ((struct Unk581A4Map *)gMapData)->unk00; j++)
            rows[i][j] = v;
    }
}
