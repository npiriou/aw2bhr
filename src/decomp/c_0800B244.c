#include "global.h"

/* Promoted from assembly; each function below is byte-for-byte
 * identical to the original. Order is address order and must
 * stay that way -- the linker places this file's .text as one
 * contiguous block at 0x0800B244.
 * sub_0800B244 @ 0x0800B244
 */

/* The screen-descriptor address computation of sub_08001158, done twice: once
 * for (x, y) at entry and once for (x, y - 1) in the 0x23 branch.
 *
 * Two spellings here are load-bearing and neither is guessable:
 *   * `tiles += off` and not `*(u16 *)(tiles + off)`. The latter accumulates
 *     into off's register and `p + 0xA22 + off` reassociates to
 *     `p + (0xA22 + off)`; only the compound assignment leaves the sum in
 *     tiles' register, which is what `add r1, r8; adds r1, r1, r0` says.
 *   * `y - 1` written out at every use rather than bound to a local. A local
 *     gives a single `subs r6, r7, #1`; the inline form gives the ROM's
 *     `subs r4, r7, #1` ... `adds r6, r4, #0` temp-plus-copy pair, at all
 *     three sites.
 *
 * The `default: goto skipN` in the first switch of each arm is not a
 * decoration either: a plain `break` would fall into the three calls, and the
 * ROM branches past them. */
void sub_0800B244(int x, int y)
{
    u8 *p;
    u8 *rows;
    u8 *tiles;
    int t;
    int off;
    int tile;
    int v;

    p = gMapData;
    t = y * 2;
    rows = p + 0x417A;
    off = (*(u16 *)(rows + t) + x) * 2;
    tiles = p + 0xA22;
    tiles += off;
    tile = *(u16 *)tiles;

    if (sub_0800B1FC(x, y) == 0)
    {
        if (y <= 0)
            return;

        if (sub_0800119C(x, y - 1, 1) != 0)
        {
            switch (sub_0800B1FC(x, y - 1))
            {
            case 0:
                v = 2;
                break;
            case 1:
                v = 0x22;
                break;
            case -1:
                v = 3;
                break;
            default:
                goto skip1;
            }

            sub_08001158(x, y - 1, v);
            v = sub_08001704(x, y - 1, v);
            sub_08001158(x, y - 1, v);

        skip1:
            switch (sub_0800B1FC(x, y + 1))
            {
            case 1:
                v = 0x22;
                break;
            case 0:
                v = 0x23;
                break;
            case -1:
                v = 0x23;
                break;
            default:
                v = -1;
                break;
            }

            sub_08001158(x, y, v);
        }
        else
        {
            if (sub_0800119C(x, y - 1, 3) == 0)
                return;

            switch (sub_0800B1FC(x, y - 1))
            {
            case 0:
                v = 2;
                break;
            case 1:
                v = 0x22;
                break;
            case -1:
                v = 3;
                break;
            default:
                goto skip2;
            }

            sub_08001158(x, y - 1, v);
            v = sub_08001704(x, y - 1, v);
            sub_08001158(x, y - 1, v);

        skip2:
            switch (sub_0800B1FC(x, y + 1))
            {
            case 1:
                v = 0x22;
                break;
            case 0:
                v = 0x23;
                break;
            case -1:
                v = 0x23;
                break;
            default:
                v = -1;
                break;
            }

            sub_08001158(x, y, v);
        }
    }
    else if (tile == 0x23)
    {
        if (sub_0800119C(x, y - 1, 3) == 0)
        {
            sub_08001158(x, y - 1, 1);

            if (x > 0 && sub_0800AFCC(x - 1, y - 1) != 0)
                sub_08001158(x, y - 1, 0x21);

            sub_08001158(x, y, 0x20);
            v = sub_08001A04(x, y - 1, 1);
            sub_08001158(x, y - 1, v);
        }
        else
        {
            int up;

            tiles = gMapData;
            p = tiles;
            t = (y - 1) * 2;
            rows = p + 0x417A;
            off = (*(u16 *)(rows + t) + x) * 2;
            tiles = tiles + 0xA22;
            tiles += off;
            up = *(u16 *)tiles;

            if (up == 0x20 || up == 2)
                sub_08001158(x, y - 1, 0x20);
            else
                sub_08001158(x, y - 1, 0x23);

            sub_08001158(x, y, 0x20);
        }
    }
    else if (tile == 0x22)
    {
        if (sub_0800119C(x, y - 1, 1) != 0)
        {
            if (x > 0 && sub_0800AFCC(x - 1, y - 1) != 0)
                v = 0x21;
            else
                v = 1;

            sub_08001158(x, y - 1, v);
            v = sub_08001A04(x, y - 1, 1);
            sub_08001158(x, y - 1, v);
        }
        else if (sub_0800119C(x, y - 1, 3) != 0)
        {
            sub_08001158(x, y - 1, sub_0800B1FC(x, y - 1) == 0 ? 0x20 : 0x23);
        }

        switch (sub_0800B1FC(x, y + 1))
        {
        case 0:
        default:
            v = 0x20;
            break;
        case 1:
            v = 2;
            break;
        }

        sub_08001158(x, y, v);
    }
}
