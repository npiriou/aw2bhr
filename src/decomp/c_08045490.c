#include "global.h"

/* Promoted from assembly; each function below is byte-for-byte
 * identical to the original. Order is address order and must
 * stay that way -- the linker places this file's .text as one
 * contiguous block at 0x08045490.
 * sub_08045490 @ 0x08045490
 */

#include "proc.h"
/* The proc scans the map for the first cell of its own army's terrain type
 * whose 0x51A plane byte is still clear, hands the coordinates to
 * sub_08029088 and returns; running off the end of the map (or finding no
 * free gUnitRecords slot at all) goes to label 1 instead.
 *
 * The cursor lives in the PROC, not in locals: `proc->unk2c` and
 * `proc->unk30` are read and written through memory on every iteration, which
 * is why both loops reload them and why the outer body re-reads unk30 even
 * though the tail just stored it.
 *
 * The three planes are members of a struct laid over gMapData -- the
 * ROM computes `(map + 0x417A) + y * 2`, `(map + 0x1432) + idx` and
 * `(map + 0x51A) + idx`, and only a COMPONENT_REF preserves that association.
 * The 0x51A base is the one that will not fit in a register at this pressure,
 * so it takes the single stack slot.
 *
 * `(gCurrentArmyIndex << 5) | 6` is an `orrs`, not an add: the low bits of the
 * shifted army number are known zero but the ROM still spells the combine as
 * a bitwise or. */
struct Unk45490Map
{
    /* 0x0000 */ u16 width;
    /* 0x0002 */ u16 height;
    /* 0x0004 */ u8 filler_0004[0x051A - 0x0004];
    /* 0x051A */ u8 plane51A[0x1432 - 0x051A];
    /* 0x1432 */ u8 terrain[0x417A - 0x1432];
    /* 0x417A */ u16 rowOffset[1];
};
struct Unk45490Proc
{
    /* 00 */ PROC_HEADER;
    /* 29 */ STRUCT_PAD(0x29, 0x2c);
    /* 2c */ int unk2c;
    /* 30 */ int unk30;
};

void sub_08045490(struct Unk45490Proc *proc)
{
    int idx;

    if (sub_08025AEC() == NULL)
    {
        Proc_Goto(proc, 1);
        return;
    }

    while (proc->unk30 < ((struct Unk45490Map *)gMapData)->height)
    {
        while (proc->unk2c < ((struct Unk45490Map *)gMapData)->width)
        {
            idx = ((struct Unk45490Map *)gMapData)->rowOffset[proc->unk30] + proc->unk2c;

            if (((struct Unk45490Map *)gMapData)->terrain[idx] == ((gCurrentArmyIndex << 5) | 6)
             && ((struct Unk45490Map *)gMapData)->plane51A[idx] == 0)
            {
                sub_08029088(proc->unk2c, proc->unk30);
                return;
            }

            proc->unk2c++;
        }

        proc->unk2c = 0;
        proc->unk30++;
    }

    Proc_Goto(proc, 1);
}
