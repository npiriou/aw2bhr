#include "global.h"

/* Promoted from assembly; each function below is byte-for-byte
 * identical to the original. Order is address order and must
 * stay that way -- the linker places this file's .text as one
 * contiguous block at 0x0804B55C.
 * sub_0804B55C @ 0x0804B55C
 */

/* Two things here are not free choices.
 *
 * 1. The gMapData map is addressed through a STRUCT, not through the
 * `u8 *` locals src/decomp/c_0804B42C.c uses. Both models describe the same
 * bytes, but only the struct keeps `base + 0x417A` and `base + 0x1432`
 * grouped: with plain pointer arithmetic `fold` canonicalises `(base + C) + i`
 * into `(i + base) + C` and emits the constant last, while a COMPONENT_REF is
 * not reassociable, so the ROM's `adds rX, base, #0x417A` / `adds base, base,
 * #0x1432` pairs come out AND each is materialised at its use. Binding rows
 * and terrain to locals gets the grouping right but hoists both computations
 * ahead of the `lsls`, which is the same instruction count in the wrong order.
 *
 * 2. Cases 1 and 5 must subscript the table TWICE -- `if (t[k] != 0)
 * result = t[k];` -- with no local for the loaded value. CSE collapses the two
 * reads to one either way, but the element ADDRESS then lives across the
 * compare and takes the table base's register, giving the ROM's
 * `adds r1, r0, r1` where a `v` local produces `adds r0, r0, r1`. Two bytes,
 * and the only difference between a 99.1% draft and this one. */
struct MapHdr
{
    /* 0x0000 */ u16 w;
    /* 0x0002 */ u16 h;
    /* 0x0004 */ u8 filler_04[0x142e];
    /* 0x1432 */ u8 terrain[0x2d48];
    /* 0x417a */ u16 rows[1];
};

u16 sub_0804B55C(u16 a, u8 *p, int unused)
{
    struct MapHdr *m;
    u16 *q;
    u16 result;
    u8 k;
    int t;
    int x;
    int y;

    result = a;
    x = p[2];
    y = p[3];
    m = (struct MapHdr *)gMapData;
    t = sub_08024984(m->terrain[m->rows[y] + x]);

    switch (a)
    {
    case 1:
        k = sub_0804B42C(x, y);
        if (gUnknown_08551C00[k] != 0)
            result = gUnknown_08551C00[k];
        break;
    case 6:
        q = gUnknown_08551C88[t];
        result = q[sub_080129E0() & 1];
        break;
    case 5:
        k = sub_0804B4C4(x, y);
        if (gUnknown_08551C3A[k] != 0)
            result = gUnknown_08551C3A[k];
        break;
    case 3:
        if (sub_080129E0() & 1)
            result = 0x2c;
        break;
    case 4:
        if (sub_080129E0() & 1)
            result = 0x2d;
        break;
    case 8:
        result = t + 0x2d;
        break;
    }

    return result;
}
