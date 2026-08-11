#include "global.h"

/* Promoted from assembly; each function below is byte-for-byte
 * identical to the original. Order is address order and must
 * stay that way -- the linker places this file's .text as one
 * contiguous block at 0x08061178.
 * sub_08061178 @ 0x08061178
 */

/* One row of the 12-byte-strided table that *gUnknown_085766E0 addresses,
 * reached at +0x14. See the CONFLICT note beside struct Unk085766E0 in
 * include/unknown-globals.h: that tag's declared extent (0x23) cannot carry
 * this stride, so the object is described here with a file-local view and cast
 * onto the shared symbol, exactly as the gMapData readers do.
 *
 * v[] is SEVEN bytes: the scan runs `cmp r2, #6; ble`, so indices 0..6. 0xFF
 * is both the "row absent" marker (tested at v[0] before the loop) and a
 * per-entry skip inside it. The remaining five bytes of the row are unread
 * here; only the 12-byte stride is proved.
 *
 * The two reads of v[] compile to two DIFFERENT address associations from this
 * one spelling, which is why nothing here is written as byte arithmetic:
 * the guard load folds 0x14 into the ldrb displacement (`adds r0,r3,r1;
 * ldrb r0,[r0,#0x14]`) while the loop needs the address in a register and so
 * keeps the component-ref order (`adds r0,r3,#0; adds r0,#0x14; adds r0,r1,r0`).
 *
 * `% 100` goes through __umodsi3, the UNSIGNED helper, which is what makes
 * sub_080129E0's u32 result and the u8 truncation of the remainder the right
 * reading. The `(u8)` narrowings on the parameter, on the remainder and on the
 * returned `i + 1` are all PROMOTE_MODE/return-width, not source casts. */
struct Unk61178Row
{
    /* 0x00 */ u8 v[7];
    /* 0x07 */ u8 filler_07[5];
};
struct Unk61178Tbl
{
    /* 0x00 */ u8 filler_00[0x14];
    /* 0x14 */ struct Unk61178Row rows[1];
};

u8 sub_08061178(u8 a1)
{
    struct Unk61178Tbl *tbl;
    u8 r;
    int i;

    r = sub_080129E0() % 100;
    tbl = (struct Unk61178Tbl *)gUnknown_085766E0;

    if (tbl->rows[a1].v[0] == 0xff)
    {
        if (a1 <= 2)
            return 0;
    }
    else
    {
        for (i = 0; i <= 6; i++)
        {
            if (tbl->rows[a1].v[i] != 0xff && tbl->rows[a1].v[i] > r)
                return i + 1;
        }
    }

    return 1;
}
