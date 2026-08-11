#include "global.h"

/* Promoted from assembly; each function below is byte-for-byte
 * identical to the original. Order is address order and must
 * stay that way -- the linker places this file's .text as one
 * contiguous block at 0x080455CC.
 * sub_080455CC @ 0x080455CC, sub_08045650 @ 0x08045650, sub_080456B8 @ 0x080456B8, sub_08045700 @ 0x08045700
 */

/* The map header at gMapData must be reached as a struct laid over
 * the pointer: the ROM computes `(map + 0x417A) + y * 2` and `(map + 0x1432)
 * + idx`, and only a COMPONENT_REF preserves that association -- a `u16 *`
 * cast reassociates to `(map + y * 2) + 0x417A`. Same spelling that closed
 * sub_08040790 and sub_0804189C.
 *
 * gMapData is named honestly and inline at every read: the `strb`
 * into gUnknown_020288B4 kills the CSE of the pointer deref, which is why the
 * ROM reloads `[r5]` on every inner iteration and why the address constant
 * ends up materialised twice (r8 for the outer bound, r5 for the body). */
struct Unk455CCMap
{
    /* 0x0000 */ u16 width;
    /* 0x0002 */ u16 height;
    /* 0x0004 */ u8 filler_0004[0x1432 - 0x0004];
    /* 0x1432 */ u8 terrain[0x417A - 0x1432];
    /* 0x417A */ u16 rowOffset[1];
};
/* The read-only twin of sub_080455CC: the same double scan of the map header
 * at gMapData, asking whether any cell of the parallel byte plane at
 * gUnknown_020288B4 is set. With no store in the loop, LICM hoists the height
 * and the width out of the outer loop for the guards while the map deref is
 * still reloaded per outer iteration, and strength_reduce turns
 * `gUnknown_020288B4[rowOffset[y] + x]` into the r1 pointer giv.
 *
 * `return TRUE` and not a `break`: with a break the array address would keep
 * three references at expand time and the in-loop `ldr` would stop being
 * invariant, and the reduction declines. */
struct Unk45650Map
{
    /* 0x0000 */ u16 width;
    /* 0x0002 */ u16 height;
    /* 0x0004 */ u8 filler_0004[0x417A - 0x0004];
    /* 0x417A */ u16 rowOffset[1];
};
/* The RLE COMPRESSOR, exact inverse of the matched sub_080456B8 expander: it
 * walks the whole byte plane at gUnknown_020288B4 and writes, into the caller's
 * buffer, a literal `cell | 0x80` for every non-zero cell and a plain count for
 * every run of zeroes, flushing the pending run before a literal and whenever it
 * reaches 0x7f, then terminating with 0xff.
 *
 * gUnknown_020288B4 is bound to a LOCAL: the ROM materialises its address before
 * the two zero inits and indexes it `adds r1, r6, r5` (pointer + int).  Naming
 * the array inline instead emits the address AFTER the loop guard and the
 * reversed `adds r1, r5, r6`, and swaps the two pool words -- 2 instructions.
 *
 * The map header is NOT bound: the ROM keeps `[gMapData]` in r3 across
 * the loop (LICM hoisted the pointer deref, which is why the `ldr` sits after
 * `run = 0` and `i = 0` rather than before them) but reloads width and height
 * every iteration, because the `strb` through the `u8 *` parameter kills them.
 *
 * `run` is u8, read off the truncation on its own increment. */
struct Unk45700Map
{
    /* 0x0000 */ u16 width;
    /* 0x0002 */ u16 height;
};

void sub_080455CC(void)
{
    int x;
    int y;
    int idx;

    for (y = 0; y < ((struct Unk455CCMap *)gMapData)->height; y++)
    {
        for (x = 0; x < ((struct Unk455CCMap *)gMapData)->width; x++)
        {
            idx = ((struct Unk455CCMap *)gMapData)->rowOffset[y] + x;

            if (((struct Unk455CCMap *)gMapData)->terrain[idx] == 0x10)
                gUnknown_020288B4[idx] = 0x63;
            else
                gUnknown_020288B4[idx] = 0;
        }
    }
}

bool8 sub_08045650(void)
{
    int x;
    int y;

    for (y = 0; y < ((struct Unk45650Map *)gMapData)->height; y++)
        for (x = 0; x < ((struct Unk45650Map *)gMapData)->width; x++)
            if (gUnknown_020288B4[((struct Unk45650Map *)gMapData)->rowOffset[y] + x] != 0)
                return TRUE;

    return FALSE;
}

/* An RLE expander into gUnknown_020288B4: a byte with bit 7 set is one
 * literal (its low seven bits), a byte without it is a run of that many
 * zeroes, and 0xff terminates.
 *
 * `*src` is spelled out at every read rather than bound to a local -- the
 * `strb` through `dst` is what kills the CSE, so the load survives inside the
 * zero-run loop while the pair of masks in the literal arm share the one load
 * the `!= 0xff` test made. The `src + 1` in the run arm's preheader is the
 * trailing `src++` hoisted (src is invariant in the loop); the zero-trip guard
 * branches straight to it, which is why the increment is not conditional. */
void sub_080456B8(u8 *src)
{
    u8 *dst;
    int i;

    dst = gUnknown_020288B4;

    while (*src != 0xff)
    {
        if (*src & 0x80)
        {
            *dst = *src & 0x7f;
            src++;
            dst++;
        }
        else
        {
            for (i = 0; i < *src; i++)
            {
                *dst = 0;
                dst++;
            }

            src++;
        }
    }
}

void sub_08045700(u8 *dst)
{
    u8 *src;
    int i;
    u8 run;

    src = gUnknown_020288B4;
    run = 0;

    for (i = 0;
         i < ((struct Unk45700Map *)gMapData)->width
           * ((struct Unk45700Map *)gMapData)->height;
         i++)
    {
        if (src[i] != 0)
        {
            if (run != 0)
            {
                *dst = run;
                dst++;
                run = 0;
            }

            *dst = src[i] | 0x80;
            dst++;
        }
        else
        {
            run++;

            if (run == 0x7f)
            {
                *dst = run;
                dst++;
                run = 0;
            }
        }
    }

    if (run != 0)
    {
        *dst = run;
        dst++;
    }

    *dst = 0xff;
}
