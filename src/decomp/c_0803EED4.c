#include "global.h"

/* Promoted from assembly; each function below is byte-for-byte
 * identical to the original. Order is address order and must
 * stay that way -- the linker places this file's .text as one
 * contiguous block at 0x0803EED4.
 * sub_0803EED4 @ 0x0803EED4
 */

/* gMapData's screen descriptor, reached through a struct for the
 * reason recorded in c_0800B1FC.c: a member access computes base + offsetof
 * first and then adds the scaled index, which is the ROM's order. */
struct MapScreen3EED4
{
    /* 0x0000 */ u8 filler_0000[0x1432];
    /* 0x1432 */ u8 terrain[0x417A - 0x1432];
    /* 0x417A */ u16 rowOffset[1];
};

/* The second index is written `rowOffset[..] + (unk00 + pos.unk00)` with the
 * parenthesis load-bearing: -fforce-addr materialises the rowOffset element's
 * ADDRESS when the left operand is expanded and defers the `ldrh` to the add,
 * so the two byte reads land between the address and the load exactly as the
 * ROM has them. Without the parentheses the sum reassociates and the `ldrh`
 * moves four instructions earlier. */
u8 sub_0803EED4(int a1, int a2)
{
    struct Unk02028360Pos pos;
    struct Unk02028360 *p;
    u8 ret;

    ret = ((struct MapScreen3EED4 *)gMapData)->terrain[((struct MapScreen3EED4 *)gMapData)->rowOffset[a2] + a1];
    p = sub_0803DE94(a1, a2);
    if (p != NULL)
    {
        sub_0803DF98(p->unk02_6, &pos);
        ret = ((struct MapScreen3EED4 *)gMapData)->terrain[((struct MapScreen3EED4 *)gMapData)->rowOffset[p->unk01 + pos.unk02] + (p->unk00 + pos.unk00)];
    }

    return ret;
}
