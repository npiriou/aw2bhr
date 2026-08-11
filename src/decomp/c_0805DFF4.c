#include "global.h"

/* Promoted from assembly; each function below is byte-for-byte
 * identical to the original. Order is address order and must
 * stay that way -- the linker places this file's .text as one
 * contiguous block at 0x0805DFF4.
 * sub_0805DFF4 @ 0x0805DFF4
 */

/* sub_0805DFF4 @ 0x0805DFF4, 364 bytes.
 *
 * gUnknown_0816DA58 is not an object: the ROM word at 0x0816DA58 holds
 * 0x030040D8, i.e. it is agbcc's own -fforce-addr address constant for
 * gUnknown_030040D8. Naming the global honestly reproduces it and the build
 * places the .rodata word.
 *
 * Byte +9 of the record carries a THREE-BIT FIELD at bits 3..5, and that is
 * measured rather than guessed: the ROM clears it with `movs #0x39; rsbs #0;
 * ands` -- the SImode -57 that store_bit_field builds -- and sets it with the
 * same AND plus `movs #0x10; orrs`, i.e. the value 2 shifted into place. No
 * plain-u8 spelling produces either: `x &= ~0x38` on a u8 member is narrowed to
 * QImode by combine and comes out as a single `movs #0xc7`, and routing the
 * value through an `int` temporary keeps the SImode constant but puts the
 * loaded byte in the AND's destination register instead of the constant.
 * store_bit_field is the only expander that emits the constant into the
 * destination. The overlay struct lives here rather than in
 * include/unknown-globals.h because offset 9 falls inside struct
 * Unk030040D8's unk07[5], which c_0802966C.c indexes with a runtime subscript.
 *
 * p->unk01 and p->unk02 are NAMED LOCALS: the ROM loads them once into two
 * callee-saved registers before the sub_0805BFDC branch and reuses them for the
 * `x | (y << 16)` word afterwards. That pair is why this function pushes r7.
 *
 * The trailing sub_0805F4F8() belongs to the p == NULL arm as a whole, not to
 * the gUnknown_030046B8 test inside it -- the `beq` when bit 0 is clear lands on
 * the single call, and the sub_08059A0C == 0 path reaches TWO consecutive calls
 * by falling through.
 */

struct Unk0805DFF4Rec
{
    /* 0x00 */ u8 filler_00[0x09];
    /* 0x09 */ u8 unk09_0 : 3;
               u8 unk09_3 : 3;
               u8 unk09_6 : 2;
};

void sub_0805DFF4(void)
{
    u8 buf;
    union Unk802C57CBuf v;
    struct UnitRecord *p;
    int q;
    u8 x;
    u8 y;

    sub_0805E440();
    sub_0805D888();

    if (gUnknown_03004784[0] > (u8)(gUnknown_030040D8->unk07[3] % 100)
        || sub_0804415C(gCurrentArmyIndex))
        sub_0805E718();

    sub_08058F30(&buf);
    gUnknown_030013EC(gUnknown_030040D8->unk02, gUnknown_030040D8->unk03,
                      gUnknown_030040D8->unk00, buf, 0);
    sub_08058058(sub_0804151C());
    q = sub_08057F00(1) / gUnknown_085766E0->unk04[5];
    if (q == 0)
        q = gUnknown_085766E0->unk00;
    p = sub_08058144(q, 0);
    if (p != 0)
    {
        ((struct Unk0805DFF4Rec *)gUnknown_030040D8)->unk09_3 = 0;
        x = p->unk01;
        y = p->unk02;
        if (gUnknown_030046B8 & 1)
            sub_0805BFDC(x, y, 0x14, 2);
        else
            sub_0805BFDC(x, y, 7, 1);
        v.raw = x | (y << 16);
        sub_080591E4(&v);
    }
    else
    {
        if (gUnknown_030046B8 & 1)
        {
            ((struct Unk0805DFF4Rec *)gUnknown_030040D8)->unk09_3 = 2;
            sub_0805FB70();
            gUnknown_030013EC(gUnknown_030040D8->unk02, gUnknown_030040D8->unk03,
                              0x14, 0x78, 0);
            if (sub_08059A0C(gUnknown_03003F20))
            {
                gUnknown_030013EC(gUnknown_030040D8->unk02, gUnknown_030040D8->unk03,
                                  gUnknown_030040D8->unk00, 0x78, -1);
                v.pos.unk00 = 0x270F;
                sub_0805C0AC(&v);
                if (v.pos.unk00 != 0x270F)
                    sub_080591E4(&v);
            }
            else
            {
                sub_0805F4F8();
            }
        }
        sub_0805F4F8();
    }
}
