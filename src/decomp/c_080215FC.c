#include "global.h"

/* Promoted from assembly; each function below is byte-for-byte
 * identical to the original. Order is address order and must
 * stay that way -- the linker places this file's .text as one
 * contiguous block at 0x080215FC.
 * sub_080215FC @ 0x080215FC
 */

/* gMapData is a screen descriptor: u16 width at +0, u16 height at +2,
 * u16 rowOffset[] at +0x417A. This fills rowOffset[y] = y * width.
 *
 * The loop is written as an explicit entry test plus a do/while because that is
 * what the codegen says: the entry test loads the global (`ldr r2,=g; ldr r0,[r2]`)
 * while the bottom test re-reads the height through the body's local `p`
 * (`ldrh r2,[r2,#2]`). A plain `for` whose condition names the global reloads it
 * at the bottom -- the `strh` kills the cached pointer load -- which is one
 * instruction too many. */
void sub_080215FC(void)
{
    u8 *p;
    u8 *rows;
    u32 t;
    u8 i;

    i = 0;

    if (i < *(u16 *)(gMapData + 2))
    {
        do
        {
            p = gMapData;
            t = i * 2;
            rows = p + 0x417A;
            *(u16 *)(rows + t) = i * *(u16 *)p;
            i++;
        }
        while (i < *(u16 *)(p + 2));
    }
}
