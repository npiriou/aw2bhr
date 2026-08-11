#include "global.h"

/* Promoted from assembly; each function below is byte-for-byte
 * identical to the original. Order is address order and must
 * stay that way -- the linker places this file's .text as one
 * contiguous block at 0x08018DF8.
 * sub_08018DF8 @ 0x08018DF8
 */

/* An installed slot callback that draws the node's coordinates relative to the
 * gMapData viewport. The slot's +0x14 word -- declared `u32`, the only
 * other access being sub_08019818's store -- is dereferenced as a node here, so
 * it is cast rather than retyped.
 *
 * The node's unk08/unk0a are declared `u16` and read `ldrsh`, so they are cast,
 * the same treatment they get in c_08017C4C.c. The viewport's +4/+6 are the
 * camera column/row pair documented on gMapData.
 *
 * gMapData is named INLINE at both uses and deliberately not bound to a
 * local: binding it moves its `ldr` ahead of the node's own unk08 read, which is
 * 7 bytes wrong. The binding local is punctuation, and here there was no
 * statement to punctuate -- CSE hoists the pointer to its first real use, which
 * is inside the subtraction. */
void sub_08018DF8(struct Unk0200C528 *slot)
{
    struct Unk0200C528Node *p;

    p = (struct Unk0200C528Node *)slot->unk14;
    sub_08043418((s16)p->unk08 - *(s16 *)(gMapData + 4),
                 (s16)p->unk0a - *(s16 *)(gMapData + 6),
                 p->unk0c);
}
