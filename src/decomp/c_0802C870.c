#include "global.h"

/* Promoted from assembly; each function below is byte-for-byte
 * identical to the original. Order is address order and must
 * stay that way -- the linker places this file's .text as one
 * contiguous block at 0x0802C870.
 * sub_0802C870 @ 0x0802C870
 */

int sub_0802C870(void)
{
  s16 i;
  int idx;
  int o;
  struct UnitRecord **pu;
  u16 *pg;
  i = 1;
  pu = &gUnitRecords;
  pg = &gUnknown_03003F2C;
  idx = (*pg) + i;
  goto test;
  body:
  i++;

  if (i > 0x32)
  {
    goto out;
  }
  idx = (*pg) + i;
  test:
  o = idx * (sizeof(struct UnitRecord));

  if ((idx = ((struct UnitRecord *) (((u8 *) (*pu)) + o))->unk00) == 0)
  {
    goto body;
  }
  out:
  if (i == 0x33)
  {
    return 2;
  }

  return 0;
}
