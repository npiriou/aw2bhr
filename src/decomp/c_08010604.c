#include "global.h"

/* Promoted from assembly; each function below is byte-for-byte
 * identical to the original. Order is address order and must
 * stay that way -- the linker places this file's .text as one
 * contiguous block at 0x08010604.
 * sub_08010604 @ 0x08010604
 */

/* The map header sub_0800F2E0 and its neighbours walk, seen through the LOCAL
 * struct spelling recorded on gMapData in include/unknown-globals.h.
 * unk0A22 is a REFINEMENT of the canonical `u8 unk051A[0x0F18]` used by the
 * twelve promoted files that already declare `struct Map`: that run splits at
 * 0x0A22 into a u16 plane of the same 0x508 cells as unk0012, indexed by the
 * same `unk417A[y] + x` key, and 0x0A22 + 0x508 * 2 lands exactly on unk1432.
 * Every access here is `(map + K) + idx`, which only the COMPONENT_REF spelling
 * preserves. */
struct Map
{
    /* 0x0000 */ u16 unk00;
    /* 0x0002 */ u16 unk02;
    /* 0x0004 */ u16 unk04;
    /* 0x0006 */ u16 unk06;
    /* 0x0008 */ u8 filler_0008[0x0A];
    /* 0x0012 */ u8 unk0012[0x0508];
    /* 0x051A */ u8 unk051A[0x0508];
    /* 0x0A22 */ u16 unk0A22[0x0508];
    /* 0x1432 */ u8 unk1432[0x0A10];
    /* 0x1E42 */ u8 unk1E42[0x0508];
    /* 0x234A */ u8 unk234A[0x0508];
    /* 0x2852 */ u8 unk2852[0x0A10];
    /* 0x3262 */ u8 unk3262[0x0F18];
    /* 0x417A */ u16 unk417A[0x100];
};

/* Maps a road/bridge cell id onto the id its neighbours should be drawn with:
 * the 0x142/0x162 pair collapses to 0x162, the 0x143/0x163 pair to 0x163, and
 * anything else to 0x180. The `add r0,#0x20` / `add r0,#1` chains are agbcc
 * reusing the previous compare's constant, not a source-level expression. */
int sub_08010604(int x, int y)
{
    u16 t;

    t = ((struct Map *)gMapData)->unk0A22[
            ((struct Map *)gMapData)->unk417A[y] + x];
    if (t == 0x142 || t == 0x162)
        return 0x162;
    if (t == 0x143 || t == 0x163)
        return 0x163;
    return 0x180;
}
