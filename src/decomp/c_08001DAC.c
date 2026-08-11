#include "global.h"

/* Promoted from assembly; each function below is byte-for-byte
 * identical to the original. Order is address order and must
 * stay that way -- the linker places this file's .text as one
 * contiguous block at 0x08001DAC.
 * sub_08001DAC @ 0x08001DAC
 */

#include "hardware.h"

/* Wave 37 (W37-E). Matched. PROMOTION NEEDS THE POOL WORD PLACED:
 *     "rodata": ["0x0808D70C"]
 * 0x0808D70C holds &gUnknown_0200B0B0 -- an -fforce-addr address constant. Note
 * the function reaches the SAME global both ways: through that forced address
 * at the top and through a plain `ldr =gUnknown_0200B0B0` pool word from the
 * second switch onwards. One source spelling, two codegen forms.
 *
 * Three things cost attempts here, all worth reusing:
 *
 * 1. `idx` MUST be a local. gUnknown_0200B0B0->unk68 is ordinary memory, so cse
 *    drops it at every `bl`; the four Div arguments each recomputed
 *    `(0x100 - unk68) >> 4` from scratch, +60 bytes. gSinLut is `const`, so its
 *    reads ARE preserved across the calls and need no local -- the cos<<4 CSE
 *    that lands in sl falls out on its own.
 * 2. The `& 0xFF` belongs INSIDE the gSinLut subscript, not in idx's own
 *    statement. As its own statement it emits `movs #255; ands` before the
 *    `ldr =gSinLut`; the ROM has the pool load first. Same size, wrong bytes.
 * 3. `switch` case bodies come out in SOURCE label order. The shared
 *    `case 0: case 0x1E:` arm sits THIRD in the ROM, after 0xA and 0x14 --
 *    putting it first (the natural reading order) was +4 bytes of extra
 *    branches.
 *
 * m1..m4 are four separate locals on purpose: one `u16 m` reused at the four
 * sites has a live range spanning the whole tail, and agbcc spills it -- +4
 * bytes of frame and one more callee-saved register, which pushes sx/sy off
 * r6/r5. Where the value is only TESTED in one arm of a nested if, pass a
 * literal 0 rather than the variable (the m2 site): passing the variable adds
 * an `adds r4, r0, #0` copy, while `0` lets cse pick the register the compare
 * just proved zero -- which is r7 (b) on one side and r4 (m) on the other,
 * exactly as the ROM has it. */

