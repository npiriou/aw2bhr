#include "global.h"

/* Promoted from assembly; each function below is byte-for-byte
 * identical to the original. Order is address order and must
 * stay that way -- the linker places this file's .text as one
 * contiguous block at 0x0804438C.
 * sub_0804438C @ 0x0804438C
 */

void sub_0804438C(int a1, int a2)
{
    sub_08044354(a1);
    sub_080440A8(a1, a2);
    gArmyRecords[a1].unk24 = 0;
    sub_08039634(a1, a2);
}
