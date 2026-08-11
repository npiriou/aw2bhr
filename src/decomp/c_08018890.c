#include "global.h"

/* Promoted from assembly; each function below is byte-for-byte
 * identical to the original. Order is address order and must
 * stay that way -- the linker places this file's .text as one
 * contiguous block at 0x08018890.
 * sub_08018890 @ 0x08018890
 */

bool8 sub_08018890(s16 a)
{
    gUnknown_03002F08.unk00 = 8;
    sub_0801A57C(gCurrentArmyIndex);
    gUnknown_0200C528[a].unk04++;
    return TRUE;
}
