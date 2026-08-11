#include "global.h"

/* Promoted from assembly; each function below is byte-for-byte
 * identical to the original. Order is address order and must
 * stay that way -- the linker places this file's .text as one
 * contiguous block at 0x08003DC4.
 * sub_08003DC4 @ 0x08003DC4
 */

/* MATCHED, wave 36 (W36-I), after five waves parked at 93.7%. PROMOTION NEEDS
 * THE POOL WORD PLACED:
 *     "rodata": ["0x0808D794"]
 * (two references, both to the same word, which holds &gMapData.)
 *
 * THE OLD PARK NOTE WAS WRONG and this replaces it. It described the residual
 * as "a pure register permutation ... nothing in the C distinguishes them" and
 * listed the axes it had ruled out -- all of them about `u8 *` arithmetic on
 * the map (`cells` locals, `*(p + K + idx)`, a permuter run). The axis that
 * mattered was not in the list because it did not exist yet when the note was
 * written: the planes must be reached as MEMBERS of a struct cast onto
 * gMapData (W34-F's rule), and the cast must stay INLINE rather than
 * being bound to a `map` local. A local keeps the pseudo live to the end of
 * its scope, so agbcc preserves the base (`mov rT,ip; adds rD,rBase,rT`) where
 * the ROM advances it (`add rBase,ip`) -- that pair IS the advance-vs-preserve
 * difference the old note called indistinguishable.
 *
 * The last 24 bytes were scope, not allocation luck: `int idx;` at function
 * scope is one allocno shared by four disjoint blocks and lands in r3 where
 * the ROM uses r1/r0. Block-scope declarations in the two case-7 arms that
 * need the `+1`/`-1` sequenced (`idx = row; idx++; idx += x;`, which also
 * stops `x + 1` being CSEd with the call argument) plus plain inline
 * expressions in the two case-1 arms is what matches. Mixing the two styles is
 * what the register numbers read out, not an inconsistency.
 *
 * `ands r2, r4` in the case-1 arm is `x & 1` with agbcc substituting the
 * switch register, which cse knows equals 1 after `cmp r2,#1; beq`. */

struct UnkDC4Map
{
    /* 0x0000 */ u16 width;
    /* 0x0002 */ u16 height;
    /* 0x0004 */ u8 filler_0004[0x1432 - 0x0004];
    /* 0x1432 */ u8 cell[0x417A - 0x1432];
    /* 0x417A */ u16 rowOffset[1];
};
#define MAP ((struct UnkDC4Map *)gMapData)

void sub_08003DC4(int x, int y, int kind)
{
    switch (kind)
    {
    case 7:
        if (x < MAP->width - 1)
        {
            int idx;

            idx = MAP->rowOffset[y];
            idx++;
            idx += x;
            if (MAP->cell[idx] == 1 || MAP->cell[idx] == 0xD)
                sub_08007F9C(x + 1, y);
        }
        if (x > 0)
        {
            int idx;

            idx = MAP->rowOffset[y];
            idx--;
            idx += x;
            if (MAP->cell[idx] == 1 || MAP->cell[idx] == 0xD)
                sub_08007F9C(x - 1, y);
        }
        break;

    case 1:
        if (x & 1)
        {
            if (y > 0)
            {
                if (MAP->cell[MAP->rowOffset[y - 1] + x] == 7
                 || MAP->cell[MAP->rowOffset[y - 1] + x] == 0xD)
                    sub_08007F9C(x, y);
            }
            if (y < MAP->height - 1)
            {
                if (MAP->cell[MAP->rowOffset[y + 1] + x] == 7
                 || MAP->cell[MAP->rowOffset[y + 1] + x] == 0xD)
                    sub_08007F9C(x, y);
            }
        }
        break;

    case 3:
        sub_0800AF74(x, y);
        break;
    }
}
