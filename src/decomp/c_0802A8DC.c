#include "global.h"

/* Promoted from assembly; each function below is byte-for-byte
 * identical to the original. Order is address order and must
 * stay that way -- the linker places this file's .text as one
 * contiguous block at 0x0802A8DC.
 * sub_0802A8DC @ 0x0802A8DC
 */

struct Map
{
    /* 0x0000 */ u16 unk00;
    /* 0x0002 */ u16 unk02;
    /* 0x0004 */ u16 unk04;
    /* 0x0006 */ u16 unk06;
    /* 0x0008 */ u8 filler_0008[0x0A];
    /* 0x0012 */ u8 unk0012[0x0508];
    /* 0x051A */ u8 unk051A[0x0F18];
    /* 0x1432 */ u8 unk1432[0x0A10];
    /* 0x1E42 */ u8 unk1E42[0x0508];
    /* 0x234A */ u8 unk234A[0x0508];
    /* 0x2852 */ u8 unk2852[0x1928];
    /* 0x417A */ u16 unk417A[0x100];
};

void sub_0802A8DC(int a1, int a2, int a3, int a4, int a5)
{
    int t;
    int k;
    int m;
    int n;

    t = sub_0803EED4(a1, a2);
    k = t & 0x1f;
    m = sub_080249C8(t);
    n = sub_08024984(t);

    sub_08011E54((void *)sub_0802A85C(k), (void *)0x06013940, 0x100);

    sub_0801BD00((a3 + gUnknown_0849A2A6[a5 * 3] + gUnknown_0849A284[0x14]) & 0x1ff,
                 (a4 + gUnknown_0849A284[0x15]) | 0x400,
                 (void *)gUnknown_0849A240,
                 0x11ca);

    if (((struct Map *)gMapData)->unk234A[
            ((struct Map *)gMapData)->unk417A[a2] + a1] == 0 && k != 8)
        m = 0;

    sub_08011E54((void *)sub_0802A880(k, n), (void *)0x06013CC0, 0x100);
    ApplyPaletteExt((u16 *)sub_0802A8AC(k, m), 0x2c0, 0x20);

    sub_0801BD00((a3 + gUnknown_0849A2A6[a5 * 3] + gUnknown_0849A284[0x12]) & 0x1ff,
                 ((a4 + gUnknown_0849A284[0x13]) & 0xff) | 0x400,
                 (void *)gUnknown_0849A1F0,
                 0x61e6);
}
