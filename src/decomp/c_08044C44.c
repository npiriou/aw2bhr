#include "global.h"

/* Promoted from assembly; each function below is byte-for-byte
 * identical to the original. Order is address order and must
 * stay that way -- the linker places this file's .text as one
 * contiguous block at 0x08044C44.
 * sub_08044C44 @ 0x08044C44, sub_08044C80 @ 0x08044C80, sub_08044CBC @ 0x08044CBC, sub_08044CF8 @ 0x08044CF8, sub_08044D34 @ 0x08044D34
 */

#include "proc.h"

/* One of five wrappers over the nine-argument sub_08044D70 (four arguments in
 * registers, five on the stack). They differ only in the blob, the palette and
 * four small integers; the wrapper's own parameter is the LAST argument and is
 * sub_08044D70's `parent` -- see the prototype's comment in
 * include/unknown-functions.h for how the nine were read off the callee.
 *
 * The `str` ordering into the stack slots and the sharing of the constant
 * registers are agbcc's: writing the call as a plain nine-argument call
 * reproduces the ROM's `movs`/`subs` chain exactly, including sub_08044C80's
 * `movs r4,#0; subs r4,#1` for -1 and sub_08044CF8's `subs r4,#2` off a live 1.
 * `pop {r4, r5, r6}; pop {r0}` -- void.
 */
void sub_08044C44(ProcPtr parent)
{
    sub_08044D70(gUnknown_084A0994, gUnknown_08112F00, gUnknown_08113BC0,
                 gCurrentArmyIndex, 2, 0, 1, 0, parent);
}

/* One of five wrappers over the nine-argument sub_08044D70 (four arguments in
 * registers, five on the stack). They differ only in the blob, the palette and
 * four small integers; the wrapper's own parameter is the LAST argument and is
 * sub_08044D70's `parent` -- see the prototype's comment in
 * include/unknown-functions.h for how the nine were read off the callee.
 *
 * The `str` ordering into the stack slots and the sharing of the constant
 * registers are agbcc's: writing the call as a plain nine-argument call
 * reproduces the ROM's `movs`/`subs` chain exactly, including sub_08044C80's
 * `movs r4,#0; subs r4,#1` for -1 and sub_08044CF8's `subs r4,#2` off a live 1.
 * `pop {r4, r5, r6}; pop {r0}` -- void.
 */
void sub_08044C80(ProcPtr parent)
{
    sub_08044D70(gUnknown_084A0994, gUnknown_0811315C, gUnknown_08113BA0,
                 gCurrentArmyIndex, 1, 0, -1, 1, parent);
}

/* One of five wrappers over the nine-argument sub_08044D70 (four arguments in
 * registers, five on the stack). They differ only in the blob, the palette and
 * four small integers; the wrapper's own parameter is the LAST argument and is
 * sub_08044D70's `parent` -- see the prototype's comment in
 * include/unknown-functions.h for how the nine were read off the callee.
 *
 * The `str` ordering into the stack slots and the sharing of the constant
 * registers are agbcc's: writing the call as a plain nine-argument call
 * reproduces the ROM's `movs`/`subs` chain exactly, including sub_08044C80's
 * `movs r4,#0; subs r4,#1` for -1 and sub_08044CF8's `subs r4,#2` off a live 1.
 * `pop {r4, r5, r6}; pop {r0}` -- void.
 */
void sub_08044CBC(ProcPtr parent)
{
    sub_08044D70(gUnknown_084A0994, gUnknown_0811315C, gUnknown_08113BC0,
                 gCurrentArmyIndex, 2, 0, 2, 1, parent);
}

/* One of five wrappers over the nine-argument sub_08044D70 (four arguments in
 * registers, five on the stack). They differ only in the blob, the palette and
 * four small integers; the wrapper's own parameter is the LAST argument and is
 * sub_08044D70's `parent` -- see the prototype's comment in
 * include/unknown-functions.h for how the nine were read off the callee.
 *
 * The `str` ordering into the stack slots and the sharing of the constant
 * registers are agbcc's: writing the call as a plain nine-argument call
 * reproduces the ROM's `movs`/`subs` chain exactly, including sub_08044C80's
 * `movs r4,#0; subs r4,#1` for -1 and sub_08044CF8's `subs r4,#2` off a live 1.
 * `pop {r4, r5, r6}; pop {r0}` -- void.
 */
void sub_08044CF8(ProcPtr parent)
{
    sub_08044D70(gUnknown_084A0994, gUnknown_081133D0, gUnknown_08113BA0,
                 gCurrentArmyIndex, 1, 1, -1, 0, parent);
}

/* One of five wrappers over the nine-argument sub_08044D70 (four arguments in
 * registers, five on the stack). They differ only in the blob, the palette and
 * four small integers; the wrapper's own parameter is the LAST argument and is
 * sub_08044D70's `parent` -- see the prototype's comment in
 * include/unknown-functions.h for how the nine were read off the callee.
 *
 * The `str` ordering into the stack slots and the sharing of the constant
 * registers are agbcc's: writing the call as a plain nine-argument call
 * reproduces the ROM's `movs`/`subs` chain exactly, including sub_08044C80's
 * `movs r4,#0; subs r4,#1` for -1 and sub_08044CF8's `subs r4,#2` off a live 1.
 * `pop {r4, r5, r6}; pop {r0}` -- void.
 */
void sub_08044D34(ProcPtr parent)
{
    sub_08044D70(gUnknown_084A0994, gUnknown_081133D0, gUnknown_08113BC0,
                 gCurrentArmyIndex, 2, 2, -1, 0, parent);
}
