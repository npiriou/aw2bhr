#include "global.h"

/* Promoted from assembly; each function below is byte-for-byte
 * identical to the original. Order is address order and must
 * stay that way -- the linker places this file's .text as one
 * contiguous block at 0x0802C3D0.
 * sub_0802C3D0 @ 0x0802C3D0
 */

#include "hardware.h"

void sub_0802C3D0(void)
{
    sub_08023824();
    sub_08023518();
    sub_08023908(8);

    if (*(u16 *)(gMapData + 0x10) & 0xf)
        return;

    if (*(u32 *)&gUnknown_030033E0 & 0x00070007)
        return;

    if (!(gpKeySt->held & 0xb))
        return;

    sub_0803B4DC(0x66);
    sub_08015C30(gUnknown_03001FBC);
}
