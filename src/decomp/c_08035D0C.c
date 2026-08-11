#include "global.h"

/* Promoted from assembly; each function below is byte-for-byte
 * identical to the original. Order is address order and must
 * stay that way -- the linker places this file's .text as one
 * contiguous block at 0x08035D0C.
 * sub_08035D0C @ 0x08035D0C
 */

/* Promoted from assembly; each function below is byte-for-byte
 * identical to the original. Order is address order and must
 * stay that way -- the linker places this file's .text as one
 * contiguous block at 0x08035D0C.
 * sub_08035D0C @ 0x08035D0C
 */

#include "proc.h"
/* Picks the help/message id for whatever the proc's cursor is standing on:
 * two overriding predicates first, then a table on the low five bits of the
 * gMapData terrain byte at the same `rowOffset[y] + x` key
 * sub_08035C90 uses (same `(v + 8) / 16` pixel-to-cell divide).
 *
 * The 0x2d arm of the switch and the sub_08035C90 early return are ONE block in
 * the ROM -- cross-jumping merged them, which is why the jump table's entries
 * for 2, 7 and 19 point back above the switch.
 *
 * There is NO `struct ... *proc = procArg;` local here, and that is 2 bytes:
 * the ROM keeps the incoming pointer in r4 alone (`push {r4, lr}`), while a
 * binding local makes a second pseudo that gets copied into r5 for the second
 * member read and pushes r5 as well.  The cast at each use is free. */
struct Unk35D0CMap
{
    /* 0x0000 */ u8 filler_0000[0x1432];
    /* 0x1432 */ u8 plane[0x417A - 0x1432];
    /* 0x417A */ u16 rowOffset[1];
};
struct Unk35D0CProc
{
    /* 0x00 */ PROC_HEADER;
    /* 0x29 */ STRUCT_PAD(0x29, 0x42);
    /* 0x42 */ s16 unk42;
    /* 0x44 */ s16 unk44;
};

u16 sub_08035D0C(ProcPtr proc)
{
    struct Unk35D0CMap *map;

    if (sub_08035C90(proc))
        return 0x2d;

    if (sub_08035CF4(proc))
        return 0x43;

    map = (struct Unk35D0CMap *)gMapData;

    switch (map->plane[map->rowOffset[
                (((struct Unk35D0CProc *)proc)->unk44 + 8) / 16]
                       + (((struct Unk35D0CProc *)proc)->unk42 + 8) / 16] & 0x1f)
    {
    case 2:
    case 7:
    case 19:
        return 0x2d;
    case 3:
        return 0x4b;
    case 4:
    case 13:
        return 0x47;
    case 1:
    case 5:
    case 6:
    case 8:
    case 10:
    case 11:
    case 12:
    case 14:
        return 0x29;
    }

    return 0;
}
