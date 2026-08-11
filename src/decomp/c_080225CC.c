#include "global.h"

/* Promoted from assembly; each function below is byte-for-byte
 * identical to the original. Order is address order and must
 * stay that way -- the linker places this file's .text as one
 * contiguous block at 0x080225CC.
 * sub_080225CC @ 0x080225CC
 */

/* The gUnknown_08499580 twin of sub_080223E0 -- same 2x2 block, same address
 * form (see the comment there), but filled with 0x360, the same value
 * sub_08013CA8 clears that whole tilemap to. */
void sub_080225CC(u16 x, u16 y)
{
    int cx;
    int cy;

    cx = (x - *(u16 *)(gMapData + 0xc)) & 0xf;
    cy = (y - *(u16 *)(gMapData + 0xe)) & 0xf;

    *(gUnknown_08499580 + cx * 2 + cy * 64) = 0x360;
    *(gUnknown_08499580 + cx * 2 + cy * 64 + 1) = 0x360;
    *(gUnknown_08499580 + cx * 2 + cy * 64 + 0x20) = 0x360;
    *(gUnknown_08499580 + cx * 2 + cy * 64 + 0x21) = 0x360;
}
