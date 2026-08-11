#include "global.h"

/* Promoted from assembly; each function below is byte-for-byte
 * identical to the original. Order is address order and must
 * stay that way -- the linker places this file's .text as one
 * contiguous block at 0x080271CC.
 * sub_080271CC @ 0x080271CC
 */

u8 sub_080271CC(int a1)
{
  int i;
  int new_var2;
  unsigned int new_var;
  int idx;
  int o;
  struct UnitRecord **pu;
  struct UnitRecord **pv;
  s16 *pe;
  if (!sub_080266DC(a1))
  {
    return 0;
  }
  if (sub_08025CF0(a1) == 0)
  {
    return 1;
  }
  i = 1;
  pu = &gUnitRecords;
  new_var2 = i;
  pe = &gUnknown_084995FE[a1];
  idx = (u16) (*pe);
  idx = idx + new_var2;
  o = idx * (sizeof(struct UnitRecord));
  if ((idx = ((struct UnitRecord *) (((u8 *) (*pu)) + o))->unk00) != 0)
  {
    goto out;
  }
  pv = pu;
  body:
  i++;

  if (i > 0x32)
  {
    goto out;
  }
  idx = i + ((u16) (*pe));
  o = idx * (sizeof(struct UnitRecord));
  new_var2 = o;
  new_var = ((struct UnitRecord *) (((u8 *) (*pv)) + new_var2))->unk00;
  if ((idx = new_var) == 0)
  {
    goto body;
  }
  out:
  if (i != 0x33)
  {
    return 1;
  }

  return 0;
}
