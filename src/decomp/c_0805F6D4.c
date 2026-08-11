#include "global.h"

/* Promoted from assembly; each function below is byte-for-byte
 * identical to the original. Order is address order and must
 * stay that way -- the linker places this file's .text as one
 * contiguous block at 0x0805F6D4.
 * sub_0805F6D4 @ 0x0805F6D4
 */

/* The file-local view of the gMapData map header that the wave-34
 * note in include/unknown-globals.h prescribes: the +0x417A row-offset table
 * has to be reached as a COMPONENT_REF, because
 * `*(u16 *)(gMapData + 0x417A + y * 2)` reassociates to
 * `(map + y * 2) + 0x417A` and the ROM has `(map + 0x417A) + y * 2`. */
struct Map
{
    /* 0x0000 */ u16 unk00;
    /* 0x0002 */ u16 unk02;
    /* 0x0004 */ u8 filler_0004[0x4176];
    /* 0x417A */ u16 unk417A[0x100];
};

/* sub_0805CA24 is DEFINED `int` in src/decomp/c_0805CA24.c, but every use here
 * is narrowed to a byte before the test (`lsls #24; cmp #0`), which is the
 * re-narrowing agbcc emits for a u8-returning callee. The promoted definition
 * wins, so the cast is written at the call site instead of retyping it. */
void sub_0805F6D4(void)
{
    int x;
    int y;
    int fx;
    int fy;

    fx = -1;
    fy = 0;
    if ((u8)sub_0805CA24())
    {
        gUnknown_030013EC(gUnknown_030040D8->unk02, gUnknown_030040D8->unk03,
                          gUnknown_030040D8->unk00, 0x78, 0);
        for (y = 0; y < ((struct Map *)gMapData)->unk02; y++)
        {
            for (x = 0; x < ((struct Map *)gMapData)->unk00; x++)
            {
                if ((s8)gUnknown_03003340[y][x] >= 0
                    && gUnknown_020288B4[((struct Map *)gMapData)->unk417A[y] + x] != 0)
                {
                    fx = x;
                    fy = y;
                }
            }
        }
        if (fx != -1)
            sub_0805D648(fx, fy, 2, 0, 0);
    }
}
