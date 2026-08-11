#include "global.h"

/* Promoted from assembly; each function below is byte-for-byte
 * identical to the original. Order is address order and must
 * stay that way -- the linker places this file's .text as one
 * contiguous block at 0x0805A744.
 * sub_0805A744 @ 0x0805A744
 */

/* Consumes the cell list sub_0805A514 (src/decomp/c_0805A514.c) produces:
 * seeds the map's 0x3C72 plane to 0x7F, redraws the 0x2852 plane, then for each
 * listed cell drives the gUnknown_030013EC hook and floods the 0x3C72 plane
 * with that cell's threat value wherever the terrain code is 0xd or 0xb and the
 * value beats what is already there.
 *
 * THE RECORD TYPE IS SETTLED BY A PRODUCER/CONSUMER PAIR, not by guesswork --
 * the wave brief's one real oracle for a byte-neutral type. sub_0805A514 writes
 * {u8 x; u8 y; s16 v;} records and terminates the array with `out->v = 0xFFFF`;
 * this function walks the same 4-byte stride and stops on `v == -1`. Same
 * struct, declared here rather than shared because it describes only a pointer
 * parameter.
 *
 * gUnknown_0816D99C IS NOT A GLOBAL. The ROM word there holds 0x08499590, i.e.
 * the ADDRESS of gMapData -- checked in baserom.gba, the same
 * -fforce-addr pattern c_0805A95C.c documents. gMapData is named
 * honestly here and the build places the word; `relocs` reports it as the same
 * address under a different symbol, which is the disassembly artefact, not a
 * difference in the ROM. Promotion must carry "rodata": ["0x0816D99C"].
 *
 * TWO THINGS IN THE INNER BODY WERE MEASURED, NOT GUESSED:
 *  - `cell` is bound as a pointer, and that is what makes the two sign-extended
 *    reads `ldrsb rN,[cell,rZERO]` instead of sub_0805AD90's `ldrb; lsl; asr`.
 *    THUMB ldrsb has only a register-offset form, so agbcc will materialise a
 *    zero register for it when the address is a live pointer local and will not
 *    when the address dies at the load. Reading it as evidence that the array
 *    is signed would be wrong: it is the SAME gUnknown_03003340, and the u8
 *    read that supplies the stored value sits right beside it.
 *  - `w` is REUSED for the terrain mask and then for the s8 compare. A fresh
 *    local there is size-exact with r0/r1 swapped through the final block: the
 *    fresh name takes the lower allocno and steals the register the ROM spends
 *    on the plane pointer. Reusing it creates no allocno, so `plane` wins r0.
 *    This is the same lever as c_0805AD90.c's `do { } while (0)`, in the
 *    cheaper of its two spellings.
 *
 * MATCHED, 1 attempt after 2 probes. */

struct Unk5A514Cell
{
    /* 0x00 */ u8 x;
    /* 0x01 */ u8 y;
    /* 0x02 */ s16 v;
};

void sub_0805A744(struct Unk5A514Cell *p)
{
    int x;
    int y;
    u8 *q;
    u8 *rows;
    u8 *cells;
    u8 *plane;
    u8 *cell;
    int v;
    int w;
    int t;
    int off;

    sub_080581A4(gMapData + 0x3c72, 0x7f);
    sub_0801F92C(gMapData + 0x2852);

    while (p->v != -1)
    {
        gUnknown_030013EC(p->x, p->y, 0x17, 0x78, -1);

        for (y = 0; y < *(u16 *)(gMapData + 2); y++)
        {
            for (x = 0; x < *(u16 *)gMapData; x++)
            {
                cell = gUnknown_03003340[y] + x;
                v = *cell;
                if (*(s8 *)cell >= 0)
                {
                    q = gMapData;
                    t = y * 2;
                    rows = q + 0x417a;
                    off = *(u16 *)(rows + t) + x;
                    cells = q + 0x1432;
                    w = cells[off] & 0x1f;
                    if (w == 0xd || w == 0xb)
                    {
                        w = *(s8 *)cell;
                        plane = q + 0x3c72;
                        plane += off;
                        if (w < *plane)
                            *plane = v;
                    }
                }
            }
        }

        p++;
    }
}
