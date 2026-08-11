#include "global.h"

/* Promoted from assembly; each function below is byte-for-byte
 * identical to the original. Order is address order and must
 * stay that way -- the linker places this file's .text as one
 * contiguous block at 0x08023BAC.
 * sub_08023BAC @ 0x08023BAC
 */

/* The wrap at the loop bottom is TWO statements, and that is the whole reason
 * the ROM's `ands r1, r0` ties the result to the CONSTANT's register:
 *
 *     a1 = a1 + 2;
 *     a1 &= 0x1F;
 *
 * Written as one expression -- in any spelling, including the six-instruction
 * `(((a1 + 2) << 16) & 0x1F0000) >> 16` that this draft used to carry -- the
 * mask is a CONST_INT at expand time, expand_binop reuses the shifted value's
 * register as the target, and you get `ands r0, r1` (4 bytes, 98.9%, where
 * this function sat for two waves). Split across two statements the u16
 * assignment emits its own lsl/lsr pair, combine folds the mask through it
 * into `(lshiftrt (and (ashift x 16) 0x1f0000) 16)`, and the constant is
 * force_reg'd at the COMBINED insn -- a later pseudo, which wins the tie.
 *
 * Measured by compile_probe this wave, all with identical surrounding code:
 *   - `(((a1 + 2) << 16) & 0x1F0000) >> 16`  -> `ands r0, r1`   (4 bytes off)
 *   - `mask = 0x1F0000;` as its own statement BEFORE the value -> right tie,
 *     wrong order (the mask pair is emitted first)
 *   - `t2 = (a1+2) << 16; mask = 0x1F0000; a1 = (t2 & mask) >> 16;`
 *     -> right order, still `and r0, r0, r1`
 *   - `a1 = (u16)(a1 + 2) & 0x1F;` -> collapses to `add; mov #0x1f; and`
 *   - the two statements above -> EXACT
 *
 * The `gUnknown_080BFBCA` vs `gUnknown_080BFBC4+6` pool relocations are the
 * same address (0x080BFBC4 + 6 == 0x080BFBCA) and contribute zero differing
 * bytes; they were never the residual, despite NOTES.md's reading. */

void sub_08023BAC(u16 a1, u16 a2, u16 a3, u16 a4)
{
    struct MapData *m;
    u16 *dst;
    u16 i;

    a1 = (a1 & 0xF) * 2;
    dst = gUnknown_08499584 + (a2 & 0xF) * 64;

    for (i = 0; i <= 15; i++)
    {
        m = (struct MapData *)gMapData;
        if (m->unk234a[m->unk417a[a4] + (a3 + i)] == 0)
        {
            dst[a1] = gUnknown_080BFBC4[m->unk0a22[m->unk417a[a4] + (a3 + i)]][0] + 0x4000;
            dst[a1 + 1] = gUnknown_080BFBC4[m->unk0a22[m->unk417a[a4] + (a3 + i)]][1] + 0x4000;
            dst[a1 + 32] = gUnknown_080BFBC4[m->unk0a22[m->unk417a[a4] + (a3 + i)]][2] + 0x4000;
            dst[a1 + 33] = gUnknown_080BFBC4[m->unk0a22[m->unk417a[a4] + (a3 + i)]][3] + 0x4000;
        }
        else
        {
            dst[a1] = gUnknown_080BFBC4[m->unk0a22[m->unk417a[a4] + (a3 + i)]][0];
            dst[a1 + 1] = gUnknown_080BFBC4[m->unk0a22[m->unk417a[a4] + (a3 + i)]][1];
            dst[a1 + 32] = gUnknown_080BFBC4[m->unk0a22[m->unk417a[a4] + (a3 + i)]][2];
            dst[a1 + 33] = gUnknown_080BFBC4[m->unk0a22[m->unk417a[a4] + (a3 + i)]][3];
        }
        a1 = a1 + 2;
        a1 &= 0x1F;
    }
}
