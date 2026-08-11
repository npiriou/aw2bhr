#include "global.h"

/* Promoted from assembly; each function below is byte-for-byte
 * identical to the original. Order is address order and must
 * stay that way -- the linker places this file's .text as one
 * contiguous block at 0x08043418.
 * sub_08043418 @ 0x08043418
 */

/* Promoted from assembly; each function below is byte-for-byte
 * identical to the original. Order is address order and must
 * stay that way -- the linker places this file's .text as one
 * contiguous block at 0x08043418.
 * sub_08043418 @ 0x08043418
 */


/* A `switch` over 19 sprite kinds that nudges the (x, y) it was handed, picks
 * an animation id and puts one sprite. Cases 4 and 8..11 are absent -- their
 * jump-table slots point at the tail, which is also the default, and the id
 * that reaches sub_0801C7DC in those cases is the selector itself.
 *
 * Three things are load-bearing and none of them is visible in the listing:
 *
 *   - `id` is REUSED as the 4-or-0 flag in case 0, rather than a `flag` local.
 *     Both spellings are the same statement count, but a separate local lands
 *     the constant straight in r2 and the original spends `adds r2, r4, #0`
 *     copying it out of the switch variable's register. Two bytes.
 *   - the rowOffset load is its own statement, with the column computed
 *     AFTER it. Folding the column into the `idx = ... + col` expression the
 *     way `c_0802E4B4.c` does leaves the column, the row and the 0x417A
 *     literal live across the `ldrh` at once, which costs a fourth
 *     callee-saved register: the original's frame is `push {r4, r5, r6, lr}`
 *     and that spelling makes it `{r4, r5, r6, r7, lr}`.
 *   - `p += 0x12; p += idx;` and not `p[0x12 + idx]`, which is the same
 *     compound-assignment rule `c_0802E4B4.c` already records for this idiom.
 *
 * The cell fetch is otherwise the c_08001158.c family expression unchanged:
 * u16 rowOffset[] at +0x417A, byte cells at +0x12. What is new here is the
 * pair of s16 camera fields at +0x04 and +0x06 -- the pixel scroll, added
 * before the `>> 4` that turns a pixel coordinate into a cell coordinate.
 * They are a different pair from the +0x0C / +0x0E tile-granularity camera
 * that unknown-globals.h already records on this descriptor.
 */
void sub_08043418(int x, int y, int id)
{
    u8 *p;
    u8 *rows;
    int t;
    int idx;
    int col;
    int row;

    switch (id)
    {
    case 0:
        x += 8;
        y += 8;
        p = gMapData;
        row = (y + *(s16 *)(p + 6)) >> 4;
        t = row * 2;
        rows = p + 0x417A;
        idx = *(u16 *)(rows + t);
        col = (x + *(s16 *)(p + 4)) >> 4;
        idx += col;
        p += 0x12;
        p += idx;
        if (*p == 0 && sub_08042424(col, row))
            id = 4;
        else
            id = 0;
        id = sub_08043574(x, y, id);
        break;
    case 1:
        x += 8;
        y += 8;
        id = sub_08043574(x, y, 0);
        break;
    case 2:
        x += 8;
        y += 8;
        id = 0x10;
        break;
    case 3:
        id = 0x11;
        break;
    case 5:
        x += 8;
        y += 8;
        id = sub_08043574(x, y, 8);
        break;
    case 6:
        x += 8;
        y += 8;
        id = sub_08043574(x, y, 0xc);
        break;
    case 7:
        x += 8;
        y += 8;
        id = 0x12;
        break;
    case 12:
        x += 8;
        y += 0x10;
        id = 0x14;
        break;
    case 13:
        x += 8;
        y += 0x10;
        id = 0x13;
        break;
    case 14:
        id = 0x15;
        break;
    case 15:
        id = 0x16;
        break;
    case 16:
        id = 0x17;
        break;
    case 17:
        id = 0x18;
        break;
    case 18:
        x += 8;
        y += 8;
        id = 0x19;
        break;
    }

    sub_0801C7DC(gUnknown_08101EC0, id, gUnknown_03004008,
                 x & 0x1FF, y & 0xFF, 0x1365, 1);
}
