#include "global.h"

/* Promoted from assembly; each function below is byte-for-byte
 * identical to the original. Order is address order and must
 * stay that way -- the linker places this file's .text as one
 * contiguous block at 0x08059674.
 * sub_08059674 @ 0x08059674, sub_08059760 @ 0x08059760, sub_08059824 @ 0x08059824, sub_080598BC @ 0x080598BC, sub_08059978 @ 0x08059978
 */

/* K&R declaration, deliberately file-local and deliberately without a
 * parameter list. sub_0805A8C0's definition takes `u16` parameters and narrows
 * them itself in its prologue; this caller passes values it has just
 * sign-extended, with no conversion instruction before the `bl`. A prototyped
 * declaration in unknown-functions.h cannot satisfy both -- see the note there
 * for the four measurements. */
int sub_0805A8C0();
struct Unk59674Map
{
    /* 0x0000 */ u8 filler_00[0x12];
    /* 0x0012 */ u8 unit[0x1420];
    /* 0x1432 */ u8 terrain[0x2d48];
    /* 0x417a */ u16 rows[1];
};
struct Unk0805DFF4Rec
{
    /* 0x00 */ u8 filler_00[0x09];
    /* 0x09 */ u8 unk09_0 : 3;
               u8 unk09_3 : 3;
               u8 unk09_6 : 2;
};

u8 sub_08059674(s16 x, s16 y)
{
    if (((struct Unk59674Map *)gMapData)->unit[
            ((struct Unk59674Map *)gMapData)->rows[y] + x]
                != gUnknown_03003F38
        && ((struct Unk59674Map *)gMapData)->unit[
            ((struct Unk59674Map *)gMapData)->rows[y] + x] != 0)
        return 0;
    if ((u8)sub_0805C988(x, y))
        return 0;
    if (gUnknown_085767D5[((struct Unk59674Map *)gMapData)->terrain[
            ((struct Unk59674Map *)gMapData)->rows[y] + x] & 0x1f] == 0)
        return 1;
    if ((((struct Unk59674Map *)gMapData)->terrain[
            ((struct Unk59674Map *)gMapData)->rows[y] + x] & 0xe0)
                != gUnknown_03004084)
    {
        if ((((struct Unk59674Map *)gMapData)->terrain[
                ((struct Unk59674Map *)gMapData)->rows[y] + x] & 0xe0) == 0)
            return 0;
        if (gUnknown_030040D8->unk00 <= 2)
            return 1;
        if ((u8)sub_0805A8C0(x, y) == 1)
            return 1;
        return 0;
    }
    if (gUnknown_085767B8[((struct Unk59674Map *)gMapData)->terrain[
            ((struct Unk59674Map *)gMapData)->rows[y] + x] & 0x1f] == 0)
        return 1;
    if (gUnknown_085767B8[((struct Unk59674Map *)gMapData)->terrain[
            ((struct Unk59674Map *)gMapData)->rows[y] + x] & 0x1f]
                == gUnknown_030046AC)
        return 1;
    if (!gUnknown_030045CC.unk00_1)
        return 0;
    return 1;
}

void sub_08059760(void)
{
    union Unk802C57CBuf v;

    sub_0801F92C(gMapData + 0x2d5a);
    gUnknown_030013EC(gUnknown_030040D8->unk02, gUnknown_030040D8->unk03,
                      gUnknown_030040D8->unk00, 0x78, -1);
    sub_0801FD9C(0x79);
    v.pos.unk00 = 0x270F;
    sub_0805A9AC(0, &v);
    if (v.pos.unk00 != 0x270F)
    {
        sub_080591E4(&v);
    }
    else
    {
        if (gUnknown_03004784[1] > (u8)(gUnknown_030040D8->unk07[3] % 100))
            sub_0805F914();
        if (gUnknown_03004784[0] > (u8)(gUnknown_030040D8->unk07[3] % 100)
            || sub_0804415C(gCurrentArmyIndex))
            sub_0805E718();
    }
    sub_0805F7B8();
}

void sub_08059824(void)
{
    union Unk802C57CBuf v;

    sub_0801F92C(gMapData + 0x2d5a);
    gUnknown_030013EC(gUnknown_030040D8->unk02, gUnknown_030040D8->unk03,
                      gUnknown_030040D8->unk00, 0x78, 0);
    sub_0801FD9C(0x79);
    v.pos.unk00 = 0x270F;
    sub_0805A9AC(1, &v);
    if (v.pos.unk00 != 0x270F)
    {
        sub_080591E4(&v);
    }
    else
    {
        if (gUnknown_03004784[1] > (u8)(gUnknown_030040D8->unk07[3] % 100))
            sub_0805F914();
    }
    sub_0805EB58();
}

void sub_080598BC(void)
{
    union Unk802C57CBuf v;
    struct Unk03003338 *p;
    int q;

    p = gUnknown_03003F20;
    sub_0805DCD4();
    sub_0801F92C(gMapData + 0x2852);
    gUnknown_030013EC(gUnknown_030040D8->unk02, gUnknown_030040D8->unk03,
                      gUnknown_030040D8->unk00, 0x78, 0);
    sub_08059A0C(p);
    q = sub_08057F00(1);
    v.pos.unk00 = 0x270F;
    sub_08059B4C(q, gUnknown_085766E0->unk04[7], 0, p, &v);
    if (v.pos.unk00 != 0x270F)
    {
        sub_080591E4(&v);
    }
    else if (gUnknown_030046B8 == 2)
    {
        ((struct Unk0805DFF4Rec *)gUnknown_030040D8)->unk09_3 = 3;
        sub_0805FB70();
        sub_0805F4F8();
    }
    sub_0805F7B8();
}

void sub_08059978(void)
{
    union Unk802C57CBuf v;
    struct Unk03003338 *p;
    int q;

    p = gUnknown_03003F20;
    sub_0805DCD4();
    sub_0801F92C(gMapData + 0x2852);
    gUnknown_030013EC(gUnknown_030040D8->unk02, gUnknown_030040D8->unk03,
                      gUnknown_030040D8->unk00, 0x78, 0);
    sub_08059A0C(p);
    q = sub_08057F00(1);
    v.pos.unk00 = 0x270F;
    sub_08059B4C(q, gUnknown_085766E0->unk04[6], 0, p, &v);
    if (v.pos.unk00 == 0x270F)
        sub_0805F7B8();
    sub_080591E4(&v);
}
