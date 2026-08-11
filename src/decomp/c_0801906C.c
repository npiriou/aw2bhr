#include "global.h"

/* Promoted from assembly; each function below is byte-for-byte
 * identical to the original. Order is address order and must
 * stay that way -- the linker places this file's .text as one
 * contiguous block at 0x0801906C.
 * sub_0801906C @ 0x0801906C
 */

/* A gUnknown_0200C528 list-script handler in the c_0801903C / c_080190EC
 * family: `s16` slot index, `return TRUE`. The parameter's zero-extended copy
 * lives in r8 across the `bl DivRem` -- PROMOTE_MODE at entry, then a signed
 * `lsls #0x10; asrs #0x10` at each of the two uses, exactly as c_0801903C.c
 * documents. Both `(s16)` casts on the u16 members are the c_080190EC.c
 * reading: they fold the truncate-then-sign-extend into the two-instruction
 * register-offset `movs r1,#8; ldrsh` and are NOT evidence of signed members.
 *
 * The loop counts DOWN in the ROM and the source counts down too -- the bottom
 * test is `bgt`, not the `bne` check_dbra_loop leaves behind when it reverses
 * an ascending counter (wave 37, W37-N).
 *
 * There is NO pointer local for the cursor, and that is the whole function.
 * The ROM's `adds r0, r7, #0` preheader copy plus `str r3, [r0]` on the loop
 * EXIT edge is gcc's `load_mems`: the slot `gUnknown_0200C528[a].unk04` is a
 * loop-invariant MEM, so loop.c hoists it into a register (reusing the r3 the
 * `->unk0c` read already loaded), runs the loop on the register, copies the
 * address into the preheader and stores back once on the way out. That is why
 * the store is SKIPPED on the zero-trip path -- the store-back sits on the exit
 * edge, which the `ble` guard bypasses -- and why a fifth callee-saved register
 * (r8) is needed. Writing the obvious `p = ...unk04; while (n>0) p++;
 * ...unk04 = p;` instead is 8 bytes longer and recomputes the store address:
 * the local kills the address pseudo, so load_mems never fires. */
bool8 sub_0801906C(s16 a)
{
    int army;
    int n;
    int r;

    army = (s16)gUnknown_0200C528[a].unk04->unk08;
    r = DivRem((s16)gUnknown_0200C528[a].unk04->unk0a, 0x18);
    n = gUnknown_0200C528[a].unk04->unk0c;
    if (gArmyRecords[army].unk1d != r)
    {
        while (n > 0)
        {
            gUnknown_0200C528[a].unk04++;
            n--;
        }
    }
    gUnknown_0200C528[a].unk04++;
    return TRUE;
}