void sub_08001DAC(void)
{
    int flag;
    int b;
    int sx;
    int sy;
    int idx;
    u16 m1;
    u16 m2;
    u16 m3;
    u16 m4;

    sx = gUnknown_0200B0B0->unk08 - ((s16)*(u16 *)(gMapData + 4) >> 4);
    flag = 1;

    switch (gUnknown_0200B0B0->unk4c)
    {
    case 0:
        gUnknown_0200B0B0->unk54 += 2;
        if (gUnknown_0200B0B0->unk54 > 7)
            gUnknown_0200B0B0->unk54 = 8;
        gUnknown_0200B0B0->unk50 += gUnknown_0200B0B0->unk54;
        if (gUnknown_0200B0B0->unk50 > 0xB7)
        {
            gUnknown_0200B0B0->unk50 = 0xB8;
            gUnknown_0200B0B0->unk4c = flag;
        }
        flag = 0;
        break;
    case 1:
        return;
    case 0xA:
        gUnknown_0200B0B0->unk54 += 1;
        if (gUnknown_0200B0B0->unk54 > 7)
            gUnknown_0200B0B0->unk54 = 8;
        gUnknown_0200B0B0->unk50 -= gUnknown_0200B0B0->unk54;
        if (gUnknown_0200B0B0->unk50 <= 0x6A)
        {
            gUnknown_0200B0B0->unk50 = 0x6A;
            gUnknown_0200B0B0->unk4c = 0xB;
        }
        break;
    }

    switch (gUnknown_0200B0B0->unk4a)
    {
    case 0xA:
        gUnknown_0200B0B0->unk4e += (0x1180 - gUnknown_0200B0B0->unk4e) >> 3;
        if (gUnknown_0200B0B0->unk4e > 0xEFF)
        {
            gUnknown_0200B0B0->unk4e = 0xFD80;
            gUnknown_0200B0B0->unk4a = 0x14;
        }
        break;
    case 0x14:
        gUnknown_0200B0B0->unk4e += (0xA0 - gUnknown_0200B0B0->unk4e) >> 3;
        if (gUnknown_0200B0B0->unk4e >= 0)
        {
            gUnknown_0200B0B0->unk4e = 0;
            gUnknown_0200B0B0->unk4a = 0x1E;
        }
        break;
    case 0:
    case 0x1E:
        if (sx <= 6)
        {
            gUnknown_0200B0B0->unk3e = 1;
            gUnknown_0200B0B0->unk4a = 0x64;
        }
        break;
    case 0x64:
        gUnknown_0200B0B0->unk4e += (-960 - gUnknown_0200B0B0->unk4e) >> 3;
        if (gUnknown_0200B0B0->unk4e < -640)
        {
            gUnknown_0200B0B0->unk4e = 0x11A0;
            gUnknown_0200B0B0->unk4a = 0x6E;
        }
        break;
    case 0x6E:
        gUnknown_0200B0B0->unk4e += (0xB40 - gUnknown_0200B0B0->unk4e) >> 3;
        if (gUnknown_0200B0B0->unk4e <= 0xCA0)
        {
            gUnknown_0200B0B0->unk4e = 0xCA0;
            gUnknown_0200B0B0->unk4a = 0x78;
        }
        break;
    case 0x78:
        if (sx > 7)
        {
            gUnknown_0200B0B0->unk3e = 0;
            gUnknown_0200B0B0->unk4a = 0xA;
        }
        break;
    }

    sx = gUnknown_0200B0B0->unk4e >> 4;
    b = gUnknown_0200B0B0->unk07 != 0;

    if (b == 0)
    {
        if ((gUnknown_0200B0B0->unk00 & 0x40) == 0)
            sub_08002964(0, sx + 7, gUnknown_0200B0B0->unk50 + 0x26,
                         gUnknown_0200B0B0->unk2a, b, flag);
        sy = gUnknown_0200B0B0->unk50;
        if ((gUnknown_0200B0B0->unk2a & 0x1F) == 8)
            sy += 6;
    }
    else
    {
        m1 = gUnknown_0200B0B0->unk00 & 0x40;
        if (m1 == 0)
            sub_080029F4(0, sx + 7, gUnknown_0200B0B0->unk50 + 0x26,
                         gUnknown_0200B0B0->unk24, m1, flag);
        sy = gUnknown_0200B0B0->unk50;
    }

    if (gUnknown_0200B0B0->unk65 != 0)
    {
        switch (gUnknown_0200B0B0->unk66)
        {
        case 0:
            gUnknown_0200B0B0->unk67 = 0xA;
            gUnknown_0200B0B0->unk66 = 8;
            gUnknown_0200B0B0->unk68 = 0;
            break;
        case 9:
            sub_08000C68();
        case 8:
            gUnknown_0200B0B0->unk66++;
        case 0xA:
            gUnknown_0200B0B0->unk68 += 0x20;
            if (gUnknown_0200B0B0->unk68 > 0xFF)
                gUnknown_0200B0B0->unk68 = 0x100;
            idx = (0x100 - gUnknown_0200B0B0->unk68) >> 4;
            SetObjAffine(0,
                Div(gSinLut[0x40 + (idx & 0xFF)] << 4,
                    gUnknown_0200B0B0->unk68 != 0 ? gUnknown_0200B0B0->unk68 : 2),
                Div(-gSinLut[idx & 0xFF] << 4, 0x100),
                Div(gSinLut[idx & 0xFF] << 4,
                    gUnknown_0200B0B0->unk68 != 0 ? gUnknown_0200B0B0->unk68 : 2),
                Div(gSinLut[0x40 + (idx & 0xFF)] << 4, 0x100));
            gUnknown_0200B0B0->unk67--;
            if (gUnknown_0200B0B0->unk67 < 0)
            {
                gUnknown_0200B0B0->unk66 = 0;
                gUnknown_0200B0B0->unk65 = 0;
            }
            break;
        }

        m2 = gUnknown_0200B0B0->unk00 & 0x40;
        if (m2 == 0)
        {
            if (b == 0)
                sub_0800272C(0, sx + 0xB, sy, gUnknown_0200B0B0->unk2a, 1, b, flag);
            else
                sub_08002844(0, sx + 0xB, sy, gUnknown_0200B0B0->unk24, 1, 0, flag);
        }
    }
    else
    {
        m3 = gUnknown_0200B0B0->unk00 & 0x40;
        if (m3 == 0)
        {
            if (b == 0)
                sub_0800272C(0, sx + 0xB, sy, gUnknown_0200B0B0->unk2a,
                             gUnknown_0200B0B0->unk34, m3, flag);
            else
                sub_08002844(0, sx + 0xB, sy, gUnknown_0200B0B0->unk24,
                             gUnknown_0200B0B0->unk34, m3, flag);
        }
    }

    sub_08003088(sx, sy);

    if (gUnknown_0200B0B0->unk50 > 0x9F)
        return;

    sub_08002510(sx, 0x26);

    m4 = gUnknown_0200B0B0->unk00 & 0x40;
    if (m4 == 0)
    {
        sub_08002298(sx, gUnknown_0200B0B0->unk50 - 10);
        sub_0801F34C(0xAA, (sx + 2) & 0x1FF,
                     (gUnknown_0200B0B0->unk50 - 1) & 0xFF, m4, 0);
        PutSprite(5, sx, gUnknown_0200B0B0->unk50, gUnknown_08485B2C, 0x3000);
    }

    if (gUnknown_0200B0B0->unk04 != 4)
        ApplyPaletteExt(gUnknown_084891C0, 0x260, 0x20);
}
