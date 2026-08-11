#include "global.h"

/* Promoted from assembly; each function below is byte-for-byte
 * identical to the original. Order is address order and must
 * stay that way -- the linker places this file's .text as one
 * contiguous block at 0x0805B3F4.
 * sub_0805B3F4 @ 0x0805B3F4
 */

void sub_0805B3F4(void)
{
    u8 buf[0x19];
    struct UnitRecord *u;

    sub_0808B6C4(buf, 0, 0x19);
    buf[1] = 8;
    buf[2] = 8;
    buf[7] = 8;
    buf[9] = 9;

    sub_080581A4(gMapData + 0x3c72, 0);
    sub_0801F92C(gMapData + 0x2852);

    u = &gUnitRecords[gUnknown_030040D8->unk07[0]];
    gUnknown_030046D4 = u->unk00;

    if (gUnknown_030040D8->unk07[4] > 7)
        gUnknown_030040D8->unk07[4] = 1;

    if (u->unk0b != 0)
        gUnknown_08576890[u->unk0b]();
    else
        gUnknown_08576890[buf[gUnknown_030046D4]]();
}
