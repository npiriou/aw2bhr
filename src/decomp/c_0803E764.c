#include "global.h"

/* Promoted from assembly; each function below is byte-for-byte
 * identical to the original. Order is address order and must
 * stay that way -- the linker places this file's .text as one
 * contiguous block at 0x0803E764.
 * sub_0803E764 @ 0x0803E764
 */

/* Walks a 0xFFFF-terminated {x, y} list and, for every cell that holds a unit,
 * appends a record through sub_0803E560.
 *
 * Two spellings here are load-bearing and both cost exactly one instruction if
 * you undo them:
 *
 * 1. `x` and `y` must be bound BEFORE the plane read. Written inline, the index
 *    expression evaluates p->unk02 first, the `ldrh` of p->unk00 moves five
 *    instructions later, and the register freed by that takes the in-loop
 *    0x417A constant so 0xFFFF never reaches r8 -- twelve instructions off.
 *
 * 2. The plane expression is named TWICE, tested directly and re-named inside
 *    the branch to bind `t`. Binding it once before the test lets local-alloc
 *    coalesce the load straight into the third argument register and emits
 *    `ldrb r2,[r1]` where the ROM has `ldrb r0,[r1]` plus `adds r2,r0,#0`.
 *    CSE still emits only one `ldrb`; it is the binding copy that survives.
 *    Moving the second mention inline into the call instead of binding it makes
 *    CSE give up and emit a second `ldrb`. Found by decomp-permuter after a
 *    hand search had ruled out every width, declaration order and test shape;
 *    see the chapter in docs/agbcc-codegen.md. The same idiom is what fixed the
 *    identical residual in sub_0803E6C4.
 *
 * sub_0803E560's u16 parameters are why `a2` is narrowed (`lsls #0x10` hoisted
 * into the preheader, `lsrs #0x10` at the call) while x, y and t are not --
 * see the note on that prototype in include/unknown-functions.h. */
struct Unk3E764Map
{
    /* 0x0000 */ u8 filler_0000[0x12];
    /* 0x0012 */ u8 unit[0x417A - 0x12];
    /* 0x417A */ u16 rowOffset[1];
};

void sub_0803E764(struct Unk02028360Pos *p, int a2)
{
    int x;
    int y;
    u8 t;

    while (p->unk00 != 0xFFFF)
    {
        x = p->unk00;
        y = p->unk02;
        if (((struct Unk3E764Map *)gMapData)->unit[((struct Unk3E764Map *)gMapData)->rowOffset[y] + x] != 0)
        {
            t = ((struct Unk3E764Map *)gMapData)->unit[((struct Unk3E764Map *)gMapData)->rowOffset[y] + x];
            sub_0803E560(x, y, t, a2);
        }
        p++;
    }
}
