#ifndef UNKNOWN_FUNCS_H
#define UNKNOWN_FUNCS_H

#include "global.h"

/* These eight take a 32-bit first argument, not the s16 they were declared with
 * until wave 7. All eight open with a bare `adds r4, r0, #0` and no extension,
 * and the ROM's own ARM PROMOTE_MODE zero-extends any sub-word parameter that
 * has to survive a call -- `s16` compiles to `adds r4, r0, #0; lsl #16; lsr #16`
 * and cannot match. Their callers in proc.c pass `dataImm`, which is s16, so
 * the caller side is unaffected either way. */
void sub_08011550(int);
void sub_0801156C(int);
void sub_08011588(int);
void sub_080115B4(int);
void sub_080115E0(int, ProcPtr);
void sub_080115F8(int, ProcPtr);
void sub_08011610(int, ProcPtr);
void sub_0801163C(int, ProcPtr);
/* Same shape and the same `int` argument as the eight above: both are
 * `adds r4, r0, #0` / `ldr r0,=<script>` / `movs r1,#3` / `bl Proc_Start` /
 * `strh r4,[r0,#0x64]`, with no narrowing on the saved argument. Both end
 * `pop {r4}; pop {r0}`, so both are void. */
void sub_08011668(int);
void sub_08011684(int);
/* What that `int` IS, since ten functions now carry it: a fade SPEED, in
 * per-frame units, and not a channel or layer mask. gUnknown_0848923C's script
 * is `CALL sub_08011054; SLEEP; REPEAT sub_080110A4`, and Proc_Start runs the
 * script to its first block BEFORE returning (src/proc.c: `RunProcessScript`
 * sits inside Proc_Start), so the CALL's own `unk64 = 0x100` is a DEFAULT that
 * the caller's `strh r4,[r0,#0x64]` then overwrites. Each frame sub_080110A4
 * does `unk66 += unk64`, clamps at 0x1000, and publishes
 * `gUnknown_03001FFC = (s16)unk66 >> 8` -- the 0..0x10 blend coefficient. So
 * the fade runs for 0x1000/arg frames, and the 4 / 0x10 / 0x40 that the
 * wrappers at 0x08071F88-0x08072040 pass are a slow / normal / fast ladder.
 * (Unproven: whether the value is meant to be read as 8.8 fixed point. The
 * accumulator is, but nothing narrows the argument to 16 bits.) */
/* `Proc_Find` on each of the four scripts the starters above run --
 * 084892C4, 0848929C, 0848925C, 0848923C -- returning 1 if any is alive and 0
 * otherwise, i.e. "a fade is in progress". `bool8` and not `int`: its only
 * caller, sub_08072288, narrows the result with `lsls r0, r0, #0x18` before
 * `cmp r0, #0`, and an int-wide return emits no such shift. */
bool8 sub_080116A0(void);
void Decompress(u8 *, void *);

/* Two leaves in the 0x08000-0x08004 block, both reached from the wrapper family
 * at 0x08004A60-0x08004B6C. sub_08000654 is already promoted in
 * src/decomp/c_08000654.c as void(void). sub_08004A30 takes an `int`: its
 * prologue is a bare `adds r4, r0, #0` with no narrowing, which PROMOTE_MODE
 * makes `int` and not a narrow type (a u8/u16 parameter surviving a `bl` would
 * carry `lsl #24; lsr #24` / `lsl #16; lsr #16`), and sub_08004A90 passes -1,
 * which fixes the sign. It ends `pop {r4}; pop {r0}; bx r0`, so it is void. */
void sub_08000654(void);
void sub_08004A30(int);
/* A sixteen-byte halfword fill in the same block, and the whole body is
 * readable: `cmp r1,#0; ble out; loop: strh r2,[r0]; adds r0,#2; subs r1,#1;
 * bgt loop`. So r0 is a `u16 *` (the `strh` with a +2 stride), r1 is a SIGNED
 * count (`ble`/`bgt`, and the zero-trip test is signed), and r2 is the halfword
 * written. Its one caller, sub_08072A88, passes a VRAM char-block address, 0x10
 * and 0 -- all three arguments are constants or an address there, so the widths
 * are byte-neutral at that call site and only the body constrains them.
 *
 * And the body rules the third one OUT as `u16`, which this was declared as
 * for part of wave 21. PROMOTE_MODE emits the `lsl #16; lsr #16` in the
 * PROLOGUE for a sub-word parameter, unconditionally -- it does NOT get
 * deleted when the only use is a `strh` that discards the upper half. A `u16`
 * third parameter compiles this function to 20 bytes; the ROM is 16 and its
 * prologue is bare. So `int`, and the sign stays undetermined because the one
 * call site passes 0. Both functions are matched, which pins it. */
void sub_08001148(u16 *, int, int);
/* The BYTE fill sixteen bytes in front of it, and the identical body with
 * `strb` and `adds #1` where sub_08001148 has `strh` and `adds #2`. Wave 42
 * matched it by changing the pointer type in c_08001148.c and nothing else.
 * The third parameter is `int` for the reason spelled out above -- a `u8` there
 * puts an `lsl #24; lsr #24` pair in the prologue that the ROM does not have.
 * No caller in the tree yet, so only the body constrains the widths. */
void sub_08001138(u8 *, int, int);

/* Two builders of the same gUnknown_030044B0 command block, both handing it to
 * sub_080308B4 at the end and both void (`pop {r0}; bx r0`).
 *
 * In each, the argument that indexes gUnknown_08499594 is the one narrowed at
 * entry with `lsls #0x18; lsrs #0x18` -- sub_08034534's second and
 * sub_080344B4's first -- so those two are `u8` and the rest are `int`. That
 * pair is the whole type readout: every other parameter reaches memory through
 * a bare `strb` into the block (+0, +1, +6, +7), which is byte-identical for
 * `int` and `u8` alike, so `int` is the weakest model that fits. sub_08034534's
 * first parameter is a command id (sub_0802CFFC passes 2) and is the field
 * sub_080344B4 hard-codes to 8. Verified at sub_0802CFFC, which calls both. */
/* Wave 30, W30-A retypes the third and fourth parameters from `int` to `u8`.
 * sub_08041E48 passes two whole-word proc members and the ROM narrows each with
 * `lsls #0x18; lsrs #0x18` before the `bl` -- 8 bytes that an `int` parameter
 * does not emit. The previous `int, int` was never evidence: the only other
 * caller, src/decomp/c_0802CFFC.c, passes two literal zeros, which are
 * byte-identical at any width. */
void sub_08034534(int, u8, u8, u8);
void sub_080344B4(u8, int, int);
/* Definition promoted in src/decomp/c_080308B4.c; its body copies src[0..19]
 * into the 0x08090CD8 ring, so the block is 20 bytes. */
void sub_080308B4(u8 *);

void sub_08012358(void);
/* Unprototyped: only ever called, never with a settled signature. r0 selects
 * among three cases and r1 is forwarded; fill this in when it is matched. */
void sub_0801BB10();
void sub_0801237C(void);
void sub_08012C58(void *); // possibly "SetupBackgrounds"

/* The two halves of the 16-slot callback list on gUnknown_03000000/0808E518:
 * sub_08011B34 inserts, sub_08011B5C removes. Both take the entry as `void *`
 * -- that is how c_08011B34.c already defines it -- so call sites registering
 * a function have to cast. */
void sub_08011B34(void *);
/* A gUnknown_03000000 list entry, registered by ADDRESS through
 * sub_08011B34 by sub_0807420C and never called directly, so `void (void)`
 * comes from the registration; corroborated by its own bytes -- `push {lr}`,
 * `pop {r0}`, and it reads no argument register (wave 28, W28-B). */
void sub_08037F1C(void);
void sub_08011B5C(void *);
/* The same "register an entry" shape on the 16-slot gUnknown_03002FA0 list, and
 * already defined this way in src/decomp/c_08011AAC.c. Its only two callers
 * (sub_0802C260, sub_0802C270) register a FUNCTION by address, so they cast --
 * exactly as the note on sub_08011B34 above describes. */
void sub_08011AAC(void *);
/* Registered by address through sub_08011AAC and never called directly, so
 * `void (void)` comes from the registration and not from a call site. Both end
 * `pop {r0}; bx r0` and neither reads an argument register. */
void sub_080184A4(void);
void sub_080184C8(void);
/* Three more entries named only by ADDRESS through that pair of lists, so the
 * `void (void)` again comes from the registration and not from a call site --
 * but here it is corroborated by a definition rather than only by the callee's
 * own bytes: sub_080111BC (src/decomp/c_080111BC.c), sub_08012A34
 * (src/decomp/c_08012A24.c) and sub_08049BAC (src/decomp/c_08049BAC.c) are all
 * already promoted as void(void). Declared here because the wrappers that
 * register them -- sub_080111AC and sub_08012A74 via sub_08011AAC,
 * sub_08049B70 via sub_08011B34 -- live in other translation units and need
 * the symbol visible to take its address. */
void sub_080111BC(void);
void sub_08012A34(void);
void sub_08049BAC(void);
/* The fourth: removed from the gUnknown_03000000 list by sub_08037780 through
 * sub_08011B5C. Not promoted yet, so `void (void)` is read off its own bytes --
 * `push {lr}` ... `pop {r0}; bx r0` (void, per the epilogue rule in
 * docs/agbcc-codegen.md) and no argument register is read before the first
 * write to it. */
void sub_08037790(void);

/* The gUnknown_0200C528 list pair. sub_080193B0 installs a script: it allocates
 * a slot, stores the script pointer into .unk00 and .unk04 and returns
 * &gUnknown_0200C528[i], or 0 when sub_08019290 reports -1 -- both arms end
 * `pop {r1}; bx r1`, so it returns a value. sub_0801930C removes every slot
 * whose .unk00 equals the argument and always returns -1. The argument is a
 * `const u8 *` because that is how the scripts it is handed (gUnknown_0849A8F0
 * and friends) are declared; nothing dereferences it here. */
struct Unk0200C528 *sub_080193B0(const u8 *);
int sub_0801930C(const u8 *);

/* `bl sub_0801537C` on gUnknown_0848A42C and nothing else. It ends
 * `pop {r1}; bx r1`, so it returns a value, and the value is sub_0801537C's --
 * a gUnknown_03001470 slot index narrowed with `lsls #0x18; asrs #0x18`, or -1
 * when the scan falls off the end. Only the non-void-ness is proved here: every
 * caller found so far (eleven in the 0x08004xxx wrapper family alone) discards
 * the result, so the width is inherited from the forwarded callee and nothing
 * in the tree contradicts or confirms it yet.
 * `int`, not the `s8` it carried until wave 12, and sub_0801537C had to move
 * with it -- the two are one decision, written up in full on sub_0801537C's
 * declaration below. Short version: with an `s8` callee agbcc re-narrows at the
 * `bl` and this function comes out four bytes too long; `int` on both sides
 * matches (work/sub_0801A168/). The one thing the ROM does NOT settle is
 * whether the source said `return` at all -- `s8 f(void) { sub_0801537C(g); }`,
 * with the keyword missing, is byte-identical -- so this is the natural-C
 * reading rather than a proof, and it is byte-neutral for every existing
 * caller either way. */
int sub_0801A168(void);

/* `pop {r0}; bx r0`, so void. The parameter is the address of a record like
 * gUnknown_030013D0 -- see that global's comment for the offsets. `void *` and
 * not a struct pointer: this is the weakest declaration that gives a clean
 * pool word at the two call sites (sub_080251AC passes gUnknown_030013D0 by
 * name, sub_080250E8 passes it and `*gUnknown_08090A40`), and the record's
 * extent is not settled. Not `const void *` -- the body writes +0x04 of the
 * pointer at +0x00. */
void sub_0802505C(void *);
/* `pop {r0}; bx r0`, so void. Four callers, all in family F000 and all passing
 * a POOL CONSTANT (0xC9A/0xC9B/0xC9C/0xC9D), which per docs/agbcc-codegen.md
 * cannot discriminate parameter width at all -- `int`, `u32` and `u16` are the
 * same `ldr r0,=imm` and even `s16` is, for a positive constant that fits. So
 * the declaration is chosen to be the one that is never wrong at a call site.
 * The callee's own prologue is `adds r5, r0, #0` THEN `lsls r5,#0x10;
 * lsrs r5,#0x10`, and the split copy is the reason `u16` is not claimed: a
 * genuine `u16` parameter narrows straight into its home register in one step
 * (compare sub_0803B524's `lsls r0,#0x10; lsrs r4,r0,#0x10`). The narrowed
 * value feeds sub_080146D4's fourth argument, which the doc's "a prologue
 * narrowing is not a parameter-width tell when the narrowed value feeds
 * another call" rule says to read as a cast at a use. Note the raw r0 is dead
 * across the following `bl sub_0802D33C`: that callee takes no arguments (it
 * loads gUnknown_0848A42C into r0 as its first instruction), so the leftover
 * is not evidence of a forwarded argument. */
void sub_0802D35C(int);
/* `int`, not a narrow type: the prologue saves the argument with a bare
 * `adds r6, r0, #0` and it survives four calls, which PROMOTE_MODE would have
 * narrowed for any sub-word parameter. Only ever called as 0 (sub_0802C0CC) or
 * 1 (sub_0802C0D8), and each value picks a different sub_0803B4DC sound id.
 * `pop {r0}; bx r0`, so void. */
void sub_0802BFD0(int);

/* `int`, not `s16` -- the same correction wave 7 made to the eight starters
 * above, and for the same reason. Both open with a bare `adds r4, r0, #0`;
 * declaring the parameter `s16` adds `lsl #16; lsr #16` (PROMOTE_MODE widening
 * a narrow parameter that has to survive the `bl`) and cannot match. Their only
 * call sites are ProcCmd_1D_0801D0E4/ProcCmd_1E_0801D104 in proc.c, which pass
 * `dataImm` -- already sign-extended by its `ldrsh`, so the caller side emits
 * the same code either way. */
void sub_08013098(int, ProcPtr);
void sub_080130B0(int, ProcPtr);
/* Starts gUnknown_0848936C and stashes three halfwords at +0x64/+0x66/+0x68 of
 * the new proc; a NULL fourth argument selects Proc_Start on tree 3 over
 * Proc_StartBlocking. The three values are bare `strh` stores, which fixes them
 * at >= 16 bits and no further. */
void sub_080130DC(int, int, int, ProcPtr);
/* Same shape on gUnknown_084893AC: arg 0 indexes gUnknown_0848950C by words
 * into +0x4c, arg 1 is a halfword at +0x44, arg 2 is the parent-or-NULL. */
void sub_08013338(int, int, ProcPtr);
void sub_08013AEC(void);
/* Wave 33, W33-D. `Proc_Start(gUnknown_08581480, parent)` then three whole-word
 * `str`s of its first three arguments into +0x34/+0x38/+0x3c and a 0 into
 * +0x40. No narrowing on any of the four, so `int` x3 plus the parent. */
void sub_08069FAC(int, int, int, ProcPtr);
/* Wave 33, W33-D. sub_0806A218 calls it immediately after `Proc_Break(proc)`,
 * which has already clobbered r0, so it takes nothing. Return unused. */
void sub_08067820(void);
/* Wave 32 (W32-B): DEFINED in src/decomp/c_080135F4.c and never declared --
 * the fourth of the byte-identical palette wrappers whose signature the
 * ApplyPaletteExt note below settles. Published unchanged. */
void sub_08013664(u16 *, u32, u16);
/* Wave 32 (W32-B): sub_08011300 and sub_08011354 are DEFINED in
 * src/decomp/c_08011300.c and src/decomp/c_08011354.c and were never declared;
 * this wave gave them their first C callers. Published unchanged.
 *
 * sub_08011228 is the HBlank handler sub_08011298 / sub_0801137C install
 * through sub_080111C8's fifth parameter. It reads no argument register before
 * writing it and its epilogue does not preserve r0, so `void (void)`.
 *
 * sub_080111C8 takes FIVE arguments, the fifth on the stack
 * (`ldr r7,[sp,#0x1c]` past a 7-register push): two whole-word addresses that
 * go into the new proc's +0x2c/+0x30, two halfwords into +0x34/+0x36 (each
 * narrowed with `lsls #0x10; lsrs #0x10` at entry, which is PROMOTE_MODE on a
 * declared-narrow parameter), and the handler into +0x38. */
void sub_08011300(void);
void sub_08011354(void);
void sub_08011228(void);
void sub_080111C8(void *, void *, u16, u16, void (*)(void));
/* Wave 32 (W32-B): adds a constant to a run of halfwords -- `*p += delta` while
 * a byte count walks down by 2. The count and the delta are both truncated to
 * 16 bits in the prologue and the loop test is unsigned, and the pointer is
 * `u16 *` because the body is `ldrh`/`strh`. sub_08045358 and sub_080453CC pass
 * a byte count of 0x800 and a delta of 0x82b0. */
void sub_08012B00(u16 *, u16, u16);
/* Wave 33 (W33-A). NOT a derived signature -- src/decomp/c_08012E74.c already
 * DEFINES this as `void sub_08012E74(u16 x, u16 y, u16 c)` and a promoted
 * definition wins over any weakest-type reasoning. It had no declaration
 * anywhere, so every caller was compiling against the implicit `int f()` and
 * skipping the u16 narrowing at the call; sub_08012E9C's third argument is the
 * `lsls #0x10; lsrs #0x10` pair that proves the u16 is real. */
void sub_08012E74(u16, u16, u16);
/* Wave 39 (W39-A). Same situation as sub_08012E74 above and settled the same
 * way: src/decomp/c_08012E9C.c already DEFINES this as
 * `void sub_08012E9C(u16 x, u16 y, u8 c)` and a promoted definition wins. It
 * had no declaration anywhere and, until sub_08012F40, no caller in C either,
 * so nothing was compiling against the implicit `int f()` and adding this
 * changes no existing file. */
void sub_08012E9C(u16, u16, u8);
void sub_08013C00(void);
void sub_08024268(void);
/* Registered by address into the gUnknown_03000000 callback list, never called
 * directly, so `void (void)` comes from the list and not from a call site. */
void sub_080246B4(void);
void sub_08024720(void);

/* The halfword CpuSet wrapper ApplyPaletteExt and sub_080135F4 both forward to:
 * `lsls r2,#0x10; lsrs r2,#0x11; bl CpuSet`, i.e. it halves a u16 byte count
 * into CpuSet's word/halfword count. The THIRD parameter is u16 -- the entry
 * narrowing is PROMOTE_MODE on a declared-narrow parameter, and it is also
 * what would make a caller with a WIDER third parameter emit `lsls #0x10;
 * lsrs #0x10` in front of the call -- see the ApplyPaletteExt note below, which
 * is exactly that case read backwards (wave 20, W20-C). */
void sub_08011C58(const void *, void *, u16);
/* Both `void (void)`. sub_0801F00C is matched and is three instructions --
 * `gUnknown_03001FE0 = 1` and `bx lr`, no argument register read. sub_08036B4C
 * opens `push {r4,lr}; ldr r0,=gUnknown_030040A0; movs r4,#0` and never reads
 * r0-r3; it ends `pop {r0}`. Named by sub_08036C4C / sub_08036C80 (wave 20,
 * W20-C). */
void sub_0801F00C(void);
void sub_08036B4C(void);
void sub_080135A4(void);
/* THE THIRD PARAMETER IS u16, NOT u32 (wave 20, W20-C). ApplyPaletteExt is one
 * of a family of four byte-identical palette wrappers -- sub_080135F4,
 * sub_08013640, sub_08013664 and this one -- and the other three are now
 * MATCHED, all with `u16`. The readout: the entry `lsls r2,#0x10; lsrs
 * r2,#0x10` is PROMOTE_MODE on a declared-narrow parameter and therefore sits
 * at the TOP of the function, ahead of the gPal address arithmetic. Declared
 * `u32`, the same two instructions still appear -- they are the conversion to
 * sub_08011C58's u16 third parameter -- but they land AFTER the address work,
 * at the call, which is one instruction pair in the wrong place and the only
 * difference between the two spellings. Every call site in the tree passes a
 * constant, so narrowing costs nothing at any of them; the 12 promoted callers
 * were re-verified after the change. */
void ApplyPaletteExt(u16 *, u32, u16);
void sub_080136C4(void);
/* Wave 40 (W40-B): the palette-fade setter family at 0x08013. The delta lands
 * in gUnknown_0200B5F4, which is s8. The prologue `lsls #0x18; lsrs #0x18` here
 * fixes the WIDTH only, not the sign: it is PROMOTE_MODE on a declared-narrow
 * parameter, and a controlled probe confirms `s8` and `u8` bodies emit that
 * identical zero-extending pair. The SIGN is settled at the callers, all of
 * which pass a constant: `f_s8(-1)` and `f_int(-1)` both emit
 * `movs r0,#1; negs r0,r0`, whereas `f_u8(-1)` emits `movs r0,#0xff`. So
 * narrowing to `s8` is free at every existing call site -- including
 * src/title-screen.c, which is upstream source and cannot be edited -- while
 * narrowing to `u8` would have changed it. */
void sub_080137AC(s8);
/* Its own parameter stays wide: the `lsls #0x18; asrs #0x18` in sub_080139C4 is
 * the narrowing to sub_0801394C's s8 SECOND parameter at the call, not a
 * prologue truncation -- the `lsls` half is hoisted out of the loop and only the
 * `asrs` repeats per call, which a prologue truncation could not do. */
void sub_080139C4(s32);
/* Wave 40 (W40-B): the single-row forms of the two row-loop setters above, and
 * the per-frame applier. sub_080138B0 had no declaration at all. Its FIRST
 * parameter is genuinely `u8` -- sub_080139C4 narrows its `int` loop counter
 * with `lsls #0x18; lsrs #0x18` at the call to the identically-shaped
 * sub_0801394C, which a declared `int` parameter would not emit -- and the
 * second is `s8` for the reason given at sub_080137AC.
 * CORROBORATED CALLER-SIDE, independently, by W40-C in the same wave: this
 * signature was derived here from the identically-shaped twin rather than from
 * sub_080138B0's own caller, and W40-C then matched that caller, sub_08013928,
 * against it -- the call site zero-extends r0 (`lsrs r0, r4, #0x18`) and
 * sign-extends r1 (`asrs r1, r5, #0x18`), which is exactly (u8, s8) and is the
 * discriminating use the derivation above lacked. Both parameters are now
 * settled from two directions; do not widen either. */
void sub_080138B0(u8, s8);
void sub_080139E0(void);

/* SUSPECT DECLARATION -- the first parameter is almost certainly
 * `const void *`, not `s32`, and this is left alone only because changing it
 * needs an edit to src/proc.c that cannot be verified without a full build.
 * The evidence, gathered in wave 12:
 *   - the body is the structural twin of sub_080152EC below (allocate a
 *     gUnknown_03001470 slot with sub_08015BD0, then forward the UNTOUCHED r0
 *     to sub_08015224 / sub_0801527C), and sub_080152EC's first parameter is
 *     settled as `const void *`;
 *   - every one of its six wrapper call sites (sub_0802A514, sub_0802A538,
 *     sub_0802A7B0, sub_080364E0, sub_0803B240, sub_08049B14) passes a ROM
 *     script-blob SYMBOL, declared `const u8 []`;
 *   - two of those blobs (0849A108, 0849E6D4) are stopped by
 *     sub_0801537C(const void *) on the same symbol, so the two ends of the
 *     pair disagree about the type of one object;
 *   - the only caller that is not a wrapper, proc.c's ProcCmd_19_0801D214,
 *     reaches the argument as `(s32)p->proc_scrUnk->dataPtr` -- a pointer the
 *     source had to cast to satisfy this declaration.
 * The cost of leaving it is one `(s32)` cast in each of the six wrappers; the
 * cast is byte-neutral (`ldr r0,=gSym` either way), so nothing is blocked.
 * To fix it: change the parameter here, drop the `(s32)` in proc.c and in the
 * six wrappers, then `make SPLIT=1 compare` -- proc.c is a multi-function file
 * so per-function trymatch is not a sufficient check.
 *
 * The RETURN type is wrong too, and independently of the above: the function
 * ends `adds r0, r4, #0; pop {r4, r5, r6}; pop {r1}; bx r1`, and `pop {r1}` is
 * the returns-a-value epilogue (docs/agbcc-codegen.md). The value is r4, i.e.
 * sub_08015BD0's slot index kept as `lsls #0x18; asrs #0x18`, so `s8` -- the
 * same readout that types sub_08015BD0 and sub_0801537C. This half is
 * byte-neutral for every caller in the tree, because all of them (proc.c and
 * the six wrappers alike) discard the result and a discarded return emits no
 * narrowing.
 *
 * FIXED IN WAVE 25, and the note above was wrong about needing to wait for the
 * parameter. `s32` is not a problem to solve: every one of the eight call
 * sites already passes a pointer through an explicit `(s32)` cast, which is
 * byte-neutral, and the body casts it back with `(const void *)`. So the
 * return type was the only thing blocking sub_080152C0, and it can be fixed
 * ALONE. Verified by reading all eight call sites -- seven in src/decomp plus
 * src/proc.c:1079 -- every one a bare expression statement discarding the
 * result. src/proc.c is NOT edited by this change; only this line moves. */
s8 sub_080152C0(s32, u8);
/* Allocates a gUnknown_03001470 slot and returns &gUnknown_03001470[i], or NULL
 * when sub_08015BD0 reports -1. Second parameter is `u8`: the prologue is
 * `lsls r1,#0x18; lsrs r1,#0x18`, which the table in docs/agbcc-codegen.md
 * gives as a narrow parameter widened for a call. Callers store a u16 through
 * the result at +0x20 (sub_08035144, sub_08042B84). 86 callers. */
struct Unk03001470 *sub_080152EC(const void *, u8);
/* The remover for the same list. It walks all 0x1e slots of
 * gUnknown_03001470 looking for the one whose .unk00 word equals the argument,
 * calls sub_08015328 on that index and returns it as `lsls #0x18; asrs #0x18`,
 * or -1 off the end of the scan -- so `s8`, exactly the readout that already
 * types its forwarder sub_0801A168 above (which is `bl sub_0801537C` and
 * nothing else). Parameter is `const void *` for the same reason
 * sub_080152EC's is: the argument is always a ROM script blob, and the pairing
 * proves it -- 0849A108, 0849D41C, 0849D55C, 0849E6D4 and 084C2198 each have a
 * sub_080152C0/sub_080152EC starter and a sub_0801537C stopper naming the same
 * symbol (see the gUnknown_0849A108 comment in unknown-globals.h). Nothing
 * here dereferences it.
 *
 * THE RETURN WIDTH IS NOT CONSTRAINED BY ANYTHING IN THE ROM, and the line
 * above used to say only "not constrained by the nine F000 wrappers -- all
 * nine discard the result". Wave 12 checked the one caller that does NOT
 * discard it, sub_0801A168 (`push {lr}; ldr r0,=0848A42C; bl sub_0801537C;
 * pop {r1}; bx r1`, sixteen bytes with no shift in it), expecting that to
 * settle the question. It does not, and the reason is worth knowing:
 *
 *   - agbcc RE-NARROWS a narrow-returning callee's result AT THE CALL SITE.
 *     With `s8` here, `return sub_0801537C(g);` emits `lsl #0x18; asr #0x18`
 *     after the `bl` whatever the caller's own return type is -- probed with
 *     `int` and `s8` returns and with an `s8` local in between, all three.
 *     So the natural `return <call>;` spelling needs `int` here, and `int` on
 *     sub_0801A168 as well.
 *   - BUT `s8 sub_0801A168(void)` with the `return` keyword simply ABSENT is
 *     byte-identical: the callee's result is already in r0, so falling off the
 *     end of a non-void function emits `pop {r1}; bx r1` and nothing else --
 *     and agbcc does not warn about it even under -Werror.
 *
 * Two source spellings, one byte sequence, opposite conclusions about this
 * declaration. Wave 12 took the `int` side and retyped both, because it is the
 * only spelling that is valid, warning-free, non-UB C for a function whose
 * whole body forwards a value -- not because the ROM decides. The change is
 * byte-neutral everywhere else: every other caller of sub_0801537C in the ROM
 * discards the result, no file in src/ calls it at all, and its own body
 * matches under either declaration (the `lsls #0x18; asrs #0x18` on the index
 * is `return (s8)i;` under `int` just as it is `return i;` under `s8`).
 * Verified by recompiling all 20 promoted src/ files that name any touched
 * symbol -- zero failures.
 * If you are matching sub_0801537C itself and this line fights you, the
 * counter-evidence you need is a caller that NARROWS its result; there is none
 * in the ROM today, and finding one flips both lines back. */
int sub_0801537C(const void *);
s8 sub_08015BD0(s32);
/* Wave 46 (W46-F): the `s32` parameter is almost certainly wrong and is left
 * alone deliberately -- retyping a shared prototype mid-wave is how builds
 * break. Every call site seen so far passes a ROM SCRIPT ADDRESS, not a
 * number: sub_080654E8 hands it gUnknown_08580AF0 / _08580B90 / _08580BC8 and
 * then hands the same three to sub_0806377C, which is declared
 * `const void *` for exactly this reason. `const void *` would be the honest
 * type and is byte-neutral (the argument is a pointer in a register either
 * way), but until then callers need an `(s32)` cast that buys nothing.
 * The RETURN is not in doubt -- `s8` is what the `lsls #24; asrs #24` at every
 * call site reads back, and it is compared against -1. */
/* Matched and promoted in src/decomp/c_080366DC.c -- it is `return
 * gUnknown_030040EC;` and that global is a `void (*)(void)`. Declared here in
 * wave 33 (W33-D) for sub_08014BE8, whose last test compares the result
 * against sub_080369BC. Only that symbol's ADDRESS is used anywhere, so
 * `void(void)` is what the comparison needs and nothing else constrains it. */
void (*sub_080366DC(void))(void);
void sub_080369BC(void);
/* Three whole-list sweeps over gUnknown_0200E438, each reached only through a
 * bare `push {lr}; bl <it>; pop {r0}` forwarder (sub_08015544, sub_08015550,
 * sub_0801555C). None of them reads r0-r3 and all three end `pop {r0}; bx r0`,
 * so all three are void(void) -- which is also what makes their forwarders
 * argument-free. */
void sub_0801D8B4(void);
void sub_0801DED8(void);
void sub_0801DF20(void);
/* Wave 50, W50-F: the other two sweeps of the same shape. Both open
 * `push {r4,r5,r6,lr}; movs r4,#0` -- neither reads an argument register -- and
 * both are called bare from sub_08052EA8's main loop. Same void(void). */
void sub_0801D8E4(void);
void sub_0801D924(void);
/* Wave 40, W40-I: the per-slot workers those sweeps drive. All four sweeps
 * (sub_0801D8E4, sub_0801D924, sub_0801DED8, sub_0801DF20) call them as
 * `f(i, 0)` or `f(i, 1)` where i is the plain int loop counter -- no narrowing
 * on either argument at any of the six call sites -- and none of them reads r0
 * after the `bl`, so `void (int, int)` is everything the call sites prove.
 * Both bodies are still unmatched assembly, so this is a floor, not a reading
 * of their prologues. */
void sub_0801D390(int, int);
void sub_0801DCD4(int, int);
/* Wave 40, W40-I: both are already DEFINED in src/decomp (c_0801DB04.c and
 * c_0801DC50.c) and were simply never declared, because until now every caller
 * was still assembly. The signatures are copied from those definitions, not
 * inferred. */
void sub_0801DB04(s16);
int sub_0801DC50(s16, u32 *, s16, int);
/* The heap pair on gUnknown_03000050. sub_08014E44 returns the block (its
 * epilogue is `pop {r1}; bx r1`) or NULL when no arena is installed;
 * sub_08014ED4 releases one and is `pop {r0}; bx r0`, i.e. void. */
void *sub_08014E44(int);
void sub_08014ED4(void *);

/* A pair always called together on the same object, each `(ptr, u8 flag)` and
 * each returning an accumulated total in r0.
 *
 * WAVE 38 (W38-J): the object IS modelled -- it is `struct UnitRecord`, and
 * the note that used to stand here ("bytes at +0, +4, +6, a 4-bit and a 7-bit
 * field ... not modelled anywhere yet") was in fact a field-for-field
 * description of it. The three offsets it lists are unk00, the unk04 bitfield
 * container and unk06_0, and the two field widths are unk04_7:4 and unk06_0:7
 * exactly as sub_08025B80 and sub_08042998 established them. unk00 clinches it:
 * both functions multiply it by 0x5c to subscript gUnknown_085D5ABC, which is
 * the defining use of that member. Retyped from `void *`; sub_08029D1C and
 * sub_08044518 forward a `void *` and convert implicitly, so the change is free
 * at every existing caller (c_08029D1C.c re-verified byte-exact after it). */
int sub_08029978(struct UnitRecord *, u8);
int sub_08029A48(struct UnitRecord *, u8);
/* Both parameters are `s16`: the prologue sign-extends r0 and r1 with
 * `lsls #16; asrs #16` before forwarding to sub_080290B0. */
void sub_08029088(s16, s16);

void sub_0801BD00(s32, s32, void *, s32);
/* Wave 40 (W40-H). sub_0801BF2C's other arm, and it takes the SAME four
 * arguments as sub_0801BD00 above -- both calls in that function build an
 * identical argument list off one SpriteEntry and differ only in the callee.
 * (The second argument is NOT the 0x1000 flag mask it looks like at a glance:
 * r2 holds 0x1000 for the flag test, but `ldrsh r2, [r4, r0]` reloads it with
 * the entry's oam0 before `adds r1, r2, #0` builds the argument.) Same weakest
 * `void *` third parameter for the same reason. */
void sub_0801C090(s32, s32, void *, s32);
/* Wave 34 (W34-H). sub_0801306C is an on-screen/visibility predicate:
 * sub_08039DBC gates its whole sub_0801BD00 call on it and tests the result
 * with a bare `lsls #0x18; cmp #0`, a low-BYTE truth test, which is what makes
 * the return `u8` rather than int. Its first two arguments are the same
 * camera-relative x and y sub_0801BD00 gets beside it, but WITHOUT the +8 and
 * the 0x1ff/0xff wrap masks -- the raw signed difference.
 *
 * sub_0801DAB0 returns `int`: sub_08039BDC stores the result straight into an
 * s16 field with `strh` and no narrowing in between, which an s16-returning
 * callee would have carried (agbcc re-narrows those at every call site). */
int sub_0801306C(int, int, int);
/* Wave 35, W35-I: the parameter list is now EMPTY, and that is a measured
 * correction rather than a weakening. sub_0801DAB0's own body in
 * asm/code-0801D390.s never reads r0 -- it opens `movs r1,#0x1f` and scans
 * gUnknown_03001430[0x1f..0] for a free affine-matrix slot -- and its two call
 * sites disagree about whether an argument is passed at all: c_08039BDC
 * materialises `proc` into r0, while sub_08016824 leaves r0 holding its own
 * incoming parameter untouched, which no spelling of `f(a)` can produce for a
 * declared-narrow parameter. The original header must have left it
 * unprototyped. `()` is the only declaration both call sites compile against;
 * the `ProcPtr` is kept in the comment because c_08039BDC's argument is real.
 * Re-verified with trymatch on c_08039BDC after the change. */
int sub_0801DAB0(/* ProcPtr */);
/* Only referenced as a value, by sub_0801F4A4 storing it in gUnknown_030013EC.
 * Its assembly takes five arguments (r0-r3 plus one halfword on the stack), but
 * nothing calls it, so the argument list is left unprototyped rather than
 * guessed -- fill it in when sub_0801F4B4 itself is matched.
 */
/* wave 49: filled in from the definition, which takes FIVE arguments -- r0, r1,
 * r2, r3 and one stacked at [sp,#0x24]. All five are `int`: no prologue
 * narrowing on r0-r3, and the stacked one is read `mov r2,sp; ldrh r2,[r2,#0x24]`
 * only because it is stored into the s16 gUnknown_084999C8->unk22, which
 * truncates the SImode stack slot to a HImode mem that THUMB cannot address off
 * sp. This agrees with the already-declared type of gUnknown_030013EC, the
 * function pointer src/decomp/c_0801F49C.c assigns it to. */
void sub_0801F4B4(int, int, int, int, int);
void PutSprite(u32, u32, u32, u16 *, u32);
void PutSpriteExt(u32, u32, u32, u16 *, u32);

void SetObjAffine(s32 index, s16 pa, s16 pb, s16 pc, s16 pd);
/* One of the four byte-identical palette wrappers docs/agbcc-codegen.md's
 * duplicate-digest section records alongside ApplyPaletteExt itself, so it
 * carries that function's signature. Declared in wave 33 (W33-D) for
 * sub_08031018, which calls it four times. */
void sub_080135F4(u16 *, u32, u16);

void sub_08030ED4(void);

/* The gUnknown_085D3DD0 lookup family. Every one of them is
 * `if (!gUnknown_03003FC0.unk08) return <fallback>; return <table entry>;` and
 * each has a one-line forwarder next to it that supplies (unk1d, unk1e) out of
 * gUnknown_08499598. sub_08042F34 and sub_08042F7C IGNORE their second
 * argument -- it is still declared, because their forwarders load and pass it.
 */
int sub_08042DCC(int);
/* Wave 38 (W38-J). Copied verbatim from the PROMOTED definition in
 * src/decomp/c_0804209C.c, which had no declaration here at all -- the only
 * caller in C so far, sub_08029D3C, would not compile without it. The `bool8`
 * return is what puts the `lsls #0x18; lsrs #0x18` re-narrowing at that call
 * site, and the `s16` parameters are what narrow the two coordinate sums
 * there, so the call site corroborates the definition rather than merely
 * agreeing with it. */
bool8 sub_0804209C(s16 x, s16 y);
/* `int` and not `u8`, even though its body is a bare `ldrb`. The conclusion
 * stands but the evidence recorded here and in src/decomp/c_08042E18.c was the
 * WRONG CALLER, corrected in wave 12: sub_08042DFC is a bare
 * `bl sub_08042E18; pop {r1}; bx r1` tail forwarder, and for those a missing
 * `return` keyword is byte-identical to `return <call>;` with an int-wide
 * callee, so it proves nothing (see the forwarding carve-out in
 * docs/agbcc-codegen.md). What actually pins it is the OTHER caller,
 * sub_0807F630: `bl sub_08042E18; adds r1, r0, #0; bl sub_08026AC0` re-passes
 * the result as an argument with no narrowing, and a `u8` return would have put
 * `lsls #24; lsrs #24` in that gap. */
int sub_08042E18(int);
int sub_08042E2C(int, int);
int sub_08042E84(int, int);
int sub_08042EDC(int, int);
int sub_08042F34(int, int);
int sub_08042F7C(int, int);
int sub_08042FC4(int, int);
u32 sub_0804301C(int, int);

/* Parameter is `u8` -- prologue narrows r0 with `lsls #24; lsrs #24` before the
 * value survives a call. Ends `pop {r0}; bx r0`, so void. */
void sub_08035144(u8);

void sub_08039F58(void);
/* Wave 33, W33-D. A (column, row, value) cell write: both coordinates are
 * compared against 0 with a SIGNED `blt` and against gUnknown_08499590's
 * halfword extents with `bge`, and neither is narrowed in the prologue, so
 * both are `int`. sub_08044994 calls it thirteen times with the column and row
 * offset by up to +/-2, which is what rules out a narrow type. */
void sub_08044854(int, int, int);

/* Reads bit `id` of one of three bit arrays chosen by range. `int`, not a
 * narrow type: sub_080485C4 tests the result with a bare `cmp r0, #0` and no
 * `lsls #24`, which a bool8 return would have forced. */
int sub_0803CBD8(int);
/* CORRECTION, wave 32 (W32-A). Both were promoted as `u8 (int)` -- one-line
 * bodies returning gUnknown_08499598[i].unk3a / .unk3b, which are u8 members --
 * and neither had a declaration or a C caller until now. sub_080264BC is the
 * first, and it takes BOTH results with a bare `adds r4, r0, #0` /
 * `adds r1, r0, #0` and no re-narrowing before comparing them against each
 * other. agbcc re-narrows a narrow-returning callee at every call site, so the
 * absence of the shift pair is the return width: `int`, not `u8`. Byte-neutral
 * in the two definitions (an `ldrb` returned as int is the same `ldrb`), both
 * re-verified. This is the wave-26 readout -- caller evidence is the only
 * evidence there is for a return width, and the bodies were a guess. */
int sub_08025CF0(int);
int sub_08025D08(int);
/* Wave 41, W41-D. Copied VERBATIM from the promoted definition in
 * src/decomp/c_080260D0.c -- the definition wins. Declared now because
 * sub_08026040 is its only caller and had no prototype for it. */
void sub_080260D0(struct UnitRecord *, int);
/* Wave 41, W41-D. sub_08026520's five callees, all of them already promoted
 * and none of them previously declared. Every signature below is COPIED FROM
 * THE DEFINITION (src/decomp/c_080263A4.c, c_080264BC.c, c_08026340.c,
 * c_08037D80.c) -- the definitions win over any call-site reading. The one
 * call site agrees: the three `u8` results go straight into `strb`s and the
 * `u32` one into a `strh`, none of them re-narrowed. */
u8 sub_080263A4(u8);
u8 sub_080264BC(u8);
u32 sub_08026368(u8);
int sub_08037D80(int);
/* Wave 41, W41-D. Matched this wave; both declarations are taken from the new
 * definitions rather than from a call site.
 *
 * sub_08026254 returns the picked entry of the 0xff-terminated
 * gUnknown_020288A0 list with a bare `ldrb`, so u8; it is void because nothing
 * is read out of an argument register.
 *
 * sub_08026424's parameter is u8 from the prologue's `lsls #0x18; lsrs #0x18`
 * and its return is u8 from the same shift pair on the way out. sub_08026520
 * stores the result with `strb` and does not re-narrow, which agrees. */
u8 sub_08026254(void);
u8 sub_08026424(u8);
/* Wave 41, W41-D. Copied VERBATIM from the promoted definition in
 * src/decomp/c_080261E8.c, whose own comment records that the `int` parameter
 * is a bare `adds r2, r0, #0` with no PROMOTE_MODE narrowing. Declared now
 * because sub_0802D67C is its first cross-file caller; it passes the u16
 * gUnknown_030033EC, which widens at the call site and costs nothing. */
bool8 sub_080261E8(int);

/* The gUnknown_02028030 single-bit readers -- `return (1 << (id & 7)) & base[id >> 3];`
 * one per byte range of the struct (unk10, unk2a, unk2d ...). Their return type
 * is EIGHT BITS WIDE, not `int`: every caller in the 0x0803C354-0x0803C670
 * predicate block narrows the result with `lsls r0, r0, #0x18` before testing
 * it, and an `int` return emits no such shift. This is the opposite reading
 * from sub_0803CBD8 four lines up, whose callers test with a bare `cmp r0, #0`
 * -- the two live side by side in the same functions (sub_0803C48C calls
 * sub_0803CAB8 with the shift and sub_0803CBD8 without), which is as direct a
 * control as this file has.
 * `u8` and not `bool8`, because the value returned is the mask bit itself
 * (0 or 1 << n), never normalised to 0/1. The declared width is all that
 * matters to codegen; the promoted definitions in src/decomp/c_0803CA9C.c and
 * c_0803CAD4.c were re-typed to `u8` and still match byte-for-byte, because the
 * `ldrb`-derived operand already has nonzero_bits <= 0xff so the narrowing is a
 * no-op. sub_0803CAF0/CB0C/CB24/CB74 are the same family and the same rule
 * applies to them; they are left out here only because nothing declared needs
 * them yet.
 *
 * Wave 44 (W44-F) found the ONE dissenting caller and did NOT change this
 * declaration. sub_0807F57C tests sub_0803CAB8's result with a bare
 * `cmp r0, #0` and no `lsls #0x18` -- it is byte-identical for all 156 bytes
 * apart from that one halfword, and four spellings were probed without
 * removing it (including `u8 t = f(x); if (t)`, which does not absorb a
 * callee's return narrowing). Ten callers against one says the original source
 * declared this function `int` in sub_0807F57C's translation unit and `u8`
 * everywhere else, which one shared header cannot express. Retyping to `int`
 * plus an explicit `(u8)` at the ten narrowing sites would match all eleven;
 * that is a fan-in-11 reshape and is left as an orchestrator decision. See
 * docs/agbcc-codegen.md. */
u8 sub_0803CA9C(u32);
u8 sub_0803CAB8(u32);
u8 sub_0803CAD4(u32);
/* Same family, same `lsls #24` evidence: sub_0803C658 narrows its result before
 * testing it. The definition in src/decomp/c_0803CAF0.c was re-typed from `int`
 * to `u8` when this was added and still matches byte-for-byte, exactly as the
 * paragraph above predicts. sub_0803CB0C in the same file is left `int` only
 * because nothing declared needs it yet. */
u8 sub_0803CAF0(u32);
/* Wave 44 (W44-C). The paragraph above named sub_0803CB24 as "left out only
 * because nothing declared needs it yet"; sub_08084938 needs it. The evidence
 * is the same `lsls r0, r0, #0x18` bare truth test at both of sub_08084938's
 * call sites, which an `int` return does not produce. Definition in
 * src/decomp/c_0803CB24.c re-typed from `int` to `u8` and re-verified
 * byte-for-byte, exactly as predicted. */
u8 sub_0803CB24(u32);
/* Same family, but reached through sub_080206B0 rather than indexing directly;
 * still unmatched. The argument is passed straight through to sub_080206B0 --
 * sub_0803CA70 opens `adds r3, r0, #0` (a dead save of the incoming parameter)
 * -- and the callers in the predicate block forward their own first parameter
 * without touching r0, so the width is unconstrained; `u32` matches the rest of
 * the family. The `u8` return is pinned by the same `lsls #24` evidence. */
u8 sub_0803CA70(u32);
/* Three one-line readers of gUnknown_03003F30's bytes +1, +3 and +5, already
 * promoted as `u8` in src/decomp/c_0803BC7C.c and declared here for the first
 * time in wave 19 because the 0x08083 sprite builders need them. The `u8` is
 * corroborated on the CALLER side by the same evidence as the family above:
 * sub_080831FC / sub_08083484 / sub_08083738 each narrow the result with a bare
 * `lsls r0, r0, #0x18` before `cmp r0, #0`, which an `int` return would not
 * emit. Declared to match the promoted definitions, not against them. */
/* Two more of the 0x08079xxx proc helpers, both void and both taking TWO
 * arguments with the FIRST unused in the body: sub_080795A8 opens
 * `adds r4, r1, #0` and then writes r0 before reading it, so the proc pointer
 * its caller leaves sitting in r0 costs nothing and is not evidence of arity --
 * the `movs r1, #0` is. sub_08079EA4 is the caller for both.
 *
 * sub_08079B38 IS NOT A REAL SYMBOL YET, and nothing may be promoted against it
 * until it is. 0x08079B38 is a genuine function entry that tools/split_asm.py
 * merged into sub_08079B04 -- see "bl to a mid-function local label is NOT a
 * tell" in docs/agbcc-codegen.md -- so `asm/` defines it only as the LOCAL label
 * `_08079B38` and no global symbol exists at that address in either build. A C
 * caller therefore cannot link against it. Declared here so the blocker is
 * recorded next to the thing that trips over it; see data/parked.json for
 * sub_08079EA4. */
/* sub_08072B54 copies BOTH arguments to callee-saved registers and then narrows
 * the first with `lsls #16; asrs #16` before handing it to sub_0803B4DC -- which
 * is copy-then-narrow, i.e. an `int` parameter with a cast at a use, not an s16
 * parameter. The second is forwarded unnarrowed. sub_08075AC4 likewise takes two
 * word arguments; its only caller passes an `ldrsh` member and a literal, which
 * constrains neither width, so both stay `int`. */
void sub_08072B54(int, int);

/* A one-line palette poke: `(x & 0x1f) / 2` indexes the u16 ROM table
 * gUnknown_081D1624, the entry goes into gPal + 0xb2, and sub_080135A4 flushes
 * it -- so the argument is a 5-bit animation phase and the halving is why the
 * table has sixteen entries for thirty-two phases. `int` because nothing is
 * narrowed at entry and its only caller, sub_08075368, passes the `int` proc
 * field +0x40 and the literal 0x18; a narrow declaration would be
 * indistinguishable at the callee (PROMOTE_MODE) and would make that caller
 * re-narrow. `pop {r0}; bx r0` -> void. */
void sub_08075340(int);
void sub_08075AC4(int, int);
/* Wave 51 (W51-L): SECOND PARAMETER RETYPED int -> u32, and the body is now
 * matched against it. The two guards are `cmp r4, #0x54; bhi` and `cmp r4,
 * #0x1c; bhi` -- UNSIGNED orderings, which an `int` parameter cannot produce
 * (it compiles to `bgt` and was the whole 8-byte residual). `u16` was ruled out
 * by probe rather than by preference: it also yields `bhi`, but PROMOTE_MODE
 * makes agbcc re-narrow it with `lsl r1,#16; lsr r4,r1,#16` at entry, where the
 * ROM opens `adds r4, r1, #0` with no narrowing at all.
 *   The remaining ambiguity -- `u32` versus `int` with a `(u32)` cast at each of
 * the two guards -- is byte-neutral and has NO oracle, and all four call sites
 * were checked. sub_080794E8, sub_08079FAC and the parked sub_08079EA4 pass the
 * literal 0 via `movs r1, #0`, which constrains nothing. sub_0807A0C4 is the
 * only one passing a value: an `int` local holding an Interpolate result that
 * is ALREADY in r1, so the ROM spends no instruction on it -- and an int-to-u32
 * argument conversion is itself free, so that call site does not discriminate
 * either. `u32` is chosen because it is one declaration against two redundant
 * casts; every call site is byte-identical under both. */
void sub_080795A8(ProcPtr, u32);
void sub_08079B38(ProcPtr, int);
/* Wave 37, W37-Q2: NOW DECLARED, copied verbatim from the definition in
 * src/decomp/c_0803BD14.c, which had been promoted with no declaration
 * anywhere. Its caller sub_0803BCDC keeps the result in a register and
 * compares it signed with no narrowing, which is the evidence for `int`. */
int sub_0803BD14(void);
u8 sub_0803BC7C(void);
u8 sub_0803BC88(void);
u8 sub_0803BC94(void);
int sub_08044374(int);

bool8 sub_0803B18C(void);
/* `int`, not the `s16` these carried until wave 11. Both prologues narrow r0
 * (`lsl #16; asr #16` in sub_0803B4DC, `lsl #16; lsr #16` in sub_0803B524),
 * which reads as a narrow parameter -- but those are conversions at the USE
 * site inside the body (sub_0803B4DC forwards to sub_0803B48C, sub_0803B524
 * needs both `(u16)a` and `(s16)a`), not PROMOTE_MODE on the parameter. The
 * discriminator is on the caller side: sub_08016104 and sub_08016130 pass a
 * u16 read out of a script stream with a bare `ldrh r0, [r0, #4]`, and an
 * `s16` parameter folds that whole thing into `movs r1,#4; ldrsh r0,[r0,r1]`
 * -- 2 bytes longer and impossible to reach any other way.
 * Safe for the existing call sites, verified by probe: proc.c's
 * ProcCmd_1B/ProcCmd_1C pass an `s16` dataImm, and `int` and `s16` compile
 * byte-identically there (both `ldrsh`); `u16` does NOT (it gives `ldrh`) and
 * would have broken proc.c, so `u16` is ruled out even though sub_0803B524's
 * own prologue looks like it. Every other caller passes a literal. */
void sub_0803B4DC(int);
void sub_0803B524(int);
/* Wave 34 (W34-H), the 0x08039xxx block. sub_08039930 and sub_080399F8 are
 * TRANSCRIBED FROM THEIR PROMOTED DEFINITIONS (src/decomp/c_08039930.c and
 * src/decomp/c_080399F8.c), not inferred -- the definition wins over any
 * caller-side guess, and neither had a declaration here before, which is why
 * every caller compiled on an implicit `int ()`.
 *
 * sub_080168BC takes `int` for the same reason sub_08016824 next to it does:
 * sub_08039338 and sub_080393AC both reach it with a bare `ldrsh` off the s16
 * gUnknown_03001FBC, which is how an s16 global feeds an `int` parameter.
 *
 * sub_08039A58 is sub_080399F8's sibling -- sub_08039948 calls the two from
 * different arms of one switch with the identical (0x2b0, 8) argument pair.
 * CORRECTED, wave 38 (W38-F): sub_08039544 was declared `void (void)` here
 * because its one caller sub_080396F4 "sets up no argument register at all".
 * That reading is wrong and the callee refutes it -- sub_08039544 opens
 * `adds r4, r0, #0` and immediately does `ldrb r0, [r4]`, so r0 IS a
 * parameter. The caller sets nothing up because the argument is already in
 * r0: the pair is `sub_08039544(sub_08039F18(proc->unk54))`, sequential `bl`s
 * carrying a nested call. That also corrects the claim below that
 * sub_08039F18's result is discarded at that site -- it is consumed here.
 *
 * src/decomp/c_080396F4.c was updated to spell the nest and re-verified with
 * try_match (still byte-identical).
 *
 * sub_08039F18 is `u8 *`, TRANSCRIBED from src/decomp/c_08039F18.c and not
 * inferred: its one readable call site (sub_080396F4) discards the result, so
 * the call site alone would have said `void`, which is byte-identical there and
 * wrong. Same trap as sub_0801306C above, whose promoted definition returns
 * `int` while its sub_08039DBC call site narrows to a byte -- that narrowing is
 * a `(u8)` cast at the use, not the return type. Where a callee already has a
 * promoted definition, read the definition; tools/proto_check.py is what
 * catches this, because per-function try_match compiles one unit and cannot. */
void sub_080168BC(int);
/* Wave 38 (W38-F). TRANSCRIBED from the promoted definition in
 * src/decomp/c_080393CC.c, which had no declaration here -- sub_080394B4 is
 * its first C caller and reaches both arguments with a bare `ldrsh` off an
 * s16 member, which is how an s16 object feeds an `int` parameter. */
/* Wave 38 (W38-F), the 0x08039xxx overworld-marker trio. All three are read
 * off sub_08039188's call sites.
 *
 * sub_08039140's RETURN IS `u8`, and that is settled from its only caller, not
 * from its body: sub_08039188 does `lsls r0,#0x18; cmp r0,#0; beq` on the
 * result, which is agbcc re-narrowing a u8-returning callee (see sub_08039850
 * and src/decomp/c_080397BC.c for the identical tell). An `int` return gives a
 * bare `cmp r0,#0` there. src/decomp/c_08039140.c was retyped from `int` to
 * match and re-verified with try_match -- its body returns literal 0/1, so the
 * change is byte-neutral there and only the caller discriminates. Parameters
 * are TRANSCRIBED from that definition.
 *
 * sub_08039064 and sub_080390CC take `u8`: sub_08039188 holds its `s8` loop
 * counter in the shifted form `i << 24` and reaches both with
 * `lsrs r5,r5,#0x18` -- a TRUNCATION of the s8 to 8 unsigned bits, which is
 * what converting `s8` to a `u8` parameter costs and which an `int` or `s8`
 * parameter could not produce (either would need `asrs`). Both RETURN `u8` on
 * the same re-narrowing tell: each result is re-narrowed with `lsls #0x18`
 * before it indexes gUnknown_0849D5C4, and sub_080390CC's is copied to a
 * callee-saved register FIRST and narrowed later, at its use. */
u8 sub_08039064(u8);
u8 sub_080390CC(u8);
/* Wave 43, W43-I. LEAVE BOTH OF THE FIRST TWO PARAMETERS ALONE. This
 * declaration is right for the DEFINITION and wrong for its only caller, and
 * that is a property of the original source, not of the repo.
 *
 * sub_08039140's prologue narrows all four parameters -- `lsls r0,#0x10;
 * lsrs r4,r0,#0x10` (u16), `lsls r1,#0x10; asrs r1,#0x10` (s16), and two
 * `lsls #0x18; lsrs #0x18` pairs (u8, u8). Drop the `u16` and c_08039140.c is
 * 4 bytes short, so the definition's types are settled.
 *
 * But sub_08039188 passes the first TWO arguments with NO narrowing at all:
 * `lsls r7,r2,#4` and `lsls r0,r0,#4` go straight into r0/r1, and a `u16` or
 * `s16` parameter costs the caller a shift pair for each. ONE unnarrowed
 * argument would be an ambiguous type signal; TWO independent ones, of
 * different signedness, is not a wrong type at all -- it is the caller's
 * translation unit having NO PROTOTYPE in scope, so C89 default argument
 * promotion passed both as `int`. The callee's own PROMOTE_MODE prologue
 * re-normalises them, which is why the ROM is correct in spite of it.
 *
 * A single shared header cannot express "declared in one TU, undeclared in
 * another", so sub_08039188 CANNOT be matched here, and retyping either
 * parameter to `int` only moves the 4-byte loss into c_08039140.c. See
 * work/sub_08039188/ -- it is parked for this reason and not for want of
 * spellings. The general lesson: an argument the caller does not narrow is
 * evidence about the DECLARATION VISIBLE TO THE CALLER, which may be no
 * declaration at all, and a second unnarrowed argument in the same call is
 * what tells the two apart. */
u8 sub_08039140(u16, s16, u8, u8);
void sub_0803941C(int, int);
void sub_08039544(u8 *);
void sub_08039930(int, ProcPtr);
void sub_080399F8(int, int);
void sub_08039A58(int, int);
u8 *sub_08039F18(int);
/* Wave 32, W32-A: hoisted from the definition in src/decomp/c_0803B55C.c, which
 * had no declaration at all -- sub_08018FE4 is its first C caller. `int` is the
 * definition's own reading (bare `adds r4, r0, #0` prologue, see the correction
 * below), and the caller agrees: it hands over a value it has already
 * sign-extended with `ldrsh`, unnarrowed, through a plain `adds r0, r1, #0`. */
void sub_0803B55C(int);
/* CORRECTION, wave 24 (W24-B): this read `s16` from wave 7 until now, and the
 * assembly contradicts it. sub_0803B5A4's prologue is a BARE `adds r4, r0, #0`
 * -- the parameter is copied to a callee-saved register with no masking at all,
 * and PROMOTE_MODE fires unconditionally, so an `s16` (or `u16`, or `u8`)
 * parameter would have to carry `lsls #0x10; lsrs #0x10` there. It does not, so
 * the parameter is word-width. This is the same readout that types the two
 * lines above and sub_0803B55C's `int` in src/decomp/c_0803B55C.c, whose
 * prologue is the identical `adds r4, r0, #0`.
 * Corroborating, inside the body: `cmp r4, #0` compares the saved value
 * unnarrowed, and the `lsls r1,r4,#0x10; lsrs r1,r1,#0x10` further down is
 * PROMOTE_MODE at the sub_08070610 CALL (its second parameter is u16), not at
 * entry -- it lands after gUnknown_03005AE0 has been loaded into r0, i.e. in
 * the middle of the argument setup, which is a conversion at a use.
 * Safe for both call sites, verified by probe: src/proc.c's ProcCmd handler at
 * line 818 passes an `s16` dataImm, and `int` and `s16` compile byte-identically
 * there (both `ldrsh`) for the reason recorded on sub_0803B4DC above; the other
 * caller, src/decomp/c_0803B5E8.c, passes the literal 2. */
void sub_0803B5A4(int);
void sub_0803B5E8(void);

/* The 0x0803B0EC-0x0803B640 block, wave 24. Widths are recorded on each.
 *   sub_0803B0EC  void(void)  -- `pop {r0}`; no argument register is read.
 *   sub_0803B118  its one parameter is forwarded UNCHANGED to sub_080153B8 in
 *                 the else arm (`adds r0, r2, #0`), which fixes the type as
 *                 sub_080153B8's own `struct Unk03001470 *`; the then arm
 *                 reaches ->unk1e through the same register.
 *   sub_0803B198  void(void).
 *   sub_0803B37C  void(void).
 *   sub_0803B3B0  `int`: the prologue goes straight to `lsls r0, r0, #2` to
 *                 index gUnknown_080910FC with no PROMOTE_MODE masking of the
 *                 incoming register, so it is word-width (wave 21's bare-
 *                 prologue rule). Its one caller passes a literal.
 *   sub_0803B4EC  `int` with a `u16` LOCAL, not a `u16` parameter -- see the
 *                 note on the definition in src/decomp/c_0803B4EC.c; both
 *                 spellings give the same entry `lsls #0x10; lsrs #0x10`, and
 *                 the local is what sub_0803B524 next door already uses.
 *   sub_0803B588  void(void).
 *   sub_0803B640  void(void). */
void sub_0803B0EC(void);
void sub_0803B118(struct Unk03001470 *);
void sub_0803B198(void);
void sub_0803B37C(void);
void sub_0803B3B0(int);
void sub_0803B4EC(int);
/* sub_0803B578 is deliberately NOT declared here. It has fan-in 0 -- no `bl`
 * to it exists anywhere in asm/ and it is reached only through a ProcCmd table
 * -- so its parameter is the proc pointer, and its definition in
 * src/decomp/c_0803B578.c names a file-local struct for it the way every other
 * promoted proc callback does. A `ProcPtr` declaration here would conflict with
 * that definition and buys nothing, since nothing calls it by name. */
void sub_0803B588(void);
void sub_0803B640(void);
/* Both already promoted (src/decomp/c_0803B0D8.c, src/decomp/c_0803B350.c) but
 * never declared, because until wave 24 nothing outside their own file called
 * them. sub_0803B118 calls the first and sub_0803B37C the second; the types
 * below are copied from those definitions, not re-derived. */
void sub_0803B0D8(void);
void sub_0803B350(u16);

/* sub_0803ABD8 is a bare `bx lr` at 4 bytes with three callers (sub_0803AD48,
 * sub_0803AFA0, sub_0803B198), all of which call it with no argument setup;
 * nothing about its signature is recoverable and void(void) is the weakest
 * model. sub_0803AF5C is already promoted as void(void) in
 * src/decomp/c_0803AF5C.c; declared here so sub_0803B0EC can call it. */
void sub_0803ABD8(void);
/* WAVE 50, W50-B. The 0x0803AF78/84/90 forwarder trio and the 0x0803A174
 * blitter. All four are ALREADY MATCHED -- src/decomp/c_0803AF78.c and
 * src/decomp/c_0803A174.c define every one of them `void f(void)`, read off
 * the `pop {r0}` epilogue. They are declared here because sub_0803A190
 * installs sub_0803A174 as a callback (`sub_08011AAC((void *)sub_0803A174)`)
 * and sub_0803AFA0 calls the other three, and a declaration that disagreed
 * with a promoted definition is exactly what broke wave 14's first split
 * build -- so these agree with those definitions rather than being re-derived
 * from the call sites. */
void sub_0803A174(void);
void sub_0803AF78(void);
void sub_0803AF84(void);
void sub_0803AF90(void);
void sub_0803AF5C(void);

/* Four more m4a entry points, on top of the five listed above. All four names
 * come from data/fe_matches.json at `full` strength except sub_080705AC and
 * sub_08070610, which are not in it and are named from the shape of the call
 * sites instead:
 *   sub_080703F4  m4aSoundInit         void(void) -- `full` in fe_matches
 *   sub_08070478  m4aSongNumStart      void(u16). The width is read off the
 *                 CALLEE, which opens `lsls r0,r0,#0x10; lsrs r0,r0,#0xd` --
 *                 PROMOTE_MODE's zero-extension with the `<<2` table index
 *                 folded into the second shift, so `u16` and not `int`.
 *   sub_080705AC  void(void): the body is a fixed eleven-iteration loop over
 *                 gUnknown_08242308 at stride 0xc calling sub_08070C90 on each
 *                 -- m4aMPlayAllStop.
 *   sub_08070610  void(void *, u16): a two-line forwarder to sub_080703D4
 *                 (MPlayFadeOut, `full` in fe_matches) that zero-extends r1 and
 *                 passes r0 through untouched, i.e. m4aMPlayFadeOut(mplayInfo,
 *                 speed). The first parameter is `void *` and not a struct
 *                 pointer because gUnknown_03005AE0 is the only thing ever
 *                 passed and no member of it is reachable from C yet.
 * sub_08071420 is MPlayVolumeControl (`full` in fe_matches), whose FE
 * signature is (MusicPlayerInfo *, u16 trackBits, u16 volume); sub_0803B35C
 * calls it with the literal 0xFFFF as the track mask. */
void sub_080703F4(void);
void sub_08070478(u16);
void sub_080705AC(void);
void sub_08070610(void *, u16);
/* Wave 38 (W38-A): retyped from `void *` to `struct MusicPlayerInfo *` when the
 * body was matched -- it reads ident at +0x34, trackCount at +0x08 and tracks
 * at +0x2c, so the struct is reached from C now. Callers passing `void *`
 * convert implicitly. Same change made to sub_08071488 below. */
void sub_08071420(struct MusicPlayerInfo *, u16, u16);
/* Two more, both `full` in data/fe_matches.json and both leaves that end in a
 * bare `bx lr` -- m4aMPlayFadeOutPause (sub_08070620, from sub_0803B7D8) and
 * m4aMPlayFadeInContinue (sub_08070640, from sub_0803B804). The second
 * parameter is `u16` off the CALLEE in both: each opens `adds r2,r0,#0;
 * lsls r1,r1,#0x10; lsrs r1,r1,#0x10`, PROMOTE_MODE's zero-extension on the
 * incoming r1 before anything else happens, and stores it with `strh` at +0x24
 * and +0x26 of the MusicPlayerInfo.
 *
 * WAVE 24: these two take `struct MusicPlayerInfo *`, NOT `void *`. Both are
 * already promoted -- src/decomp/c_08070620.c and c_08070640.c define them
 * with the named struct -- and a `void *` declaration here compiles fine under
 * `trymatch`, which builds ONE unit, then fails the SPLIT build with
 * `conflicting types`. The struct is visible at this line: global.h includes
 * unknown-globals.h (which defines it) before unknown-functions.h. When a
 * promoted file has already named a type, agree with it; weakest-model applies
 * only to types nobody has named. */
void sub_08070620(struct MusicPlayerInfo *, u16);
void sub_08070640(struct MusicPlayerInfo *, u16);

/* `int`: the prologue is a bare `adds r4, r0, #0` with no masking of the
 * incoming register, so the parameter is word-width per wave 21's bare-prologue
 * rule. The body raises gUnknown_030040A0, starts the gUnknown_084858DC blob
 * through sub_080152EC and parks the argument in the resulting object's +0x1e
 * with a `strh`. That is the SAME member sub_0803B118 reads back sign-extended
 * to drive its 0x5a timeout -- a producer and a consumer of struct
 * Unk03001470's unk1e agreeing independently, which is the discriminating pair
 * the `s16` on that member rests on. The store itself is byte-neutral.
 * `pop {r4}; pop {r0}` -> void. */
void sub_08001038(int);
/* A one-line forwarder: `Proc_Start(gUnknown_0849BC98, parent)`, with the
 * incoming r0 copied to r1 and nothing else touched, so the parameter IS
 * Proc_Start's parent and carries its type. Its one caller, sub_0803BE40,
 * passes the literal 3, i.e. PROC_TREE_3. `pop {r0}; bx r0` -> void, so the new
 * proc is discarded. */
void sub_0803433C(ProcPtr);

/* The five m4a entry points the 0x0803B3C8-0x0803B408 forwarder run wraps. All
 * five names come from data/fe_matches.json at `full` strength, and all five
 * epilogues are `pop {r0}; bx r0` (or a bare `bx lr`), so every one is void:
 *   sub_0806FD98  m4aSoundVSync        void(void), leaf
 *   sub_08070990  m4aSoundMode         void(u32) -- the mode word, masked
 *                 0xFF / 0xF00 / 0xF000 in the body, which is the canonical
 *                 SOUND_MODE_REVERB / MAXCHN / MASVOL layout
 *   sub_08070A7C  m4aSoundVSyncOff     void(void)
 *   sub_0807046C  a void(void) forwarder to sub_0806F744 (m4aSoundMain)
 *   sub_08070AF8  m4aSoundVSyncOn -- already promoted as void(void) in
 *                 src/decomp/c_08070AF8.c; declared here so the wrapper can
 *                 call it. */
void sub_0806FD98(void);
void sub_08070990(u32);
void sub_08070A7C(void);
void sub_0807046C(void);
void sub_08070AF8(void);

/* sub_0803B3C8 is `sub_0803B3D4(8)` and sub_0803B3D4 is
 * `m4aSoundMode(n << 8)`, i.e. "use n mixer channels" -- 8 is the only value
 * the ROM asks for. `int`, not a narrow type: the parameter reaches the `bl`
 * through a bare `lsls r0, r0, #8` with no PROMOTE_MODE narrowing in front of
 * it, which a `u8`/`u16` parameter surviving a call would have carried. */
void sub_0803B3D4(int);

/* The two shared bodies behind the 0x0803C574-0x0803C644 predicate wrappers.
 * Both return -1 / 0 / 1 and both end `pop {r1}; bx r1`; the eight wrappers
 * forward the result with no re-narrowing, which fixes the return at int width
 * (a `u8`/`bool8` return would have put `lsls #24; lsrs #24` after each `bl`).
 *   sub_0803C52C(id, n)  -1 if sub_0803CAB8(id), else 1 when unlock 0x21 is set
 *                        and sub_08037DA4(gUnknown_0200C420.unk10) >= n, else 0.
 *                        `n` is `int`: it is compared with a signed `blt`
 *                        against sub_08037DA4's 2..5 result.
 *   sub_0803C5E8(id)     -1 if sub_0803CAD4(id), 1 if sub_0803CAB8(id), else 0.
 * `u32` for the id, matching the rest of the gUnknown_02028030 bit family it
 * forwards to unchanged. */
int sub_0803C52C(u32, int);
int sub_0803C5E8(u32);

/* On the constants this block passes, because they look like one enum and are
 * two different things. Do not spend a wave naming them.
 *
 * The arguments to sub_0803CBD8 -- 0x21 (sub_0803C598, sub_0803C52C), 0x22
 * (sub_0803C5C0), 0x26 (sub_0803C504) -- ARE tag-like: sub_0803CBD8 dispatches
 * on the range and 0x20..0x5f indexes bit (id - 0x20) of
 * gUnknown_02028030.unk00, so they are global flag ids out of one numbering.
 * Three values is not enough to reconstruct the enum, and nothing else in the
 * tree names one yet.
 *
 * The second argument to sub_0803C52C -- 3 (sub_0803C574), 4 (sub_0803C580),
 * 5 (sub_0803C58C) -- is NOT a tag. It is an ordinal compared with `>=`
 * against sub_08037DA4's band of gUnknown_0200C420.unk10:
 *
 *     unk10 <= 0xc7 -> 2   <= 0xf9 -> 3   <= 0x117 -> 4   else 5
 *
 * i.e. the three wrappers are "rank >= 3 / 4 / 5" on a 2..5 scale whose top
 * value nothing asks for by itself, and 2 is the floor rather than a case.
 * An enum would have to be dense over 2..5 and the wrappers only cover the
 * upper three, which is what a threshold looks like and not what a tag looks
 * like.
 *
 * And the third run in the same block (sub_0803C614/620/62C/638/644) passes no
 * constant at all: five byte-identical `return sub_0803C5E8(id);` forwarders,
 * distinguished only by their addresses. Whatever separates them lived in the
 * original's names, not in its code, so five distinct source functions is the
 * only thing the ROM proves here. */

/* Maps gUnknown_0200C420.unk10 onto a 2..5 band (`> 0xc7` -> 2, `> 0xf9` -> 3,
 * `> 0x117` -> 5, else 4). Leaf, bare `bx lr`. The argument is compared with
 * signed `bgt`, and sub_08038474 forwards the result with no re-narrowing, so
 * both ends are int-wide. */
int sub_08037DA4(int);

/* Returns 0/1 and every caller narrows the result with `lsls #24` before
 * testing it, which an `int` return would not need -- so the declared return
 * type is 8 bits wide. The argument is compared with `blt`/`bgt` against 1, 3
 * and 5, i.e. signed, hence `int` rather than a narrow type. */
bool8 sub_0803E388(int);

/* Same 0/1-plus-`lsls #24` shape as sub_0803E388, at the call site in
 * sub_08045830. */
bool8 sub_08045650(void);
/* Indexes gUnknown_08499598 by its argument and ends `pop {r0}; bx r0`. */
void sub_08044AB8(int);

/* NINE arguments -- four in registers and five on the stack -- behind the five
 * wrappers at 0x08044C44-0x08044D34. Read off its body: r0 is a proc script it
 * forwards to Proc_Start; r1/r2 become words at +0x4c/+0x50 of that proc (a
 * blob and a palette); r3 and the first four stack slots become bytes at
 * +0x2c..+0x30; and the FIFTH stack slot -- the wrappers' own parameter -- is
 * `ldr r1,[sp,#0x30]` immediately before `bl Proc_StartBlocking`, so it is the
 * parent ProcPtr and not payload.
 * The four small integers are `int` because three of the five wrappers pass -1
 * in slot 6: a `u8` parameter truncates that to 0xff at the call and gives
 * `movs #0xff` where the ROM has `movs #0; subs #1`. Callee-side those two
 * slots are narrower (slot 6 is `ldrb` straight off the stack slot, slot 7
 * carries an explicit `lsl #24; lsr #24`), so `s8` is what satisfies both
 * sides; it is caller-side byte-identical to `int` for every constant these
 * five pass, and whoever matches sub_08044D70 should switch to it. */
/* Wave 32 (W32-C) corrects the EIGHTH parameter from `int` to `u8`, from the
 * body. Arguments 4..8 all land in five consecutive bytes at +0x2c..+0x30, but
 * only ONE of them is narrowed in the prologue (`lsls #0x18; lsrs #0x18` on
 * [sp,#0x2c]), and the SEVENTH is instead read `ldrb` straight out of its
 * incoming slot at the use -- which is what an `int` whose byte is wanted does
 * and what a `u8` parameter never does. With both declared `int` agbcc picks
 * the other one to read bytewise and the two stores swap places. Byte-neutral
 * at all five wrappers in src/decomp/c_08044C44.c, which pass constants. */
void sub_08044D70(const struct ProcCmd *, void *, void *, int, int, int, int, u8, ProcPtr);
/* Wave 32 (W32-C), all four PROVISIONAL -- their own bodies are still asm and
 * these are the weakest models that fit their single call sites in
 * sub_08044AB8 and sub_080445A8. sub_0801DA94 writes r0 before reading it and
 * ends `pop {r0}`, so nullary and void is a readout; the other three are not. */
void sub_0801DA94(void);
/* Defined `void *` in src/decomp/c_08043A80.c; declared here in wave 32
 * (W32-C) because sub_08044AB8 is their first C caller, which also fixes
 * sub_08039A5C's first two parameters. */
void *sub_08043A80(int);
void *sub_08043A90(int);
/* Wave 34 (W34-H): PARAMETERS 3 AND 4 RETYPED int -> u16. Both carry a
 * `lsls #0x10; lsrs #0x10` pair at ENTRY, before any use and before the first
 * store in the body, which is PROMOTE_MODE on a sub-word parameter and not a
 * cast at a use -- written as `int` with `(u16)` casts the function is 4 bytes
 * SHORT and the narrowings move to the uses (measured, 19.6%). Safe to retype:
 * grepping src/ and data/promoted.json finds no promoted caller, so only
 * still-assembly callers exist and they cannot disagree. */
void sub_08039A5C(void *, void *, u16, u16);

/* `s8`, not int: sub_08016D04 returns -1 on its default path (`movs r0,#1;
 * rsbs r0,r0,#0`), and sub_0803B904 converts the result with
 * `lsls #24; asrs #8; lsrs #16`, which is exactly `(u16)(s8)ret` -- an int
 * return would emit only the `lsl #16; lsr #16` half. The parameter is `u8`
 * because the body opens by narrowing r0 with `lsls #24; lsrs #24`. */
s8 sub_08016D04(u8);
/* First parameter `u8` -- the prologue narrows r0 before the value survives a
 * call. The second is a `void (*)(void)` callback stashed at +0x4c of the proc
 * it starts; the three call sites (sub_08038548, sub_08038568, sub_08045770)
 * pass sub_0803BA00, sub_0803B8B8 and sub_0803B8A0, all of which ignore r0. */
void sub_0803D73C(u8, void (*)(void));
void sub_0803B8A0(void);
/* The first of the three callbacks the comment above names, and the one
 * sub_08038548 passes; `void (void)` on the same evidence (wave 20, W20-C). */
void sub_0803BA00(void);

/* Only referenced as values, stored into gUnknown_03004778 by sub_0805CDF0 and
 * sub_0805CE20. Both take no arguments and both end `pop {r0}; bx r0`, so both
 * are void(void). */
void sub_0805DB64(void);
void sub_0805DB70(void);
/* The rest of the same gUnknown_03004778 callback set, stored as values by the
 * 0x0805CA60-0x0805D1F0 list builders. None reads an argument register and all
 * end `pop {r0}; bx r0`, so all are void(void). */
void sub_0805D888(void);
void sub_0805DA84(void);
void sub_0805DB0C(void);
void sub_0805DB50(void);
void sub_0805DCA4(void);
void sub_0805DCD4(void);
void sub_0805DFB8(void);
void sub_0805DFE8(void);
void sub_0805DFF4(void);
void sub_0805E160(void);
/* Wave 44, W44-E: the same callback set, one member further on. sub_0805D2A0
 * stores it into gUnknown_03004778 with a plain `str` of the address and never
 * calls it, so `void (void)` follows the pointer's own type rather than the
 * body. */
void sub_0805E3BC(void);

/* Sorts the gUnknown_030045F0 id list the 0x0805Cxxx builders have just filled.
 * Its argument arrives in r0, is spilled whole with `str r0,[sp]` and only ever
 * tested `!= 0`, so it is word-wide; every caller passes gUnknown_0300477C. */
void sub_0805D344(u32);

void sub_08063994(void);
void sub_0806A454(void);
void sub_0806CC4C(void);
void sub_0806CC64(void);

void sub_080718F0(void);
/* `ldrb r0, [r0]; bx lr` -- a 4-byte routine that exists to be COPIED, not
 * called in place: sub_0808AD6C relocates its two halfwords into a
 * caller-supplied buffer and publishes the copy through gUnknown_03000F6C.
 * The signature comes from that call site (one pointer in r0, result narrowed
 * with `lsls #24; lsrs #24` by sub_0808ADA4), not from the body. */
u8 sub_0808AD68(u8 *);
/* Wave 32 (W32-B): DEFINED in src/decomp/c_0808AD6C.c and never declared, which
 * nothing noticed until this wave gave it C callers. Published unchanged. */
void sub_0808AD6C(u16 *);
/* Wave 47 (W47-H). ProgramFlashSector's byte-copy twin of sub_0808B430: the
 * same unlock sequence and sector-address computation, but the fill loop
 * copies from a caller buffer instead of storing 0xFF. The u16 return is
 * settled at its only caller, sub_0808B5B8, which re-narrows the result with
 * `lsls #0x10; lsrs #0x10`; the second parameter is a byte pointer because
 * that caller advances it by the sector size in bytes. */
u16 sub_0808B540(u16, const u8 *);
/* Wave 46 (W46-D). The flash chip's ReadFlashId: relocates sub_0808AD68's
 * `ldrb r0,[r0]; bx lr` into a stack buffer with sub_0808AD6C, calls the copy
 * through `sp + 1` twice to read the maker and device bytes, and returns them
 * packed as `(maker << 8) | device`. The u16 return is settled at its one
 * caller, sub_0808AB8C, which re-narrows with `lsls #0x10; lsrs #0x10`. */
u16 sub_0808AAF4(void);
/* Two whole-list sweeps, each reached only through a 16-byte
 * `push {lr}; ldr r0,=g; bl <it>; pop {r0}; bx r0` forwarder. Both end
 * `pop {r4,...}; pop {r0}; bx r0`, so both are void.
 *
 * sub_0806377C walks gUnknown_03001470[29..0] (stride 0x60, cursor starting at
 * base + 0xAE0 and stepping down) comparing `.unk00` against its argument and
 * calling sub_08015C30(i) on each hit -- the descending twin of sub_0801537C,
 * which does the same scan ascending and calls sub_08015328. `.unk00` holds the
 * blob the slot was created for: sub_080656E0 is `sub_080152EC(gUnknown_08580C7C,
 * 3)` and sub_08065700 is `sub_0806377C(gUnknown_08580C7C)`, with the identical
 * pair repeated at sub_0806D820 / sub_0806D840 over gUnknown_08581F40. So the
 * parameter is sub_080152EC's `const void *` and nothing narrower is provable:
 * the body only compares it as a word.
 *
 * sub_08067504 is Proc_BreakEach open-coded over proc.c's sProcArray -- 32
 * slots at stride 0x6c, `proc->proc_script == arg` selects, Proc_Break(proc)
 * acts. The compared word IS proc_script, so the parameter is proc.h's
 * `const struct ProcCmd *` rather than an opaque pointer. */
void sub_0806377C(const void *);
/* Wave 44 (W44-A): the sub_0806377C scan repeated twice more over the same
 * descending gUnknown_03001470[29..0] cursor, and all three are now matched.
 * sub_080637D8 is sub_0806377C with sub_08015A30 in place of sub_08015C30 --
 * one differing instruction. sub_08063814 inverts the guard (`beq` skips the
 * call, so it acts on every slot that is NOT the argument's) and calls
 * sub_08015328, whose s16 parameter is the whole reason its index narrows with
 * `lsls #0x10; asrs #0x10` where the other two use the u8 pair. Same
 * `const void *` as sub_0806377C for the same reason: the body only ever
 * compares the argument against `.unk00` as a word, so nothing narrower is
 * provable. sub_080637AC (declared below) is the fourth member -- it returns
 * the slot instead of acting on it. */
void sub_080637D8(const void *);
void sub_08063814(const void *);
/* Wave 44 (W44-A): spins until REG_SIOCNT's bit 7 clears, giving up after
 * 0x795C iterations, then calls sub_08063614(0x258). Both builds' only caller
 * discards nothing -- it ends `pop {r0}; bx r0` with r0 dead, so void. */
void sub_0806362C(void);
/* Wave 44 (W44-A): HAND-WRITTEN, see data/asm-resident.json. A busy-wait that
 * reads the PC to pick its per-iteration cost, so the argument is a delay
 * measured in cycles, not iterations. Declared here only so its one C caller
 * sub_0806362C can name it; `int` is the argument's widest honest type and the
 * single call site passes a plain constant. */
void sub_08063614(int);
void sub_08067504(const struct ProcCmd *);
/* The gUnknown_08613EE4 screen-fade driver, plus the three void(void) routines
 * the 41 wrappers at 0x08071F88-0x08072288 hand to it or call beside it.
 *
 * sub_080722B8(kind, speed, parent, onDone):
 *   `kind` is 0..7 and indexes gUnknown_081CBF68 -- 0x60 bytes of data, i.e.
 *   exactly 8 records of 12, which is why the prologue scales it by `n*3<<2`.
 *   Each record is
 *     { ProcPtr (*start)(const struct ProcCmd *, ProcPtr);
 *       void (*setup)(s8);
 *       s32 dir; }
 *   `start` is 0x0801C8F4 or 0x0801C95C (the Proc_Start / Proc_StartBlocking
 *   pair) applied to gUnknown_08613EE4 with `parent` as the second argument --
 *   which is what fixes the wrappers' third parameter as ProcPtr; `setup` is
 *   one of sub_080137AC / sub_08013830 / sub_080138B0 / sub_0801394C; `dir` is
 *   +1 or -1, i.e. fade in versus fade out. The ROM passes kind 0,1,2,3,4,6,7.
 *
 *   `speed` is the same quantity as the `int` on the sub_08011550 family at the
 *   top of this header, on a different accumulator, and again it is NOT a
 *   bitmask. It is stored as a WORD at +0x54 and read twice: sub_08072344 adds
 *   it to the +0x5c accumulator every frame and finishes at 0x200, so the fade
 *   lasts 0x200/speed frames; and sub_080722B8 itself passes
 *   `(s8)(MAX(speed >> 4, 1) * dir)` to `setup` as the per-step delta. That
 *   shift is `asrs`, not `lsrs`, so `speed` is signed -- `int`, not `u32`.
 *   The wrappers pass 4 / 8 / 0x10 / 0x20 / 0x40 = 128 / 64 / 32 / 16 / 8
 *   frames, with 0x10 the nominal rate (it is also what sub_08072394 passes to
 *   sub_08013780, and what leaves the `>> 4` scale at 1).
 *
 *   `onDone` is stored at +0x4c and invoked by sub_08072320 through a bare
 *   `bl _call_via_r0` with no arguments, guarded by `cmp r0, #0` -- so it is a
 *   `void (*)(void)` and NULL is the "nothing to do" case. It has to be
 *   declared as a function pointer rather than an int: that is what makes the
 *   wrappers' literal-pool word relocate against sub_080723DC / sub_08072454
 *   instead of becoming a plain constant. */
void sub_080722B8(int, int, ProcPtr, void (*)(void));
void sub_080723DC(void);
void sub_08072454(void);
void sub_08072394(void);
/* Already matched and promoted as src/decomp/c_08013780.c, which is where this
 * signature comes from -- the definition wins over any weaker model. It is
 * declared here because wave 26 promoted its two callers (sub_08072394 and
 * sub_080723A8) into a different unit. */
void sub_08013780(u16, u16, u8);
/* Promoted as src/decomp/c_080723C0.c. Declared here for sub_08072394 and
 * sub_080723A8, which both call it as their tail statement. */
void sub_080723C0(void);
/* The per-frame step of the gUnknown_08613EE4 fade sub_08072320 drives. It
 * returns "still fading": sub_08072320 narrows the result with `lsls #0x18`
 * before testing it, which is the caller-side tell for an 8-bit return, and
 * `movs r0,#1` / `movs r0,#0` reaching a common epilogue via `b` is the value
 * itself. It reads the proc's +0x54/+0x58/+0x5c, so it takes the proc; ProcPtr
 * is the weakest model and this will want a struct once it is matched. */
u8 sub_08072344(ProcPtr);
/* Starts the gUnknown_08613F2C proc and RETURNS it -- settled from the
 * callers, since the value in r0 at the epilogue is a coincidence of the last
 * store. sub_080725E4 and sub_080725FC both write +0x3a through the result,
 * which is what makes the return real rather than accidental. The fifth
 * argument arrives on the stack and is the parent. */
ProcPtr sub_080725A8(int, int, int, int, ProcPtr);
/* Promoted as src/decomp/c_08072970.c, whose definition fixes all three
 * parameters as u32. The first is really a FUNCTION POINTER -- sub_08072948
 * calls the +0x2c field it lands in through `_call_via_r1` -- so its callers
 * cast, e.g. sub_08072BA4 passing `(u32)sub_08072BBC`. */
void sub_0807298C(u32, u32, u32);
/* Publishes an x/y scroll pair into one of four BG scroll shadows selected by
 * the first argument (0..3 -> BG0/BG1/BG2/BG3; anything else is a no-op).
 *
 * ALL THREE parameters are 16-bit, not just the second. sub_08072C40's own
 * prologue is `lsls/lsrs #16` on r0, r1 AND r2 -- PROMOTE_MODE zero-extends
 * every sub-word parameter at entry, so those three pairs ARE the declaration,
 * and a `u32` parameter could not produce one. Declared `(u32, u16, u32)` from
 * wave 12 until wave 21 matched the callee itself; the first and third were
 * never checked against a body, only against callers that happened to agree.
 * They agree because every promoted caller passes either a literal or a `u16`
 * lvalue, so the retype is byte-neutral at all of them (re-verified with
 * trymatch: c_08068AC4, c_08069EAC, c_0806A054, c_0806BB08, c_0806C52C,
 * c_0806EB5C, c_08075368) -- which is exactly why the wrong spelling survived
 * nine waves. The `(u16)` casts some promoted call sites carry on the third
 * argument were compensating for the wrong declaration; they are now
 * redundant, and harmless.
 *
 * The `bgt` on the selector is taken on the ZERO-EXTENDED value with no
 * sign-extension inserted, which is what makes the first parameter `u16`
 * rather than `s16`. */
void sub_08072C40(u16, u16, u16);
s32 Interpolate(s32, s32, s32, s32, s32);

s32 Div(s32, s32);

/* ---- callees of the three wrapper families worked in wave 12 ---- */

/* Run-or-defer. If gUnknown_03001FE0 is non-zero it calls
 * `fn(gUnknown_03001FE0, arg)` immediately through `bl _call_via_r2`;
 * otherwise it queues the pair with `sub_0801EDC0(fn, (s16)arg)`.
 * `pop {r1}; bx r1`, so it returns a value -- both of its wrappers
 * (sub_0802BCD8, sub_0803A53C) discard it and are themselves void.
 * The callback is `void *` and cast at the call site, the same convention
 * sub_08011AAC / sub_08011B34 already use: the two functions registered here
 * (sub_0802BC5C, sub_0803A07C) are void(void) bodies that ignore both of the
 * arguments they are handed, so no honest function-pointer type exists.
 * The second parameter's WIDTH is not recoverable -- sub_0801F024 narrows r1
 * with `lsls #16; lsrs #16` in its own prologue, but both call sites pass a
 * `movs`-sized literal and that is byte-identical for every integer type.
 *
 * THE SECOND PARAMETER IS `u16` (wave 29, W29-B, matching the definition). The
 * paragraph above is right that the two call sites cannot see it -- both pass a
 * `movs`-sized literal -- but the definition can: r1 is narrowed
 * `lsls #0x10; lsrs #0x10` IN PLACE at entry, which is PROMOTE_MODE on a
 * declared-narrow parameter, and the queueing arm then converts it to `s16`
 * with `lsls #0x10; asrs #0x10` for sub_0801EDC0. A declared `int` gives
 * neither shift pair. Both promoted callers stay byte-identical. */
int sub_0801F024(void *, u16);
/* sub_0801F024's queueing arm. Its second parameter is `s16` (`lsls #0x10;
 * asrs #0x10` at entry) and its first is ORed with 0x80000000 before being
 * handed to sub_0801ECE8, i.e. a tagged callback word; `void *` is what its one
 * caller passes and needs no conversion. Returns a value. */
int sub_0801EDC0(void *, s16);

/* ---- wave 29, W29-B: address-locality block 0x0801F --------------------- */
/* The gUnknown_0848B738 palette loader. Argument 1 indexes the table (no entry
 * narrowing, so `int`); argument 2 is a palette slot, scaled `* 0x20` and cut
 * to 16 bits -- `lsls #0x15; lsrs #0x10` is a NET LEFT SHIFT OF FIVE under a
 * `(u16)` cast, not a mask plus a shift, and it has to be written as the
 * MULTIPLY `(u16)(b * 0x20)`. BOTH parameters are `int`, and that is measured:
 * a `u8` second parameter splits the pair into `lsls #0x18` at entry and
 * `lsrs #0x13` at the use, and it also forces a narrowing into sub_0801F150 in
 * front of the `bl` that the ROM does not have. */
void sub_0801F178(int, int);
/* sub_0801F084's if-arm callees. sub_0801BF2C walks the gUnknown_0200D510 layer
 * list whose head it selects with `lsls #4` on its argument -- a SpriteEntry
 * index, so `int` and no narrowing anywhere; sub_0801EE10 ignores r0 entirely
 * (`b` straight past the prologue). Both end `pop {rN}; pop {r0}; bx r0`, so
 * both are void. */
void sub_0801BF2C(int);
void sub_0801EE10(void);
void sub_0801F084(void);
/* Two halves of one mapping between a tile/palette id and the six-entry
 * gUnknown_0848B738 table: sub_0801F400 is the table's third column read out of
 * a jump table, and sub_0801F3D4 the inverse, a descending if/else ladder over
 * the same six bounds. Both are SIGNED compares on an unnarrowed argument, and
 * both fall off the end on an out-of-range value -- sub_0801F400 with r0 never
 * written, so the argument itself comes back out. */
int sub_0801F3D4(int);
/* Wave 33, W33-B: sub_0801F19C/sub_0801F234 both call this as
 * `sub_0801F444(a1, sub_0801F3D4(a1))` and hand the result to CpuFastSet as its
 * `const void *` source, so it returns a pointer and takes two ints. */
void *sub_0801F444(int, int);
/* Wave 33, W33-B: block 0x0801E000's sprite-request sink. Given a filled
 * gUnknown_0200ED20 entry and an s16, returns an s16 handle (caller re-narrows
 * and compares against -1). */
s16 sub_0801A718(struct Unk0200ED20 *, s16);
/* Wave 40 (W40-J): the pop half of the same free list. `u32` is NOT a choice
 * here -- src/decomp/c_0801A700.c is promoted and defines it that way, so
 * sub_0801EE10 casts the result to `struct Unk0200ED20 *`. sub_0801A6C0 is the
 * list reset; `bool8` agrees with its own 98.4% parked draft and its result is
 * ignored at every call site, so nothing here can discriminate it. */
u32 sub_0801A700(void);
bool8 sub_0801A6C0(void);
/* Wave 40 (W40-J): the sprite-request renderer. Every argument type is read off
 * sub_0801EE10's call site, which fills them straight out of one
 * struct Unk0200ED20 entry: three `ldrsh` (s16), the entry's tagged unk04
 * pointer in r3, then the entry's `long long` unk0c as the [sp]/[sp,#4] pair
 * and a final `ldrsh` of unk0a at [sp,#8]. Seven argument slots. */
int sub_0801E9B0(s16, s16, s16, void *, long long, s16);
/* Wave 40 (W40-J): block 0x0801E000's per-frame interpolation sweep over the
 * 32 gUnknown_0808F0B4 records, and the OAM-attribute expander. sub_0801E3E8's
 * fourth argument is a u16 instruction stream walked with `ldrh rN,[r4]; adds
 * r4,#2`, and its fifth arrives on the stack as an affine-parameter index. */
void sub_0801E2A4(void);
int sub_0801E3E8(int, int, int, u16 *, int);
/* Wave 42 (W42-G): sub_0801DFE8's third switch arm, called with exactly the
 * argument block sub_0801E3E8 gets (same five slots, same registers, same
 * `str` of the fifth). Types are copied from that sibling; the result is
 * ignored at the only call site, so `int` is the weakest thing that fits. */
int sub_0801E508(int, int, int, u16 *, int);
/* Wave 33, W33-B: sub_080466DC's tail reset, nullary void. */
void sub_08045FC8(void);
/* Wave 33, W33-B: the 56-frame cursor advance sub_080466DC leads with (matched,
 * c_080466A4.c); nullary void. */
void sub_080466A4(void);
int sub_0801F400(int);

/* A `bx lr` stub taking the address of one of the two gUnknown_030013B0 /
 * gUnknown_030013D0 records; sub_080252EC calls it once on each. The body
 * reads nothing, so the parameter type is unconstrained and `void *` is the
 * weakest model that accepts both. */
void sub_080252E8(void *);

/* All void(void): every one ends `pop {r0}` (or a bare `bx lr`), and none of
 * them reads r0-r3 before writing it. They are here because the wave-12
 * three-call wrapper family (sub_08002EB4, sub_08028154, sub_0802CD00,
 * sub_0802CD14, sub_08034FD8, sub_08034FEC, sub_0805DB50) and the two
 * sub_0801F024 wrappers name them and nothing else declared them yet.
 * Note in particular that the three-call wrappers are three INDEPENDENT
 * statements and not a nest: no argument register is set up anywhere in them,
 * and every callee here takes none. */
/* The two single-slot callback setters on gUnknown_030040D0 / gUnknown_030040EC,
 * already defined this way in src/decomp/c_080366C4.c and declared here only
 * because sub_080370F0 lives in another translation unit. They take
 * `void (*)(void)` and NOT `void *`, so their call sites pass a function by
 * name with no cast -- the opposite of the sub_08011AAC / sub_08011B34 /
 * sub_0801F024 convention a few lines up. sub_0803662C registers sub_08036944 /
 * sub_080369BC through the same pair, sub_080370F0 registers sub_08036884 /
 * sub_080368E8; the two wrappers are what fix which setter drives which slot. */
void sub_080366C4(void (*)(void));
void sub_080366D0(void (*)(void));
void sub_08002D7C(void);
void sub_08002DEC(void);
void sub_08002E5C(void);
void sub_0801A614(void);
void sub_08023348(void);
void sub_08023354(void);
void sub_08024584(void);
void sub_0802BC5C(void);
void sub_0802D4B0(void);
void sub_0802D504(void);
void sub_0803662C(void);
void sub_08036884(void);
void sub_080368E8(void);
void sub_0803A07C(void);
/* Wave 29, W29-B: sub_0803A440 registers this one alongside sub_0803A07C
 * through the same sub_0801F024, so the same void(void) shape. */
void sub_08039F80(void);
void sub_0805E5AC(void);
void sub_0805E718(void);
void sub_0805F4CC(void);
/* Already promoted in src/decomp/c_080735B0.c (a DMA0 shutdown); declared here
 * so sub_0806F2C0 / sub_080735D0 / sub_080736D8 can hand its address to
 * sub_08011AAC. */
void sub_080735B0(void);

/* --- wave 12, families F003 and F005 -------------------------------------
 * F003 is 34 twelve-byte `push {lr}; movs r0,#K; bl S; pop {r0}; bx r0`
 * forwarders and F005 is 19 sixteen-byte `push {lr}; bl S; bl S; pop {r0};
 * bx r0` pairs. Every one is `void f(void)` by the `pop {r0}` rule. The
 * callees below are what those 40 wrappers name; the ones already promoted
 * are declared here verbatim from their definitions, the rest carry their
 * evidence.
 *
 * Caveat that applies to every PARAMETER type in this block: F003 always
 * passes a literal `movs r0,#K`, so the call site emits the same two bytes
 * for `int`/`u32`/`u16`/`s16`/`u8` alike. None of the wrappers constrains
 * these widths, and none of them changes if a later wave corrects one. The
 * widths come from the callee's own prologue, which the doc warns is weak
 * whenever the narrowed value goes on to feed a call -- flagged per entry. */

/* Already promoted, declared here from their definitions:
 *   src/decomp/c_0800056C.c  void sub_0800056C(u16 a)
 *   src/decomp/c_0803B6E8.c  void sub_0803B6E8(int a)
 *   src/decomp/c_08073900.c  void sub_08073900(s32 arg)
 *   src/decomp/c_08073C88.c  void sub_08073C88(s32 a) */
void sub_0800056C(u16);
void sub_0803B6E8(int);
void sub_08073900(s32);
void sub_08073C88(s32);

/* `int`: the prologue is a bare `adds r4, r0, #0` and the saved value then
 * survives `bl sub_08011B18` before being compared, which PROMOTE_MODE would
 * have narrowed for any sub-word parameter. `pop {r4,r5,r6}; pop {r0}`, void.
 * Only ever called as 1 (sub_08023348) and 0 (sub_08023354). */
void sub_08023360(int);
/* `u8`: opens `lsls r0,#0x18; lsrs r0,#0x18` with the result going straight
 * into a four-way `cmp`/`beq` chain, not into a call, so this is PROMOTE_MODE
 * on the parameter rather than a cast at a use. `pop {r0}`, void. */
void sub_0802776C(u8);
/* `u8`: opens `lsls r0,#0x18; lsrs r3,r0,#0x18` and stores the result with
 * `strb`. Independently confirmed -- the gUnknown_0849ECDC comment in
 * unknown-globals.h already records "a u8 argument by sub_0803B930".
 * `pop {r4,r5,r6,r7}; pop {r0}`, void. */
void sub_0803B930(u8);
/* At least 16 bits, and nothing narrower is provable: the whole body is
 * `strh r0, [=gUnknown_030005CE]` followed by a sub_08071420 call, and the
 * doc's setter table makes a bare `strh` identical for int/u32/u16/s16.
 * `int` is the weakest reading. `pop {r0}`, void. */
void sub_0803B35C(int);
/* `int`, on the sub_0803B4DC precedent: the prologue is a bare
 * `adds r4, r0, #0` and the `lsls #0x10; asrs #0x10` lands in the MIDDLE of
 * the argument setup for sub_08023168, after both globals have been loaded --
 * a cast at the use, not PROMOTE_MODE at entry. The same value is handed to
 * sub_08043418 later, also as `(s16)`. `pop {r4,r5}; pop {r0}`, void. */
void sub_08023274(int);
/* Wave 33, W33-B: the fifth callee shaped like sub_080230DC -- two s16 values
 * and one more, then two s16 out-parameters. Prototype derived from the call
 * sites in sub_08023274 / sub_080232CC (arg2 narrowed s16, arg3=&x, arg4=&y on
 * the stack). */
void sub_08023168(s16, s16, s16, s16 *, s16 *);
/* `int`: the prologue saves r0 with a bare `adds r4, r0, #0`, adds 0x28 to a
 * copy and calls sub_0801B768 with it, then masks the saved value `& 0x3ff` --
 * no narrowing anywhere, and a u8/u16 parameter surviving a `bl` would carry
 * one. `pop {r4}; pop {r0}`, void. */
void sub_0801B780(int);
/* `u16`, but only on the Proc-wrapper table's `adds r4, r0, #0; lsl #16;
 * lsr #16` row -- the copy and its narrowing are the first two things the
 * function does, before any global is touched. Weaker than the entries above
 * because the narrowed value does then feed `bl sub_0801A548`, which is the
 * shape wave 11 showed can be a cast at a use (sub_0803B524). Its only known
 * caller, sub_0804A03C, passes 0, so nothing here can settle it.
 * `pop {r4}; pop {r0}`, void. */
void sub_0801A5B0(u16);

/* CORRECTION, wave 20 (W20-C): the F005 header above says "Every one is
 * `void f(void)` by the `pop {r0}` rule". That rule settles the RETURN type
 * and nothing else; the empty PARAMETER list was an assumption and it is wrong
 * for two of the nineteen. Every call site of all 19 was re-read: only these
 * two have caller-side argument setup, and both are decisive.
 *   sub_0801A538 takes FOUR -- sub_08019DA8 passes 0, 1, 6 and 0xC, four
 * distinct non-zero constants, and the other three sites pass four zeros.
 *   sub_08085298 takes ONE -- all four callers do `adds r0, rN, #0` off a
 * callee-saved proc pointer in the instruction before the `bl`.
 * In both, the parameters are DEAD: the body's first `bl` overwrites r0, so
 * the definitions are byte-identical either way and no oracle in this tree
 * could have caught it from the callee side. Both promoted definitions were
 * retyped and re-verified. THE GENERAL POINT, since it recurs: an unused
 * parameter is invisible in the callee and visible only at a call site, which
 * is why arity has to be read from the CALLERS even when the body is two
 * instructions long. */
void sub_0801A538(int, int, int, int);
/* Both are `void (void)`: each opens `push {r4,r5,r6,r7,lr}; movs r5,#0` with
 * no read of r0-r3 anywhere, and both end `pop {r0}; bx r0`. They are a pair
 * of 76-byte twins over gUnknown_080909A4 and gUnknown_080909B0 respectively,
 * and sub_0803A460 / sub_08047094 call them back to back (wave 20, W20-C). */
void sub_08022580(void);
void sub_080227A8(void);
/* Wave 41 (W41-C). The per-tile helpers those two twins call, one each.
 * Both take (u16, u16) = map cell x, y, and both return void.  Two independent
 * readings agree: (a) the CALL SITE in sub_08022580 / sub_080227A8 builds each
 * argument as `... ; lsls #0x10; lsrs #0x10` immediately before the `bl`, which
 * is a u16 conversion of an int sum, and discards r0 afterwards; (b) each
 * CALLEE opens `lsls r0,#0x10; lsrs rN,r0,#0x10` / `lsls r1,#0x10;
 * lsrs rM,r1,#0x10`, agbcc's u16-parameter prologue, and neither sets r0 on
 * any path that reaches its epilogue (sub_08022428 tail-calls sub_080223E0 in
 * one arm and sub_0802216C in the other, both void).  The call site is the
 * evidence that settles the widths; the definitions merely corroborate.
 * They are the same-shaped siblings of sub_080225CC / sub_080227F4, which are
 * already declared (u16, u16) just above. */
void sub_08022428(u16, u16);
void sub_08022618(u16, u16);
/* Wave 41 (W41-C). `void (void)` on the callee's own evidence: it opens
 * `push {r4,r5,r6,lr}; movs r5,#0; ldr r6,=<force-addr word>` and never reads
 * r0-r3, and it ends `pop {r0}; bx r0` with r0 left holding sub_08013B1C's
 * result, which no caller uses.  Same 16-iteration draw sweep as the
 * sub_08022580 twins, one row at a time. */
void sub_08021D10(void);
/* Wave 41 (W41-C), both from their own definitions (now matched).
 * sub_08021750's parameter is an `int` whose only use is `+ 0x4c` narrowed to
 * u8 for sub_0803CF3C's first parameter; nothing dereferences it, so it is not
 * a pointer.  sub_080212AC's is `u16` -- the `lsls #0x10; lsrs #0x10` prologue
 * -- and it indexes both gUnknown_08499598 (0x3c stride) and the two
 * gUnknown_084995F4 / _FE halfword tables as an army slot.  Neither sets r0. */
void sub_08021750(int);
void sub_080212AC(u16);
/* Wave 49 (W49-K), from sub_080213AC's call sites plus each callee's prologue.
 * Both take a pointer into the gUnknown_08499590 map at +0x1E42 (one 0x508-byte
 * overlay plane per slot); sub_08020754 opens `adds r5,r0,#0` and never narrows
 * it, sub_080206E4 opens `mov ip,r0`.  sub_080206E4's second parameter is `u16`
 * -- its prologue narrows with `lsls r1,#0x10; lsrs r7,r1,#0x10` -- but that is
 * the callee's own `(u16)` cast at a use, NOT the parameter's type: agbcc
 * narrows a `u16` argument AT THE CALLER too, and sub_080213AC's two call sites
 * pass `1 - unk0d` with no narrowing at all.  The caller is the authority here,
 * so the parameter is `int`; declaring it `u16` costs 4 bytes per call site. */
void sub_08020754(u8 *);
void sub_080206E4(u8 *, int);
/* Wave 49 (W49-K), from their own definitions.  sub_080213AC reads nothing in
 * r0 and sets nothing.  sub_08021810 takes TWO out-pointers to bytes: r0 and r1
 * move straight to sb and r8 with no narrowing and both are only ever written
 * through with `strb`. */
void sub_080213AC(void);
void sub_08021810(u8 *, u8 *);
void sub_08085298(ProcPtr);
/* Wave 30, W30-E. Nullary on the callee's own evidence: it opens
 * `push {r4,r5,lr}; movs r4,#0; ldr r5,=gUnknown_081D93F8` and never reads r0.
 * sub_080851CC/sub_08085208 still hold their proc pointer in r0 at the `bl`,
 * which costs no instruction and is not argument setup. */
void sub_080853B0(void);

/* F005's callees. None of the 38 reads an argument register before writing
 * it -- every one opens with a `bl`, a pool `ldr` or a `movs` into r0 -- so
 * all take no parameters, and that is what makes the family two statements
 * rather than `g(f())`: a nullary second callee cannot consume the first's
 * result, and with a parameter it would need one. All end `pop {r0}` and are
 * void except sub_0801B4C0. Already promoted, declared from their
 * definitions: sub_0803B3EC (c_0803B3C8.c), sub_080199F8 (c_080199E0.c),
 * sub_08042C10, sub_08042B70, sub_08034F7C (c_08034F6C.c), sub_08013C54,
 * sub_08013AFC (c_08013AD4.c), sub_08034F48, sub_0805AC88. Three more of the
 * set -- sub_08023348, sub_08024584 and sub_0803662C -- are declared further
 * up this file by the F001 forwarder block, which reached them independently;
 * they are not repeated here. The probe behind the "two `bl`s" claim is in
 * docs/agbcc-codegen.md under Proc wrappers. */
void sub_0800485C(void);
void sub_08012A74(void);
void sub_08016E3C(void);
void sub_08017208(void);
void sub_0801759C(void);
void sub_080199F8(void);
void sub_0801A664(void);
void sub_080258CC(void);
/* Wave 45, W45-H. Declared to agree with the promoted definition in
 * src/decomp/c_0803442C.c, which had no header declaration. */
void sub_0803442C(u8 *src, u8 *dst);

/* Wave 45, W45-H. The twelve arms of sub_0805FD64's jump table on
 * gUnknown_030045D4. Nine of them already have promoted `void f(void)`
 * definitions and these declarations are copied from those, so they are a
 * contract that already holds:
 *   sub_08060424, sub_0806044C, sub_08060474  -- src/decomp/c_08060424.c
 *   sub_080604A4                              -- src/decomp/c_080604A4.c
 *   sub_08060324, sub_08060384, sub_080603D4  -- src/decomp/c_080601F0.c
 *   sub_0806050C                              -- src/decomp/c_0806050C.c
 *   sub_08060554                              -- src/decomp/c_08060554.c
 * sub_0805FF64 is matched this wave and is `void f(void)`.
 * sub_0805FE0C and sub_0805FFA0 are NOT matched and their arity is NOT proved:
 * sub_0805FD64 sets up no argument register and discards nothing, which is only
 * a floor. If either turns out to take an argument, fix it here. */
void sub_0805FE0C(void);
void sub_0805FF64(void);
void sub_0805FFA0(void);
void sub_08060324(void);
void sub_08060384(void);
void sub_080603D4(void);
void sub_08060424(void);
void sub_0806044C(void);
void sub_08060474(void);
void sub_080604A4(void);
void sub_0806050C(void);
void sub_08060554(void);
void sub_08034890(void);
void sub_08034F48(void);
void sub_08034F7C(void);
void sub_08035224(void);
void sub_08035354(void);
void sub_08042B70(void);
void sub_08042C10(void);
void sub_08013C54(void);
void sub_08013AFC(void);
void sub_0803B3EC(void);
void sub_080553C8(void);
void sub_08054C04(void);
void sub_0805AC88(void);
void sub_08061B4C(void);
/* Wave 50, W50-M, MATCHED. The map-editor debug overlay: reads the unit under
 * the cursor out of the gUnknown_08499590 +0x12 plane, publishes it to
 * gUnknown_03003F38 and gUnknown_030040D8, and prints five sub_08013428 lines
 * for it plus four each for its two transported units. */
void sub_08062DF0(void);
/* The one non-void member: `pop {r3,r4,r5}; pop {r4,r5,r6,r7}; pop {r1};
 * bx r1`. Its only caller, sub_0803AF90, discards the result, so only the
 * non-void-ness is proved and the width is a guess. */
int sub_0801B4C0(void);

/* Wave 40 (W40-F). The five 0x0801B000-block helpers that had no declaration,
 * copied verbatim from their promoted definitions in src/decomp/c_0801B598.c
 * and src/decomp/c_0801B6A8.c -- the evidence for every one of these types is
 * written up there and is not repeated. */
u16 sub_0801B598(u8, void (**)(void));
u16 sub_0801B618(u16, int);
int sub_0801B648(u16, int);
void sub_0801B66C(u16, int, int, int);
void sub_0801B6A8(u8 *, u32);

/* Wave 40 (W40-F). The 0x1000-byte staging buffer's validator: it checks a
 * four-byte magic, a 0x55/0xaa pair at the two ends of the block, a byte
 * checksum and its complement, and a constant 0xf. Returns 0 when every one of
 * those holds and 1 otherwise, so the sense is "failed", not "ok".
 *
 * `int` and not `u8`: its one caller sub_0801B018 truth-tests the result with a
 * bare `cmp r0, #0` and no re-narrowing, and agbcc re-narrows a narrow-returning
 * callee at every call site, so nothing here makes it narrow. */
int sub_0801B09C(void);


/* ---------------------------------------------------------------------------
 * Callees of the 12-byte forwarder family (`push {lr}; bl X; pop {r0}; bx r0`
 * -- family F001 in data/families.json, 50 members), settled in wave 12.
 *
 * A forwarder proves NOTHING about its callee: its `pop {r0}` overwrites
 * whatever came back, so `void f(void)` and `int f(void)` are byte-identical at
 * that call site. Every line below was therefore read off the CALLEE's own
 * body -- `pop {r0}; bx r0` (or a bare `bx lr` with no `movs r0` before it) is
 * what makes each one void, and the argument count is the number of r0-r3 read
 * before being written. The forwarders set up no argument register at all, so
 * each one passes its own parameters straight through; that is also why the
 * arity is invisible on the forwarder side and had to come from the callee.
 * -------------------------------------------------------------------------- */

/* void(void): none reads r0-r3 before writing it, and every one ends
 * `pop {r0}; bx r0`. The ones marked with a file are already promoted, and
 * these declarations have to keep agreeing with those definitions. */
void sub_080039E4(void);
void sub_08002FE4(void);
void sub_080123EC(void);
void sub_08022A08(void);
void sub_08024830(void);
void sub_0802481C(void);
void sub_08026290(void);
void sub_080267AC(void);
void sub_0802D3B0(void);
void sub_08011B18(void);            /* src/decomp/c_08011B18.c */
void sub_08013378(void);
void sub_08037678(void);
void sub_0803B774(void);            /* src/decomp/c_0803B774.c */
void sub_0803C890(void);
void sub_08049BD8(void);            /* src/decomp/c_08049BD8.c */
void sub_08052F3C(void);
/* Wave 51, W51-A. The two u16 parameters are confirmed by this function's own
 * prologue (`lsls #0x10; lsrs #0x10` on r0 and on r1). The THIRD parameter is
 * NOT confirmed here and cannot be: the body never reads r2, and an unused
 * trailing parameter is byte-neutral -- (u16, u16) and (u16, u16, int) compile
 * to an identical instruction stream, measured. It is declared with three
 * because sub_080536D8 calls this through the 0x50-byte THUMB pointer table in
 * data/data-0848B688.s, whose sibling sub_08052E04 is already declared
 * void (u16, u16, int) and which is subscripted with `lsls #2` and called via
 * `bl _call_via_ip` with three arguments; routines sharing one table share a
 * signature. Caller evidence only -- if a differently-shaped caller ever turns
 * up, this is the declaration to suspect. */
void sub_080523E8(u16, u16, int);
/* Wave 50, W50-F: already DEFINED as void(void) in src/decomp/c_08057464.c and
 * simply never declared, because until now its only caller was assembly. */
void sub_08057464(void);
void sub_080736D8(void);
void sub_080767A8(void);            /* src/decomp/c_080767A8.c */
/* Wave 37, W37-Q2: its one caller sub_0803BBD4 passes the u8
 * gUnknown_0200C420.unk0d straight through with no narrowing, and the callee
 * itself compares r0 against a `ldrb` from gUnknown_081D938C's table, so the
 * parameter is a byte id. The result is discarded at the only call site and
 * nothing re-narrows it, so `void` is the weakest model that fits. */
/* RETYPED u8 -> int in wave 44 (W44-F), and the body is the discriminator the
 * wave-37 caller-side reading could not reach. sub_08080F54 opens
 * `push {r4,lr}; adds r2,r0,#0` with NO `lsls r0,#0x18; lsrs r0,#0x18` -- and a
 * declared-narrow parameter's PROMOTE_MODE narrowing sits at the TOP of the
 * function unconditionally (same readout as the ApplyPaletteExt note above), so
 * the parameter is NOT eight bits wide. The wave-37 evidence is not contradicted,
 * only under-determined: its one caller sub_0803BBD4 passes a u8 struct field,
 * whose `ldrb` is already zero-extended, so the call site costs the same
 * instruction either way. c_0803BBD4.c was re-verified against the ROM after
 * this change. */
void sub_08080F54(int);
void sub_08078790(void);
void sub_08078864(void);
void sub_0808A6A0(void);
/* Wave 51 (W51-J). Nullary: its own prologue reads no argument register before
 * writing one, and sub_080880BC's only call site sets nothing up (r0 still
 * holds the byte it just wrote to gUnknown_03001FE8). */
void sub_0808A3DC(void);
/* Wave 51 (W51-J). Matches the promoted definition in src/decomp/c_0808A47C.c,
 * which was never declared in a header. */
void sub_0808A47C(void);

/* Tail of sub_08089F90/sub_08089C14's "animation finished" branch: both set
 * gUnknown_03005908 = 1 and then hand their own single pointer argument
 * (the same struct they were called with) straight to this.  Typed `void *`
 * because the struct is only described locally in those two files. */
void sub_08089464(void *);
/* src/decomp/c_080895E4.c already DEFINES this one and its tag is TU-local
 * there, so the tag is forward-declared here the way struct Unk4E334 is: the
 * definition wins, and sub_08089C14's caller casts to the incomplete type
 * rather than restating the body. */
struct Unk080895E4Proc;
void sub_080895E4(struct Unk080895E4Proc *);

/* Two EMPTY functions -- each is a lone `bx lr`, four bytes, nothing else.
 * `void(void)` and not `void(int)`: their only call sites are inside
 * sub_0803AFA0 (reached through the forwarders sub_0803AF78 / sub_0803AF84),
 * and there r0 holds the `1 & flags` / `4 & flags` of the test that just
 * branched -- a leftover, not an argument anyone set up. */
void sub_0801B4B8(void);
void sub_0801B4BC(void);

/* strcpy-shaped, and already defined as `char *(char *, const char *)` in
 * src/decomp/c_0808B678.c. Declared here so its forwarder sub_08004E38 can call
 * it; sub_08004E88 confirms the two arguments (`ldr r0,[r4]; adds r0,#0x9c;
 * adds r1, r5, #0`). The forwarder itself is void -- `pop {r0}`, not
 * `pop {r1}` -- so the char * result is dropped. */
char * sub_0808B678(char *, const char *);

/* One pointer in r0, void. NOT a Proc: it loads a pointer out of +0x20, which
 * is `proc_prev` inside PROC_HEADER, and indexes that as an array of 0x20-byte
 * records with a callback at +0x0c. Fields named: +0x20 (the record array),
 * +0x24 (one byte per record index), +0x31 and +0x42. Left `void *` until the
 * object is modelled; sub_08019B80 is the sibling that walks the same thing. */
void sub_08019B50(void *);

/* Proc callbacks, each `void(ProcPtr)`: all four read exactly r0, dereference
 * it past PROC_HEADER's 0x29 bytes, and end `pop {r0}`. sub_0803927C is proved
 * rather than inferred -- its siblings sub_080392C8 / sub_080392F4 hand the
 * same pointer to Proc_Break. `ProcPtr` is a placeholder for whatever struct
 * each one eventually gets; it is `void *`, so it costs no codegen, and it will
 * need retyping when these callees are matched themselves. */
void sub_08035F68(ProcPtr);         /* +0x36, and +0x39 via sub_08035E90 */
void sub_08035FA8(ProcPtr);         /* +0x36 */
void sub_0803927C(ProcPtr);         /* +0x2c, +0x30, +0x64 */
void sub_0807BF74(ProcPtr);         /* +0x58, a counter it decrements by 0x100 */

/* m4a. sub_0806F744 is SoundMain (data/fe_matches.json, `full`) and belongs to
 * the m4a_asm.s half of the driver -- see data/asm-resident.json for why no C
 * will match it. It is declared here only so m4aSoundMain (sub_0807046C,
 * declared above) can call it. sub_080703B8 is MPlayContinue, already defined
 * in src/decomp/c_080703B8.c; its forwarder sub_080705D8 is therefore
 * m4aMPlayContinue, and sub_080705E4 beside it is m4aMPlayAllContinue. */
void sub_0806F744(void);
void sub_080703B8(struct MusicPlayerInfo *);
/* Wave 31 (W31-C), the m4a callees the 0x08070478..0x080705A8 entry points
 * reach. sub_08070BAC is MPlayStart -- it takes the player info and the song
 * header word from the song table, in that order. sub_08070C90 is MPlayStop:
 * one player, and sub_08070544 gates it on `mplayInfo->songHeader` already
 * being that song. sub_080703D4 is the MPlayFadeOut sub_08070610 forwards to;
 * the `lsls #0x10; lsrs #0x10` in that forwarder is PROMOTE_MODE on its OWN u16
 * parameter and says nothing about this one's width. Its FIRST parameter is
 * declared from the promoted definition in src/decomp/c_080703B8.c and not from
 * that call site: sub_08070610 itself takes `void *`, which converts silently
 * and would have hidden the disagreement until the SPLIT build. */
/* Wave 47 (W47-B): the second parameter is retyped from `void *` to
 * `struct SongHeader *`. The body reads five members through it -- trackCount,
 * priority, reverb, tone and part[i] -- so the shape is no longer invisible.
 * Byte-neutral at every existing call site: they all pass `struct Song::header`,
 * which is `void *` and converts implicitly. */
void sub_08070BAC(struct MusicPlayerInfo *, struct SongHeader *);
void sub_08070C90(struct MusicPlayerInfo *);
/* Wave 47 (W47-A), the two per-track m4a helpers that walk a track's channel
 * chain. Both take (mplayInfo, track) and both IGNORE the first parameter --
 * r0 is dead on entry in each body and only r1 is read. The parameter is kept
 * because sub_08070C90 passes it: `adds r0,r6,#0; adds r1,r5,#0` at the call
 * site is two argument copies, not one.
 *   sub_0807004C -- TrackStop. Detaches every SoundChannel on the chain,
 *     silencing the CGB ones through SoundInfo::CgbOscOff first.
 *   sub_080702C0 -- ply_endtie. Reads the next command byte through
 *     MusicPlayerTrack::cmdPtr and sets 0x40 on the channel whose `mk` matches.
 *     Its OWN first parameter is a floor read off the register use, not off a
 *     caller: nothing in C reaches it yet, so `struct MusicPlayerInfo *` is
 *     lifted from the sub_0807004C twin beside it rather than proved. */
void sub_0807004C(struct MusicPlayerInfo *, struct MusicPlayerTrack *);
void sub_080702C0(struct MusicPlayerInfo *, struct MusicPlayerTrack *);
/* Wave 47 (W47-B), the rest of the m4a driver core.
 *   sub_0806F7C8 -- never CALLED, only its address is taken, with the THUMB bit
 *     masked off so sub_080703F4 can CpuSet its code into IWRAM. `void(void)`
 *     is the weakest shape that fits.
 *   sub_0806FDE4 -- MPlayMain. Also never called from C here; sub_08070B34
 *     parks its address in SoundInfo::func, and that member's type is what
 *     pins the signature.
 *   sub_0807004C -- TrackStop. sub_08070BAC calls it once per track in each of
 *     its two loops, always (mplayInfo, track).
 *   sub_080706B0 -- MPlayExtender, handed the gUnknown_030057D0 CGB channel
 *     array by m4aSoundInit.
 *   sub_080707E0 -- Clear64byte. Its parameter is retyped from `int` (see
 *     gUnknown_030057CC in unknown-globals.h): sub_08070B34 hands it a
 *     `struct MusicPlayerInfo *`, which is exactly 0x40 bytes, and that is the
 *     first call site to say anything about the type at all.
 *   sub_080707F4 -- SoundInit, already promoted in src/decomp/c_080707F4.c
 *     taking `struct SoundInfo *`; declared here so m4aSoundInit can call it.
 *   sub_08070A28 -- SoundClear, `pop {r0}; bx r0` and no argument register read
 *     before being written.
 *   sub_08070B34 -- MPlayOpen. The third parameter is narrowed
 *     `lsls #0x18; lsrs #0x18` on entry and clamped to 0x10, so u8. */
void sub_0806F7C8(void);
void sub_0806FDE4(struct MusicPlayerInfo *);
void sub_0807004C(struct MusicPlayerInfo *, struct MusicPlayerTrack *);
void sub_080706B0(struct CgbChannel *);
void sub_080707E0(void *);
void sub_080707F4(struct SoundInfo *);
void sub_08070A28(void);
void sub_08070B34(struct MusicPlayerInfo *, struct MusicPlayerTrack *, u8);
void sub_080703D4(struct MusicPlayerInfo *, u16);

/* The gUnknown_0200C528 lookup, and the s16 twin of sub_08015BD0: it scans the
 * ten 0x18-byte slots for the one whose .unk00 equals its argument and returns
 * that index, or -1. `const u8 *` for the argument to agree with
 * sub_080193B0/sub_0801930C, which take the same script pointers; `s16`
 * because its one result-keeping caller (sub_08019850) re-narrows with
 * `lsls #0x10; asrs #0x10`, and because the scan's own counter is a halfword
 * (`lsls #0x10; asrs #0x10` on every iteration). */
s16 sub_08019290(const u8 *);

/* Three argument-free routines named by the gUnknown_0200C528 cursor-advance
 * family at 0x08017E8C-0x0801903C, which lives in another translation unit and
 * so needs them visible. sub_08017E74/sub_08017E80 are already promoted in
 * src/decomp/c_08017E74.c as void(void) (`gUnknown_03001404 = 1` / `= 0`);
 * sub_08042B9C ends `pop {r0}; bx r0` and reads no argument register, so it is
 * void too. `(void)` is load-bearing at the call sites, not cosmetic: it is
 * what leaves the wrapper's own r0 untouched across the `bl`, which is exactly
 * what the ROM has, and it is why the wrapper's prologue narrowing is a
 * parameter-width tell there rather than a cast at a use. */
void sub_08017E74(void);
/* Wave 32, W32-A: sub_080193B0's other two openers, both already DEFINED in
 * src/decomp (c_08013D40.c, c_080198AC.c) as `void (void)` but never declared
 * -- sub_080193B0 is the first C caller of either. Copied from those
 * definitions, not inferred. */
void sub_08013D40(void);
void sub_080198AC(void);
/* Wave 32, W32-A: still assembly. Nullary -- no argument register is read
 * before being written -- and it returns 0 or 1 from two `movs` split across a
 * `b`, which its callers (sub_08014400, sub_08019510) truth-test with a bare
 * `lsls #0x18; cmp #0`, i.e. a one-byte result. */
bool8 sub_08014BE8(void);
void sub_08017E80(void);
void sub_08042B9C(void);
/* Wave 28, W28-A: a fourth, promoted in src/decomp/c_08017EEC.c as void(void).
 * sub_0801820C is its first cross-file user and only takes its ADDRESS, handing
 * it to sub_08011AAC's `void *` parameter. */
void sub_08017EEC(void);

/* The six redraw passes the four wrappers at 0x08023DCC-0x08023EA4 forward
 * their whole argument list to. All six are `pop {r0}`, i.e. void, and all six
 * narrow the arguments they use with `lsls #0x10; lsrs #0x10` -- PROMOTE_MODE
 * on u16 parameters that have to survive a call. 08023A4C/08023BAC mask their
 * first two with `& 0xf` instead, which is consistent with u16 and does not
 * contradict it. */
void sub_08023A4C(u16, u16, u16, u16);
void sub_08023BAC(u16, u16, u16, u16);
void sub_08023D14(u16, u16, u16, u16);
void sub_08023D48(u16, u16, u16, u16);
void sub_08023D7C(u16, u16, u16, u16);
void sub_08023DA4(u16, u16, u16, u16);


/* Dependencies of the F001 callees matched in wave 12.
 *
 * sub_08037790 and sub_08037628 are both void(void): neither reads r0-r3
 * before writing it, and both end `pop {r0}; bx r0`. sub_08037790 is only ever
 * named by address -- sub_08037678 registers it with sub_08011B5C and nothing
 * calls it -- so `void (void)` there comes from the registration list, exactly
 * as it does for sub_080184A4 higher up this file. */
void sub_08037628(void);

/* The gUnknown_030058E0 display-list builders. sub_080785FC resets the cursor
 * (`gUnknown_03005944 = 0`, then `bx lr`) and sub_08078758 fills the five words
 * of gUnknown_030059C0 with 1; both take nothing and return nothing.
 *
 * The middle four are `int(int)`. Each takes a byte index in r0, writes three
 * or four bytes into gUnknown_030058E0 starting there, and RETURNS the advanced
 * index -- every one ends `pop {r1}; bx r1`, which is the value-returning
 * epilogue. The width is pinned by sub_08078864, which chains all four
 * (`bl` ... `bl` with NOTHING between them, so each result is the next
 * argument): a narrow parameter or return would have put `lsl #24; lsr #24`
 * between consecutive calls, and there is none. */
/* Wave 37, W37-Q2: NOW DECLARED. The definition in src/decomp/c_08078198.c
 * was promoted with no declaration anywhere and picked `s32` for a body that
 * only ever returns the literals 1 and 0 -- byte-neutral there, so nothing in
 * that unit could settle it. Its first cross-unit caller does: sub_0803BBD4
 * re-narrows the result with `lsls r0,#0x18; lsrs r0,#0x18` before the zero
 * test, which is agbcc re-narrowing a BYTE-returning callee and is exactly
 * what an `s32` return could not produce. c_08078198.c was retyped to bool8
 * and re-verified byte-identical. */
bool8 sub_080781F0(void);
void sub_080785FC(void);
int sub_08078608(int);
int sub_08078658(int);
int sub_080786A4(int);
int sub_080786F0(int);
/* sub_08078740 is sub_08078758's twin: it walks the same five words of
 * gUnknown_030059C0 backwards storing 0 (`ldr r1,=g; movs r2,#0; adds r0,r1,#0;
 * adds r0,#0x10;` loop `str r2,[r0]; subs r0,#4; cmp r0,r1; bge`), takes
 * nothing and ends `bx lr`. It is DEFINED and matching in
 * src/decomp/c_08078740.c and was simply never declared -- the wave-14
 * "promoted but no prototype" trap. Its nullary-ness is what makes the third
 * `bl` of family F035 a statement of its own rather than a nest. */
void sub_08078740(void);
void sub_08078758(void);

/* The three gUnknown_0200E438 accessors that the ~40 sprite-attribute setters
 * between 0x0804B180 and 0x08053614 all use. Already promoted (and matched) in
 * src/decomp/c_0801566C.c, c_08015608.c and c_08015928.c; declared here so
 * callers can be written. The `struct UnkVec` by-value third form is real --
 * sub_08015608's two arguments arrive in r1 and r2 and are stored as
 * consecutive words -- and the eight bytes it moves are a `struct OamData`;
 * see the note on that type in unknown-globals.h. */
void sub_0801566C(s16, struct UnkVec *);
void sub_08015608(s16, struct UnkVec);
void sub_08015928(s16, u32);

/* ---- wave 35 (W35-K): blocks 0x08013 and 0x08015 ---------------------- *
 * sub_0801348C is the key-state update and sub_08013510 its once-per-frame
 * driver; the second parameter is `s16` from BOTH sides -- sub_0801348C opens
 * with one `lsls #0x10` feeding an `asrs` (which a u16 parameter never needs)
 * and sub_08013510 converts its u16 local with `lsls #0x10; asrs #0x10` at the
 * call. See struct Unk03002090 in unknown-globals.h for why the parameter is
 * NOT spelled `struct KeySt *` even though it is the same twenty bytes.
 *
 * sub_0801DC04 is declared from sub_08015438's call site and nowhere else:
 * r0 is sub_08015438's third parameter passed through untouched, r1 and r2 are
 * both `lsls #0x10; asrs #0x10` narrowings. It indexes gUnknown_0200E438,
 * whose entries sub_08015438 then writes.
 *
 * Wave 40, W40-I corrected the return type from `s8` to `int`, on evidence the
 * caller alone could not supply: sub_0801DC04's own body narrows the value it
 * returns with `lsls #0x10; asrs #0x10`, and an s8-returning function narrows
 * its returns at #0x18. So the s8 in sub_08015438's epilogue is a truncation
 * sub_08015438 itself performs, not a re-narrowing agbcc owes the callee, and
 * it is now spelled as an explicit `(s8)` cast there. That leaves c_08015438
 * byte-identical (re-verified with trymatch) and lets the definition match --
 * `s8` could not produce the #0x10 pair under any body spelling. `s16` was
 * ruled out the same way from the other side: it would add an automatic #0x10
 * re-narrow in sub_08015438 on top of the #0x18 the ROM has there.
 *
 * sub_080303B0/sub_080303C8/sub_08030234 are all reached only from
 * sub_08013510: the first's result is re-narrowed `lsls #0x18; lsrs #0x18` and
 * compared against 1, the second's `lsls #0x10; lsrs #0x10` into a u16 local,
 * and the third's r0 is dead at the call and never read afterwards. None takes
 * an argument -- r0 is written before any read in all three call sites. */
struct Unk03002090;
void sub_0801348C(struct Unk03002090 *, s16);
int sub_0801DC04(void *, s16, s16);
bool8 sub_080303B0(void);
u16 sub_080303C8(void);
void sub_08030234(void);
/* ---- wave 25 (W25-C): the 0x08015 slot-accessor callees. None of these ten
 * had ever been declared, and three of them are the "PROMOTED BUT NEVER
 * DECLARED" trap -- sub_0801D96C, sub_0801DA44 and sub_0801DA54 are already
 * matched in src/decomp/c_0801D96C.c, c_0801DA44.c and c_0801DA54.c, so their
 * types below are COPIED FROM THOSE DEFINITIONS and were not re-derived.
 *
 * THE COPIED TYPES REFUTE THE sub_080156C4 NOTE DIRECTLY BELOW. That note
 * argues the `lsls #0x10; asrs #0x10` in sub_080156C4's tail is agbcc
 * re-narrowing an s16-returning sub_0801DA54; sub_0801DA54's promoted
 * definition returns `u32`, so no re-narrowing happens and the shift pair is
 * sub_080156C4's own `return (s16)...` cast under an `int` return type. Same
 * for sub_080156A0/sub_0801DA44. The declaration below is unchanged and still
 * right -- only the reason for it was wrong. Wave 25.
 *
 * The seven genuinely new ones, each read off the callee's own body:
 *   sub_080151B0  the slot initialiser. r0 is stored to BOTH .unk00 and .unk04
 *     (`const void *`), r1 narrows IN PLACE (`lsls #0x18; lsrs #0x18`, no copy)
 *     so it is a declared `u8`, r2 is a bare `strb` into .unk14 (u8). Ends
 *     `pop {r0}; bx r0`, so void.
 *   sub_08015A30  runs the slot's script through gUnknown_0848A160; r0 narrows
 *     in place to u8 and it ends `pop {r0}`, so void.
 *   sub_08015224  sub_0801527C's twin -- same save/restore of gUnknown_03001FBC
 *     around sub_080151B0/sub_08015A30, but it stores 0 to .unk12 where
 *     sub_0801527C stores 4, and narrows its index u16 rather than u8. Returns
 *     `adds r0,r5,#0` off an `asrs #0x18`, so s8.
 *   sub_08015438  parameters 3 and 4 are declared HERE AS ITS CALLER ALREADY
 *     DECLARES THEM (`void *`), deliberately not re-derived: inside
 *     sub_08015438 parameter 4's only use is `lsls #0x10; asrs #0x10`, i.e. a
 *     small SIGNED INTEGER and almost certainly not a pointer. Left alone
 *     because sub_08015410 is its only promoted caller, forwards both unchanged
 *     and is byte-identical either way. Settle it when sub_08015438 is matched.
 *   sub_0801D778 / sub_0801D804  ARITY IS READ OFF THE CALLEE, NOT THE WRAPPER:
 *     D778 forwards r0-r3 and supplies a fifth 0 to sub_0801D6E8, so four
 *     parameters; D804 writes r3 itself before calling sub_0801D78C, so three.
 *     `int` returns: neither narrows its own result, and the SINGLE
 *     `lsls #0x18; asrs #0x18` at sub_08015578's merge point is that function's
 *     own s8 return -- an s8 return here would have put one after EACH `bl`.
 *   sub_0801D9E4  THREE parameters, and this is the pass-through-wrapper trap
 *     in its pure form: its only caller sub_080155E8 touches r0 alone, so the
 *     arity is completely invisible there. The body `strh`s into [r1] and [r2]
 *     after an `asrs #8`, so both are `s16 *` out-parameters. */
void sub_080151B0(const void *, u8, u8);
void sub_08015A30(u8);
s8 sub_08015224(const void *, s16, u8);
s8 sub_08015438(void *, int, void *, void *, int);
int sub_0801D778(int, int, int, int);
int sub_0801D804(int, int, int);
void sub_0801D96C(int, s16, s16);
void sub_0801D9E4(int, s16 *, s16 *);
u32 sub_0801DA44(int);
u32 sub_0801DA54(int);
/* The six 0x08015 block members matched in wave 25 that had no prototype.
 *   sub_0801527C  sub_08015224's twin and sub_080152EC's worker: same
 *     save/restore of gUnknown_03001FBC around sub_080151B0/sub_08015A30,
 *     storing 4 rather than 0 to the slot's .unk12. Returns its own second
 *     argument re-narrowed `lsls #0x18; asrs #0x18`, so s8 out of a u8
 *     parameter -- the sign change is real code and not a spelling.
 *   sub_08015578  dispatches on `a != 0xff` between sub_0801D778 (four
 *     arguments, `a` included) and sub_0801D804 (three, `a` dropped). s8
 *     return; parameters 2 and 3 are forwarded untouched to both callees and
 *     are therefore invisible here -- `int` is the weakest fit, not a proof.
 *   sub_080155A0  `return sub_08015578(0xff, a, b, c);`, i.e. the "no first
 *     argument" entry point to the same dispatcher.
 *   sub_080155E8  a THREE-argument forwarder whose last two arguments are
 *     invisible in its own body; see the sub_0801D9E4 note above.
 *   sub_080156A0  sub_080156C4's exact twin on sub_0801DA44 instead of
 *     sub_0801DA54, `int` return for the same reason.
 *   sub_08015CE4  `sub_08015328(a); return 0;` -- the `movs r0,#0` before the
 *     epilogue is what makes it non-void. Nothing constrains the return type
 *     further; its single caller discards it. */
s8 sub_0801527C(const void *, u8, u8);
s8 sub_08015578(s16, int, int, u8);
s8 sub_080155A0(int, int, u8);
void sub_080155E8(s16, s16 *, s16 *);
int sub_080156A0(s16);
int sub_08015CE4(u8);
/* Two more from the wave-25 extension.
 *   sub_08015158  counts the slots whose .unk00 is 0 -- the same 30-slot scan
 *     sub_08015184 clears and sub_08015BD0 searches. A leaf with no `push`;
 *     the u8 accumulator comes back already zero-extended, so nothing
 *     constrains the return width and `int` is the weakest fit.
 *   sub_08015CF4  one step of a slot's command list, advancing the .unk04
 *     cursor by 8 and always returning 1. The 8-byte record type is local to
 *     src/decomp/c_08015CF4.c because it only ever describes that cursor. */
int sub_08015158(void);
int sub_08015CF4(u8);
/* `return sub_0801DA54(gUnknown_03001470[a].unk26);` -- a lookup keyed on the
 * same slot index everything else in this family uses. The PARAMETER is a free
 * choice at every call site found so far: sub_0804E7A8 and sub_0804FCA4 pass
 * gUnknown_03001FBC, which is a declared `s16` global and therefore loads with
 * `ldrsh` whichever way the parameter is declared. The RETURN is `int` on the
 * caller's evidence rather than the callee's: both callers put the result in a
 * `u16` local and get `lsls #0x10; lsrs #0x10`, which an `s16`-declared return
 * would have made `asrs`. The `lsls #0x10; asrs #0x10` in sub_080156C4's own
 * tail belongs to sub_0801DA54's declared return width, not to this one --
 * see the "missing return keyword" carve-out in docs/agbcc-codegen.md. */
int sub_080156C4(s16);
/* Continuation callbacks handed to sub_08015928 as a bare pool word by
 * sub_0804D928 and sub_0804E3B4. Only the symbol's address is used, so the
 * signature is not recoverable from the call site; both are themselves members
 * of the same setter family and take no arguments there. Declared `void(void)`
 * so `(u32)` casts of them compile. */
/* Wave 34 (W34-L) RETYPES sub_0804DA40, and the note above now applies to
 * NEITHER of the two: its body has since been read and it is the same
 * continuation shape as sub_0804E4CC below -- `mov r8,r1` before any write,
 * `lsls r0,#0x10; asrs #0x10` into sub_0801566C, and r1's halfword at +4 read,
 * masked with 0xfc00 and stored back. Both call sites cast to `(u32)`, so
 * nothing moves. */
void sub_0804DA40(s16, u16 *);
/* Wave 34, W34-L. Matched and promoted in src/decomp/c_0804D25C.c; declared
 * here for sub_0804D1AC, its one `bl` caller. Same (u16, int, u16) shape as
 * sub_0804E100 -- parameter 2 is never read in the body, but the caller passes
 * the slot index. */
void sub_0804D25C(u16, int, u16);
/* Wave 34, W34-L. sub_080534A0 calls it with the literals 0 and 1 as the two
 * sides; its own prologue opens `lsls r0,#0x10; lsrs r0,#0x10`, PROMOTE_MODE on
 * a declared-narrow parameter, and the value indexes gUnknown_020296B0 and
 * gUnknown_030045A4 as an unsigned side id. Nothing is returned -- the `bl` is
 * the last thing on its path. */
void sub_08053520(u16);
/* Wave 34, W34-L. sub_080535E0 is matched and promoted in
 * src/decomp/c_080535E0.c; the other two are declared for sub_08053270 and
 * sub_080531D4, their `bl` callers, which set up no argument register and
 * discard no result. */
void sub_080535E0(void);
void sub_08053820(void);
void sub_0804BA4C(void);
/* Wave 34, W34-L. sub_08053670's tail call: both argument registers are set up
 * from halfword values (the side index and gUnknown_02029808[side].unk02[i]),
 * and nothing is done with a result. */
/* Wave 50, W50-F: THREE parameters, not two -- the wave-34 reading was taken
 * from the call site alone. The body narrows r0 AND r2 (`lsls #0x10; lsrs #0x10`
 * on each) and never reads r1 at all; r2 is the loop count. sub_08053670 sets up
 * only r0 and r1 at the `bl` because r2 ALREADY holds the value it wants there:
 * the `ldrh r2, [r1]` two statements earlier loaded
 * gUnknown_02029808[side].unk0e[unk00] into it for the `cmp #0xfe` guard, so the
 * third argument costs no instruction. Reading arity off that call site alone is
 * exactly the trap the header warns about elsewhere. The middle parameter is
 * genuinely unused by the body. */
void sub_080536D8(u16, u16, u16);
/* Wave 34, W34-L. sub_08053670 passes the same halfword side index and uses no
 * result. */
/* Wave 34 integration: the parameter is int, per the promoted definition. A
 * u16 parameter opens its body with `lsls #0x10; lsrs #0x10` under PROMOTE_MODE
 * and an int does not, so unlike u8-vs-s16 this IS visible in the callee -- and
 * the callee is the side that can see it. The narrowing the caller shows is a
 * cast at the use. */
void sub_08057BCC(int);
/* Matched and promoted in src/decomp/c_0804E100.c; declared here in wave 33
 * (W33-D) for sub_0804E050, its only `bl` caller. Parameter 2 is never read in
 * the body -- see that file -- but the caller does pass the slot index. */
void sub_0804E100(u16, int, u16);
/* Wave 33 (W33-D) RETYPES sub_0804E4CC from the `void(void)` placeholder above:
 * its body reads BOTH argument registers before writing them -- r0 is
 * sign-extended `lsls #0x10; asrs #0x10` and handed to sub_0801566C as the proc
 * id, and r1 is a pointer whose halfword at +4 is read, masked with 0xfc00 and
 * stored back. So the continuation family DOES take arguments and the comment
 * above is only right about sub_0804DA40, whose body has not been read. Both
 * call sites cast to `(u32)`, so nothing else moves. */
void sub_0804E4CC(s16, u16 *);
/* Wave 33, W33-D. sub_0804E214's continuation, and the same shape as
 * sub_0804E4CC above -- its prologue is `mov r8,r1` / `lsls r0,#0x10;
 * asrs r7,#0x10` / `bl sub_0801566C`, so both argument registers are live on
 * entry. Only `(u32)` casts of the symbol exist, so the declaration is free.
 * Wave 33 orchestrator: the second parameter is `struct Unk4E334 *`, not
 * `u16 *` -- src/decomp/c_0804E334.c is PROMOTED and defines it that way, and
 * proto_check rejects the disagreement. The tag is TU-local to that file, so it
 * is forward-declared here; since only `(u32)` casts exist, no call site moves. */
struct Unk4E334;
void sub_0804E334(s16, struct Unk4E334 *);

/* ---- wave 40 (W40-A): the 0x08003/0x08004 map-stamp block ----
 * Four of these are `void (void)` and it is settled from the DEFINITION rather
 * than guessed at a call site: sub_08003ED0, sub_08004724, sub_080040C8 and
 * sub_0800449C are each reached only by a bare `bl` with no argument register
 * written beforehand and no result read afterwards. sub_08003B8C is the sole
 * caller of all four and it is matched, so the contract has an oracle.
 *   sub_08003DC4's third argument is a `ldrb` of the map's +0x1432 terrain byte
 * widened at the call, so `int` is the weakest type that fits. sub_08003F44's
 * three are plain `int` -- that is what src/decomp/c_08003F44.c already
 * DEFINES, and it is repeated here only so its callers in this block stop
 * carrying a local prototype; the promoted definition is what wins.
 *   sub_0800401C is a rounded-rectangle stamp -- (cx, cy, w, h, value), the
 * fifth argument arriving at [sp, #0x28]. Parked; see work/sub_0800401C/. */
/* Wave 51 (W51-I): sub_08003B6C is the block's random helper -- every one of
 * the ~40 call sites in sub_080040C8/sub_0800449C is `sub_08003B6C(range,
 * base)` with the result used as a plain int.  The signature is not guessed:
 * src/decomp/c_08003B6C.c already DEFINES `int sub_08003B6C(int, int)` and
 * that definition wins; this only stops callers carrying a local prototype. */
int sub_08003B6C(int, int);
void sub_08003DC4(int, int, int);
void sub_08003F44(int, int, int);
void sub_08003ED0(void);
void sub_0800401C(int, int, int, int, int);
void sub_080040C8(void);
void sub_0800449C(void);
void sub_08004724(void);

/* ---- wave 13 (A1) ----
 * sub_08001D04 is a 32-byte linear search over the byte pairs at
 * gUnknown_084859E0: it walks two bytes at a time until `[p] == arg` and
 * returns `[p+1]`, or 14 if it hits the 0xFF terminator. `int(int)` and not a
 * narrow type either side -- the argument is compared with a bare `cmp` after
 * a `ldrb` of the table (so no promotion is visible on it) and sub_0800272C
 * feeds the result straight into `<< 12` with no `lsl/lsr` pair in between.
 *
 * sub_0803F6BC is the VRAM loader behind it: r2 is passed to sub_08011E54 as
 * a destination pointer (hence `void *`), r3 is only ever tested against zero
 * and gates the whole body, and r0 is the 6..17 switch selector. */
int sub_08001D04(int);
void sub_0803F6BC(int, int, void *, int);
/* Wave 42, W42-A. One argument (a map-tile halfword in sub_08002F1C, passed
 * with no narrowing at the call), and the result is handed straight to
 * sub_0802BD54's `u32` third parameter with no `lsl/lsr` pair after the `bl`,
 * so the return is word-width. Parameter left as the weakest `int`. */
u32 sub_0800C8A0(int);

/* The deferred-copy queue push, with 58 callers -- the widest fan-in in the
 * ROM after the proc API. It appends (src, dest, size) to gUnknown_0200B3B4[]
 * and returns the slot index, or -1 when the 0x30 slots are full and 0 when
 * gUnknown_030044D0 says to copy immediately; the value-returning epilogue
 * (`pop {r4,r5,r6}; pop {r1}; bx r1`) is what makes it non-void, and every
 * caller found so far discards the result. The third parameter is `u16`, not
 * `int`: the prologue narrows r2 with `lsls #16; lsrs #16` before the `strh`,
 * which is PROMOTE_MODE on a declared-narrow parameter and is not emitted for
 * an `int` that merely gets stored as a halfword. Neither pointer is `const`
 * -- both are stored into the queue record as plain words. */
/* Wave 32 (W32-B) RETYPES the return `int` -> `s16`, on the definition itself:
 * this wave promoted sub_08011E54's body and its epilogue narrows the queue
 * index with `lsls #0x10; asrs #0x10` before returning it. `int` drops that
 * pair and comes out 4 bytes short (84.0%). Its five siblings on the same
 * queue -- sub_08011D10, sub_08011D7C, sub_08011DE8, sub_08011EF0,
 * sub_08011F70 -- all return s16 with the identical narrowing, so the family
 * agrees. Byte-neutral for every existing caller: all eight discard the
 * result, so none of them re-narrows. */
s16 sub_08011E54(void *, void *, u16);
/* CpuFastSet wrapper: r0 indexes the (width, height) byte pair at
 * gUnknown_0848B780 + 4*r0 to get a tile count, r1 is a VRAM base and r2 a
 * tile index, and it copies `((w*h) & 0x3FF) * 0x20` bytes to
 * `r1 + ((r2 & 0x3FF) << 5)`. So the base is a `void *` and the index a plain
 * int; `pop {r0}` makes it void. */
void sub_0801F19C(int, void *, int);
/* gUnknown_0810BE60 / gUnknown_0810E820 by name and nothing else; both are
 * already promoted (matched) in src/decomp/c_08026190.c. */
u8 *sub_08026190(void);
u8 *sub_08026198(void);
/* `gUnknown_08499608[sub_08042DE0(a1) - 1][a2] << 2` over a 0x32-halfword row
 * -- a tile index, which is why sub_08002844 masks the result with 0x3FF.
 * `pop {r4}; pop {r1}` is the value-returning epilogue; both parameters arrive
 * bare and unnarrowed, so both are `int`. */
int sub_080261A4(int, int);

/* ---- wave 13 (A4) ----
 * All twelve already have a matched definition in src/decomp/ and none had a
 * prototype here yet; the signatures below are copied from those definitions
 * except the three that are still assembly.
 *
 * sub_0803CE28 takes two `int`s: `adds r6,r0,#0; adds r4,r1,#0` and then
 * `lsls r4,#5; adds r4,r4,r6; lsls r4,#1` with no sign- or zero-extension on
 * either, which is the `int` tell from the parameter rule in
 * docs/agbcc-codegen.md. sub_08023908 saves r0 bare (`adds r6,r0,#0`) with no
 * extension, same reading. sub_08023518 reads no argument register. */
void sub_08001D8C(void);
void sub_08001D9C(void);
void sub_08003934(void);
void sub_08003948(void);
void sub_080039BC(void);
void sub_080039D0(void);
void sub_08012BC8(u16 *, u16, u16, u16, u16, u16);
void sub_08023518(void);
void sub_08023824(void);
void sub_08023908(int);
void sub_0803CE28(int, int);
void sub_0803CEAC(void);

/* The sub_08023360 screen-init batch. sub_08011C68 is the VRAM blitter behind
 * sub_080116E8 and friends: r0 is a source of every pointer type in the tree
 * (`u16 *` tilemap buffers, the `u8 *` returns of sub_08026190/sub_08026198
 * and bare ROM blobs), so `const void *` is the only first parameter every
 * call site compiles against; r1 is always an absolute VRAM address and r2 a
 * byte count that the body narrows itself (`lsls r2,#0x10`), so it is not a
 * narrow parameter.
 *
 * sub_08035020 and sub_0801A57C take `u16`: both open `lsls r0,#0x10;
 * lsrs r0,#0x10` on an argument that survives a `bl`, which is PROMOTE_MODE on
 * a u16 parameter. sub_0803F80C, sub_08037150 and sub_08043834 save r0 bare
 * with no extension, so `int`. The rest read no argument register. */
void sub_08010FE0(void);
void sub_08011018(void);
void sub_080116E8(void);
/* THIRD PARAMETER RETYPED int -> u16 in wave 27, when the definition was
 * promoted. It is not a byte-neutral choice and the probe blind-spot warning
 * does NOT apply here -- `int` plus a `(u16)` cast at each use is a visibly
 * different function. With u16, PROMOTE_MODE's `lsls r2,#0x10` is emitted once
 * and every use folds its `lsrs #0x10` away: the `& 0x1f` test happens in the
 * shifted domain against 0x1f0000 (`movs #0xf8; lsls #0xd`), and `/2` and `/4`
 * come out as a bare `lsrs #0x11` / `#0x12`. With `int` the mask is a plain
 * imm8 AND that clobbers r1, which costs the dst pointer a spill into r4 and
 * turns `push {lr}` into `push {r4, lr}` -- 8 bytes and a different prologue.
 * sub_08011C90 follows its sibling; its own body is byte-identical either way.
 * Re-verified afterwards: the matched caller src/decomp/c_08023360.c, whose
 * `((u16)sub_080261A0() & 0x3ff) * 0x20` argument is the only non-constant one
 * in the ROM, still matches -- shorten_binary_op keeps that multiply in HImode
 * so the narrowing conversion is free. */
void sub_08011C68(const void *, void *, u16);
void sub_080128D0(void);
void sub_0801A57C(u16);
void sub_08022A34(void);
void sub_08023860(void);
/* Wave 36 (W36-A): the F043 quartet is PROMOTED byte-for-byte in
 * src/decomp/c_08023DCC.c but had no declaration anywhere, which blocked
 * sub_08023EEC (its only caller). Signatures copied verbatim from the
 * promoted definitions -- not re-derived. */
/* Wave 36 (W36-A). sub_08024C58 had no declaration and is called by BOTH
 * sub_080251D8 and sub_08024F20. Read off its own prologue, not guessed:
 * r0 is dereferenced as the gUnknown_030013D0 record (`ldr r3,[r5]` then
 * `ldrb [r3,#4]`, the unk00->unk04_0 chain c_08024DDC.c already models);
 * r1 is compared against 1 with no entry narrowing, so `int`; r2 opens
 * `lsls #0x18; lsrs #0x18`, so `u8`. */
void sub_08024C58(struct Unk030013D0 *, int, u8);
/* Wave 36 (W36-A). PROMOTED in src/decomp/c_08024A2C.c with no declaration
 * anywhere; signature copied verbatim from that definition, not re-derived.
 * It blocked sub_080251D8 and sub_08024F20. */
void sub_08024A2C(struct Unk030013D0 *, s16);
/* Wave 36 (W36-A). Six more PROMOTED-but-undeclared callees, blocking
 * sub_08024C58. Signatures copied verbatim from src/decomp/c_08043304.c,
 * c_08042C24.c and c_08042E2C.c. `struct Unk43304` is a FILE-LOCAL tag in
 * c_08043304.c; declaring it incomplete here keeps that file compiling
 * unchanged and callers cast onto it -- do not invent a body for it. */
struct Unk43304;
struct Unk08024ABCArg;
void sub_08024ABC(struct Unk08024ABCArg *, struct Unk08024ABCArg *, s16, u8);
/* Wave 36 (W36-A) RETYPES sub_08024ED8 from the file-local `struct Unk8024ED8 *`
 * that c_08024ED8.c invented ("the record itself is not modelled -- only +8 is
 * reached here") to the shared struct Unk030013D0, and the evidence is a CALL
 * SITE, which is the only place it could come from: sub_08024F20 hands it
 * gUnknown_030013D0 and gUnknown_030013B0, the same two records it has just
 * passed to sub_08024A2C, sub_08024ABC, sub_08024C58 and sub_08024E60. The two
 * models agree where they overlap -- Unk8024ED8's `s16 unk08` is
 * Unk030013D0's unk08, the field c_08024DDC.c writes and tests `>= 0`.
 *   It is BYTE-VISIBLE and that is how it was found: with two different tags
 * naming one symbol, agbcc emits a -fforce-addr .rodata word for one reference
 * and a plain inline pool word for the other, and the ROM shares ONE word
 * (0x08090A38) between them. Same tag on both references, one word, match.
 * c_08024ED8.c is edited to agree and re-verified byte-for-byte. */
void sub_08024ED8(struct Unk030013D0 *, struct Unk030013D0 *);
void sub_08024E60(struct Unk030013D0 *, struct Unk030013D0 *);
int sub_08043304(struct Unk43304 *);
int sub_0804334C(struct Unk43304 *);
int sub_0804338C(struct Unk43304 *);
int sub_08042CF8(int, int);
int sub_08042E64(int);
int sub_08042EBC(int);
void sub_08023DCC(u16, u16, u16, u16);
void sub_08023E14(u16, u16, u16, u16);
void sub_08023E5C(u16, u16, u16, u16);
void sub_08023EA4(u16, u16, u16, u16);
int sub_080261A0(void);
void sub_0802D2EC(void);
void sub_08035020(u16);
void sub_080354FC(void);
void sub_08035568(void);
void sub_08037150(int);
void sub_0803F80C(int);
void sub_08043834(int);
void sub_080546BC(void); /* src/decomp/c_080546BC.c */
/* sub_080315E8 ends `pop {r1}; bx r1`, so it returns a value -- its own
 * second argument, re-narrowed to u16. Its first two arguments arrive
 * `lsls #0x10`, so both are u16; the THIRD is never read by the body, but
 * sub_080339B0 passes 0x40 in r2, so the declaration really does have three
 * parameters and only the width of the unused one is open. */
u16 sub_080315E8(u16, u16, int);
/* Wave 42, W42-L: sub_08031948's third callee, and undeclared before this wave.
 * `bl sub_08031824` with no argument register set up in front of it and no use
 * of r0 after it, so void/void is the weakest model that fits. Its own body has
 * not been read -- if a later agent matches it and finds parameters, the call
 * site here cannot contradict that, because a wrapper passing nothing costs no
 * instructions either way. */
void sub_08031824(void);
void sub_08033930(void);
void sub_0803D48C(void);
void sub_08085AF4(void); /* src/decomp/c_08085AF4.c */
/* sub_08027B10 takes five arguments and narrows none of them: r0-r3 are
 * `adds rN, rM, #0` / `mov r8, r3` and go straight into the proc it starts
 * as words at +0x2c..+0x38, and the fifth (`ldr r1,[sp,#0x14]`) is handed to
 * Proc_Start as the parent, so it is a ProcPtr. sub_0802813C is a bare
 * `bx lr` table read of gUnknown_08499E38[gUnknown_02028E40] whose caller
 * compares it with 4. */
void sub_08027B10(int, int, int, int, ProcPtr);
void *sub_0802813C(void);
/* The sub_0802966C batch. sub_08015328 takes s16 (`lsls #0x10` and then BOTH
 * `lsrs` and `asrs` on the same value); sub_08015C30 and sub_08029868 take u8
 * (`lsls #0x18; lsrs #0x18`), and sub_0802966C passes the s16 gUnknown_03001FBC
 * to both, which is why the call site reads it with `ldrb`. sub_0802E7C8's
 * third argument is saved bare (`adds r5,r2,#0`) so it is a pointer, and its
 * result is narrowed by the CALLER, so the return is int and the caller's u8
 * is a local. Its other three are `int` and NOT the narrow types its own body
 * re-narrows them to: sub_0802966C reaches it with `ldrsh` on two u16 globals
 * and with `movs r3,#1; negs r3,r3` for -1, and neither is expressible through
 * a u16/u8 parameter (those give `ldrh` and `movs r3,#255`). The narrowing
 * inside the callee is its own cast. sub_080357E0 narrows all four
 * register arguments to u16 and forwards its fifth straight to
 * sub_08035760 as a pointer. */
void sub_08015328(s16);
void sub_08015C30(u8);
void sub_080294FC(void);
void sub_08029570(void);
void sub_08029868(u8);
void sub_0802D558(void);
int sub_0802E7C8(int, int, void *, int);
void sub_08034F8C(void);
int sub_080357E0(u16, u16, u16, u16, void *);
/* `bool8` on the sub_080116A0 precedent: sub_0802B91C tests the result with
 * `lsls r0,#0x18; cmp r0,#0`, and an int-wide return emits no such shift. Its
 * first argument is narrowed to u8 by the body and its third to u16; the
 * second is passed through unnarrowed and sub_0802B91C hands it the u16
 * gUnknown_030033EC. */
bool8 sub_0802706C(u8, u16, u16);
/* Wave 35 (W35-J). Classifies the map cell at (x, y): 0 when the cell is
 * empty, 1 when its unit is idle and 2 otherwise. Both parameters are u8 --
 * the prologue's `lsls #0x18` pair on each is PROMOTE_MODE's truncation, and
 * the y one fuses with the `* 2` halfword index into `lsrs #0x17`. The return
 * is u8: sub_0802B768 and sub_0802B7E8 both re-narrow it with `lsls #0x18`
 * before scaling it into gUnknown_0849A2A0, and both callers pass
 * gUnknown_03003130's u8 unk10/unk11 with a plain `ldrb`.
 *   NOTE the third argument at its own call to sub_0802706C carries an
 * explicit (u8) cast: the ROM truncates the army id with `lsls #0x18;
 * lsrs #0x18` where this prototype's u16 would emit `#0x10`. The prototype is
 * NOT wrong -- sub_0802B91C (promoted, byte-exact) passes an s16 there with no
 * truncation at all, which a u8 parameter could not produce. */
u8 sub_0802B6C8(u8, u8);
void sub_0802B750(void);
/* Wave 35 (W35-J). Five signatures copied VERBATIM out of byte-verified
 * definitions in src/decomp/ that had no declaration in this header -- the
 * same class W35-B hit on four callees. A symbol missing here is not evidence
 * it is underived. */
bool8 sub_0802CBC8(void);
void sub_080428F0(s16);
void sub_080272B4(void);
void sub_08028EE4(void);
void sub_0803446C(void);
/* Wave 35 (W35-J). The PARAMETER is invisible in the callee -- sub_08029490
 * never reads r0 -- but sub_080293C8 materialises `adds r0, r4, #0` before the
 * `bl`, and only a call site can prove an argument. It is the proc
 * sub_080293C8 itself received and later hands to Proc_End, so ProcPtr. The
 * callee is byte-identical either way. */
bool8 sub_08029490(ProcPtr);
void sub_0802B768(void);
void sub_0802B7E8(void);
/* Wave 35 (W35-J). SIXTEEN bits, not eight: sub_0802A258 truth-tests the
 * result with `lsls r0, #0x10; cmp r0, #0`, where a bool8/u8 return would give
 * `lsls #0x18`. The callee only ever returns the literals 0 and 1, so its own
 * body cannot tell u8 from s16 -- the width is visible at the call site and
 * nowhere else. u16 vs s16 is still open; nothing narrows or sign-extends it. */
s16 sub_0802A1E4(s16, s16);
/* Wave 35 (W35-J). Allocates a sub_080152EC slot and seeds it from a cursor
 * pair, a distance and a flag. The second parameter is NEVER read -- r1 is
 * overwritten with the literal 0 for sub_080152EC before anything touches it
 * -- but it cannot be dropped, because a3 arrives in r2 and a4 in r3. */
void sub_08029CB8(struct Unk802C57C *, u8, int, u8);
/* Wave 35 (W35-J). Only its ADDRESS is ever taken (sub_0802A7C4 hands it to
 * sub_0801F024), so nothing in this tree constrains the signature; `void
 * (void)` is a placeholder and the call site casts. NOT proven. */
void sub_0802AA78(void);
/* A `gUnknown_030030F0.unk02 = 1` / `= 0` pair, both `bx lr` leaves with no
 * argument register read. */
void sub_0803BD54(void);
void sub_0803BD60(void);

/* Three more of the same sprite-attribute setter family's helpers, all reached
 * from sub_0804D290/sub_0804DCA8.
 *
 * sub_080155C0 is the position setter: `lsl #16; asr #16` on all three
 * arguments inside the callee AND at every call site, which is s16 on both
 * ends. sub_0804BCB8 is the same shape with `lsl #16; lsr #16` -- four u16s.
 * sub_08057D44 is already matched in src/decomp/c_08057D44.c and returns
 * `gUnknown_08555450[a2][a1]`, a u32 its callers use as the base of an array
 * of halfword pairs. */
void sub_080155C0(s16, s16, s16);
/* All four parameters are narrow -- the prologue is four `lsls #0x10; lsrs
 * #0x10` pairs in argument order -- but PROMOTE_MODE zero-extends every
 * sub-word parameter regardless of signedness, so the prologue cannot separate
 * u16 from s16 and the CALLERS have to. Argument 3 is SIGNED, from two of them:
 * sub_0804F658 and sub_0804E584 both pass gUnknown_0855214C[side] and both emit
 * `movs rN, #0; ldrsh rD, [rB, rN]` -- a u16 parameter rewrites an `ldrsh` into
 * an `ldrh` and needs no zero register, so the register-offset form is only
 * reachable with a narrow signed parameter. The other three have no informative
 * call site yet (sub_0804D290, sub_0804DCA8 and sub_0804F18C all pass literals
 * for 3 and 4), so they stay u16 as the weaker choice. Retyping 3 is
 * byte-neutral for the two promoted callers, which pass a literal 0. */
void sub_0804BCB8(u16, u16, s16, u16);
/* Wave 32, W32-A: hoisted from src/decomp/c_08056E28.c, which defined the
 * record type locally and had no declaration. sub_0804BCB8 is its first C
 * caller: it fills a `struct Unk56E28` on its own stack and passes `&r`, which
 * is what moved the type into include/unknown-globals.h. */
void sub_08056E28(struct Unk56E28 *);
u32 sub_08057D44(int, int);
/* Two more sub_08015928 continuations, in exactly the position sub_0804DA40 and
 * sub_0804E4CC occupy above: sub_0804C6DC and its twin sub_0804CC38 hand each
 * one over as a bare pool word and nothing else in the ROM references either,
 * so only the address is used and `void(void)` comes from the registration
 * rather than from a call site.
 *
 * Wave 34 (W34-B) RETYPES both, the same correction wave 33 made to
 * sub_0804E4CC and for the same reason: the `void(void)` above was inferred
 * from the registration and the bodies refute it. Each reads BOTH argument
 * registers before writing them -- r0 is sign-extended `lsls #0x10;
 * asrs #0x10` and handed to sub_0801566C as the proc id, and r1 is a pointer
 * whose halfword at +4 is read, masked with 0xfc00 and stored back. They are
 * sub_0804E4CC's shape with a non-power-of-two tile pitch (0x30 and 0x38,
 * hence the __udivsi3/__umodsi3 pair where that one shifts). Both call sites
 * cast to `(u32)`, so nothing else moves. */
void sub_0804C8C8(s16, u16 *);
void sub_0804CE24(s16, u16 *);

/* ---- wave 13 (A2): sub_0802E4B4's callees ----
 * Every signature below is read off the CALLEE's own prologue, not off the
 * call site.
 *
 * sub_080242B0 / sub_0802D5E8 take `s16`: both open by narrowing r0 and r1
 * with `lsls #0x10; asrs #0x10` (the y one folded into `asrs #0xf` because it
 * is immediately doubled) before touching anything else, which is PROMOTE_MODE
 * on signed halfword parameters. sub_080242B0 returns a BYTE -- its only
 * caller narrows the result with `lsls #0x18` before the test.
 *
 * sub_08074320 / sub_08035584 / sub_080202A4 all take one gUnknown_08499594
 * ELEMENT pointer: each recovers the index with `p - gUnknown_08499594` and
 * multiplies it back up by the 0x0c stride. They are declared on
 * struct Unk030040D8 * because that is the type of the global every caller
 * hands them; see the note on struct Unk030040D8 in unknown-globals.h.
 *
 * sub_08022990's first two arguments are NOT narrowed at entry -- it forwards
 * r0/r1 untouched to sub_08013C00 -- so they are word-wide; only the third is
 * `u16` (`lsls #0x10; lsrs #0x10; mov r8, r2`). */
u8 sub_080242B0(s16, s16);
void sub_0802D5E8(s16, s16);
void sub_0802D458(void);
/* Wave 33, W33-E: RETYPED from `void` to `u8`. This is one of the six identical
 * null-guards the note on sub_080742FC below describes, and like them it ends
 * `lsls #0x18; lsrs #0x18` into a register that is then moved to r0 across an
 * unconditional branch -- keeping r0 live out, which agbcc does only for a
 * non-void return. Declared `void` it is 6 bytes off, exactly as that note says
 * for its five siblings. Both callers discard the result, so the retype cannot
 * disturb them.
 *
 * The parameter really is a gUnknown_08499594 record: the body computes
 * `a1 - gUnknown_08499594` as an exact division by the 12-byte stride (the
 * 5/17/257/65537 multiply chain for 0x55555555) and masks the index with 0xC0.
 * `struct Unk030040D8 *` is kept because sub_08074484's second parameter --
 * which this forwards a1 to unchanged -- already names that type, and the two
 * struct names are the same object under two names (see unknown-globals.h). */
u8 sub_08074320(struct Unk030040D8 *);
/* Wave 33, W33-E. The 0x08074000 block's one undeclared callee. sub_08074A28
 * passes it the already sign-extended `(s8)*proc->unk_2c` in r0 with no
 * narrowing in front of the `bl` -- so the parameter is word-wide, not `s8` --
 * and discards any result. */
/* NARROWED in wave 38 (W38-B): `s16`, not `int`. Its own prologue opens
 * `lsls r0,#0x10; lsrs r0,#0x10`, which is PROMOTE_MODE on a sub-word
 * parameter and cannot appear for an `int` one, and every later use of the
 * value re-sign-extends (`lsls #0x10; asrs #0x10`) -- the wave-11 reading, and
 * the u16-at-entry/s16-at-use pair that pins the sign. sub_080747FC confirms
 * it from the caller side: it holds the loop counter as a plain `int` and
 * narrows with `lsls #0x10; asrs #0x10` in front of the `bl`. The wave-33
 * reading above ruled out `s8`, which `s16` also satisfies -- sub_08074A28
 * passes an already sign-extended `s8`, so gcc emits no extra narrowing there
 * and c_08074A28.c is byte-identical under either spelling (re-verified). */
void sub_08074754(s16);
/* Wave 33, W33-E. All four are already PROMOTED (src/decomp/c_08074BDC.c and
 * src/decomp/c_08074C5C.c) and were never declared; these publish those
 * definitions unchanged. sub_08074C84 is the caller that needed them: it picks
 * the 4BDC/4C1C camera-relative pair or the 4C5C/4C70 absolute pair on its
 * fourth argument and clamps a scroll target with whichever it chose. */
int sub_08074BDC(int);
int sub_08074C1C(int);
int sub_08074C5C(int);
int sub_08074C70(int);
/* CORRECTION, wave 28 (W28-A): declared `void` here, and it returns a value.
 * The epilogue is `pop {r4}; pop {r1}; bx r1` -- the value-returning form -- and
 * the two arms set r0 to the sub_080355CC result and to 0 respectively. Its one
 * promoted caller (src/decomp/c_0802E4B4.c) discards the result, which is why
 * nothing caught it; re-verified byte-for-byte after the change. */
ProcPtr sub_08035584(struct Unk030040D8 *);
void sub_08024454(void);
/* Wave 38, W38-G. `u8 *` CONFIRMED, and the 28 call sites say more than the
 * width: every one of them passes `gUnknown_08499590 + K` -- the map pointer
 * plus a constant PLANE offset -- so the parameter is a plane base inside the
 * map descriptor, never an arbitrary buffer. Read off asm/code-0801D390.s and
 * src/decomp/c_0802E4B4.c: K = 0x2852 (sub_0802E2D0, sub_0802E4B4,
 * sub_08038D7C, sub_0803E808 and c_0802E4B4.c), 0x2D5A (sub_08038D7C's other
 * arm) and 0x193A (sub_080219AC). The one caller that does not spell the
 * addition at the call site, sub_0803E9F8, forwards its own pointer parameter
 * unchanged (`adds r0, r1, #0`), so it is not a counter-example.
 *   That is what fixes the whole shape: the function rebuilds the
 * gUnknown_03003340 row-pointer table as `a1 + rowOffset[y]` off the map's
 * +0x417A halfword table, i.e. gUnknown_03003340 is a per-PLANE row index that
 * is rebuilt whenever the active plane changes, not a fixed screen. K = 0x193A
 * corroborates independently: unknown-globals.h has +0x193A typed `s8` with -1
 * as its sentinel, and sub_0801FD9C reads gUnknown_03003340's rows through
 * `ldrsb` treating negatives as empty -- producer and consumer agreeing on the
 * signedness without either being able to see the other.
 *   The declaration is NOT `u8 **` and the ROM's three-level load chain is not
 * evidence of one: the pool word at 0x08090934 is agbcc's own -fforce-addr copy
 * of &gUnknown_08499590 (see include/unknown-globals.h), and the honest
 * spelling reproduces all three levels. */
void sub_0801F92C(u8 *);
void sub_080202A4(struct Unk030040D8 *);
void sub_08022990(int, int, u16);
void sub_08038C98(void);

/* ---- wave 13 (A2): sub_080345C8's gUnknown_030032D8 state-machine table ----
 * All eighteen are void(void): not one reads r0-r3 before writing it (every
 * body opens with a `bl`, a pool `ldr` or a `movs` into r0) and every one ends
 * `pop {r0}; bx r0`. That is also what makes the dispatch a bare `bl` per arm
 * with no argument setup. */
void sub_0802DC2C(void);
void sub_08034350(void);

/* ---- wave 25: the 0x08034xxx block's own callees ----
 * The void(void) group first: every one of these is reached by a bare `bl`
 * with no argument setup and its callers discard r0.
 */
void sub_0802150C(void);
/* Wave 32, W32-A: sub_0802150C's four callees, each already DEFINED in
 * src/decomp (c_0802BB74.c, c_08034FA4.c, c_08025E74.c, c_0803DE68.c) as
 * `void (void)` but never declared, because sub_0802150C is their first C
 * caller. Copied from those definitions, not inferred. */
void sub_0802BB98(void);
void sub_08034FA4(void);
void sub_08025E74(void);
void sub_0803DE68(void);
void sub_08021598(void);
void sub_080215B8(void);
void sub_080215D0(void);
void sub_0802FA64(void);
void sub_0805FD64(void);
void sub_08034394(void);
void sub_080343D8(void);
void sub_08034598(void);
void sub_08034780(void);
void sub_08034838(void);
/* Wave 41, W41-B: copied from the promoted definition in
 * src/decomp/c_08020824.c, not inferred -- sub_08020864 is its first C caller.
 * The `int` return is the definition's; sub_08020864 narrows the result to u16
 * itself before comparing it with 2, which is a u16 local at the call site and
 * not evidence about this signature. */
int sub_08020824(u16, u16);
/* Wave 49, W49-I: both copied VERBATIM from the byte-verified definitions in
 * src/decomp/c_08020864.c, which had no declaration anywhere. sub_08020984 is
 * the first C caller of either; it truth-tests both results `lsls #0x18` wide,
 * which is a (u8) at the call site and not evidence about the return type. */
int sub_08020864(u16);
int sub_080208C8(int);
/* Wave 49, W49-I: copied VERBATIM from the byte-verified definition in
 * src/decomp/c_08020634.c, which had no declaration anywhere. */
void sub_08020634(u8 *, u8 *);
/* sub_08034890 loads gUnknown_03003FC0.unk02 with `ldrb` and passes it with no
 * further narrowing, which is byte-identical for `int` and `u8`, so `int` is
 * the weakest model. */
void sub_0802163C(int);
/* sub_08034290 and sub_080342BC both pass a `movs`-sized 0. */
void sub_080638D0(int);
/* Definitions promoted in src/decomp/c_08034380.c and c_08034400.c -- these
 * declarations must agree with those. */
int sub_08034380(u8 *);
void sub_08034400(u8 *, u8 *);
/* Returns s16: sub_08034394 re-narrows the result with `lsls #0x10; asrs #0x10`
 * before comparing it against -1. The first parameter is `void *` and not a
 * struct pointer -- sub_08031790 and sub_08033678 pass gUnknown_03004400 where
 * sub_08034394 passes &gUnknown_030046C0, so no one struct type covers it. The
 * second is a predicate run over the object; sub_08034394 passes
 * sub_08034380 and the other two call sites pass a `movs`-sized 0. */
s16 sub_080309AC(void *, int (*)(u8 *));
/* Wave 43 (W43-C). The same shape as sub_080309AC above, and typed from the
 * same evidence. Returns s16: its ONE caller, sub_080336BC, re-narrows the
 * result with `lsls #0x10; asrs #0x10` before comparing it against -1, and the
 * callee's own body confirms nothing narrower fits -- the two exits produce
 * `movs r0,#1; rsbs r0,r0,#0` (-1) and an unextended `ldrb`, so u8 is
 * impossible (it would truncate the -1 the caller tests for) and s8 would have
 * made the caller shift by 24.
 *   The second parameter is a predicate run over the buffer, called through
 * `bl _call_via_sl`. Declared `int (*)(u8 *)` to agree with sub_080309AC, but
 * note the disagreement: sub_08030B00 narrows the predicate's result with
 * `lsls r0,#0x18` before testing it, which argues for a u8/bool8 return. This
 * is byte-neutral here because the sole caller passes a `movs`-sized 0, so it
 * is recorded rather than acted on -- a real function pointer reaching either
 * of these two would settle it. */
s16 sub_08030B00(void *, int (*)(u8 *));
/* ---- wave 31, W31-B: the 0x08033000 block's undeclared callees ---- */
/* Nullary and void: it reads no argument register before writing one and ends
 * `pop {r0}` off the frame. It is the big "reset every per-army table" routine
 * behind gUnknown_0849B018 / gUnknown_0849B01C. */
void sub_0802F03C(void);
/* Wave 33, W33-B: sub_0802F348's two other reset helpers, both nullary void
 * (`bl` with no argument setup). */
void sub_0802F23C(void);
void sub_0802F28C(void);
/* Wave 33, W33-B: matched. The link-record reset; nullary void. */
void sub_0802F348(void);
/* Wave 33, W33-B: sub_08030CCC's predicate; returns u8 (caller re-narrows the
 * result with lsls/lsrs #24), nullary. */
/* Wave 33 orchestrator: `int`, not `u8` -- src/decomp/c_08030D1C.c is PROMOTED
 * and returns literal 0/1 as `int`. Its one caller (sub_08030CCC) assigns into
 * its own local, so the caller carries no evidence about the return width. */
int sub_08030D1C(void);
/* Wave 33, W33-B: the blend-setup tail sub_08030F60 calls; nullary void
 * (matched, c_08030F20.c). */
void sub_08030F20(void);
/* TWO parameters and an `int` result (-4 on the early-out, 5 on the normal
 * path); every caller so far discards it. The first is `u16 *` and not an
 * opaque handle -- the body spins on `ldrh r0,[r2]; strh r0,[0x0400012A]` until
 * the register reads back what it wrote, i.e. it is the SIOMLT_SEND source.
 * The second is signed: `cmp r4,#0; bge`. */
int sub_0802F8FC(u16 *, int);
/* Wave 41 (W41-C). The block 0x08030 link-state entry points, all from their
 * own definitions (now matched). The first five are `void (void)`: none reads
 * r0-r3 before writing it, and each ends `pop {r0}; bx r0` with nothing setting
 * r0 on any path.
 *   sub_08030838 takes ONE pointer and it is the same `struct
 * Unk08090CD8Entry *` the ring holds: the body copies unk00/unk01/unk02/unk04
 * and then unk04 payload bytes out of it, field for field, into
 * gUnknown_0849B018->unk12c[]. Nothing narrows the parameter, and it is
 * dereferenced immediately, so it is a pointer and not a handle. */
void sub_08030584(void);
void sub_08030600(void);
void sub_08030670(void);
void sub_080306E4(void);
void sub_08030768(void);
void sub_08030838(struct Unk08090CD8Entry *);
/* Wave 41, W41-D. Copied VERBATIM from the promoted definition in
 * src/decomp/c_0802ECEC.c -- it arms timer 3 with a reload of -cycles and
 * 0xc3. sub_0802F8FC is its first cross-file caller and passes the
 * gUnknown_08090C44 entry the gUnknown_03000578 cursor selects, which is what
 * retyped that table from `void *[]` to `int []`. */
void sub_0802ECEC(int);
/* Declared WITHOUT a prototype, like sub_0801C240: its single parameter is a
 * pointer to the link-session record at gUnknown_03003F70, whose +0x14..+0x4b
 * this function walks, and whoever promotes it will want to name that struct.
 * It RETURNS a status code (0x71, 5, a xor of the SIOCNT bits, ...) which all
 * three callers discard. */
int sub_08062FF4();
/* All five are already DEFINED as `void f(void)` in src/decomp (c_08031430.c,
 * c_08031CE4.c, c_08031E6C.c, c_08032454.c, c_08032D60.c) and none had a
 * declaration -- sub_08033150 is the first caller from another unit. */
void sub_08031430(void);
void sub_08031CE4(void);
void sub_08031E6C(void);
void sub_08032468(void);
void sub_08032D60(void);

/* ---- wave 31, W31-B: the 0x0801A000 block's undeclared callees ---- */
/* Nullary and void, and both halves read off its own body: it opens with
 * `ldr r4, =gUnknown_0200CD0C; bl sub_0808AB8C` -- no argument register is read
 * before being written -- and ends `pop {r0}; bx r0`. Defined this wave in
 * src/decomp/c_0801AFF4.c. */
void sub_0801AFF4(void);
/* Nullary; returns 0 or 1 in r4. `bool8` and not `int`: sub_0801AFF4 re-narrows
 * the result with `lsls r0, r0, #0x18` before testing it, which an int-wide
 * return never produces. */
bool8 sub_0808AB8C(void);
/* ONE parameter and void: the prologue is `str r0, [sp]` and the body tests
 * that slot with `cmp r0, #0` on two paths, and the epilogue is `pop {r0}`.
 * sub_0801A79C passes a literal 0. */
void sub_0801B2FC(int);

/* ---- wave 31, W31-B: the 0x0800B000 and 0x0808B000 blocks ---- */
/* The fourth member of the group at unknown-functions.h's sub_0800BF78 note --
 * same `(x, y)` cell pair as sub_0800BC98 beside it, and sub_0800BEB8 tests its
 * result with a bare `cmp r0, #0`, so `int`. */
int sub_0800BCD0(int, int);
/* Wave 31, W31-B. Nullary and its result discarded by its one caller.
 *
 * NOTE its epilogue is `pop {r4, r5, pc}`, not agbcc's usual `pop {rN}; bx rN`,
 * and sub_0808BBA4 next door ends `pop {pc}` the same way. Those two are the
 * only functions in the 0x0808Bxxx block with that epilogue -- sub_0808B184,
 * sub_0808B304 and sub_0808BB0C, all matched in wave 31, use the normal
 * interworking return. So this pair comes from a translation unit built
 * WITHOUT Thumb interworking, and sub_0808BBA4 cannot match on size until that
 * is modelled: its body is byte-identical up to the epilogue and 2 bytes short
 * there. See work/sub_0808BBA4/sub_0808BBA4.c. */
void sub_0808BB58(void);
/* Wave 50, W50-F: reads no argument register (its whole body is a
 * gUnknown_03000F80 once-only guard around sub_0808BB58) and its one call site,
 * sub_08052EA8, sets none up. void(void). */
void sub_0808BBA4(void);

/* ---- wave 31, W31-B: the 0x08053000 extension block's callees ---- */
/* All four take ONE `u16` and return nothing, and both halves are read off
 * their own bodies: every prologue is `lsls r0, r0, #0x10; lsrs rN, r0, #0x10`
 * -- PROMOTE_MODE on a declared-narrow parameter, not a cast at a use, since
 * nothing copies r0 first -- and every epilogue is `pop {r0}`. They are the
 * per-channel step functions sub_08053F50 and sub_08053F90 drive with 0 and 1,
 * the same shape as the already-declared sub_08053660. */
void sub_08053FBC(u16);
void sub_0805414C(u16);
void sub_08054278(u16);
void sub_08054488(u16);
/* Wave 37, W37-B -- block 0x08054, the cutscene player's per-side steps. Every
 * one of these opens with `lsls rN,#0x10; lsrs rN,#0x10` on each argument
 * register it reads, so the parameters are 16 bits wide; PROMOTE_MODE makes
 * that prologue identical for s16, and nothing at any call site or in any body
 * signs one, so u16 is the weakest fit. Arity is the count of those narrowings,
 * corroborated at the call sites in sub_08053F50 and sub_08054E8C.
 *   sub_08054E8C is the only one that returns anything: its four cases assign
 * 1, 2 or 3 into the same pseudo that holds the fourth parameter and the
 * default path returns that parameter unchanged, so the return type is the
 * parameter's. Nothing matched calls it, so the width is not caller-confirmed.
 *   sub_08054C5C and sub_08057AE8 read no argument register before writing it
 * (both load a global as their first act) and both results are discarded at
 * sub_08054C04's two call sites. */
void sub_080541F0(u16, u16);
void sub_080542EC(u16, u16);
void sub_080543E0(u16, u16);
void sub_08054500(u16, u16);
void sub_08054BA0(void);
void sub_08054C5C(void);
u16 sub_08054E8C(u16, u16, u16, u16);
void sub_08054EE0(u16, u16);
void sub_08054F50(u16, u16);
void sub_08055004(u16, u16);
/* Wave 51, W51-H. Both narrow r0 and r1 with `lsls #0x10; lsrs #0x10` at entry,
 * so both parameters are 16 bits wide. The ARGUMENT ORDER IS OPPOSITE between
 * the two: sub_08055768 takes (side, count) -- its r0 is what indexes
 * gUnknown_03004580 with `lsls #4` -- and sub_08055A38 takes (count, side),
 * with the side in r1. Their only callers are sub_08055654 and sub_08055940
 * respectively, and neither uses a result, so `void` is the weakest fit rather
 * than a proof. */
void sub_08055768(u16, u16);
void sub_08055A38(u16, u16);
void sub_08057AE8(void);
/* Wave 37, W37-B. Already DEFINED in src/decomp/c_0805198C.c and
 * src/decomp/c_08050F24.c with exactly these signatures -- copied from the
 * definitions rather than inferred, so the declarations cannot drift. */
void sub_0805198C(u16, u16);
void sub_08050F24(u16, u16);
/* Wave 37, W37-B. Read off sub_08054C04's one call site, which supplies
 * gUnknown_03004580[0][3] and gUnknown_03004580[1][3] with bare `ldrh`s, and
 * confirmed against the callee's own two `lsls #0x10; lsrs #0x10` narrowings.
 * The result is discarded there; nothing else matched calls it. */
void sub_0804B744(u16, u16);
/* Nullary and void: neither reads an argument register before writing one --
 * sub_0804B330's prologue narrows r0, so it takes one, while sub_08057BDC
 * opens with a pool `ldr` -- and both end `pop {r0}`. */
void sub_0804B330(u16);
void sub_08057BDC(void);
/* The four battle-animation entry points sub_0805DCA4 and sub_0805DFB8 select
 * between. All four are nullary and void on their own bodies: each opens with a
 * pool `ldr` and none reads an argument register before writing one, and every
 * epilogue is `pop {r0}`. Their call sites set up no arguments either. */
void sub_08059760(void);
void sub_08059824(void);
void sub_080598BC(void);
void sub_08059978(void);
/* Nullary and void; defined this wave in src/decomp/c_08053F0C.c and declared
 * here because sub_0805316C calls it from another unit. */
void sub_08053F0C(void);
/* Already DEFINED with these signatures in src/decomp/c_0804B3CC.c,
 * c_08053860.c and c_08053660.c; none had a declaration, and the four
 * functions matched in this block are their first cross-unit callers. */
void sub_0804B3CC(void);
void sub_08053860(void);
void sub_08053BB8(void);
void sub_08053660(u16);
/* A text-width helper: sub_08034A44 turns the result into the centred x of a
 * 240-pixel line, `(0xf0 - len * 8) / 2`, and the `/ 2` is a bare `lsrs #1`, so
 * the value is UNSIGNED -- a signed halving would have been the
 * `lsr #31; add; asr #1` triple. */
u32 sub_0808B6B0(const char *);
/* More void(void) callees of the 0x08034xxx block, all reached by a bare `bl`
 * with the result discarded. */
void sub_08028CF4(void);
void sub_08037F80(void);
/* WAVE 28 (W28-B): sub_080742FC and sub_08074460 return `u8`, not `void`.
 * Both end `lsls r0,#0x18; lsrs r0,#0x18` then `pop {r1}; bx r1` -- the shift
 * pair is the declared-narrow return's own epilogue narrowing and the `pop`
 * into r1 rather than r0 is agbcc keeping r0 live out, which it does only for
 * a non-void return.  Declared `void` each is 6 bytes off.  They are two of
 * six identical null-guards around one slot of struct Unk08074584. */
u8 sub_080742FC(void);
u8 sub_08074410(int, struct Unk030040D8 *);
u8 sub_0807443C(void);
const struct Unk08074584 *sub_08074584(void);
/* Already PROMOTED (src/decomp/c_08035000.c) and never declared here;
 * signature copied verbatim from that definition. */
const struct Unk085C77A0 *sub_08035000(int);
u8 sub_08074484(u8 *, struct Unk030040D8 *, int);
/* Already PROMOTED (src/decomp/c_08074570.c) and never declared here until
 * wave 38 (W38-E); signature copied verbatim from that definition. Its only
 * caller is sub_08074484, which does `adds r4, r0, #0` right after the `bl`
 * and keeps walking the same 8-byte-stride record array through the result --
 * so the pointer return and the `u8 *` parameter are both fixed by that use. */
u8 *sub_08074570(u8 *);
void sub_0802817C(void);
u8 sub_08074460(void);
/* sub_08034DB0 tests the result with a BARE `lsls #0x18` -- flags only, no
 * `lsrs` -- which is the truth-test shape of a byte-wide return. */
bool8 sub_0803B628(void);
/* sub_08034DCC reaches it as `ldr r0, =gUnknown_030033EC; ldrb r0, [r0]`. That
 * global is a u16, so the `ldrb` is a truncation folded into the load, which is
 * what a `u8` parameter costs and an `int` one would not. */
/* It returns `int`, not `void` (wave 28, W28-B) -- and never executes a
 * `return` statement: every arm of its switch ends in a call to a void
 * function and the value in r0 is whatever that call left there. The tell is
 * the epilogue: `pop {r1}; bx r1` rather than `pop {r0}; bx r0`. agbcc pops
 * the return address into r1 precisely when r0 is live out, i.e. when the
 * function has a non-void return type, and declaring it void is 2 bytes off.
 * This is an implicit-int definition with no return, not a value anyone uses:
 * its callers discard r0. */
int sub_08043DAC(u8);
/* Centred x for `s` on a 240-pixel line. Returns `int`: its one caller narrows
 * the result with `lsls #0x10; lsrs #0x10`, which is also exactly what passing
 * an int to sub_080119A0's `u16` first parameter costs, so the shift pair is
 * not evidence of a narrow return. */
int sub_08034A44(const char *);
void sub_08034938(void);
void sub_080349E4(void);
void sub_08034AF8(void);
void sub_08034C90(void);
void sub_08034CA4(void);
void sub_08034CB8(void);
void sub_08034CD4(void);
void sub_08034D18(void);
void sub_08034DB0(void);
void sub_08034DCC(void);
void sub_08034DF8(void);
void sub_08034EA4(void);
void sub_08034ED0(void);
void sub_08034EF0(void);
void sub_08034F1C(void);
void sub_0806171C(void);
/* `int`, NOT `s8`, and the call site is the proof: sub_080345C8 tests the
 * result twice with a bare `cmp r0, #0` and an `s8` return puts a
 * `lsls r0, #0x18` in front of each one (PROMOTE_MODE on the return value).
 * Its own body is byte-identical under either -- `ldrb; lsls #0x18; asrs #0x18`
 * is the s8 MEMBER read, not the return conversion -- so src/decomp/c_08034F6C.c
 * was retyped to agree; it still matches. */
int sub_08034F6C(void);

/* ---- wave 13 (A2): sub_080355CC's callees ----
 * sub_0803649C scans gUnknown_03003124[0..2] for a free slot and returns the
 * index or -1; `s16` because both returns are narrowed (`lsls #0x10;
 * asrs #0x10`) inside it and the caller narrows nothing.
 * sub_08035AE8 and sub_08035B00 both end on an `ldrsh`, so both return s16;
 * their parameters are read off their own prologues (`lsls #0x10; asrs #0x10`
 * = s16, `lsls #0x10; lsrs #0x10` = u16).
 * sub_0801C210's three parameters are likewise its own: a bare pointer copy,
 * then `lsls #0x10; lsrs #0x10` and `lsls #0x18; lsrs #0x18`. It returns
 * sub_0801C6E8's allocation or NULL.
 * sub_0801C4D4's second parameter is word-wide -- it copies r1 with a bare
 * `adds r4, r1, #0` and uses it as `lsls r4, #1` -- so the `ldrsb` at the call
 * site is the s8 STRUCT FIELD being read, not a narrow parameter.
 * sub_08035B3C returns the compressed graphic sub_080355CC hands to
 * Decompress, so `u8 *` (NOT const -- Decompress takes `u8 *`). */
s16 sub_0803649C(void);
void *sub_080364C4(void);
void *sub_08035B68(u16);
struct Unk0801C210 *sub_0801C210(void *, u16, u8);
void sub_0801C4D4(struct Unk0801C210 *, int);
/* Four parameters, read off sub_080272C4 / sub_08027428 (its only callers) and
 * its own prologue: r0 is compared against 0x7f with a SIGNED `ble`, r1 is
 * dead until r2 is copied over it, r2 is the value that copy carries, and r3
 * goes straight to sub_0801C4D4's `struct Unk0801C210 *` first parameter.
 * Nothing narrows any of them at entry, so `int` for the first three, and the
 * callers pass three words out of the proc plus the sub_0801C210 result
 * (wave 20, W20-C). */
void sub_08027560(int, int, int, struct Unk0801C210 *);
s16 sub_08035AE8(s16);
s16 sub_08035B00(u16);
u8 *sub_08035B3C(ProcPtr);
void sub_080359A4(ProcPtr);
/* sub_080359A4's own callees. All three are declared `int`-wide on the CALLER
 * side: sub_080359A4 hands them `(v + 8) / 16` expressions and struct fields
 * with no narrowing anywhere, and a u16 parameter would have put
 * `lsls #0x10; lsrs #0x10` in front of every one. Their own prologues do
 * narrow, which under the wave-11 reading is a cast at a use rather than
 * PROMOTE_MODE. sub_080255F4 returns a BYTE -- the caller tests it with
 * `lsls r0, #0x18`. */
/* RETYPED in wave 43 (W43-B) from `(int, int)` to `(s16, s16)`, on
 * callee-side evidence the note above could not see. sub_080358C4's entry is
 * `lsls r1,#0x10; lsrs r7,#0x10` for the SECOND parameter -- a ZERO-extension,
 * which no `int` parameter ever produces -- and every later use of it
 * sign-extends (`lsls #0x10; asrs #0x10`). That pair is PROMOTE_MODE read
 * forwards: agbcc zero-extends a sub-word parameter at entry regardless of
 * signedness, and the sign is in the shift pair at the use. The FIRST
 * parameter shows only `lsls #0x10; asrs #0x10` because its use is in the
 * entry block, so combine folds the promotion into it; the second parameter's
 * first use is past two `if` blocks, and combine does not cross a basic block.
 * Spelling it `int` costs a `u16` local to carry the zero-extended copy, and
 * that extra pseudo spills three more callee-saved registers than the ROM
 * pushes -- measured with compile_probe, which is how the retype was found.
 * sub_080359A4 is the only caller and is still `asm`, so no promoted file
 * depends on the old spelling. */
void sub_080358C4(s16, s16);
/* CORRECTED in wave 18 (W18-C): sub_080255F4 is
 * `u8 (struct UnitRecord *, s16, s16)`, not `(int, int, int)`.
 *   - Parameter 0 is a POINTER, not an int: the body NULL-tests it
 *     (`cmp r3,#0`), reads `[r3,#1]`, and takes `r3 - gUnknown_08499594` as an
 *     exact pointer difference by 12 (the *0xAAAAAAAB;asr#2 magic). Its caller
 *     sub_0802571C passes `&gUnknown_08499594[id]`.
 *   - Parameters 1 and 2 are 16 bits: the prologue carries `lsls #0x10;
 *     lsrs #0x10` on BOTH, which is PROMOTE_MODE and which an `int` parameter
 *     never produces. They are SIGNED because each is re-narrowed with
 *     `lsls #0x10; asrs #0x10` at its use.
 * The wave-15 comment above inferred `int` from sub_080359A4 passing
 * `(v + 8) / 16` with no narrowing, on the grounds that "a u16 parameter would
 * have put `lsls #0x10; lsrs #0x10` in front of every one". That is true of
 * u16 and NOT of s16: the argument there is `ldrsh` + 8 + `asrs #4`, which
 * gcc's num_sign_bit_copies puts at 20 sign bits, so the s16 conversion is
 * provably a no-op and combine deletes it. The other caller passes two `ldrb`s,
 * which is a no-op for the same reason. Neither call site was ever evidence
 * against s16 -- READ AN ABSENT NARROWING AS PROOF ONLY WHEN THE VALUE'S RANGE
 * DOES NOT ALREADY FIT. Neither caller is promoted. */
u8 sub_080255F4(struct UnitRecord *, s16, s16);
/* Wave 29 (C) retyped the return from `int` to `u8`, on two independent
 * readings that agree. CALLEE side: sub_0801C254's own tail is
 * `bl sub_0801C2DC; lsls r0,#0x18; lsrs r0,#0x18` before the shared epilogue,
 * and its early-out arm sets `movs r0,#0` -- a byte-wide result being
 * normalised on the way out. CALLER side: sub_0803F4C8 (wave 29) truth-tests
 * the result with `lsls r0,#0x18; cmp r0,#0`, which is the re-narrowing agbcc
 * puts at every call of a narrow-returning callee; an `int` return there is a
 * bare `cmp r0,#0`. The only promoted caller, src/decomp/c_08075C98.c, DISCARDS
 * the result, so the change is byte-neutral for it -- rechecked with
 * try_match. */
u8 sub_0801C254(struct Unk0801C210 *, int, int);

/* ---- wave 18 (W18-C): the 0x080255F4 / 0x080257C0 neighbour-scan pair ----
 * All four return a byte -- every call site tests the result with a bare
 * `lsls r0, #0x18; cmp r0, #0` and sub_08025744 re-narrows sub_08026F5C's
 * result before returning it. sub_08025598 and sub_08025744 have the SAME
 * body shape (both open `lsls #0x10; asrs #0x10` on r0 and `lsls #0x10;
 * asrs #0xf` on r1, the latter being the `y * 2` row index folded into the
 * sign extension) and they still take DIFFERENT parameter widths. Each width
 * was read off the matched caller, which is the only place it is visible:
 *
 *   sub_08025598(s16, s16) -- sub_080255F4 narrows `x - 1` / `y - 1` with
 *     `lsls #0x10; asrs #0x10` in front of the call and leaves the bare
 *     `x` / `y` alone. That asymmetry IS the s16 conversion: its operands come
 *     from `(s16)` parameters, so num_sign_bit_copies proves the bare values
 *     already fit and drops those two, and does not for the +-1 forms.
 *
 *   sub_08025744(int, int) -- sub_080257C0's four arguments are all
 *     `ldrb`-derived, so an s16 conversion would be elided at ALL FOUR sites
 *     and the usual narrowing readout is blind here. The width still shows,
 *     as an ORDERING difference: with `int` the two calls whose second
 *     argument carries the arithmetic emit `subs r1, r5, #1; adds r0, r4, #0`,
 *     which is the ROM; with `s16` they come out in argument-number order.
 *     sub_08025744's other caller sub_08020DBC has the same tail and the same
 *     order. WHERE A NARROWING WOULD BE ELIDED, ARGUMENT-SETUP ORDER IS STILL
 *     A READOUT.
 *
 * sub_08026F5C takes s16 for the same reason its sibling sub_08026FD0 does:
 * the body is `(s16)a >> 6` (`lsls #0x10; asrs #0x16`) and sub_080257C0
 * narrows a pointer difference to s16 to pass it. */
bool8 sub_08026F5C(s16);
u8 sub_08025598(s16, s16);
u8 sub_08025744(int, int);
u8 sub_080257C0(u16);

/* ---- wave 13 (A2): sub_08040640's callees ----
 * sub_08026100 takes three word-wide arguments (`adds r4, r0, #0;
 * adds r5, r2, #0` and a signed `cmp r4, #0; blt`, no narrowing) and is void
 * (`pop {r4,r5,r6}; pop {r0}`).
 * sub_0801C70C takes SIX -- four in registers and two on the stack. Only the
 * last is narrow: it arrives as `ldr r1, [sp, #0x1c]; lsls #0x10; lsrs #0x10`
 * on its way to sub_0801C210's u16 second parameter. The first is `const void *`
 * so the ROM blobs its callers pass (gUnknown_08111D94 here, gUnknown_0810A3E8
 * and gUnknown_0810AFC8 from sub_0803F128) do not have to drop const; it is
 * forwarded to sub_0801C210, whose own first parameter will need widening to
 * match when that function is written. Returns a value, discarded here. */
void sub_08026100(int, int, int);
int sub_0801C70C(const void *, int, int, int, int, u16);
/* Wave 32 (W32-B): scans the gUnknown_02028360 record array for the first entry
 * whose 4-bit field at bits 6..9 of +0x02 equals the argument, returning that
 * entry or NULL; the walk stops when bit 0x3c0 of the same halfword is clear.
 * Same record type and same bitfield sub_0803DE94 and the 0x0803Exxx callers
 * use. Returns the pointer in r0 with a `movs r0,#0` miss path, so it is not
 * void. */
struct Unk02028360 *sub_0803E354(int);
/* Wave 32 (W32-B): DEFINED in src/decomp/c_0803FF2C.c and never declared until
 * sub_0803FF48 gave it a C reference. See that file for the ProcPtr retype. */
void sub_0803FF2C(ProcPtr);

/* ---- wave 13 (A8) ----
 * sub_08053614(procId, palette): the id is SIGNED (`lsls #0x10; asrs #0x10` at
 * entry, and it is compared against -1) and the palette is UNSIGNED
 * (`lsls #0x10; lsrs #0x10`), which is the entry-narrowing readout and not a
 * guess. sub_0805741C's single argument is unsigned by the same test.
 * Both return nothing -- each ends `pop {r0}; bx r0` after a `pop` of the
 * callee-saved set, and their 41 call sites in sub_08053860 / sub_08053BB8
 * discard r0. */
void sub_08053614(s16, u16);
void sub_0805741C(u16);

/* The two halves of the "poke the eight neighbours of (x, y)" pair
 * sub_080081E0/sub_080083E0. Both arguments arrive as bare `adds rN, r0, #0`
 * with no narrowing on either end, so `int` on both, and sub_080015E4's result
 * is only ever `cmp r0, #0` -- an int return, since a bool8 one would carry
 * `lsl #24; lsr #24` at the eleven call sites. sub_08007F14's third argument is
 * a tile value out of the +0x0A22 u16 array, compared against 0x2a and 1 in the
 * callee with no narrowing. */
int sub_080015E4(int, int);
void sub_08007F14(int, int, int);

/* ---- wave 50 (W50-E) ----
 * Four callees of sub_08000E48 that were already DEFINED in src/decomp and
 * only ever missing a declaration. Each signature is copied from the promoted
 * definition, not inferred -- a definition wins over a weaker guess, and
 * disagreeing here is a `conflicting types` build failure rather than a
 * mismatch. sub_08001124 is the block clear (`u8 *`, so sub_08000E48's two
 * calls cast); sub_08007328 returns int and every caller so far discards it. */
void sub_08001124(u8 *, int);
void sub_08003910(void);
int sub_08007328(void);
void sub_080078E4(int, int);
void sub_08002EB4(void);

/* ---- wave 13 (A6) ----
 * The tile-edit helpers sub_0800B244 drives, all keyed by the same (x, y) cell
 * coordinate pair. Every argument arrives as a bare `adds rN, r0, #0` with no
 * narrowing at either end, so `int` throughout; sub_08001158 is already
 * defined that way in src/decomp/c_08001158.c and is only missing a prototype.
 * sub_0800B1FC's result is switched on against -1, 0 and 1, so it is a signed
 * int and not a bool; sub_0800119C's and sub_0800AFCC's are only ever
 * `cmp r0, #0`, and sub_08001704's and sub_08001A04's are forwarded straight
 * into sub_08001158's third parameter, which fixes them as int. */
void sub_08001158(int, int, int);
int sub_0800119C(int, int, int);
int sub_08001704(int, int, int);
int sub_08001A04(int, int, int);
int sub_0800AFCC(int, int);
int sub_0800B1FC(int, int);

/* ---- wave 14 (C) ----
 * Four more cell predicates on the same (x, y) key, all read off their own
 * prologues rather than guessed from a call site: each opens with a bare
 * `adds rN, r0, #0` / `adds rN, r1, #0` pair and no narrowing, so both
 * parameters are `int`, and each ends `pop {r1}; bx r1`, so all four return a
 * value. `int` on the return for all four: sub_0800A3D4 and sub_0800BF78 test
 * every one of them with a bare `cmp r0, #0` and no `lsls #0x18`, and
 * sub_0800A798 returns -1 on its first path (`movs r0,#1; rsbs r0,r0,#0`), so
 * it is signed. sub_080094EC is already defined as int(int, int) in
 * src/decomp/c_080094EC.c and was only missing a prototype; sub_08009B84's
 * result is forwarded into sub_08001158's third parameter, which is int. */
int sub_080094EC(int, int);
int sub_08009B38(int, int);
int sub_08009B84(int, int);
int sub_0800A798(int, int);

/* Three more on the same key, from sub_0800BF78. Same `adds rN, r0, #0` entry
 * reading for the parameters. The epilogues split: sub_0800BC98 ends
 * `pop {r4}; pop {r1}; bx r1` and its one call site tests the result with a
 * bare `cmp r0, #0`, so `int`; sub_0800C124 and sub_0800C22C both end
 * `pop {r0}; bx r0`, so both are void. */
int sub_0800BC98(int, int);
void sub_0800C124(int, int);
void sub_0800C22C(int, int);

/* Two more, from sub_0800977C. Same entry reading, and both end `pop {r1}`, so
 * both return a value; sub_08009720's result is only ever `cmp r0, #0`'d, and
 * sub_08009BF4's is fed to the branchless `rsbs; orrs; lsrs #0x1f` form of
 * `!= 0`, which is a plain int operand here (no `lsls #0x18` in front of it). */
int sub_08009720(int, int);
int sub_08009BF4(int, int);

/* ---- wave 23 (W23-C) ----
 * sub_0801A368 draws a box into one of the four 0x800-byte tilemap buffers
 * (gUnknown_08499578/7C/80/84, `u16 *` per src/decomp/c_08023360.c): one call
 * for the top row, `height - 2` calls for the middle rows and one for the
 * bottom, each 32 entries apart, then it marks whichever of the four buffers
 * it wrote with sub_08013AD4(0..3).
 *
 * The three helpers' parameter widths are read from the CALL SITE, not from
 * their own prologues: sub_0801A368 holds its own third argument as a bare
 * `int` (nothing masks r2 at entry, and it is spilled whole to [sp,#4]) and
 * re-narrows it with `lsls #0x10; asrs #0x10` before all three calls, and
 * narrows a `u16` local the same SIGNED way for sub_0801A240's fourth. A
 * sign-extending narrow at a call site is what a declared `s16` parameter
 * produces -- but an explicit `(s16)` cast against an `int` parameter is
 * byte-identical, so the WIDTH is proved and the declaration is not. Re-read
 * it off the callee prologues when one of the three is matched. */
void sub_0801A1D8(u16 *, int, s16, int);
void sub_0801A240(u16 *, int, s16, s16, int);
void sub_0801A2E4(u16 *, int, s16, int);
void sub_0801A368(int, int, int, int, u16 *, int);

/* ---- wave 13 (A8), second block ----
 * The sub_08065990 / sub_0806D944 screen-setup callees. Every parameter list
 * below is the entry-narrowing readout of the callee, not a guess from the
 * call site:
 *   sub_0801A444  four `lsls #0x10; asrs #0x10` -> four s16
 *   sub_0801F150  r0 is scaled by 136 as an index and never narrowed (int),
 *                 r1 is `str`ed whole (a pointer), r2 is `strh`ed (u16) and
 *                 r3 is `strb`ed (u8)
 *   sub_0801F234  r0 goes straight to sub_0801F3D4 with no narrowing -> int
 *   sub_0802D5A0 / sub_0802D5CC  nothing is narrowed at entry
 *   sub_08073304  seven arguments -- three arrive on the stack at [sp,#0x20],
 *                 [sp,#0x24] and [sp,#0x28] after a 32-byte push, and the
 *                 narrowing there is u16/u16/u16/u8 for arguments 3-6
 * sub_0801F49C is already DEFINED in src/decomp/c_0801F49C.c as
 * `u8 *sub_0801F49C(void)`; this only publishes it, and the pointer return is
 * what lets sub_08065990 spell `Decompress(sub_0801F49C(), ...)` with nothing
 * between the two `bl`s. */
void sub_08013B0C(void);
void sub_08013B1C(void);
void sub_08013CA8(void);
void sub_0801A444(s16, s16, s16, s16);
/* Wave 31 (W31-C), the undeclared callees of the 0x0807B000 proc tree. Each is
 * reached as `adds r0, r4, #0; bl ...` from a proc step with the step's own
 * proc pointer and nothing else, and every caller discards r0 -- so one
 * pointer argument, void return. `void *` rather than a struct pointer because
 * each caller in that block models the proc with its own local layout and no
 * two of them agree on more than the offsets they touch. */
void sub_0807C034(void *);
void sub_0807C278(void *);
void sub_0807C2D4(void *);
void sub_0807C46C(void *);
void sub_0801F114(void);
/* ARGUMENT 4 IS `int`, NOT `u8` (wave 29, W29-B, from the definition). Nothing
 * narrows r3 at entry and nothing narrows it before it is forwarded to
 * sub_0801F178 -- a declared `u8` puts `lsls #0x18; lsrs #0x18` in one place or
 * the other. The `strb` at +4 is the member truncating, not the parameter. All
 * seven promoted call sites pass a `movs`-sized literal and are unaffected. */
void sub_0801F150(int, void *, u16, int);
void sub_0801F234(int);
/* Five arguments, all `int`: the prologue copies r0-r3 with bare
 * `adds rN, rM, #0` / `mov r8, r2` and narrows none of them, and the fifth
 * arrives at [sp, #0x1c] after a 24-byte push. `pop {r0}; bx r0` -> void.
 * It is a PutSpriteExt front end -- arguments 2 and 3 are the x|flags and
 * y|flags words, and 1 and 5 are looked up in gUnknown_0848B780 /
 * gUnknown_0848BAE4 to pick the OBJ data. */
void sub_0801F34C(int, int, int, int, int);
u8 *sub_0801F49C(void);
void sub_0802D5A0(void *, int, int);
void sub_0802D5CC(int, int);
void sub_08065238(void);
void sub_0806574C(void);
void sub_0806D268(void);
/* Wave 46 (W46-E): had no declaration. `pop {r0}; bx r0` and no argument
 * register read before it is written, so void(void). */
void sub_0806D53C(void);
void sub_0806D620(void);
void sub_0806D820(void);
void sub_0806D850(void);
/* Wave 46 (W46-E): declared to match the already-promoted definition in
 * src/decomp/c_0806D8B8.c, which had no declaration here at all. Nullary and
 * void: `pop {r0}; bx r0`, and its one caller sub_0806D850 sets up no argument
 * register and discards r0. */
void sub_0806D8B8(void);
/* WAVE 35 (W35-D): RETURN TYPE CORRECTED from `void` to ProcPtr. The last
 * thing before the epilogue is `adds r0, r7, #0`, which a void function does
 * not emit -- it hands back the gUnknown_086140D4 proc it just started. All
 * five existing callers (c_080339B0, c_0806D944, c_0806EB5C x2, c_080767C0 x2)
 * discard the result, so widening the declaration costs them nothing; each was
 * left untouched. */
ProcPtr sub_08073304(const void *, void *, u16, u16, u16, u8, int);
void sub_080733B8(void);
/* Wave 45 (W45-I). sub_080772B8 is its ONLY caller, so nothing else constrains
 * these: an id in r0 read `ldrsh` out of gUnknown_08615194[..].unk00, the ROM
 * table gUnknown_086145C8 in r1, and an 8-byte stack buffer in r2 that the
 * caller reads back as five `u8` digits. Weakest types that fit; the pointer
 * halves are `void *` because nothing here proves either element type. */
void sub_080733C8(int, const void *, void *);
/* Wave 45 (W45-I). Writes the decimal digits of its second argument BACKWARDS
 * from its first, one halfword per digit with 0x32 added -- a tile index, hence
 * `u16 *`. `int` for the value: it is divided and remaindered by 10 with
 * __divsi3/__modsi3, the signed helpers. sub_080772B8 is the only caller. */
void sub_0807728C(u16 *, int);
/* The HBlank window-line generator that sub_08073B00 drives: five s32-shaped
 * arguments, the fifth on the stack and only ever 0 or 1. */
void sub_08073998(int, int, int, int, int);
/* Swaps the gUnknown_0202FDE0 / gUnknown_0202FDE4 double buffer. */
void sub_08073AE8(void);
void sub_08063928(int);

/* The two payload handlers sub_0804E8F0/sub_0804FE10 pick between. Three u16
 * parameters on both: each opens `lsl #16; lsr #16` on r0, r1 and r2, and both
 * end `pop {r0}; bx r0`, so void. */
void sub_0804EA54(u16, u16, u16);
/* Wave 37, W37-J. Three neighbours that are DEFINED in src/decomp
 * (c_0804EDAC.c, c_0804EE08.c, c_080520B8.c) but had no declaration here,
 * so nothing outside their own unit could call them. The signatures are
 * copied verbatim from those definitions, not inferred. sub_0804EB78 calls
 * sub_0804EDAC and sub_080520B8; sub_0804F3C8 calls sub_0804EE08 and
 * sub_080520B8, in both cases with the `(side, slot, gUnknown_03001FBC)`
 * shape sub_0804BDD8/sub_0804BECC already use. */
void sub_0804EDAC(u16, u16, s16);
void sub_0804EE08(u16, u16, s16);
void sub_080520B8(u16, u16);
void sub_0804EAEC(u16, u16, u16);

/* ---- wave 13 (A7) ----
 * sub_0808BB0C is fabsf: sub_0808B710 (matched, the ROM's sinf) hands it the
 * range-reduced float in r0 and compares the result against 1.1920929e-07
 * before running the sin polynomial, so it takes a float and returns one. */
float sub_0808BB0C(float);

/* ---- wave 13 (A8), fourth block ----
 * The nineteen tile-action handlers sub_080085E0 dispatches to. Every one takes
 * the (x, y) cell pair as a bare `adds rN, r0, #0` with no narrowing at either
 * end -- same reading as the sub_0800B244 group above -- so `int` throughout,
 * and sub_0800C454's third argument is unnarrowed too. The three declared
 * `int`-returning ones are the predicates: sub_0800B528's result is tested
 * `blt 0` (so it is signed and -1 means "none"), sub_0800C840's and
 * sub_0800BF78's and sub_08009F10's are only `cmp r0, #0`. */
void sub_080011F4(int, int, int);

/* Wave 50 (W50-I). sub_080012DC is already matched in src/decomp/c_080012DC.c
 * as `int sub_080012DC(int)` and was simply never declared; this line only
 * publishes that definition's own signature. Its single caller sub_0800C454
 * narrows the result with `lsls #16; lsrs #16`, so the (u16) is a CAST AT THE
 * CALL SITE and not a narrow return type -- the definition returns a plain
 * `int` register (`adds r0, r2, #0; bx lr`) whose values run to 0x1D7. */
int sub_080012DC(int);
void sub_08007CA0(int, int);
/* Wave 37 (W37-D), blocks 0x08007 and 0x08008. The (int, int) pair is the same
 * (x, y) cell coordinate the rest of the 0x0800xxxx tile family takes, and the
 * arity is read off the call sites rather than guessed: every caller sets r0
 * and r1 from its own pair and leaves r2 holding whatever the previous `bl`
 * left there, which a third parameter could not survive.
 * Return types come from the epilogue: `pop {rN}; bx rN` with rN == r0 is void,
 * and the three that return pop into r1 instead. */
void sub_08007354(void);
void sub_080079B8(int);
void sub_08007A30(void);
void sub_08007BA4(int, int);
void sub_08007C04(int, int);
void sub_080080F8(int, int);
int sub_08008928(void);
/* First parameter is a mode flag (1 = install the window/palette state,
 * 0 = clear the record's unk00); the other two are the cell pair, in that
 * order -- `lsls r2,r2,#1` is the y*2 row index. */
int sub_08008A8C(int, int, int);
int sub_08008D70(int, int);
void sub_08008E3C(int, int);
/* Callees of the above from outside the batch. Arity read from each one's own
 * prologue in asm/code.s -- all five save r0 and r1 and none reads r2.
 * sub_0800B61C returns s16: sub_080080F8 re-narrows its result with
 * `lsls #0x10; asrs #0x10` and then uses the value, which is agbcc's
 * re-narrowing of a narrow-returning callee. */
void sub_0800A588(int, int);
void sub_0800ABD0(int, int);
void sub_0800B048(int, int);
s16 sub_0800B61C(int, int);
int sub_0800F418(int, int);
/* Wave 42 (W42-C). Void is read off the tail: it ends `pop {r0}; bx r0` after
 * the register restores rather than after a call, and its only caller
 * (sub_08010ADC) discards r0. */
void sub_08010664(int, int);
void sub_08010ADC(int, int);
/* Already promoted in src/decomp/, but never declared -- wave 37 (W37-D) needed
 * them from a second unit. Types are the promoted definitions', not new work. */
void sub_080088F0(void);
int sub_08008B70(int, int);
int sub_08008CB8(int, int);
int sub_08008D14(int, int);
void sub_08008BB8(int, int);
void sub_08008F6C(int, int);
int sub_08009F10(int, int);
void sub_0800AF74(int, int);
int sub_0800B528(int, int);
/* RETYPED wave 37 (W37-C), void -> int.  sub_0800BA9C ends with an explicit
 * `movs r0,#0` on two paths and `movs r0,#1` on the third, which a void
 * definition deletes; its only caller sub_0800A098 drops the result, so the
 * retype is byte-neutral there and sub_0800A098 was re-verified after it. */
int sub_0800BA9C(int, int);
void sub_0800BEE4(int, int);
int sub_0800BF78(int, int);
void sub_0800C454(int, int, int);
void sub_0800C608(int, int);
int sub_0800C840(int, int);
void sub_0800CF28(int, int);
void sub_0800EC20(int, int);
void sub_0800F4E0(int, int);
void sub_08010D28(int, int);
void sub_08010D80(int, int);

/* ---- wave 13 (A7), second block ----
 * Three plain `int`s: sub_08085410 (matched) hands it two `ldrb` bytes and its
 * own second parameter with no narrowing anywhere, and sub_080430B0 scales all
 * three as bare indices (`muls r3, r2, #0x5c`, `r1*0x44`, `r0*0x104`). The
 * result is SIGNED and small -- sub_08085410 switches on it over -30..80 with
 * `adds r0, #0x1e; cmp r0, #0x6e; bls`, the signed-range jump-table idiom. */
int sub_080430B0(int, int, int);

/* Called by sub_08069EAC with a literal 0 or 1 -- a direction/side flag. The
 * literal argument fixes nothing about the width, so `int` is the weakest
 * declaration that compiles the call site exactly. */
void sub_08069D3C(int);

/* The glyph blitter sub_0801172C dispatches to. All three parameters are u16:
 * sub_0801172C passes `y - 7` for three of its cases and that argument alone
 * carries `lsls #16; lsrs #16`, which is the truncation a u16 parameter forces
 * on an expression that is not provably 16-bit -- the other two arguments and
 * the tile constants are provably narrow and so carry none. */
void sub_08011704(u16, u16, u16);

/* Wave 37 (W37-P1): copied verbatim from the promoted definition in
 * src/decomp/c_0801172C.c, which was never declared anywhere. */
int sub_0801172C(u16, u16, u8);

/* The 0x0806A054 screen-setup group.
 *
 * sub_080674F4 takes a `u8`: its one call site passes `gUnknown_0202F204++`,
 * whose value already arrives zero-extended from `ldrb`, and the ROM still
 * emits `lsl #24; lsr #24` -- that redundant pair is the argument conversion
 * and an `int` parameter cannot produce it. Its result is stored with `str`.
 * sub_080677BC is matched (c_080677BC.c): it starts gUnknown_08580FE4 under
 * the caller's fourth argument as parent and files the first three at +0x58,
 * +0x34 and +0x38, all `str`. */
int sub_080674F4(int);
void sub_080670F8(const u8 *);
void sub_08069FD0(void);
ProcPtr sub_080677BC(s32, s32, s32, ProcPtr);

/* The two debug-text primitives sub_08057464 (matched) drives. Both narrow r0
 * and r1 with `lsls #16; lsrs #16` in their own prologue, which is PROMOTE_MODE
 * on declared-`u16` parameters; the caller passes small constants and bare
 * `ldrh` results, so nothing is emitted at the call site either way and the
 * `u16` reading comes entirely from the callee. They differ only in the third
 * argument: sub_080119A0 saves it bare and uses it as a string, sub_08011A20
 * saves it bare and it arrives from `ldrsh` on an s16 global -- unnarrowed, so
 * `int` and not `s16`, by the parameter rule in docs/agbcc-codegen.md. */
void sub_080119A0(u16, u16, const char *);
/* Wave 37 (W37-P1): third parameter retyped `int` -> `u32`. The body divides
 * and takes the remainder with __udivsi3 / __umodsi3, which only an UNSIGNED
 * operand produces, and the `adds r4, r2, #0` that saves it is the FIRST
 * instruction after the push -- an `int` parameter copied into a `u32` local
 * inside the body sinks that copy below the two u16 narrowings. Byte-neutral
 * at both call sites (the s16 value the note above describes still arrives in
 * the register unchanged); c_08057464.c re-verified. */
void sub_08011A20(u16, u16, u32);
/* Wave 37 (W37-P1): the hex sibling of sub_08011A20 -- same (x, y, value)
 * shape, but the digit is extracted with `ands #0xf` / `lsrs #4` rather than
 * __umodsi3 / __udivsi3, so the third parameter is UNSIGNED here where
 * sub_08011A20's arrives as a sign-extended `int`. */
void sub_080119D4(u16, u16, u32);

/* Two more of the 0x08068AC4 group. sub_0806775C starts a proc under the
 * caller's second argument (the gUnknown_08580FCC entry noted in
 * unknown-globals.h) and its first argument is a literal 0 at the one call
 * site. sub_080718F8's first argument is a byte cursor into the
 * gUnknown_08499584 buffer, its second a ROM blob and its third a literal 0.
 *
 * NOTE, so nobody tries to "fix" this: sub_080718F8 is not a function. It is a
 * linker THUMB->ARM interworking veneer for the ARM routine sub_0800043C -- see
 * data/asm-resident.json, which records both halves. The original source called
 * sub_0800043C and the linker synthesised the veneer; we cannot, because the
 * split already owns the veneer's four bytes as its own unit, so C has to name
 * the veneer and let the `bl` land there. This is a spelling forced by the
 * split, exactly like the `&gUnknown_XXXXXXXX` pool-word case in
 * docs/agbcc-codegen.md, and it is NOT a claim about the original. Four matched
 * drafts now rely on it -- sub_08068AC4 (promoted) plus sub_0806BB08,
 * sub_0806EB5C and sub_0806ED7C -- so if sub_0800043C is ever promoted, every
 * one of them has to be re-pointed at it and re-verified. The declared
 * types describe sub_0800043C's parameters, since the veneer has none of its
 * own. */
void sub_0806775C(int, ProcPtr);
void sub_080718F8(void *, u8 *, int);

/* ---- wave 13 (A1) ----
 * Declared WITHOUT a prototype, the same way sub_0801BB10 is above, and that
 * is deliberate: src/decomp/c_0801C240.c already defines it as
 * `void sub_0801C240(struct Unk_0801C240 *)` on a struct type that is local to
 * that file, so any prototype here naming a different pointer type makes that
 * already-matched file fail with `conflicting types`. An empty parameter list
 * is compatible with the existing definition and still lets sub_0801C2DC call
 * it. The real fix is to lift struct Unk_0801C240 into unknown-globals.h and
 * merge it with struct Unk0801C2DC (it is the same object -- 0801C240 clears
 * its +0x00, which is the same `unk00 != NULL` gate sub_0801C2DC opens with);
 * that is a promotion-time edit to another agent's file, not a wave edit. */
void sub_0801C240();

/* sub_08011BD4 returns s16: sub_08021DD8 compares its result with
 * `lsls r0, #16; cmp r0, #0x1000 << 16`, which is the sign-extending form of
 * `<= 0x1000` and is only generated for a 16-bit signed value. The two
 * sub_08021Dxx animation kickers take a small int selector (0..7) passed bare
 * in r0 with no narrowing, and both return nothing. */
s16 sub_08011BD4(void);
void sub_08021D64(int);
void sub_08021DA0(int);

/* The BIOS fast fill/copy. sub_080688E4 calls it with a stack zero, a VRAM
 * destination and 0x01000100 -- bit 24 set is FILL, low 21 bits the word
 * count. Declared here rather than in a gba/ header because nothing else in
 * the tree has needed it yet. */
void CpuFastSet(const void *, void *, u32);
/* The BIOS affine-matrix helper, first needed in wave 26 by sub_080678EC. Its
 * source block is exactly `struct BgAffineSrcData` (include/gba/types.h): the
 * ROM builds a 0x14-byte stack frame whose two words, four halfwords and
 * trailing halfword land on texX/texY, scrX/scrY/sx/sy and alpha in that order.
 * The destination is gUnknown_030024D0, which hardware.h already declares as
 * `volatile u32 [4]` -- 0x10 bytes, i.e. one BgAffineDstData -- so callers cast
 * it explicitly rather than the declaration being changed under its other
 * users. */
void BgAffineSet(struct BgAffineSrcData *, struct BgAffineDstData *, s32);
/* `pop {r0}; bx r0`, so void. Its argument is used as a full word
 * (`lsls r5, r0, #0x18` at a USE, with no PROMOTE_MODE pair in the prologue),
 * which per the copy-then-narrow rule makes the parameter `int` and the
 * narrowing an (s8) cast inside the body. sub_080674BC passes a constant 1. */
void sub_08013928(int);
/* Both parameters are used as full words -- r0 is scaled by 0x44 into
 * gUnknown_084A0090 with no mask, r1 gets `+0x80` and `& 0x3ff` before being
 * turned into a VRAM offset -- so `int` twice; `pop {r0}` makes it void.
 * sub_08067DF8 passes a u8 proc field and a u16 table element, both of which
 * widen for free. */
void sub_08043BF8(int, int);
/* ---- wave 28 (W28-B): the rest of the 0x08043Axx graphics-slot block ----
 * All of these scale their first argument by 0x44 into gUnknown_084A0090 with
 * no mask, exactly as sub_08043BF8 above does, so it is `int` in every one of
 * them; all end `pop {r0}` / `pop {r1}` with no caller reading r0, so all are
 * void. sub_08043AA0 and sub_08043AC0 take the `% 24` remainder of that index
 * -- and sub_08043AC0 takes it AGAIN on its own parameter, which is why its
 * body opens with two chained __modsi3 calls on one value.
 * sub_08017860, sub_08042FFC and sub_08043D84 are already PROMOTED
 * (src/decomp/c_08017860.c, c_08042E2C.c, c_08043D84.c) and were never
 * declared here; the signatures are copied verbatim from those definitions.
 * sub_080436DC's third argument is `strh`-stored into the u16
 * gUnknown_030005D0 and its one readable caller (sub_0804360C) loads a u16
 * global into it, so nothing narrows either way -- `int` is the weakest fit.
 * sub_08043D00 reads no argument register and its caller sub_08043D5C sets
 * none up. */
void sub_08043AA0(int, int);
void sub_08043AC0(int, int, int);
void sub_08043B14(int, int);
void sub_08043B44(int);
void sub_08043BC8(int, int);
void sub_080436DC(int, int, int);
void sub_0804365C(int, int);
void sub_08043D00(void);
/* sub_08017860 returns `int`, not the `u8` its promoted definition used to
 * name (wave 28, W28-B). Its body is `ldrb r0, [&gUnknown_0200C420 + 0x20 + i]
 * ; bx lr`, which is the same four instructions either way -- byte-neutral at
 * the definition, so the definition never had an oracle for it. The readout is
 * at sub_08043AA0, which forwards the result straight into sub_08043AC0's
 * third parameter with a bare `adds r2, r0, #0`: a u8 return re-narrows at
 * every call site, and that call site does not. c_08017860.c was retyped to
 * match and re-verified. */
int sub_08017860(int);
int sub_08042FFC(int);
u16 sub_08043D84(u8);
/* No calls, no arguments read, and its one caller (sub_08067070) discards
 * whatever is in r0 afterwards, so `void` is the weakest model that fits. */
void sub_080658AC(void);
/* The three gUnknown_030030B4-style BG-control field setters, already matched
 * in src/decomp/c_08012C1C.c / c_08012C30.c / c_08012C48.c. They take the
 * `struct Unk8012C30 *` those files define; the tag is forward-declared here so
 * callers in other units can name it without duplicating the layout, and they
 * cast their `union BgCntBuf *` at the call. */
struct Unk8012C30;
void sub_08012C1C(struct Unk8012C30 *, u32);
void sub_08012C30(struct Unk8012C30 *, u32);
void sub_08012C48(struct Unk8012C30 *, u32);
/* Starts the follow-up proc for sub_080688E4 under the caller's fourth
 * argument; the first three are `int` (0xC00, -0x28 and 0 at the one call
 * site, so the second is signed). */
void sub_08067898(u32, u32, u32, ProcPtr);
/* The gUnknown_08580FF4 proc's +0x60 setter (see the script's note in
 * unknown-globals.h). sub_08069B90 calls it with a bare `movs r0, #1` and
 * nothing narrows the argument. */
void sub_080678BC(u32);

/* Three more of the sub_0806B708 group. sub_08072C28's first argument is the
 * gUnknown_0849957C buffer and the other two are byte counts/offsets (0x400
 * and 0x280 at the one call site). sub_0806AF44 takes the caller's proc. */
void sub_08072C28(u16 *, u32, u16);
void sub_0806B120(void);
/* Wave 34, W34-E: RETYPED from `void` to `int`. The body settles it -- both
 * exits load r0 (`movs r0, #0` on the rejected-unit path, `movs r0, #1` after
 * either arm of the tail), which a void function would not do. Its one caller
 * is c_0806B708.c:57 and it DISCARDS the result, so the change is byte-neutral
 * there; c_0806B708.c re-verified after the edit.
 *
 * Wave 34, W34-K: it has a SECOND caller, and that one does not discard the
 * result -- sub_0806B910 narrows it with `lsl #0x18` before testing it, which
 * is a byte-width return, not `int`. Per the "settle a return type from the
 * CALLERS" rule this is evidence that `u8`/`s8` is the truth and the body-side
 * `int` is the guess. It is left as `int` here deliberately: sub_0806B910
 * spells the narrowing as a `(u8)` cast at its own call site, which is
 * byte-identical and costs no other file a change. Whoever revisits this
 * should retype the header and DROP that cast, then re-verify c_0806B708.c,
 * work/sub_0806AF44 and src/decomp/c_0806B910.c together. */
int sub_0806AF44(ProcPtr);

/* Wave 34, W34-E: both are already DEFINED and promoted (src/decomp/c_08087884.c
 * and c_08087974.c) but had no declaration anywhere; these two lines just
 * publish the promoted signatures so sub_08085F94 can call them. Do not weaken
 * them -- the definitions win. */
void sub_08087884(int, ProcPtr);
void sub_08087974(int, ProcPtr);

/* SWI 6's remainder wrapper: sub_0807D918 uses the result as a plain index. */
int DivRem(int, int);
/* Two more gUnknown_030058E0 display-list builders, both taking a byte pulled
 * out of that array. sub_08043E3C's second argument is a VRAM tile address
 * (0x06013000 / 0x06013480 / 0x06013900 at the three call sites). */
void sub_08043BA4(int, int, int);
void sub_08043E3C(int, void *, int);

/* sub_08068BE4 (matched) calls it `sub_080673B0(0x20, 1, proc)` and hands it
 * its own proc pointer, which is the third-argument-is-the-parent shape the
 * gUnknown_08580E94 note in unknown-globals.h already records for it. Neither
 * of the first two arguments is narrowed at the call site. */
void sub_080673B0(u32, u32, ProcPtr);

/* sub_08015410 forwards to sub_08015438 and RETURNS s8: it sign-extends
 * sub_08015438's result with `lsls #24; asrs #24` before returning, and its
 * caller sign-extends again before a `strh`, which only makes sense if the
 * declared return is a signed byte. Parameters 2 and 5 are `u8` -- both are
 * re-narrowed with `lsls #24; lsrs #24` inside the callee, which is PROMOTE_MODE
 * on a declared-narrow parameter. sub_0804C400 takes the same u16 unit index
 * everything else in that family does. */
s8 sub_08015410(void *, u8, void *, void *, u8);
/* OPEN, and it does not matter to any caller yet: sub_0804C400's prologue is
 * `adds r4,r0,#0; lsls r4,#0x10; lsrs r4,#0x10`, which is copy-THEN-narrow,
 * and the rule in docs/agbcc-codegen.md reads that as an `int` parameter with
 * a cast at a use rather than a declared-narrow one. The register pressure
 * does not explain the copy -- sub_08053670 also has to keep its narrowed
 * argument across two calls and gets `lsls r0,r0,#0x10; lsrs r5,r0,#0x10`
 * with no copy at all. Left as u16 because that is what its whole family
 * takes and because it is byte-neutral for the three wrappers that only
 * forward to it (sub_0804C488, sub_0804C498 in work/, whose own parameter is
 * narrowed by PROMOTE_MODE either way). Settle it when sub_0804C400 itself is
 * matched, not before. */
void sub_0804C400(u16);
/* Two more of the same family, both called by sub_0804E7A8 / sub_0804FCA4 with
 * (side, slot) out of gUnknown_03001470[gUnknown_03001FBC].unk30 / .unk34.
 *   sub_08056E9C is PROMOTED in src/decomp/c_08056E9C.c as (u16, u16) and had
 * no prototype, which is the "promoted and still undeclared" trap in
 * docs/agbcc-codegen.md -- the types here are copied from the definition, not
 * re-derived.
 *   sub_0804BDD8's third parameter is `s16`: its prologue zero-extends r2 AND
 * sign-extends the same value (`lsls #0x10; lsrs r0,#0x10 ... asrs r2,#0x10`),
 * which is PROMOTE_MODE plus a signed use, and both callers pass the s16
 * gUnknown_03001FBC. The return is `int` on the same caller-side evidence as
 * sub_080156C4 above: both callers narrow it with `lsls #0x10; lsrs #0x10`
 * into a u16 local. */
void sub_08056E9C(u16, u16);
int sub_0804BDD8(u16, u16, s16);

/* The 0x0806E000 screen's helpers, all named only by sub_0806EB5C. The five
 * that end `pop {rN}; bx rN` after a bare `bl Proc_Start` pass their own last
 * argument straight through as Proc_Start's parent, which is what types it
 * ProcPtr; sub_0806E5CC narrows its first argument with `lsls #16; lsrs #16`
 * and is therefore u16, the rest arrive unnarrowed and are `int`. */
void sub_0806F000(int, int);
void sub_0806EB28(ProcPtr);
void sub_0806E5CC(u16, ProcPtr);
void sub_0806E8C8(int, ProcPtr);
/* Wave 32 (W32-C), both read off their own bodies.
 *
 * sub_0806E6C8 is `adds r4,r0,#0; ldr r0,=gUnknown_08582BB4; bl
 * Proc_StartBlocking; str r4,[r0,#0x5c]` -- r1 is never touched, so it is
 * Proc_StartBlocking's parent forwarded unchanged, and the saved value is
 * stored as a WORD with no narrowing anywhere in the body, hence `int` and not
 * the `u16` its one caller happens to pass (sub_0806F0A0, an `ldrh`).
 *
 * sub_0806E8E4 overwrites r0 with the =gUnknown_08582C24 pool word before any
 * read, so Proc_Find takes one argument and this takes one: the saved r4 is
 * tested `!= 0` only, and picks +0x5c or +0x58 on the found proc. */
void sub_0806E6C8(int, ProcPtr);
void sub_0806E8E4(int);
/* Defined in src/decomp/c_0806F064.c; declared here in wave 32 (W32-C) because
 * sub_0806F0A0 is its first C caller. Signature copied from that definition. */
u16 sub_0806F064(u16, u16 *);
/* Defined in src/decomp/c_0806C1C8.c; declared here in wave 32 (W32-C) for its
 * first C callers, sub_0806C380 and sub_0806C410. */
void sub_0806C1E4(void);
/* Defined in src/decomp/c_0806E740.c and src/decomp/c_0801F48C.c; declared here
 * in wave 32 (W32-C) for their first C callers, sub_0806E830 and sub_08049D88.
 * Signatures copied from those definitions. */
void sub_0806E7FC(void);
u8 *sub_0801F494(void);
/* Wave 32 (W32-C). Reads its first argument's +0x00 and +0x01 BYTES -- the
 * gUnknown_08499594 unit record's own unk00/unk01 -- and forwards r1 untouched
 * as Proc_Start's parent, then parks the record at the new proc's +0x4c. */
void sub_0802A54C(struct UnitRecord *, ProcPtr);
/* Wave 32 (W32-C). The tail call of sub_080310D8 / sub_08031128 /
 * sub_0803117C, all three of which reach it with no argument setup at all.
 * Declared void(void) as the weakest model that fits -- NOT read off its own
 * prologue, which is still asm, so treat it as provisional. */
void sub_08031018(void);
/* ---- wave 32 (W32-C): the 0x0803B block's callees, all read off their own
 * bodies except sub_08015638, whose definition is src/decomp/c_08015638.c ----
 *
 * sub_08015900 / sub_080158D4 are the getter/setter pair on one +0x40
 * halfword of the gUnknown_0200E438 record a gUnknown_03001470 slot names.
 * Both narrow their first argument `lsls #0x10; asrs #0x10`, so it is `s16`;
 * sub_08015900 returns `movs r1,#0; ldrsh r0,[r0,r1]`, the register-offset
 * ldrsh, so its return is `s16` too. sub_080158D4's second argument reaches a
 * bare `strh` with no narrowing, but its one caller casts to s16 explicitly.
 *
 * sub_08016E04 is `lsls r0,#0x10; cmp r0,#0` and the branching 0/1 return, so
 * it takes a 16-bit value and answers a bool.
 *
 * sub_08016EA4 writes r0 before reading it, so it is nullary; it ends
 * `pop {r0}; bx r0`, the void epilogue. */
s16 sub_08015900(s16);
void sub_080158D4(s16, s16);
struct UnkVec sub_08015638(s16);
bool8 sub_08016E04(u16);
void sub_08016EA4(void);
/* Wave 32 (W32-C). sub_08065700 is `sub_0806377C(gUnknown_08580C7C)` and
 * nothing else, so void(void) -- the same one-line forwarder shape the comment
 * higher up this file already records for it. sub_08064B68's parameter width
 * is PROVISIONAL: its only call in C so far (sub_08067020) passes the literal
 * -2 and its own body is still asm, so `int` is the weakest model that fits
 * rather than a readout. */
void sub_08064B68(int);
void sub_08065700(void);
void sub_0806E510(ProcPtr);
void sub_0806E728(ProcPtr);
void sub_08073FF4(int, const void *, ProcPtr);
/* Already matched (c_08074028): a one-liner `Proc_EndEach(gUnknown_08614220)`,
 * so void/void off its own body. Declared in wave 45 (W45-I) because
 * sub_0806F27C is its only caller and needed to see it. */
void sub_08074028(void);
/* Wave 44 (W44-I). The 0x08073CB8 blit chain, three nested levels over one
 * bitmap. Types copied from the innermost level's promoted definition
 * (src/decomp/c_08073D1C.c): `src` is a u8 nibble source, `dst` a u32 tile
 * row, and the trailing `int` is the bitmap width in tiles, threaded down
 * UNCHANGED through both loops -- each level advances its own pointers and
 * counts with a separate register, and passes the original parameter on.
 * sub_08073D1C had no declaration at all until now; only its definition
 * existed, so every caller outside its own file compiled implicitly. */
void sub_08073D1C(u8 *, u32 *, int);
void sub_08073CF4(u8 *, u32 *, int);
void sub_08073CB8(u8 *, u32 *, int, int);

/* Named only by the two wave-13 A7 near-misses sub_0806BB08 and sub_080867BC.
 * sub_0808B6E8 is a memcpy: (dst, src, size) with a stack destination and an
 * 0x12 literal size. sub_08087248 and sub_08087298 take nothing and return a
 * value that is stored with `strh` into a u16 global and used in `int`
 * arithmetic first, so `int`. sub_08086BF8/sub_08086CE0 take the same three
 * arguments in the same order at both call sites. */
/* sub_0806B9CC immediately precedes sub_0806BA6C and shares its shape: four
 * byte parameters stored via bare `strb rN,[..]` with no prologue narrowing
 * at all, so int is the weakest type that fits (wave 22, src/decomp/c_0806B9CC.c). */
/* Takes no arguments -- its first instruction writes r0 -- and `pop {r0}`
 * makes it void. Its one caller, sub_0806C8DC, happens to have a zero in r0 at
 * the call, which is a coincidence of the preceding store and not an argument.
 * It clears a 20x22 halfword window of *gUnknown_08499578 and flushes. */
void sub_0806C8A0(void);
void sub_0806B9CC(int, int, int, int);
/* Wave 48 (W48-C), matched byte-for-byte, so this is the definition's own
 * signature rather than a caller-side guess. Renders a NUL-terminated byte
 * string into two tilemap rows and RETURNS its pixel width (8 per glyph) --
 * sub_0806BF40, its only caller so far, discards the result, which is why the
 * return type is not visible there. The first parameter is a `u16 *` tilemap
 * cursor, not a struct pointer; see src/decomp/c_0806BD6C.c for the
 * discriminator. */
int sub_0806BD1C(u16 *, u8 *);
/* Wave 48 (W48-C), matched byte-for-byte. Starts the gUnknown_08581A34 proc
 * under `parent` and loads a byte string into its +0x2a halfword table. The
 * first parameter is a row index, scaled `* 24 + 8` into the proc's +0x58. */
void sub_0806BED8(int, u8 *, ProcPtr);
void sub_0806BA6C(int, int, int, int);
void *sub_0808B6E8(void *, const void *, int);
/* Wave 32 (W32-B): four arguments, read off its own prologue -- r1, r2 and r3
 * are copied to r7/r5/r6 before anything else and r0 is used in place. It packs
 * r0 and r1 into PutSpriteExt's two coordinate words (masking each with
 * 0xfffffe00 / 0xffffff00, re-ORing the low bits and biasing r1 by -0x30),
 * forwards r2 as PutSpriteExt's fifth argument and r3 as its first, and pins
 * the OAM blob to gUnknown_084A0790. Nothing narrows any of them, so `int`. */
void sub_08043FD8(int, int, int, int);
/* Wave 32 (W32-B): both DEFINED in src/decomp/c_08064DDC.c and never declared
 * until sub_08064E5C, promoted this wave, became their first C caller.
 * Published unchanged. */
void sub_08064DDC(int, int, int);
void sub_08064E1C(int, int, int);
void sub_08086EB0(int);
void sub_08087104(void *);
int sub_08087248(void);
/* Wave 34, W34-B. sub_080870B8's only callee, and its whole signature is read
 * off that call site: four calls in a row, each `movs r0,#K; movs r1,#0x10;
 * adds r2,rN,#0` with nothing after the `bl`, so three arguments and void.
 * Nothing narrows any of them at the call, so `int` is the weakest that fits.
 * The four K values are 0x9f/0xb7/0xcf/0xe7 -- a 0x18 stride -- with a constant
 * 0x10 second argument, which reads as (dest, width, value).
 *
 * sub_0803CA54 is sub_08087104's guard: it is called with NO argument setup at
 * all, the byte just loaded from gUnknown_02027F74.unk04[] still being live in
 * r0, and its result is tested `lsls #0x18; cmp #0`.
 *   That `lsls` is NOT a narrow return type here, and the first version of this
 * declaration got it wrong: src/decomp/c_0803CA54.c is PROMOTED and defines
 * `int sub_0803CA54(u32)`, so the definition wins and the declaration is
 * published to match it. proto_check.py caught the disagreement -- try_match
 * compiles one unit and cannot see this class of error, which is exactly the
 * case the "prototypes are a contract" rule warns about. The byte-width test at
 * the call site therefore belongs to the CALLER, spelled `(u8)sub_0803CA54(v)`,
 * and that spelling is what reproduces the ROM. */
/* RETYPED in wave 36 (W36-B): the first two parameters are u16, not int.
 * Five call sites in three functions all narrow the argument to 16 bits, and
 * two of them could not be matched while the declaration said `int`:
 *   - sub_080436DC and sub_0804365C compute `(coord + K) & 0x1ff` for argument
 *     1. With an `int` parameter agbcc emits `ldr rC,=0x1FF; ands r0,rC` (two
 *     instructions); the ROM has `ldr r2,=0x1FF; adds r1,r2,#0; ands r0,r1`
 *     (three). The extra copy is the u16 conversion's own pseudo -- the value
 *     is provably < 0x200 so the truncation itself folds away, but the extra
 *     SET stops the constant's register being reused as the AND's operand.
 *     The same function's OTHER arm, which calls sub_080119A0(u16, u16,
 *     const char *) with the identical expression, already emitted the ROM's
 *     three instructions, which is the controlled comparison.
 *   - argument 2 is `lsls #0x10; lsrs #0x10`-narrowed at every site
 *     (sub_080436DC's `y + 3`, sub_0804365C's `y`).
 *   - src/decomp/c_0802BBDC.c had already worked around it with an explicit
 *     `(u16)` cast on argument 1 and a note saying the parameter "is declared
 *     int; the cast reproduces" the narrowing. That is the wrong-prototype
 *     tell described in the wave brief.
 * Byte-neutral for every existing caller: c_0802BBDC.c's casts are idempotent
 * and c_0803ABDC.c / c_080870B8.c pass constants. Argument 3 stays wide --
 * sub_080436DC hands it a whole-word `ldr` with no narrowing. */
/* Parameter 3 RETYPED from `int` to `u32` in wave 41 (W41-B): the body's
 * divide and remainder are __udivsi3 / __umodsi3. Byte-neutral at all five
 * promoted callers -- a 32-bit parameter narrows at neither spelling -- and all
 * five were re-verified. Note c_080870B8.c passes a value it has just compared
 * against -1, so the ROM really does render 0xFFFFFFFF as decimal there; that
 * is the original's behaviour, not an artefact of this retype. */
void sub_0802BD54(u16, u16, u32);
/* Wave 41, W41-B: copied from the promoted definition in src/decomp -- its four
 * callers (sub_0802BE28 / sub_0802BE80 / sub_0802BEC4 / sub_0802BF20) are its
 * first C callers. `u8` first parameter and `u16` third are read straight off
 * the prologue's `lsls #0x18; lsrs #0x18` and `lsls #0x10; lsrs #0x10`; the
 * second is `s16` because the body sign-extends it (`asrs #0x10`) for the
 * `> 5` clamp before zero-extending it again. */
void sub_0802BDBC(u8, s16, u16);
int sub_0803CA54(u32);
u16 sub_08087298(void);
/* Wave 46, W46-G: the SECOND parameter is retyped `u32` -> `int`, from the
 * bodies of both functions rather than from a call site.  Each opens with
 * `ldrb` of gUnknown_02027F74.unk37 compared against it using `bge`, and
 * follows with `ble` against the literal 2 -- both SIGNED, which a u32 operand
 * cannot produce (the u8 promotes to int and would then convert to unsigned,
 * giving `bhs`/`bls`).  Byte-neutral at every call site: both spellings are
 * word-wide with no narrowing, so no caller changes. */
void sub_08086BF8(u32, int, int);
void sub_08086CE0(u32, int, int);

/* Wave 46, W46-G.  Copied from the promoted definition in
 * src/decomp/c_0802490C.c, which is authoritative -- it returns a `u8 *` out of
 * gUnknown_08610A38 and nothing narrows it.  sub_08086BF8/sub_08086CE0 are the
 * first callers to need it; they hand the result straight to sub_080149C0's
 * `u8 *` fourth parameter. */
u8 *sub_08024944(u16);

/* ---- wave 13 (A1), second block ----
 * Three more of the (x, y) cell family the A8 block above documents, same
 * `adds rN, r0, #0` reading, same `int` throughout.
 *
 * sub_0800164C is the "is this cell road-like" predicate: it reads
 * gUnknown_08499590 + 0x1432 + rowOffset[y] + x and returns 1 for 7, 0xD or
 * 0x13 (the last through the branchless `eors #0x13; rsbs; orrs; lsrs #0x1f`
 * idiom) and 0 otherwise. Value-returning epilogue. Its three constants are
 * also what pins sub_0800C124's switch case list -- see the "two empty cases
 * collapse, three do not" rule in docs/agbcc-codegen.md.
 *
 * sub_08008C34 returns a value too (`pop {r4,r5}; pop {r1}`) and is only ever
 * used as a guard (`cmp r0, #0; bne`), so `int` with the signedness inherited
 * rather than proved. sub_08007F9C ends `pop {r0}`, so it is void. */
int sub_0800164C(int, int);
int sub_08008C34(int, int);
/* Wave 31 (W31-C). sub_08008C7C is sub_08008C34's own second test, on the same
 * (x, y): it reads the +0x0A22 u16 tile plane through the +0x417A rowOffset[]
 * and returns 0 for tile 0x13 or 0x16 and 1 otherwise. `movs #0` / `movs #1`
 * split across an unconditional `b`, so it is `if (...) return 0; return 1;`
 * and not a returned comparison.
 *
 * sub_08025308's result is stored with a bare `strb` into four consecutive
 * bytes of gUnknown_0200B0B0 by sub_080088F0, with NO `lsls #0x18; lsrs #0x18`
 * in front -- so `int`, per the re-narrowing rule. Its argument is the 1-based
 * army number, matching the note on gUnknown_08499594.
 *
 * sub_0802BBDC takes the `ldrsh` element of gUnknown_08090A98 that sub_0802BC5C
 * hands it, so `s16`. sub_08046A84's two arguments are the byte pair above.  */
int sub_08008C7C(int, int);
int sub_08025308(int);
void sub_0802BBDC(s16);
/* Wave 49, W49-J: RETYPED from (int, int) to (u8, u8) for the same reason as
 * sub_08046778/sub_08046914 below -- the definition's prologue narrows BOTH
 * registers with `lsls #0x18; lsrs #0x18`, which an int parameter never gets.
 * The note above about "the byte pair" is what the prologue confirms. */
void sub_08046A84(u8, u8);
/* Wave 49, W49-J. Read off the definition: r0 is a pointer (untouched, then
 * `adds r0,#0x21`), r1/r2/r3 are each `lsls #0x10; lsrs #0x10`, and the value
 * returned in r0 is the u16 counter held in sl the whole way. Same record and
 * same gUnknown_02028DD8 scan as sub_0804769C. */
u16 sub_08047740(struct Unk0804769C *, u16, u16, u16);
void sub_08007F9C(int, int);

/* The pair sub_08040CA4 opens with, both (id, tileBase, paletteNum). The first
 * argument reaches them as a bare `ldrb` off the caller's struct with no
 * re-narrowing, so it is `int` and not `u8`; both return nothing. */
void sub_0804103C(int, int, int);
void sub_08041128(int, int, int);

/* wave 13 (A5), the sub_0805D438 script step's callees. sub_080129E0 is the
 * LCG already matched in src/decomp/c_080129E0.c and returns u32 -- the caller
 * takes it modulo 100 through __umodsi3, which is the unsigned helper and so
 * corroborates the return type. sub_08071908 is compared against zero with a
 * bare `cmp r0,#0`, no `lsls #24`, so it is int-wide rather than bool8. */
u32 sub_080129E0(void);
void sub_0805A95C(void);
void sub_0805E9DC(void);
int sub_08071908(void *);

/* Three more of the screen-setup group, from sub_08080498.
 *
 * sub_08012B70 takes five arguments -- r0..r3 plus one word at [sp] -- and
 * sub_08073574 takes six, r0..r3 plus two words. Both counts are read off the
 * call site's `str rN, [sp, #k]` stores, which are the only evidence available;
 * the widths are `int` because every argument arrives as a bare register or a
 * shifted immediate with no narrowing. sub_0807898C is called for effect with
 * no argument register read and ends `pop {r0}; bx r0`. */
void sub_0807898C(void);
/* Wave 39 (W39-A): sub_08012B70's last THREE arguments retyped int -> u16, and
 * the second void * -> u16 *, off the callee's own prologue. c_080399F8.c had
 * predicted the fifth and deferred the retype for want of caller re-verification;
 * this wave did that -- sub_080399F8, sub_0807FE90 and sub_08080498 were all
 * re-run through trymatch after the change and all three are still byte-
 * identical. They pass 0, 0 and either a constant or an already-cast value, so
 * the narrowing is a no-op at every call site.
 *
 * The width evidence is a SPLIT PROMOTE_MODE pair: `lsls r2,#0x10` and
 * `lsls r3,#0x10` sit in the prologue with NO matching `lsrs`, and those `lsrs`
 * reappear at the use fused with the scale (`lsrs #0xf` for x, i.e. *2, and
 * `lsrs #0xa` for y, i.e. *0x40), while the fifth argument keeps its whole
 * `lsls #0x10; lsrs #0x10` pair at entry. An `int` parameter with an explicit
 * `(u16)` cast emits the narrowing whole at the USE instead and leaves no
 * prologue `lsls` behind -- that spelling was probed and costs those two.
 * The second argument is walked directly as a halfword pointer: `adds r5,r1,#0`
 * is a prologue copy, ahead of the stack-parameter load, which a `void *`
 * copied into a local `u16 *` cannot reproduce (it lands seven instructions
 * later). gUnknown_080A31A4 was retyped u8 [] -> u16 [] to suit; it has no
 * other reader.
 *
 * sub_08012B70 itself is NOT matched -- parked at 87.5% with the instruction
 * stream 1:1 and only register numbers differing. See work/sub_08012B70/. */
void sub_08012B70(u16 *, u16 *, u16, u16, u16);
void sub_08073574(int, int, int, int, int, int);

/* The two text/graphics emitters sub_080852A8 chooses between. Six arguments
 * each -- r0..r3 plus two words at [sp] and [sp, #4]. They differ in the third
 * argument's type and that is real: sub_08014668 takes a u16 tile value read
 * with `ldrh`, sub_080149C0 takes a `u8 *` read with `ldr` out of
 * gUnknown_08610A38. */
/* Wave 21 (W21-A) corrects sub_08014668 on two axes, from the body rather than
 * from the call sites:
 *   Arguments 4..6 are `u16`, not `int`. They are narrowed `lsls #0x10;
 * lsrs #0x10` BEFORE the `gUnknown_03002514 = 0` store, i.e. in the prologue,
 * which is PROMOTE_MODE on a narrow parameter and not a conversion at the
 * sub_080147B4 call -- an `int` spelling emits the same three narrowings but
 * interleaved with the outgoing `str`s, eight instructions later. Byte-neutral
 * at the three existing call sites (c_08084580.c twice, c_080852A8.c once):
 * every argument passed is already a u16 `ldrh` or a constant.
 *   It RETURNS sub_080152EC's result. `pop {r1}; bx r1` where r0 is otherwise
 * free is the value-returning epilogue -- its byte-identical twin
 * sub_080146D4 does the same, and the two void functions next to them
 * (sub_08066BF4, sub_08066C70) pop into r0. Also byte-neutral at the callers,
 * which all discard it. */
struct Unk03001470 *sub_08014668(int, int, u16 *, u16, u16, u16);
void sub_080149C0(int, int, u16 *, u8 *, int, int);
/* Wave 34 (W34-I). Takes the same `u8 *` buffer sub_080149C0 fills as its
 * fourth argument -- sub_0804A1E4 hands it gUnknown_030044E0->unk2c
 * immediately after that call. Returns u8: the one caller stores the result
 * with a bare `strb` and then re-narrows it `lsls #0x18` to test it. */
int sub_08014CEC(u8 *);

/* Takes the raw, UNCLAMPED Interpolate result from sub_080737EC -- the clamp to
 * 0..0xF0 there feeds gUnknown_030024E4 and not this call, which is visible as
 * two separate registers live across the bitfield block. */
void sub_08073714(int);

/* The per-frame tail of sub_08076494 / sub_0807662C: called unconditionally,
 * last, with no argument register read, and both callers ignore any result. */
void sub_080763C0(void);

/* Its fourth parameter is an 8-byte aggregate BY VALUE: sub_08022BB8 passes
 * word 0 in r3 and word 1 in the first outgoing stack slot, with the trailing
 * `1` above it at [sp, #4]. `struct UnkVec` and `struct OamData` are the same
 * eight bytes; the caller fills the OAM view and hands over the vector view. */
void sub_0801C01C(u16, u16, void *, struct UnkVec, int);

/* Both take ONE parameter that their own bodies never read -- a dead leading
 * parameter, which is invisible in the body and only readable at the callers.
 * Both callers set r0 up: sub_08043DAC does `ldrb r0,[r0,#0x1d]` and
 * sub_08080EE4 / sub_08080EF8 do `ldr r0,=gUnknown_03005970; ldr r0,[r0]`.
 * Neither load could survive -O2 if the value were unused at the call, so the
 * parameter is real however dead it is inside.
 *
 * `int` and not a narrow type: sub_08080EE4 passes the whole word out of
 * gUnknown_03005970 with no `lsl #24; lsr #24` in front of the `bl`, which a
 * u8 parameter would have forced. Both are void (`pop {r0}; bx r0`). */
void sub_08043DF4(int);
void sub_08043E18(int);

/* Wave 33, W33-E. The only undeclared callee of the 0x0807F / 0x08080 block,
 * and its prototype reads straight off its own prologue and epilogue.
 *
 * TWO byte-string parameters: r0 and r1 are both copied out at entry (`adds r3,
 * r0, #0` / `adds r7, r1, #0`) and both are then walked with `ldrb`, r0 one byte
 * at a time and r1 indexed by a running counter. A THIRD parameter that the body
 * never reads -- `movs r2, #0` is the first thing done to r2 -- but both callers
 * (sub_0807FA88, sub_080805E0) set it up with `adds r2, r4, #0` from their own
 * proc pointer, and that copy could not survive -O2 if the argument were unused
 * at the call.
 *
 * The u16 return is not a guess from the call sites: the body itself keeps the
 * count pre-shifted in sl and returns `mov r3, sl; lsrs r0, r3, #0x10`, i.e. it
 * truncates to 16 bits before `bx`. Both callers then re-narrow with the usual
 * `lsls #0x10; lsrs #0x10` before the `str`. */
u16 sub_0807F8FC(u8 *, u8 *, void *);
/* Wave 33, W33-E. sub_080800B0 installs it with `sub_08011AAC((void *)fn)`, the
 * VBlank-callback slot, so nothing sets an argument register and nothing reads a
 * result. */
void sub_080801A8(void);
/* Wave 33, W33-E. sub_0807FA34 calls it last with no argument register set up
 * (r0 still holds Proc_Start's return at the `bl`) and discards any result. It
 * is the sibling of sub_08080EE4 named in the note on sub_08043DF4 above. */
void sub_08080EF8(void);

/* Its two callers -- sub_08041E94 and sub_080424FC -- both pass exactly
 * `gUnknown_030040D8`, i.e. the dereferenced pointer global, so the parameter
 * is that struct pointer rather than the `int` a bare `adds r4, r0, #0` would
 * otherwise suggest.
 *
 * The `u8` return is read off the body's own `lsls #0x18; lsrs #0x18` in front
 * of `pop {r1}`, and is NOT firm: both call sites discard the result, so per
 * the forwarding rule in docs/agbcc-codegen.md nothing here proves the width --
 * the shift is equally the re-narrowing of sub_08074484's own u8 return. Revisit
 * when a caller keeps the value. */
u8 sub_080743E8(struct Unk030040D8 *);

/* A three-way range dispatch on the first argument: 0x60..0x9f goes to
 * sub_0803C9D4, 0x20..0x5f to sub_0803CA00, 0x00..0x1f to sub_0803CB40, and
 * anything else falls out doing nothing. `pop {r0}; bx r0`, so void.
 *
 * The second parameter is at least 16 bits: sub_08038690 passes its own
 * incoming r0 straight through with no narrowing, and sub_0803CBA0 itself
 * narrows it with `lsls #0x18; lsrs #0x18` at each of the two uses that forward
 * it -- a cast at a use, not PROMOTE_MODE, so the two inner callees take `u8`
 * and this one does not.
 * The first parameter is `int` on the same evidence from sub_08038690 (a bare
 * `movs r0, #0x60`), but it is the weaker half: sub_080190EC reaches it with
 * `movs r1,#8; ldrsh r0,[r0,r1]`, which an `s16` parameter would also produce,
 * and no call site yet separates the two. */
void sub_0803CBA0(int, int);

/* Four already-promoted definitions that had no prototype, so every new caller
 * hit `implicit declaration` under -Werror. These agree with
 * src/decomp/c_08019260.c, c_08019850.c, c_0804018C.c and c_08074AAC.c by
 * construction; see those files for the evidence behind each type. */
bool8 sub_08019260(void);
/* Wave 34, W34-L. sub_0804AAF8's only call site sets up no argument register
 * and discards nothing (the `bl` is the last instruction before the epilogue),
 * so both the arity and the void return are the weakest types that fit. */
void sub_0804A760(void);
/* Wave 30, W30-E. Both widths are read off the callee, not guessed at the
 * call: it opens `lsls r0,#0x18; lsrs r2,r0,#0x18` (a u8 parameter, used as
 * the index into one of four ROM byte tables) and ends `ldrb r0, [r0]; bx lr`
 * (a u8 return). Its three callers are all still in asm/. */
u8 sub_0804A18C(u8);
bool8 sub_08019850(void);
void sub_0804018C(void *);
void sub_08074AAC(const u8 *, ProcPtr);

/* ---- wave 14 (B): callees of families F011, F024, F025 and F045 ---- */

/* The nine FIRST callees of family F011 (`bl f; ldr r0,=g; movs r1,#N; bl h`).
 * All nullary: r0 is written before it is read in every one of them --
 * sub_08038D7C, sub_08016ED8 and sub_08044BB0 open with a pool `ldr`,
 * sub_080745C0 with a pool `ldr` and a `movs`, and sub_08037F18 is a bare
 * `bx lr` with no body at all. Every one that pushes lr pops into r0, so void;
 * sub_080745C0 is a leaf ending in a bare `bx lr`, where the return type is
 * not recoverable and `void` is the byte-neutral choice (its only caller,
 * sub_08078124, discards). sub_0801A168, sub_080116E8 and sub_08023348 are
 * already declared above and are the same reading. */
void sub_08016ED8(void);
void sub_08037F18(void);
void sub_08038D7C(void);
void sub_08044BB0(void);
void sub_080745C0(void);

/* Registered through sub_0801F024((void *)sub_08039188, 2) by sub_08039264 --
 * the third member of the callback set the sub_0801F024 comment above
 * describes, and the same reading applies: it is a void(void) body that
 * ignores whatever sub_0801F024 hands it, so the `(void *)` cast at the call
 * site is the honest spelling and not a workaround. */
void sub_08039188(void);

/* Walks the byte-stream script in r0 until it sees a 1, calling
 * sub_0801B7C0(cursor, arg) on each opcode and advancing by
 * sub_0808B6B0(cursor) + 1. `pop {r4, r5}; pop {r0}`, so void. The first
 * parameter is genuinely `const u8 *` and not the opaque `const void *` its
 * F011 siblings use -- this one dereferences it (`ldrb r0, [r4]`). The second
 * is `int`: `adds r5, r1, #0` with no shift pair in the prologue, and its only
 * call site (sub_0801B750) passes a `movs`-sized 0, which is byte-identical
 * for every integer type. */
void sub_0801B8A8(const u8 *, int);

/* Family F024's callee. A four-instruction leaf: `str r1,[r0,#4]` then zeroes
 * +0x08 (word) and +0x10 (halfword), then `bx lr`. Leaf with a bare `bx lr`, so
 * the return type is not recoverable -- all six call sites discard, and r0 is
 * still the incoming pointer at exit, so `void` is byte-neutral.
 *
 * The first parameter is `struct Unk03001470 *`, NOT the `void *` a wave-14
 * agent inferred as "the weakest model that fits". The weakest-model rule is
 * the right default for a type nobody else has named, but this one is already
 * named: src/decomp/c_08063A30.c is promoted and matching with that struct,
 * writing ->unk04, ->unk08 and ->unk10, and struct Unk03001470 is defined in
 * unknown-globals.h with exactly those fields. A declaration that disagrees
 * with a promoted definition is a build break, not a weaker claim -- it is
 * what broke the first SPLIT=1 build of wave 14, and per-function trymatch
 * cannot see it because it compiles one unit. The F024 wrappers keep their
 * `void *` parameters and convert implicitly at the call, so nothing is lost
 * by agreeing with the definition. */
void sub_08063A30(struct Unk03001470 *, const void *);

/* Family F025's third callee. Its parameter is an INT and not a pointer:
 * `adds r5, r0, #0` then `adds r6, r5, (&gUnknown_02027F74 + 4)` uses it as an
 * offset, and it stores the same value at +0x54 of the proc it finds with
 * Proc_Find(gUnknown_08616D94) -- which is exactly the field sub_0808789C
 * reads back out. `pop {r0}`, so void. */
void sub_08087B74(int);

/* Family F045's three second callees, all nullary and all void (`pop {r0}`):
 * sub_0802D4A0 is `bl sub_0801A664; bl sub_08034F7C` and the other two are
 * `ldr r0,=<0849A5xx>; bl sub_080193B0`. Because none of them reads r0 before
 * writing it, the `bl sub_0801A168; bl <this>` pair in every F045 member is
 * two statements and a nest is not expressible -- see the wrapper section of
 * docs/agbcc-codegen.md. */
void sub_0802C144(void);
void sub_0802C1B0(void);
void sub_0802D4A0(void);

/* ------------------------------------------------------------------------
 * Wave 15, agent A -- the callees of families F035, F055, F056, F058, F062
 * and F066. Six cold families, twenty members, all 12-20 bytes; the work was
 * entirely here, in the prototypes, and the twenty bodies were one line each.
 * ------------------------------------------------------------------------ */

/* Three of these are PROMOTED AND MATCHED already and were simply never
 * declared -- the wave-14 "promoted but no prototype" trap, which reads like a
 * missing function rather than a missing line. Copied from the definitions,
 * not re-derived:
 *   src/decomp/c_08013AD4.c   void sub_08013AD4(u8 a1)      -- `g |= 1 << a1`
 *   src/decomp/c_08011218.c   void sub_08011218(void)       -- Proc_EndEach
 *   src/decomp/c_08034308.c   void sub_08034308(ProcPtr parent)
 * and sub_0806CC00 is defined in src/title-screen.c, which is upstream's own
 * matching source and must not be edited; `s32` is copied from there. */
void sub_08013AD4(u8);
void sub_08011218(void);
void sub_08034308(ProcPtr);
void sub_0806CC00(s32);

/* Nullary and void, both by the `pop {r0}` epilogue rule and by every call
 * site. sub_0802465C has five callers and sub_0803BCA0 two; not one of them
 * writes r0 before the `bl` or reads it after, so neither takes an argument
 * and neither result is used. This is what makes the `bl`-runs in F035 and
 * F056 sequences of statements rather than nests -- a nest is not expressible
 * when the second callee has no parameter. */
void sub_0802465C(void);
void sub_0803BCA0(void);

/* `s16`, and this is the discriminator the F066 family turns on.
 *
 * sub_0803B48C's own prologue is `lsls r0,#0x10; lsrs r1,r0,#0x10` -- a
 * PROMOTE_MODE zero-extension, which per docs/agbcc-codegen.md is identical
 * for `s16` and `u16` and therefore proves only "narrow". The sign comes from
 * the CALLERS, and all five agree on signed:
 *   0x08035F0E  adds r0,r0,r4; adds r0,r0,r5; lsls #0x10; asrs #0x10; bl
 *   0x08035F5x  movs r1,#0x1a; ldrsh r0,[r2,r1]; bl        (twice)
 *   0x0803Cxxx  movs r1,#0x1e; ldrsh r0,[r0,r1]; bl        (twice)
 *   sub_0803B4DC itself: lsls #0x10; asrs #0x10; bl
 * The first is decisive: a sum of three ints sign-narrowed at the call. With
 * an `int` parameter that site needs an explicit `(s16)` cast and so does
 * sub_0803B4DC's body; with `s16` every one of the five is cast-free.
 *
 * This REFINES the sub_0803B4DC comment higher up this file, which reads its
 * `lsl #16; asr #16` as "a conversion at the USE site inside the body". True,
 * but the conversion is IMPLICIT -- forced by this parameter -- so there is no
 * cast in sub_0803B4DC's source. Nothing in src/decomp/ calls sub_0803B48C, so
 * this declaration costs nothing today. */
void sub_0803B48C(s16);

/* `int`, from the other direction: sub_0801D84C's prologue is a bare
 * `adds r4,r0,#0` and the value goes straight into `muls r0,r7,r0` (r7 = 0x4c)
 * with no shift pair anywhere, so it is at least 32 bits wide; and its caller
 * at 0x0801D840 passes a bare `adds r0,r3,#0` with no conversion.
 *
 * Consequence for sub_08015568, and it is worth stating because it is NOT
 * decidable: the `lsls #0x10; asrs #0x10` in sub_08015568 cannot be an
 * implicit conversion the way sub_0803B4DC's is, so it is either an explicit
 * `(s16)` cast on an `int` parameter or an `s16` parameter with no cast.
 * Probed side by side and the two are BYTE-IDENTICAL (as are `int`-param-plus-
 * cast against an `s16` callee, and `s16` param against an `s16` callee); only
 * a `u16` parameter differs, giving `lsr` where the ROM has `asr`. The `int`
 * spelling is declared below as the weaker contract -- a future caller passing
 * an int then needs no conversion -- but the evidence does not separate them
 * and sub_08015568's single caller (`movs r1,#0x26; ldrsh r0,[r4,r1]`, an s16
 * object) is equally consistent with both. Byte-neutral, so no oracle. */
void sub_0801D84C(int);
void sub_08015568(int);
void sub_08072BBC(int);

/* The 0x08044 per-army funds block (wave 25). Every one of these takes the
 * army slot index that subscripts gUnknown_08499598, passed and compared as a
 * bare `int` at every call site in the block -- no prologue masking anywhere,
 * which per the PROMOTE_MODE rule is positive evidence for a wide parameter.
 *
 * sub_08044094 and sub_08044354 are copied verbatim from the already-promoted
 * definitions in src/decomp/c_08044094.c and c_08044354.c, which had no
 * prototype; the definitions win, so these must agree with them and not be
 * re-derived.
 *
 * sub_0804415C returns a NARROW value: sub_0804423C, sub_08044280 and
 * sub_080442AC each follow the `bl` with a bare `lsls #0x18; cmp #0`, which is
 * a truth test on eight bits and which an `int` return would not emit. Its
 * signedness is not settled -- no caller keeps the value.
 *
 * sub_080441D4 and sub_08044208 return `int` and are compared SIGNED against
 * sub_08044094's result (`cmp r4,r0; bge` in all three predicates), which is
 * what forces that result into an `int` local at each of those call sites --
 * `u32 < int` would have given `bcc`.
 *
 * sub_08044080's and sub_08039634's return values are discarded by their only
 * callers here, both of which are themselves void (`pop {r0}; bx r0`), so
 * `void` is unproved for both -- it is the spelling the source most likely had,
 * and it is byte-neutral at these two sites either way. */
u32 sub_08044094(int);
int sub_0804419C(int);
int sub_080441D4(int);
int sub_08044208(int);
u8 sub_0804423C(int);
void sub_08044354(int);
u8 sub_0804415C(int);
/* `u32` second parameter, not `int`: src/decomp/c_08044080.c has defined it
 * that way since it was promoted, and a promoted definition wins over a
 * weakest-model guess. Caught by tools/proto_check.py before the build --
 * trymatch compiles ONE unit, so this passes every per-function check and
 * fails `make SPLIT=1 compare` with `conflicting types` (wave 25). */
void sub_08044080(int, u32);
void sub_080440A8(int, int);
void sub_0804438C(int, int);
/* Wave 31 (W31-C), the undeclared callees of the 0x08060000 block. Arity is
 * read off the argument registers written immediately before each `bl`, and
 * void-ness off whether r0 is live after it.
 *
 * sub_08025E08 takes three bytes of gUnknown_030046C0 and its result is
 * discarded (sub_080600F0 ends `pop {r0}`). sub_08042634 takes two, sub_08042C24
 * four in registers plus a fifth pushed as `str r4, [sp]` -- that fifth is the
 * `ProcPtr` parent its promoted definition in src/decomp/c_08042C24.c names,
 * NOT an int, and sub_08060474 passes it as PROC_TREE_3 (wave 31 declared it
 * `int` first and proto_check caught it). sub_080425FC takes the one u8
 * recorded above with sub_08041978.
 *
 * sub_08057FA8's result is compared with the `int` gUnknown_030045D8 with no
 * re-narrowing, so it returns `int`; sub_08060ED4's is `cmp r0, #0` with no
 * re-narrowing either. sub_08060FFC's IS re-narrowed -- `lsls #0x18; lsrs #0x18`
 * on return in sub_08060D4C -- which is the tell for a u8 return. */
/* NOT void -- corrected in wave 34 (W34-D). The note above read its void-ness
 * off sub_080600F0 discarding the result, which cannot distinguish "returns
 * nothing" from "returns something the caller ignores". Its own body settles
 * it: both exits load r0 (`adds r0, r4, #0` with sub_08025C98's pointer, or
 * `movs r0, #0`) and it leaves on `pop {r1}; bx r1`, which does not touch r0.
 * The value is sub_08025C98's `void *` passed straight through. Widening void
 * to void * costs the existing caller nothing. */
void *sub_08025E08(int, int, int);
void sub_080425FC(u8);
void sub_08042634(int, int);
void sub_08042C24(int, int, int, int, ProcPtr);
void sub_0802C0CC(void);
void sub_0802C0D8(void);
void sub_0806096C(void);
void sub_080609B8(void);
void sub_08060A20(void);
void sub_08060F00(void);
void sub_08060F74(void);
int sub_08057FA8(int);
/* Wave 32, W32-A: sub_08057FA8's neighbour and sub_08060A20's other counter.
 * `int` on both ends from its own body -- the prologue is a bare
 * `adds r5, r0, #0` with no PROMOTE_MODE narrowing, and it returns the r4 it
 * counts up over gUnknown_08499594, masking the parameter against
 * gUnknown_085D5ABC[type].unk1a. Still assembly; declared here because
 * sub_08060A20 is its only caller. */
int sub_08057F54(int);
int sub_08060ED4(int);
/* Wave 47, W47-E, from the 0x08060 AI block.
 *
 * sub_08060718 takes ONE s16: sub_080606D0 reads its stack local with
 * `movs r1, #0; ldrsh r0, [r5, r1]` at every call, and a `ldrsh` at the CALLER
 * is the s16 readout -- a u16 parameter would have needed the zero-extending
 * `ldrh` there. Its own prologue zero-extends (PROMOTE_MODE does that
 * regardless of signedness, so the prologue proves only the width), and it
 * re-signs the value with `lsls #0x10; asrs #0x10` before handing it to
 * sub_08060894, whose promoted definition already takes s16.
 *
 * sub_08060D4C, sub_08060D78 and sub_08060894 are hoisted from their promoted
 * definitions in src/decomp/c_08060D4C.c and c_08060894.c, which had none.
 * sub_080606D0 is sub_08060D78's first C caller and hands it `mov r0, sp` --
 * the same r0 the preceding `strh` set up -- so the argument is the local's
 * ADDRESS, an out-parameter, which is what that definition already assumes.
 *
 * sub_08060DAC's result is a signed loop count in sub_080606D0 (a `cmp r0, #0;
 * ble` zero-trip guard over a countdown), so `int`. sub_08057FE8's is stored
 * whole into the `int` gUnknown_030045D8 with no re-narrowing. sub_08060930,
 * sub_08060A7C and sub_08060AB0 are called for effect only, with no argument
 * register written before the `bl` and no use of r0 after it. */
void sub_08060718(s16);
void sub_08060D4C(void);
void sub_08060D78(s16 *);
void sub_08060894(s16);
int sub_08060DAC(void);
int sub_08057FE8(int);
void sub_08060930(void);
void sub_08060A7C(void);
void sub_08060AB0(void);
/* Wave 32, W32-A: hoisted from the definition in src/decomp/c_08061DA8.c, which
 * had no declaration. sub_08060894 is its first C caller and agrees -- it takes
 * the result with a bare `adds r2, r0, #0`, tests it against 0 and hands it to
 * `__divsi3` as a signed divisor, none of which re-narrows. */
int sub_08061DA8(int);
u8 sub_08060FFC(void);
void sub_08039634(int, int);
void sub_08044560(void);
/* sub_08039ACC's first three parameters are `u16` and its fourth is wide, read
 * straight off its own prologue: r0, r1 and r2 each get `lsls #0x10; lsrs
 * #0x10` into a callee-saved register, while r3 goes to `adds r6,r3,#0` with no
 * masking at all and is then tested `cmp r6,#0`. Its single caller
 * sub_08044B08 narrows all three of ITS arguments by 24 instead, which is that
 * function's own `u8` parameters under PROMOTE_MODE and not a conversion for
 * this callee -- a u8 value needs no instruction to reach a u16 parameter. */
void sub_08039ACC(u16, u16, u16, int);
/* Copied from the promoted definition in src/decomp/c_080443C4.c, which had no
 * prototype; sub_080447EC and sub_0804483C both tail-forward their own argument
 * to it untouched, so neither constrains the type further. */
void sub_080443C4(ProcPtr);
void sub_080447EC(ProcPtr);
void sub_0804483C(ProcPtr);
void sub_08044B08(u8, u8, u8);
/* Returns bool8, not `int` (wave 28, W28-B). The definition is byte-neutral
 * either way -- both arms are `movs r0,#0` / `movs r0,#1` -- so it never had
 * an oracle there. All THREE call sites supply one: sub_08043DF4,
 * sub_08043E18 and sub_08080F0C each truth-test the result with a BARE
 * `lsls r0, r0, #0x18`, which is what agbcc emits for a byte-wide return and
 * an `int` return does not produce at all. c_08044BA0.c was retyped and
 * re-verified. */
bool8 sub_08044BA0(int);
int sub_0804440C(struct Unk030040D8 *);
int sub_0804443C(struct Unk030040D8 *);
int sub_08044460(struct Unk030040D8 *);
int sub_08044488(struct Unk030040D8 *);
int sub_080444B4(struct Unk030040D8 *);

/* Family F058's callee, and the family is three calls to it with 0, 1 and 2.
 * The parameter is `int`, not the `u8` its body suggests: the prologue is
 * `adds r4,r0,#0` THEN `lsls r0,r4,#0x18; lsrs r0,r0,#0x18` -- copy-then-narrow,
 * which per docs/agbcc-codegen.md is a cast at a use (it feeds
 * sub_0803CCB8(a, gUnknown_0200B204)) and not a narrow parameter; the saved r4
 * is switched on unnarrowed as 0/1/2 immediately afterwards.
 *
 * It RETURNS a value and that is decisive rather than inferred: all three F058
 * members end `pop {r1}; bx r1`, the value-returning epilogue, so they are
 * non-void; and a narrow return type here would put `lsl; lsr` after the `bl`
 * in each of them, because agbcc re-narrows on the CALLEE's declaration. There
 * is none, so `int`. (The value itself is r3, which is 0 on every path.) */
int sub_08005474(int);

/* Family F055's two same-callee members call one of these twice, with 0 then
 * 1. Both are `u16` parameters: each opens `lsls r0,#0x10; lsrs r4,r0,#0x10`
 * and the saved r4 is then used WITHOUT a second shift pair -- as an index
 * (`lsls r0,r4,#2`), a compare against an `ldrh`, and `strh r4,[r0]`. Per the
 * PROMOTE_MODE rule the entry pair alone cannot separate `s16` from `u16`; the
 * absence of any `asr` at the uses is what picks unsigned.
 * sub_0804FF44's third caller corroborates rather than contradicts: 0x0804C0FC
 * passes `adds r0,r7,#0` with no narrowing, but r7 is itself
 * `lsls r0,#0x10; lsrs r7,r0,#0x10` from that function's own prologue, so gcc
 * already knows the value fits and drops the conversion.
 * Byte-neutral at every F055 call site regardless -- all six pass a literal. */
void sub_0804B3E0(u16);
void sub_0804FF44(u16);

/* Family F062's second pair. sub_0808606C and sub_08086688 both open
 * `adds r4,r0,#0` and dereference it at +0x30/+0x4c/+0x5c and +0x48..+0x6a, so
 * each takes the object in r0; `ProcPtr` (= `void *`) is the weakest model and
 * nothing has named the struct yet. Both `pop {r0}`, so both void.
 * sub_08087C14's parameter is an INT and not a pointer, on exactly the
 * evidence that fixed its sibling sub_08087B74 above: it is added to
 * &gUnknown_02027F78 as an offset, and the caller at 0x080879xx reads it back
 * out of +0x54 of a proc -- the field sub_08087B74 stores it into. */
/* Wave 44 (W44-C) RETYPED sub_0808606C's parameter from `ProcPtr` to its own
 * struct, on register-count evidence from the now-matched definition. The ROM
 * keeps the object in ONE pseudo (`push {r4,lr}; adds r4,r0,#0`, everything
 * through r4). `ProcPtr` forces the body to open
 * `struct Unk8606CProc *proc = procp;`, and because the pointer is live across
 * both `bl sub_08086A58` calls that copy does not coalesce: agbcc emits
 * `push {r4,r5,lr}; adds r4,r0,#0; adds r5,r4,#0` and then splits the shared
 * `str r0,[rN,#0x30]` tail across r4 and r5, which also blocks the cross-jump
 * the ROM performs there. +4 bytes and two wrong registers, from the
 * declaration alone. Only a forward declaration lives here -- the layout stays
 * in src/decomp/c_0808606C.c, per the parameter-struct rule. ProcPtr is
 * `void *`, so sub_08086058's `sub_0808606C(proc)` converts implicitly and is
 * byte-neutral; it was re-verified. */
struct Unk8606CProc;
void sub_0808606C(struct Unk8606CProc *);
void sub_08086688(ProcPtr);
void sub_08087C14(int);
/* Wave 44 (W44-C), read off its two call sites in sub_0808606C (now matched).
 * Three 32-bit arguments and no narrowing at either call: argument 1 is a
 * whole-word `ldr` of the proc's +0x5c (or that minus one), argument 2 is
 * `gUnknown_03005928` (declared `u32`) plus zero or one, argument 3 is the
 * literal -1 or 0. Both call sites discard the result, so `void` is the
 * weakest model; the definition at 0x08086A58 is still asm. */
void sub_08086A58(int, u32, int);

/* ---- wave 15 (B): callees of families F046, F049, F057, F060, F061, F063 ----
 *
 * Four of these six are already PROMOTED and had no prototype, which under
 * -Werror reads as a missing function rather than a missing line; the
 * signatures are copied verbatim from the definitions rather than re-derived:
 *   src/decomp/c_080432E0.c  int   sub_080432E0(int)
 *   src/decomp/c_0800B4F0.c  int   sub_0800B4F0(int, int)
 *   src/decomp/c_0802C62C.c  bool8 sub_0802C62C(void)
 *   src/decomp/c_0802C660.c  bool8 sub_0802C660(void)
 * The two bool8s are corroborated by their only call sites, the four members
 * of family F046: sub_0802C62C's result carries `lsls #0x18; lsrs #0x18`
 * before `cmp #1` (value kept, so eight bits wide) and sub_0802C660's a bare
 * `lsls #0x18` before `cmp #0` (truth test). An `int` return would emit
 * neither. */
int sub_080432E0(int);
int sub_0800B4F0(int, int);
bool8 sub_0802C62C(void);
bool8 sub_0802C660(void);

/* The 0x0802C0E8 block's own callees and forwarders (wave 24, W24-A).
 *
 * sub_0802C0E8 takes a parameter it never reads -- its body opens by loading
 * gUnknown_03003FC0 straight over r0 -- so the width is settled entirely at its
 * one call site, sub_0802C118: that hands it the u16 global gUnknown_030033EC
 * with a bare `ldrb`. A `u8` parameter is what turns a u16 load into a byte
 * load; an `int` one would have emitted `ldrh`. Returns nothing (`pop {r0};
 * bx r0`).
 *
 * sub_0802C154 matches the promoted definition in src/decomp/c_0802C154.c;
 * declared here because sub_0802C16C and sub_0802C184 are its first callers.
 *
 * sub_08016D30's widths are read off its own prologue, which is unambiguous:
 * `lsls r0,#0x10; lsrs r4,#0x10` then `lsls r1,#0x18; lsrs r7,#0x18`, i.e.
 * (u16, u8). Its sole caller sub_0802C1D0 corroborates the u16 -- sub_08016D04
 * returns s8 and the conversion to u16 is the fused `lsls #0x18; asrs #8;
 * lsrs #0x10` triple, which neither an `int` nor an `s8` parameter produces.
 *
 * sub_08078E14 returns `int`: sub_0802C604 range-checks the result with signed
 * `bgt`/`blt` and no `lsls`/`lsrs` re-narrowing in front of them, and agbcc
 * re-narrows a narrow-returning callee at every call site.
 *
 * The three void/void entries are called with no argument register set up and
 * their results discarded; the arity is a floor, not a proof. */
void sub_0802C0E8(u8);
void sub_0802C154(int);
void sub_08016D30(u16, u8);
int sub_08078E14(void);
void sub_0803B828(void);
void sub_080366A4(void);
void sub_08028CD8(void);

/* More of the same block (wave 24, W24-A extension).
 *
 * sub_08016DB8's u16 is read off its own prologue (`lsls r0,#0x10;
 * lsrs r0,#0x10`), and its caller sub_0802CFDC corroborates it with the same
 * fused `lsls #0x18; asrs #8; lsrs #0x10` s8-to-u16 conversion sub_0802C1D0
 * uses on sub_08016D30.
 *
 * sub_080442AC and sub_08044280 take `int` and return `int`, both on bare
 * evidence: each opens `adds r5, r0, #0` with no masking of the incoming
 * register (the wave-21 wide-parameter rule), and their shared caller pair
 * sub_0802C820 / sub_0802C848 feeds the result straight into an `ands` with no
 * `lsls #0x18; lsrs #0x18` in front of it -- which agbcc emits at every call
 * site of a narrow-returning callee, so the 0/1 bodies do NOT make these bool8.
 *
 * sub_080344F0 takes `int`, the weakest type that fits, and this is one place
 * the wide-parameter rule does NOT apply: the argument's only use in the callee
 * is `strb r0, [r2, #6]`, so a u8 parameter's PROMOTE_MODE extension would be
 * dead and deleted. u8 and int are indistinguishable here. Contrast
 * sub_0802CD54, where the u8 parameter feeds a WORD `str` and the shift pair
 * duly appears.
 *
 * The void/void entries are called with no argument register set up and their
 * results discarded; the arity is a floor, not a proof. */
void sub_08016DB8(u16);
void sub_080344F0(int);
int sub_080442AC(int);
int sub_08044280(int);
void sub_08034F10(void);
void sub_080485AC(void);
void sub_08046764(void);
void sub_0802C280(void);

/* Wave 36 (W36-B). Three promoted-but-undeclared definitions, copied verbatim
 * out of src/decomp/ rather than re-derived: sub_08043898 and sub_080438FC
 * from src/decomp/c_08043834.c, sub_080442E4 from src/decomp/c_080442E4.c.
 * sub_080442E4's `bool8` is corroborated at sub_080436DC's call site, which
 * truth-tests the result with `lsls r0, #0x18` -- the narrowing agbcc emits
 * for a narrow-returning callee and does NOT emit for sub_08044280 (see the
 * note above), whose result the same function casts `(u8)` by hand exactly as
 * src/decomp/c_080442E4.c already does. */
void sub_08043898(int, int, int);
void sub_080438FC(int, int, int);
bool8 sub_080442E4(int);

/* Wave 36 (W36-B), the sub_08042650 group. The first three are copied verbatim
 * out of src/decomp/ (c_08042E2C.c, c_080265B0.c, c_08024058.c) rather than
 * re-derived -- all three are promoted, byte-verified and were undeclared.
 * sub_080409E8 is the only one derived here, from sub_08042650's call site: it
 * is 0x08042's undeclared callee and nothing in src/decomp defines it. FIVE
 * arguments -- `sub sp,#4` plus `str r3,[sp]` immediately before the `bl`, so
 * the pushed word is argument 5, not a local -- and `void`, since the result
 * is discarded and the call is followed by an unconditional branch. All five
 * are wide: arguments 1 and 2 arrive as plain `ldrh`s of gUnknown_03003100's
 * u16 cursor halves with no further narrowing, which an `int` parameter
 * produces and a `u16` parameter would not distinguish, so `int` is the
 * weakest type that fits. */
int sub_08042F14(int);
void sub_080265B0(u8, u8);
void sub_08024058(s16, s16);
void sub_080409E8(int, int, int, int, int);

/* The 0x0802CB-0x0802CD predicate cluster's callees (wave 24, W24-A).
 *
 * sub_080422A8, sub_080421D0 and sub_0804223C all take their coordinate pair as
 * s16, read straight off their own prologues: each opens `lsls rN,#0x10;
 * asrs rN,#0x10` on the coordinate registers (sub_080422A8's first is
 * `asrs #0xf`, the same sign extension fused with the `* 2` its index needs).
 * The two three-argument ones additionally open `adds r7, r0, #0` with no
 * masking, so the leading pointer is wide, and both immediately do
 * `ldrb r4,[r7]` -- struct Unk030040D8's unk00.
 *
 * All three return bool8: their call sites narrow the result with
 * `lsls #0x18; lsrs #0x18` before `cmp #1`, the value-kept form, which agbcc
 * only emits for a narrow-returning callee.
 *
 * sub_0802C8F8 and sub_0802C958 return bool8 on the weaker truth-test form
 * (`lsls #0x18; cmp #0`) at their four call sites in this same block, and are
 * called with no argument register set up. */
bool8 sub_080422A8(s16, s16);
bool8 sub_080421D0(struct Unk030040D8 *, s16, s16);
bool8 sub_0804223C(struct Unk030040D8 *, s16, s16);
/* Wave 38 (W38-K). Two more members of the same predicate cluster, both already
 * PROMOTED (src/decomp/c_0804209C.c, src/decomp/c_0804247C.c) and neither
 * declared anywhere until now -- signatures copied from the definitions except
 * for sub_0804247C's return, corrected below.
 *
 * sub_0804247C was promoted as `int`. It is not: it is bool8, on exactly the
 * wave-24 evidence recorded above for sub_080422A8. Its ONLY caller,
 * sub_08041758, narrows the result with `lsls #0x18; lsrs #0x18` before
 * `cmp #1` -- the value-kept form agbcc emits only for a narrow-returning
 * callee, never for an `int` one. The definition's body is byte-neutral either
 * way (it returns literal 0 or 1, so there is nothing to re-narrow), which is
 * why `int` survived promotion unchallenged; c_0804247C.c was re-run through
 * trymatch after this edit and still matches byte-for-byte.
 * This also brings it into line with sub_0804236C, its near-twin: the two are
 * the same shape over different tables and their two callers, sub_0804151C and
 * sub_08041758, differ in ONE instruction (the `bl` target) and nothing else --
 * which is only possible if the two callees agree on return width. */
bool8 sub_0804236C(s16, s16);
bool8 sub_0804247C(s16, s16);
bool8 sub_0802C8F8(void);
bool8 sub_0802C958(void);
/* sub_0802CBA0 is matched in wave 24 and four of its neighbours call it
 * (sub_0802CC90, sub_0802CCCC, sub_0802CBC8, sub_0802CC04), all with the
 * `lsls #0x18; cmp #0` truth test that its bool8 return produces. */
bool8 sub_0802CBA0(void);

/* Family F061's three callees are one shape -- (x, y) cell predicates on the
 * gUnknown_08499590 screen, the same key sub_080015E4 and sub_0800B4F0 use --
 * and sub_0800977C is the only one that was still undeclared. Both parameters
 * arrive as a bare `adds rN, r0, #0` / `adds rN, r1, #0` with no narrowing, so
 * `int` on both, and it ends `pop {r1}; bx r1`. The return is `int` and not
 * bool8: all four F061 call sites feed the result straight into `lsls r0, #3`
 * / `#2` / `#1` or a bare `orrs`, with no `lsls #0x18; lsrs #0x18` in front of
 * it -- and agbcc re-narrows a narrow-returning callee at every call site. */
int sub_0800977C(int, int);

/* The CpuFastSet half of the pair sub_08011C68 heads (see that comment).
 * Identical reading: r0 and r1 are passed through to CpuFastSet untouched, so
 * `const void *` and `void *`; r2 is a byte count the body narrows itself
 * (`lsls r2,#0x10; lsrs r2,#0x12`, i.e. (u16)n / 4), so it is NOT a narrow
 * parameter; `pop {r0}`, so void. Where sub_08011C68 picks CpuSet or
 * CpuFastSet on `n & 0x1f`, this one is unconditional. */
void sub_08011C90(const void *, void *, u16);

/* ---- wave 15 (C): callees of the four gUnknown_08499590 screen readers ----
 *
 * sub_080433F8 is already PROMOTED (src/decomp/c_080433F8.c) and had no
 * prototype; the signature is copied verbatim from the definition.
 *
 * sub_08043070 returns u16, and that is a READOUT rather than a guess: all
 * four of sub_08024ABC's call sites narrow the result with `lsls #0x10;
 * lsrs #0x10` before storing it, and the two locals it feeds are compared
 * with `bhi` -- an UNSIGNED compare, so the locals themselves are `u32` and
 * cannot be the source of the narrowing. agbcc re-narrows a narrow-returning
 * callee at every call site, which is exactly what is there. Its five
 * arguments arrive as two `ldrb`s off gUnknown_08499598[army], two more
 * `ldrb`s (or the literal 0x19), and one stack word; nothing narrows on the
 * caller side, so `int` throughout.
 *
 * sub_08042D50's result is compared `cmp r5, r0; bgt` -- signed -- so `int`,
 * and neither argument is narrowed at any of its two call sites. */
int sub_080433F8(int, int, int);
/* WAVE 28 (W28-B) REFUTES THE u16 ABOVE: sub_08043070 returns `int`. The
 * argument above reads the four call sites and never read the CALLEE, and the
 * callee is decisive -- a u16-returning agbcc function narrows its own result
 * in its epilogue, and 0x08043070 ends `pop {r4,r5,r6}; pop {r1}; bx r1` with
 * no `lsl #0x10; lsr #0x10` anywhere. Declared u16 the definition gains that
 * pair and misses by 4 bytes. The call-site narrowing is real but comes from
 * an explicit (u16) cast in sub_08024ABC, which is forced independently: its
 * v1/v3 are compared `bhi`, so they are u32 and cannot themselves narrow.
 * c_08024ABC.c now spells the casts and was re-verified. */
int sub_08043070(int, int, int, int, int);
int sub_08042D50(int, int);
/* Wave 34 (W34-H). A number/icon blitter sub_08039F80 calls four times. The
 * FIRST parameter is u16: two of the four call sites carry a
 * `lsls #0x10; lsrs #0x10` on it and the other two do not, and the two that do
 * not are exactly those whose argument is a `ldrb` plus a small constant --
 * provably in range, so agbcc drops the narrowing. An `int` parameter would
 * have narrowed at none of them. The third parameter is `int`: one site hands
 * it sub_08042D50's word result with no narrowing at all. */
/* RETYPED wave 41 (W41-B), and the body is now matched, so this is read off the
 * definition rather than the call sites. Parameter 3 is UNSIGNED: the body
 * divides and takes the remainder with __udivsi3 / __umodsi3, and an `int`
 * emits __divsi3 / __modsi3 -- a diff at the first call. Parameter 2 is 16-bit,
 * not `int`: the prologue's `lsls r1, #0x10` is the narrowing, fused with the
 * `& 0xFF` the value is passed under. Both changes are byte-neutral at the only
 * caller (c_0803A07C.c, which passes the constants 0x28 and 0x30), and it was
 * re-verified. Same two changes to sub_0802BD54, its twin. */
void sub_0802BCF0(u16, u16, u32);
/* Wave 32 (W32-B): SIX arguments, off its own prologue -- r0..r3 plus
 * `ldr r3,[sp,#0x14]` and `ldr r4,[sp,#0x18]` past a 4-register push and a
 * 4-byte frame. It parks the first in gUnknown_03004480 with `strh`, forwards
 * arguments 2..5 to the gUnknown_030013EC callback (four registers plus one
 * stack word through `bl _call_via_r4`) and restores gUnknown_03004480 from
 * gUnknown_030033EC afterwards. Nothing narrows any of them, so `int`. */
void sub_0802026C(int, int, int, int, int, int);
/* Wave 32 (W32-B): one `struct UnitRecord *`, read off its own body -- it
 * passes `unk00` to sub_08042D1C and compares the result against the 7-bit
 * bitfield at +6 (`ldrb; lsls #0x19; lsrs #0x19`), which is that type's
 * unk06_0. Returns the smaller of the two, so `int`. */
int sub_08058224(struct UnitRecord *);
/* Wave 32 (W32-B): four arguments, all forwarded to PutSpriteExt -- r3 becomes
 * its first, r2 its fifth, and r0/r1 are masked with 0xfffffe00 / 0xffffff00
 * and re-ORed, i.e. coordinates. Nothing narrows them, so `int` throughout. */
void sub_08043B60(int, int, u32, u32);
/* Wave 31 (W31-C), the extension blocks 0x08021000 / 0x08038000 / 0x08057000 /
 * 0x08058000. Everything below with `(void)` is reached by a bare `bl` with no
 * argument setup and its r0 discarded by the caller's `pop {r0}`.
 *
 * The four that already have promoted definitions are declared FROM those
 * definitions and not from these call sites: sub_0803CA28 (u32, u8),
 * sub_08038548, sub_0803B8B8, sub_0803BCB8, sub_0801F838 (u8) and
 * sub_08042D1C.
 *
 * sub_08038474 is NOT void: `push {lr} ... pop {r1}; bx r1` leaves r0 alone
 * across the epilogue, so it returns sub_08037DA4's result, which that
 * function's own declaration already types `int`. */
void sub_0803CA28(u32, u8);
void sub_08038484(void);
/* Wave 43 (W43-F). VOID and nullary: no argument register is read before being
 * written and the epilogue pops the return address into r0 without setting a
 * result. It has NO callers anywhere in the ROM, so nothing else constrains
 * it -- it is the per-map setup routine that fills gUnknown_0202FDEC. */
void sub_08038240(void);
void sub_08038548(void);
void sub_08038568(void);
int sub_08038434(void);
int sub_08038474(void);
/* Wave 43 (W43-E). Both MATCHED, so these two signatures come from their own
 * definitions rather than from a call site.
 *
 * sub_08038368 is VOID and takes THREE arguments: the epilogue pops the return
 * address into r0 and sets no result, and r0/r1/r2 are each copied to a
 * callee-saved register in the prologue before anything is written. It appends
 * one gUnknown_0200C420.unk38[] record -- a1 is the id it first searches the
 * live run for, a2 the bits-8..19 field and a3 the bits-20..31 field. a1 is
 * `int` and NOT `u8`: sub_08038484, its only caller, passes the int expression
 * `gUnknown_03003FC0.unk02 - 0x8a` with no re-narrowing before the `bl`, which
 * a narrow parameter would force. a2 and a3 arrive as bare `ldrh` results, so
 * `u16` would fit them equally well (PROMOTE_MODE zero-extends either way and
 * the bitfield stores mask to 12 bits regardless) -- `int` is the weakest type
 * that fits and nothing in the tree discriminates.
 *
 * sub_080381C0 is nullary: no argument register is read before being written,
 * and it is reached only from sub_08038240. It returns a plain 0/1 that
 * nothing narrows. */
void sub_08038368(int, int, int);
/* Wave 43 (W43-F) RETYPED this from `int` to `bool8`, on its CALLER rather than
 * its body -- W43-E's note above reads "returns a plain 0/1 that nothing
 * narrows", and its one caller does narrow it. sub_08038240 truth-tests the
 * result with `lsls r0,#0x18; cmp r0,#0`, and that leading shift is agbcc
 * re-narrowing a sub-word return; an `int` return truth-tests with a bare
 * `cmp r0,#0` and no shift. Byte-neutral for sub_080381C0 itself, whose two
 * arms return the literals 0 and 1 and so need no narrowing either way -- its
 * match was re-verified after this change.
 *   WHAT MAKES THIS ONE DECISIVE is that sub_08038240 feeds the result STRAIGHT
 * into the `if`, with no local in between. Probed both ways: `if (int_fn())`
 * emits a bare `cmp r0,#0` and `if (bool8_fn())` emits `lsl #0x18; cmp r0,#0`.
 * Do NOT generalise the shift on its own -- where the result lands in a local
 * first, a `u8` local reproduces it from an `int` return and the tell proves
 * nothing. See the sub_0807821C note below, which made exactly that error. */
bool8 sub_080381C0(void);
/* Wave 43 (W43-F). A saturating add into the two words of gUnknown_0808E558's
 * record, each clamped at 9999 (0x270F) with an UNSIGNED `cmp; bhs`. VOID: the
 * epilogue is `pop {r4,r5}; pop {r0}; bx r0`, which pops the return address
 * into r0 and sets no result.
 *   `u32` and NOT the `int` this declaration first carried: src/decomp is
 * authoritative and c_080176C0.c was promoted long ago as `void
 * sub_080176C0(u32 a)`. Caught by tools/proto_check.py, which is the ONLY
 * check that sees it -- try_match compiles one unit and sub_08038240 matched
 * byte-for-byte under both spellings, because its argument is an unnarrowed
 * `ldrh` result that needs no conversion either way. The unsigned clamp
 * corroborates the promoted type. */
void sub_080176C0(u32);
/* Wave 43 (W43-F), all four read off their own prologues for sub_08038484.
 *
 * sub_0803BADC and sub_08045790 are nullary: each is `push {lr}; ldr r0,=<proc
 * script>; movs r1,#3; bl Proc_Start`, so no argument register is read before
 * being written and neither sets a result.
 *
 * sub_0807823C's whole body is `bx lr` -- it does nothing at all. Its arity is
 * therefore invisible from the body and comes from the one call site, which
 * sets up r0 only. `int` because that site passes the int expression
 * `gUnknown_03003FC0.unk02 - 0x8a` with no narrowing before the `bl`.
 *
 * sub_0807821C returns `gUnknown_08615194[a].unk02 & 0x10` as a literal 0 or 1
 * through two arms. Its parameter is scaled `(a * 2 + a) << 4`, a 0x30 stride,
 * from the same unnarrowed int expression as sub_0807823C's.
 *   ITS RETURN WIDTH IS UNPROVEN and bool8 is a CONVENTION, not a measurement.
 * This note previously claimed the width was forced because sub_08038484
 * re-narrows the result with `lsls #0x18; lsrs #0x18`, "which an `int` return
 * would not produce". That is wrong, and W43-E's challenge is what got it
 * probed: sub_08038484 assigns the result to a LOCAL first, and an `int` return
 * assigned into a `u8` local emits exactly the same shift pair. `bool8` + an
 * `int` local and `int` + a `u8` local are byte-identical, so the call site
 * cannot discriminate and there is no oracle at all here -- bool8 is kept only
 * because the body returns literal 0/1 like its sibling sub_0803861C.
 *   CONTRAST sub_080381C0 above, where the same tell IS decisive: its caller
 * truth-tests the result directly with no local to absorb a narrowing. The
 * discriminator is the absence of an intervening local, NOT the shift. */
void sub_0803BADC(void);
void sub_08045790(void);
void sub_0807823C(int);
bool8 sub_0807821C(int);
void sub_080346FC(void);
void sub_0803BCB8(void);
void sub_0803B8B8(void);
/* Wave 49, W49-G. CORRECTED from (u16, u16, u8 *): the first two parameters are
 * INT. sub_0802042C's own prologue is `adds r5,r0,#0; adds r7,r1,#0` with no
 * PROMOTE_MODE narrowing at all, which settles it directly, and the wrong width
 * was invisible until a differently-shaped caller appeared: sub_0805D648 passes
 * two SIGN-EXTENDED s16 parameters (`lsls #0x10; asrs #0x10`) and the ROM hands
 * them over with a bare `adds r0,r4,#0`, where a u16 formal makes agbcc emit a
 * zero-extension pair on top of the sign extension it already needs for the
 * comparisons that follow. Byte-neutral at the only C caller,
 * src/decomp/c_08038BE0.c, which passes two u16 members -- re-verified with
 * try_match after the change. */
void sub_0802042C(int, int, u8 *);
void sub_080386EC(int);
void sub_08038B84(void);
void sub_080389D8(void);
void sub_0803832C(void);
void sub_08038BE0(void);
void sub_08026768(void);
void sub_08026924(void);
void sub_08026BAC(void);
void sub_08035490(void);
void sub_0803E3D8(void);
void sub_080455CC(void);
/* Wave 37, W37-P2. Never declared when it was promoted; the signature is
 * copied verbatim from the definition in src/decomp/c_080452C0.c. */
void sub_080452C0(int, int, int);
void sub_0804C0FC(int);
void sub_080566C8(int);
void sub_08057138(void);
void *sub_08057D58(int, int, int);
int sub_08042D1C(int, int);
void sub_0801F838(u8);
/* wave 49: both were promoted with no declaration anywhere; signatures copied
 * verbatim from the definitions in src/decomp/c_0801F6F0.c and
 * src/decomp/c_0801F888.c, which sub_0801F4B4 calls from another unit. Both
 * re-verified by trymatch exit code after the declarations were added. */
void sub_0801F6F0(u8, u8, u8);
void sub_0801F888(int);
void sub_0801FD9C(int);
/* Wave 35 (W35-H): RETYPED from `void sub_08020354(int, int)`. The old
 * two-argument spelling contradicts the callee's own prologue, which saves r0,
 * r1 AND r2 (`adds r4,r0,#0; adds r5,r1,#0; adds r6,r2,#0`) and then narrows
 * only the first two with `lsls #0x10; lsrs #0x10` -- u16 parameters, per the
 * PROMOTE_MODE rule. r2 is a gUnknown_08499594 ELEMENT pointer: the body
 * recovers the index as `r2 - gUnknown_08499594` through the usual *5/*17/*257
 * shift-add chain for the 12-byte stride. sub_0802CA78 and sub_0802CB20 both
 * pass three arguments at the call site. No .c file used the old declaration,
 * so nothing had to be re-verified. */
void sub_08020354(u16, u16, struct UnitRecord *);
/* Wave 35 (W35-H): sub_080203C0's own prologue is `adds r3,r0,#0; adds r2,r1,#0;
 * cmp r3,#0; ble` -- two int parameters, signed. Its result is discarded at
 * sub_0802CC40's call site. */
void sub_080203C0(int, int);
int sub_08058744(void);

/* Wave 35 (W35-H). Four 0x0804xxxx predicates and one 0x08019xxx refresh, all
 * previously undeclared. Each reads NO argument register before writing it
 * (sub_0804151C, sub_08041758 and sub_08019E68 all open `push {r4-r7}; mov
 * r7,sb; mov r6,r8; push` and then load a global; sub_080416A4 the same with
 * sl as well), so all four are nullary. The three predicates are `int` and not
 * `bool8`: every call site in the 0x0802C block tests them with a bare
 * `cmp r0,#0` and NO `lsls r0,r0,#0x18`, which is exactly the discriminator
 * that separates them from sub_0802C8F8 / sub_0802CBA0 / sub_08042084 two
 * instructions earlier in the same functions. */
int sub_0804151C(void);
int sub_08041758(void);
int sub_080416A4(void);
/* CORRECTED to `void` in wave 40 (W40-F), from `int`. The old reading came
 * from the prologue shape; the EPILOGUE settles it and says the opposite.
 * sub_08019E68 ends `pop {r4, r5, r6, r7}; pop {r0}; bx r0` -- it pops the
 * return address THROUGH r0, destroying anything r0 held, which agbcc only
 * does when r0 is dead at the return. A function returning a value ends
 * `pop {r1}; bx r1`, which is what an `int` declaration produces here and is
 * 2 bytes different. Its two callers (src/decomp/c_0802CDA4.c and
 * c_0802CE04.c) both discard the result, so nothing else moves. */
void sub_08019E68(void);

/* Wave 40 (W40-F). Draws the option list, one sub_08014A5C row per selectable
 * entry, then flushes the text buffer to 0x06007000. Its one caller is
 * sub_08019E68 just above, which discards nothing because there is nothing to
 * discard: same `pop {r0}` epilogue, so void. The parameter is the same
 * gUnknown_03001470 slot overlay sub_08019A60 and sub_08019E68 use -- see
 * struct Unk8019A60 in include/unknown-globals.h. */
/* Wave 40, W40-H's fix, KEPT -- with its diagnosis corrected by W40-F, because
 * the version of this comment written at the time says the tag is missing and
 * it is not.
 *
 * What happened: W40-F added the prototype below in one turn and the struct
 * definition to unknown-globals.h in the NEXT one. global.h includes
 * unknown-globals.h before this header, so the two together are fine -- but for
 * the one turn in between, this prototype referenced an undeclared tag, agbcc
 * emitted "`struct Unk8019A60' declared inside parameter list", and -Werror
 * made it fatal for EVERY compile in the tree. W40-H compiled inside that
 * window and read the state correctly. The struct has been in
 * unknown-globals.h since, next to gUnknown_08499578.
 *
 * The forward declaration stays: it costs nothing and it makes this header
 * self-contained, so the same mistake cannot break the tree again if the
 * include order in global.h ever changes. Do not replace it with a concrete
 * type here -- the definition belongs in unknown-globals.h.
 *
 * The transferable lesson is the ordering: a prototype naming a new tag and the
 * tag's definition must land in the SAME edit, or every other agent in the wave
 * is blocked until the second one arrives. */
struct Unk8019A60;
void sub_08019C40(struct Unk8019A60 *);

/* Wave 35 (W35-H): promoted in src/decomp/c_0802C1F0.c, src/decomp/c_080425B8.c
 * and src/decomp/c_0802D40C.c and still undeclared; signatures copied verbatim
 * from the definitions. The four 0x0802D4xx wrappers are `void (void)` there,
 * yet sub_0802CDA4 calls them through a table with three arguments -- which is
 * why that function's local table is declared `void (*[4])()` with no prototype
 * rather than with a parameter list. */
void sub_0802C1F0(const u8 *, u8 *, int);
void sub_080425E0(u8);
void sub_08042618(int, int);
void sub_0802D40C(void);
void sub_0802D41C(void);
void sub_0802D42C(void);
void sub_0802D43C(void);

/* Wave 35 (W35-H). Promoted definitions that had no declaration; signatures
 * copied verbatim from src/decomp/c_0802D33C.c, c_0802DFC8.c and c_0802E698.c. */
int sub_0802D33C(void);
void sub_0802DFC8(void);
void sub_0802E698(void);
void sub_0802E6C0(void);
void sub_0802E6F8(void);

/* Wave 35 (W35-H): the nine arms of sub_0802DC2C's `switch
 * (gUnknown_03003334)`. Every one is nullary -- none reads an argument register
 * before writing it -- and every one is entered by a bare `bl` with the result
 * discarded, so all nine are void. */
void sub_0802DCB4(void);
void sub_0802DE1C(void);
void sub_0802DEFC(void);
void sub_0802E260(void);
void sub_0802E278(void);

/* Wave 35 (W35-H).
 *
 * sub_08020D50 is sub_08020354's three-argument twin: `adds r4,r2,#0` then
 * `lsls #0x10; asrs #0x10` on r0 and r1 -- SIGNED halfwords, where sub_08020354
 * narrows the same pair unsigned -- and r2 is again a gUnknown_08499594
 * element pointer.
 *
 * sub_0802D67C and sub_0803AA78 each open `lsls r0,r0,#0x18; lsrs r0,r0,#0x18`,
 * PROMOTE_MODE on a declared u8 parameter.
 *
 * sub_0802E724 takes s16 and returns bool8. Both parameters carry ONE
 * `lsls #0x10; asrs #0x10` rather than two pairs: that is combine folding
 * PROMOTE_MODE's zero-extension into the signed use, not an `int` parameter
 * with a cast -- an `int` would need no shift at all. The bool8 return is read
 * off sub_0802E60C, which narrows the result with `lsls r0,r0,#0x18` before
 * testing it.
 *
 * sub_0802E60C takes s16 too, and it is the clean case: it keeps BOTH views of
 * each parameter (`lsls #0x10` once, then `lsrs` into r7/r6 and `asrs` into
 * r0/r1), because it stores the unsigned view into gUnknown_03003100's u16
 * members and passes the signed view to sub_0802E724. */
void sub_08020D50(s16, s16, struct UnitRecord *);
void sub_08024500(void);
void sub_0802D67C(u8);
void sub_0803AA78(u8);
bool8 sub_0802E724(s16, s16);
void sub_0802E60C(s16, s16);

/* Wave 35 (W35-H). sub_08034F54 is `gUnknown_030030F0[1] = 0; bx lr` -- a leaf
 * with no prologue at all, so nullary and void beyond doubt. sub_08038AD8 reads
 * no argument register before writing it. sub_08025BB4 forwards r0 untouched to
 * sub_08035740 (its first instruction is that `bl`), and its one call site in
 * sub_0802E60C passes gUnknown_03003110. sub_0802E940 is promoted in
 * src/decomp/c_0802E920.c. */
void sub_08034F54(void);
void sub_08038AD8(void);
void sub_08025BB4(void *);
void sub_0802E940(void);

/* Wave 35 (W35-H). sub_08039264 is `bl sub_08038D7C; sub_0801F024(sub_08039188,
 * 2)` -- nullary, and `pop {r0}` so void. sub_0802D7B0 is a bare `bx lr`.
 *
 * sub_080146D4 is PROMOTED in src/decomp/c_08014668.c and was still
 * undeclared; the signature is copied verbatim from that definition.
 * tools/proto_check.py caught a first draft of it that guessed
 * `void (int, int, u16 *, int, int, int)` from sub_0802D35C's call site alone
 * -- the return type and the three trailing widths are invisible there. What
 * the call site DOES show, and the definition agrees: it takes SIX arguments,
 * because it pushes six registers and then
 * `sub sp,#0xc`, so `[sp,#0x24]` and `[sp,#0x28]` are arguments 5 and 6. The
 * first two are `int` and NOT `s16`, read off the callee rather than the call
 * site: it copies them with a bare `adds r5,r0,#0` / `adds r6,r1,#0` and pays
 * no PROMOTE_MODE narrowing, so the `lsls #0x10; asrs #0x10` sub_0802D35C emits
 * on argument 1 is an explicit `(s16)` cast in that caller's own source. */
void sub_08039264(void);
void sub_0802D7B0(void);
void sub_08024274(void);
void sub_0804256C(void);
void sub_0803A9C8(u8);
void sub_0802EC64(void);
/* Wave 35 (W35-H): the two IRQ handlers sub_0802EA5C installs through
 * sub_0801BB00 (slots 7 and 6). Neither reads an argument register. */
void sub_0802ED00(void);
void sub_0802ED40(void);
struct Unk03001470 *sub_080146D4(int, int, u16 *, u16, u16, u16);

/* Wave 35 (W35-H): promoted in src/decomp/c_08042084.c. RETYPED there from
 * `int` to `bool8` -- the body (`if (x != 0) return 1; return 0;`) is
 * byte-identical either way and so says nothing, but sub_0802CC40 narrows the
 * result with `lsls r0,r0,#0x18` before testing it, which an `int` return does
 * not emit. That file was the only caller in src/decomp and re-verifies. */
bool8 sub_08042084(u8 *);

/* Also already PROMOTED (src/decomp/c_08026F9C.c, src/decomp/c_080225CC.c) and
 * still undeclared; signatures copied verbatim from the definitions.
 *
 * CORRECTED in wave 18 (W18-C): sub_08026FD0's first parameter is `s16`, not
 * `u16`. The definition is byte-neutral either way -- `u16 a` with `(s16)a` in
 * the body and `s16 a` with a bare `a` are the same bytes, which is why this
 * stayed wrong -- but the CALL SITES are not. Three of the eleven load that
 * argument sign-extended: sub_08061B4C and sub_08062AE4 both do
 * `movs r1,#0; ldrsh r0,[r0,r1]` on gUnknown_03003F2C (declared `u16`!), and
 * sub_080257C0 does the same on gUnknown_084995FE. A `u16` parameter folds the
 * truncation into the load and emits a 2-byte `ldrh`; only a narrow SIGNED
 * parameter rewrites it to the 4-byte register-offset `ldrsh`. No caller was
 * promoted when this was corrected, so the edit cost one trymatch on
 * c_08026F9C.c. */
bool8 sub_08026FD0(s16, u8);
/* Wave 34 (W34-I). sub_08026FD0's sibling -- gUnknown_08499598's unk2a comment
 * records the pair as comparing that field between two entries, and both take a
 * unit id and a second id truncated to a byte. sub_080422A8 is the only caller
 * in C so far and it hands the arguments the OTHER way round from sub_0804236C's
 * sub_08026FD0 call: the cell's unit id first, gUnknown_03003F38 second. Return
 * is u8 -- that caller re-narrows with `lsls #0x18` before testing.
 * CAVEAT: the first parameter's WIDTH is inherited from the sibling, not proved.
 * The only call site passes a value that came from a `ldrb`, so s16, u8 and int
 * are all byte-identical there; nothing here discriminates them. */
/* Wave 34 integration: the second parameter is s16, not u8. The declaration
 * added this wave read it as u8 from a call site, but PROMOTE_MODE zero-extends
 * every sub-word parameter at entry, so u8 and s16 are identical there and the
 * caller cannot discriminate them. The promoted definition's body can. */
bool8 sub_08026F9C(s16, s16);
void sub_080225CC(u16, u16);
/* Wave 41 (W41-C). Copied verbatim from the promoted definition in
 * src/decomp/c_080227F4.c so sub_08022990 can call it; same (u16, u16) map
 * cell as its sibling sub_080225CC just above. */
void sub_080227F4(u16, u16);

/* sub_08022618 tests the result with a bare `lsls r0, #0x18; cmp r0, #0`, so
 * bool8; its argument is `(u16)id` where id is an s16 local, and the pair
 * `lsls #0x10; lsrs #0x10` in front of the call is that conversion, not a
 * re-narrowing -- a plain `int` parameter would leave the sign-extension
 * (`lsls; asrs`) that the same value's OTHER use, as an array index, does
 * carry. So the parameter is 16 bits wide and unsigned. */
bool8 sub_0802571C(u16);

/* Three more already-promoted, still-undeclared callees of sub_08042998,
 * copied verbatim from src/decomp/c_08025B28.c and src/decomp/c_080424BC.c;
 * sub_08042C9C is the one that is genuinely new. Its result is multiplied by
 * an `int` with no narrowing in front, so `int`; its two arguments are
 * gUnknown_030033EC (u16, passed with a bare `adds r0, r4, #0`) and a `ldrb`,
 * neither narrowed at the call. */
void sub_08025B58(u16, u32);
void sub_08025B80(struct UnitRecord *, u8);
void sub_080424E4(void);
/* Both parameters CORRECTED to `int` (wave 26, W26-C). The paragraph above read
 * them off the CALL SITE, where neither is narrowed -- but that is byte-neutral,
 * because a u16 global reaching an `int` parameter is already zero-extended by
 * its own `ldrh` and needs no instruction either way. The callee's own prologue
 * decides it, and sub_08042C9C's is a bare `adds r4, r0, #0; adds r5, r1, #0`
 * with both values then living across two `bl`s. Declared `(u16, u8)` the body
 * gains four instructions -- PROMOTE_MODE's copy-then-narrow on each -- and does
 * not match; declared `(int, int)` it is byte-exact. The bare-prologue rule in
 * docs/agbcc-codegen.md, applied to the definition rather than the caller. */
int sub_08042C9C(int, int);

/* Promoted but never declared -- the wave-14 "promoted but no prototype" trap
 * again. src/decomp/c_080616F0.c defines it `void sub_080616F0(void)` and that
 * definition wins; this only publishes it (wave 23, A -- needed by
 * sub_0807898C). */
void sub_080616F0(void);

/* ...and sub_08042998 itself, which those three serve and which was never
 * declared either (wave 23, A -- needed by sub_0802D0F4). Nullary: its
 * prologue writes r0-r3 before reading any of them (`ldr r4, =gUnknown_08499594`
 * onwards). Void: it ends `pop {r0}; bx r0`, so r0 carries the return address
 * and cannot carry a value, and sub_0802D0F4's `bl` discards it. */
void sub_08042998(void);

/* ---- family F059's two undeclared callees (wave 15, C) ----
 *
 * sub_0803CCB8's parameter is `int`, not `u8`: its body opens
 * `lsls r0,#0x18; lsrs r0,#0x18` with no `adds rN, r0, #0` copy in front, which
 * is a cast at a use rather than a narrow parameter, and F059's three members
 * pass it the bare constants 0/1/2. Its SECOND argument is an address that it
 * forwards untouched to sub_0803CC84 as the destination of a NUL-terminated
 * byte copy out of gUnknown_020280C0[i].unk02, so `u8 *`. It RETURNS `bool8`:
 * the body is `movs r0,#1` / `movs r0,#0`, and every F059 call site carries
 * `lsls r0,#0x18; lsrs r0,#0x18` before `cmp r0,#1` -- value kept, eight bits
 * wide, which agbcc only emits for a narrow-returning callee.
 *
 * sub_0803CDBC's third argument is likewise reached by a shift pair split
 * around an `+ 0x500000` in its own body, so `int` on all three. Its return is
 * NOT settled here: it ends `pop {r1}; bx r1` and returns literal 0/1, but all
 * three F059 callers discard the result, so `int` and `bool8` are
 * indistinguishable from this side. Declared `int` as the weakest fit -- retype
 * it from a caller that uses the value. */
bool8 sub_0803CCB8(int, u8 *);
int sub_0803CDBC(int, int, u8); /* W35-E: 3rd was `int`; the ROM opens
                                 * `lsls #0x18 ... adds #0x05000000 ... lsrs
                                 * #0x18`, which only a declared u8 parameter
                                 * produces -- `(u8)a3 + 5` on an int folds to
                                 * three instructions. No caller in src/. */

/* ---- wave 16 (A): callees of families F067-F072, F075, F077 ----
 *
 * Every `void f(void)` below is read the same way and the readout is cheap:
 * the function writes its first argument register before reading it (usually
 * `ldr rN,=gSym` as instruction 2), so it takes nothing, and it ends
 * `pop {r0}; bx r0`, which docs/agbcc-codegen.md fixes as void. None of them
 * is called with a live r0 anywhere in the ROM.
 *
 * The six that already have a definition in src/decomp/ are declared here for
 * the first time -- they were only ever named in comments. Each declaration
 * was copied from the promoted definition, not re-derived:
 * sub_0801E0C8 (c_0801E0C8.c), sub_0801EFD8 (c_0801EFD8.c), sub_08015550 and
 * sub_0801555C (c_08015544.c), sub_0802A538 (c_0802A508.c), sub_0802A7B0
 * (c_0802A7B0.c), sub_0802C57C and sub_0802C594 (c_0802C57C.c). */
void sub_0801E0C8(int, int);
void sub_0801EFD8(void);
void sub_08015550(void);
void sub_0801555C(void);
void sub_0802A538(void);
void sub_0802A7B0(void);
void sub_0802C57C(void);
void sub_0802C594(void);

/* F069 members, matched in wave 16 and called from F070/F071: sub_0801F050
 * takes sub_0801E0F0 as its else-arm and sub_0801F0C8 takes sub_0801EFA8 as
 * its whole body. Both are `push {lr} ... pop {r0}; bx r0`. */
void sub_0801E0F0(void);
void sub_0801EFA8(void);

void sub_0801BBC4(void);
void sub_0801BC08(void);
void sub_0801BCA8(void);
void sub_0801EFF4(void);
void sub_080219AC(void);
void sub_080245D4(void);
void sub_08026D68(void);
void sub_080424FC(void);
void sub_08061F34(void);
void sub_08062038(void);
void sub_0807F238(void);

/* Both parameters are eight bits wide *somewhere*: the prologue is
 * `lsls r0,#0x18; lsrs r0,#0x18` / `lsls r1,#0x18; lsrs r6,r1,#0x18` with no
 * `adds rN, r0, #0` copy in front of either, which per the F059 note above is
 * a cast at a use just as readily as a narrow parameter -- and the two are
 * byte-identical here because F068's three call sites all pass literals
 * (`movs r0,#K`). `int` is the weaker fit and is what is declared; a caller
 * passing a variable would settle it, and there is none. `pop {r0}` = void. */
/* CORRECTED in wave 24 (W24-A) from `(int, int)`. Both parameters are u8 and
 * the callee's own prologue says so outright: `lsls r0,#0x18; lsrs r0,#0x18`
 * then `lsls r1,#0x18; lsrs r6,#0x18`, PROMOTE_MODE's zero-extension, which an
 * `int` parameter does not pay. The old declaration survived because the only
 * promoted caller (src/decomp/c_080199A4.c) passes literal constants, which are
 * byte-identical either way -- the exact "a wrong prototype that every existing
 * caller happens to agree with" case. sub_0802CE70 is the differently-shaped
 * caller that exposes it: it passes the u16 gUnknown_030033EC and the ROM emits
 * `ldrb`, the byte load only a u8 parameter produces out of a halfword global.
 * c_080199A4.c re-verified after the change. */
void sub_08019940(u8, u8);

/* Takes no argument and RETURNS one (`pop {r1}; bx r1`). Arity is not guessed
 * from F075's call sites -- all three save r0 into r4 before the call, so r0 is
 * live there for the *later* Proc_Start* argument and a pass-through would look
 * identical. The readout is on the callee side: sub_080413E8's first act is
 * `bl sub_0804138C`, and sub_0804138C is four instructions that only store 0
 * to gUnknown_030040A8, reading no argument register either. So nothing in the
 * chain consumes r0. Every caller in the ROM discards the result, so `int` is
 * the weakest fit for the return rather than a measurement. */
int sub_080413E8(void);
/* Wave 50, W50-J. Both newly declared from sub_080413E8's call sites.
 * sub_0804138C: `void (void)` -- the comment above already records that it is
 * four instructions that only store 0, and sub_080413E8 calls it with no
 * argument set-up and ignores r0.
 * sub_080413B4: the parameter list is NOT inferred, it is copied from the
 * PROMOTED definition in src/decomp/c_080413B4.c, which is byte-exact and
 * wins over any weaker model. sub_080413E8 passes (x, y, unitId, 0) in one
 * arm and (x, y, 0, kind) in the other, which agrees. */
void sub_0804138C(void);
void sub_080413B4(int, int, int, int);

/* The structural twin of sub_0801537C above, and NOT interchangeable with it:
 * sub_0801537C scans gUnknown_03001470[] for the slot whose `.unk00` equals the
 * argument, while this one compares the argument against the slot's own ADDRESS
 * (`lsls;adds;lsls #5; adds r0,r0,r2; cmp r0,r1`) -- so its parameter is a
 * pointer to the slot, and `struct Unk03001470 *` is what the tree already
 * calls that (sub_080152EC returns it). Both then call the same teardown and
 * return the index as `lsls #0x18; asrs #0x18`, or -1. Declared `int` for the
 * same reason sub_0801537C is: no caller in the ROM narrows the result, so the
 * width is undecidable and `int` is the only spelling that is valid C for a
 * function whose value is a plain forwarded index. */
int sub_080153B8(struct Unk03001470 *);

/* ---- wave 16 (B) ----
 * DEFINED (matched) in src/decomp/c_0803BB44.c, which is where the argument
 * for the u8 return lives; these declarations only exist so the F081 wrappers
 * in the sibling translation unit see it. The two derivations are independent
 * and agree: that file read the width off sub_0803BB14's `lsls #0x18;
 * lsrs #0x18`, and wave 16 read it off sub_0803BAFC's and sub_0803BB2C's. */
u8 sub_0803BB44(void);
u8 sub_0803BB5C(void);
u8 sub_0803BB74(void);

/* sub_080324C4 is the 452-byte screen setup behind sub_08032688; the prologue
 * is `mov r8,r0; adds r6,r1,#0; lsls r2,#0x18; lsrs r7,r2,#0x18`, so arguments
 * one and two arrive unnarrowed and three is `u8`. Argument two is compared
 * against -1 and is therefore signed, and the `pop {r0}; bx r0` epilogue
 * clobbers r0, which makes it void.
 *
 * sub_08029AF8 is the same shape one level down: `adds r5,r0,#0` and then
 * `ldrb r1,[r5]` types argument one as a pointer (`void *` is the weakest
 * spelling that its two wrappers can pass), `lsls r1,#0x10; lsrs r7,r1,#0x10`
 * types argument two `u16` and `lsls r2,#0x18; lsrs r2,r2,#0x18` types three
 * `u8`. It DOES return a value -- the epilogue is `mov r0,r8; ...; pop {r1};
 * bx r1` and r8 is an accumulator started at 0 -- but both callers discard it,
 * so `int` is a weakest fit rather than a measurement.
 *
 * sub_08053670 narrows its only argument `lsls #0x10; lsrs r5,r0,#0x10` and
 * ends `pop {r0}; bx r0`, so it is `void(u16)` -- the same unit index
 * sub_0804C400 takes. */
void sub_080324C4(int, int, u8);
/* Wave 38 (W38-J): same object as sub_08029978 / sub_08029A48 above, and
 * retyped on the same evidence -- it walks unk00 (the `* 0x5c` subscript into
 * gUnknown_085D5ABC) and the unk04_0:7 field, clamping the latter at 100. */
int sub_08029AF8(struct UnitRecord *, u16, u8);
void sub_08053670(u16);

/* The three callees of sub_08043418.
 *
 * sub_08042424 is the c_08001158.c map-cell idiom again, on the +0x1432
 * terrain plane rather than the +0x12 one: `lsls r1,#0x10; asrs r1,#0xf` is
 * an s16 column scaled by 2 for rowOffset[] and `lsls r0,#0x10; asrs r0,#0x10`
 * an s16 row, so both parameters are s16 and sub_08043418 casts to them at the
 * call. The return is u8 -- sub_08043418 truth-tests the result with a BARE
 * `lsls #0x18`, which is what agbcc emits for a u8-returning callee whose
 * value is only tested.
 *
 * sub_08043574 is nine instructions with no frame: it adds 1, 2 or 3 to its
 * third argument according to `x > 0xcf` and `y > 0x7f` and returns it. All
 * three comparisons are signed (`ble`), so all three parameters are `int`, and
 * `adds r0,r2,#0; bx lr` returns the third.
 *
 * sub_0801C7DC takes SEVEN arguments -- it pushes five registers and one word,
 * then reads [sp,#0x18], [sp,#0x1c] and [sp,#0x20]. Argument one is the table
 * (`ldrh [r0]`, `ldrh [r0,#2]`), two is a halfword index into it, three is a
 * count it feeds to DivRem, and four/five/six/seven are forwarded to
 * PutSpriteExt as (r1, r2, stack, r0) respectively -- i.e. x, y, an OAM word
 * and a layer. It returns a value that all three callers discard. */
u8 sub_08042424(s16, s16);
int sub_08043574(int, int, int);
int sub_0801C7DC(const u16 *, int, int, int, int, int, int);

/* ---- wave 17 (B) ----
 * The position half of the ~40 sprite setters between 0x0804B180 and
 * 0x08053614: it takes a side (gUnknown_0300453C), a proc id
 * (gUnknown_03001FBC) and an (x, y) in world coordinates, subtracts the scroll
 * origin pair out of gUnknown_084C3F70/gUnknown_084C3F78 and forwards to
 * sub_080155C0. All four parameters are narrowed `lsls #0x10; lsrs #0x10` at
 * entry, which is only PROMOTE_MODE and does not settle signedness; the
 * signedness comes from the USES -- arguments two, three and four are each
 * re-extended `lsls #0x10; asrs #0x10` before they are used, so those three
 * are signed, while argument one is only ever an index (`lsls #2`, `lsls #4`)
 * and is unsigned. Callers agree: sub_08051DE0 and its two siblings pass
 * gUnknown_0300453C bare but sign-extend the two u16 position sums, which is
 * the `u16 -> s16` conversion and is four instructions of real code.
 * `pop {r0}; bx r0` makes it void. */
void sub_08050528(u16, s16, s16, s16);
/* Matched and promoted in src/decomp/c_080513FC.c; declared here in wave 33
 * (W33-D) so sub_0805131C, which tail-calls it with the same (side, slot,
 * proc) triple, can be written. The signature is that definition's. */
void sub_080513FC(u16, u16, s16);
/* Wave 36, W36-C. Copied verbatim out of the promoted definition in
 * src/decomp/c_08051920.c, which had no declaration anywhere and blocked
 * sub_080517BC's call site with an implicit-declaration error. */
void sub_08051920(u16, u16, s16);
/* Wave 36, W36-C. Derived from sub_08053FBC's call site, which is the only one
 * in the tree: `adds r0,r6,#0` materialises the u16 side and `adds r1,r4,#0` a
 * halfword freshly loaded with `ldrh`, and the result is not read. */
void sub_080540F0(u16, u16);

/* wave 18 (W18-C): sub_08052CA4's extra callee. Two u16 parameters -- the
 * prologue is `lsls #0x10; lsrs #0x10` on both, PROMOTE_MODE, and it stores
 * them straight back into gUnknown_0300453C / gUnknown_0300451C, which are
 * both u16. The THIRD parameter is DEAD: r2 is never read anywhere in the
 * body, and all three call sites (sub_08052650, sub_08052AF4, sub_08052CA4)
 * emit `movs r2, #0` immediately before the `bl`, which is deliberate argument
 * setup and not a leftover -- so the arity is three even though the body
 * cannot see it. Void: `pop {r4, r5, r6}; pop {r0}`. */
void sub_08052E04(u16, u16, int);

/* The two emitters family F092 drives, and the routine it calls when the
 * counter runs out. All three take their argument (if any) in r0 and all three
 * end `pop {r0}; bx r0`, so all three are void.
 *
 * sub_080645AC and sub_08064E5C both open `adds rN, r0, #0` and then read
 * +0x28 / +0x2a / +0x1c through it -- the struct Unk08580934_Obj offsets --
 * so the parameter is that type rather than a bare `void *`. sub_080645AC also
 * calls through +0x4c (`ldr r1,[r4,#0x4c]; bl _call_via_r1`), which is past
 * the modelled extent; the field is deliberately NOT added, because a
 * one-instruction indirect call is not enough to type it.
 *
 * sub_08030178 reads no argument register before writing it -- it opens
 * `ldr r5,=gUnknown_08090CAC; ldr r6,[r5]` -- so it is nullary. */
void sub_080645AC(struct Unk08580934_Obj *);
void sub_08064E5C(struct Unk08580934_Obj *);
/* Wave 29 (C). The draw call sub_08064CA8 and sub_08064CFC both tail into:
 * one sub_0801F34C with an id taken from gUnknown_08580934->unk11[obj->unk1c].
 * `pop {r4}; pop {r0}` -> void. */
void sub_08064BF4(struct Unk08580934_Obj *);
void sub_08030178(void);
/* Wave 49 (W49-B). sub_080301E8's first prototype; it was promoted alongside
 * sub_08030178 in c_08030178.c but never declared, so sub_080303C8 -- its only
 * caller outside that file -- could not name it. Agrees with the promoted
 * definition: nullary, and `pop {r4,r5}; pop {r0}` -> void. */
void sub_080301E8(void);

/* The "this slot has arrived" handler sub_08050958 calls when a moving unit
 * crosses its bound. All three parameters are narrowed `lsls #0x10; lsrs #0x10`
 * at entry, and the signedness comes from the uses: one and two are only ever
 * array indices (`muls` by 0xb4, `lsls #3`) into gUnknown_02029A10, so u16,
 * while three is re-extended `lsls #0x10; asrs #0x10` and tested against -1
 * before going to sub_080153F0 -- the proc-id pattern, hence s16. The caller
 * agrees: it passes two u16 locals bare and gUnknown_03001FBC via `ldrsh` with
 * no conversion on any of the three. `pop {r0}; bx r0` makes it void. */
void sub_08050AEC(u16, u16, s16);

/* The rest of sub_08051F4C's callees. All six end `pop {r0}; bx r0`.
 *
 * sub_080504A8 narrows both arguments `lsls #0x10; lsrs #0x10` with no copy in
 * front, which is a declared-narrow pair; sub_08051D74 is the same shape.
 *
 * sub_08016824 and sub_08016944 are `adds rN, r0, #0` and THEN the narrowing,
 * which docs/agbcc-codegen.md reads as an `int` parameter with a cast at a
 * use rather than a declared-narrow one. Byte-neutral at sub_08051F4C's call
 * sites either way (the argument arrives via `ldrsh`, so neither `int` nor
 * `s16` needs a conversion), so this follows the rule rather than measuring.
 *
 * sub_080157A4 and sub_080157F4 are a pair -- the same eight instructions
 * writing gUnknown_0200E438[...].unk3c and .unk3e. Argument one narrows
 * `lsls #0x10; asrs #0x10` with no copy, so s16. Argument two is a bare `strh`
 * setter and therefore cannot be distinguished from int/u32/u16 BY THE
 * CALLEE -- the CALLER settles it: sub_08051BEC and sub_08051F4C sign-extend a
 * u16 table value before passing it, which is the `u16 -> s16` conversion and
 * is two instructions of real code that no other declaration produces. Both
 * are already PROMOTED (src/decomp/c_080157A4.c, c_080157F4.c) and both were
 * declared `u16 b` there; those two definitions were retyped to agree and
 * re-verified, which is the "the prototype is the suspect" case the brief
 * describes. */
void sub_080504A8(u16, u16);
void sub_08051D74(u16, u16);
/* First parameter s16 (`lsls #0x10; asrs #0x10` at entry, the proc-id shape).
 *
 * THE SECOND IS A CONTRADICTION AND `void *` IS THE CALLER-SIDE ANSWER
 * (wave 20, W20-C). sub_080156E8 narrows r1 `lsls #0x10; lsrs #0x10` at entry
 * and its tail-callee sub_080156FC (matched) uses the same value as
 * `(u16)x * 4` added to the pointer at gUnknown_0200E438[..].unk48 -- i.e. a
 * small TABLE INDEX, not an address. But every caller passes the word straight
 * out of gUnknown_02029BA8 with `ldr` and NO narrowing in front of the `bl`,
 * which a declared-`u16` parameter would have forced. Per the brief's rule the
 * prototype is settled from the CALLERS, so it is wide here; the entry
 * narrowing belongs to sub_080156E8's own declaration in its own unit, which
 * is the "two prototypes disagreed across files" case docs/agbcc-codegen.md
 * describes. Consequence worth recording: the note on struct Unk02029BA8 in
 * include/unknown-globals.h calls those members addresses "because they go to
 * sub_080156E8" -- that inference is WRONG, they are indices; the `void *`
 * declaration survives only because it is byte-identical at every call site. */
/* Types COPIED FROM THE PROMOTED DEFINITION src/decomp/c_080153F0.c, not
 * re-derived -- a promoted definition wins over a fresh declaration. It was
 * never declared here because nothing had called it across a file boundary
 * until sub_08051D74 (wave 20, W20-C). The `bool8` return is what makes a
 * caller's `if (sub_080153F0(x))` a bare `lsls #0x18; cmp #0` with no
 * `lsrs`. */
bool8 sub_080153F0(s16);
void sub_080156E8(s16, void *);
/* Wave 35, W35-I: `s16`, corrected from `int`. Its prologue is the
 * copy-then-ZERO-extend `adds r5,r0,#0; lsls #0x10; lsrs #0x10`, which is the
 * declared-narrow parameter that has to live across a `bl` (the c_080154C4
 * case), NOT an int with a cast -- an int parameter shifts straight out of r0
 * with no copy, probed both ways. Its neighbour sub_080168BC really is `int`:
 * that one opens with the SIGN-extending `asrs` and no zero-extend.
 * All six promoted callers already pass an s8 or an s16, so the narrowing is
 * free at every one; re-verified with trymatch. */
void sub_08016824(s16);
void sub_08016944(int);

/* ---- wave 35, W35-I: the gUnknown_03001470 float path-mover at 0x080161B4 -
 * Four script-opcode handlers plus their shared per-frame step. The slot's
 * 0x3c..0x5f tail is used as SIX FLOATS plus an int frame counter here (see the
 * note on struct Unk03001470 in include/unknown-globals.h): unk3c/unk40 are the
 * current x/y, unk4c/unk50 the velocity, unk54/unk58 the acceleration and unk5c
 * the remaining frame count. sub_080162A4 is the void step -- it integrates,
 * calls sub_080155C0 with the truncated position, and retires the command when
 * the counter runs out; the three handlers are `bool8` returning FALSE, the same
 * "do not yield" convention as src/decomp/c_08016104.c's four. `u8` parameter
 * from the `lsls #0x18; lsrs #0x18` at entry of each. */
void sub_080162A4(u8);
/* `u8` off its own prologue in asm/code.s (`lsls #0x18; lsrs #0x18`), not off
 * the call site -- sub_08016D30 hands it a u8 parameter unchanged, which is
 * arity- and width-blind. The return is UNPROVED: sub_08016D30 is its only
 * caller and discards r0, so `void` is the weakest type that fits. W35-I. */
void sub_08016F38(u8);
/* The free half of sub_0801DAB0's affine-matrix slot allocator (sub_0801DAB0
 * scans gUnknown_03001430[0..0x1f] for a zero entry, this one releases). `s16`
 * from its own prologue in asm/code-0801D390.s -- `lsls #0x10; asrs #0x10` and
 * then a signed compare against -1 -- and sub_080168BC hands it a bare `ldrsh`
 * of a declared-s16 member, which agrees. Void: no caller reads r0. W35-I. */
void sub_0801DAE8(s16);

/* ---- wave 29, W29-B: address-locality block 0x0803D --------------------- */
/* A predicate over the four teams: it loops sub_080266DC(i) for i in 1..4 and
 * every caller tests the result `lsls #0x18; cmp #0`, which is the `bool8`
 * shape. Takes nothing -- r0 is written before it is read. */
bool8 sub_0803861C(void);
/* Already DEFINED in src/decomp/c_0803B79C.c; published here so sub_0803D978
 * can call it. */
void sub_0803B7B4(void);
/* The 0x02028360 sweep sub_0803DE68 finishes with, and the lookup sub_0803DF54
 * opens with. sub_0803DE94 takes two arguments (r0 and r1 are both copied
 * before anything else) and returns a pointer into the gUnknown_02028360 array;
 * `int` for both because sub_0803DF54 forwards its own two straight through
 * with no conversion in front of the `bl`. */
void sub_0803DE14(void);
struct Unk02028360 *sub_0803DE94(int, int);
/* Wave 43 (W43-G). sub_0803DE94 with a third test: the same rectangle scan
 * over gUnknown_02028360, additionally requiring the record's unk02_6 kind
 * field to equal the third argument. Returns the record or NULL, read off the
 * body (`adds r0,r3,#0` on the hit, `movs r0,#0` on the fall-out) and NOT off
 * any caller -- it has none in this tree, so the pointer return is the twin's
 * shape rather than a measured fact, and the three `int` parameters are the
 * weakest types that fit. */
struct Unk02028360 *sub_0803DEEC(int, int, int);
/* Wave 31 (W31-C). sub_0803DF98 fills the same {u16, u16} out-parameter
 * sub_0803DFE0 does -- sub_0803E310 binds a local one, calls it as
 * `sub_0803DF98(7, &pos)` and then subtracts pos.unk00 / pos.unk02 from its own
 * first two arguments.
 *
 * sub_0803E01C is the allocator sub_0803E088 wraps: six arguments forwarded
 * from r0..r3 and two stack slots WITHOUT being touched (so nothing about their
 * types is visible from the wrapper), and it returns the object whose +3
 * bitfields and +5/+6/+7 bytes sub_0803E088 then fills in. `void *` because no
 * caller yet names more of the object than those four offsets.
 *
 * sub_0803E088 itself: ten arguments, ALL `int`. r0 is live at the epilogue --
 * `pop {r1}; bx r1` -- so the allocated object is returned.
 *
 * The seventh and tenth reach their stack slots with `add rN, sp, #imm; ldrb`,
 * which looks like a `u8` parameter and is not one: a u8 stack parameter is
 * zero-extended ONCE in the prologue by PROMOTE_MODE, whereas these are loaded
 * afresh at each use -- twice for the seventh. The `ldrb` is agbcc folding the
 * `(u8)` of the destination FIELD into the load, the same fold sub_080604BC
 * gets on the s16 gUnknown_03001FBC. Spelling them `u8` costs two prologue
 * shift pairs and moves the loads, and it also forces a narrowing into
 * sub_0803E310's argument setup. */
void sub_0803DF98(int, struct Unk02028360Pos *);
/* Wave 38, W38-C. Appends one record to *gUnknown_03003338; defined in
 * src/decomp/c_0803E560.c. The four parameters are `u16` on CALLER evidence
 * only -- the callee body compiles byte-identically with `int`, see the note in
 * that file. sub_0803E764 and sub_0803E6C4 both narrow exactly the arguments
 * that are not already provably 16-bit and leave the `ldrh`/`ldrb` results
 * alone, which is implicit conversion to a u16 parameter rather than four
 * explicit casts. Note this is NOT the "no prologue narrowing" rule: that rule
 * holds for STACK parameters (see sub_0803E088 above), and these four are all
 * register parameters, where PROMOTE_FUNCTION_ARGS leaves the callee clean. */
void sub_0803E560(u16, u16, u16, u16);
void *sub_0803E01C(int, int, int, int, int, int);
void *sub_0803E088(int, int, int, int, int, int, int, int, int, int);
void *sub_0803E7C0(int, int);
void *sub_0803E7E4(int, int);
/* Wave 49, W49-C. The 0x0803Exxx / 0x0803Fxxx map-decoration group: every one
 * of these was DEFINED by a promoted src/decomp file (or, for sub_0803E6C4,
 * called from one) without ever being declared here, because until this wave
 * nothing outside its own translation unit called it. sub_0803E3D8,
 * sub_0803EBA4 and sub_0803ED60 are those callers. Each signature below is
 * copied from the promoted definition, not inferred -- disagreeing with one
 * would not compile. sub_0803E6C4 is the exception and is read off
 * sub_0803ED60's call site: three whole words, the same (x, y, s8) triple
 * sub_0803E594 takes one instruction earlier, which is consistent with the two
 * being the near-twins docs/agbcc-codegen.md records them as. */
void sub_0803E108(int, int, int, int);
void sub_0803E158(int, int, int, int);
void sub_0803E1B0(int, int, int, int, int, int);
void sub_0803E208(int, int, int, int, int, int, int);
void sub_0803E260(int, int, int, int, int);
void sub_0803E2B8(int, int, int, int, int, int);
void sub_0803E310(int, int, int, int, int, int);
void sub_0803E554(void);
void sub_0803E594(int, int, int);
void sub_0803E6C4(int, int, int);
void sub_0803E764(struct Unk02028360Pos *, int);
void sub_0803E808(int, int, int, int, int);
void sub_0803EF44(int, int, ProcPtr);
void sub_0803F0A4(int, int, int, int, int, int, ProcPtr);
void sub_0803F2B8(int, int, int, ProcPtr);
void sub_0803F510(int, int, ProcPtr);
/* Wave 49, W49-C, from sub_0803E808's call site: three u8 arguments
 * (`ldrb` of gUnknown_030033EC, then `(u8)x` and `(u8)y` cell coordinates,
 * each an explicit `lsls #0x18; lsrs #0x18` on an int loop counter) and a
 * result consumed by a bare `lsls #0x18; cmp #0` truth test, i.e. one byte
 * wide. */
bool8 sub_08020DBC(u8, u8, u8);
void sub_0803D3F0(void);
/* Decodes one gUnknown_02028360 entry's position into a {u16, u16} pair,
 * returning whether the entry's 4-bit kind at unk02 bits 6..9 is one it
 * understands. `bool8`: sub_0803DF54 tests it `lsls #0x18; cmp #0`. */
bool8 sub_0803DFE0(struct Unk02028360 *, struct Unk02028360Pos *);
struct Unk02028360 *sub_0803DF54(int, int);
void sub_0803D724(u8);

/* ---- wave 29, W29-B: address-locality block 0x08016 --------------------- */
/* sub_08016A14's definition is src/decomp/c_08016A14.c; it was never declared
 * here because nothing had called it across a file boundary until sub_08016C70
 * and sub_08016E14. */
void sub_08016A14(void);
/* Returns the ADDRESS of one of gUnknown_0200C420's three byte flags -- see
 * the note on unk09 in include/unknown-globals.h. The parameter is SIGNED
 * (`lsls #0x18; asrs #0x18` in place at entry); the switch has no default, so
 * on any other value the incoming r0 falls out of the function unchanged,
 * which is what the shared `bx lr` with r0 untouched says. */
u8 *sub_08016C9C(s8);
/* Its two callers. sub_08016CD8 re-narrows the byte it loads to `s8`
 * (`ldrb; lsls #0x18; asrs #0x18` -- a `ldrsb` is what a `s8 *` return would
 * have given instead), sub_08016CEC stores a `u8` through the pointer. */
s8 sub_08016CD8(s8);
void sub_08016CEC(s8, u8);
/* `u8` first parameter (`lsls #0x18; lsrs #0x18` in place at entry) and the
 * result is discarded at its one caller sub_08016C70. It returns a value
 * (`movs r0,#1` on the a==0 path, sub_0801A7D8's result otherwise). */
int sub_0801ABF8(u8);
/* Wave 41 (W41-A): sub_0801ABF8 passes gUnknown_0200CC2C as this function's
 * second argument, which agrees with the `void *` already declared below --
 * but note that sub_0801A7D8 OVERWRITES r1 before reading it
 * (`ldr r1,=gUnknown_0200CC34; ldr r1,[r1]`), so the "buffer it hands to
 * sub_0801B598" in the note below is gUnknown_0200CC34 and not this
 * parameter. Nothing in the callee types argument 2 at all; it is the call
 * sites that have to agree, and they do. */
/* Argument 1 is `u8` (narrowed in place at entry), argument 2 a buffer it hands
 * to sub_0801B598, argument 3 a SIGNED byte count (`subs r7,r7,r2` with a `ble`
 * compare against 0xFAD). Returns int. */
int sub_0801A7D8(u8, void *, int);
/* sub_08016B2C's mirror -- the same two block copies in the other direction,
 * returning the same 0x5CC. Its one caller sub_08016E3C discards the result. */
int sub_08016BC0(void *);
void sub_08016C70(u8);
void sub_08016E14(void);
/* Wave 38, W38-H: both promoted already (src/decomp/c_08016E8C.c and
 * src/decomp/c_08017870.c) but never declared, because nothing had called
 * either across a file boundary until sub_08016ED8 and sub_08016A54.
 * sub_08017870's parameter pair is the promoted definition's, not a guess. */
void sub_08016E8C(void);
void sub_08017870(int, u8);

/* ---- wave 29, W29-B: address-locality blocks 0x0803A and 0x08084 -------- */
/* The two gUnknown_03004100 consumers sub_0803AA78 chains. Argument 1 of both
 * is the `u8` gUnknown_0849D89C->unk00 (`ldrb` at the call); sub_0803A190 does
 * not narrow it at entry so it is `int` there, sub_0803A2BC narrows it
 * `lsls #0x18` in place so it is `u8`. Argument 2 is the same
 * struct UnitRecord sub_08025BE0 initialises. */
void sub_0803A190(int, struct UnitRecord *);
void sub_0803A2BC(u8, struct UnitRecord *);
/* A predicate over the gUnknown_0849EDB0 list; takes nothing (r0 is dead at
 * entry) and its one caller sub_08084920 tests the result `lsls #0x18;
 * cmp #0`. */
bool8 sub_0803C814(void);
/* WAVE 29: declared `ProcPtr` from the caller side, which conflicted with the
 * already-promoted definition in src/decomp/c_080852A8.c. The definition wins:
 * `ProcPtr` and `struct Unk080852A8 *` are both pointers and byte-neutral at
 * every call site, while the definition's body actually dereferences +0x66 as
 * an s16 -- and sub_08084BD4's own independently-derived struct agrees that
 * +0x66 is s16, so two derivations corroborate the field. Incomplete on
 * purpose; the definition completes it privately. */
struct Unk080852A8;
void sub_080852A8(struct Unk080852A8 *);
void sub_080157A4(s16, s16);
void sub_080157F4(s16, s16);
/* sub_080154C4 and sub_08015504 are ONE SOURCE AT TWO BIT POSITIONS: each
 * fetches the slot's stashed OBJ attributes with sub_0801566C into a stack
 * local, sets a single bit of byte 1, and hands the eight bytes back to
 * sub_08015608. They differ in exactly two instructions -- `lsls #4` with mask
 * ~0x10 against `lsls #5` with mask ~0x20 -- which under `struct OamData` are
 * `mosaic` (bit 12) and `bpp` (bit 13). NOT `vFlip`, which this comment used to
 * claim: vFlip is bit 29 and lives in byte 3, exactly where the struct's own
 * note in unknown-globals.h puts the measured hFlip/vFlip pair. The `mov #N;
 * neg` form of the mask is the bitfield-store tell -- a hand-written
 * `p[1] = (p[1] & ~0x10) | ((b & 1) << 4)` through a `u8 *` folds the mask to
 * `mov r0,#0xef` and comes out two bytes short.
 *
 * BOTH PARAMETERS ARE DECLARED-NARROW, AND THE `int, int` THIS LINE USED TO
 * CARRY WAS WRONG (wave 25, W25-C). The reasoning behind it -- copy-then-narrow
 * (`adds r5,r0,#0; lsls #0x10; asrs #0x10`) means an `int` parameter with a cast
 * at the use -- does not hold when the parameter's pseudo LIVES ACROSS A CALL.
 * agbcc then emits assign_parms' incoming-argument copy and the PROMOTE_MODE
 * extension as two separate insns instead of fusing them, so a declared
 * `s16`/`u8` parameter produces the copy-then-narrow shape by itself. Measured
 * with compile_probe on this exact pair: `(s16, u8)` with `o.mosaic = b;` is
 * byte-exact for both functions; `(int, int)` with a `(u8)` cast sinks the
 * narrowing PAST the call; `(int, u8)` keeps the narrowing but drops the copy
 * and is two bytes short. The doc rule is sound only for a parameter that dies
 * inside its own statement. */
void sub_080154C4(s16, u8);
void sub_08015504(s16, u8);
/* Promoted in src/decomp/c_080156FC.c and never declared -- the "promoted but
 * no prototype" trap again. Types copied from the definition, not re-derived.
 * Its only caller is the forwarder sub_080156E8 above. */
void sub_080156FC(s16, u16);
/* Both parameters narrow in place with no copy (`lsls #0x10; lsrs #0x10` into
 * r5 and r6 at entry), the same readout as sub_080504A8 above, so `u16, u16`
 * (wave 20, W20-C). Called as the tail of sub_08052718 and sub_08052BBC. */
void sub_08052818(u16, u16);
/* The first two parameters narrow at entry (`lsls #0x10; lsrs #0x10`), the
 * THIRD DOES NOT -- its only use is `lsls r2,#0x19; lsrs r2,#0x10`, i.e.
 * `(u16)(x * 0x200)`. That asymmetry is the readout: agbcc did NOT fold the
 * entry narrowing of r0 into r0's own first use (`lsls r4,r0,#4`), so a
 * missing pair on r2 means r2 is not a narrow parameter (wave 20, W20-C).
 * All three call sites -- sub_08050134, sub_08050364 and the pass-through
 * wrapper sub_0805040C -- are byte-identical with `u16` here, so this is the
 * weakest type that fits rather than a proof. */
void sub_08050424(u16, u16, int);
/* Third parameter is `int`: it arrives as `adds r4, r2, #0` and is narrowed
 * `lsls #0x10; asrs #0x10` only at the use, which is docs/agbcc-codegen.md's
 * copy-then-narrow readout. The first two narrow in place, so they are u16.
 * Void -- it tail-calls sub_080155C0 and no caller reads r0. Nine call sites;
 * sub_0804C828 / sub_0804CD84 pass gUnknown_03001FBC as the third argument
 * with NO `mov r2` at the call, because the allocator had already loaded it
 * there (wave 20, W20-C). */
void sub_0804DC5C(u16, u16, int);

/* sub_0803B3C8 has been PROMOTED (src/decomp/c_0803B3C8.c) since wave 9 and was
 * never declared here, because nothing had called it across a file boundary
 * yet. sub_0806C52C is the first, and the failure reads as a missing function
 * rather than a missing line -- see the wave-14 note in
 * docs/agbcc-codegen.md. Types copied from the definition, not re-derived. */
void sub_0803B3C8(void);

/* Returns 1 when gUnknown_03003FC0's byte 1 is 1 and sub_0803CBD8(0x60) is
 * non-zero, else 0. `int`, and this one is settled by population rather than by
 * the body: 20 call sites across asm/ and not one narrows the result -- 18 are
 * a bare `cmp r0, #0` (or a copy then a compare) and TWO use it directly as a
 * scaled index (`lsls r0, r0, #2` before an `ldr`), which is the value being
 * KEPT at full width and is the decisive half of that census. A `u8`/`bool8`
 * return would have put `lsls r0, #0x18` after every one of the 20 `bl`s. */
int sub_0803866C(void);

/* ---- wave 20 (W20-B) ----
 * A Proc_Start front end: it forwards (r0, r1) unchanged and passes r2 through
 * as sub_08071B28's fourth argument, which sub_08071B28 hands to Proc_Start as
 * the PARENT (`adds r1, r3, #0` ... `bl Proc_Start`) -- hence ProcPtr rather
 * than a bare pointer. Nothing is narrowed at entry in either function, so the
 * first two are `int`. `pop {r4,r5}; pop {r0}` -> void. */
void sub_08071B0C(int, int, ProcPtr);

/* ---- wave 23 (W23-B) ----
 * The same front end onto the same callee with a different script, and typed
 * by exact analogy: `sub_08071B28(&gUnknown_08613F54, a1, a2, a3)` with a3
 * landing in r3, which sub_08071B28 forwards to Proc_Start as the parent.
 * Bare prologue (`adds r4,r0,#0; adds r5,r1,#0; adds r3,r2,#0`) -> the first
 * two are wide. `pop {r4,r5}; pop {r0}` -> void. */
void sub_08071AF0(int, int, ProcPtr);

/* DEFINED in src/decomp/c_08084858.c (matched since wave 19); this publishes
 * it and RETYPES the return from `int` to `u8`. The body cannot tell the two
 * apart -- `ldrb r0,[r0]; bx lr` either way -- but two callers can, and both
 * were still asm when the definition was promoted: sub_08084864 and
 * sub_0808488C each follow the `bl` with a bare `lsls r0, r0, #0x18` before
 * the `cmp`, which is the narrow-return re-narrowing agbcc emits at every call
 * site, in its truth-test form. An `int` return emits no shift at all.
 * gUnknown_03000650 is `u8 []`, so `u8` is also the weakest type that fits.
 *   MEASURED CAVEAT, so nobody re-derives it: `int` plus an explicit `(u8)`
 * cast at each call site is BYTE-IDENTICAL here (the cast's `lsr` is dead in a
 * comparison against 0 and combine drops it), so the call sites do not
 * discriminate the two models -- they only rule out an uncast `int`. `u8` is
 * chosen because it needs no cast anywhere. sub_08080FE0 and sub_08084700, the
 * other two callers, are still asm and were not consulted. */
u8 sub_08084858(int);

/* Five arguments, typed from the callee's own entry narrowing and not from a
 * call site. r0 and r1 are masked with 0xFFFFFE00 / 0xFFFFFF00 and truncated to
 * u16 -- a 9-bit x and an 8-bit y, the same OAM coordinate pair sub_0801F34C
 * takes -- and r2/r3 pass through to `ip`/`r8` untouched, so all four are word
 * wide. The FIFTH arrives at [sp, #0x1c] and is narrowed on arrival
 * (`lsls r2, r2, #0x18`) before a bare `cmp r2, #0`, which is a sub-word
 * parameter used as a truth test. Both call sites in sub_08084F44 pass the
 * literal 1, so they do not discriminate `u8` from `int` and this is settled on
 * the callee side alone. */
void sub_08043C28(int, int, int, int, u8);

/* A Decompress front end: `lsls r0, r0, #3` indexes an 8-byte-stride table at
 * gUnknown_08616AC0 and the word it loads is Decompress's source. The scale is
 * the whole type argument -- an index used as `x * 8`, nothing narrowed at
 * entry, so `int`. `pop {r0}; bx r0` with r0 holding Decompress's return, which
 * no caller reads: sub_08082660 discards it, so `void`. */
void sub_080845A8(int);

/* Returns a PALETTE pointer, not a value: both arms end in an address --
 * `&gUnknown_0823DC38[x * 16]` when sub_08084858 says 0, and the fixed
 * gUnknown_0812596C otherwise -- and sub_08082660's only use hands the result
 * straight to ApplyPaletteExt's `u16 *` first parameter with no arithmetic.
 * The `lsls r0, r4, #5` is a BYTE offset, i.e. 32 bytes = 16 u16 = one 16-colour
 * palette, which is what fixes the element type. The argument is unnarrowed at
 * entry and is forwarded to sub_08084858(int), so `int`. */
u16 *sub_08084864(int);

/* The four helpers sub_0800CFDC drives, all of them (column, row) queries or
 * writes on the gUnknown_08499590 map descriptor. Every one opens with bare
 * `adds rN, r0, #0` / `adds rM, r1, #0` and narrows neither, so both parameters
 * are `int`; sub_0800CFDC passes `x - 1`, `x + 1`, `y + 1` and `y - 1` into them,
 * which an s16 pair would have re-narrowed at each of the 32 call sites.
 *   sub_0800E8CC and sub_0800E9F4 return through `adds r0, r6, #0` and their
 * results are used at full width -- sub_0800CFDC ANDs two of them together and
 * switches on the result over the range 6..31, and compares the other against 1
 * and 2 with no `lsls` in between -- so both are `int`. The other two end
 * `pop {r0}; bx r0`, i.e. void. */
int sub_0800E8CC(int, int);
int sub_0800E9F4(int, int);
void sub_0800EAF4(int, int);
void sub_0800EB5C(int, int);

/* Wave 50 (W50-I). sub_0800CFDC itself, the 6,384-byte driver the four helpers
 * above belong to, was never declared. It is parked, not matched, but
 * work/sub_0800CFDC/sub_0800CFDC.c settled its shape as `void (int, int)` and
 * its ONE caller sub_0800CF28 agrees: `adds r0,r4,#0; adds r1,r5,#0;
 * bl sub_0800CFDC` with nothing read out of r0 afterwards. */
void sub_0800CFDC(int, int);

/* ---- wave 21 (W21-A): the gUnknown_0200C020 pair ----
 * sub_08014074 is already matched in src/decomp/c_08014074.c and was simply
 * never declared; its parameter type is that file's, now shared through
 * unknown-globals.h.
 *
 * sub_080147B4 fills the same object, and its narrow parameters are what make
 * sub_08014668/sub_080146D4 match: EVERY narrowing in those two functions is a
 * conversion at this call, not a parameter-width tell of their own (the rule
 * the sub_0802D35C comment above states). Their six parameters all stay `int`
 * / `u16 *` as already declared.
 *   Parameters 2 and 3 are `s16` and NOT the u16 sub_080147B4's own prologue
 * shows -- PROMOTE_MODE zero-extends every narrow parameter at entry
 * regardless of signedness, so the prologue cannot separate them, and the
 * callers sign-extend (`lsls #0x10; asrs #0x10`) where 5..7 zero-extend
 * (`lsrs`). Parameter 4 is stored as a word at +0x28 and never narrowed, and
 * is `u16 *` only so that sub_08014668's third argument forwards without a
 * cast. Parameters 5 and 7 are `u16` on the body's own evidence (5 is
 * zero-extended and scaled `lsrs #0xe` into gUnknown_08610A38[]; 7 is
 * zero-extended and stored to two halfwords); 6 is only ever `strh`'d, so its
 * width is a floor and u16 is the weakest spelling that costs the callers
 * nothing. */
/* The two blob starters the sub_0802D4xx / sub_0802D5xx duplicate pairs use.
 * Both take the blob in r0 and neither dereferences it, hence `const void *`.
 * The narrow arguments are read off each callee's own prologue, which is the
 * only evidence: sub_08019F2C zero-extends r1, r2, r3 and the one stack word
 * at [sp, #0xc] (five parameters, the fifth arriving as a word) and forwards
 * all five to sub_08019F90; sub_0801A104 zero-extends r1, r2, r3 only (four
 * parameters). u16 rather than s16 is a floor -- PROMOTE_MODE cannot separate
 * them and no call site narrows in a way that would (wave 21, W21-A). */
/* Wave 26 (W26-A) corrects the RETURN type of sub_08019F2C from void to int.
 * Both it and its twin sub_08019F50 end `pop {r1}; bx r1`, the returns-a-value
 * epilogue, and a probe settles that the epilogue register IS the discriminator
 * here: the identical body written `void f(...) { g(...); }` pops r0 and written
 * `int f(...) { return g(...); }` pops r1. The value is sub_08019F90's, so that
 * one returns a value too. `int` on all three because every call site in the ROM
 * discards the result, which emits no narrowing and so cannot constrain the
 * width; the change is byte-neutral for callers for the same reason. */
int sub_08019F2C(const void *, u16, u16, u16, u16);
int sub_08019F50(const void *, u16, u16, u16, u16);
int sub_08019F90(const void *, u16, u16, u16, u16);
/* RETYPED IN WAVE 31 (W31-B), from `void`, on the discriminator the paragraph
 * above already established for sub_08019F2C / sub_08019F50: sub_0801A104 ends
 * `pop {r1}; bx r1`, the returns-a-value epilogue. Written `void` the body is
 * otherwise byte-identical but pops r0, and that 2-byte difference was the
 * whole remaining diff. It returns sub_08019F50's `int` result. Byte-neutral
 * for callers -- src/decomp/c_0802D458.c and c_0802D4B0.c both discard it, so
 * no narrowing appears at either call site; both re-verified with trymatch. */
int sub_0801A104(const void *, u16, u16, u16);
void sub_08014074(struct Unk08014074 *);
void sub_080147B4(struct Unk08014074 *, s16, s16, u16 *, u16, u16, u16);
/* Wave 40, W40-C. Advances the text cursor by `a2` sub-tile units: it adds to
 * the fractional accumulator unk40 and, for every whole 8 it carries out,
 * steps unk34 by 2 and unk32 by 1. Returns whether a fraction is left over.
 * The `struct Unk08014074 *` first parameter is read off the offsets alone
 * (unk32 u8, unk34 u16, unk40 u8, all already in that type) -- its only
 * caller sub_08014400 is still asm, so it is not corroborated caller-side. */
int sub_08014CA4(struct Unk08014074 *, int);

/* ---- wave 24 (W24-C), the 0x08078 block ---- */

/* Five already-promoted `void (void)` leaves that had no header declaration --
 * every one of them has a definition in src/decomp/ (c_08074744.c, c_08074F1C.c,
 * c_08075304.c, c_080755E0.c, c_0801C1F8.c) and the prototypes below just record
 * what those definitions already say. sub_08078018 calls the first four in a
 * row; sub_080780A0 calls the last. */
void sub_08074744(void);
void sub_08074F1C(void);
void sub_08075304(void);
void sub_080755E0(void);
void sub_0801C1F8(void);

/* Promoted as `u8 sub_0803BD6C(void)` in src/decomp/c_0803BD54.c, and
 * sub_080780A0 corroborates it from the caller side: the result is re-narrowed
 * `lsls #0x18; lsrs #0x18` before `cmp r0, #1`. */
u8 sub_0803BD6C(void);

/* Promoted as `void sub_08014BC0(ProcPtr)` in src/decomp/c_08014BB4.c. */
void sub_08014BC0(ProcPtr);

/* Starts the gUnknown_08615ACC proc under `parent` and stashes its four other
 * arguments at +0x2c, +0x30, +0x58 and +0x54 of it. Its prologue is bare for
 * r0/r1/r2/r3 -- no masking of any incoming register -- so the first three are
 * WIDE (wave 21's rule: PROMOTE_MODE would have cost a shift pair otherwise),
 * and the fifth argument arrives as a word at [sp, #0x14]. r3 is only ever
 * stored, never dereferenced, hence `const void *`. */
void sub_080785CC(s32, s32, s32, const void *, ProcPtr);

/* The fourth parameter is `lsls #0x18`-tested at entry and nothing else, so it
 * is a narrow flag; r1 and r2 are untouched at entry and are `strh`'d later,
 * which makes their width a floor and `s32` the weakest spelling. r0 is
 * null-checked and handed to Proc_StartBlocking as a parent. Returns 0 or 1. */
s32 sub_08074C84(ProcPtr, s32, s32, u8);

/* r0 walks a `const s8 *` at word 0 of the struct it points at and compares a
 * count against the u8s at +4 and +5; r1 is the caller's own proc, which it
 * writes +0x58 of on one path. The return is re-narrowed `lsls #0x18` by
 * sub_0807831C, its only caller, so it is a u8 predicate. */
u8 sub_080782C0(struct Unk80782C0 *, ProcPtr);

/* All three parameters are read straight off this function's own prologue,
 * which zero-extends r0 by 16 and r1/r2 by 24. u16/u8/u8 rather than the signed
 * spellings is a floor -- PROMOTE_MODE cannot separate them -- but the widths
 * are hard, and sub_08078498 pays for them with three shift pairs at the call. */
void sub_08019818(u16, u8, u8);

/* Matched in src/decomp/c_08076770.c and src/decomp/c_08078770.c; these just
 * record what those definitions already say, for sub_08078568 and for the
 * sub_080787B8 / sub_08078800 pair. */
void sub_08076770(s32, s32, s32, ProcPtr);
void sub_08078770(void);

/* ---- wave 24 (W24-C), the 0x08078 block, second pass ---- */

/* RETURN TYPE SETTLED FROM THE CALLER, and it is not what the body suggests.
 * sub_08078E20's own body is `movs r0,#1` / `movs r0,#0`, which says nothing;
 * sub_08078E94, its only caller, re-narrows the result with a BARE
 * `lsls r0, #0x18` before `cmp r0, #0`. A bare lsls is a truth test on a NARROW
 * value, so the declared return is u8. Declaring it `s32` compiles the callee
 * byte-identically -- 0 and 1 need no narrowing -- and is invisible until a
 * caller like this one appears, which is the failure mode the prototype rules
 * in the wave brief describe. */
u8 sub_08078E20(void);

/* sub_08075E68's prologue reads r0 only, and r1 survives untouched into
 * `bl Proc_StartBlocking` -- a pass-through parent, invisible in the body.
 * Both of sub_08078270's arguments are accounted for that way. */
void sub_08075E68(s32, ProcPtr);

/* WAVE 36 (W36-E): RETYPED u16 -> int, and the old evidence did not
 * discriminate. It read "the result is re-narrowed `lsls #0x10; lsrs #0x10` at
 * sub_08078E48's call site and handed straight to sub_0801A5B0(u16)" -- but an
 * `int` return handed to a `u16` parameter emits exactly that shift pair, so
 * that site is silent about the return type. sub_0807A860 IS decisive: it uses
 * the result directly as a `* 0x3c` array subscript with NO narrowing at all,
 * and agbcc re-narrows a narrow-returning callee's result at EVERY call site.
 * sub_08078E48 was re-verified byte-for-byte after the change. */
int sub_0807A908(void);

/* First parameter WIDE (bare prologue, `adds r5, r0, #0`, then compared
 * against 0 and decremented); second zero-extended by 24 at entry, so u8. */
void sub_0807A99C(s32, u8);

/* Both promoted already, neither previously declared: src/decomp/c_08078AF0.c
 * and the sub_08078E20 draft. */
void sub_08078AF0(void);

/* ---- wave 26 (W26-A), the 0x08019000 block ---- */

/* Three already-promoted definitions that had no declaration, copied verbatim
 * from src/decomp/c_08019C24.c, c_08022ADC.c and c_0801A604.c. */
void sub_08019C24(void);
void sub_08022ADC(void);
void sub_0801A604(void);

/* Argument-free and result-discarded at every site in this block; each one's
 * own prologue writes r0 from a literal pool before reading any argument
 * register, and each ends `pop {r0}` rather than `pop {r1}`, so `void (void)`
 * is read off the callee and not guessed from the call.
 *   sub_08014878  three `sub_0801537C(<script>)` calls in a row.
 *   sub_08019380  the sweep that clears gUnknown_0200C508[0..7] backwards.
 * sub_0803670C is the one INFERRED member: no caller in this block sets an
 * argument register for it, which fixes the arity at zero, but its own body was
 * not read. */
void sub_08014878(void);
void sub_08019380(void);
void sub_0803670C(void);

/* All six parameters WIDE, read off sub_08014A5C's own prologue: r1, r2 and r3
 * go straight into r8/sb/r4 and the fifth arrives as a whole word at [sp,#0x20]
 * -- not one lsl/lsr pair among them, so nothing here is narrow. The third is
 * `void *` because sub_080195C8's caller hands it the same +0x18 word that
 * sub_08019578 dereferences. Result discarded, hence void. */
void sub_08014A5C(int, int, void *, int, int, int);

/* The three walkers over the 0x48-byte object sub_08019B50 models (see its
 * declaration above): all three read exactly r0 and take nothing else.
 * sub_08019B80's return is `u8` from its ONE result-keeping caller,
 * sub_08019D0C, which truth-tests it with a bare `lsls #0x18` -- that is a
 * floor on the narrowing, not proof of a boolean, so `u8` rather than `bool8`.
 * sub_08019578 walks a different object (the +0x18/+0x1e/+0x30 one that
 * sub_080195C8 owns) and is `void *` for the same "not modelled yet" reason. */
void sub_08019A60(void *);
u8 sub_08019B80(void *);
void sub_08019578(void *);
void sub_080196F4(void *);

/* Proc callbacks in the 0x08019D00 group, each installed by sub_08019F90 as a
 * function-pointer word (`ldr r0, =sub_08019D78`). sub_08019D48 is declared
 * here because sub_08019D78 and sub_08019DA8 forward their own untouched r0 to
 * it. */
void sub_08019D48(ProcPtr);

/* Reached only through sub_080192EC, which computes &gUnknown_0200C528[i] and
 * passes it -- so the parameter is the slot itself, not an index. */
void sub_080192C4(struct Unk0200C528 *);

/* Both parameters are `s16` and that is MEASURED, against the promoted
 * `void sub_08022AD0(u16, u16)` in src/decomp/c_08022AD0.c that wave 26 has now
 * corrected. sub_08019DCC hands sub_08022AD0 an `ldrsh` member with no
 * conversion at all and a computed expression narrowed `lsls #0x10; asrs
 * #0x10`; the u16 spelling probes as `ldrh` plus a zero-extending `lsrs`, which
 * is not what the ROM does. sub_0802323C's second parameter shows the identical
 * sign-extending shape in sub_08019DEC. The definition's own body is two plain
 * `strh`s and is byte-identical either way -- c_08022AD0.c was re-verified with
 * trymatch after the change -- so the caller is the only evidence there is.
 * sub_0802323C's third parameter is a bare `movs r2, #3`, hence wide. */
void sub_08022AD0(s16, s16);
void sub_0802323C(s16, s16, int);

/* Wave 26, W26-C: callees of the 0x08017658-0x08017E08 gUnknown_0200C528
 * script-command block.
 *
 * The nullary voids first -- each is reached with no argument register set up,
 * each ends `pop {r0}`, and every caller discards r0. sub_08026798 zeroes
 * gUnknown_030032C0 then calls sub_08020984; sub_080185A0 copies
 * gUnknown_08499588 to 0x06006800. */
void sub_08026798(void);
void sub_080185A0(void);
/* Already promoted in src/decomp/c_080179D0.c and declared from that
 * definition; sub_08017994 needs it visible from another translation unit. */
void sub_080179D0(u16 *);
/* `s16` off its own prologue -- `lsls #0x10; asrs #0x10` before it indexes
 * gUnknown_0200C528 -- and void (`pop {r0}`). */
void sub_080192EC(s16);
/* The twin of sub_08029088 above: the same `lsls #0x10; asrs #0x10` pair on r0
 * and r1, then `movs r2, #1` and a tail call into sub_080290B0. */
void sub_0802909C(s16, s16);
/* Three `s16` parameters, all three sign-extended in the prologue. It returns
 * sub_08025C5C's object or NULL; nothing models that pointee yet, so `void *`.
 * Both callers discard the result, so the return type has no oracle today. */
void *sub_08025C98(s16, s16, s16);
/* NULLARY, even though its one call site happens to leave a pointer live in r0
 * across the `bl`: the body's first instruction pair is `ldr r0,
 * =gUnknown_08499EE4; bl sub_08015BD0`, so it overwrites r0 before any read
 * and there is no parameter. It returns 1/0 through the split-`b` form and
 * sub_08017ABC re-narrows with `lsls #0x18; lsrs #0x18`, hence u8. */
bool8 sub_080281A0(void);
/* Scans gUnknown_085C77A0[].unk2c for its argument and returns the matching
 * index, or the 0xbf bound. The scan counter is kept `lsls #0x10; lsrs #0x10`
 * right up to `adds r0, r1, #0`, so the return is `u16`; the parameter is the
 * `u32` that unk2c already is (bare `adds r2, r0, #0`, no narrowing). */
u16 sub_080206B0(u32);
/* Its FIRST parameter is dead -- the body clobbers r0 with the
 * gUnknown_08499EE4 pool word before reading it -- and is readable only at
 * sub_08017B08, which loads it `ldrb` from gUnknown_03003FC0.unk02. The second
 * is stored whole with `str` into the resulting slot's +0x18. */
void sub_080281D8(u8, u32);

/* The gUnknown_0200C528 script-command handlers take the slot index and return
 * the "advance to the next command" flag. The RETURN TYPE IS MEASURED here,
 * and it is not the `bool8` the promoted files in this block guessed:
 *
 *   sub_08017D30 is `if (g != 1) return sub_08017A58(a); else { advance;
 *   return TRUE; }`, and the ROM narrows sub_08017A58's result with
 *   `lsls #0x10; asrs #0x10`. A `bool8` callee probes as `lsls #0x18;
 *   lsrs #0x18` there (measured with compile_probe, wave 26), and no `bool8`
 *   spelling of sub_08017D30 produces a 16-bit pair at all. sub_08017CF0 shows
 *   the identical shape over sub_08017A80.
 *
 * src/decomp/c_08017A58.c was therefore retyped `bool8` -> `s16` this wave. Its
 * own body is `return FALSE`, byte-identical either way, and it was re-verified
 * with trymatch after the change.
 *
 * The same argument makes the WHOLE family `s16 (s16)`: a wrapper that returns
 * its wrapped handler's value with no conversion must share that handler's
 * type, and these all sit in one dispatch table. c_08017B64.c, c_08017BD4.c
 * (three functions), c_08017D70.c and the `bool8` members of this block are
 * very likely mistyped the same way -- but every one of them returns a
 * CONSTANT, so none of them has an oracle and nothing forces the edit. They are
 * left alone deliberately. Settle the family in one pass, not piecemeal. */
s16 sub_08017A58(s16);
s16 sub_08017A80(s16);
/* Wave 37, W37-O2. sub_08019404 is that family seen from the DISPATCHER
 * side: it loops `while (gUnknown_0848A244[node->filler_00[0]](a) != 0)` with
 * the sign-extended slot index in r0 and tests the result `lsls #0x10;
 * cmp #0; bne`, a 16-bit truncate -- the same width the handlers return.
 * sub_08017988 is `s16` on caller-side evidence: sub_08019470 opens with
 * `bl sub_08017988; lsls r0,#0x10; asrs r2,r0,#0x10; cmp r2,#0`, and then
 * REUSES that sign-extended zero as the halfword it stores, so the narrowing
 * is the return conversion and not a cast at a use. */
s16 sub_08017988(void);
void sub_08019404(s16);
/* Wave 37, W37-O2. Two leaves in the same block, neither with a caller in
 * the index -- both signatures are read off their own prologues and both are
 * byte-verified. sub_08019688 walks a byte stream: `p` is the cursor it
 * returns, `term` the record terminator it scans for, `count` how many
 * records to skip and `stride` the step it takes while scanning. All three
 * scalars are `u8` because the prologue truncates each with
 * `lsls #0x18; lsrs #0x18` -- PROMOTE_MODE would have left an `int` alone.
 * sub_08019910 is a predicate over gUnknown_03003110. */
u8 *sub_08019688(u8 *, u8, u8, u8);
bool8 sub_08019910(void);
/* The two callbacks this block installs into gUnknown_0200C528[i].unk08, each
 * of which clears that same slot when its own predicate fires. That
 * install/remove pair is what fixes the parameter as `struct Unk0200C528 *`:
 * the `str` is at +8 of the 0x18-byte SLOT, not of the 0x10-byte node. */
void sub_08017ABC(struct Unk0200C528 *);
void sub_08017C4C(struct Unk0200C528 *);

/* Wave 28, W28-A: five more of the same kind at 0x08018000, all installed into
 * a slot's `.unk08` and all clearing it again on their own predicate.
 * sub_08018A28 is the decisive one for the SLOT-not-node reading a second time:
 * it reads +0x11 with `ldrb` and +0x12 with `ldrsh`, and sub_08018A64 -- which
 * installs it -- writes those two members of `&gUnknown_0200C528[a]` from the
 * node's unk08/unk0a in the instructions just before the `str` at +8.
 * sub_080180A8 and sub_08018DF8 are reached only as installed callbacks too
 * (from sub_080180CC and from the +0x14 slot pointer respectively). */
void sub_080180A8(struct Unk0200C528 *);
void sub_080180CC(struct Unk0200C528 *);
/* Wave 32, W32-A: two more of the same install chain -- sub_0801853C stores
 * sub_080184EC into `&gUnknown_0200C528[a].unk08` and sub_080184EC stores
 * sub_080184E0 into its own slot's +8, so all three walk the same 0x18-byte
 * SLOT. sub_080184E0 was promoted in wave 28 against a file-local duplicate of
 * that struct (`struct Unk80184D8`, filler[8] + a callback word); it is
 * retyped to the shared struct here so the two files can name one type. */
void sub_080184E0(struct Unk0200C528 *);
void sub_080184EC(struct Unk0200C528 *);
void sub_080185BC(struct Unk0200C528 *);
/* Wave 33, W33-G. The two gUnknown_0200C528 list-script handlers next door to
 * sub_08018758, and the same bool8 shape: `movs r3,#0` / `movs r3,#1` split
 * across a `b` into one `adds r0,r3,#0`. sub_08014004's argument is the
 * `u8 *` element of gUnknown_08610A38[] (the sub_080149C0 precedent above) and
 * its result is re-narrowed `lsls #0x18` at the call site, which is a byte
 * return. */
bool8 sub_08014004(u8 *);
bool8 sub_080185D0(s16);
bool8 sub_08018694(s16);
void sub_08018A28(struct Unk0200C528 *);
void sub_08018AA8(struct Unk0200C528 *);
void sub_08018DF8(struct Unk0200C528 *);

/* Wave 26, W26-C extension: callees of the 0x08042 block.
 *
 * sub_08043120 / sub_08043190 / sub_08043200 are sub_080430B0's siblings,
 * declared above as `int (int, int, int)`. Every one of them is reached from
 * the same call shape -- `ldrb` of gUnknown_08499598[a].unk1d and .unk1e into
 * r0/r1 with the caller's second argument already parked in r2 -- so they take
 * the same three wide arguments and their results are added to or offset by a
 * constant, hence `int`. */
int sub_08043120(int, int, int);
int sub_08043190(int, int, int);
int sub_08043200(int, int, int);
/* The one-argument partners called first in sub_08042D1C / sub_08042D50, whose
 * results are added to the above. `int` on both counts: the argument is
 * forwarded from a bare `adds r0, rN, #0` with no narrowing, and the result
 * feeds an `adds`. */
int sub_080433B8(int);
int sub_080433C8(int);
/* sub_080425E0 and sub_080425FC narrow their own parameter `lsls #0x18;
 * lsrs #0x18` and hand it straight over, so the first parameter is `u8`; the
 * second is a bare `movs r1, #1` / `#0`. sub_08041820's three are all wide --
 * its two callers copy r0 and r1 with bare `adds` and add a `movs r2`. Both
 * results are discarded and both callers end `pop {r0}`. */
void sub_08041978(u8, int);
void sub_08041820(int, int, int);
/* sub_080424BC is promoted in src/decomp/c_080424BC.c and declared from that
 * definition. sub_080425B8 is the shared prologue its four 28-byte wrappers all
 * open with -- nullary, `pop {r0}`, and no caller reads r0. */
void sub_080424BC(void);
void sub_080425B8(void);
/* sub_080432A8 is another of the sub_080430B0 siblings but takes only TWO
 * arguments -- sub_08042D84 sets up r0 and r1 from the record and nothing else.
 * sub_080433E8 is the one-argument partner whose result is added to it, exactly
 * as sub_080433B8 / sub_080433C8 pair with theirs. sub_08043050's result is
 * masked `& 0x80` by sub_08042C68, so it is at least 8 bits wide; the promoted
 * definition in src/decomp/c_08042E2C.c names `u32`, and u32 vs int is
 * word-width both ways -- byte-neutral at every call site -- so the definition
 * wins. Its argument is forwarded from a bare register. */
int sub_080432A8(int, int);
int sub_080433E8(int);
u32 sub_08043050(int);
/* sub_08043270 / sub_080433D8 are the sub_08043200 / sub_080433C8 pair one more
 * time, reached by sub_08042C9C with the identical call shape. */
int sub_08043270(int, int, int);
int sub_080433D8(int);
/* sub_0804C400 passes it the slot index and gUnknown_08552148[index], both with
 * a bare `adds`, and discards nothing -- `pop {r0}`, void.
 *
 * Wave 34 (W34-B) narrows both to `u16`. The body opens with a single
 * `lsls r0,#0x10; lsrs r0,#0x10` BEFORE the first store, which is PROMOTE_MODE
 * on a declared-narrow parameter; `int` emits no shift there at all, because
 * the only consumers are a `strh` into gUnknown_0300453C and a `lsls #1`
 * table index, neither of which needs a zero-extension of its own. An explicit
 * `(u16)` cast would put the pair at the USE rather than at entry.
 *   Parameter 2 is never read: the body recomputes gUnknown_08552148[a] for
 * itself rather than using the value sub_0804C400 hands it. Kept because the
 * caller does pass it -- the same situation sub_0804E100 documents above.
 * Byte-neutral at that call site (a u16 rvalue needs no conversion either
 * way); c_0804C400.c re-verified as still matching after the change. */
void sub_0804C340(u16, u16);

/* sub_0804C268 is the same function as sub_0804C340 with different arguments --
 * 216 B against 192 B, the same statement order, the same single callee, and
 * the same eight data refs. Both matched byte-for-byte in wave 51 (W51-D) on
 * this signature. Both parameters ARE read here (unlike sub_0804C340's second),
 * and both carry the entry `lsls #0x10; lsrs #0x10` pair that PROMOTE_MODE
 * emits for a declared-narrow parameter; `int` emits no shift there at all.
 * No caller exists in src/decomp yet, so this declaration constrains nothing
 * that is already promoted. */
void sub_0804C268(u16, u16);

/* ---- wave 26 (W26-A), the 0x08004000 extension block ---- */

/* Three already-promoted `void (void)` leaves that had no declaration --
 * src/decomp/c_08003704.c, c_080037AC.c and c_08003040.c. sub_08003704 and
 * sub_080037AC are byte-for-byte the same function. */
void sub_08003704(void);
void sub_080037AC(void);
void sub_08003040(void);

/* `void (void)`: two pool-fed calls and `pop {r0}`, no argument register read. */
void sub_08002E3C(void);

/* WIDE and SIGNED, and read off the callee: the prologue is a bare
 * `adds r5, r0, #0` followed by `cmp r5, #0; bge`, which is a signed test on a
 * whole word. It agrees with src/decomp/c_08004A60.c's note that
 * sub_08004A30's own parameter is `int` because sub_08004A90 passes -1, and
 * sub_08004A30 forwards it here untouched. */
void sub_08003C48(int);

/* Parameters 2, 3 and 4 are zero-extended `lsls #0x10; lsrs #0x10` at entry, so
 * u16; the first is a pointer nothing narrows. Both call sites in the 0x08004
 * block (sub_08004B7C, sub_08004C34) discard the result. */
/* RETYPED IN WAVE 31 (W31-B), from `void`, for the same reason as
 * sub_0801A104: the epilogue is `pop {r1}; bx r1` and it returns
 * sub_08019F2C's `int`. Byte-neutral for src/decomp/c_08004B7C.c and
 * c_08004BD8.c, which discard it; both re-verified with trymatch. */
int sub_0801A148(const void *, u16, u16, u16);

/* Four wide parameters. sub_08004D74 forwards its own r0 and r1 untouched and
 * adds a `u8 *` third and a literal fourth; sub_08004D90 passes all four. The
 * callee's prologue narrows none of them (`adds r7, r2, #0`, `cmp r0, #0`). */
void sub_08004DD4(int, int, u8 *, int);

/* Two wide parameters -- `adds r5, r0, #0; mov r8, r1` with no narrowing. The
 * second is a VRAM destination sub_08004D90 computes as
 * `&gUnknown_08499578[...]`. */
void sub_0801F2AC(int, u16 *);

/* Wave 37, W37-A. The two forwarders described in the note above, declared for
 * their only callers sub_0800572C and sub_08005874. sub_08004D74 pushes only
 * {lr} and forwards r0/r1 into sub_08004DD4 untouched, adding
 * gUnknown_08610A38[0x9FA] and a literal 0 -- so two wide parameters. Both end
 * `pop {r0}; bx r0`, the VOID epilogue. sub_08004D90's third parameter is the
 * same `u8 *` sub_08004DD4 declares: both call sites hand it
 * `gUnknown_0200B204` straight through with `adds r2, rN, #0`. */
void sub_08004D74(int, int);
void sub_08004D90(int, int, u8 *);

/* Wave 37, W37-A. The string-nonempty test the 0x0800520C triple guards its
 * sub_08004E38 copy with; `const char *` follows sub_08004E38's own second
 * parameter, which is the string it copies INTO the same buffer. The return is
 * NARROW -- all three call sites do a bare `lsls r0, r0, #0x18; cmp r0, #0`
 * with no `lsrs`, which is the truth-test-of-a-narrow-return shape and is what
 * an `int`-returning callee cannot produce. NOTE: sub_080051EC itself is still
 * unmatched and its listing in asm/ does not read as compiler output (a `cmp`
 * of a byte against a pool ADDRESS, and a backward branch into the middle of a
 * two-instruction block); the prototype here is read off the CALLERS only. */
bool8 sub_080051EC(const char *);

/* Wave 37, W37-A: promoted in src/decomp/c_080059E4.c but never declared;
 * sub_08005AA0 tail-calls it. */
void sub_080059E4(void);

/* Wave 37, W37-A: the two three-slot screen builders, matched in this wave and
 * called by sub_080057EC / sub_08005964 respectively. Both end
 * `pop {rN}; pop {r0}; bx r0` and neither caller looks at r0. */
void sub_0800572C(void);
void sub_08005874(void);

/* The two targets in this block that other members of it call and that were not
 * already declared above (sub_0800485C is at line 1065, sub_08004A30 at 54).
 * sub_08004E44 returns a value (`pop {r1}; bx r1`) and every caller discards
 * it, so `int` is the weakest fit. */
void sub_08004C10(void);
void sub_08004C5C(void);
int sub_08004E44(void);

/* ------------------------------------------------------------------ *
 * Wave 27, W27-B: the 0x0803C block                                   *
 * ------------------------------------------------------------------ */

/* The two upper ranges of sub_0803CBA0's three-way bit-id dispatch, the same
 * family as the promoted sub_0803C8F0. The second parameter arrives
 * `lsls #0x18; lsrs #0x18` at both call sites, so `u8`; the first is the
 * biased id (`id - 0x60`, `id - 0x20`) with no narrowing on it at all, so
 * `u32` to agree with sub_0803C8F0 and sub_0803CA70. */
void sub_0803C9D4(u32, u8);
void sub_0803CA00(u32, u8);

/* Wave 37, W37-Q1: four more members of the same bit-setter family that have
 * had promoted definitions in src/decomp/c_0803C8F0.c and c_0803C950.c since
 * wave 27 but were never DECLARED anywhere. Signatures copied verbatim from
 * those definitions -- they are authoritative over any caller-side guess. */
void sub_0803C8F0(u32, u8);
void sub_0803C950(u32, u8);
void sub_0803C97C(u32, u8);
void sub_0803C9A8(u32, u8);

/* Wave 37, W37-Q1, the gUnknown_02027FB0 / gUnknown_0849EDB0 group.
 *
 * sub_0803C750 takes one of the gUnknown_0849EDB0[].unk08 callbacks and either
 * claims a free slot or bumps an existing one's use count; it returns 1/0 and
 * its caller tests the result `lsls #0x18; cmp #0`, so `bool8`.
 *
 * sub_0803C784 fills its caller's buffer with the indices of the rows whose
 * unk08 both returns 1 and is admitted by sub_0803C750, terminated by 0xff,
 * capped at 32 entries -- `u8 *` out-parameter, no return.
 *
 * sub_0803C670 and sub_0803C1D4 take nothing and return nothing: both are
 * reached by a bare `bl` and neither leaves a value in r0 across its epilogue
 * (`pop {r0}; bx r0` overwrites it). */
bool8 sub_0803C750(int (*)(int));
void sub_0803C784(u8 *);
void sub_0803C670(void);
void sub_0803C1D4(void);

/* The bottom range of that same dispatch, matched in wave 27. Neither
 * parameter is narrowed -- sub_0803CBA0 forwards both registers untouched,
 * which is what separates it from the two above. The bit index is used BOTH as
 * `id >> 3` (arithmetic `asrs`, so signed) and as the shift count of
 * `1 << id`, un-masked, exactly as in the promoted reader sub_0803CB74. */
void sub_0803CB40(int, int);

/* Matched in wave 27. `u8` return: sub_0803CC64 re-narrows the result
 * `lsls #0x18; lsrs #0x18` before comparing it against the 0xff empty-slot
 * sentinel, and agbcc only emits that for a narrow-returning callee. The
 * parameter is `u16` from the `lsls #0x10; lsrs #0x10` the caller applies to
 * its own parameter before the `bl`. */
u8 sub_0802490C(u16);

/* The promoted string copier in src/decomp/c_0803CC84.c, declared here so
 * sub_0803CCB8 can call it. */
void sub_0803CC84(u8 *, const u8 *);

/* Wave 41 (W41-C). Copied verbatim from the promoted definition in
 * src/decomp/c_0803CF04.c so sub_08021750 can call it; the definition wins over
 * anything a call site would suggest.  Note the second parameter really is
 * `int` and not a pointer -- sub_08021750 hands it `gUnknown_08499590 + 0x421A`
 * and needs an explicit cast.  Its first parameter being u8 is what the
 * `adds r0,#0x4c; lsls #0x18; lsrs #0x18` at that call site is narrowing: the
 * +0x4c happens on the caller's own int parameter and the u8 truncation is the
 * conversion to THIS declaration. */
void sub_0803CF3C(u8, int);

/* sub_0803CF04's two callees. sub_0801AC58's first parameter is `u8` -- the
 * caller computes `a + 5` and then narrows `lsls #0x18; lsrs #0x18` before the
 * `bl`, which is the callee's width showing through. sub_0803D2F8's first is
 * sub_0803CF04's own second parameter forwarded with nothing done to it. Both
 * second parameters are &gUnknown_02000000. */
/* RETURNS int, not void (wave 29, W29-B): both exits set r0 (`movs r0,#1` at
 * _0801ACAE, `movs r0,#0` at _0801AD48) and its new caller sub_08016E3C
 * branches on the result (`cmp r0,#0; bne`). sub_0803CF04 discards it, which is
 * why the void spelling survived. */
int sub_0801AC58(u8, u8 *);
void sub_0803D2F8(int, u8 *);
void sub_0803D238(u8 *);

/* Wave 43 (W43-G). Copied verbatim from the promoted definition in
 * src/decomp/c_080215FC.c so sub_0803D2F8 can call it. */
void sub_080215FC(void);

/* Wave 43 (W43-G). The parameter is `u8`: the prologue narrows r0 in place
 * (`lsls #0x18; lsrs #0x18`) and every later use reads the narrowed value.
 * RETURNS int -- both exits set r0 (`movs r0,#0` at _0803D4CE, `movs r0,#1`
 * at _0803D532) -- but the width is NOT settled, because the only caller
 * (sub_0803D48C, a 0..11 loop) discards it and so re-narrows nothing. `int`
 * is the weakest spelling that fits and costs no caller a conversion; if a
 * caller ever appears that tests the result, it may well be bool8. */
int sub_0803D4A8(u8);

/* Matched in wave 27. The first parameter is `u8` (`lsls #0x18; lsrs #0x18`
 * in the prologue, in place on r0); the second is never touched -- it is
 * forwarded straight to sub_0803D2F8 -- and sub_0803CF3C is a pass-through
 * wrapper for the pair, so its arity is read off THIS prologue, which does
 * read r1. */
void sub_0803CF04(u8, int);

/* ------------------------------------------------------------------ *
 * Wave 27, W27-B: callees of the 0x08037 block that had no prototype  *
 * ------------------------------------------------------------------ */

/* Nullary and result-discarding, all read off their own call sites in
 * sub_08037124 / sub_08037750 / sub_08037E64: the call is a bare `bl` with no
 * argument register set up before it and nothing reads r0 after it, and each
 * caller's own epilogue is the void `pop {r0}; bx r0`. */
void sub_080169E8(void);
void sub_08036B34(void);
void sub_0803D6B8(void);
void sub_08037DC8(void);

/* sub_080375A4 is `sub_08037448(gUnknown_08090EF0[a])` -- the argument arrives
 * already zero-extended by the `ldrb` of the table read, so nothing in the
 * caller fixes the width and `int` is the weakest fit. The result is
 * discarded (sub_080375A4 ends `pop {r0}; bx r0`). */
/* Wave 43 (W43-D): RETYPED from `void sub_08037448(int)`. The body returns 1
 * or 0 in r0 (`movs r0, #1` / `movs r0, #0` both reaching the epilogue), which
 * no `void` function can be spelled to do; and the parameter is narrowed
 * `lsls #0x18; lsrs #0x18` on entry with NO preceding copy, which is
 * PROMOTE_MODE's u8 parameter conversion rather than a cast at a use.
 * Byte-neutral at the only promoted caller: c_080375A4.c passes a `const u8`
 * element, which converts to u8 and to int identically, and discards the
 * result. c_080375A4.c was re-verified with trymatch after the change. */
u8 sub_08037448(u8);

/* Wave 43 (W43-D), settled from the definition rather than a call site.
 * `u8` return: sub_08037448 re-narrows the result `lsls #0x18` before testing
 * it, which agbcc only emits for a narrow-returning callee, and the two
 * `movs r0, #0/1` need no narrowing of their own. Both parameters are narrowed
 * `lsls #0x10; lsrs #0x10` on entry. */
u8 sub_080373F0(u16, u16);

/* Wave 43 (W43-D): both of these are DEFINED in src/decomp (c_08026340.c and
 * c_0803CC3C.c) but had no declaration here, so sub_080373F0/sub_08037448 --
 * the first callers outside those files -- would have got the C89 implicit
 * `int ()`. Copied from the promoted definitions, not guessed: an implicit
 * `int` return for sub_08026340 loses the caller's `lsls #0x18; lsrs #0x18`.
 * sub_0803CC64 genuinely returns `int`, so its caller's `lsls #0x18` is a u8
 * cast written in the source, not agbcc re-narrowing a u8 callee. */
u8 sub_08026340(void);
int sub_0803CC64(u16);

/* Matched in wave 27. `u8` and not `int`: the `lsls #0x18; lsrs #0x18` is in
 * the prologue operating on r0 IN PLACE, which is PROMOTE_MODE narrowing a
 * sub-word parameter, not a cast at a use (that would copy first). Its one
 * caller sub_08037E64 passes gUnknown_03003FC0.unk01, itself a `u8`. */
void sub_080375A4(u8);

/* The three arms of sub_080375D4's `switch (p->unk1e++ & 0x3f)`, each called as
 * `f(p->unk18)`. struct Unk03001470's unk18 is already `int`, so these agree
 * with it; the results are discarded. */
void sub_0801B6EC(void *);
void sub_0801B6FC(void *);
void sub_08037A78(int);

/* The two halves of sub_08037638. sub_08037610 stashes its argument at
 * +0x18 of a fresh gUnknown_03001470 slot (see that struct's unk18 note), and
 * sub_08037638 hands it `a + ((c & 0x3ff) << 5)`. sub_0803768C then takes
 * sub_08037638's four arguments unchanged and untouched -- no narrowing on
 * either side, so all four are word-wide. */
void sub_08037610(int);
void sub_0803768C(int, int, int, int);

/* Same shape as ApplyPaletteExt, and sub_08037790 is its only readable call
 * site: a palette source, a byte offset and a byte count. The offset is
 * `gUnknown_0300057C * 0x20 + 0x1c` narrowed `lsls #0x10; lsrs #0x10` at the
 * call, which is where the `u16` comes from -- an `int` parameter would leave
 * the shift pair out. The count is the literal 2. */
void sub_0801368C(u16 *, u16, u16);

/* sub_08037FB4 is `sub_08049F08(1, parent)`. gUnknown_0849F4B4's note already
 * records that sub_08049F08 Proc_StartBlocking's a script and forwards its own
 * parent, which is what fixes the second parameter; the first is the literal 1
 * selecting among scripts. The result is discarded. */
void sub_08049F08(int, ProcPtr);

/* --- the 0x08044 block (wave 27, W27-C) ---------------------------------- */

/* Returns a byte: sub_080448E4 re-narrows the result with `lsls #0x18;
 * lsrs #0x18` before storing it, which agbcc only emits for a narrow-returning
 * callee. Declared without a prototype -- the two arguments it is passed there
 * are gUnknown_030033EC (u16) and the literal 1, and neither settles a
 * declared parameter type.
 *
 * PROTOTYPED, wave 36 (W36-D), from the BODY, which the call site could not
 * show: sub_0805C290 opens `lsls r0,#0x10; lsrs r4,r0,#0x10` and
 * `lsls r1,#0x18; lsrs r5,r1,#0x18` -- a PROMOTE_MODE narrowing pair on each of
 * two argument registers, which agbcc emits only for a declared `u16` and a
 * declared `u8` parameter. Byte-neutral at sub_080448E4, whose arguments were
 * already a u16 global and the literal 1; c_080448E4.c re-verified with
 * trymatch after the change. */
u8 sub_0805C290(u16, u8);

/* Wave 36, W36-D. The three arms of sub_0805C290's `rand() % 3` dispatch.
 * Parameter types are its own two parameters forwarded unchanged, and the u8
 * return is the `lsls #0x18; lsrs #0x18` sub_0805C290 applies to all three
 * results on the shared tail. Bodies not read. */
u8 sub_0805C2DC(u16, u8);
u8 sub_0805C514(u16, u8);
u8 sub_0805C720(u16, u8);

/* --- the 0x08058 cell-search block (wave 36, W36-D) ----------------------- */

/* The scoring core of the block and its accept-only twin, both matched this
 * wave. Five and three parameters; the fifth of sub_08058CE8 arrives at
 * `[sp, #0x20]` (five pushed words plus three = 0x20, so the first stack
 * argument). Both end `pop {r0}; bx r0`, so void. The (x, y) out-pointer is
 * `u16 *` and not `s16 *` on the CALLERS' evidence: sub_08058BB4,
 * sub_08058C54 and sub_08058DEC all re-read element 0 with a plain `ldrh` to
 * compare it against the 0x270F sentinel they seeded, where an s16 local would
 * have used `ldrsh` (sub_08058F90 does exactly that for its own stack s16).
 * sub_08058CE8's fourth parameter is a running best score, read AND written. */
void sub_08058CE8(int, int, int, int *, u16 *);
void sub_08058E88(int, int, u16 *);

/* Wave 36, W36-D, declared from its call site in sub_08058F90 -- the body was
 * not read. Three arguments: a 1-based army index (the literals 1..4), a
 * pointer to sub_08058F90's own stack `s16` sentinel, and sub_08058F90's
 * pointer parameter forwarded unchanged. The result is never used, so void. */
void sub_08059050(int, s16 *, void *);

/* CORRECTION (wave 30, W30-C): this was `void sub_0806AA80(s16, s16)`, inferred
 * from sub_08044968 passing two already-narrow gUnknown_08499594 bytes with no
 * narrowing in between -- which is byte-neutral at that call site and therefore
 * settles nothing. The BODY is decisive the other way: sub_0806AA80 opens with a
 * bare `adds r4, r0, #0` / `adds r5, r1, #0` and goes straight to `lsls r4, r4,
 * #4`, with NO `lsls #0x10; lsrs #0x10` prologue pair. agbcc's PROMOTE_MODE
 * emits that pair for EVERY sub-word parameter regardless of signedness, so its
 * absence proves both parameters are `int`. Re-verified: sub_08044968 is still
 * byte-identical with the wider declaration (the caller's operands are already
 * narrow, so the conversion costs nothing). Whether sub_08029088 -- the twin
 * this note used to lean on -- is really (s16, s16) is now unsupported by this
 * evidence and was NOT rechecked. */
void sub_0806AA80(int, int);

/* ---- wave 30 (W30-C): the 0x0806A block ---------------------------------- */

/* Already DEFINED in src/decomp/c_08063980.c as `void sub_08063980(int vcount)`
 * and matched, but nothing declared it. Every one of its nine callers passes a
 * bare `movs` immediate (0, 0x50, ...), so `int` is what the definition says and
 * no caller contradicts it. */
void sub_08063980(int);

/* The palette-ramp step sub_0806A680 runs each frame. Its prologue overwrites
 * r0 with the gPal pool word before reading it, so it takes nothing; `pop {r0};
 * bx r0`, so void. */
void sub_0806A5B8(void);

/* sub_0806AA80's only callee. r0 is the proc Proc_Start just returned (the body
 * writes words at +0x30..+0x4c and halfwords at +0x58..+0x60 of it), and r1/r2
 * are used unnarrowed as `lsls #0xc` shift operands, so both are `int`. */
void sub_0806A6F0(ProcPtr, int, int);

/* A BG0 scroll ping-pong: each of these installs the OTHER through
 * sub_080638D0, so their addresses cross unit boundaries and both need to be
 * visible here. `sub_080638D0(int)` is what the header already carries, so the
 * install is spelled `sub_080638D0((int)sub_0806A180)`. Neither reads an
 * argument register and both end `pop {r0}; bx r0`, so `void(void)`.
 * sub_0806A1A8 installs sub_0806A158 the same way. */
void sub_0806A158(void);
void sub_0806A180(void);
/* Registered through `sub_0801F024((void *)sub_0806A534, 0x7F)` by
 * sub_0806A578, i.e. the void(void) callback shape sub_0801F024's other
 * clients carry. */
void sub_0806A534(void);

/* ---- wave 30 (W30-C): the 0x08063 block ---------------------------------- */

/* Already DEFINED and matched in src/decomp; both had their parameter struct
 * hoisted into unknown-globals.h in this wave so that sub_08063430 and
 * sub_08063BBC -- which call them with their OWN proc pointer -- can name the
 * same type instead of each declaring a private copy. Both definitions were
 * re-verified byte-exact after the move. */
void sub_08062FB8(struct Unk08062FB8 *);
void sub_08063BE0(struct Unk8063BE0 *);

/* Neither parameter is narrow: sub_08011D7C's prologue is a bare
 * `adds r4, r0, #0` / `adds r5, r1, #0` with no PROMOTE_MODE shift pair. r0 is
 * stored as a whole word into a 12-byte record and r1 with `strh` at +8, so the
 * store width does not settle r1 and `int` is the weakest model. The return IS
 * narrow -- both exit paths sign-extend (`lsls #0x10; asrs #0x10`, and -1 on the
 * full arm) -- hence `s16`. sub_0806366C discards it. sub_08011D7C is still
 * unmatched, so this declaration is a contract, not a promoted signature. */
s16 sub_08011D7C(void *, int);

/* ---- wave 30 (W30-C): the 0x08066 block ---------------------------------- */

/* An (x, y) pair. Both prologues are a bare `adds r4, r0, #0` / `adds r5, r1,
 * #0` with no PROMOTE_MODE shift pair, so both parameters are `int`; both end
 * `pop {r0}; bx r0`, so void. They are twins -- same guard on
 * gUnknown_03004008, same `& 0x1FF` / `& 0xFF` wrap, differing only in the id
 * they pass sub_0801F34C (0x43 vs 0x44) and sub_08064500's extra palette
 * write. */
void sub_08064474(int, int);
void sub_08064500(int, int);

/* Wave 36 (W36-G). Both derived from their own prologues, not from the call
 * site -- sub_08066808 calls them back to back with no argument setup between
 * the two `bl`s, which by itself would be equally consistent with
 * `sub_08064A44(sub_080654E8())`. Ruled out on the callee side: sub_08064A44's
 * first touch of r0 is `ldr r0, =gUnknown_0816E0EC`, a WRITE, so it reads no
 * argument; and sub_080654E8 opens `bl sub_080733B8` before r0 is read and ends
 * `bl sub_0806377C; pop; bx r0`, so it neither takes nor returns a value. */
void sub_080654E8(void);
void sub_08064A44(void);

/* Wave 36 (W36-G). Walks the 30 gUnknown_03001470 slots backwards (`movs r5,
 * #0x1d`, `subs r4, #0x60` per step) and calls the second parameter on every
 * slot whose unk00 equals the first, via `adds r0, r4, #0; bl _call_via_r6` --
 * so the callback takes the SLOT as its one argument. The callback type comes
 * from the only argument seen so far, sub_08065F78, whose promoted definition in
 * src/decomp/c_08065F68.c is `void sub_08065F78(void *a)`; that definition
 * outranks anything inferred here. The first parameter is `const void *`
 * because sub_0806675C passes gUnknown_08580AF0 to this and to
 * sub_0806377C(const void *) alike. */
void sub_08063A00(const void *, void (*)(void *));

/* Wave 36 (W36-G). Four sub_08063A00 callbacks that were already MATCHED and
 * promoted -- src/decomp/c_08065F68.c and c_08066200.c -- but had no declaration
 * anywhere, so every caller failed to compile. Signatures copied verbatim from
 * those definitions, which are byte-verified and outrank anything inferred from
 * a call site. (This is the "407 promoted-but-undeclared" case: absence from
 * this header is not evidence a symbol is underived.) */
void sub_08065F68(void *);
void sub_08065F78(void *);
void sub_08066200(void *);
void sub_08066210(void *);

/* Wave 36 (W36-G). Three more of the same case -- matched and promoted in
 * src/decomp/c_08063A3C.c, c_08065EF4.c and c_08065EB4.c, declared nowhere.
 * sub_08063A3C's return type is load-bearing rather than cosmetic:
 * sub_08065F88 calls it and sub_08063A30 back to back with only `ldr r1, =sym`
 * between the two `bl`s, which is the nested call
 * `sub_08063A30(sub_08063A3C(), gUnknown_08580D90)` -- r0 already holds the
 * result, so the nesting costs zero instructions and a nullary declaration would
 * have hidden it. */
struct Unk03001470 *sub_08063A3C(void);
void sub_08065EF4(void);
void sub_08065EB4(void);
void sub_08066BF4(void);
void sub_08066D30(void);

/* Wave 36 (W36-G). src/decomp/c_08066BF4.c had promoted this with a private
 * `struct Unk08066C70` tag whose members were unk47/unk48/unk49/unk4b at
 * exactly the offsets struct Unk08580934_Obj already declares -- the same
 * object under two names, and sub_08066D74 proves it by passing
 * gUnknown_08580934->unk54[unk33] straight in. The private tag is removed and
 * that file now names the shared type; the offsets are identical, so the change
 * is byte-neutral and both functions in it still match. */
void sub_08066C70(struct Unk08580934_Obj *);

/* Wave 36 (W36-G). Both nullary and void, read off their own prologues: each
 * opens `ldr r0, =gUnknown_085802A4` -- a WRITE of r0 -- so neither reads an
 * argument, and sub_08066D74 uses neither result. */
void sub_0806530C(void);
void sub_0806540C(void);

/* Wave 36 (W36-G), matched this wave. The parameter is the index into
 * gUnknown_08580934->unk54[]; sub_08066D74 passes the s8 unk33 with no
 * narrowing at entry, so `int`. */
void sub_08066B8C(int);

/* Wave 44 (W44-B). Both are already PROMOTED and matched -- src/decomp/
 * c_08066580.c and src/decomp/c_080665BC.c -- but neither had ever been
 * declared, because until now every caller of either lived in asm. The wave-44
 * brief reported block 0x08066000 as `undeclared callees: 0`; that count is
 * measured against the header, and these two are the counter-example. Types are
 * copied verbatim from the promoted definitions, which win over any inference:
 * sub_08066580's three parameters are bare `adds rN, rM, #0` copies with no
 * PROMOTE_MODE narrowing, and sub_080665BC's is the slot index it uses to
 * subscript gUnknown_08580934->unk74[]. Both end `pop {..}; pop {r0}; bx r0`,
 * so both are void. sub_080665D4 and sub_0806666C (this wave) are the first
 * C callers of either. */
void sub_08066580(int, int, int);
void sub_080665BC(int);

/* Wave 49 (W49-L). Three more already-PROMOTED functions that had never been
 * declared, for the same reason as the pair above -- until now every caller
 * lived in asm. Types are copied verbatim from the promoted definitions in
 * src/decomp/c_08065E5C.c and src/decomp/c_080665D4.c, which win over any
 * inference; sub_08066A20 and sub_08066874 are the first C callers. */
void sub_08065E5C(void);
void sub_080665D4(void);
void sub_0806666C(void);

/* Wave 36 (W36-G), all three read off their own prologues at sub_08066220's
 * call sites.
 *   sub_08065C9C takes ONE parameter and it is `int`, not `u8`: its prologue is
 * `adds r6, r0, #0` with no shift pair, and agbcc opens every sub-word parameter
 * with one (sub_080660BC below does exactly that for its first and third).
 *   sub_08066078 is nullary -- its first touch of r0 is
 * `ldr r0, =gUnknown_08580934`, a write -- which also rules out its being a
 * nested call on the sub_080660BC that immediately precedes it.
 *   sub_080660BC is (u16, int, u8): `lsls r0,#0x10; lsrs r0,#0x10` and
 * `lsls r2,#0x18; lsrs r2,#0x18`, while r1 goes straight to r7 unnarrowed. The
 * caller sign-extends the s8 gUnknown_08580934->unk32 into r1, which is what
 * passing that field to an `int` parameter looks like. */
void sub_08065C9C(int);
void sub_08066078(void);
void sub_080660BC(u16, int, u8);

/* The two halves of sub_0806630C's dispatch and the two of sub_08066B40's.
 * sub_08065F88's prologue overwrites r0 with a pool word before reading it, so
 * that one is `void(void)` on its own evidence.
 *
 * The other five are declared WITHOUT a prototype on purpose, the way
 * sub_0805C290 above is: their call sites set up no argument, which does not
 * prove there is none -- in each case r0 happens to hold the dispatch value at
 * the `bl` -- and none of their bodies has been read. An empty parameter list
 * keeps the argument-less calls legal without asserting an arity that a later
 * wave would have to unpick. Do not "tidy" these into `(void)` without reading
 * the callee. */
void sub_08065F88(void);
void sub_08066220();
/* Wave 49 (W49-L): these two are now READ, which is the condition the note
 * above sets for tidying them. Both write r0 before touching it -- the first
 * instruction after the prologue is `movs r5, #0` and the first use of r0 is
 * `adds r0, r1, #0` off a pool word -- so both are genuinely nullary, and
 * their only C caller (src/decomp/c_08066B40.c) already passes nothing. */
void sub_08066874(void);
void sub_08066A20(void);
void sub_08066D74();
void sub_08066EBC();
void sub_08066F20();

/* The dispatch chain itself, all three `void(void)`: each reads its selector
 * out of *gUnknown_08580934 and none touches an argument register before
 * writing it. Declared here because they call each other across units --
 * sub_08066B6C picks between sub_0806630C and sub_08066B40, and sub_08066FE0
 * calls sub_08066B6C. */
void sub_0806630C(void);
void sub_08066B40(void);
void sub_08066B6C(void);

/* ---- wave 30 (W30-C): the 0x08076 block ---------------------------------- */

/* NOT ordinary compiler output and NOT a `_call_via_rN` trampoline: 0x08071900
 * is four bytes of `bx pc; nop`, a THUMB-to-ARM interworking veneer in front of
 * the ARM routine that follows it, and data/asm-resident.json already carries
 * it. Eleven callers, all passing four register arguments. r0 and r1 are
 * addresses (sub_0807606C passes `gUnknown_08551A00 + 0x140` and an offset into
 * gUnknown_08499578), r2 and r3 small counts. The result is discarded
 * everywhere it is called from so far. */
void sub_08071900(void *, void *, int, int);
/* Wave 35 (W35-B). sub_08071900's sibling one entry along, called by
 * sub_08077690 and sub_08077DF0 as `sub_08071918(buf + 0x80, 0x1e, 7, 0)` --
 * the same (buffer, width, height) triple with a fourth constant instead of
 * sub_08071900's source pointer. Declared from the call sites only.
 *
 * NOT MATCHABLE FROM C, and it is in data/parked.json: its `thumb_func_start`
 * label sits 8 bytes early and swallows the trailing alignment of an ARM/THUMB
 * interworking veneer table, so the ROM carries four leading `movs r0, r0` that
 * no C emits. Declare it and call it; do not try to decompile it. */
void sub_08071918(void *, int, int, int);
/* Wave 35 (W35-B), all three declared from call sites in the 0x08077 block.
 * sub_080733A0 and sub_08074EEC are ALREADY PROMOTED (src/decomp/c_080733A0.c,
 * src/decomp/c_08074EEC.c) and were simply never declared here; these two
 * signatures are copied from those definitions, not inferred, because a
 * promoted definition outranks a call-site guess. sub_080752D8 is not promoted
 * -- sub_08077870 calls it `sub_080752D8(0)` and sub_0807703C `sub_080752D8(2)`,
 * both dropping the result. */
void sub_080733A0(int);
void sub_08074EEC(int);
void sub_080752D8(int);
/* Wave 35 (W35-B). Also already promoted (src/decomp/c_080763B0.c) and never
 * declared -- sub_0807703C is its only caller in the whole ROM, which is why
 * nothing had needed it. Signature copied from the definition. */
void sub_080763B0(void);
/* Wave 35 (W35-B). sub_0807703C's two remaining callees in the 0x08076 block.
 * sub_08076E20 takes the raw gpKeySt->unk00 key mask and eases the camera one
 * frame; `int` and not `u16` because its own body opens `lsls #0x10; lsrs #0x14`
 * -- a u16 parameter would have narrowed to 16 in the prologue and then shifted
 * by 4, which is a different pair. sub_08076F34 receives the caller's proc in
 * r0 unchanged and drops its result. */
void sub_08076E20(int);
void sub_08076F34(ProcPtr);

/* Returns 1, 0 or -1 with `bx lr` and reads no argument register. The return is
 * `int`, not a narrow type: sub_0807610C consumes it with a bare
 * `adds r1, r0, #0` and no re-narrowing, and agbcc re-narrows a
 * narrow-returning callee's result at every call site. */
int sub_08075EC4(void);

/* Takes the proc sub_0807610C is running -- it reads and writes the word at
 * +0x3c of it as a change-detect cache. `void *` is a placeholder for that
 * struct, which belongs to whichever unit defines the proc; the parameter is
 * definitely a pointer and definitely one argument. */
void sub_08075F44(void *);

/* Wave 34, W34-G. Already DEFINED and matched in src/decomp/c_0807606C.c, but
 * nothing declared it -- until this wave its only caller (sub_0807614C) was
 * still in asm/. Declared with an INCOMPLETE tag rather than the `void *` used
 * for sub_08075F44 just above, because that definition already names a struct
 * type and a promoted definition wins over a fresh declaration: `void *` here
 * would make c_0807606C.c itself stop compiling with `conflicting types`.
 * The tag is completed by that file; callers in other units pass their own
 * proc struct through a cast, which is byte-neutral. */
struct Unk807606C;
void sub_0807606C(struct Unk807606C *);

/* Wave 34, W34-G. Two arguments, read off sub_08075DBC's call site, which is
 * the only one in C so far: `movs r0, #0xf` and `movs r1, #1; rsbs r1, r1, #0`
 * for -1. The second being built by negation rather than taken from a pool word
 * says only that it is a small signed value; both are modelled `int` as the
 * weakest fit. The body is still in asm/, so nothing here is confirmed from the
 * callee side. */
void sub_08075A54(int, int);

/* Helpers of the 0x08076 screen setup. All of them IGNORE r0 -- sub_08076888
 * overwrites it with a pool word in its first two instructions, and
 * sub_08076858 / sub_0807681C / sub_08076B20 never read it.
 *
 * WAVE 34 (W34-J) RETYPED sub_08076888 FROM `void(void)` TO ONE PARAMETER.
 * The old prototype was the "wrong prototype every existing caller happens to
 * agree with" case in the brief: its only two C callers, sub_08076ADC and
 * sub_08076B7C, both call it while their own proc pointer is already sitting in
 * r0, so forwarding it costs zero instructions and `void(void)` was invisible.
 * sub_08076A68 is the differently-shaped caller that settles it -- it calls
 * sub_08076888 in the MIDDLE of its body, after two other calls have clobbered
 * r0, and the ROM spends an `adds r0, r5, #0` to put the proc back. A
 * `void(void)` prototype cannot emit that instruction at all.
 * That the callee then discards the argument is a fact about the callee, not
 * about the calls; sub_08076858 / sub_0807681C / sub_08076B20 keep `void(void)`
 * because no caller of theirs has ever paid an instruction to set r0.
 * sub_08076B7C gained the same parameter and forwards it (byte-neutral: it has
 * no declaration here and no C caller). */
void sub_08076888(ProcPtr);
void sub_08076858(void);
void sub_0807681C(void);
void sub_08076B20(void);

/* Fills a caller-supplied 12-byte record from a table index: sub_08075E68
 * passes its own s32 first argument and `mov r1, sp`, then reads back an s16 at
 * +0x02, an s16 at +0x04 and a word at +0x08. +0x00 is written by the callee
 * but never read by that caller, so the record's own layout is only partly
 * known -- the parameter is `void *` here for that reason.
 * TYPED in wave 38 (W38-B): the record is `struct Unk0202FE38` -- the function
 * is the lookup half of the same 12-byte list sub_08074754 appends to, and it
 * copies +0x00/+0x02/+0x04/+0x08 out of the matching entry field by field. It
 * also RETURNS: 1 on a hit (after compacting the array over the removed
 * entry) and 0 when the scan runs off the end of the 16 slots, which the old
 * `void` could not express. */
int sub_08074834(s32, struct Unk0202FE38 *);

/* Returns a u8 -- sub_08075BF4 re-narrows the result with `lsls #0x18; lsrs
 * #0x18` before storing it, which is agbcc re-narrowing a narrow-returning
 * callee at the call site. The third parameter is an OUT pointer to a single
 * byte: sub_08075BF4 hands it the address of its own s16 +0x64 and then reads
 * that field back with `ldrb` before widening it to a halfword. */
u8 sub_080759A0(int, int, u8 *);

/* The two axes of the camera clamp, driven together by sub_08076E20. Each
 * takes a SIGNED halfword delta -- both begin `lsls r0, r0, #0x10` and their
 * caller passes a halved s16 with no narrowing of its own, which is
 * PROMOTE_MODE doing the work at the callee. Each returns 1 if it actually
 * moved its pair and 0 otherwise, and sub_08076E20 SUMS the two results, so the
 * return is a plain int rather than a bool-shaped narrow value. */
int sub_08076CAC(s16);
int sub_08076D68(s16);

/* Starts gUnknown_086143B8 under `parent`. WAVE 34 (W34-G) SETTLED THE FIRST
 * TWO AS s16, retyping them from `int` -- the note here previously said the
 * width was undecided and that `int` was the weakest model, and the body
 * decides it. `int` parameters put the subtract in SImode ahead of the
 * narrowing (`subs #16; lsls #0x10; lsrs #0x10`, and the constant needs no pool
 * word at all); only a 16-bit parameter lets combine move the subtract INTO the
 * shifted domain, which is what produces the ROM's `lsls #0x10; adds
 * <0xFFF00000>; lsrs #0x10` and what makes the two parameters SHARE that one
 * pool word. s16 rather than u16 because u16 additionally hoists both `lsls`
 * to the top of the function, ahead of the pool load. Byte-neutral at
 * sub_08076C8C, the only promoted caller, which passes literal zeros; the
 * remaining caller sub_08077CAC is still in asm/.
 * The third is a plain word (`adds r6, r2, #0`, compared against 2 and stored
 * with `str`), the fourth goes straight to Proc_Start as the parent. */
void sub_0807548C(s16, s16, int, ProcPtr);

/* Wave 34, W34-G. sub_0807548C's re-entry twin on the same proc and the same
 * three leading parameter types -- it Proc_Finds gUnknown_086143B8 rather than
 * starting one, so there is no parent and the arity stops at three. */
void sub_0807553C(s16, s16, int);

/* Wave 34, W34-G. Starts gUnknown_08614370 under `parent` and RETURNS the new
 * proc -- sub_0807519C, its only caller, stores the result into the ten-entry
 * pointer table at +0x3c of its own proc, so the return is a pointer and not
 * the dropped s32 of the usual starter. The widths are read off that CALL SITE,
 * not off this body: agbcc's PROMOTE_MODE zero-extends every sub-word parameter
 * on entry, so the body's four uniform `lsls #0x10; lsrs #0x10` pairs say
 * nothing about signedness. At the call sub_0807519C narrows argument 1 with
 * `lsls #0x10; lsrs #0x10` (unsigned) and arguments 2 and 3 with `lsls #0x10;
 * asrs #0x10` (signed), and passes argument 4 straight out of an `ldrh` with no
 * extension at all -- hence u16, s16, s16, u16. */
void *sub_08075058(ProcPtr parent, u16 a2, s16 a3, s16 a4, u16 a5);

/* Already DEFINED and matched in src/decomp with exactly these signatures, but
 * nothing declared them -- until this wave every caller was still in asm/, so
 * the omission was invisible. sub_08076ADC, sub_08076BF0, sub_08076C1C and
 * sub_08076C8C are the first promoted callers. */
void sub_0807639C(ProcPtr);
void sub_08074ED0(void *, ProcPtr);
void sub_08078480(void *, ProcPtr);
void sub_08078540(void *, ProcPtr);

/* Two starters run back to back on the caller's own proc as parent.
 * sub_08076ADC calls it across a unit boundary. */
void sub_08076C8C(ProcPtr);

/* ---- wave 30 (W30-C): the 0x0806B extension block ------------------------ */

/* sub_080137AC's neighbour and evident twin -- sub_0806B09C calls
 * `sub_080137AC(-1)` and sub_0806B0E0 calls `sub_08013830(1)`, the two of them
 * bracketing the same proc's fade. The argument is a bare `movs r0, #1`, which
 * settles nothing about the width, so `int` is the weakest model;
 * sub_080137AC next to it carries `s32` on no stronger evidence.
 * WAVE 40 (W40-B): narrowed to `s8`. The body needs it -- as `int` the prologue
 * truncation is absent and the function is 4 bytes short -- and the constant
 * arguments make it free at both call sites; see sub_080137AC for the probe. */
void sub_08013830(s8);

/* ---- wave 30 (W30-C): the 0x08065 extension block ------------------------ */

/* The cursor-step half of sub_08065EB4's dispatch. All three parameter widths
 * are read off the prologue: `adds r6, r0, #0` (a bare copy, so `int`),
 * `lsls r1, #0x10; lsrs r1, #0x10` (`u16` -- it is a key bitmask, tested
 * against 0x40 and 0x80), and `lsls r2, #0x18; lsrs r2, #0x18` (`u8`, used only
 * as a truth test to gate a sound effect). */
void sub_08065DAC(int, u16, u8);

/* The other half. Declared WITHOUT a prototype for the sub_0805C290 reason:
 * sub_08065EB4 reaches it on the arm where r0 happens to hold the `& 1` result,
 * so the call site cannot show whether that is an argument, and the body was
 * not read. */
void sub_08065D20();

/* Wave 30, W30-E: NOW DECLARED, copied verbatim from the definition in
 * src/decomp/c_08064BC8.c, and sub_0806502C is matched with an explicit
 * `(struct Unk08580934_Obj *)` on sub_080152EC's result. A pointer cast emits
 * no instruction, so the cast is a statement about the type model and costs
 * the match nothing; leaving the function unmatched bought nothing either.
 * The type conflict W30-C recorded below is REAL and still open -- the cast is
 * the marker for it, not a resolution of it. */
void sub_08064BC8(struct Unk08580934_Obj *, int, int, int);

/* W30-C's original note, kept because the evidence in it is what a future
 * unification pass needs.
 * sub_0806502C is `sub_08064BC8(sub_080152EC(gUnknown_08580A38, 3),
 * obj->unk28 + 8, obj->unk1c, obj->unk1c * 2)` -- but sub_080152EC returns
 * `struct Unk03001470 *` while src/decomp/c_08064BC8.c defines its first
 * parameter as `struct Unk08580934_Obj *`, so the call cannot be written
 * without a cast under -Werror.
 *
 * That is evidence, not an obstacle: it is the SECOND independent sign this
 * wave that the two structs describe one object. sub_08066580 stores another
 * sub_080152EC slot into gUnknown_08580934->unk74[], whose only reader touches
 * +0x08 -- a `u32` at the same offset in both models -- and the fields
 * sub_0806502C reads here line up too (unk1c at +0x1c in both, unk28 at +0x28
 * in both, with Unk08580934_Obj's unk2a sitting in the high half of
 * Unk03001470's u32 unk28). Unifying them is a multi-file retype that touches
 * five already-promoted writers of that u32 and wants a union for the
 * word/halfword duality; it was deliberately NOT started mid-wave.
 * sub_0806502C is left unmatched rather than papered over with a cast. */

/* --- the 0x0803B block (wave 27, W27-C) ---------------------------------- */

/* Already DEFINED in src/decomp with these exact signatures; they simply had
 * no declaration because nothing outside their own file called them until
 * this block. Copied from the definitions, not invented:
 *   src/decomp/c_08016E74.c, c_08017688.c, c_08034334.c, c_08038690.c,
 *   c_0803B83C.c, c_0803B8C4.c, c_080846F4.c. */
void sub_08016E74(void);
void sub_08017688(u16);
void sub_08034334(void);
void sub_08034338(void);
void sub_08038690(int);
void sub_0803B83C(void);
void sub_0803B8C4(void);
u8 sub_080846F4(void);

/* sub_0803BA1C takes nothing and returns nothing anybody reads: sub_0803BA4C
 * calls it first with no argument register set up and discards r0. */
void sub_0803BA1C(void);

/* Three function addresses, in r0/r1/r2, all pool words -- sub_0803B874 is
 * nothing but the call. Declared WITHOUT a prototype: the three are
 * sub_08034334, sub_08034338 and sub_0803B83C, all `void (void)`, but that is
 * what this ONE call site passes and not evidence about the parameter types. */
void sub_08012FB8();

/* --- the sub_08036B34 / AgbMain unit (wave 27, W27-C) --------------------- */

/* sub_08036B48 IS A FUNCTION and the index does not know it. The index gives
 * sub_08036B34 a size of 24, but its body plus its one pool word only reach
 * 0x08036B48; the remaining four bytes are `b .` and two of alignment padding.
 * AgbMain reaches them with `bl _08036B48` -- a CALL, not a branch -- so this
 * is a separate two-byte `for (;;) ;` function that got folded into its
 * neighbour's extent because it has no symbol. It is defined in the unit's own
 * source, immediately after sub_08036B34, which is what puts it at that
 * address.
 *
 * It is `static` there and so is NOT declared here. That is not a style
 * choice: the ROM's `bl` into it carries NO relocation, which only happens
 * when the assembler can resolve the target itself, which only happens for a
 * LOCAL symbol. Declared globally the branch keeps its relocation and AgbMain
 * misses by exactly those two bytes. Verified with
 * `tools/trymatch.py AgbMain --unit`. */

/* Plain `void (void)` leaves of the boot/reset unit; each is called with no
 * argument set up and its result discarded. */
void sub_0801F018(void);
void sub_08036A50(void);
void sub_08036AB8(void);
void sub_08036B28(void);
void sub_08036B34(void);
void sub_08036C08(void);
void sub_08036C4C(void);
void sub_08036E18(void);
void sub_08036E54(void);
void sub_0801BABC(void);
void sub_080128C4(void);
void sub_0801B6BC(void);
void sub_0803486C(void);
void sub_08034848(void);
void sub_0801BCE0(void);
void sub_08015544(void);
void sub_08011C18(void);
void sub_08011A84(void);
void sub_080191B0(void);
void sub_08015184(void);
void sub_08010F94(void);
void sub_08013434(void);
void sub_0801F4A4(void);
void sub_0801295C(void);
void sub_0803B688(void);

/* sub_08080F90 takes one argument; AgbMain's chain passes the literal 0. */
void sub_08080F90(int);

/* Returns a value AgbMain compares against -1, so at least `int` wide and
 * SIGNED at the comparison. Declared WITHOUT a prototype because the two
 * arguments AgbMain passes (a buffer address and 0x8000) do not settle their
 * declared types and a wrong prototype would be a claim, not a contract. */
int sub_08014DA8();

/* Declared without prototypes for the same reason -- the argument shapes are
 * read off AgbMain's call sites only:
 *   sub_0801A79C  five arguments, the fifth on the stack (two function
 *                 addresses, a buffer, the literal 2, a second buffer).
 *   sub_080129D4  one wide argument (the literal 0x0A6B99CD).
 *   sub_0801BB00  (index, handler) -- see gUnknown_030040D0's note above.
 *   sub_08016B2C / sub_08016A54  taken only as addresses, never called here. */
void sub_0801A79C();
void sub_080129D4();
/* WAVE 37: `void sub_0801BB00();` was declared HERE as well as at its real
 * prototype below (`void sub_0801BB00(int, void *);`). Both are legal C89 and
 * compile -- int and void * are unaffected by default argument promotions, so
 * the two declarations are compatible and proto_check.py correctly passed them.
 * The empty-parens form was removed anyway because it gives callers NO ARITY
 * CHECKING, and missing arity checking is what let four `too few arguments`
 * callee-retype breaks through in waves 35-36. The two-parameter prototype below
 * is the one to use; it agrees with the promoted definition in
 * src/decomp/c_0801BB00.c. */
/* sub_08016B2C IS called after all -- sub_08016E14 (wave 29, W29-B) calls it
 * and feeds the result to sub_0801A7D8 as a byte count, so it is `int` and not
 * `void`; its one argument is the save-block base it copies into
 * (`adds r7, r6, #0x48` and word `ldm`/`stm` runs off it), and it returns the
 * constant 0x5CC. sub_08036B34 only takes its ADDRESS, and sub_0801A79C is
 * declared without a prototype, so the retype costs that call site nothing. */
int sub_08016B2C(void *);
void sub_08016A54();
void sub_080366F4(void);

/* The BIOS-style reset entry at 0x0808AAD4, already named in the assembly.
 * sub_08036CB4 passes 0xFE, which is a RegisterRamReset-shaped flag word, so
 * it takes an argument; the width is not settled beyond "fits in r0". */
void SoftReset(int);

/* ---- wave 27 (W27-A) ---- */

/* The BIOS block-copy SWI. Undeclared until this wave even though the
 * sub_08011C58 note far above was already written against it -- nothing that
 * CALLS it had been promoted. Signature copied from CpuFastSet, which the same
 * BIOS group declares, rather than re-derived. */
void CpuSet(const void *, void *, u32);

/* sub_08071AF0 / sub_08071B0C's shared worker, read off its own body:
 *     sub_08071B28(pal, index, b, parent)
 * Proc_Starts gUnknown_08613E54 under `parent`, takes the 0x30-byte
 * gUnknown_0202F2DC record at `index`, CpuSets 0x10 halfwords of
 * &gPal[index * 16] INTO that record -- a backup, not a load -- then stores
 * &gPal[index * 16] at the record's +0x24 and `pal`, its own FIRST argument, at
 * +0x20.
 *
 * THIS REFUTES the wave-20 note on sub_08071B0C above, which read that first
 * argument as the proc script. The script is gUnknown_08613E54 and it is
 * hard-coded in sub_08071B28's own literal pool; the first argument is ROM
 * PALETTE DATA. The confirming reader is wave 27's sub_08071C84 /
 * sub_08071CA4, which CpuSet the very same two symbols the other way -- out of
 * ROM and into &gPal[a * 16] -- which no proc script would survive. The two
 * front-ends' `(int, int, ProcPtr)` signatures are untouched by this and stay
 * as they are: neither of them ever sees the palette, they only load its
 * address into r0 on the way past.
 *
 * Returns the record (`adds r0, r4, #0` at the end). Both front-ends discard it
 * with `pop {r0}`, so nothing here pins the pointee and `void *` is the
 * weakest type that fits. */
void *sub_08071B28(const void *pal, int index, int b, ProcPtr parent);

/* The setter gUnknown_03002FA0's note in unknown-globals.h already names:
 * sub_0801BB00(slot, handler). The second parameter is an opaque `void *` on
 * the same evidence as sub_08011AAC's -- sub_08012A54 hands it that function's
 * own untyped pass-through parameter and does nothing else with it, so no data
 * type could describe it. */
void sub_0801BB00(int, void *);

/* All `void (void)`, all called as bare statements with no argument register
 * read. sub_08010FA0, sub_08012A24, sub_080122EC and sub_08013324 are the
 * display-shadow resets promoted in src/decomp; sub_0803DDF4 is still asm and
 * its prologue reads no parameter. */
void sub_08010FA0(void);
void sub_08012A24(void);
/* Types copied from the definition in src/decomp/c_0801224C.c, not re-derived.
 * sub_0801220C is its first cross-file caller and passes two literals, which is
 * byte-identical at any width and so adds no evidence either way. */
void sub_0801224C(u16, u16);
void sub_080122EC(void);
void sub_08013324(void);
void sub_0803DDF4(void);

/* The two window/blend openers promoted in src/decomp/c_08071CF4.c and
 * c_08071DB4.c. Each takes the proc it seeds, and wave 27's sub_08071E80 /
 * sub_08071EB8 are the first cross-file callers either has ever had -- they
 * forward their own r0 untouched. The tags are FORWARD-DECLARED and left
 * incomplete on purpose: the two definitions complete them privately in their
 * own translation units, and a pass-through caller never needs the layout. */
struct Unk08071CF4;
struct Unk08071DB4;
void sub_08071CF4(struct Unk08071CF4 *);
void sub_08071DB4(struct Unk08071DB4 *);

/* ---- wave 28 (W28-A): the 0x08018000 gUnknown_0200C528 script block ---- */

/* Types COPIED FROM the promoted definitions in src/decomp, not re-derived --
 * these five had no declaration because no promoted unit had yet called them
 * across a file boundary, and this block is the first to do so. */
void sub_08012A54(void *);   /* c_08012A54.c */
void sub_0802DCA4(void);     /* c_0802DCA4.c */
bool8 sub_0802C550(void);    /* c_0802C550.c */
void sub_0803B3E0(void);     /* c_0803B3C8.c */
/* RETYPED from the promoted `s32` to `bool8` -- see the note on the definition
 * in src/decomp/c_08078198.c. Its three callers all live in this block and all
 * three truth-test the result with a bare `lsls #0x18`. */
bool8 sub_08078198(void);    /* c_08078198.c */
void sub_08043418(int, int, int); /* c_08043418.c */

/* CORRECTION, wave 28 (W28-A): src/decomp/c_08018BAC.c declared this `bool8`.
 * The body cannot tell -- it is `movs r0, #1` at every width -- and its three
 * callers say otherwise. sub_08018BCC, sub_08018F34 and sub_08018F74 each end
 * `bl sub_08018BAC; lsls r0, #0x10; asrs r0, #0x10`. A `bool8` (QImode) result
 * converted to anything is `lsls #0x18; lsrs #0x18`, and a `s16` result in an
 * `int` context needs no conversion at all; only an INT result converted to the
 * caller's own `s16` return type produces the pair the ROM has. The definition
 * was retyped to `int` and re-verified with trymatch (still byte-identical). */
int sub_08018BAC(s16);

/* The 0x08018000 block's own helpers, none of which had a prototype.
 *
 * sub_08018018 takes `u8`: both callers (sub_080180A8, sub_080180CC) hand it
 * the s16 slot member unk0e through `lsls #0x18; lsrs #0x18`, which is the
 * narrowing a u8 parameter forces and an `int` parameter would not.
 * sub_0801815C likewise -- sub_080180CC reaches gUnknown_03002F08.unk02, a u16,
 * with `ldrb`, which is only a u8-context read.
 *
 * sub_08018254 takes `s16`: sub_08018464 holds its own s16 parameter in r4 and
 * re-derives `lsls #0x10; asrs #0x10` immediately before the `bl`.
 *
 * sub_08014824 returns at least `int`: sub_080185BC truth-tests the result with
 * a BARE `cmp r0, #0` and no narrowing, which rules out every sub-word return.
 *
 * sub_0801A548 mirrors sub_0801A57C above -- same caller shape, same u16 global
 * (gUnknown_030033EC) passed with a plain `ldrh`. All four returns are unused
 * at every known call site, so `void` is a floor, not a measurement. */
void sub_08018018(u8);
void sub_0801815C(u8);
void sub_08018254(s16);
int sub_08014824(void);
void sub_0801A548(u16);

/* ---- wave 28 (W28-A extension): the 0x08035000 block ---- */

/* Copied from the promoted definitions in src/decomp, not re-derived. */
ProcPtr sub_080355CC(u16, u16, u16, u16); /* c_080355CC.c */
int sub_08042DE0(int);                    /* c_08042DE0.c */
/* Wave 34, W34-F. sub_08041DF8 does `bl sub_080413A4; ldrb r2,[r0]` and passes
 * that byte to sub_08034534's u8 third parameter, so the return is a pointer
 * whose first byte is a u8 -- `u8 *` is the weakest model that fits. Its one
 * argument is gUnknown_03003F40, declared `int` above. */
/* Wave 34 integration: returns struct Unk03003338 *, per the promoted
 * definition in src/decomp/c_080413A4.c. The `ldrb r2,[r0]` at sub_08041DF8's
 * call site fits both this and a bare u8 *, so weakest-model does not apply --
 * a promoted definition already names the type. Callers cast; a pointer cast
 * emits nothing. */
struct Unk03003338 *sub_080413A4(int);
/* Wave 34, W34-F. sub_080411FC hands it two bytes off its own proc (+0x44 and
 * +0x4a), both plain `ldrb` with no narrowing at the call site, so `int` is the
 * weakest model. void: the result is never read. */
void sub_08041258(int, int);
/* Wave 34, W34-F. sub_0804103C passes the packed u8 selector and uses the
 * result as `(r - 1) * 0x400`, a byte offset into gUnknown_081218BC, so int. */
int sub_08024984(int);
/* Wave 34, W34-F. A map-cell predicate: (x, y) are compared against the
 * gUnknown_08499590 header's u16 extents SIGNED and against zero, so s16, and
 * the third argument selects the gUnknown_085D5ABC record whose unk19 picks the
 * terrain-cost row. Returns u8 -- its only caller sub_08041F38 re-narrows the
 * result `lsls #0x18; lsrs #0x18` before comparing it to 1. */
u8 sub_08041EA8(s16, s16, int);
/* Wave 34 (W34-F matched the body, W34-I declared it). Probes the four cells
 * around (x, y) with sub_08041EA8 and returns a 4-bit direction mask, so u8 --
 * every caller in block 0x08042 re-narrows the result `lsls #0x18` before
 * testing it. The first two arguments are WIDE: sub_08041F38 does `x - 1` and
 * `x + 1` on them before handing them to sub_08041EA8's s16 parameters, and no
 * re-narrowing appears between the arithmetic and the call. */
u8 sub_08041F38(int, int, int);
/* Wave 34, W34-F. The tail sub_08041820 falls into when sub_0803DF54 finds no
 * entry; same proc start, but it clears the cell plane itself and stores the
 * two coordinates into the new proc. Third argument is used as a u8. */
void sub_0804189C(int, int, int);
/* The tag is FORWARD-DECLARED and left incomplete on purpose, the sub_08071CF4
 * precedent: src/decomp/c_08035828.c completes it privately, and sub_08035810 --
 * its first cross-file caller -- only forwards a `Proc_Find` result. */
struct Unk35828Proc;
void sub_08035828(struct Unk35828Proc *);

/* The block's own helpers.
 *
 * sub_08035124 takes `u8`: sub_080351F0 reaches gUnknown_03003FC0.unk2e with a
 * plain `ldrb` and the callee's prologue is `lsls #0x18; lsrs #0x18` operating
 * on r0 IN PLACE before any global is touched, which is PROMOTE_MODE and not a
 * cast at a use.
 *
 * sub_0803F5E4's parameters arrive as a bare `adds r0, r4, #0` and a
 * `movs r1, #0x48` with no narrowing between them, so `int` is the weakest
 * reading of both -- and the first is handed the already-narrowed `u16`
 * parameter of sub_08035020, which converts silently either way.
 *
 * sub_08035760's second parameter is what settles its arity at two:
 * sub_080357E0 loads it from [sp, #8], its own FIFTH argument, immediately
 * before the `bl`. sub_08035740 forwards its own r0 there the same way.
 *
 * sub_08071488's third parameter is `s16`: both sub_08035E24 call sites end
 * `rsbs r2, r2, #0; lsls r2, #0x10; asrs r2, #0x10`, and sub_08035E6C passes a
 * bare 0, which settles nothing. The three void returns are floors -- every
 * call site discards the result. */
void sub_080350E4(void);
void sub_08035124(u8);
void sub_080352B4(void);
void sub_080353E8(void);
void sub_0803F5E4(int, int);
/* Wave 44, W44-E: copied verbatim from the promoted definition in
 * src/decomp/c_0803F5C8.c, which had no prototype anywhere; the definition
 * wins. sub_0805C988 and sub_0805C9CC both walk the returned record with
 * `adds r2, #8` and read unk00/unk01/unk02, which is struct Unk02028360's
 * stride and layout -- independent corroboration of the return type. */
struct Unk02028360 *sub_0803F5C8(int);
/* Wave 49, W49-C, both read off sub_0803FC28's call sites (neither is promoted
 * yet). sub_0803F908 takes five whole words, the fifth on the stack, and the
 * third is one of seven ROM blobs in the 0x0849FAxx run -- see the note on those
 * in include/unknown-globals.h for why `const u8 *` is the weakest model that
 * fits. sub_08027198's result reaches sub_0803F908's fourth argument through a
 * bare `adds r3, r0, #0` with nothing re-narrowing it, and the same argument is
 * a plain -1 at the case-2 site, so the parameter and the return are both
 * `int`. */
void sub_0803F908(int, int, const u8 *, int, int);
int sub_08027198(int);
void sub_0803FC28(int, int, int, int);
/* Wave 33, W33-B: matched. Two int parameters; the first is a dead parameter
 * (r0 is clobbered before use) and the second is the VRAM tile index. void. */
void sub_0803FD80(int, int);
/* Wave 33, W33-B: the sub_0803F110/sub_0803F128 accessor pair and the
 * sub_0803F27C/sub_0803F29C helpers, called by sub_0803F140 and its siblings.
 * sub_0803F110 returns a Decompress source (`u8 *`); sub_0803F128 returns a
 * sprite descriptor handed to sub_0801C70C's `const void *` (`const u16 *`,
 * matching c_0803F128.c); sub_0803F27C returns an int layer value;
 * sub_0803F29C writes two int out-parameters (read back with word `ldr`). */
u8 *sub_0803F110(int);
const u16 *sub_0803F128(int);
int sub_0803F27C(int);
void sub_0803F29C(int *, int *, int);
void sub_08035760(ProcPtr, void *);
/* Wave 43, W43-B. sub_08035850 spawns the gUnknown_0849BDB8 proc on
 * PROC_TREE_5 and seeds it. All three parameters are `int`: each reaches its
 * use unnarrowed -- `lsls #4` straight off the incoming register for the first
 * two, a bare `strb` for the third -- and an s16 parameter would have needed a
 * sign-extension in front of the shift. `pop {r0}` makes it void.
 *
 * sub_08035BC4's three parameters are s16, and that is PROMOTE_MODE read
 * forwards: each is zero-extended once at entry and sign-extended again at
 * every arithmetic use, while the gUnknown_03001470 unk1e/unk20 stores use the
 * RAW zero-extended copies -- which is exactly `strh` of an s16 parameter.
 * Its own fan-in is 0, so nothing constrains this from the caller side.
 *
 * The last three are the "PROMOTED BUT NEVER DECLARED" trap: all are already
 * matched, in src/decomp/c_0801BD00.c and src/decomp/c_08042E2C.c. The types
 * below are COPIED FROM THOSE DEFINITIONS and were not re-derived. */
void sub_08035850(int, int, int);
void sub_08035BC4(s16, s16, s16);
void sub_0801BDB4(s32, s32, u16 *, s32);
int sub_08042F5C(int);
int sub_08042FA4(int);
void sub_08035DF4(void *);
void sub_08035E90(ProcPtr);
/* Wave 38 (W38-A): first parameter retyped from `void *` to
 * `struct MusicPlayerInfo *` -- see sub_08071420. */
void sub_08071488(struct MusicPlayerInfo *, u16, s16);

/* ---- wave 28 (W28-A extension): the 0x08036000 block ---- */

/* Copied from the promoted definitions in src/decomp; the 0x08036000 block is
 * the first cross-file caller each of them has had.
 * sub_08036CB4 lives in the AgbMain unit (c_08036B34.c) but is an ordinary
 * global, so the relocation is real -- it is NOT the `static` sub_08036B48
 * hazard that unit carries. */
bool8 sub_0802759C(void);    /* c_0802759C.c */
void sub_08036CB4(void);     /* c_08036B34.c */
void sub_0804A010(void);     /* c_0804A010.c */
/* The heap free's forwarder. Its own definition's comment already records the
 * evidence for `void *`: sub_080363D0 does `ldr r0, [r4, #0x48]` immediately
 * before the `bl`, and that is now a real cross-file call rather than a note. */
void sub_080364D4(void *);   /* c_080364D4.c */

/* RETYPED in wave 36 (W36-F) from `void (void)`. The old note reasoned from
 * sub_0803647C setting up no argument, but "no setup" is exactly what a
 * forwarded first parameter looks like: sub_0803647C opens `adds r4, r0, #0`
 * and r0 still holds its own proc at the `bl`, so the call costs zero
 * instructions either way. The BODY settles it -- sub_08036024 opens
 * `adds r3, r0, #0` and dereferences r3 at +0x36 as the proc's u8 state, then
 * hands the same pointer to sub_08035E90(ProcPtr). src/decomp/c_0803647C.c was
 * updated to pass `proc` and re-verified byte-for-byte with trymatch. */
void sub_08036024(ProcPtr);
/* Wave 45, W45-B: this one was DEFINED in src/decomp/c_080364D4.c since wave 30
 * but never declared here, so sub_080364F4 -- its first C caller -- would not
 * compile. Signature copied from that definition, not re-derived. */
void sub_080364E0(void);
/* Wave 45, W45-B: four more that are DEFINED in src/decomp but were never
 * declared here, so sub_080360D0 -- their first C caller -- would not compile.
 * Every signature is copied verbatim from the promoted definition
 * (src/decomp/c_0802723C.c, c_080360A4.c, c_0803647C.c, c_0805C974.c), not
 * re-derived from a call site.
 *   sub_0805C974's `int` return is the definition's, and it is why its callers
 * have to narrow: sub_080360D0 tests it `lsls #0x18; cmp #0`, i.e. `(u8)`. */
void sub_08027278(int, int);
void sub_080360A4(ProcPtr);
void sub_0803647C(ProcPtr);
int sub_0805C974(void);

/* ---- callees of the 0x0801B000 block (wave 28) ------------------------ */

/* sub_0808AE54 takes FOUR arguments, and that is MEASURED, not guessed.
 * sub_0801B66C forwards only two of them and yet still spends
 * `push {r4, lr}; adds r4, r1, #0` parking its second parameter in a
 * callee-saved register before narrowing its first into r1. The same body
 * probed against 2-, 3- and 4-argument declarations reproduces that prologue
 * ONLY at four: with two or three arguments r2 (and r3) are free, the narrowed
 * value lands there, and nothing is saved.
 *
 * This is the direct-call analogue of the `_call_via_rN` arity tell -- a
 * pass-through argument costs no instruction, but it does occupy a register,
 * and when it occupies the last free scratch the pressure is visible in the
 * prologue. It only reads out when the wrapper has something else that must
 * live across a call or a clobber, so it is not a general method; here the
 * u16 narrowing of the first parameter supplies exactly that.
 *
 * The return is unused at the only known call site, so `void` is a floor. */
void sub_0808AE54(u16, int, int, int);

/* sub_0808AC44 takes TWO arguments, by the same register-pressure readout that
 * measures sub_0808AE54 above -- and this one was caught by a failed match
 * rather than predicted, which is what makes it worth writing down.
 *
 * sub_0801B598 sets up r0 only, so the argument count looks like one. But the
 * ROM narrows its u8 parameter into r2, skipping r1, and a one-argument
 * declaration puts it in r1 and misses by exactly those 2 bytes. r1 is reserved
 * because a second, forwarded parameter is riding it. Three arguments would
 * have pushed the narrowed value to r3, so two is exact, not a floor.
 *
 * The widths at both ends are independent facts: `lsls #0x18; lsrs #0x18` at
 * entry is the u8 first parameter, and the `lsls #0x10; lsrs #0x10` after the
 * call is agbcc re-narrowing a u16-returning callee. The u8 agrees with the
 * note on gUnknown_03000F70 in unknown-globals.h, which has sub_0808AC44
 * rejecting an id above 3. Nothing types the second parameter, so `int`. */
/* WAVE 29 (C) types the second parameter. It is not `int`: sub_0808AC44's own
 * body does `ldr r0, =sub_0808AC20; str r0, [r2]`, i.e. it publishes the timer
 * IRQ handler THROUGH the pointer, so the parameter is where the caller wants
 * the installed callback written back. sub_0808AC20 is `void (void)`.
 * The arity readout above is untouched.
 *
 * CORRECTED at wave-29 integration: the note here originally said "the only
 * caller, sub_0801B598, is not promoted, so nothing has to change with it".
 * sub_0801B598 IS promoted (src/decomp/c_0801B598.c, its own single-function
 * unit) and this retype broke `make SPLIT=1 compare` on it -- per-function
 * try_match compiles one unit and cannot see a caller in another file, so the
 * split build is the only thing that catches this. Its four remaining callers
 * are still in asm/code.s, which is what the "not promoted" reading confused
 * it with. sub_0801B598 is a pure forwarder with no C caller and no
 * declaration here, so the pointer type was propagated through it rather than
 * cast at the call site; forwarding a pointer parameter unchanged is
 * byte-neutral and it re-verified. */
u16 sub_0808AC44(u8, void (**)(void));

/* sub_0808AF00 returns at least `int`. sub_0801B648 returns its result
 * directly with NO re-narrowing, and that is decisive here rather than merely
 * suggestive: the function's other arm returns an `int` parameter, so the
 * return type is int, and a u16-returning callee would have been re-narrowed
 * before being widened back. Arity is a floor for the usual reason. */
/* WAVE 44 (W44-G) settles the arity at TWO from the callee's own body, which is
 * what the "floor" above was waiting for: sub_0808AF00 opens `adds r5, r1, #0`
 * and hands r5 to the relocated sub_0808AED0 as its `src`, so r1 is a real
 * parameter. Both un-promoted callers (sub_0808AFC8 at 0x0808B00E and
 * sub_0801B648) set up r0 AND r1. sub_0801B648's promoted body was updated to
 * forward its own `b` explicitly and re-verified byte-for-byte -- a
 * pass-through argument costs no instruction, so that edit is free.
 *
 * The second parameter is a SOURCE BUFFER POINTER by its use (sub_0808AED0's
 * first parameter) but is typed `int` here because sub_0801B648's other arm
 * RETURNS the same value as an `int`; `int` is the spelling every existing
 * caller already agrees with, and sub_0808AF00 casts it at the one use. */
int sub_0808AF00(u16, int);

/* ---- the 0x0808AE30 SRAM block (wave 44, W44-G) ----------------------- */

/* sub_0808AE30 / sub_0808AED0 are the two leaf SRAM primitives, and they are
 * declared here rather than left file-local because sub_0808AE54, sub_0808AF00
 * and sub_0808AF74 take their ADDRESSES as pool words in order to copy their
 * machine code onto the stack. Both signatures are read off c_0808AE30.c's
 * matched body and off the argument set-up at those three call sites.
 *
 * sub_0808AED0 returns a POINTER, not a flag: its mismatch arm is
 * `subs r0, r3, #1`, the second parameter's pre-increment value, and its
 * fall-through arm is `movs r0, #0`. Its callers only ever test it against 0,
 * which is why sub_0808AF00/sub_0808AF74 can return `int`. */
void sub_0808AE30(const u8 *, u8 *, int);
u8 *sub_0808AED0(const u8 *, u8 *, int);

/* sub_0808AF74 is sub_0808AF00 with a caller-supplied length instead of
 * gUnknown_08485550.unk18. Its only caller (sub_0808B02C, still in asm) sets up
 * r0, r1 and r2 and then does `adds r3, r0, #0; cmp r3, #0`, returning that
 * value -- so three arguments and an `int` return, on the same reasoning as
 * sub_0808AF00 above. Parameter 2 is `int` for the same reason too. */
int sub_0808AF74(u16, int, int);

/* ---- the 0x0808B flash driver (wave 47, W47-D) ------------------------ */

/* sub_0808B02C is sub_0808AFE8 with a caller-supplied length: same three-try
 * program-and-verify loop, but sub_0808AF74 in place of sub_0808AF00, so it
 * carries the extra `int` and returns `int` for the same reason. */
int sub_0808B02C(u16, int, int);

/* Erase-sector primitives. Every one of these returns a u16 status -- each
 * caller re-narrows the result with `lsls #0x10; lsrs #0x10`, which is agbcc
 * re-narrowing a u16-returning callee, and 0x000080FF is the shared "bad sector
 * number" code the range checks return.
 *
 * sub_0808B430 writes the JEDEC unlock/erase command for ONE sector and is
 * called only by sub_0808B4B4, which passes it a u16; sub_0808B0E8 is its
 * already-in-asm twin one level up, called by sub_0808B31C the same way. */
u16 sub_0808B0E8(u16);
u16 sub_0808B430(u16);
u16 sub_0808B4B4(u16);

/* sub_0808B184's two parameters are read off its matched body in
 * src/decomp/c_0808B184.c: a byte source and a flash destination, both walked
 * one byte at a time by sub_0808B31C. */
u16 sub_0808B184(u8 *, u8 *);

/* sub_0808B31C programs one sector: sector number then the source buffer it
 * hands to sub_0808B184 unchanged apart from the increment, hence `u8 *`. */
u16 sub_0808B31C(u16, u8 *);

/* sub_0808B2E0 is the "is this sector still blank" scan and is the one function
 * in the block that does NOT return a status: it counts down from
 * gUnknown_03005C78->unk04 and returns whatever is left when the walk hits a
 * non-0xFF byte, with no narrowing at `adds r0, r1, #0`, so the return is the
 * member's own width. */
u32 sub_0808B2E0(u8 *);

/* sub_0808A368 forwards its only argument unchanged as sub_08071AF0's
 * ProcPtr third parameter, so it is a ProcPtr and nothing narrows it. */
void sub_0808A368(ProcPtr);

/* sub_0808AC7C is the arm/start half of the 0x03000F68 timer module described
 * in unknown-globals.h -- it takes the same u8 slot id sub_0808AC44 does
 * (`lsls #0x18; lsrs #0x18` at entry) and returns nothing. */
void sub_0808AC7C(u8);
/* The disarm half of the same 0x03000F68 timer module: stops the timer, clears
 * the timer's REG_IE bit and restores REG_IME. void/void -- sub_0808ADA4 calls
 * it with no argument set up and ignores r0 afterwards, returning a value it
 * had already parked in r8 across the call. */
void sub_0808AD24(void);

/* sub_0801B9C8 -- four arguments. The result is added to a u8 field and to a
 * word field with no narrowing in between, so the return is at least `int`.
 *   Parameters 3 and 4 WERE `u16` on caller-side evidence only, and the note
 * here already flagged both as unmeasured ("a cast at the argument", "the
 * WRAPPER's width"). Wave 37 (W37-G) measured them at the CALLEE and they are
 * int: sub_0801B9C8's prologue is `adds r7,r0,#0; adds r5,r1,#0;
 * adds r4,r2,#0; adds r1,r3,#0` with NO `lsls #0x10; lsrs #0x10` on either --
 * agbcc's PROMOTE_MODE emits that pair at entry for every sub-word parameter,
 * so its absence is decisive. The `lsls/lsrs` that does appear is on the SUM
 * `record.x + a3`, i.e. the u16 conversion at sub_0801BA1C's own u16
 * parameter, which is the copy-then-narrow shape of an int parameter.
 * Re-verified src/decomp/c_0801B964.c (sub_0801B998, the only caller) after
 * the change: still byte-exact, because a u16 value converts to int free. */
int sub_0801B9C8(int, u32, int, int);

/* sub_0801BA4C returns a SIGNED halfword: sub_0801BAA8 re-narrows the result
 * with `lsls #0x10; asrs #0x10`, and the ARITHMETIC shift is the sign.
 *
 * WAVE 29 (C) REFUTES THE PARAMETER. It was `void *` here and `u8 *` in
 * src/decomp/c_0801BAA8.c, on the reading that sub_0801BAA8's `adds r0, #0x5a`
 * walks 0x5a bytes into a struct. It does not: sub_0801BA4C's body is a
 * DEGREE-BASED SINE lookup on the value it is handed --
 *   while (x < 0)     x += 0xb4 * 2;   (360)
 *   while (x > 0x167) x += -0x168;     (-360)
 *   if (x > 0xb3) x -= 0xb4;           fold the lower half-turn
 *   if (x > 0x5a)  x = 0xb4 - x;       mirror about 90
 *   gUnknown_0808F048[x], negated when the original was >= 180
 * -- every step a SIGNED compare and an add on the value itself. No pointer
 * undergoes modular reduction against 360. So sub_0801BAA8(x) is
 * sub_0801BA4C(x + 90), i.e. cosine, and 0x5a is a quarter turn, not a member
 * offset.
 *
 * Corroborated from a caller, which is the only place it could be seen:
 * sub_08064034 and sub_0806407C build rotation matrices and pass THE SAME
 * sign-extended s16 angle to both functions, storing the two results as the
 * cos/sin entries of a 20.12 matrix. A pointer cannot be that argument.
 *
 * `int` and not `s16`: the prologue is a bare `adds r2, r0, #0` with no
 * narrowing anywhere, and the reduction loops need the full value.
 * src/decomp/c_0801BAA8.c was retyped in the same edit and re-verified
 * byte-exact -- `p + 0x5a` and `a + 0x5a` are the same `adds r0, #0x5a`. */
s16 sub_0801BA4C(int);
s16 sub_0801BAA8(int);

/* sub_080718E8(src, count) -- all three flush paths in the 0x0801B000 block
 * (sub_0801BBC4, sub_0801BC08, sub_0801BCA8) call it with a pending-copy
 * descriptor's unk00 pointer and its unk0a halfword count taken straight from a
 * `ldrh`, either right after the CpuFastSet that copies that same range or, in
 * sub_0801BCA8, instead of it. */
void sub_080718E8(void *, u16);

/* Three the tree already uses but never declared. sub_0801BB88 and
 * sub_0801BE78 are copied from their PROMOTED definitions in
 * src/decomp/c_0801BB88.c and src/decomp/c_0801BE78.c, which win over any
 * weaker model; sub_0801DF94 is still assembly and its `void (void)` is a floor
 * read off sub_0801BCE0, which sets up no argument register. sub_0801B768
 * likewise comes from src/decomp/c_0801B768.c. */
void sub_0801B768(int);
void sub_0801BB88(int);
void sub_0801BE78(void);
void sub_0801DF94(void);

/* ---- callees of the 0x0806E000 block (wave 28) ------------------------ */

/* sub_08073F90 hands out a u16 PAIR through two out-parameters. sub_0806E6F4
 * gives it two adjacent halfword slots of its own frame (`mov r0, sp` and
 * `sp + 2`, with `sub sp, #4` reserving exactly the two) and then reads both
 * back with `ldrh`. The signature was already recorded in the note on
 * gUnknown_03000044/46 in unknown-globals.h; this is the declaration. */
void sub_08073F90(u16 *, u16 *);

/* Two more of sub_0806E11C's teardown calls. `void (void)` is a floor for both
 * -- it sets up no argument register and ignores both results -- but they sit
 * in a run with sub_0806D620, already declared the same way. */
void sub_0806D34C(void);
void sub_0806D840(void);

/* ---- callees of the 0x0801E000 block (wave 28) ------------------------ */

/* sub_0801E18C takes the record INDEX, and that is measured rather than
 * assumed. Its three callers sub_0801E22C, sub_0801E248 and sub_0801E264 all
 * compute `&gUnknown_0200F720[i]` and then call it, and in all three the ROM
 * leaves the index in r0 and puts the computed address in the next register
 * DOWN from the argument block -- r2/r3 in the two-argument caller, r3/r4 in
 * the three-argument one, r4/r5 in the four-argument one. Declared `(void)`,
 * agbcc reuses r0 for the address in every one of them and all three miss.
 * The register that survives is the argument. */
void sub_0801E18C(int);

/* Wave 42, W42-F: the two callees of sub_0801DF94 that had no declaration in
 * this header. Both are `void` -- each ends `pop {r0}; bx r0`, which destroys
 * the callee's result in r0 before returning. sub_0801E0A4 sets up r0/r1/r2 for
 * sub_08011C90 entirely from its own constants, so it takes nothing.
 *
 * sub_0801E22C's three narrow parameters are NOT a caller-side reading: it is
 * already promoted as `void sub_0801E22C(int index, u16 a, u16 b, u16 c)` in
 * src/decomp/c_0801E22C.c, and the promoted definition wins. The call site
 * cannot see the difference -- sub_0801DF94 (its only caller) passes 0x100,
 * 0x100 and 0 as constants, which need no narrowing either way -- so `int`
 * would have compiled and matched here while silently disagreeing with the
 * definition, exactly the class of error trymatch cannot catch because it
 * compiles one unit. tools/proto_check.py is what caught it. */
void sub_0801E22C(int, u16, u16, u16);
void sub_0801E0A4(void);

/* sub_0801E334 returns `int`, NOT the `u16` its promoted definition in
 * src/decomp/c_0801E334.c carried until wave 28 -- the definition has been
 * retyped to agree and re-verified byte-identical (the body is `return *p;`,
 * one `ldrh`, which zero-extends and so needs no extra instruction either
 * way).
 *
 * The evidence is at the CALLER, which is where a return type is settled:
 * sub_0801E950 forwards the result straight into sub_0801E0C8's `int`
 * parameter with NO re-narrowing, and agbcc re-narrows a narrow-returning
 * callee at every call site. Declared `u16`, sub_0801E950 gains an
 * `lsls #0x10; lsrs #0x10` pair it does not have. */
int sub_0801E334(u16 *);

/* SIX parameters, not seven, and the fifth is 64 BITS WIDE. That is the whole
 * story of this little family and it was worth four functions.
 *
 * Read as seven `int`s, all four wrappers miss by exactly one register: three
 * come out 4 bytes SHORT and sub_0801ED80 8 bytes short. The tells, which only
 * make sense together:
 *
 *   * sub_0801E930 materialises the two zero words into TWO callee-saved
 *     registers (`movs r4,#0; movs r5,#0`) and then builds -1 as
 *     `movs #1; rsbs` rather than the one-instruction `subs r4,#1` an
 *     already-live zero would have allowed. Seven `int`s let CSE collapse the
 *     two zeros into one register, which is the missing 4 bytes. A DImode
 *     constant occupies a REGISTER PAIR and cannot be collapsed.
 *   * sub_0801ED80 spends `sub sp, #4` BEFORE its push and then round-trips r3
 *     through `str r3,[sp,#0x20]; ldr r1,[sp,#0x20]`. That is not a spill and
 *     not varargs (agbcc's varargs prologue is `push {r2,r3}`, measured): it is
 *     a 64-bit parameter STRADDLING the register/stack boundary, so gcc
 *     reserves a home slot to make its two words contiguous. agbcc even labels
 *     the reload `@ created by thumb_load_double_from_address`.
 *   * sub_0801ED80 then reads THREE stack slots (0x20/0x24/0x28) where six
 *     `int`s give only two.
 *
 * A 12-byte struct by value was tried first and is refuted: it compiles to
 * `ldmia`/`stmia` block copies that appear nowhere in the ROM.
 *
 * The first parameter of sub_0801ECE8 is s16 -- both wrappers narrow with
 * `lsls #0x10; asrs #0x10` at the call and the ARITHMETIC shift is the sign.
 * Nothing types the rest, so `int` is what costs no instruction. Whether the
 * 64-bit parameter is really one quantity or two words the callers happen to
 * pass adjacently is NOT settled here -- what is measured is its width and its
 * alignment behaviour at a call. */
int sub_0801E338(int, int, int, int, long long, int);

/* ---- wave 29, W29-B: address-locality block 0x0801D --------------------- */
/* The two workers the 0x0801D7xx wrappers forward to. ARITY IS READ OFF THEIR
 * OWN PROLOGUES, not off the wrappers: sub_0801D6E8 reads `[sp,#0x14]` after
 * pushing five registers, so five parameters; sub_0801D78C reads `[sp,#0x18]`
 * and `[sp,#0x1c]` after pushing five and subtracting 4, so six. Both return a
 * value (sub_0801D6E8's early exit is `adds r0,r3,#0` on a -1). Every argument
 * is forwarded unchanged at all five call sites, so `int` throughout is what
 * costs no instruction -- it is the weakest fit, not a proof of the widths. */
int sub_0801D6E8(int, int, int, int, int);
int sub_0801D78C(int, int, int, int, int, int);
/* sub_0801D348's other branch. Its body wants narrower types than this
 * (`lsls #0x10; asrs #0x10` on argument 1, `strh` on 2 and 3, `ldrh` on the
 * stack argument 5) but the declaration is kept wide DELIBERATELY: its one
 * caller forwards all five straight through with no conversion, which a
 * declared-narrow parameter would not have allowed. Re-derive it when
 * sub_0801E4B0 itself is matched. */
int sub_0801E4B0(int, int, int, int, int);
/* Wrappers matched in wave 29. sub_0801D7D4 keeps its own r3 and appends
 * (0, 0x1d); sub_0801D804 is the same call with argument 4 forced to 0, the
 * same relationship sub_08015578's pair has. sub_0801D7EC forces argument 4 to
 * 0 and pushes its own last two along. */
int sub_0801D7D4(int, int, int, int);
int sub_0801D7EC(int, int, int, int, int);
/* Argument 7 is a 64-bit quantity: it is loaded as two adjacent words at
 * [sp,#0x24]/[sp,#0x28] with agbcc's own "created by
 * thumb_load_double_from_address" pairing and lands in sub_0801E338's declared
 * `long long` slot. Argument 8 is `u16` at entry (`lsls #0x10; lsrs #0x10`) and
 * is cast to `(s16)` at the one use. Argument 1 is only ever tested `& 1`. */
void sub_0801D348(int, int, int, int, int, int, long long, u16);
void sub_0801D81C(int);
/* The three fixed-point readers. All divide by 256 -- the `cmp #0; bge;
 * adds #0xff` before the `asrs #8` is a SIGNED DIVIDE rounding toward zero, not
 * a shift, and the members are the s32 unk0c/unk10/unk14/unk18. sub_0801D9AC
 * reads position plus offset, sub_0801D9E4 the offset alone and sub_0801DA14
 * the position alone. */
void sub_0801D9AC(int, s16 *, s16 *);
void sub_0801DA14(int, s16 *, s16 *);
int sub_0801ECE8(s16, int, int, int, long long, int);

/* FIVE parameters: three in registers, then the same 64-bit quantity starting
 * in r3 and continuing on the stack, then an s16. sub_0801EDF8 narrows its own
 * fourth parameter with `lsls #0x10; asrs #0x10` into the slot past it. */
int sub_0801ED80(int, int, int, long long, s16);

/* ---- wave 29 (C) ---- */
/* FOUR parameters and void (`pop {r0}`). r0/r1 are parked in r8/sb untouched
 * and stored to +0x2c/+0x30 of the proc it starts; r2 goes to the `int`
 * gUnknown_030044D4 AND to +0x54 as a word, so it is a full word; r3 is the
 * parent handed to Proc_StartBlocking. sub_0803FECC passes 0 for the third and
 * sub_0803FEDC/sub_0803FF04 pass -1 and -2, which is why it is signed. */
void sub_0803FF48(int, int, int, ProcPtr);
/* src/decomp/c_0803F3E4.c already defines this as
 * `void sub_0803F3E4(int a, int b, ProcPtr parent)`; the declaration is added
 * here because sub_0803F3C8 calls it from another unit. Both its word stores
 * are `str`, and `adds r1, r2, #0` at the call in the definition fixes the
 * parent as the third parameter. */
void sub_0803F3E4(int, int, ProcPtr);

/* The two arms of sub_08041958.
 *
 * sub_0804074C is declared WITHOUT a prototype, the same way sub_0801C240 is
 * above: its first parameter is a pointer to an object whose +0x02 halfword and
 * +0x04 byte it reads, and whoever promotes sub_0804074C will want to name that
 * struct locally. Its second parameter is the parent for Proc_StartBlocking.
 *
 * sub_08040790 takes THREE. The third is not visible at sub_08041958's call --
 * r2 already holds the proc there, so no instruction sets it up -- but
 * sub_08040790's own prologue does `adds r1, r2, #0` before
 * `bl Proc_StartBlocking`, which is a parent arriving in r2. The first two are
 * stored as words at +0x2c/+0x30 of the new proc and are also used as a cell
 * column and a cell row into the gUnknown_08499590 grid, so `int` for both. */
void sub_0804074C();
void sub_08040790(int, int, ProcPtr);

/* ---- wave 31, W31-B: the 0x08040000 block's cross-unit callees ---- */
/* Already DEFINED in src/decomp/c_0803FECC.c with exactly these signatures;
 * declared here because sub_0804026C and sub_08040290 call them from another
 * unit. Both prologues are bare `adds rN, rM, #0` copies with no PROMOTE_MODE
 * narrowing, so all three parameters are word wide, and the third is the
 * ProcPtr they forward to sub_0803FF48's parent slot. */
void sub_0803FEDC(int, int, ProcPtr);
void sub_0803FF04(int, int, ProcPtr);
/* THREE parameters and void (`pop {r0}` after a `bl` whose result is dropped).
 * The first two are a cell column and a cell row into the gUnknown_08499590
 * grid -- r0 is added to a rowOffset entry and r1 is scaled `lsls #1` as the
 * row index -- and the third is untouched all the way through to
 * sub_0804046C's fifth argument, which is a Proc_StartBlocking parent. Its one
 * caller sub_080409B4 passes its own proc there. */
void sub_08040380(int, int, ProcPtr);
/* SIX parameters, the last two on the stack, and void. Nothing narrows any of
 * them: the prologue is four bare register copies plus `ldr r7,[sp,#0x18]` /
 * `ldr r1,[sp,#0x1c]`, and r7 reaches a `strh` only at the store, which is a
 * conversion at a use and not a declared width. The sixth is the
 * Proc_StartBlocking parent. sub_08040624 is the four-parameter wrapper that
 * fixes the third and fourth at 0x1CA and 5. */
void sub_08040554(int, int, int, int, int, ProcPtr);
void sub_08040624(int, int, int, ProcPtr);

/* Wave 29 (C), the 0x0808A block.
 *
 * sub_0808A5C4 reads no argument register before writing it (it opens with a
 * `bl`) and ends `pop {r0}`, so nullary and void.
 *
 * sub_08014740 takes SIX and returns the sub_080152EC slot it allocates (r8 is
 * that result and is what r0 carries out). FIVE of the six are `u16`: its
 * prologue narrows r0, r1, r3 and both stack arguments with `lsls #0x10;
 * lsrs #0x10`, which is PROMOTE_MODE and which an `int` parameter never
 * produces. The third is untouched and is forwarded as a pointer --
 * sub_0808A6A0 passes the dereferenced `u16 *` gUnknown_08499578. */
void sub_0808A5C4(void);
/* CORRECTION, wave 32 (W32-A): arguments ONE and TWO are `s16`, not `u16`.
 * The wave-29 reading above is right that the prologue narrows them -- but
 * PROMOTE_MODE zero-extends EVERY sub-word parameter regardless of signedness,
 * so the prologue proves narrow and says nothing about the sign. sub_08077214
 * is the first caller that hands them a variable rather than a literal, and it
 * sign-extends both (`asrs r5, r5, #0x10; asrs r4, r4, #0x10`) with no further
 * conversion in front of the `bl`. Through a `u16` parameter agbcc would
 * convert that back down with `lsls; lsrs` instead. The other three narrow
 * arguments keep `u16` -- nothing has exercised their sign yet.
 * Byte-neutral for the two existing callers (c_08018758.c and c_0808A664.c both
 * pass literals); both re-verified. */
struct Unk03001470 *sub_08014740(s16, s16, u16 *, u16, u16, u16);
/* Promoted as `void sub_0808AC20(void)` in src/decomp/c_0808AC20.c; declared
 * here because sub_0808AC44 publishes its address through a parameter. */
void sub_0808AC20(void);

/* The BIOS LZ77 decompressor, VRAM variant. Nothing declared it before wave 29
 * even though `asm/` calls it in several places; the shape is the standard BIOS
 * one and sub_0804BB28 passes a ROM blob pointer and a destination. */
void LZ77UnCompVram(const void *, void *);
/* Declared WITHOUT a prototype, like sub_0801C240: its first parameter is
 * walked as a `u16 *` (64 halfwords, each halved per 5-bit channel) into a
 * scratch buffer before a CpuFastSet, and whoever promotes it will want to name
 * that pointer type. Its second parameter is the CpuFastSet destination. */
void sub_0804BD58();

/* Wave 29, W29-A -- the 0x0802D000 address-locality block's callees. Each was
 * already DEFINED or is still asm; none had a declaration, so these publish
 * what their own prologues say.
 *
 * sub_08029948's parameter is `int`, not `u16`: the prologue is
 * `adds r4,r0,#0` FOLLOWED by `lsls #0x10; lsrs #0x10`, i.e. copy-then-narrow,
 * which is the cast-at-a-use shape and not PROMOTE_MODE on a declared-narrow
 * parameter. The narrowed value is `strh`ed into gUnknown_03001470[i].unk22.
 * Its two callers (sub_0802D168, sub_0802D1A0) pass literals, so they cannot
 * discriminate.
 *
 * sub_080637AC is a slot lookup over gUnknown_03001470: it walks the array
 * DOWN from index 0x1d (base + 0xae0, `subs r1,#0x60` per step) and returns
 * the first element whose unk00 equals the argument, else 0 -- so the return is
 * a `struct Unk03001470 *` and the argument is the script address callers hand
 * it (gUnknown_0848A42C at sub_0802D33C). `const void *` is the weakest model:
 * nothing dereferences the argument, only compares it.
 *
 * sub_080236E8 / sub_08042650 / sub_08042864 / sub_08060684 / sub_080606A0 all
 * open by loading a pool word or making another `bl` and never read r0, so
 * `void (void)`; all five end `pop {r0}; bx r0`.
 *
 * sub_0802D5B8 is `Decompress(sub_08037250(), a1)` -- the destination buffer,
 * hence `void *` to match Decompress's second parameter.
 *
 * sub_0802D7B4's parameter is `int`: its one readable caller sub_0802D99C
 * SIGN-extends the value into r0 (`lsls #0x10; asrs #0x10`) immediately before
 * the `bl`, which a declared-narrow parameter would not ask for -- a u16
 * parameter makes the caller emit the zero-extending `lsrs` instead. The
 * `lsls #0x10; lsrs #0x10` in sub_0802D7B4's own prologue is a `u16` local it
 * spills to [sp,#0x1c], not PROMOTE_MODE. */
void sub_08029948(int);
struct Unk03001470 *sub_080637AC(const void *);
void sub_080236E8(void);
void sub_08042650(void);
void sub_08042864(void);
void sub_08060684(void);
void sub_080606A0(void);
void sub_0802428C(void);
void sub_08035810(void);
/* Wave 34 (W34-I). All three are called argument-free by the block-0x08042
 * cursor helpers (sub_080424FC, sub_0804256C, sub_08042B9C). sub_080176A4 and
 * sub_080198D0 discard the result at every call site, so void.
 * sub_08035170's result is stored straight into gUnknown_03003FC0's byte at
 * +0x2e with a bare `strb` and no re-narrowing -- which does NOT discriminate
 * u8 from int, since `strb` truncates either way. u8 is the weaker guess of
 * the two and is recorded as UNPROVED. */
void sub_080176A4(void);
void sub_080198D0(void);
u8 sub_08035170(void);
u8 *sub_08037250(void);
void sub_0802D5B8(void *);
void sub_0802D76C(void);
void sub_0802D7B4(int);

/* Wave 29, W29-A -- the 0x08025000 address-locality block's callees.
 *
 * TWO OF THESE FIX AN ARITY THAT IS INVISIBLE AT THE CALL, and both are read
 * off the callee's prologue exactly as docs/agbcc-codegen.md says to:
 *
 *   sub_08024F20 takes THREE arguments. r2 is never written before
 *   `ldrh r0,[r2]` / `ldrh r0,[r2,#2]`, whose results are `strb`ed into
 *   gUnknown_08499594[i].unk02 / .unk03 -- the {u16;u16} pair, i.e.
 *   struct Unk802C57C. Its ONE caller, sub_080251BC, never touches r2, so
 *   sub_080251BC has a third parameter too and forwards it for free. r0 and r1
 *   are `s16`: sub_080251BC narrows both with `lsls #0x10; asrs #0x10` in front
 *   of the `bl` and tests its own r1 raw (`cmp r1,#0`, no PROMOTE_MODE), so the
 *   sign-extension is the CONVERSION at the call and not a cast in the caller.
 *
 *   sub_08035740 -- already defined as `void sub_08035740(void *)` in
 *   src/decomp/c_08035740.c but never declared -- reads r0 before writing it,
 *   and sub_08025BB4 opens with a bare `bl sub_08035740`. So sub_08025BB4 has a
 *   `void *` parameter it forwards. `void sub_08025BB4(void)` is refutable, not
 *   just unproved: it would have to pass a literal, and any literal costs a
 *   `movs r0,#N` the ROM does not have.
 *
 * sub_080251D8 likewise reads r0 (`adds r1,r0,#0` then `lsls #0x10; asrs #0x10`
 * at a use -- copy-then-narrow, so `int`), which is why sub_080251BC's else-arm
 * `bl` needs no argument setup at all.
 *
 * sub_08025AEC and sub_080254AC both scan gUnknown_08499594 for a free slot and
 * return the element address or 0, so both return `struct UnitRecord *`; both
 * end `pop {r1}` / `bx lr` with r0 live. sub_08025BE0 initialises one of those
 * records field by field at +0..+0xb, which is the whole 0x0c-byte struct.
 *
 * sub_080211DC is `(u8, s8)` off its own prologue: BOTH arguments are narrowed
 * in place with `lsls #0x18; lsrs #0x18` (PROMOTE_MODE, which zero-extends
 * whatever the signedness), and the second one alone is re-read at its use as
 * `lsls #0x18; asrs #0x18` before going out on the stack -- the second shift
 * pair is where the sign lives. sub_08025340 passing -1 corroborates it. */
void sub_08024F20(s16, s16, struct Unk802C57C *);
void sub_080251D8(int);
void sub_080211DC(u8, s8);
struct UnitRecord *sub_080254AC(void);
struct UnitRecord *sub_08025AEC(void);
void sub_08025BE0(struct UnitRecord *, u8);
void sub_08025D20(int);
void sub_08035740(void *);
/* Wave 29, W29-A. `s16` and not `u16`, and the discriminator is entirely on the
 * CALLER side: sub_08025C5C's own prologue is `lsls #0x10; lsrs #0x10` on all
 * three, which PROMOTE_MODE emits for u16 and s16 alike (probed both). What
 * separates them is sub_08025C98 / sub_08025CC8, which SIGN-extend all three
 * arguments before the `bl`; declaring the parameters `u16` makes those two
 * callers emit `lsrs` there instead and neither one matches. Returns the
 * gUnknown_08499594 slot sub_08025AEC handed out, or NULL. */
struct UnitRecord *sub_08025C5C(s16, s16, s16);
/* Wave 32 (W32-B): the same three `s16` as sub_08025C5C, read straight off its
 * own prologue (`lsls #0x10; asrs #0x10` on r0, r1 and r2 -- SIGN extension, so
 * not PROMOTE_MODE's zero-extend), and it returns sub_08025C5C's slot unchanged
 * or 0. sub_08045564 writes through the result, which is what fixes the return
 * type rather than only the family resemblance. */
struct UnitRecord *sub_08025CC8(s16, s16, s16);

/* Wave 29, W29-A. Both are already DEFINED (src/decomp/c_0803CD14.c,
 * src/decomp/c_0803CCEC.c) and were never declared; these publish the
 * definitions unchanged. sub_0802490C and sub_08024944 are the callers, and
 * they corroborate the `u8` parameter -- each narrows `id + 0x4c` with
 * `lsls #0x18; lsrs #0x18` in front of the `bl`. The `lsls #0x18; lsrs #0x18`
 * AFTER sub_0802490C's call is not a re-narrowing of sub_0803CD14's `int`
 * result; it is sub_0802490C's own `u8` return conversion. */
int sub_0803CD14(u8);
u8 *sub_0803CCEC(u8);

/* Wave 29, W29-A -- the 0x08026000 / 0x0802A000 blocks' callees.
 *
 * sub_0803FECC takes THREE arguments, and the third is the invisible one again:
 * its whole body is `adds r3,r2,#0; movs r2,#0; bl sub_0803FF48`, so it forwards
 * r2 into sub_0803FF48's declared `ProcPtr` fourth parameter and passes 0 for
 * the third. sub_0802A588 opens `adds r2, r0, #0` -- it is parking its own proc
 * pointer in r2 for exactly that argument, which is otherwise unexplained.
 *
 * sub_08026584 is a bare `bx lr`, four bytes. Nothing about its signature is
 * recoverable from the callee.
 *   The second parameter is u16, settled in wave 45 (W45-D) from sub_080265D0,
 * which is the only caller that passes a NON-CONSTANT: it holds the value in a
 * u32 (the `__fixunsdfsi` result is compared against 9999 at full width, with
 * no narrowing anywhere in the body) and then spends `lsls #0x10; lsrs #0x10`
 * on it immediately before the `bl`. An `int` parameter would not pay for that
 * pair. sub_080265B0 passes literals 5 and 0xa, where u16 and int are identical
 * code, which is why the old `int` reading survived -- it had no discriminating
 * caller. c_080265B0.c re-verified as still MATCHED after this change.
 *
 * sub_08025D60 walks a 12-byte record list recursively and takes a signed index
 * -- `asrs r4,r4,#6` on the argument is arithmetic. void (`pop {r0}`).
 * sub_08020984 reads no argument register and ends `pop {r0}`. */
void sub_08026584(u8, u16);
void sub_08020984(void);
void sub_0803FECC(int, int, ProcPtr);
void sub_08025D60(int);
/* Wave 34, W34-F. sub_08040200 hands it the same struct Unk02028360 * it has
 * just been reading unk00/unk01 off, in r0 and nothing else; the result is
 * dropped. */
/* Wave 34 integration: agrees with the promoted definition in
 * src/decomp/c_0803E0D0.c, which returns its argument's type and is
 * deliberately non-void with no return statement (see the comment there).
 * The tag is completed in that .c; an incomplete type is all a pointer
 * parameter needs. Callers holding another view of the object cast. */
struct Unk3E0D0;
struct Unk3E0D0 *sub_0803E0D0(struct Unk3E0D0 *);
/* Wave 34, W34-F. Both matched this wave and both are dispatched from
 * sub_080407E4's jump table. sub_08040200 takes the proc's entry pointer and
 * the proc itself, the same (entry, parent) pair sub_0804026C and sub_08040290
 * beside it take; sub_080402B4 takes the two cell coordinates instead, and
 * forwards its ProcPtr to sub_0803FF48's declared fourth parameter. */
void sub_08040200(struct Unk02028360 *, ProcPtr);
void sub_080402B4(int, int, ProcPtr);
/* The other two arms of the same jump table, already promoted in
 * src/decomp/c_0804026C.c; declared here so sub_080407E4 can dispatch to them.
 * Signatures copied from that file, not inferred. */
void sub_0804026C(struct Unk02028360 *, ProcPtr);
void sub_08040290(struct Unk02028360 *, ProcPtr);
/* Wave 34, W34-F. Called by sub_08040380 with no argument register set up at
 * the call site and its result dropped, so `void (void)` is the weakest model
 * that fits. This was the only genuinely EXTERNAL undeclared callee across
 * blocks 0x08040 and 0x08041 -- the other four the block screen reported were
 * themselves targets in this batch. */
void sub_08021CB4(void);
/* Already promoted in src/decomp/c_08040430.c; declared here so sub_08040380
 * can drive the pair. Signatures copied from that file, not inferred. */
void sub_08040430(int, int);
void sub_0804046C(int, int, int, int, ProcPtr);
/* Wave 34, W34-F, both off sub_080408A0's call sites. sub_080232CC takes two
 * small literals (2, 0x12) and sub_0804096C the caller's own proc; neither
 * result is read. */
void sub_080232CC(int, int);
void sub_0804096C(ProcPtr);

/* Wave 29, W29-A -- sub_08052EE4 / sub_08052F20's callees. All four read no
 * argument register before writing it and all four end `pop {r0}` (sub_08012420
 * is already matched in src/decomp/c_08012420.c and simply had no declaration).
 * The void returns are floors: sub_08052F20 discards every result. */
void sub_08012420(void);
void sub_080546F0(void);
void sub_08054B14(void);
void sub_08057270(void);


/* ---- Wave 30, W30-A: the 0x08031/0x08032/0x08039 address-locality block ---- */

/* Already promoted as `void sub_080337D8(u32, u32, ProcPtr)`
 * (src/decomp/c_080337D8.c) but never declared here; sub_08031BF0 is the first
 * caller outside its own unit. */
void sub_080337D8(u32, u32, ProcPtr);
/* The five-argument sibling of sub_080337D8: sub_08031C1C passes the same
 * gUnknown_02000000 buffer, a 0xA5C size, two zeros and its own proc on the
 * stack. The proc is last, matching sub_080337D8's third-and-last position.
 *
 * Wave 34, W34-C RETYPES THE RETURN from `void` to `int`, on the body: it has
 * two exits, `movs r0,#1; rsbs r0,r0,#0` (return -1) when the size argument
 * exceeds 0x7FFF80, and a `movs r0,#0` immediately before the shared epilogue
 * on the success path. A void function does not set r0 on the way out, and it
 * certainly does not set it on BOTH paths. Its one promoted caller
 * (src/decomp/c_08031BF0.c) discards the result, so the change is byte-neutral
 * there; re-verified with try_match after the edit. */
int sub_0803376C(u32, u32, int, u8, ProcPtr);
/* Wave 34, W34-C. Undeclared callees reached from the 0x08032..0x08034 block.
 * Every signature below is read off the CALL SITE, not a body.
 *
 * sub_0803CD2C: sub_080328EC passes 0x200 and the s16 gUnknown_0849B060->unk04
 * truncated by a bare `ldrb` -- the load-width fold a u8 parameter forces -- and
 * re-narrows the result with `lsls #24; lsrs #24` before comparing it to 0.
 *
 * sub_08026704: sub_080349E4 passes the u16 gUnknown_030033EC with a plain
 * `ldrh` (no shifts either side, so the parameter is no narrower than 16 bits;
 * `int` is the weakest that fits) and re-narrows the result `lsls #16;
 * lsrs #16`, which only a u16 return emits.
 *
 * sub_080348B4 and sub_0802F4F4 are both tested `lsls #24; cmp #0` under an
 * `if`, the test-the-low-byte form; sub_0802F4F4 is already recorded elsewhere
 * in this header as returning s8 from its readers of
 * gUnknown_0849B018->unk06.
 *
 * sub_08063454 / sub_08063518 take gUnknown_03003F70, the same link-session
 * record sub_08062FB8 and sub_08062FF4 do. Their signatures are NOT read off
 * sub_08033470's call site: both are already promoted
 * (src/decomp/c_08063454.c, src/decomp/c_08063518.c) and the DEFINITIONS win,
 * so these declarations copy them verbatim. Wave 34, W34-C first wrote them as
 * `u8 *` from the caller and tools/proto_check.py caught it -- that is the
 * wave-14 SPLIT=1 breaker, and per-function try_match cannot see it because it
 * compiles one unit. Callers pass gUnknown_03003F70, declared `u8 []`, so the
 * cast is at the call site. */
u8 sub_0803CD2C(u16, u8); /* W35-E: 1st was `int`. The ROM narrows it
                           * `lsls #0x10; lsrs #0x10` BEFORE the u8 second
                           * parameter is touched -- entry-order parameter
                           * conversion, not a use-site cast. Its one caller
                           * (c_080328EC.c) passes the constant 0x200. */
u16 sub_08026704(int);
bool8 sub_080348B4(void);
s8 sub_0802F4F4(void);
/* Wave 41, W41-D. The two `(gUnknown_0849B018->unkNN >> index) & 1` bit
 * readers, copied VERBATIM from their promoted definitions
 * (src/decomp/c_0802F460.c, c_0802F480.c) -- the definitions win. Declared now
 * because sub_0802F504 and sub_0802F534 are their first cross-file callers.
 * Both call sites pass an `int` loop counter and agbcc converts it with
 * `lsls #0x18; asrs #0x18`, which is the s8 parameter and not a source cast. */
bool8 sub_0802F460(s8);
bool8 sub_0802F480(s8);
/* Wave 42, W42-M. Copied from the promoted definition (src/decomp/c_0802F504.c)
 * -- the definition wins. Declared now because sub_0803227C is its first
 * cross-file caller. The caller narrows the result `lsls #0x18; lsrs #0x18`,
 * which looks like a u8 return but is not: PROMOTE_MODE holds the s8 pseudo
 * zero-extended, and every READ of it re-extends signed (`lsls #0x18; asrs`). */
s8 sub_0802F534(void);
void sub_08063454(struct Unk08062FB8 *, int, int, u8, s8);
int sub_08063518(struct Unk08062FB8 *);
/* Wave 42 (W42-K). The parameter is a BYTE BUFFER and `u32` is very probably
 * the wrong spelling -- the body is sub_080308B4's twin and copies
 * `unk06[i] = a1[i]` for i = 0..127 out of it, and sub_080308B4 is promoted
 * taking `u8 *`. LEFT AS `u32` ANYWAY, deliberately: the one caller,
 * src/decomp/c_0803355C.c, is already matched and passes `proc->unk24`, whose
 * file-local struct member is `u32` and is assigned in a chain with three
 * integer siblings (`p->unk24 = p->unk26 = p->unk28 = p->unk2a = 0`), so it
 * cannot become a pointer without churn there. The question is byte-neutral --
 * the value arrives in r0 either way -- so there is no oracle to settle it and
 * the promoted caller wins. sub_08030930 casts internally. */
void sub_08030930(u32);
/* Wave 50, W50-J. Two arguments, from sub_08033C68's only call site: r0 is the
 * u8 proc->unk36 cursor and r1 is the caller's own proc pointer, which is the
 * (value, proc) order sub_080337D8 above also uses. Return value unused there,
 * and `int` is the weakest first parameter that reproduces the bare `ldrb` at
 * the call -- a `u8` parameter is byte-identical, so the width is NOT settled.
 * Arity is a floor, not a proof: anything past r1 would be invisible here. */
void sub_0803388C(int, ProcPtr);
void sub_080338C0(int);
void sub_08026900(void);
void sub_0802BFA8(void);
void sub_080351F0(void);
/* Already promoted as `void sub_08034A58(int, const char *)`
 * (src/decomp/c_08034A44.c) but never declared here; sub_08034A7C is the first
 * caller outside its own unit. */
void sub_08034A58(int, const char *);
/* Already promoted as `void sub_080328C0(u16 *)` (src/decomp/c_080328C0.c) but
 * never declared here; sub_080328EC is the first caller outside its own unit. */
void sub_080328C0(u16 *);
void sub_08030F60(int);
/* sub_08031E7C passes (0x11, -1). The -1 is `movs r1,#1; rsbs r1,r1,#0`, the
 * constant, NOT a bitfield mask -- it goes straight out as the argument with
 * no `ands` anywhere.
 * WAVE 40 (W40-B): narrowed to (u8, s8). Required by sub_080139C4, which
 * narrows both arguments at the call (`lsls #0x18; lsrs #0x18` on the first,
 * `lsls #0x18; asrs #0x18` on the second) -- neither is emitted against an
 * `int` parameter. Free at this call site: a probe of `g_narrow(0x11, -1)`
 * against `g_int(0x11, -1)` emits the same `movs r1,#1; negs r1,r1;
 * movs r0,#0x11`, so the note above still holds and c_08031E7C.c is unaffected.
 * See sub_080137AC for why the second parameter is signed. */
void sub_0801394C(u8, s8);
/* Wave 49 (W49-F) retypes the RETURN from void to int, on the body rather than
 * on a caller: sub_0802F588 has two exits, `movs r0,#1; rsbs r0,r0,#0` (-1, the
 * ring-full failure) and `adds r0,r6,#0` (the halfword count it queued), and a
 * void definition cannot produce either.  Byte-neutral at every one of its
 * eight callers, all of which discard the result -- an ignored int call and a
 * void call are the same instruction stream. */
int sub_0802F588(struct Unk0202575C *, int);
void sub_0803227C(void);
/* Coordinates: sub_08032420 feeds it `gUnknown_0849B060->unk04 * 40` and
 * `->unk06 * 40`, each emitted as `lsls #2; adds; lsls #0x13; asrs #0x10` --
 * the x5 strength reduction with the x8 folded into the s16 narrowing, which
 * is what a declared s16 parameter costs and an int parameter does not. */
void sub_08032340(ProcPtr, s16, s16);
void sub_08032950(void);
void sub_08032A00(void);
/* Mutually recursive HBlank/VCount handlers: each installs the other with
 * sub_080638D0. Declared so either can name the other's address. */
void sub_08032B84(void);
void sub_08032BA4(void);
/* THREE parameters, and the third is proved rather than guessed: sub_080397BC
 * copies its incoming proc pointer into r2 BEFORE loading either argument out
 * of it (`adds r2,r0,#0; ldr r0,[r2,#0x54]; ldr r1,[r2,#0x58]`). With only two
 * parameters agbcc keeps the base in r0 and moves the first argument in last
 * (`ldr r2,[r0,#0x54]; ldr r1,[r0,#0x58]; adds r0,r2,#0`) -- same 16 bytes,
 * five of them different. The copy lands in r2 because r2 IS the third
 * argument register. Wave 30, W30-A. */
/* Already promoted as `void sub_08044144(int)` (src/decomp/c_08044144.c) but
 * never declared here; sub_08039F58 is the first caller outside its own unit. */
void sub_08044144(int);
void sub_08044B28(int, int, ProcPtr);
/* Same third-parameter proof from sub_08039650, where it additionally forces
 * the r2/r3 split between the proc pointer and the gUnknown_08499598 base. */
void sub_08080E74(int, int, ProcPtr);
/* sub_08039820's predicate. `lsls r0,r0,#0x18` on the result before the `cmp`
 * is a narrow return being re-narrowed, so it is u8/bool8 and not int. */
u8 sub_08039850(ProcPtr);
/* Returns a literal 0 that sub_08039820 discards; the narrow return type is
 * inferred from sub_08039850, the alternative it is selected against. */
u8 sub_080398D0(ProcPtr);
/* The u16 entry narrowing `lsls #0x10; lsrs #0x10` in sub_080397F4's own
 * prologue IS the parameter declaration -- its only argument, an `ldrh` out of
 * gUnknown_085D3DD0[..].unk20[], needs no conversion. */
void sub_080397F4(u16);
/* All three parameters int, read off the promoted definition in
 * src/decomp/c_08039BB4.c (bare `adds rN,rM,#0` saves, no PROMOTE_MODE
 * narrowing). Never declared here before wave 30. */
void sub_08039BB4(int, int, int);

/* ---- Wave 30, W30-B ---- */

/* sub_0801489C IS A FOUR-BYTE `bx lr` AND NOTHING ELSE (0x0801489C, one
 * instruction plus alignment). There is no prologue, so the usual
 * read-the-callee's-narrowing route to its widths does not exist -- every field
 * of this declaration comes from its four call sites, and two of them
 * (sub_080148A0, sub_080148E0) are wave 30's:
 *   - FIVE parameters. Both callers `sub sp, #4` and `str rN, [sp]` a zero
 *     before the `bl`; nothing else in either function needs stack space.
 *   - Parameter 2 is `u16`: the value both callers compute is
 *     `unk000[i] * 2 + unk408[i]`, a 17-bit sum, and both narrow it with
 *     `lsls #0x10; lsrs #0x10` immediately before the call.
 *   - The RETURN is `u16` on the same tell -- both callers re-narrow the result
 *     with `lsls #0x10; lsrs #0x10` before returning it, which is what agbcc
 *     puts at the call of a narrow-returning callee.
 *   - Parameters 1, 3, 4 and 5 are NOT constrained. r0 is forwarded untouched
 *     from the caller's own first argument, r2/r3 are either 0 or values
 *     already zero-extended by the caller's PROMOTE_MODE, and the stack word is
 *     always 0 -- every one of those is byte-identical under `int`, `u8` or
 *     `u16`, so `int` is the weakest model rather than a reading. */
u16 sub_0801489C(int, u16, int, int, int);
/* Measures a string: it walks a NUL-terminated byte sequence, special-cases the
 * range 9..10, and accumulates gUnknown_084C36E4[c] per character. Its only
 * caller sub_08014D20 converts the result to tiles as `(w + 6) / 8` with the
 * signed `bge; adds #7; asrs #3` bias sequence, so the return is a SIGNED word
 * -- an unsigned one would be a bare `lsrs #3`. */
int sub_08014D38(const char *);
int sub_08014D20(const char *);
/* The two halfword-valued queries sub_08027844 / sub_08027A08 run on
 * gUnknown_03001FBC. Both return s16: each caller re-narrows the result with
 * `lsls #0x10; asrs #0x10` and then compares it SIGNED (`cmp #0x10; bgt`,
 * `cmp #4; bgt`). The parameter is s16 for the same reason sub_080157A4 /
 * sub_080157F4's first is -- gUnknown_03001FBC is a declared `s16` global and
 * arrives via `ldrsh`.
 *
 * CORRECTED at wave-30 integration: the return is `u16`, NOT `s16`, and the
 * caller-side reading above is a textbook cast-at-a-use error. Both bodies are
 * a single `return tbl[i].field;` compiled to `ldrh r0, [r0, #60]` flowing
 * straight into `bx lr` -- an UNSIGNED halfword load with no re-narrowing. `s16`
 * forces `ldrsh`, which needs the offset in a register (`movs r1, #60; ldrsh
 * r0, [r0, r1]`) and costs +4 bytes on a 36-byte function; measured at 77.8%.
 * So the callers' `lsls #0x10; asrs #0x10` is an explicit `(s16)` cast in the
 * CALLER's source, which is exactly what the brief's copy-then-narrow rule says
 * a narrowing after a `bl` means when the value is used afterwards. A signed
 * compare downstream constrains the caller's local, not the callee's return.
 * The discriminating evidence here is callee-side (`ldrh` vs `ldrsh`) and it
 * beats the call-site shape. */
u16 sub_08015820(s16);
u16 sub_080157D0(s16);
/* sub_0801C210's allocator and initialiser, read off sub_0801C210 (their only
 * caller) plus their own bodies. sub_0801C6E8 scans gUnknown_03000288's 16
 * slots and returns the free one or NULL, which is the value sub_0801C210
 * NULL-tests and returns. sub_0801C69C takes the handle plus sub_0801C210's
 * three arguments forwarded unchanged -- their widths are invisible at that
 * call (the values are already zero-extended by sub_0801C210's own
 * PROMOTE_MODE, so any narrowing there would be elided), so these mirror
 * sub_0801C210's declared widths rather than measuring anything. */
struct Unk0801C210 *sub_0801C6E8(int);
void sub_0801C69C(struct Unk0801C210 *, void *, u16, u8);
/* The rest of the 0x0801Cxxx animation-handle vocabulary.
 *   sub_0801C27C / sub_0801C2DC  the two halves of "advance one step":
 *     sub_0801C254 calls them in that order and re-narrows only the second's
 *     result (`lsls #0x18; lsrs #0x18`), which is what makes sub_0801C254 `u8`.
 *   sub_0801C640  installs a script: it STORES its second argument into the
 *     handle's +0x00 and derives +0x04/+0x08/+0x0c from it. `void *` because
 *     the body reads it as u16-offset table or as u32 pointers depending on
 *     the handle's +0x20 bit 1, so no single element type describes it.
 *   sub_0801C51C  a PASS-THROUGH wrapper and the arity is only visible that
 *     way: it never touches r1 at all, yet calls sub_0801C640, which reads r1
 *     and stores it. A one-parameter sub_0801C51C would be storing garbage.
 *   sub_0801C67C  re-runs sub_0801C2DC with +0x18/+0x1a forced, restoring
 *     +0x1a afterwards. void -- `pop {r4, r5}; pop {r0}`. */
/* WAVE 36 (W36-H) CORRECTION to the two lines below: sub_0801C27C takes THREE
 * parameters and returns void, and sub_0801C2DC's `u8` is right.
 *   sub_0801C27C's own body opens `adds r5,r1,#0; adds r6,r2,#0` and feeds both
 * to PutSpriteExt (r1 OR-ed with the priority word, r2 forwarded whole), so the
 * arity is a hard readout from the definition. It was invisible at its ONE
 * caller because sub_0801C254 forwards its own a2/a3 untouched -- the
 * pass-through case in the brief -- and c_0801C254.c is updated to pass them.
 * `_0801C2D2` is reached by three paths and sets no r0, so it is void; the `u8`
 * sub_0801C254 re-narrows comes from sub_0801C2DC alone. */
void sub_0801C27C(struct Unk0801C210 *, int, int);
u8 sub_0801C2DC(struct Unk0801C210 *);
/* sub_0801C3EC and sub_0801C53C: sub_0801C27C's two conditional side calls,
 * both `adds r0,r4,#0; bl` on the handle with no other argument register set
 * and no result read. */
void sub_0801C3EC(struct Unk0801C210 *);
void sub_0801C53C(struct Unk0801C210 *);
void sub_0801C640(struct Unk0801C210 *, void *);
void sub_0801C51C(struct Unk0801C210 *, void *);
void sub_0801C67C(struct Unk0801C210 *);
/* The 0x08028xxx block's callees.
 *   sub_080266DC(u8) -> u8   sub_080288D8 and sub_08028904 both narrow the
 *     argument to a byte (`lsls #0x18; lsrs #0x18`) off a u16 parameter and
 *     truth-test the result with `lsls r0,#0x18`.
 *   sub_080271CC(int) -> u8  INT, not u16, and sub_080289BC is what proves it:
 *     it passes a raw `int` parameter bare, where a u16 parameter would have
 *     put `lsls #0x10; lsrs #0x10` in front of the `bl`. Its other caller
 *     sub_08028990 passes an already-zero-extended u16 and so cannot see the
 *     difference. Result re-narrowed to u8 at both sites.
 *   sub_08028B70 returns `int`: sub_08028CF4 tests it with a BARE `cmp r0,#0`
 *     and then casts to u8 (`lsls #0x18; lsrs #0x18`) for sub_08019940's u8
 *     first parameter -- a narrow return would have re-narrowed before the
 *     compare instead.
 *   sub_08028BAC returns a byte (`lsls r0,#0x18; cmp r0,#0` at the one site).
 *   sub_08028A68 / sub_08028AEC / sub_08027118 / sub_08025EA0 are argument-free
 *     and result-discarded at every site in this block. */
/* Spelled `bool8` to agree textually with the promoted definition in
 * src/decomp/c_080266DC.c. `bool8` IS `u8` (include/gba/types.h:27), so this is
 * the same type either way and no caller changes -- but tools/proto_check.py
 * compares declaration TEXT and does not resolve typedefs, so the two spellings
 * read as a mismatch. Wave 30. */
bool8 sub_080266DC(u8);
/* Wave 41 (W41-C). Copied verbatim from the promoted definition in
 * src/decomp/c_080176A8.c so sub_08026768 can call it; it was undeclared. */
void sub_080176A8(void);
/* Wave 41 (W41-C). Copied verbatim from the promoted definition in
 * src/decomp/c_08042DE0.c so sub_08026A48 can call it; it was undeclared. */
int sub_08042DFC(int);
/* Wave 41 (W41-C). The team-colour assignment pair, both from their own
 * definitions (now matched).
 *   sub_08026A88(slot, colour) answers whether `colour` is still free among
 * gUnknown_08499598[1 .. slot-1]. Its SECOND parameter is `int`, NOT `u8`:
 * sub_08026AC0 calls it twice, and the second call passes a plain int loop
 * counter with a bare `adds r1, r4, #0` -- a `u8` parameter would have forced
 * an `lsls #0x18; lsrs #0x18` there and none is present. The callee side cannot
 * discriminate (PROMOTE_MODE zero-extends either way, and the value is only
 * compared against a `ldrb`), so the call site is the whole evidence.
 * It returns bool8: both call sites narrow the result with `lsls r0,#0x18`
 * before testing it.
 *   sub_08026AC0(slot, fallback) returns `int` and not `u8`. Its body ends on a
 * bare `adds r0, r4, #0` where r4 may hold the `fallback` argument unchanged; a
 * u8 return would have had to truncate there, and nothing does. Its one caller
 * consumes the result with `strb`, which narrows for free either way, so the
 * callee's missing truncation is the only discriminator. */
bool8 sub_08026A88(int, int);
int sub_08026AC0(int, int);
/* Wave 41 (W41-C), all three from their own definitions (now matched) and all
 * three nullary -- none reads r0-r3 before writing it.
 *   sub_0802672C returns bool8: its two exits are `movs r0,#0` and
 * `movs r0,#1` split across an unconditional `b`, the if/else-return shape.
 *   sub_08026A48 and sub_08026B28 are void -- both end `pop {r0}; bx r0` with
 * nothing setting r0 on any path. */
bool8 sub_0802672C(void);
void sub_08026A48(void);
void sub_08026B28(void);
/* Wave 45 (W45-D), both for sub_08026D68, which was the first caller of either
 * to need a declaration. Copied verbatim from the definitions rather than
 * inferred: sub_08026C6C's is src/decomp/c_08026C6C.c (a leaf whose `u8`
 * parameter is fixed by its dense jump table over ids 6..20, returning the u32
 * gUnknown_03003FC0.unk28), and sub_08026CD0's is its own body, matched earlier
 * this wave -- it reads and writes only globals and ends `pop {r0}; bx r0`. */
u32 sub_08026C6C(u8);
void sub_08026CD0(void);
/* Wave 39 (W39-E). Was undeclared even though src/decomp/c_08026F28.c has been
 * promoted; sub_08028BAC is its first caller outside its own file, and an
 * implicit declaration there would default-promote both arguments to int and
 * drop the `lsls #0x10; lsrs #0x10` pair the ROM has in front of the `bl`.
 * Text copied from that definition. */
bool8 sub_08026F28(u16, u16);
u8 sub_080271CC(int);
int sub_08028B70(void);
u8 sub_08028BAC(void);
void sub_08028A68(void);
void sub_08028AEC(void);
void sub_08027118(void);
void sub_08025EA0(void);
/* sub_08028874's SECOND PARAMETER IS `int`, NOT `u8` -- corrected in wave 30
 * from the caller, which is the only place it is visible. sub_08028894 saves
 * both of its own `int` parameters with bare `adds rN, rM, #0`, builds
 * SEPARATE u16-narrowed copies for its sub_08028848 call, and then passes the
 * RAW originals to sub_08028874 with no narrowing at all. A `u8` parameter
 * there emits `lsls #0x18; lsrs #0x18` in front of that `bl`; a `u16` one emits
 * `lsls #0x10; lsrs #0x10`. Neither is in the ROM. The already-promoted
 * definition in src/decomp/c_08028874.c could not see this: its only use of the
 * value is a `strb` into a u8 struct member, which is byte-identical for every
 * width, so the definition is the weaker evidence here. Retyped and re-matched
 * with try_match. */
void sub_08028874(int, int);
void sub_08028894(int, int);
u8 sub_080288D8(u16);
u8 sub_08028904(u16);
/* Wave 38, W38-I. Was undeclared even though src/decomp/c_08028944.c has been
 * promoted since wave 32; sub_08028A68 and sub_08028AEC are the first callers
 * outside its own file. Text copied from that definition. */
bool8 sub_08028944(u16);
u8 sub_08028990(u16);
u8 sub_080289BC(int);
void sub_08028568(void);
void sub_08028168(void);
/* sub_080276D0 / sub_080276F0 are the 0 and 1 halves of one two-line body;
 * sub_08027844 / sub_08027A08 are the 0x10 and 4 halves of another. All four
 * are argument-free and end `pop {r0}` / `pop {r4}; pop {r0}`, i.e. void.
 * sub_08027FBC's second and third parameters are u16: each is used as
 * `lsls #0x10; lsrs #0xc`, which is PROMOTE_MODE's zero-extension FUSED by
 * combine with a `* 0x10` -- three instructions collapsed to two, and a shape
 * an `int` parameter cannot produce. */
void sub_080276D0(void);
void sub_080276F0(void);
void sub_08027844(void);
void sub_08027A08(void);
void sub_08027FBC(void *, u16, u16);
/* The 0x08005xxx menu block. sub_08005838 and sub_080059B4 take THREE
 * arguments and read only the third, which arrives `lsls #0x18; lsrs #0x18`,
 * i.e. a `u8` parameter under PROMOTE_MODE. The first two are dead in both
 * bodies, so `int` is the weakest model for them and not a measurement. */
void sub_08005154(void);
void sub_0800517C(void);
void sub_0800518C(void);
void sub_08005580(void);
void sub_08005838(int, int, u8);
void sub_080059B4(int, int, u8);
void sub_08005D14(void);
void sub_08005EF0(int);
void sub_080145BC(void);
/* The gUnknown_03000050 arena's allocate / free pair, one level below
 * sub_08014E44 / sub_08014ED4. Each takes the arena handle in r0 -- its
 * `!= -1` gate is in the caller, not here -- and the caller's own argument
 * untouched in r1. sub_08014DCC's result is what sub_08014E44 returns, so
 * `void *`; sub_08014ED4 discards sub_08014E68's and ends `pop {r0}`. */
void *sub_08014DCC(int, int);
/* Wave 40 (W40-D): sub_08014E68 RETURNS int, corrected from the body. It sets
 * r0 to 1 on both refusal paths (null pointer, or a header already marked
 * free) and to 0 on the path that actually frees, and its epilogue is
 * `pop {r1}; bx r1` -- the value-returning form this header already reads that
 * way for sub_08014668. Byte-neutral at its one caller: c_08014ED4.c discards
 * the result. */
int sub_08014E68(int, void *);
/* Never declared here before wave 30, though both have matched definitions in
 * src/decomp/ -- these two lines just publish what those files already say
 * (c_08014D7C.c, c_08028848.c), so that sub_08014DA8 and sub_08028894 can call
 * them without an implicit declaration. */
int sub_08014D7C(void *, u32);
void sub_08028848(u16, u16);
/* sub_080281D8 parks its second argument in the slot's +0x18 and sub_08028190
 * is what consumes it -- `ldr r0,[r0,#0x18]`, skip if zero, hand to
 * sub_080196F4(void *). That shared displacement on a sub_080152EC slot is why
 * the parameter is `struct Unk03001470 *` and not a Proc. */
void sub_08028190(struct Unk03001470 *);
/* Already MATCHED as src/decomp/c_0804360C.c and simply never declared here;
 * sub_080276D0 / sub_080276F0 need it. Its argument is the same
 * gUnknown_08090A98 element they have just stored into
 * gUnknown_03003130.unk04 -- r0 still holds it at the `bl`. */
void sub_0804360C(int);
/* Wave 30, W30-B extension work.
 * sub_08022DD4's three parameters are s16. Its own prologue zero-extends the
 * first two (PROMOTE_MODE, which says nothing about signedness) and every use
 * inside re-narrows with `lsls #0x10; asrs #0x10`, including the third, which
 * is the switch selector. Its only caller sub_080230C4 forwards three
 * sign-extended values and nothing else in the ROM sees it.
 * sub_080230DC takes FIVE, and the last two are OUT parameters: `push` saves
 * four registers plus lr, so `ldr r0, [sp, #0x14]` is argument 5, and both it
 * and r3 are written with `strh` and never read. Its THIRD parameter is dead --
 * r2 is overwritten by a pool `ldr` before any read -- but sub_0802323C
 * materialises it, so it is declared. */
void sub_08022DD4(s16, s16, s16);
void sub_080230C4(s16, s16, s16);
void sub_080230DC(s16, s16, s16, s16 *, s16 *);
/* Wave 33, W33-G. Steps the gUnknown_030033E4 cursor by the gUnknown_08499C7C
 * direction entry gpKeySt selects. `pop {r0}` epilogue, so void. */
void sub_0802361C(void);
/* Wave 33, W33-G, both from sub_080211DC's call sites.
 *
 * sub_08042D84 takes the gUnknown_08499594 slot NUMBER -- recovered there as
 * `e - gUnknown_08499594 + 1`, the *5/*17/*257/*65537 shift-add chain plus
 * `rsbs; asrs #8` that is agbcc's exact division by the 0x0c stride -- and that
 * slot's u8 unk00 unit-type id. Its result is added to a small bonus and only
 * then narrowed, so the return is at least `int`.
 *
 * sub_080210C8's third argument is `s16` (the sum is narrowed `lsls #0x10;
 * asrs #0x10` at the call), the fifth is sign-extended from a byte into a whole
 * stack word so it is taken wide, and the sixth is a plain count. Arguments 1,
 * 2 and 4 arrive as bare `ldrb`s. */
/* Wave 33 orchestrator: second parameter is `int`, not `u8` --
 * src/decomp/c_08042C24.c is PROMOTED and defines it that way, and a `u8`
 * declaration makes every caller narrow. Its only promoted caller
 * (src/decomp/c_080433B8.c) re-verified byte-identical after this change. */
int sub_08042D84(int, int);
/* Wave 36 (W36-L): RETYPED from `void sub_080210C8(u8, u8, s16, u8, int, int)`
 * on the CALLEE's own prologue, which is the only side that can show this.
 * Parameters 1-3 are narrowed `lsls #0x10; lsrs #0x10` at entry, not
 * `lsls #0x18; lsrs #0x18` -- halfwords, not bytes -- and each is re-extended
 * `lsls #0x10; asrs #0x10` at every use, which is PROMOTE_MODE's zero-extension
 * at entry plus the signed view at the use, i.e. `s16`. Parameter 4 (the switch
 * discriminator) carries ONE `lsls #0x10; asrs #0x10`, the combine-folded form
 * of the same thing, so it is `s16` and not `u8`. Parameter 5 is zero-extended
 * from a byte at entry and SIGN-extended at its use, so `s8`; parameter 6 is
 * zero-extended once, so `u8`.
 *
 * Byte-neutral for the one promoted caller (src/decomp/c_080211DC.c), which was
 * re-verified: arguments 1, 2 and 4 arrive as `ldrb`s and widen to s16 for
 * free, argument 5 is already that caller's own `s8`, and argument 6 is
 * `(a1 >> 6) + 1` on a u8, whose range agbcc can prove fits. */
void sub_080210C8(s16, s16, s16, s16, s8, int);
void sub_08049FB0(void);
void sub_08049FD4(void);
void sub_08049EB4(void);
void sub_08049B80(void);
/* Wave 30, W30-D. THREE parameters on CALLER-side evidence, which is the only
 * evidence there is: sub_080030BC's own body reads r0 only (r5 = r0, and r1/r2
 * are clobbered by a pool `ldr` before any read), but its sole caller
 * sub_08003088 materialises r1 and r2 from saved registers before each of its
 * four `bl`s. A body that ignores its later arguments is ordinary; the call
 * site is the stronger evidence. void because sub_080030BC tail-calls
 * sub_080032EC and sub_08003088 discards r0. */
void sub_080030BC(int, int, int);
/* Wave 30, W30-D. Five parameters, all typed from sub_080487B4's OWN prologue,
 * which narrows every one of them: r0 and r1 with `lsls #0x18; lsrs #0x18`
 * (u8), r3 and the stack argument at [sp, #0x28] with `lsls #0x10; lsrs #0x10`
 * (u16). r2 is kept whole and used as the base of `adds r1, r7, r1` after the
 * index is scaled `lsls #1`, so it is a halfword pointer -- and sub_0804931C
 * passes gUnknown_08499578, which is already declared `u16 *`. The stack slot
 * is argument five: `push {r4,r5,r6,r7,lr}` + `push {r5,r6,r7}` + `sub sp,#8`
 * is exactly 0x28. Return unused at all three call sites. */
void sub_080487B4(u8, u8, u16 *, u16, u16);
/* Wave 30, W30-D. Three callees of the 0x08075/0x08087 blocks that had no
 * declaration. sub_08085F40 and sub_0803D960 are already PROMOTED
 * (src/decomp/c_08085F40.c, src/decomp/c_0803D960.c) and these two lines just
 * publish the signatures those definitions already have -- sub_0803D960's
 * parameter is the Proc_StartBlocking parent it forwards.
 * sub_08075904 takes an index it scales by 0x30 (`lsls #1; adds; lsls #4`,
 * i.e. a 3<<4-byte record) into gUnknown_08615194 + 0xc, and returns: one arm
 * is a bare `movs r0, #0`. Its two callers both discard the result, so `int`
 * is the widest thing the body supports and nothing narrows it. */
int sub_08075904(int);
void sub_080879A0(void);
void sub_08085F40(void);
void sub_0803D960(ProcPtr);

/* ---- Wave 31, W31-A ---------------------------------------------------- */

/* Read off their own prologues and epilogues. Each is called from this wave's
 * batch with no argument register set up and its result discarded, so the CALL
 * SITES prove nothing about arity -- the void-ness below comes from the
 * callees' `pop {r0}; bx r0` (or bare `bx lr`) and, where the body was read,
 * from no argument register being live on entry. */
void sub_0802C2B4(void);   /* pop {r0}; bx r0; body reads no argument register */
void sub_0803B3F8(void);   /* one-call forwarder to sub_0806FD98 */
void sub_0803B408(void);   /* one-call forwarder to sub_0807046C */
void sub_08011FF0(void);   /* 424-byte DMA-queue drain; nothing read from r0-r3 */
void sub_0802E920(void);   /* matched by this wave */
void sub_0802E960(void);   /* type fixed by contract: it is handed to
                            * sub_080366C4, which takes `void (*)(void)` */

/* ARITY NOT PROVEN. sub_08000664 and sub_08000DC0 call these four with nothing
 * in r0-r3 and discard the result, so `void (void)` is the weakest declaration
 * that fits THIS caller and nothing more. Re-derive from the callee body before
 * relying on it. */
void sub_0800057C(void);
void sub_08002EC8(void);
void sub_080035C8(void);
void sub_08003640(void);

/* u8 return, not int: sub_0802E6C0 and sub_0802E278 re-narrow the result with
 * `lsls #0x18` before testing it, which agbcc emits only for a sub-word return
 * type. sub_08034F60's body is one `ldrb`; sub_0802DBF8's returns 0 or 1. */
u8 sub_08034F60(void);
u8 sub_0802DBF8(void);

/* Defined in src/decomp/c_0802E4B4.c -- declared here for sub_0802E6C0.
 * RETYPED u16 -> s16, wave 36 (W36-M). sub_0802DCB4 reaches
 * gUnknown_030033E4's two halves as s16 OBJECTS (`movs rI,#0; ldrsh`) at this
 * call site, and a u16 PARAMETER makes that impossible: agbcc's call-site
 * promotion follows the parameter's signedness, so a u16 parameter emits
 * `ldrh` no matter how the argument is spelled -- the sign-extending load only
 * survives into a signed parameter. Byte-neutral on both the definition
 * (PROMOTE_MODE zero-extends s16 and u16 alike at entry, and the body's first
 * act is `sx = x` into an s16 local) and on the other caller sub_0802E698,
 * which passes u8 fields with no narrowing either way; both re-verified. */
void sub_0802E4B4(s16, s16);

/* SIGNED parameters, and this is a RETYPE of what src/decomp/c_08022AAC.c
 * carried. Wave 31, W31-A: that definition guessed `u16` from its own body,
 * which cannot tell -- agbcc's PROMOTE_MODE narrows a parameter of EITHER
 * signedness with `lsls #0x10; lsrs #0x10`, and the only uses are `strh` and
 * `<< 4`, both sign-blind. The first two C callers (sub_0802E698 and
 * sub_0802E6F8, this wave) DO discriminate: they read
 * gUnknown_03003100.spos.unk00/.unk02 with `ldrsh` and pass them straight
 * through. A u16 parameter would have forced a zero-extending `lsls/lsrs` pair
 * at each call site that the ROM does not have. Re-verified byte-for-byte
 * against sub_08022AAC after the change. */
void sub_08022AAC(s16, s16);

/* The BIOS IntrWait(1, 1) stub: `movs r2,#0; svc #5; bx lr`. */
void VBlankIntrWait(void);

/* Eight parameters, all read off its own prologue: r0 is untouched (it is
 * `strh`-ed through at +0/+2/+0x40/+0x42, a 2x2 halfword tilemap block, so
 * `u16 *`), r1 `lsls/lsrs #0x18`, r2 `#0x10`, r3 `#0x18`, then the four stack
 * arguments at [sp,#0x34..0x40] narrow to u8, u16, u16, u8 in that order. */
void sub_0802216C(u16 *, u8, u16, u8, u8, u16, u16, u8);

/* s16 return: every exit is an `ldrsh`, a `-1`, or an `asrs #0x10`, and
 * sub_08007DB0 re-narrows the result with `lsls #0x10; asrs #0x10` before
 * testing it >= 0 -- the signed pair, which only a signed narrow return type
 * produces. */
s16 sub_08007DD0(int, int);

/* Both matched by this wave. sub_080016D0 RETURNS sub_08001704's result: its
 * epilogue is `pop {r1}; bx r1`, which leaves r0 alone, where a void function
 * pops the return address into r0 itself. */
int sub_080016D0(int, int);
void sub_08007D70(int, int);

/* ---- Wave 31, W31-A, second batch ------------------------------------- */

/* One parameter, and it is NOT visible from either call site -- sub_080293A0
 * forwards its own r0 without touching it. The callee's body is what settles
 * it: `adds r1, r0, #0; ldr r0, =gUnknown_08499FEC; bl Proc_StartBlocking`, so
 * the incoming r0 is the parent proc. */
void sub_08028ED0(ProcPtr);

/* `ldr r0, =gUnknown_030040A8; ldr r0, [r0]; bx lr` -- a word-wide getter.
 * NOT u16: sub_08029234 `strh`s the result with no re-narrowing, which a
 * sub-word return type would have forced. `u32` rather than `int` to agree
 * with src/decomp/c_0804138C.c, promoted in this same wave -- the width is what
 * the caller proves and the signedness is unconstrained either way. */
u32 sub_08041398(void);

/* Two int parameters: neither is narrowed on entry, and both are scaled `<< 4`
 * before being stored as words. sub_08029C28 passes two `ldrh` values, which a
 * wider parameter takes for free. */
void sub_0802723C(int, int);

/* Read off its own prologue: r0 and r1 arrive untouched (they are sign-extended
 * at the sub_08022AAC call, which is a cast at a use), and r2 narrows
 * `lsls/lsrs #0x18`. `pop {r0}` makes it void. sub_08029088 and sub_0802909C
 * are the same call with 0 and 1 for that last byte. */
void sub_080290B0(int, int, u8);

/* One struct-pointer parameter -- sub_08030C98 calls it with
 * `&gUnknown_030040C0` still sitting in r0 from the two stores above the call,
 * and the body reads +0, +6 and +8 through it. */
void sub_0802EA5C(struct Unk030040C0 *);

/* Defined in src/decomp/c_0802EAFC.c; declared here for sub_08030C98. */
void sub_0802EAFC(void);

/* Returns a value (`pop {r1}; bx r1`) and reads no argument register. The
 * result is either -1 or gUnknown_0300055C's word, so `int`; sub_08030D1C
 * `strb`s it into a volatile s8 with no narrowing, which is the truncating
 * store and not evidence of a narrow return. */
int sub_0802EB28(void);

/* ARITY NOT PROVEN beyond "reads no argument register before writing it": its
 * first instructions load r0 from a pool word. 1,556 bytes, so only the
 * prologue was read. */
void sub_08046030(void);

/* ARITY AND WIDTHS NOT PROVEN -- every one of these is declared from its CALL
 * SITE only, and the call sites in this batch cannot see past what they load.
 * Each declaration below is the weakest one that reproduces the ROM bytes at
 * the caller; re-derive from the callee body before relying on any of them.
 *
 * The three void ones are called with nothing in r0-r3 and their results
 * discarded, so `void (void)` is byte-identical to any other shape here.
 * sub_08001DAC is 1,260 bytes and was deliberately NOT read. */
void sub_08001DAC(void);
void sub_08002AB0(void);
void sub_08002C38(void);
void sub_0801F1EC(int, int);      /* (0xAA, 0xAA or 0xAB) from sub_08002EF8 */
/* RETYPED `int` -> `u16` in wave 35 (W35-C), from the CALLEE's own body, which
 * is the evidence this declaration never had -- the note above says each of
 * these is "the weakest one that reproduces the ROM bytes at the caller" and
 * asks for exactly this re-derivation. sub_080247A4 opens
 * `lsls r0,r0,#0x10; lsrs r5,r0,#0x10`, the u16-parameter tell: the promoted
 * entry value IS the truncated one, it feeds the `muls #0x5c` row index
 * directly, and the shifted form left in r0 is reused for the 0xb4..0xbf range
 * test (`adds r0,r0,#0xFF4C0000; lsrs #0x10`) with the constant pre-shifted --
 * which only happens when the parameter's own mode is HImode. Caller-neutral:
 * the one caller passes a zero-extended `ldrb`, which needs no narrowing for
 * either type, so no promoted file changes. */
void sub_080247A4(u16);           /* one `ldrb` from a byte table */
void sub_080860DC(ProcPtr);       /* sub_0808603C hands it the same proc it
                                   * gives sub_08086688, which IS declared
                                   * ProcPtr -- that is the whole argument */
/* Wave 43 (W43-D) CONFIRMED `int`, do not "fix" it to u16. The body copies the
 * second argument with a bare `adds r6, r1, #0` and never narrows it: a u16
 * parameter would arrive with PROMOTE_MODE's `lsls #0x10; lsrs #0x10` instead,
 * which is +2 bytes and was measured as the function's only diff.
 * (gUnknown_0849957C, 0x6200) */
void sub_08037A20(u16 *, int);
void sub_080620C0(void);
void sub_080620FC(int, int);      /* (0,1) (1,6) (2,5) from sub_0806209C */
/* Matched in src/decomp/c_0806209C.c as `void sub_0806209C(void)`; this
 * declaration only makes it visible to its one caller, sub_08062038. */
void sub_0806209C(void);
/* Wave 48 (W48-D).  `pop {r0}; bx r0` in the epilogue, so void: the return
 * register is destroyed restoring lr.  Rebuilds the gUnknown_03003F20 cell
 * list from gUnknown_084995A0 and takes nothing. */
void sub_08062330(void);
/* Wave 48 (W48-D), MATCHED.  Two OUT-parameters, both `int *` from the `str`
 * at each -- not u8 *, though the values stored are u8 map coordinates.
 * Returns 1 when it found a cell and 0 when it did not (`movs r0,#1` / `movs
 * r0,#0` across a `b`, the two-arm form). */
int sub_080623C4(int *, int *);
/* Wave 48 (W48-D).  void: `pop {r0}; bx r0`, and it reads no argument
 * register before writing it. */
void sub_08062474(void);
/* Wave 48 (W48-D), from the CALL SITE in sub_08062474 only -- not matched, so
 * this is the weakest contract that fits, NOT ground truth.  Two arguments:
 * r0 is a small literal 1..4 (`movs r0,#N`) and r1 is the u8
 * gUnknown_085D5ABC[..].unk1d passed with no narrowing in front of it, which
 * an int parameter and a u8 parameter both explain.  Result unused at the
 * only call site, and there are four of them. */
/* Wave 50 (W50-A), RETYPED from (int, int).  Both parameters are narrowed by
 * PROMOTE_MODE at entry before anything else happens -- `lsls r0,#0x10;
 * lsrs r0,#0x10; str r0,[sp,#4]` and `lsls r1,#0x18; lsrs r1,#0x18;
 * str r1,[sp,#8]` -- which is the sub_080247A4 readout above: the spilled
 * value IS the truncated one, so the parameters' own modes are HImode and
 * QImode.  Caller-neutral: the one caller (sub_08062474) is still assembly. */
void sub_08062560(u16, u8);
/* Wave 50 (W50-A), RETYPED from int.  Same readout: `lsls r0,#0x18;
 * lsrs r0,#0x18; str r0,[sp,#4]` at entry and the spilled value is the
 * truncated one.  Caller-neutral: src/decomp/c_08062C7C.c passes its own u8
 * parameter, which needs no narrowing for either type. */
void sub_080627F4(u8);
void sub_08062AE4(void);
/* Wave 50 (W50-A).  void: `pop {r0}; bx r0` destroys the return register, and
 * it reads no argument register before writing it. */
void sub_08062C94(void);
/* Wave 50 (W50-A).  Types copied from the already-promoted definition in
 * src/decomp/c_08062730.c -- NOT re-derived.  sub_08062560's call site hands it
 * `gUnknown_030040D8` (declared `struct Unk030040D8 *`) as the first argument
 * and casts; the result is tested `lsls #0x18` at that site, so the caller
 * narrows the `int` return to u8 itself. */
int sub_08062730(struct UnitRecord *, struct UnitRecord *);
void sub_08077620(int, int);      /* (0, 0xA8 - gUnknown_0300064C), twice */
void sub_080758BC(int, int, int, ProcPtr);
                                  /* FOUR arguments: sub_08077E9C sets r0, r1
                                   * and r2 and leaves its own r0 sitting in r3
                                   * from `adds r3, r0, #0` at the top -- the
                                   * fourth argument is invisible except as
                                   * that copy */
/* CORRECTION, wave 32 (W32-A): it RETURNS a value. Wave 31 declared this
 * `void` from the call site, where sub_08077790 discards the result -- which is
 * no evidence either way. The body settles it: `adds r0, r7, #0` immediately
 * before an epilogue that pops the return address into r1 (`pop {r1}; bx r1`)
 * and leaves r0 intact. A void function pops into r0 itself and clobbers it.
 * The value is the seventh argument narrowed to a halfword, so it is already in
 * u16 range and nothing re-narrows it; `int` is the weakest type that fits and
 * the width is otherwise unconstrained.
 * The eight PARAMETER types are unchanged and were re-checked: the prologue
 * copies every one with a bare `adds` and each narrowing is at its use. Making
 * the last three `u16` was TRIED and is 8 bytes WORSE -- agbcc then needs a
 * third high register (sl) and pushes it, because a narrow parameter's
 * PROMOTE_MODE copy lives from the prologue to its last use. `int` with the
 * casts at the uses is right. */
int sub_08077214(u16 *, int, int, int, int, int, int, int);
/* Wave 32, W32-A: sub_08077620's other callee, still assembly. Four `int`s --
 * every one arrives as a bare `adds rN, rM, #0` with no PROMOTE_MODE narrowing
 * -- and void: `pop {r0}; bx r0`. It masks the first two into OBJ x/y fields
 * and forwards to PutSpriteExt with its own gUnknown_084A07DA object list. */
void sub_0804402C(int, int, int, int);

/* Matched by this wave. sub_080771C0 and sub_080771F0 hand each other's ADDRESS
 * to sub_080638D0, so each needs the other declared; sub_08002EF8 is called by
 * sub_08002E3C in the same block. */
void sub_080771C0(void);
void sub_080771F0(void);
void sub_08002EF8(void);
                                  /* EIGHT, four of them on the stack. Widths
                                   * are wide open: the ROM passes literals and
                                   * one `ldrh`, and every narrower parameter
                                   * type accepts those for free. */

/* ---- wave 33 (W33-C): the 0x08063 / 0x08064 address-locality blocks ---- */

/* Splits a value into three decimal digits written back through three separate
 * u8 pointers -- ones, tens, hundreds in that argument order reversed (the
 * ones pointer is the LAST parameter). `strb` at every store site pins u8; the
 * value is used whole by __divsi3/__modsi3 with no narrowing, so `int`.
 * 0xFF is its "blank this digit" code and 10 is what all three get for a zero
 * input. Matched in wave 33, src/decomp/c_08063A58.c. */
void sub_08063A58(int, u8 *, u8 *, u8 *);
/* sub_08063B50's only callee and its only caller. Five arguments, the fifth
 * pushed with `str r4, [sp]`. The first three arrive as full words straight out
 * of `ldr` on +0x24/+0x28/+0x2c of struct Unk8063BE0; the fourth is an `ldrb`
 * and the fifth an `ldrh`, both of which widen into `int` for free, so `int`
 * five times is the weakest model that fits every site. Its own body (272 B,
 * Div/SetObjAffine/gSinLut) is still asm and may narrow these later. */
void sub_08063CCC(int, int, int, int, int);

/* ---- wave 46 (W46-B): the 0x08064 block -------------------------------- */

/* CORRECTION TO THE WAVE-46 BRIEF, and it cuts BOTH ways -- record both halves,
 * because the first half alone is the dangerous reading.
 *
 * The brief said block 0x08064000 has ZERO undeclared callees and that no new
 * prototype would be needed. Grepping include/ for the five names returns
 * nothing, which reads exactly like "undeclared" -- and acting on that reading
 * is how this nearly broke the build. All five are already DEFINED, and matched,
 * in src/decomp/ (c_08063DDC.c, c_08063E28.c, c_08063FEC.c, c_08064034.c); they
 * are simply not declared in any shared header. So the brief was right that
 * nothing needed deriving and wrong that nothing needed writing.
 *
 * GREP src/decomp/ AND NOT ONLY include/. `trymatch` compiles ONE unit and
 * cannot see a prototype that disagrees with a promoted definition in another
 * unit, so a wrong signature here passes every per-function check and breaks
 * only the SPLIT build -- the wave-14 failure, repeated. The first draft of
 * this block declared all five as `int *` from the assembly alone and matched
 * sub_08064214 byte-for-byte with it.
 *
 * sub_08063FEC, sub_08064034 and sub_0806407C each fill one 0x30-byte matrix
 * (twelve words) from an angle. sub_08064214 gives them three consecutive 0x30
 * slots of its own frame and an `(s16)` cast of each of its three parameters --
 * `adds rN, r0, #0` then `lsls #0x10; asrs #0x10`, which is copy-then-narrow,
 * so its OWN parameters are int and the cast is at the use. The angle argument
 * is declared int for the same reason sub_0801BA4C's is: those two build
 * rotation matrices by handing it straight to sub_0801BA4C / sub_0801BAA8.
 *
 * sub_08063E28(a, b, out) composes two matrices. sub_08064214 calls it twice
 * with out == b, which is why the destination is a third argument and not a
 * return value.
 *
 * sub_08063DDC(v, m, dst) transforms one three-word vector by the rotation part
 * of one matrix; sub_08064214 passes a gUnknown_0202F140 entry's two vectors
 * and the composed matrix.
 *
 * EVERY SIGNATURE BELOW IS COPIED FROM THE PROMOTED DEFINITION, not derived
 * from the call site -- the definitions win. Note that sub_08064034 and
 * sub_0806407C take their own per-function matrix tags rather than
 * `struct Mtx43`: c_08064034.c re-declared the identical 4x3 layout under
 * separate tags so promote.py could merge that draft into a unit that already
 * defined Mtx43, and a declaration here has to agree with what is written
 * there. The angle is `s16` at all three matrix builders for the same reason. */
/* sub_080633E4 is likewise DEFINED in src/decomp/c_080633E4.c and merely never
 * declared in a shared header -- see the correction above. Its signature is
 * that file's, `(struct Unk08062FB8 *, u16)` returning int, NOT the
 * `(..., int)` the call site alone suggests. sub_08063528 tail-returns its
 * result from three separate sites (jump.c cross-jumps the three into one
 * `bl`). The second argument arrives either as a bare `movs r1, #0` or as a
 * `ldrh` of the link record's +0x00, which is a (u16) cast at the call and not
 * a narrow
 * parameter -- `int` takes both without a narrowing anywhere. */
int sub_080633E4(struct Unk08062FB8 *, u16);

/* THE TAGS MUST BE FORWARD-DECLARED AT FILE SCOPE FIRST. A tag that appears for
 * the first time inside a prototype has PROTOTYPE scope in C89, so the later
 * file-scope definition in src/decomp/ would be a DIFFERENT type and every one
 * of these would become an incompatible redeclaration. These four lines are
 * what make the declarations agree with the definitions. */
struct Vec3;
struct Mtx43;
struct Unk64034Mtx;
struct Unk6407CMtx;

void sub_08063FEC(struct Mtx43 *, s16);
void sub_08064034(struct Unk64034Mtx *, s16);
void sub_0806407C(struct Unk6407CMtx *, s16);
void sub_08063E28(struct Mtx43 *, struct Mtx43 *, struct Mtx43 *);
void sub_08063DDC(struct Vec3 *, struct Mtx43 *, struct Vec3 *);

/* ---- wave 33 (W33-C): the 0x08085 address-locality block ---- */

/* The twenty-row unit-info table builder sub_08085244 calls. TWO parameters,
 * and the first one is UNUSED inside the body -- its prologue is `mov sb, r1`
 * with r0 written (`movs r0,#0`) before it is ever read. The call site is what
 * proves the arity: sub_08085244 sets up r0 (its own `s16 *`) and r1
 * (`p[0x33]`, the army index every one of the callees below takes) immediately
 * before the `bl`. r1 is used unnarrowed as a gUnknown_08499598 index and as
 * sub_08085410 / sub_08085638 / sub_080856A0's first argument, so `int`.
 * `pop {r4,r5,r6,r7}; pop {r0}` makes it void. */
void sub_08085708(s16 *, int);
/* Already MATCHED (32 bytes, never promoted or declared): returns
 * `&gUnknown_0810E6E0[(gUnknown_08499598[i].unk1a - 1) * 0x20]`, the stride-0x20
 * palette row that table's note describes. `u16 *` rather than `u8 *` because
 * its only caller, sub_08085950, hands the result straight to
 * ApplyPaletteExt's `u16 *` first parameter with no arithmetic in between; the
 * index is used as a full word (`lsls r1,r0,#4; subs r1,r1,r0`) so `int`. */
/* Wave 33 orchestrator: `u8 *`, not `u16 *` -- src/decomp/c_080261C8.c is
 * PROMOTED and computes `u8 *base + (tbl[i].unk1a - 1) * 0x20` in bytes. Its one
 * caller passes the result straight to ApplyPaletteExt with no indexing, so the
 * element width is byte-neutral there and the definition is the only evidence. */
u8 *sub_080261C8(int);

/* ---- wave 33 (W33-C): the 0x0804C address-locality block ---- */

/* THREE parameters, all narrowed `lsls #0x10; lsrs #0x10` in its own prologue.
 * The third is s16 rather than u16: it is spilled whole and then re-widened
 * with `lsls #0x10; asrs #0x10` before being compared against -1, which is the
 * sign-extend-at-the-use half of a declared s16 (PROMOTE_MODE zero-extends both
 * at entry, so the prologue does not discriminate). Every one of its five
 * callers passes gUnknown_03001FBC there, which is s16 too. Void epilogue. */
void sub_0804CA98(u16, u16, s16);
/* Already DEFINED and matched in src/decomp/c_0804BDD8.c but never declared;
 * sub_0804CA44 is its first C caller. Published verbatim from that file. */
int sub_0804BECC(u16, u16, s16);

/* ---- wave 33 (W33-C): the 0x08050 address-locality block ---- */

/* The song-stop half of the gUnknown_0824238C / gUnknown_08242308 pair those
 * globals' note describes; sub_080504A8 calls it twice with bare `movs`
 * immediates (0x3b, 0x3c). u16 song index, matching the `lsls #0x10; lsrs #0xd`
 * stride-8 scaling that note records for the same table. */
void sub_08070544(u16);

/* ---- wave 33 (W33-F): blocks 0x08002 and 0x08057 ---- */

/* sub_08002964 hands it that function's fourth argument raw (`adds r0,r5,#0`)
 * and discards the result, so `int` is the weakest fit and nothing in this
 * wave can see the return type. */
/* Wave 33 orchestrator: returns `int`, not `void`. W33-A matched the body
 * byte-for-byte as `int sub_08001230(int)`; W33-F declared it `void` from its
 * call site in sub_08002964, which discards the result and so cannot see the
 * return type at all. The definition wins; the call site is byte-identical
 * either way. This was the wave's one flagged cross-agent collision. */
int sub_08001230(int);
/* The two OBJ-graphics lookups behind sub_08002964 and sub_080029F4: the
 * returned word becomes sub_08011E54's `void *` source with no arithmetic and
 * no narrowing in between, which is the only evidence either way. */
/* Wave 33 orchestrator: `const u8 *`, not `void *` -- src/decomp/c_0802A838.c
 * is PROMOTED and defines both this way. Weakest-model applies to types nobody
 * has named; here the definition names one and proto_check rejects the
 * disagreement. Callers take the result as `const u8 *` or cast. */
const u8 *sub_0802A85C(int);
const u8 *sub_0802A838(int);
/* sub_08002DEC calls it four times and feeds each result straight into
 * ApplyPaletteExt's `u16 *` first parameter, so that fixes the return type the
 * same way sub_08082660's does. Both arguments are bare `movs` immediates at
 * every site, so their widths are not visible -- `int` is the weakest fit. */
/* Wave 33 orchestrator: `const u8 *`, not `u16 *` -- src/decomp/c_0802A880.c is
 * PROMOTED and defines it that way. */
const u8 *sub_0802A8AC(int, int);
/* sub_08057048 fills a six-halfword stack record and passes its address; the
 * record's layout is described in that function's own file. */
void sub_080570C4(void *);
/* Wave 48, W48-G. PROMOTED in src/decomp/c_08057110.c with exactly this
 * signature: (chr, offset, pal, flip) -> gUnknown_08551A00[offset]. */
void sub_08057110(u16, u16, u16, u16);
/* Wave 48, W48-G. The gUnknown_08551A04 twin of sub_08057110, PROMOTED in
 * src/decomp/c_0805701C.c with exactly this signature. */
void sub_0805701C(u16, u16, u16, u16);
/* Wave 48, W48-G. The four sprite-row painters sub_080579B8 fans out to, all
 * called with the identical (u16 *dst, int idx, &pos) triple. sub_080576D4 and
 * sub_08057A24 are PROMOTED with exactly this signature; the third argument's
 * pointee is the two-halfword {u16 x; u16 y;} record those files spell locally,
 * so the tag is only forward-declared here and each .c completes it.
 * sub_080577E4/sub_08057860 are not matched yet -- their third argument is
 * assumed the same record because sub_080579B8 hands all four the same stack
 * slot, which is evidence about the CALL, not about their bodies. */
struct Unk8057Pos;
void sub_080576D4(u16 *, int, struct Unk8057Pos *);
void sub_0805772C(u16 *, int, struct Unk8057Pos *);
void sub_080577E4(u16 *, int, struct Unk8057Pos *);
void sub_08057860(u16 *, int, struct Unk8057Pos *);
void sub_08057A24(u16 *, int, struct Unk8057Pos *);
/* Wave 48, W48-G. The two per-side fan-outs over the four painters above, both
 * MATCHED with this signature. sub_08057AE8 hands each `gUnknown_08499578`,
 * which is already a `u16 *`. */
void sub_080579B8(u16 *);
void sub_08057A80(u16 *);
/* Allocates a slot or fails: sub_0801D6E8 calls it when its own fifth argument
 * is above 0x1f, compares the result against -1 and RETURNS IT UNCHANGED on
 * that path (`adds r0,r3,#0`, not a rematerialised -1), which is what makes the
 * return an `int` rather than something narrower. */
int sub_0801E13C(void);


/* ---- wave 34 (W34-D): block 0x08027 ---- */
/* The two halves of one clamp pair on gUnknown_03003130.unk04, and they are
 * their own block's undeclared callees. Both take nothing and return nothing:
 * zero argument registers are read before being written and both end on the
 * `pop {r0}; bx r0` void epilogue. sub_080275B4 walks unk04 down toward 3 and
 * sub_08027608 walks it up toward 0xad -- the two bounds gUnknown_08090A98
 * holds as its [0] and [1]. */
void sub_080275B4(void);
void sub_08027608(void);
/* Both are `void (void)` proc/queue callbacks whose ADDRESS is taken by
 * sub_0802776C's switch and handed to sub_0801F024. */
void sub_08027658(void);
void sub_08027710(void);
/* sub_08027984's argument arrives `movs r1,#0; ldrsh r0,[r5,r1]` off the s16
 * gUnknown_03001FBC and its result is discarded. `int` and NOT `s16`: the
 * ldrsh is the global's own width, so the call site cannot see the parameter's
 * -- and src/decomp/c_08016944.c already promotes the body as `int`. Declared
 * s16 first this wave and proto_check caught it; sub_08027984 is byte-identical
 * either way, which is exactly why the definition has to be the tie-break. */
void sub_08016974(int);


/* ---- wave 34 (W34-D): block 0x08025 ---- */
/* THREE parameters, not one, and src/decomp/c_08026588.c was corrected to
 * match. Its promoted body uses only the first, which is why the one-parameter
 * form survived -- an unused register parameter costs no instructions, so the
 * definition is byte-identical either way and had no oracle. sub_080250E8 is
 * the differently-shaped caller that finally shows it: it sets r0, r1 AND r2
 * before each of its two `bl`s, and r0/r1 are each the result of a full
 * twenty-instruction pointer-difference-by-12 divide. Nobody computes that to
 * pass a dead argument. Both are army numbers -- `(p - gUnknown_08499594) / 64
 * + 1` truncated to u8 -- and r2 is a `ldrb` of the record's unk00. */
/* Takes the record and a displayed-HP value (`Div(hp - 1, 10) + 1`, or 0 when
 * the 7-bit unk04_0 field is empty). The second parameter is `int`: sub_08025D60
 * passes that expression with no narrowing at all, while sub_0802505C's u16
 * local is narrowed on its own account before the call. Result unused at both
 * sites. */
/* Promoted in src/decomp/c_08025D20.c; declared here so sub_08025D60 (its
 * caller, a different unit) can see them. */
void sub_08025D20(int);
void sub_08025D40(int);
void sub_08026588(u8, u8, u8);
/* Promoted in src/decomp/c_08042C24.c as `int sub_08042C68(int, int)`; declared
 * here so sub_080253B0 can see it. */
int sub_08042C68(int, int);
void sub_08025B24(struct UnitRecord *, int);
/* One record pointer, result unused (sub_08025D60). */
void sub_0802A5C4(struct UnitRecord *);
/* Wave 39, W39-F. The 0x0802A3FC unit-scan block.
 *
 * sub_0802A304 is a sub_0802A38C callback, the sibling of sub_0802A2E4, and
 * both are typed `void *` because the object -- a struct UnitRecord record --
 * is reached through a file-local view: sub_0802A304 indexes unk07/unk08 as a
 * 2-element cargo array, which the shared struct cannot express and which is
 * not worth reshaping it for (see the note on struct UnitRecord.unk07).
 *
 * sub_0802A38C and sub_0802A258 are DELIBERATELY NOT DECLARED HERE.
 * src/decomp/c_0802A38C.c defines sub_0802A38C with a file-local `struct
 * Unk2A38C *` parameter, so any declaration in this header is a conflicting
 * type for that unit; the callers declare it themselves as
 * `bool8 sub_0802A38C(void *, int (*)(void *))`, which is the weakest
 * spelling that agrees with both call sites. The work/ draft for sub_0802A258
 * is in the same position.
 *
 * ONE FINDING TO CARRY FORWARD: work/sub_0802A258 declares itself `bool8`,
 * and that is REFUTED by its only call site. sub_0802A3FC truth-tests the
 * result with `lsls r0, #0x10`, which is a 16-bit return; a bool8/u8 return
 * gives `lsls #0x18`. The callee's own body returns only the literals 0 and 1
 * and so cannot tell the two apart -- exactly the situation the note on
 * sub_0802A1E4 above describes for that function. u16 vs s16 is still open;
 * nothing narrows or sign-extends it. Whoever promotes sub_0802A258 should
 * make it 16-bit. */
int sub_0802A2E4(void *); /* promoted in src/decomp/c_0802A2E4.c */
int sub_0802A304(void *);
void sub_0802A3FC(void);
void sub_0802A6B0(void);
/* Promoted in src/decomp/c_08025B28.c; declared here so other units can call
 * it. */
void sub_08025B28(u16, u32);
/* The gUnknown_085D5ABC[type].unk14 permission-table predicates. sub_08025F74
 * takes the record directly and sub_08025EF0 the two unit ids -- same three
 * tests, and sub_08025EF0 additionally requires the two ids to share an army
 * (`(a & 0xc0) == (b & 0xc0)`). Both return a byte. */
bool8 sub_08025F74(struct UnitRecord *, u8);
bool8 sub_08025EF0(int, int);
bool8 sub_08025FC0(struct UnitRecord *, struct UnitRecord *);
bool8 sub_080253B0(struct UnitRecord *);

/* Wave 34, W34-K -- block 0x08069 / 0x0806B. Every signature below is read off
 * the CALL SITE, per wave 30; none of these had a declaration anywhere.
 *
 * sub_080679D8 takes TEN arguments: four in registers plus six stack slots,
 * which is what the `sub sp, #0x18` in sub_08069154 is reserving. All ten are
 * plain words at the one call site (1, -1, 0x170, 0x88, -0x3800, 0, 0xc0,
 * 0x100, 0xc, proc), and the second and fifth are negative, so `int`. */
void sub_080679D8(int, int, int, int, int, int, int, int, int, ProcPtr);
/* Wave 51, W51-H. The 0x08067xxx proc helpers the three 0x08069 timeline
 * functions drive. ARITY READ OFF EACH CALLEE'S OWN PROLOGUE, not off a call
 * site: sub_08067BD0 and sub_08067D04 each copy r0/r1/r2 into callee-saved
 * registers and hand r3 to Proc_Start as the parent, which is four; the four
 * `void` ones write no argument register before their first `bl`.
 *   sub_08067BD0's second parameter is SIGNED -- sub_080691BC passes -1 there
 * (`movs r1,#1; rsbs r1,r1,#0`) while its siblings pass 1 -- and the callee
 * feeds it to `lsls r0, r6, #7`. The first and third are `int` for symmetry,
 * not from evidence. */
void sub_080677E8(void);
void sub_0806780C(void);
void sub_08067A24(void);
void sub_08067BD0(int, int, int, ProcPtr);
void sub_08067C7C(u32); /* `u32`, not `int`: c_08067C7C.c already DEFINES it
                         * that way and a promoted definition wins over a
                         * declaration. Byte-identical at both call sites. */
void sub_08067D04(int, int, int, ProcPtr);
void sub_08067D4C(void);
void sub_08067DD4(ProcPtr);
/* sub_08067ED0 takes NINE. Its first parameter is `u8` and its second `u16`:
 * sub_08069DE8 narrows the first with `lsl #0x18; lsr #0x18` on the one path
 * where the value is not a constant zero, and loads the second with `ldrh`
 * from a member it reads with a full `ldr` at the very next call -- so the
 * narrowing is the parameter's, not the member's.
 *
 * Parameters 7 and 8 are `u8`, taken from the PROMOTED definition in
 * src/decomp/c_08067ED0.c rather than from the call site. sub_08069DE8 passes
 * small constants and a value it has just compared against zero, so `int` was
 * byte-identical there and the call site could not discriminate -- the
 * definition is the only evidence, and it wins. */
void sub_08067ED0(u8, u16, int, int, int, int, u8, u8, ProcPtr);
/* Five arguments, the fifth on the stack.
 *
 * Wave 36, W36-K: RETYPED from `(int, int, int, int, ProcPtr)`. The note that
 * used to sit here said "BOTH arrive as full words", read off sub_08069DE8's
 * call site -- but that call passes the CONSTANTS 0x280 and 3, which narrow at
 * compile time and so show nothing either way. The callee's own prologue is
 * decisive: sub_080686E8 opens `lsls r5,#0x10; lsrs r5,#0x10` on r2 and
 * `lsls r4,#0x18; lsrs r4,#0x18` on r3, i.e. PROMOTE_MODE zero-extension of a
 * u16 and a u8 parameter, before any use. Retyping is byte-neutral at
 * sub_08069DE8 (both arguments are literals there) and is what matches the
 * definition. Same shape as sub_08068810 below. */
void sub_080686E8(int, int, u16, u8, ProcPtr);
/* Wave 36, W36-K. The graphics loader both sub_080686E8 and sub_08068810 hand
 * a `gUnknown_08610A38[]` blob to. First parameter is `u8 *`: the body walks it
 * with `ldrb [p]` and `ldrb [p + n]` as a 0xff-terminated id list. Second is
 * u16 (`lsls #0x10; lsrs #0x10` at entry, then `lsls #5` for a 0x20-byte VRAM
 * stride). Returns the loop counter held in ip -- an `int` count, not a u8;
 * both callers only `strb` it, which is byte-neutral either way, so `int` is
 * the weaker fit that costs no caller a re-narrowing. */
int sub_08068038(u8 *, u16);
/* Wave 36, W36-K. sub_080686E8's seven-argument sibling: same proc setup, same
 * gUnknown_085D3DD0 -> gUnknown_08610A38 -> sub_08068038 graphics chain, but it
 * fills four more proc bytes (+0x38, +0x39, +0x4e, +0x4f) instead of +0x4d.
 * Arguments 5 and 6 narrow at entry exactly as sub_080686E8's 3 and 4 do. */
void sub_08068810(int, int, int, int, u16, u8, ProcPtr);
/* Wave 36, W36-K. Both are already PROMOTED and byte-verified -- the signatures
 * below are copied verbatim from src/decomp/c_080673D0.c and
 * src/decomp/c_080678BC.c, which had no declaration anywhere, so sub_08068A00
 * could not see them. */
void sub_080673D0(u32, u32, ProcPtr);
void sub_080678D4(u32);
/* Wave 36, W36-K. Four more already-PROMOTED, previously undeclared functions
 * sub_080699E8 needs; signatures copied verbatim from src/decomp/c_0806978C.c,
 * src/decomp/c_080697BC.c and src/decomp/c_080697CC.c. */
void sub_0806978C(void);
void sub_080697A4(void);
void sub_080697BC(void);
void sub_08069924(u8);
/* Wave 36, W36-K. Still asm. One argument, materialised at the call site
 * (`adds r0, r4, #0` on sub_080699E8's own proc immediately before the `bl`),
 * result unused. */
void sub_0806974C(ProcPtr);
/* Wave 36, W36-K. Promoted in src/decomp/c_0806E740.c, previously undeclared;
 * signature copied verbatim. */
void sub_0806E7C0(int, int, ProcPtr);
/* Wave 36, W36-K. Both promoted (src/decomp/c_0806AEC4.c, c_0806E210.c) and
 * previously undeclared; signatures copied verbatim. */
void sub_0806AEC4(int);
void sub_0806E210(int, ProcPtr);
/* Wave 36, W36-K. Promoted in src/decomp/c_0806F0A0.c, previously undeclared.
 * Its definition names a struct tag that lives in that .c, so the tag is
 * FORWARD-DECLARED here rather than retyped to ProcPtr: retyping would have
 * made the promoted definition disagree with this prototype, and the definition
 * wins. Callers outside that file cast. */
struct Unk0806F0A0Proc;
void sub_0806F0A0(struct Unk0806F0A0Proc *);
/* Four arguments, all in registers, the last the caller's proc. */
void sub_08068014(int, int, int, ProcPtr);
/* Takes the caller's proc and nothing else; sub_0806B910 calls it with a bare
 * `adds r0, r4, #0` and discards whatever comes back. */
void sub_0806AD04(ProcPtr);
/* Already DEFINED and promoted in src/decomp/c_0806BD6C.c but never declared.
 * This line publishes the promoted signature so sub_0806BD84 can call it. Do
 * not weaken it -- the definition wins. */
void sub_0806BD6C(u16 *, u16);
/* Two more from this block, promoted alongside their callers. sub_080697CC is
 * the graphics loader sub_08069864 calls; sub_0806B87C is the fade starter
 * sub_0806B910 calls. */
void sub_080697CC(void);
void sub_0806B87C(ProcPtr);

/* ---- Wave 35 (W35-A) ---- */
/* PARKED in data/parked.json (95.5%), so declared from its call site in
 * sub_0804365C rather than promoted. It reads NO argument register --
 * parked.json records its body as
 * `if (gUnknown_085C77A0[gUnknown_03003FC0.unk02].unk24) return it;
 *  if (gUnknown_03003FC0.unk30) return it; return 0;` -- and sub_0804365C
 * uses the result (`adds r4,r0,#0; cmp r4,#0`), so it is not void. The
 * parked entry probed u16 and int and found them byte-identical, so `int`
 * is the weakest fit. */
int sub_08043630(void);
/* EIGHT arguments, read straight off sub_080438FC's two call sites: three
 * pointers to the caller's own stack slots in r0/r1/r2, a plain int in r3,
 * then four stack words at [sp+0], [sp+4], [sp+8] and [sp+0xc], the last of
 * which is a fourth pointer to a caller stack slot. Both call sites agree on
 * every position; only [sp+0], [sp+4] and [sp+8] differ in value between
 * them. Nothing reads r0 afterwards, so void. */
void sub_080439A8(int *, int *, int *, int, int, int, int, int *);

/* ---- wave 35 (W35-C): blocks 0x08021-0x08025 ---- */
/* Promoted from the MATCHED sub_08024DDC; declared here so sub_08024E60 (which
 * calls it twice, the second time with its arguments swapped) can see it. Both
 * parameters are the gUnknown_030013D0 record -- see struct Unk030013D0 in
 * unknown-globals.h for how the layout was measured. VOID, and that is settled
 * from the callee rather than guessed: sub_08024DDC ends `pop {r0}; bx r0`,
 * which overwrites any result, and sub_08024E60 reads nothing after either
 * `bl`. */
void sub_08024DDC(struct Unk030013D0 *, struct Unk030013D0 *);
/* TWO arguments, both materialised immediately before the `bl` as bare
 * `movs rN,#imm` (sub_080246B4 calls it as (1, 8) and (0, 8) from two arms of
 * one switch), and the result is discarded. `int` is the weakest type that
 * fits -- the two call sites only ever pass 0, 1 and 8, so nothing constrains
 * the width, and nothing narrows on return. */
void sub_0803F880(int, int);
/* THREE arguments, and the third is what proves the arity: sub_08024058
 * materialises `movs r2,#0xe0; ands r2,r3` -- the top-three-bits field of the
 * gUnknown_08499590 +0x1432 terrain byte it has just written back -- in the
 * instruction pair immediately before the `bl`, with r0 and r1 already holding
 * its own two sign-extended s16 parameters. Nobody computes a mask to pass a
 * dead argument. Result unused (r0 is not read before the next `bl`). */
/* RETYPED, wave 39 (W39-C), from `void sub_080240B4(int, int, int)`. The body
 * settles all four: it RETURNS `(s16)n` (`lsls r2,#0x10; asrs r2,#0x10;
 * adds r0,r2,#0` at the epilogue), and it narrows r0/r1 to 16 bits and r2 to 8
 * at entry. Per PROMOTE_MODE the entry pair is a ZERO-extend for `s16` just as
 * much as for `u16`; what makes these s16 rather than u16 is that every USE
 * re-extends with `asrs` (the second shift pair). The narrow types cost the
 * promoted caller sub_08024058 nothing -- it already holds two s16 parameters
 * and a `v & 0xe0` that fits u8 -- and c_08024058.c was re-run through
 * try_match after this change and still matches. The old prototype's `void`
 * was read off that one caller discarding the result, which is exactly the
 * "agreement between files, not correctness" failure the brief describes. */
s16 sub_080240B4(s16, s16, u8);
/* Wave 39, W39-C. sub_080240B4's near-twin, same three narrowed parameters and
 * the same `(s16)n` return, read off its own body -- its only caller
 * (sub_08028580) is still assembly, so nothing external constrains it. */
s16 sub_0802419C(s16, s16, u8);
/* Already DEFINED and promoted in src/decomp/c_080249EC.c but never declared.
 * This line publishes the promoted signature so sub_08024A2C (a different
 * unit) can call it. The definition wins -- do not weaken it. */
int sub_080249EC(int, s8, u8);
/* Two more undeclared callees of block 0x08024, both from sub_080246B4 and
 * both with the SAME first argument, the literal 0x48. sub_0803F8E0's second
 * argument is `Div(gUnknown_03004008, 20) % 4` and sub_0803FE50's is
 * gUnknown_03004008 itself, each materialised in r1 immediately before its
 * `bl`; nothing reads r0 after either, so both are void. `int` throughout is
 * the weakest type that fits -- gUnknown_03004008 is already s32 and neither
 * value is narrowed on the way in. (Neither was on the wave brief's
 * undeclared-callee list for this block, which listed only sub_080240B4,
 * sub_08024DDC and sub_0803F880 -- the list undercounts.) */
void sub_0803F8E0(int, int);
void sub_0803FE50(int, int);
/* The WRAM half of the BIOS decompressor, declared to match LZ77UnCompVram
 * above (same argument shape, same const-ness). sub_080247A4 passes a ROM blob
 * out of gUnknown_085C77A0[].unk2c as the source and the gUnknown_03003F68
 * buffer as the destination. */
void LZ77UnCompWram(const void *, void *);
/* Called with nothing set up in r0-r3 and its result discarded, twice, from
 * both arms of sub_080247A4 -- so `void (void)` is byte-identical to any other
 * shape here and is the weakest that fits. */
/* WAVE 35, W35-E: WAS `void sub_08037B84(void);`, WHICH IS WRONG. The promoted
 * definition in src/decomp/c_08037B84.c has taken `void *p` since it landed,
 * and sub_0803CD2C / sub_0803CDBC / sub_0803CE28 all materialise the argument
 * (`adds r0, r5, #0` where r5 already holds gUnknown_02000000 -- a nullary
 * callee needs no such copy). It has no other caller in src/, so nothing had
 * ever exercised the disagreement. */
void sub_08037B84(void *);

/* WAVE 35 (W35-D), block 0x0806D. Four of these were already PROMOTED with no
 * declaration anywhere (sub_0806DCB8 / sub_0806DD34 / sub_0806DDF4 in
 * src/decomp/c_0806DCB8.c and c_0806DDF4.c) -- a symbol missing from this
 * header is not evidence it is underived. Signatures copied from those
 * definitions; sub_0806DD34's parameter is a struct local to c_0806DCB8.c, so
 * the tag is forward-declared here to give it file scope rather than prototype
 * scope. Its object IS a struct Unk08580934_Obj (sub_0806DE38 passes
 * gUnknown_08580934->unk54[unk33] straight to it) -- the two models agree on
 * every offset; retyping c_0806DCB8.c is left for a wave that can re-verify
 * both matched functions in it. */
/* WAVE 35 (W35-D), the m4a block. All four are undeclared and unpromoted;
 * these signatures are read off the call sites in sub_08070350, sub_080707F4
 * and sub_08072B54.
 *   sub_0806F734 -- the MP2K umul3232H32 shape: two u32 in, one out,
 *     sub_08070350 nests one call inside the other.
 *   sub_0806FBD4 -- takes the ADDRESS of gUnknown_03005740 (`ldr r4,=sym;
 *     adds r0,r4,#0`), which sub_080707F4 then also parks in SoundInfo+0x34,
 *     so it fills a jump table rather than being called with a value.
 *   sub_080714FC -- third argument is narrowed `lsls #0x18; asrs #0x18` at the
 *     call, so the parameter is s8; the second is the 0xffff track mask.
 * sub_08073228 is likewise read off sub_08073304's call site (four arguments,
 * the fourth being the proc it has just started). */
/* Only their ADDRESSES are used -- sub_080707F4 parks them in the SoundInfo
 * hook block; nothing in C calls either yet, so `void (void)` is the weakest
 * shape that fits. */
void sub_080700C0(void);
void sub_080718E4(void);
int sub_08072B2C(int);
u32 sub_0806F734(u32, u32);
void sub_0806FBD4(void *);
void sub_080708EC(u32);
void sub_08070668(struct MusicPlayerInfo *);
void sub_080714FC(struct MusicPlayerInfo *, u16, s8);
/* Wave 38 (W38-A): m4a ClearModM, already DEFINED in src/decomp/c_08071564.c as
 * `void sub_08071564(struct MusicPlayerTrack *)` but never declared here. Its
 * two callers sub_08071584 and sub_080715F8 pass the loop's own track pointer
 * straight through in r0, which agrees. The definition wins, not this line. */
void sub_08071564(struct MusicPlayerTrack *);
/* Wave 38 (W38-A): m4a ModDepthSet / LFOSpeedSet -- the two MPlay*Control entry
 * points that call ClearModM. Third parameter is u8 in both: `lsls #0x18;
 * lsrs #0x18` at entry and a bare `strb` into `mod` (+0x17) / `lfoSpeed`
 * (+0x19). Both have fan-in 0, so nothing corroborates from a call site. */
void sub_08071584(struct MusicPlayerInfo *, u16, u8);
void sub_080715F8(struct MusicPlayerInfo *, u16, u8);
/* Wave 38 (W38-A): the wave-27 palette pair, already DEFINED in
 * src/decomp/c_08071C84.c as `void (int)` but never declared here. Declared now
 * because sub_08071CC4 / sub_08071CDC call them. The definitions win. */
void sub_08071C84(int);
void sub_08071CA4(int);
void sub_08073228(const void *, void *, u16, ProcPtr);
void sub_0806CFC8(int, int);
void sub_0806D050(int, int);
void sub_0806D0D8(struct Unk08580934_Obj *);
void sub_0806D3AC(struct Unk08580934_Obj *);
void sub_0806DC50(int);
void sub_0806DCB8(void);
struct Unk0806DD34;
void sub_0806DD34(struct Unk0806DD34 *);
void sub_0806DDF4(void);


/* Wave 35, W35-E. Derived from the call sites in 0x0803A/0x0803C/0x0803E;
 * none of these has a definition in src/decomp/ or an entry in
 * data/promoted.json, so the arity is read off the argument setup and the
 * void-ness off the discarded results.
 *
 * sub_08071948 takes 5 (r0-r3 plus one `str rN,[sp]`), sub_080376DC takes 6
 * (r0-r3 plus `[sp]` and `[sp,#4]`), sub_0801FAC4 takes 5. sub_0803AB3C and
 * sub_0803D6D0 get no argument set up at all. */
void sub_0803AB3C(void);
/* Wave 38 (W38-A): fifth parameter retyped `int` -> `u16`. It arrives on the
 * stack and the ROM normalises it `ldr r0,[sp,#0x18]; lsls r0,#0x10;
 * lsrs r6,r0,#0x10` -- that is PROMOTE_MODE's prologue zero-extension of a
 * sub-word parameter, which an `int` does not get, and with `int` the function
 * is two instructions short at entry. All three promoted callers
 * (c_0803A338.c, c_0803A4A8.c, c_08046D30.c) pass the literal 0x8360, so the
 * narrowing is a compile-time no-op there; all three re-verified byte-for-byte
 * after this change. It is a BG tilemap entry (palette 8 | tile 0x360), which
 * is 16-bit anyway. */
void sub_08071948(u16 *, int, int, const void *, u16);
/* W35-E: sub_0803D6FC is DEFINED in src/decomp/c_0803D6B8.c (not c_0803D6FC.c),
 * which is why a glob for its own file missed it. Signature copied verbatim. */
struct Unk3D6FC;
void sub_0803D6FC(struct Unk3D6FC *);
void sub_080376DC(void *, int, int, int, int, int);
void sub_0803CFA4(const void *, u8 *, int);
void sub_0803D6D0(void);
void sub_08026040(int, int, int, int);
/* wave 49: corrected (int x5) -> (u16, u16, u16, u16, u8), read straight off
 * the definition's prologue -- r0..r3 are narrowed `lsls #0x10 / lsrs #0x10`
 * and the stacked fifth `lsls #0x18 / lsrs #0x18`, none of which agbcc emits
 * for an int parameter. Byte-neutral at the only call site
 * (src/decomp/c_0803E9F8.c already casts arguments 1 and 2 to u16 and passes
 * literals for 4); c_0803E9F8 re-verified by trymatch exit code after the
 * change. */
void sub_0801FAC4(u16, u16, u16, u16, u8);
void sub_0801FCE0(int, int, int);
void sub_0801FD30(int, int, int);
/* Wave 35, W35-E: matched definitions in this wave's batch. */
void sub_0803A338(void);
void sub_0803A4A8(void);
void sub_0803A5B8(void);
/* Wave 43, W43-I: FOURTH PARAMETER ADDED, and W35-E's arity above is what was
 * wrong -- not the types. sub_0803AB3C sets r3 up as well, `lsls r3,#0x18;
 * asrs r3,#0x18` immediately before the `bl`, which is an s8 conversion of a
 * value that is 0, 0xd or -1 and cannot be anything but a fourth argument.
 * c_0803AAC0.c never reads it; a trailing parameter that is never referenced
 * costs no instructions, and that file was re-verified byte-for-byte after the
 * change. The arity was read "off the argument setup" in wave 35 from the call
 * sites that existed THEN -- sub_0803AB3C was still unmatched, so its r3 was
 * never in evidence. */
void sub_0803AAC0(u8, u8, u8, s8);
void sub_0803CEB8(u8, const void *);
void sub_0803CF54(u8, const void *, u8);
int sub_0801AD70(u8);
/* Wave 50 (W50-K). Read off sub_0801B2FC's two call sites, which set up NO
 * arguments at all (`bl sub_0801ADC8` with r0-r3 dead) and use no result. That
 * is the whole evidence: arity is invisible in a pass-through wrapper, but this
 * is not one -- nothing is forwarded. NOT PROVEN against its own prologue; if a
 * later wave promotes sub_0801ADC8 itself, its definition wins over this. */
void sub_0801ADC8(void);
/* Wave 50 (W50-K). Copied from the PROMOTED definition in src/decomp/c_0801B018.c
 * rather than read off a call site -- a promoted definition wins. sub_0801AC58
 * calls it with an `int` loop index and the ROM narrows it (`lsls #0x10;
 * lsrs #0x10`) at the call, which is what the u16 parameter produces. */
int sub_0801B018(u16);
/* Wave 41 (W41-A): the slot allocator sub_0801AD70/sub_0801ADC8/sub_0801B2FC
 * all ask before touching gUnknown_0200CC38. RETURN TYPE READ OFF ITS OWN
 * BODY, not off a call site: the "none" path is `ldr r0, =0x0000FFFF`, a
 * POSITIVE pool constant -- an s16 function returning -1 emits
 * `movs r0,#1; rsbs r0,r0,#0` instead -- and the success path narrows r8 with
 * `lsls #0x10; lsrs #0x10`. So it returns u16 and its sentinel is 0xFFFF, not
 * -1. sub_0801AD70's `lsls r0,#0x10; cmp r0, #0xFFFF0000` is combine folding
 * the re-narrow into `(u16)result == 0xFFFF`; an (s16) reading of that test is
 * byte-identical at the call site and WRONG about the callee.
 * The parameter is narrow (`lsls #0x10; lsrs #0x10` at entry) and is only ever
 * tested for equality -- against 0xff and against a u8 array element -- so
 * nothing here discriminates its signedness; u16 agrees with the return. */
u16 sub_0801B120(u16);
void sub_08022AF8(u8, u8, u8, u8); /* W35-E: signature copied from the
                                    * promoted definition in c_08022AF8.c. */

/* Wave 35, W35-F -- block 0x0804B. All read off call sites in sub_0804B744,
 * which materialises every argument. sub_0804B55C's third parameter is dead in
 * its own body (r2 is written by `lsls r2,r6,#1` before any read) but the
 * caller emits `movs r2,#0` in front of both calls, so the arity is 3.
 * sub_0804B850's last three arguments arrive as `str rN,[sp,#0/4/8]` against a
 * `sub sp,#0xc` frame, i.e. parameters 5-7. sub_0804B42C/sub_0804B4C4 are
 * PROMOTED in src/decomp/c_0804B42C.c and these signatures are copied from
 * there, not inferred. */
int sub_0804B42C(int, int);
int sub_0804B4C4(int, int);
u16 sub_0804B55C(u16, u8 *, int);
u16 sub_0804B644(u16, u16);
/* Wave 43, W43-K: parameter 1 was `int` and is NARROW. sub_0804B850's prologue
 * truncates BOTH r0 and r1 with `lsls #0x10; lsrs #0x10` back to back, which is
 * the PROMOTE_MODE entry pattern for a sub-word parameter -- an `int` gets no
 * such pair, and a cast at the single use would sit next to that use instead of
 * in the prologue. The only caller, c_0804B744.c, passes the literals 0 and 1,
 * so the change is byte-neutral there; re-verified after the edit. */
void sub_0804B850(u16, u16, void *, void *, void *, void *, void *);
/* Wave 43, W43-K -- sub_0804B850's three callees, read off its call sites.
 * sub_0804BB28 and sub_0804BB44 are PROMOTED in src/decomp/c_0804BB28.c and
 * these agree with the definitions there; sub_0804BB28's third parameter is
 * dead in its own body and was added to that definition in this wave (see the
 * comment on it). sub_0804BB74's third parameter is `u32` because its body
 * opens `lsrs r6, r6, #2` on the raw incoming value -- a bare logical shift
 * right, which an `int` would have made `asrs`; its first parameter is NOT
 * narrowed at entry, so it is `int` where sub_0804BB28's sibling index is. */
/* Wave 43, W43-K: both are PROMOTED -- src/decomp/c_0804A18C.c and
 * c_0804AE10.c -- and these are copied from those definitions, not inferred.
 * sub_0804A6D8 is the first C caller of either. */
void sub_0804A1E4(u8);
void sub_0804AE10(void);
void sub_0804BB28(int, void *, int);
void sub_0804BB44(int, void *, int);
void sub_0804BB74(int, void *, u32, int);
void sub_0804B8BC(u16, u16);
/* Wave 35, W35-F. sub_0804D6C8/sub_0804D6FC are PROMOTED in
 * src/decomp/c_0804D6C8.c and c_0804D6FC.c with an unread second parameter;
 * copied verbatim rather than inferred. sub_080505A4 is read off sub_0804D0FC
 * and sub_0804D4E8, which pass a u16 group index and the u16 entry field
 * unk1e and discard nothing. */
void sub_0804D6C8(u16, int, u16);
void sub_0804D6FC(u16, int, u16);
void sub_080505A4(u16, u16);

/* Wave 35, W35-E: byte-verified definitions in src/decomp/ that had no
 * declaration anywhere (src/decomp/c_08037638.c, c_0803D3D8.c, c_080248F8.c).
 * Signatures copied verbatim from those definitions. */
void sub_08037638(int, int, int, int);
void sub_0803D3D8(int, u8 *);
u8 sub_080248F8(void);

/* Wave 35, W35-L: the 0x08047-0x08049 batch and the callees it reaches.
 *
 * sub_08014B0C is the structural twin of sub_080149C0 above -- six arguments,
 * four in registers and two on the stack -- and both sub_080487B4 and
 * sub_08047B98 call it. The FOURTH parameter is `int` and not `u8 *`: that is
 * where the two call sites disagree with sub_080149C0's shape, sub_08047B98
 * passing `p->unk1f + 1` and sub_080487B4 passing the word
 * gUnknown_0849EDB0[i].unk04. Everything else follows sub_080149C0.
 *
 * sub_080199D0 takes one argument (`movs r0,#1`) whose width is not
 * recoverable from the call site; `u8` follows the note on gUnknown_0300xxxx
 * in unknown-globals.h that records it setting a byte from its argument. Its
 * result is discarded, so `void`.
 *
 * sub_08047190 / sub_08047920 / sub_080488E0 are declared from their call
 * sites in sub_08047B98 and sub_08048F4C only; `void *` is the weakest type
 * for the record pointer sub_08047B98 forwards unchanged, and every result is
 * discarded. Widen these when the definitions are matched. */
/* Wave 40 (W40-D) corrects arguments 4..6 from the BODY, the same axis wave 21
 * corrected sub_08014668 on. Argument 4 is `u32`: it is the dividend of a
 * `% 10` / `/ 10` pair that compiles to __umodsi3/__udivsi3, which a signed
 * spelling cannot produce. Arguments 5 and 6 are `u16`, narrowed
 * `lsls #0x10; lsrs #0x10` in the PROLOGUE -- ahead of the destination
 * arithmetic and grouped with each other, i.e. PROMOTE_MODE on narrow
 * parameters. An `int` spelling cannot emit them at all: argument 5's only use
 * is an `orrs` feeding a `strh` and argument 6's is a multiply feeding one, so
 * force_to_mode folds a cast at either use away entirely. Byte-neutral at both
 * call sites -- c_08047B98.c passes `p->unk1f + 1`, 0x8000 and 0, and
 * c_080487B4.c passes a word, an already-`(u16)`-cast product and 0. */
void sub_08014B0C(int, int, u16 *, u32, u16, u16);
/* Wave 40 (W40-D), matched. sub_08014B0C's five-argument sibling and the same
 * digit renderer, but SIGNED: its `% 10` / `/ 10` pair compiles to
 * __modsi3/__divsi3 where sub_08014B0C's compiles to the unsigned pair, so
 * argument 4 is `int` there and `u32` above. It also drops sub_08014B0C's
 * tile-group argument, using the fixed tile pair 0x123/0x133 instead. Argument
 * 5 is `u16` on the same prologue evidence. No caller exists in src/ yet;
 * signature copied verbatim from the matched definition. */
void sub_08014B60(int, int, u16 *, int, u16);
void sub_080199D0(u8);
void sub_08047190(void *, u8);
void sub_08047920(void *);
/* Wave 48, W48-I: both MATCHED this wave, so these two are the definitions'
 * own types rather than call-site guesses. sub_0804769C returns a u16 counter
 * and its sole caller sub_080484CC truth-tests the result with a bare
 * `lsls #0x10`, which agrees. `struct Unk0804769C` is in unknown-globals.h
 * because caller and callee land in different translation units. */
u16 sub_0804769C(struct Unk0804769C *, u16);
void sub_080484CC(struct Unk0804769C *);
/* Declared from sub_080484CC's call site only: one argument, the same record
 * pointer forwarded unchanged, result discarded. `void *` is the weakest type
 * that fits; widen it when the definition is matched. */
void sub_080482D8(void *);
void sub_080488E0(void);
/* Wave 43, W43-L. Already DEFINED in src/decomp/c_08048F10.c; it had no
 * declaration here because nothing outside its own unit called it until
 * sub_08048FD8 (matched this wave) did. The type is the definition's: it
 * returns the NEW value of the s16 gUnknown_084C30F8->unk832 as u16, and
 * sub_08048FD8 truth-tests that result with a bare `lsls #0x10`, which agrees. */
u16 sub_08048F10(void);
/* Wave 43, W43-L. Already DEFINED in src/decomp/c_080485DC.c; it had no
 * declaration here because nothing outside its own unit called it until
 * sub_08048FD8 (matched this wave) did. The type is the definition's. */
void sub_080485DC(const u8 *);
/* Wave 43, W43-L, matched -- this declaration is taken from sub_08048850's OWN
 * definition rather than from a call site, which is why the widths are firm.
 * Both parameters narrow in the prologue (`lsls #0x10; lsrs #0x10`), so both
 * are 16-bit; UNSIGNED because a1 only ever feeds `i < a1 + 3` and a2 only
 * ever indexes the u8 array gUnknown_02028E1C, and PROMOTE_MODE would emit the
 * same prologue for s16 either way -- nothing re-narrows a use, so the sign is
 * unproved and u16 is the weakest fit. Void: the epilogue sets no r0 and both
 * callers (sub_08049178, sub_08049B80) discard. */
void sub_08048850(u16, u16);

/* The W35-L batch itself. sub_08048EC4 returns `u16`: sub_080490BC narrows its
 * result with `lsls #0x10; lsrs #0x10` and then reuses the same register as an
 * argument, so the value is kept, not just tested. sub_080485F8's epilogue is
 * `pop {r0}; bx r0`, the void spelling. */
u16 sub_08048EC4(void);
void sub_080485F8(void);

/* ---- wave 36 (W36-E) ---- */

/* Promoted in src/decomp/c_080248E4.c since wave 30 but never declared -- the
 * fifth instance of the 407-function gap. Copied verbatim from that
 * definition; sub_0807B2F8 hands the result to sub_080149C0's `u8 *`. */
u8 *sub_080248E4(void);
/* Promoted in src/decomp/c_08074714.c and likewise never declared; copied
 * verbatim from that definition. sub_08076888 calls it with a literal 4. */
void sub_08074714(ProcPtr);

/* Proc handlers in the 0x08076 and 0x0807A-0x0807B blocks, all taking their own
 * proc and returning nothing (`pop {..}; pop {r0}; bx r0` with no value set). */
void sub_08076298(ProcPtr);
void sub_0807B2F8(ProcPtr);
void sub_0807BCF0(ProcPtr);
void sub_0807BED8(ProcPtr);
/* Wave 48 (W48-C), read entirely off its ONE call site, sub_0806AB24 -- the
 * body is still assembly, so every field below is a caller-side reading and
 * none of it is confirmed at the callee.
 *   FIVE parameters, the fifth on the stack (`str r4, [sp]` with sp lowered by
 * 8). A text layout/wrap routine: arg1 is whatever `u8 *sub_08024944(u16)`
 * returned, arg2 is `&<u16 local>` -- an OUT-parameter, read back with `ldrh`
 * straight after the call -- arg3 is a `u8 *` into the caller's own proc at
 * +0x2f which the caller then fills as a per-line width array, arg4 is a u16
 * proc field (a wrap width), arg5 is the caller's proc.
 *   The result is a LINE COUNT: the caller stores it into an `int` field and
 * uses it as a `< n` loop bound over arg3's array. Declared `u16` because the
 * call site narrows with `lsls #0x10; lsrs #0x10` -- but that is byte-neutral
 * against `int` plus an explicit `(u16)` cast at the one use (agbcc re-narrows
 * a narrow-returning callee at every call site), so the width is NOT proved.
 * Likewise arg4: a narrow and a wide parameter are identical at this call. */
void sub_0807B51C(int, int, int, int);
void sub_0807B738(ProcPtr);
u16 sub_0807B7BC(u8 *, u16 *, u8 *, int, void *);
/* Nullary: sub_0807A0C4 calls it with `bl` and no argument register set up,
 * and its own prologue reads none. It ends by writing 2 into +0x3a of the
 * struct Unk03001470 sub_08014740 returns, so the result is discarded too. */
void sub_0807A860(void);
/* sub_0807A860 hands it the u8 at +0x1d of a gUnknown_08499598 record and
 * sub_08078E14's `int` result, and narrows the value it returns to u16 for
 * sub_08014740's fourth parameter. `int` both ways is the weakest model that
 * gives that single `lsls #0x10; lsrs #0x10` at the call site; a `u16` return
 * would be byte-identical here, so this is NOT proved. */
int sub_0807A3AC(int, int);
/* sub_0807BCF0 hands it its own proc once a frame counter passes a threshold;
 * the result is not used. */
void sub_0807BFB8(ProcPtr);

/* Wave 36 (W36-F). The per-frame update/vblank dispatchers at 0x08036884,
 * 0x080368E8, 0x08036944, 0x080369BC, 0x08036A50 and 0x08036AB8 call these in
 * long straight-line runs, and none of them was declared anywhere.
 *
 * The first block is copied VERBATIM from byte-verified definitions already in
 * src/decomp/ (see c_0801348C.c, c_08013B2C.c, c_0801F050.c, c_0801F0AC.c,
 * c_08054B7C.c) -- not re-derived from the call sites. */
void sub_08013510(void);
void sub_08013B2C(void);
void sub_0801F050(void);
void sub_0801F06C(void);
void sub_0801F0AC(void);
void sub_0801F0C8(void);
void sub_0801F0E0(void);
void sub_0801F0FC(void);
void sub_08054B7C(void);

/* The second block has no promoted definition. Each prologue in asm/ opens by
 * writing r0 (or by not touching an argument register at all) before any read,
 * so all are nullary; each is called for effect with the result discarded, so
 * `void` is the weakest type that fits every site. sub_0803B404 is a lone
 * `bx lr`. */
void sub_08011AD8(void);
void sub_08011B98(void);
void sub_08015954(void);
void sub_08019470(void);
void sub_0802FACC(void);
void sub_080345C8(void);
/* Wave 36 (W36-A) is matching sub_08023EEC itself and owns its final
 * signature. Declared here only because sub_080369BC could not compile without
 * it: its prologue opens `ldr r2, =gUnknown_08090A1C` with no argument register
 * read, so it is nullary, and sub_080369BC discards the result. If W36-A's
 * definition disagrees, W36-A's wins. */
void sub_08023EEC(void);
void sub_0803B404(void);
void sub_0803F990(void);

/* Handed to sub_08011B34's `void *` slot by sub_0803662C, via the
 * `(void *)fn` house convention. All nullary by the same prologue reading;
 * sub_0803550C and sub_08043590 open on a global, the rest write r0 first. */
void sub_08021DD8(void);
void sub_08022048(void);
void sub_08022A6C(void);
void sub_0803550C(void);
void sub_08043590(void);
void sub_0803678C(void);

/* Wave 36 (W36-F), block 0x08037. Read off each callee's own prologue in
 * asm/code-0801D390.s, not guessed from the call site:
 *   sub_08037170  four `lsls #16`/`lsrs #16` pairs on r0..r3, so four u16s;
 *                 `pop {r0}; bx r0`, so void.
 *   sub_080377C4  `mov sl, r0` with no narrowing -- one pointer argument.
 *   sub_080378A8  `str r0, [sp]` -- one pointer argument (a VRAM address).
 *   sub_08037B90  opens `movs r3, #0` and never reads an argument register.
 *                 Nullary for real: sub_08037CF8's `bl sub_080378A8` /
 *                 `bl sub_08037B90` pair with nothing between it is TWO
 *                 statements, not a nested call.
 *   sub_08013D00  selects one of gUnknown_08499578/7C/80/84, derefs it and
 *                 returns `*p + y * 64 + x * 2` -- a u16 * into a tilemap.
 *                 r0 is compared against 1/2/3, r1 is the x term, r2 the y. */
u16 *sub_08013D00(int, int, int);
void sub_080377C4(void *);
void sub_080378A8(void *);
void sub_08037B90(void);
void sub_08037170(u16, u16, u16, u16);
void sub_080360D0(ProcPtr);

/* sub_080366D0 and sub_080366C4 take `void (*)(void)`, so these two are fixed
 * as nullary void by the parameter type, not by inference. */
void sub_08036944(void);
void sub_080369BC(void);

/* Wave 36 (W36-I), all read off sub_08004E88 / sub_0800487C / sub_08004970,
 * which are the only call sites in the tree that constrain them:
 *   sub_08002F1C  no argument register written before the `bl`, result
 *                 discarded -- nullary void.
 *   sub_0800C8D8  likewise -- but see below, this one was WRONG.
 *   sub_0800C874  nullary; its r0 is `strb`d straight into
 *                 gUnknown_0200B0B0->unk12, so the return is at least a byte
 *                 and `int` is the weakest fit (no re-narrowing appears).
 *   sub_0800CAA0  nullary; its r0 is handed on as sub_0800CB30's second
 *                 argument with no narrowing between the two `bl`s.
 *   sub_0800CB30  called as (1, sub_0800CAA0()).
 *   sub_0808B694  two pointers -- a struct Unk03001470 field address and
 *                 &gUnknown_0200B0B0->unk9c -- and the result is a bare
 *                 `cmp r0,#0`, so a comparison predicate. */
void sub_08002F1C(void);
/* Wave 48 (W48-A) retypes sub_0800C8D8 from `void` to `int`.  W36-I inferred
 * void from its two call sites (sub_08003B8C and sub_08004E88) discarding the
 * result, which is exactly the blind spot the brief warns about: a discarded
 * result constrains nothing.  The body ends `adds r0, r5, #0` before the pop,
 * where r5 is a counter the second loop increments, so it returns that count --
 * the same "how many entries did I touch" value its neighbours sub_0800C874,
 * sub_0800C8A0 and sub_0800C6A8 all return.  Both callers still discard it and
 * both were re-verified byte-identical under the new type.
 *
 * sub_0800C958 had no declaration anywhere in the tree.  It is `int (int)`:
 * the argument reaches sub_0800C6E8's first parameter unchanged and is then
 * switched over the same 0x28/0x48/0x68/0x88 ids, and the result is summed
 * into sub_0800C9E8's counters with no narrowing at either of its uses. */
int sub_0800C8D8(void);
int sub_0800C958(int);
/* sub_08004E38's signature is copied verbatim from the byte-verified
 * definition in src/decomp/c_08004E38.c, which had no declaration anywhere.
 * sub_0800376C likewise (src/decomp/c_0800376C.c). sub_080036A4 and
 * sub_08003814 are wave-36 matches in work/, both plainly nullary void. */
void sub_08004E38(char *, const char *);
void sub_080036A4(void);
void sub_0800376C(void);
void sub_08003814(void);

/* Family F059. The byte-verified definitions in src/decomp/c_080055B8.c were
 * written `(void)`, but their only call site -- sub_08004D28 -- materialises
 * `movs r0,#0; movs r1,#0; movs r2,#0` in front of every one of the three
 * `bl`s, and only a call site can prove an argument. All three parameters are
 * unused in the bodies, which is why `(void)` was byte-identical there and is
 * NOT evidence against them; the definitions were widened to match and
 * re-verified. Wave 36, W36-I. */
void sub_080055B8(int, int, int);
void sub_08005634(int, int, int);
void sub_080056B0(int, int, int);
int sub_0800C874(void);
int sub_0800CAA0(void);
/*   sub_0800CB30  called as (0, 0) and then (1, <the first call's result>) in
 *                 sub_08004F9C, which is what proves the return value.
 *   sub_0800C9E8  nullary; its r0 is narrowed `lsls #0x18; lsrs #0x18` into
 *                 sub_0803CF54's declared u8 third parameter. */
int sub_0800CB30(int, int);
int sub_0800C9E8(void);
/* sub_080032EC: sub_080030BC's tail call, `(lane, x >> 4, 0x6a)`. */
void sub_080032EC(int, int, int);
int sub_0808B694(const void *, const void *);

/* ---- wave 36 (W36-J): the 0x08046-0x08049 address-locality block ---------- */

/* Both have byte-verified definitions in src/decomp/ and had no declaration in
 * any header -- the "407 promoted, 0 declared" case from the brief. Signatures
 * copied verbatim from c_080468D4.c and c_080470DC.c, not inferred. */
void sub_080468D4(int);
void sub_080470E8(void);

/* Derived from their call sites in sub_08046D30 and sub_08046E48, which is all
 * there is: both take gUnknown_02028DD5 and gUnknown_02028DD6 in r0/r1, each a
 * plain `ldrb` of a u8 global with no narrowing before the `bl`, so `int` is
 * the weakest model that fits. Neither result is used and neither caller
 * re-narrows, but with the value discarded that proves nothing about the
 * return, so both are declared void off the call sites alone. */
/* Wave 49, W49-J: RETYPED to (u8, u8) off the definitions' OWN prologues, which
 * outrank the call sites the paragraph above reasons from -- each opens with
 * `lsls #0x18; lsrs #0x18` on r0 and on r1 and spills nothing else, and an
 * `int` parameter is never narrowed at entry. Byte-neutral at every caller: the
 * arguments were already plain `ldrb`s of u8 globals, so nothing has to
 * re-narrow. Both really are void -- sub_08046778 tail-calls sub_08013AEC and
 * sub_08046914 does the same, with nothing in r0 after it. */
void sub_08046778(u8, u8);
void sub_08046914(u8, u8);

/* Wave 43, W43-L. RETYPED from all-`int` off sub_080499F8's call site to the
 * widths sub_08049944's OWN prologue proves -- the definition matched this
 * wave. All five arguments narrow at entry, in argument order, before any
 * other work: `lsls #0x10; lsrs #0x10` on a1/a2/a3/a5 and `lsls #0x18;
 * lsrs #0x18` on a4. An `int` parameter cast at a use puts those shifts at the
 * use, not in the prologue, so these are declared narrow.
 *
 * The SIGNS are settled at the uses, per PROMOTE_MODE (entry is zero-extending
 * for s16 and u16 alike): a2 and a3 are re-widened `lsls #0x10; asrs #0x10`
 * before sub_08014B0C's two `int` parameters, which only an s16 source can
 * produce; a1 is compared `blo`/`bhs` against the digit ramp, unsigned; a5
 * goes to sub_08014B0C's `u16` fifth parameter unnarrowed.
 *
 * Byte-neutral at the one existing caller and re-verified there: c_080499F8.c
 * passes a u16 member, two literals, a u8 member and 0x3000, every one of
 * which converts to the narrow parameter with the same instructions it already
 * used to convert to `int`. */
void sub_08049944(u16, s16, s16, u8, u16);

/* Wave 36 (W36-L): the four undeclared callees of the 0x08020/0x08021 blocks,
 * all read off their call sites in sub_08020354 / sub_080201E0 / sub_08020D50 /
 * sub_080210C8, which are byte-identical in shape.
 *
 * sub_0801F9C0, sub_080200EC and sub_08020B88 are one three-way twin set: each
 * is called twice with (x, y, value, flag). The FIRST two arguments separate
 * them -- sub_08020354 narrows its pair with `lsls #0x10; lsrs #0x10` before
 * passing them to sub_0801F9C0, while sub_080201E0 and sub_08020D50 narrow
 * theirs with `asrs` before passing them to sub_080200EC / sub_08020B88 -- so
 * u16 for the first and s16 for the other two.
 *
 * The THIRD argument is the decisive one and it is read off the SECOND call
 * site in each caller, where the value is `gUnknown_085D5ABC[t].unk0e - 1`, an
 * `ldrb` minus one. sub_0801F9C0's caller emits `lsls #0x10; lsrs #0x10` on it;
 * sub_080200EC's and sub_08020B88's emit NOTHING. That is not a difference in
 * the callers: num_sign_bit_copies proves `(u8)x - 1` already sign-extends from
 * HImode (range -1..254) so a SIGNED narrowing is elided, while the UNSIGNED
 * one is real (-1 becomes 0xffff). So the third parameter is u16 in the first
 * and s16 in the other two. The fourth is a literal 0 / 0xff / 1 / -1 and -1 is
 * materialised as `movs #1; rsbs`, never `movs #0xff`, so it is signed and
 * `int` is the weakest model.
 *
 * sub_08020EDC takes six: three s16 coordinates, a `u8 *` into the
 * gUnknown_08499590 map (`gUnknown_08499590 + 0x1E42 + sel * 1288`), then two
 * stack words. Arguments 5 and 6 are `int`: sub_080210C8 sign-extends its own
 * s8 fifth parameter (`lsls #0x18; asrs #0x18`) and zero-extends its own u8
 * sixth (`lsls #0x18; lsrs #0x18`) before storing each to the outgoing frame,
 * which is the widening a narrow argument into an `int` parameter costs -- a
 * narrow parameter here would have needed no instruction at all. */
/* Promoted in src/decomp/c_080223E0.c with no declaration anywhere; signature
 * copied verbatim from that definition. */
void sub_080223E0(u16, u16);
/* wave 49: 4th parameter corrected int -> u8, the same correction wave 41 made
 * to the twin sub_080200EC below and for the same reason. The definition's
 * prologue narrows r3 with `lsls #0x18 / lsrs #0x18`, which agbcc does not emit
 * for an int parameter, so the width is 8 bits; PROMOTE_MODE zero-extends
 * either signedness so the prologue alone does not settle the sign, but the
 * only call site (src/decomp/c_080201E0.c's twin aside, here c_08020354.c)
 * passes the literals 0 and 0xff, and 0xff is materialised as `movs #0xff`
 * rather than the `movs #1; rsbs` a negative would need -- so unsigned.
 * sub_08020354 re-verified by trymatch exit code after the change. */
void sub_0801F9C0(u16, u16, u16, u8);
/* wave 41: 4th parameter corrected int -> s16. The definition's prologue
 * narrows r3 with `lsls #0x10 / lsrs #0x10`, which agbcc does not emit for an
 * int parameter -- so the width is 16 bits and the old `int` was a guess made
 * before any definition existed. Byte-neutral at the only call site
 * (src/decomp/c_080201E0.c passes the literals 1 and -1); sub_080201E0
 * re-verified by trymatch exit code after the change. */
void sub_080200EC(s16, s16, s16, s16);
/* Wave 49, W49-I. 4th parameter CORRECTED int -> s16, the same correction wave
 * 41 made to the twin sub_080200EC above and for the same reason: the
 * definition's prologue narrows r3 with `lsls #0x10 / lsrs #0x10`, which agbcc
 * does not emit for an int parameter. The body then reads that 16-bit stack
 * slot back with `ldrb` for its `gUnknown_03003340[yy][xx] =` store. Signedness
 * is not settled by the prologue (PROMOTE_MODE zero-extends either), but the
 * only caller (src/decomp/c_08020D50.c) passes 0 and -1 and -1 is materialised
 * as `movs #1; rsbs` -- so signed. Byte-neutral there; c_08020D50 re-verified
 * by try_match exit code after the change. */
void sub_08020B88(s16, s16, s16, s16);
void sub_08020EDC(s16, s16, s16, u8 *, int, u8);

/* Wave 36, W36-M. Every signature below is COPIED VERBATIM from a byte-verified
 * definition in src/decomp/ -- none of them had a declaration in any header,
 * which blocked the 0x08028-0x0802D block. Do not re-derive them from argument
 * registers; the definition outranks that. */
u32 sub_08012E4C(void);                              /* c_08012E4C.c */
void sub_080251BC(int, int, struct Unk802C57C *);    /* c_080251BC.c */
void sub_08037200(u16, u16, u16, u16);               /* c_08037200.c */
u8 sub_0803EED4(int, int);                           /* c_0803EED4.c */
int sub_080249C8(int);                               /* c_080249C8.c */
const u8 *sub_0802A880(int, int);                    /* c_0802A880.c */
void sub_0802E250(void);                             /* c_0802E250.c */
void sub_0802A7C4(void);                             /* c_0802A7C4.c */
void sub_0802DBE4(void);                             /* c_0802DBE4.c */
void sub_0803A59C(void);                             /* c_0803A59C.c */
void sub_0803A8F0(struct UnitRecord *);             /* c_0803A8F0.c */
void sub_080470F8(u16);                              /* c_080470F8.c */
struct UnitRecord *sub_08025580(void);              /* c_08025580.c */
/* VARARGS, and that is the whole reason 0x08090AC4-0x08090B44 is full of
 * printf-style strings: the debug overlay in sub_080281F0 / sub_080283E4 is a
 * run of sub_08013428(x, y, "SNOW:%s", ...) calls. */
void sub_08013428(int, int, const char *, ...);      /* c_08013428.c */
/* RETYPED wave 36 (W36-M): c_0802D9B8.c defined it over a local
 * `struct Unk2D9B8Proc` whose three s16 members are exactly struct
 * Unk03001470's unk1e/unk20/unk22, and its ONLY caller sub_0802DA18 passes
 * `&gUnknown_03001470[i]`. The definition was retyped to agree; byte-identical,
 * re-verified with try_match. */
void sub_0802D9B8(struct Unk03001470 *);             /* c_0802D9B8.c */
/* Undeclared and unpromoted. Read off sub_0802DCB4's call site, which passes
 * gUnknown_030033E4's two halves read as s16 OBJECTS and re-narrows the result
 * with `lsls #0x18` -- the same call shape as its neighbour sub_0802E4B4. */
u8 sub_0802E2D0(s16, s16);

/* ---- wave 37 (W37-C) ----
 * The 0x0800A/0x0800B cell-update block. Every one takes the same (x, y) cell
 * key as a bare `adds rN, r0, #0` / `adds rN, r1, #0` pair with no narrowing,
 * so `int` on both.
 *   sub_08007F68 and sub_0800AA30 are already promoted (c_08007F68.c,
 * c_0800AA30.c) and were only missing a prototype; these agree with the
 * definitions.
 *   sub_08009918 returns 0/1 built with `movs r0,#0` / `movs r0,#1`; its one
 * caller sub_0800A95C shifts the result left by up to 8 with no narrowing, so
 * `int`.
 *   sub_0800A098 and sub_0800A588 are void: every call site drops the result
 * and both end `pop {r0}; bx r0` after a non-call.  sub_0800A95C's result is
 * an `ldrsh` out of a ROM table that sub_0800A588 tests with `cmp r2,#0` both
 * `bge` and `ble`, so it is a signed value -- `int`, on the same reading as
 * sub_0800A798. */
void sub_08007F68(int, int, int);
int sub_0800AA30(int, int, int);
int sub_08009918(int, int);
void sub_0800A098(int, int);
void sub_0800A588(int, int);
int sub_0800A95C(int, int);
/* sub_08009538 returns 0/1 (`movs r0,#0` / `movs r0,#1`), tested with a bare
 * `cmp r0, #0` by its one caller.  sub_0800A3D4 is promoted in
 * src/decomp/c_0800A3D4.c as void(int, int); this only adds the prototype.
 *   NOTE on the s16 sub_0800B61C declared above (wave 36 block): sub_0800BB2C
 * corroborates it -- that caller re-narrows with `lsls #0x10; asrs #0x10` too.
 * sub_0800A95C is the near-twin that does NOT get an s16 return despite the
 * identical body shape (`ldrsh` out of a ROM table, returned straight): its
 * caller sub_0800A588 uses the result with no narrowing at all, so an s16
 * return there would add a `lsls #16; asrs #16` the ROM does not have.  The
 * two together are the reminder that the return width lives at the CALLERS. */
int sub_08009538(int, int);
void sub_0800A3D4(int, int);
void sub_0800BB2C(int, int);
/* Same (x, y) key, same bare `adds rN, r0, #0` entry, no narrowing anywhere.
 * sub_0800168C's result is only ever `cmp r0, #0` (sub_0800AF74), so int.
 * sub_0800BEB8 is promoted void(int, int) in src/decomp/c_0800BEB8.c and was
 * missing only a prototype.  sub_0800A2EC, sub_0800AF24 and sub_0800BEE4 end
 * `pop {r0}; bx r0` after a non-call, so void; sub_0800A6AC and sub_0800A884
 * return an `ldrsh` table entry that is negative on the reject path, so int on
 * the same reading as sub_0800A95C. */
int sub_0800168C(int, int);
void sub_0800BEB8(int, int);
void sub_0800A2EC(int, int);
int sub_0800A6AC(int, int);
int sub_0800A884(int, int);
void sub_0800AF24(int, int);

/* Wave 37 (W37-E). The 0x0800xxxx map-cursor block. Every arity below was read
 * off the CALLEE's own prologue, not off a call site; every result is discarded
 * at every call site found in asm/, and each of these ends `pop {rN}; bx rN`,
 * so `void` is the return type unless noted.
 *
 * The seven nullary ones are sub_0800057C's jump-table arms (cases 0..7 of
 * gUnknown_0200B0B0->unk04); none of them reads r0-r3 before writing it. */
void sub_080005FC(void);
void sub_08000650(void);
void sub_08000664(void);
void sub_08000694(void);
void sub_0800081C(void);
void sub_08004CA0(void);
void sub_08005F4C(void);

/* Nullary: its first act is `bl sub_08025E74`, so nothing in r0-r3 is live. */
void sub_08003B8C(void);

void sub_08000BF8(void);
void sub_08000C68(void);
void sub_08000CCC(int);
void sub_08000DF8(int);

/* `int` return: sub_08000CCC keeps the result in r0 and does signed modular
 * arithmetic on it (`subs #4; bge; adds #0x11`), with no re-narrowing after the
 * `bl`. One argument -- r1 onwards is dead on entry. */
int sub_08001D24(int);
void sub_080073F8(int, int);

/* sub_08002510's first parameter is DEAD in the body (r0 is never read before
 * being written) but real: sub_08001DAC passes it, exactly as sub_080030BC's
 * a2/a3 are declared-and-unread. The second is masked `& 0xff` and becomes
 * sub_0801F34C's third argument. */
void sub_08002298(int, int);
void sub_08002510(int, int);

/* Wave 50, W50-G. Both are nullary and void: sub_08002AB0 and sub_08002C38
 * `bl` each of them with no argument setup at all and discard r0, and neither
 * callee reads r0-r3 before writing them (sub_08007B54's first instruction
 * after the push loads its own pool word; sub_08007B74's loads
 * gUnknown_0200B0B0). */
void sub_08007B54(void);
void sub_08007B74(void);

/* Arities from sub_08001DAC's call sites, which put 5, 6 and 7 words in place;
 * sub_0800272C's is additionally fixed by its promoted definition in
 * src/decomp/c_0800272C.c and must agree with it. */
void sub_0800272C(int, int, int, int, int, int, int);
void sub_08002844(int, int, int, int, int, int, int);
void sub_08002964(int, int, int, int, int, int);
void sub_080029F4(int, int, int, int, int, int);
void sub_08003088(int, int);

/* Two out-parameters, both written with a full-word `str` in the callee body,
 * and an int return tested `cmp r0, #0` by both readers. */
int sub_0800C6E8(int, int *, int *);


/* ---- Wave 37 (W37-H): block 0x08035 predicates ----
 * sub_08035C90 and sub_08035CF4 are both narrowed by their one caller
 * sub_08035D0C with `lsls r0,#0x18; cmp r0,#0`, so both return u8 -- the bare
 * `cmp r0,#0` an int return would give is not what the ROM has.  That is why
 * src/decomp/c_08035CF4.c's `int sub_08035CF4(void)` was retyped to
 * `u8 sub_08035CF4(ProcPtr)` this wave: the caller also loads r0 with the proc
 * (`adds r0, r4, #0`) immediately before the `bl`, so the argument is real even
 * though the body ignores it.  Both re-verified byte-exact after the change.
 *   sub_08035D0C's result is re-narrowed `lsls #0x10; lsrs #0x10` by
 * sub_08035E90, so u16 and not u8: its returned constants reach 0x4b.
 *   sub_08035080 is nullary (r0 is written before it is read in its prologue)
 * and returns 0/1, narrowed `lsls #0x18` by sub_08035170.
 *   sub_080129F8 is the same story one block over: it is promoted in
 * src/decomp/c_080129F8.c and had no prototype at all; its only caller
 * sub_08035170 narrows with `lsls #0x18`, so the definition was retyped from
 * `int` to `u8` (byte-exact before and after -- it returns 0/1 constants). */
u8 sub_08035C90(ProcPtr);
u8 sub_08035CF4(ProcPtr);
u16 sub_08035D0C(ProcPtr);
u8 sub_08035080(void);
u8 sub_080129F8(u16);

/* ---- Wave 37 (W37-F): blocks 0x08009 and 0x0800F, map-tile predicates ----
 * All take the same (x, y) key as the rest of the gUnknown_08499590 map family;
 * every arity was read off the callee's own prologue (r0 and r1 saved, r2 never
 * read), not off a call site.
 *   sub_0800F2E0 is already promoted `int (int, int)` in src/decomp/c_0800F2E0.c
 * and was missing only a prototype.
 *   sub_0800F318 and sub_0800F368 return 0/1 built with `movs rN,#0` /
 * `movs rN,#1`; sub_0800F3B8 and sub_0800F418 shift those results left by up to
 * 7 and `orrs` them together with no narrowing, so int.
 *   sub_0800F3B8 returns that same 8-bit mask, unnarrowed, so int.
 *   sub_08009310 returns 0/1 the same way.
 *   sub_08009264 sets no return value on either exit path and ends
 * `pop {r4,r5,r6}; pop {r0}; bx r0` after a `bl sub_08007F9C` whose own result
 * is void, and its one caller sub_0800F4E0 (itself void) drops it: void.
 *   sub_08009CF8's result is only ever turned into a 0/1 by sub_08009918's
 * `rsbs; orrs; lsrs #0x1f` (a returned `!= 0`), so nothing narrows it: int. */
void sub_08009264(int, int);
int sub_08009310(int, int);
int sub_08009CF8(int, int);
int sub_0800F2E0(int, int);
int sub_0800F318(int, int);
int sub_0800F368(int, int);
int sub_0800F3B8(int, int);

/* ---- Wave 37 (W37-G): blocks 0x08010, 0x08016, 0x08018, 0x0801B ----
 * The (x, y) map-key family again -- every arity below was read off the
 * callee's own prologue in asm/code.s, not off a call site.
 *   sub_0800C574 takes THREE: it saves r0/r1/r2 into r5/r6/r7 before touching
 * anything, and ends `pop {r4,r5,r6,r7}; pop {r0}; bx r0`, which destroys r0,
 * so it cannot return a value.
 *   sub_0800F564 takes three (r2 is copied to r6 first and indexes the
 * gUnknown_0848894C / gUnknown_08488954 direction pair); its result is
 * compared against 2 by sub_08010B34 with no narrowing, so int.
 *   sub_0800F8D4 takes two and its result is only zero-tested: int.
 *   sub_0800FD44 takes three (`cmp r2,#0; bne` in the prologue) and its result
 * is forwarded straight into sub_08001158's int third parameter.
 *   sub_08018194 narrows its one argument `lsls #0x18; lsrs #0x18`, so u8, and
 * ends `pop {r0}; bx r0`: void.
 *   sub_080179AC reads no argument register and ends `pop {r0}; bx r0`: void.
 *   sub_0801B7C0 is the opcode dispatcher of the gUnknown_0808EF64 byte-stream
 * (see gUnknown_0808EF64's note): r0 is dereferenced `ldrb r0,[r7]` so it is a
 * pointer, r1 is opaque, and it returns -1 for "no slot".
 *   sub_0801BA1C narrows its second argument `lsls #0x10; lsrs #0x10` (u16),
 * loops on the third with `cmp r2,#0; ble` (signed int) and `strh`s through
 * the first, so that one is a pointer; `pop {r0}; bx r0`, so void.
 *   sub_08010604 / sub_08010DD4 / sub_08010B34 take the (x, y) pair as plain
 * ints (no prologue narrowing) and return an int tile id / 0/1 / -1.
 *   sub_0801659C narrows its one argument `lsls #0x18; lsrs #0x18` and always
 * returns 0, the same bool8 (u8) shape as its sub_080161B4 neighbours. */
void sub_0800C574(int, int, int);
int sub_0800F564(int, int, int);
int sub_0800F8D4(int, int);
int sub_0800FD44(int, int, int);
int sub_08010604(int, int);
int sub_08010B34(int, int);
int sub_08010DD4(int, int);
bool8 sub_0801659C(u8);
void sub_080179AC(void);
void sub_08018194(u8);
int sub_0801B7C0(const char *, int);
void sub_0801BA1C(void *, u16, int);
/* Wave 40 (W40-G). Both are already DEFINED in src/decomp/ and the definition
 * wins: sub_0801B738 in src/decomp/c_0801B70C.c (an IWRAM-overlay trampoline,
 * four arguments, first u8) and sub_0801B964 in src/decomp/c_0801B964.c (the
 * paired-counter advance returning 1 on wrap). sub_0801B8D0 is the first
 * cross-file caller of either. It truth-tests sub_0801B964's result through
 * `lsls #0x18` with no `lsrs`, which is a u8 CAST at the call site and not a
 * narrow return type -- the definition returns int. */
int sub_0801B738(u8, int, int, int);
int sub_0801B964(int);
/* sub_0801B8D0(str, vram, kind) -- the glyph-run renderer, matched in wave 40
 * and called only by sub_0801B7C0. It walks a NUL-terminated string, hands each
 * byte to sub_0801B738 with a running x, and returns `Div(w + 7, 8) * 2`, a
 * width in half-tiles; sub_0801B7C0 both adds that to its halfword cursor and
 * stores `w >> 1` (an ARITHMETIC shift) into a u8, which is what makes the
 * return plain int rather than unsigned.
 *   The second parameter is `int` and not a pointer even though every caller
 * builds a VRAM address for it: it is forwarded to sub_0801B738's int second
 * parameter, whose promoted definition wins. The caller keeps the
 * `(u8 *)(charbase) + (tile * 32 + 0x06000000)` association documented in
 * src/decomp/c_0801B780.c and casts the whole sum back to int, which is free. */
int sub_0801B8D0(const u8 *, int, int);
/* sub_0801BAB8 is a lone `bx lr` -- the do-nothing default entry sub_0801BABC
 * writes into all 15 slots of gUnknown_03002FE0. Its address is only ever
 * TAKEN, never called here, so `void (void)` records nothing but the fact that
 * it is a function. IrqMain is crt0.s's ARM interrupt entry, which sub_0801BABC
 * CpuFastSets into IWRAM; declared so that pool word can be named. */
void sub_0801BAB8(void);
void IrqMain(void);
/* Already DEFINED in src/decomp/c_0804B0CC.c; signature copied from the
 * definition, which wins. sub_080048D4 is its first cross-file caller and
 * passes `(int)&gUnknown_0200B0B0->unk9c` -- the int first parameter is the
 * definition's, so the pointer is cast at the call site. */
void sub_0804B10C(int, u8);
/* Already DEFINED in src/decomp/c_0801820C.c; sub_08018254 is its first
 * cross-file user and only takes its address, storing it into
 * gUnknown_0200C528[a].unk08 through a `void *` cast. Signature copied from
 * the definition, which wins. */
void sub_0801820C(struct Unk0200C528 *);

/* Wave 37 (W37-H). sub_08038848 pushes one step onto the gUnknown_0849D5F8
 * move stack. Both parameters are s8: the entry pair is `lsls #0x18; lsrs
 * #0x18` (PROMOTE_MODE, which says nothing) but every use inside is
 * `lsls #0x18; asrs #0x18`, and its one caller sub_08038C98 reads the first
 * with a bare `ldrsb` and sign-extends the second. */
void sub_08038848(s8, s8);

/* Promoted in src/decomp/c_080386DC.c as void(int, int); this only adds the
 * prototype its caller sub_08038C98 needs (wave 37, W37-H). */
void sub_080386DC(int, int);

/* Already defined in src/decomp/c_08012F6C.c with exactly this signature;
 * nothing declared it, so its one caller sub_080729EC could not see it
 * (wave 37, W37-N). It is the CpuFastSet/CpuSet data-move front end. */
void sub_08012F6C(const void *src, void *dst, int size);

/* Wave 37 (W37-H) -- FINDING, NOT YET APPLIED: sub_08015438's FOURTH
 * parameter is declared `void *` and is not one. Every caller that exists in C
 * today passes 0 or NULL there (c_08027A50, c_080393CC, c_0803B264, and
 * c_08015410 which only forwards), so the wrong type has been invisible.
 * sub_08035BC4 -- still unmatched, parked in work/sub_08035BC4/ -- passes a
 * sign-extended s16 VALUE in r3 (`lsls r3,#0x10; asrs r3,#0x10` immediately
 * before the `bl`), which no pointer expression produces. The fix is `int` for
 * parameter 4 here and in src/decomp/c_08015438.c, plus `NULL` -> `0` in
 * src/decomp/c_08027A50.c; it is byte-neutral (same register, same width) but
 * it touches five promoted files, so it needs a wave with budget to re-run
 * try_match on all of them. */

/* Wave 37 (W37-I). sub_08034A7C is defined in src/decomp/c_08034A7C.c and was
 * never declared here; sub_08034AF8 is its first caller outside its own unit.
 * Both parameters are plain `int` -- the y arrives as a `movs #0x4e` immediate
 * and the second as a bare `ldrb` of gUnknown_08499598[i].unk1a. */
void sub_08034A7C(int, int);
/* Byte-returning predicate: every caller re-narrows the result with
 * `lsls #0x18` before testing it (sub_08034AF8, and the guard the
 * gUnknown_08499598.unk2d note describes). */
bool8 sub_08026D44(int);
void sub_08026F04(void);
void sub_080268F4(void);
void sub_08044178(int);
void sub_0802BFBC(void);
void sub_08034C8C(void);
/* sub_0803D788 calls it as (gUnknown_08499584, 0, 0x800) with the length built
 * once in a callee-saved register and copied into r2 -- arity 3, no width
 * evidence beyond the register.
 *
 * WAVE 37: this was first declared `(void *, int, int)` from that caller-side
 * reading alone, which proto_check.py then flagged against the ALREADY-PROMOTED
 * definition in src/decomp/c_08013098.c: `void sub_080130C8(u16 *dst, int delta,
 * int size)`. The definition wins -- it is byte-verified, and the brief's rule is
 * that a promoted definition beats "the weakest type that fits". This is the same
 * error that broke wave 14's first SPLIT=1 build (`void *` declared as the weakest
 * model against a promoted named type).
 *
 * Agreeing costs nothing here and is strictly better: gUnknown_08499584 is itself
 * declared `u16 *`, so the sole call site now type-checks exactly rather than
 * decaying through void *. */
void sub_080130C8(u16 *, int, int);

/* Wave 37 (W37-J3), block 0x08056.
 *
 * sub_0805601C passes FIVE arguments to sub_080560A4 -- r0..r3 plus one stack
 * word -- but that callee's own prologue narrows only r0 and r1, so the last
 * three are dead inside it and their widths are the caller's. Every argument
 * at both call sites is one of sub_0805601C's own u16 parameters or a small
 * constant. sub_0805634C narrows all three of its arguments at entry. */
void sub_080560A4(u16, u16, u16, u16, u16);
void sub_0805634C(u16, u16, u16);
/* Wave 49, W49-M, block 0x08056. sub_0805634C computes
 * `t = sub_0805653C(c, b)` -- note the REVERSED argument order at that call
 * site, the third parameter first -- and passes t on to sub_080564B8 as its
 * third argument.
 *   The RETURN TYPE is 32-bit and UNSIGNED, and both halves of that are read
 * off the caller rather than the body: `mov r8, r0` after the `bl` with NO
 * re-narrowing rules out a u16 return (agbcc re-narrows a narrow-returning
 * callee's result at every call site), and the two loops that then use it as a
 * bound against a u16 counter compare `blo`/`bhs`, which an `int` would have
 * made `blt`/`bge`. The body itself only ever returns 0, 1 or its own u16
 * first parameter. */
u16 sub_0805653C(u16, u16);
/* Wave 49, W49-M. Arities off the two call sites in this block: sub_0805634C
 * calls sub_080564B8 with r0/r1/r2 and sub_08056638 with r0 alone, and both
 * callees narrow exactly those registers at entry. Widths from those
 * prologues. */
void sub_080564B8(u16, u16, u16);
void sub_08056638(u16);
void sub_0805601C(u16, u16, u16, u16);
void sub_08056D8C(u16, u16, u16);
/* sub_08056EEC fills a six-halfword stack record and passes its address -- the
 * same shape, and the same `void *` spelling, as sub_08057048/sub_080570C4.
 * The record's layout is described in sub_08056EEC's own file. */
void sub_08056F8C(void *);
/* The second argument is never read: sub_08056EEC narrows r0 and r2 and leaves
 * r1 alone. Arity 3 is from the r2 use, not from a caller -- nothing calls it
 * yet. */
void sub_08056EEC(u16, u16, u16);
/* Promoted in src/decomp/c_080573F0.c and never declared until now; the file
 * defined it with no prior prototype in scope. void/void off the definition. */
void sub_080573F0(void);

/* ---- wave 37 (W37-O1), the 0x08019000 block ---- */

/* Four argument-free, result-discarded callees of sub_080191B0. Read off each
 * callee's own prologue, not off the call site: sub_0803CB8C, sub_080198C4 and
 * sub_0801797C each write a literal-pool global before touching an argument
 * register, and sub_08017A0C's prologue loads its own pool words first. */
void sub_0803CB8C(void);
void sub_080198C4(void);
void sub_0801797C(void);
void sub_08017A0C(void);

/* Wave 40, W40-E. NO callers anywhere in the tree, so neither the return type
 * nor its existence is constrained by anything -- the body ends `movs r0,#0`
 * before the pop, and `int` is the natural-C reading of that, not a proof.
 * The parameter IS settled: it is sign-extended once for the loop's `!=` test
 * and the pre-shifted copy is re-`asr`ed for the gUnknown_0200C528 index. */
int sub_08017F0C(s16);

/* Its one caller (asm/code-0806CFC8.s, inside sub_08074384) does
 * `ldr r0, [r4, #4]; cmp r0, #0; beq ...; bl sub_08019348` and then ignores
 * r0, so: one argument, result discarded. The argument is the same `const u8 *`
 * script pointer sub_080193B0 takes -- sub_08019348 forwards it unchanged on
 * one path and stores it into gUnknown_0200C508 on the other. */
void sub_08019348(const u8 *);

/* A gUnknown_0200C528 list-script handler in the c_0801903C / c_080190EC
 * family: s16 slot index, `movs r0, #1` before the epilogue. Never reached by
 * a `bl` -- it is installed as a table word like the rest of that family. */
bool8 sub_0801906C(s16);


/* ---- wave 37 (W37-K1) ----
 * The 0x0805A / 0x0805D adjacent-cell probe family: the twin of the
 * sub_08058BB4 / sub_08058C54 / sub_08058CE8 / sub_08058E88 block already
 * matched in src/decomp/c_08058BB4.c, and typed off that block.
 *
 * sub_0805ACFC and sub_0805C128 are the per-cell testers. Both open
 * `adds r4,r0,#0 / adds r5,r1,#0 / adds r6,r2,#0` with no PROMOTE_MODE shift
 * pair, so both coordinates are `int`; both end `pop {r4,r5,r6}; pop {r0};
 * bx r0`, so both are void. The out-pointer is `u16 *` and not `s16 *`:
 * sub_0805ACA8 and sub_0805A854 read slot 0 back with a plain `ldrh` for the
 * 0x270F sentinel test, the same discriminator c_08058BB4.c records.
 *
 * sub_0805ACA8 and sub_0805A854 are the four-neighbour drivers. Each returns
 * 1/0 as `movs r0,#1` / `movs r0,#0` split across a `b`, which is the one
 * spelling `return <cmp>;` cannot produce, so `int` and an explicit if/else.
 * sub_0805A854 takes the cell as a `u16 *` pair and updates it in place.
 *
 * sub_0805A5E0 stores a whole word through its one argument
 * (`movs r0,#1; rsbs r0,r0,#0; str r0,[r7]`) and ends `pop {r0}; bx r0`, so
 * `void (int *)` -- an out-parameter, not a return value.
 *
 * sub_0805D648 takes FIVE arguments: r0-r3 plus one stack word, set up as
 * `movs r2,#0; str r2,[sp]` before r2 is reloaded with the third. In
 * sub_0805DA84 r0 and r1 arrive as `movs rK,#0 / ldrsh` off a `u16` pair, so
 * they are `s16` parameters -- declaring them `u16` emits `ldrh` at that call
 * site instead and does not match -- and r3 arrives as `lsls #0x18;
 * lsrs #0x18` off an `int`, so `u8`.
 *
 * sub_08058BB4 is already DEFINED in src/decomp/c_08058BB4.c and had no
 * declaration anywhere; this publishes that definition unchanged (re-verified
 * with try_match after the edit). Its `u16` first parameter is what makes
 * sub_0805DA84's `lsls #0x10; lsrs #0x10` appear in front of the `bl`. */
void sub_0805ACFC(int, int, u16 *);
/* Wave 48, W48-F: u8, not int -- same correction as sub_0805BA34/sub_0805BB8C/
 * sub_0805BC7C in this block. sub_0805A514 re-narrows the result with
 * `lsls #0x18; lsrs #0x18` before `cmp r0,#1`, which agbcc does not emit for an
 * int return. Byte-neutral in src/decomp/c_0805ACA8.c (only 0 and 1 are
 * produced) and re-verified there after the change. */
u8 sub_0805ACA8(int, int, u16 *);
void sub_0805C128(int, int, u16 *);
int sub_0805A854(u16 *);
void sub_0805A5E0(int *);
/* Wave 52, W52-D. sub_0805A268, sub_0805A388 and sub_0805A514 are DELIBERATELY
 * NOT DECLARED HERE, and the reason is worth recording because two waves have
 * now been tempted to add them. All three are already DEFINED, in
 * src/decomp/c_0805A268.c and c_0805A514.c, taking `struct Unk5A514Cell *` --
 * a FILE-LOCAL tag repeated verbatim in three promoted files
 * (c_0805A268.c, c_0805A514.c, c_0805A744.c) and therefore unnameable from a
 * shared header. Declaring them here with any other pointer type is a real
 * cross-unit disagreement that `try_match` cannot see; `tools/proto_check.py`
 * catches it and did. A caller repeats the tag body and casts at the call
 * site instead -- see work/sub_0805E160 and work/sub_0805E87C.
 *   The caller-side evidence, for whoever eventually merges the types: all
 * three take the dereferenced gUnknown_03003F20 in r0, which
 * include/unknown-globals.h declares `struct Unk03003338 *`. So
 * struct Unk03003338 and struct Unk5A514Cell describe the same object and the
 * cast is not a coincidence.
 *
 * Wave 52, W52-D. NULLARY on a BARE PROLOGUE: sub_0805E2AC pushes, saves the
 * high registers and adjusts sp without copying r0 anywhere, and sub_0805E160's
 * only call to it sets up no arguments at all. W21-B's rule says a bare prologue
 * is evidence for a WIDE parameter rather than for none, so this is the weaker
 * of the two readings and is UNPROVEN -- sub_0805E160 matched byte-for-byte
 * either way, because r0 happens to hold the cell's x at that call and a
 * forwarded argument costs no instruction. Settle it from sub_0805E2AC's own
 * body when that function is derived. */
void sub_0805E2AC(void);
/* Wave 49, W49-G. CORRECTED from (s16, s16, int, u8, int): parameters 3 and 5
 * are u8, not int. sub_0805D648's own prologue narrows all five --
 * `lsls #0x10 / lsrs #0x10` on the first two and `lsls #0x18 / lsrs #0x18` on
 * the last three -- and agbcc emits no PROMOTE_MODE pair at all for an `int`
 * parameter. The first two stay s16 because both are sign-extended at their
 * uses (`lsls #0x10; asrs #0x10`) before an int comparison against
 * gUnknown_030040D8->unk02/unk03. Every caller is still in asm/, so this
 * costs nothing in the tree. */
void sub_0805D648(s16, s16, u8, u8, u8);
/* Wave 49, W49-G. sub_0805D5EC is DEFINED in src/decomp/c_0805D5EC.c and had no
 * declaration; this publishes it unchanged so sub_0805D648 can call it. */
void sub_0805D5EC(void);
/* Wave 49, W49-G. sub_08071910 is the linker's THUMB->ARM veneer for
 * sub_08000554, which data/asm-resident.json records and which is LONGJMP:
 * `ldm r0!, {r4-fp, ip, sp, lr}` then `movs r0,r1; moveq r0,#1; moveq pc,lr`.
 * Declaring it NORETURN is what the ROM requires, not a convenience:
 * sub_0805D648 ends with `bl sub_08071910` and NO epilogue, and its first call
 * site is followed directly by a literal pool, so agbcc must know control does
 * not come back. Spelled against the veneer rather than sub_08000554 because
 * the veneer is the symbol the ROM's `bl` encodes; naming the ARM function
 * would make the linker synthesise a second one. */
void sub_08071910(u8 *, int) __attribute__((noreturn));
int sub_08058BB4(u16, u16 *);
/* Wave 49, W49-D. Already DEFINED in src/decomp/c_08058BB4.c but never declared;
 * added because sub_080587FC calls it. Signature copied from the definition. */
int sub_08058C54(int, int, u16 *);
/* Wave 49, W49-D. Declared from its only call site (sub_080587FC) plus its own
 * body: it parks r0 in sb and finishes `mov r1,sb; str r0,[r1]`, a WORD store,
 * and returns 0 or -1. */
int sub_08058A2C(int *);
/* Wave 51, W51-K.  sub_0805878C is DEFINED in src/decomp/c_0805878C.c and was
 * never declared; this publishes that definition unchanged so sub_0805E718 can
 * call it. */
struct Unk03003338 *sub_0805878C(void);
/* Wave 51, W51-K.  sub_080587FC TAKES AN ARGUMENT, which its only call site
 * (sub_0805E718) hides completely: the ROM there is `bl sub_08058744;
 * bl sub_080587FC` with no argument register written in between, because
 * sub_08058744's result is already in r0.  This is the wave-50/51 arity rule --
 * a callee whose argument is already live costs no instruction at the call.
 * sub_080587FC's own prologue is `str r0, [sp, #8]`, a WORD spill with no
 * PROMOTE_MODE narrowing, and it reloads that slot for two `cmp r0, #0` tests,
 * so `int`.  It returns the record count `(p - gUnknown_03003338) >> 3` and
 * sub_0805E718 tests it against 0. */
int sub_080587FC(int);

/* Wave 37, W37-K2: the undeclared callees reached from the 0x0805D / 0x08060 /
 * 0x08061 blocks. Arity is read off the argument registers written immediately
 * before each `bl`, and void-ness off whether r0 is live afterwards.
 *
 * sub_08061868, sub_08061B00, sub_080606D0, sub_0805D438 and sub_08061AC4 are
 * five of the six arms of sub_0806171C's jump table: each is reached with no
 * argument register written and its result is discarded by a `pop {r0}`
 * epilogue. sub_08061AC4's is copied verbatim from the promoted definition in
 * src/decomp/c_08061AC4.c, which wins over any guess here.
 *
 * sub_08061178 takes `(u8)(gUnknown_030046C0.unk06 - 1)` -- the caller's
 * `subs #1; lsls #0x18; lsrs #0x18` is PROMOTE_MODE's caller-side narrowing for
 * a u8 parameter -- and its result is stored with a bare `strb`, so u8 out.
 * sub_080611D8 fills a 4-byte stack object whose two halves are then read with
 * `ldrh` and handed to sub_08025E08, and its result is tested with
 * `lsls #0x18; cmp #0`, so it returns a byte.
 *
 * sub_08058144 returns a POINTER: sub_0805DFF4 tests the result against 0 and
 * then reads bytes +1 and +2 off it, which is struct UnitRecord's unk01/unk02
 * pair.
 * WAVE 45, W45-E: the `struct UnitRecord *` here is WRONG and I left it alone
 * rather than break a promoted file. The matched body returns
 * `&gUnknown_084995A0[v]` -- the ROM scales v by 8 (`lsls #3`) and reads
 * `unk03[a2]` at +3, which is struct PropertyListEntry (0x08), not UnitRecord
 * (0x0c, and it is reached by a *3*4 chain everywhere else). The +1/+2 evidence
 * above does not discriminate: PropertyListEntry's filler_00[3] covers those bytes
 * too. Fixing this means retyping to `struct PropertyListEntry *`, naming
 * filler_00's three bytes, and editing src/decomp/c_0805DFF4.c (its local `p`)
 * -- byte-neutral in both, but it touches a promoted file, so it wants an
 * orchestrator's re-sweep rather than a mid-wave unilateral edit.
 * sub_08057F00's result is __divsi3's dividend, so `int`. sub_08059A0C's
 * is only tested against 0. sub_0805BFDC takes four registers and nothing on
 * the stack. sub_080591E4 and sub_0805C0AC both take the address of the same
 * 4-byte (u16, u16) stack object, so `void *` is the weakest model that fits
 * both. */
void sub_080606D0(void);
void sub_08061868(void);
void sub_08061AC4(void);
void sub_08061B00(void);
void sub_0805D438(void);
u8 sub_08061178(u8);
u8 sub_080611D8(void *);
/* Wave 49, W49-E. sub_08061668's signature is COPIED FROM the promoted
 * definition in src/decomp/c_08061668.c, which wins over any reading here.
 * RECORDED, NOT ACTED ON: its only C caller, sub_080611D8, narrows the result
 * to a byte before testing it (`lsls r0,#0x18; cmp r0,#0`), which is the
 * caller-side evidence for a `u8` return -- and per the brief a return type is
 * settled from the callers. Retyping it to `u8` is byte-neutral inside
 * c_08061668.c itself (its body only ever returns the constants 0 and 1), so
 * the change is safe, but it touches a promoted file and wants an
 * orchestrator's re-sweep rather than a mid-wave unilateral edit.
 * sub_080611D8 therefore reproduces the narrowing with a u8 local.
 *
 * sub_08061308 takes (unit-class byte, small mode selector, the same 4-byte
 * (u16, u16) object sub_08061668 fills) and its result is compared against 1
 * after `lsls #0x18; lsrs #0x18`, so it returns a byte. All five call sites are
 * in sub_080611D8 and all five pass the same three operand classes. */
int sub_08061668(u16 *);
u8 sub_08061308(u8, u8, u16 *);
void sub_080610D0(void);
void sub_0806056C(u8);
void sub_0805E440(void);
void sub_0805F4F8(void);
void sub_0805FB70(void);
/* Wave 51, W51-N. Callees of the 0x08059/0x0805F battle-cursor block that had
 * no declaration. All four take the `union Unk802C57CBuf` scratch cell as a
 * `void *` out-parameter, the same contract sub_080591E4 and sub_08059C00
 * already carry.
 *   sub_0805FC1C's first argument is the 3-bit field at gUnknown_030040D8+9
 * bits 3..5, arriving zero-extended (`lsls #0x1a; lsrs #0x1d`), so `int` is the
 * weakest fit.
 *   sub_0805C988 and sub_0805A8C0 both return a BYTE: sub_08059674 tests the
 * first with a bare `lsls #0x18` truth test and re-narrows the second with
 * `lsls #0x18; lsrs #0x18` before `cmp #1`. Their parameter widths are NOT
 * pinned -- sub_08059674 hands them cell coordinates that are already
 * sign-extended for its own map arithmetic, so CSE supplies the `asrs` either
 * way and `s16` would emit the same bytes. `int` is the weakest fit. */
void sub_0805FC1C(int, void *);
int sub_0805C988(int, int);
/* sub_0805A8C0 IS DELIBERATELY NOT DECLARED HERE. Wave 51, W51-N measured both
 * sides and they genuinely disagree, so the original cannot have had a
 * prototype in scope at its caller:
 *   - the DEFINITION needs `u16` (src/decomp/c_0805A8C0.c). Its ROM prologue
 *     zero-extends BOTH parameters itself (`lsls #16; lsrs #16` twice);
 *     retyping them to `int` deletes those four bytes and drops the function to
 *     10.9%.
 *   - the CALLER sub_08059674 passes values it has just SIGN-extended (`asrs
 *     #16`) with no conversion instruction between them and the `bl`. Adding
 *     `u16` parameters to a visible prototype makes agbcc insert `lsrs #16` at
 *     the call, which keeps the u16 copies live across the whole body and costs
 *     r8 -- +12 bytes, 16.5%.
 * Both are consistent with K&R default promotion: no prototype in scope, so the
 * caller passes ints and the callee narrows its own parameters at entry.
 * No declaration here can serve both. Measured, all four:
 *   `(u16, u16)` -> caller 16.5%;  `(int, int)` -> definition 10.9%;
 *   omitted entirely -> agbcc runs warnings-as-errors and rejects the implicit
 *   declaration;  `()` -> caller matches but the definition will not compile
 *   ("an argument type that has a default promotion can't match an empty
 *   parameter name list declaration").
 * So the caller carries its own K&R declaration in src/decomp/c_08059674.c and
 * the header stays silent. Both functions verified byte-for-byte that way. Do
 * not "fix" this by adding a prototype -- it will break one of the two. */
void sub_08059C60(void *);
void sub_08059464(void *);
/* CORRECTED in wave 45 (W45-H): `void sub_08058058(void)` was wrong on both
 * counts. The body opens `adds r7, r0, #0` -- r0 is READ before being written
 * and is the starting element index into the gUnknown_03003F20 list
 * (`lsls r0,r7,#2; adds r2,r2,r0`) -- and it RETURNS that index advanced by the
 * number of records appended (`adds r0,r7,#0`), with an early `movs r0,#0` on
 * the guard-fail path. The old prototype was invisible because its only caller,
 * sub_0805DFF4, calls it immediately after sub_0804151C, whose `int` result is
 * already sitting in r0 -- a nested call is byte-identical to two statements,
 * so `sub_08058058(sub_0804151C())` and `sub_0804151C(); sub_08058058();`
 * compile the same. src/decomp/c_0805DFF4.c is updated to the nested spelling
 * and re-verified. */
int sub_08058058(int);
int sub_08057F00(int);
struct UnitRecord *sub_08058144(int, int);
void sub_080591E4(void *);
int sub_08059A0C(void *);
void sub_0805BFDC(int, int, int, int);
void sub_0805C0AC(void *);
/* Wave 51, W51-C. The four battle-animation entry points sub_08059760,
 * sub_08059824, sub_080598BC and sub_08059978 fill the same 4-byte (u16, u16)
 * stack object sub_080591E4 and sub_0805C0AC take, and hand its address to one
 * of these two before testing the low halfword against 0x270F -- so `void *`
 * on the last parameter for the same reason it is `void *` there.
 *   sub_0805A9AC's first argument is a 0/1 selector (`movs r0,#0` in
 * sub_08059760, `movs r0,#1` in sub_08059824) and nothing else is set up, so
 * the arity is two.
 *   sub_08059B4C takes FIVE: r0 is sub_08057F00(1)'s `int` result left in place,
 * r1 a gUnknown_085766E0->unk04[] byte, r2 the constant 0, r3 the
 * gUnknown_03003F20 list pointer, and the stack word is the (u16, u16) object.
 * `int` on r1 rather than `u8` because the value arrives already zero-extended
 * from an `ldrb` and neither width costs an instruction at the call.
 *   sub_0805EB58 and sub_0805F914 are called with no argument set-up and their
 * results are unused. */
void sub_0805A9AC(int, void *);
/* Wave 52, W52-B. No prototype existed; this one is read off the MATCHED body
 * (work/sub_0805A6DC). One argument: the output cursor for the same
 * {u8 x; u8 y; s16 v;} 4-byte record c_0805A514.c produces, spelled `u8 *`
 * because the plain scalar-pointer stores are what the byte match needs -- a
 * `struct Unk5A514Cell *` cursor is 4 bytes short. The result is the record
 * COUNT, formed as `(out - (u8 *)gUnknown_03003F20) >> 2`. Its one caller,
 * sub_0805F4F8, is still asm, so nothing else constrains this yet. */
int sub_0805A6DC(u8 *);
void sub_08059B4C(int, int, int, void *, void *);
void sub_0805EB58(void);
void sub_0805F914(void);
/* Wave 51, W51-E. The 0x0805ED70/0x0805EE40/0x0805EF00/0x0805F074 quartet is
 * the same battle-animation entry-point shape W51-C documents just above, and
 * these declarations come out of matching all four.
 *   All four take nothing and compute nothing after their last call.
 * sub_0805EF00's one caller, sub_0805F074, calls it with no argument register
 * written and discards, which is the only caller evidence any of them has.
 *   sub_08059AEC is nullary: three of the four call it immediately after the
 * gUnknown_030013EC indirect call with no argument set-up at all, and no caller
 * reads r0 afterwards.
 *   sub_08059E3C, sub_08059F24 and sub_0805A008 each take ONE argument and it
 * is the same one in all three -- the dereferenced gUnknown_03003F20 list
 * pointer, arriving as a bare `adds r0,rN,#0` off the register that has held it
 * since the prologue. That is the operand sub_08059A0C already takes as
 * `void *`, and sub_08059F24/sub_0805A008 are the two arms of a single
 * if/else in sub_0805ED70, so they share a signature by construction. All
 * three results are discarded at every call site seen, hence `void`. */
void sub_08059AEC(void);
void sub_08059E3C(void *);
void sub_08059F24(void *);
void sub_0805A008(void *);
void sub_0805ED70(void);
void sub_0805EE40(void);
void sub_0805EF00(void);
void sub_0805F074(void);
/* Wave 51, W51-M. The same entry-point shape once more, declared from
 * sub_0805ECDC's body: it takes nothing, its result is never read, and it ends
 * on the shared `bl sub_0805F7B8` tail the quartet above ends on.
 *   sub_080590DC is sub_080591E4's twin: sub_0805ECDC picks between the two on
 * gUnknown_085D5ABC[unk00].unk1a == 0x20 and hands each the address of the SAME
 * four-byte stack object, so they share a signature by construction -- the same
 * `void *` sub_080591E4 already carries. Result discarded, hence void.
 *   sub_08058F90 is already DEFINED in src/decomp/c_08058BB4.c as
 * `int sub_08058F90(void *)`; this only re-declares it, because sub_0805ECDC is
 * its first matched caller and compares the result against -1 as a full word. */
void sub_0805ECDC(void);
void sub_080590DC(void *);
int sub_08058F90(void *);
/* Wave 51, W51-M. The two arms of sub_0805E9DC's local function-pointer pair
 * (the 8-byte .rodata template at 0x0816DA88, holding 0x0805E87D and
 * 0x0805E779). Reached through `bl _call_via_r0`, i.e. a NULLARY indirect call
 * -- gcc parks the pointer in the first free scratch register, so r0 means no
 * argument register is live -- and the result is discarded.
 *   sub_0805EA54 is nullary on the same evidence: sub_0805E9DC calls it with no
 * argument set-up and never reads r0. Bodies not read. */
void sub_0805E778(void);
void sub_0805E87C(void);
void sub_0805EA54(void);
/* Wave 45, W45-E. sub_08057EC0 takes NO arguments: its only caller
 * (sub_08058144) re-enters it at the top of a loop with r0/r1 holding whatever
 * the previous iteration left there, and it writes r0 before reading it. It
 * returns a POINTER -- an element of the 4-byte-strided array *gUnknown_03003F20
 * whose lowest-`unk02` live entry it selects and then stamps with 0x7FFF, or 0
 * when none is live. sub_08058144 reads bytes +0 and +1 off the result as a
 * (column, row) pair, so the record is {u8, u8, s16}. `void *` is the weakest
 * model that fits; the record has no struct tag yet, and gUnknown_03003F20's
 * declared `struct Unk03003338 *` (stride 0x08) is NOT it -- sub_08057EC0 walks
 * the array with `adds r2, #4`. */
void *sub_08057EC0(void);
/* Wave 45, W45-F. sub_0805BB8C adds four sub_0805BBF8 results together with no
 * `lsls #0x18; lsrs #0x18` between the `bl` and the add, so the return is NOT
 * re-narrowed and the function returns `int` rather than a byte -- even though
 * the body only ever returns 0 or 1. Both parameters are `int` for the same
 * reason in reverse: the body tests `x < 0` off the raw argument register with
 * no sign-extension pair, which a declared s16 would have forced. */
int sub_0805BBF8(int, int);
/* Wave 45, W45-F. The fourth argument is an OUT parameter, a pair of adjacent
 * halfwords: sub_0805BAFC writes `strh` at +0 and +2, and sub_0805BA34 seeds
 * +0 with 9999 before four calls and re-reads it afterwards with a plain
 * `ldrh` -- unsigned, so `u16 *` rather than `s16 *` even though the values
 * stored are map coordinates. Third argument is a unit-type id (`ldrb` off
 * struct UnitRecord's unk00 at the call site), passed full-width. */
void sub_0805BAFC(int, int, int, u16 *);
/* Wave 45, W45-F. Three nullary queries -- sub_0805BE10, sub_0805BE54 and
 * sub_0805BEF0 call each with no argument register written beforehand, and
 * compare the results (and `+ 5` / `+ 2` of them) with a SIGNED `bge`, so the
 * return is `int`. */
int sub_08058318(void);
int sub_0805848C(void);
int sub_080585D4(void);
/* Wave 47, W47-G. The other two census counters of the same family, both
 * already DEFINED (src/decomp/c_080583DC.c, src/decomp/c_08058254.c) and never
 * declared; these publish the definitions unchanged. sub_0805BEA0 compares
 * `sub_08058254() < sub_080583DC() + 5` with a signed `bge`, the same tell. */
int sub_080583DC(void);
int sub_08058254(void);
/* Wave 49, W49-A. Same family again, already DEFINED (src/decomp/c_080586CC.c)
 * and never declared; this publishes the definition unchanged. Its one caller
 * found so far, sub_0805B2EC, only truth-tests the result (`cmp r0,#0`), so it
 * adds no width evidence either way -- the `int` is the definition's. */
int sub_080586CC(void);
/* Copied verbatim from the promoted definition in src/decomp/c_08058BB4.c,
 * which had no prototype; the definition wins. */
void sub_08058F30(u8 *);

/* Wave 37, W37-K3. Copied verbatim from the promoted definitions in
 * src/decomp/, which had no prototype anywhere; the definition wins. */
void sub_08061CDC(void);
void sub_08062028(void);
void sub_08062C7C(u8);

/* Wave 37, W37-K3. A whole-struct assignment `*dst = *src` of the 0x130-byte
 * struct Unk085771C4, compiled out of line: 0x10 bytes of unrolled ldrb/strb
 * then a 24-trip loop copying 0xc at a time, 0x130 in total. Both parameters
 * are that record type -- sub_08061788 passes a ROM row of gUnknown_085771C4
 * as the source and two different RAM records as the destination. Not yet
 * matched; the signature is read off the copy length, which is why the pointee
 * is the sized record rather than `void`. */
void sub_08061A40(struct Unk085771C4 *, const struct Unk085771C4 *);
/* Wave 50, W50-M, MATCHED. Builds one gUnknown_085771C4-shaped record into the
 * destination: the 0x10-byte header is copied from gUnknown_085771C4[a2] and
 * the 24 following 0xc-byte rows are that record summed byte-wise with
 * gUnknown_085771C4[gUnknown_0857690C[a3][gUnknown_08499598[a4].unk1d]]. The
 * pointee is the sized record for the same reason sub_08061A40's is -- the
 * 0x130 stride is what proves it. Parameter widths are read straight off the
 * entry narrowings: `lsls/lsrs #0x18` twice then `#0x10`. */
void sub_08061928(struct Unk085771C4 *, u8, u8, u16);

/* Wave 37, W37-K3. Nullary: each prologue overwrites r0 before reading it. */
void sub_080607E8(void);
void sub_08061E98(void);
void sub_0806279C(void);

/* Wave 44, W44-H. The three arms of sub_08061E98's on-stack handler table.
 * That function copies { sub_08061DCC, sub_08061E54, sub_08061E80 } from its
 * own unit's .rodata onto its stack with one `ldm`/`stm` pair and dispatches on
 * the low three bits of a unit record's unk09 (`bl _call_via_r1` -- ONE
 * argument, per the register-index rule). The three ROM words at 0x0816DB10
 * read 0x08061DCD / 0x08061E55 / 0x08061E81 out of baserom.gba, THUMB bit set,
 * and that is what fixes the ORDER; nothing in the dispatch does.
 *   Each takes one gUnknown_08499594 unit record -- sub_08061E98 passes
 * `&gUnknown_08499594[i]` with no arithmetic between the load and the call.
 * The tags below are the ones the promoted definitions in src/decomp/ already
 * use, and they stay INCOMPLETE here: each handler reads the record through its
 * own bitfield view because struct UnitRecord declares +0x04 and +0x09 as
 * plain bytes, and splitting those would touch every other reader of that
 * shared struct (the same reasoning the sub_08061DCC note below records).
 * Declaring the tag incomplete lets every existing definition keep compiling
 * with no edit, and lets a caller name the functions. If a later wave gives the
 * unit record one shared bitfield type, these three and sub_08061E98's own
 * local view should collapse into it. */
struct Unk8061DCC;
struct Unk61E54;
struct Unk61E80;
void sub_08061DCC(struct Unk8061DCC *);
void sub_08061E54(struct Unk61E54 *);
void sub_08061E80(struct Unk61E80 *);

/* Wave 44, W44-H. Copied verbatim from the promoted definition in
 * src/decomp/c_0802700C.c, which had no prototype anywhere; the definition
 * wins. sub_08061F34 is the first caller in C -- it passes the u16
 * gUnknown_030033EC and two int cell coordinates and tests the result with a
 * bare `lsls #0x18`, all of which the definition's types already produce. */
bool8 sub_0802700C(int, int, int);

/* Wave 37, W37-K3, matched this wave. */
void sub_08061788(u16);
void sub_08061868(void);
void sub_08061B00(void);
void sub_08061CF8(void);
/* sub_08061DCC is deliberately NOT declared here. Its parameter is one unit
 * record -- the same layout as struct UnitRecord (unk00 subscripts
 * gUnknown_085D5ABC[] with the `* 0x5c` stride, unk04_0 and unk06_0 come out
 * with that struct's documented `ldrb; lsls #25; lsrs #25`) -- except that it
 * WRITES the low three bits of unk09, and only a real bitfield reproduces the
 * ROM's SImode `movs #8; rsbs; ands` mask; the hand-written `(x & ~7) | K`
 * narrows to `movs #0xf8; ands` and is two instructions, not three. Splitting
 * UnitRecord's plain `u8 unk09` would touch every other reader of that shared
 * struct, so the draft carries its own tag and the prototype stays out of the
 * header until a caller needs it. Both callers are still assembly. */

/* Wave 37, W37-Q3 -- the gUnknown_03001470 "wake every pending slot" pass.
 * sub_08015954 and sub_08015994 both set unk12 bit 0 across the 30 slots and
 * then run `do { sub_08015A9C(); } while (sub_08015B94());`. Neither call sets
 * up an argument register, so both are nullary; sub_08015B94's result is
 * narrowed `lsls r0,r0,#0x18` at both call sites, which is the u8/bool8
 * readout, and its body returns only 0 and 1.
 *
 * sub_080159E0 was promoted in an earlier wave (src/decomp/c_080159E0.c) and
 * was never declared ANYWHERE; the signature below is copied verbatim from
 * that definition. */
void sub_080159E0(u8);
void sub_08015994(void);
void sub_08015A9C(void);
bool8 sub_08015B94(void);
/* A gUnknown_0848A160 opcode handler, so `u8` slot index in and the
 * interpreter's keep-going flag out, like every other handler in that table.
 * The parameter width is hard: the prologue is `lsls r0,r0,#0x18; lsrs`. */
bool8 sub_08015D24(u8);

/* ---- wave 37 (W37-Q4) ---- */
/* Builds the unlocked-CO list in gUnknown_020288A0 and returns how many it
 * wrote, not counting the 0xff terminator. `u8`: sub_08043D00 re-narrows the
 * result with `lsls #0x18; lsrs #0x18` before doing anything with it. */
u8 sub_08043CA0(void);
/* Wave 50 (W50-H). sub_08043C98 is sub_08043CA0's neighbour and its only known
 * caller is sub_0803BFBC, which stores the result with a whole-word `str` into
 * struct Unk08580934's `u8 *unk18`. That member's type was settled in wave 36
 * from its own reader (`ldr [r0,#0x18]` then an unscaled index and `ldrb`), so
 * `u8 *` is the return type both ends already agree on and the assignment needs
 * no cast. Nothing narrower is provable -- this is the only call site. */
u8 *sub_08043C98(void);
/* Third argument is compared against 14 as a SIGNED int and then used two ways:
 * as sub_08043AA0's palette-slot index when it is small, and as CpuFastSet's
 * `void *` destination when it is not -- so `int`, with the cast at the
 * CpuFastSet call. Second argument is the `u16 *` tile buffer the nibble
 * rewrite walks with `strh`/`adds #2`. */
void sub_08043E8C(int, u16 *, int);

/* Wave 44, W44-D. PARKED (data/parked.json) -- declaration only. Arity and
 * void-ness read straight off sub_08087548's one call site: r0 arrives as
 * sub_08037D80's `int` result with no re-narrowing, r1 is that loop's `int`
 * counter and r2 is sub_08087548's own third parameter forwarded unchanged, so
 * three `int`s. sub_08087548 discards the result and is itself void
 * (`pop {r0}; bx r0`), so nothing constrains a return value -- `void` is the
 * weakest type that fits. */
void sub_08087514(int, int, int);

/* Wave 44, W44-D. Already MATCHED and promoted as
 * `void sub_08043FA8(int a, void *b, int c)` in src/decomp/c_08043FA8.c but
 * never declared here; sub_08087B74 is the first caller in a different unit.
 * Copied verbatim from the definition -- the promoted file wins. */
void sub_08043FA8(int, void *, int);

/* ---- wave 45, W45-G: the 0x0800C region ---- */

/* sub_0800C7E8 masks its own parameter `& 0x1f` before switching on it, so
 * nothing at a call site constrains the width and `int` is the weakest type
 * that fits.  It RETURNS: three arms write r2 (0, 1 or 2) and the shared tail
 * is `adds r0, r2, #0`, and its caller sub_0800C840 forwards that result
 * straight out through `pop {r1}; bx r1`. */
int sub_0800C7E8(int);

/* sub_0800C2D0 has exactly ONE caller in the tree, sub_0800C22C, which drops
 * the result at all nine call sites -- so `void` is the weakest type that
 * fits and no other caller can disagree.  Three `int`s: the first two are
 * sub_0800C22C's own (x, y) parameters plus/minus 1 with no narrowing, the
 * third is a literal 0 or 1 flag. */
void sub_0800C2D0(int, int, int);

/* Wave 45, W45-G.  The (x, y) cell pair, unnarrowed, exactly as the
 * sub_08007D70 / sub_0800BEE4 neighbours take it. */
void sub_0800CEF8(int, int);

/* Wave 45, W45-G.  sub_0800C7A4 and sub_0800C75C are the clear and the set
 * half of one pair: both switch the same first parameter over
 * 0x28/0x48/0x68/0x88 to an index 0..3 and write gUnknown_0200B0B0's unk17
 * and unk1b at that index.  The first parameter is not narrowed at entry, so
 * `int`; sub_0800C75C's second and third are stored with bare `strb`s, which
 * makes their width a floor only, and `int` is the weakest that fits.  Both
 * are void -- sub_0800C7A4 falls out of the default arm to a bare `bx lr`
 * with r0 still holding the parameter. */
void sub_0800C7A4(int);
void sub_0800C75C(int, int, int);

/* Wave 45, W45-G.  THREE parameters, not the two the register scan reports:
 * r2 is passed straight through to sub_08001158's int third parameter without
 * ever being written, and its caller sub_0800EAF4 sets it to a literal 0x25
 * and 0x65 at the two call sites.  Void: `pop {r4, r5}; pop {r0}; bx r0`. */
void sub_0800EBFC(int, int, int);

/* Wave 46, W46-A.  Copied verbatim from the promoted definition in
 * src/decomp/c_080736F4.c, which is authoritative -- the function was matched
 * without ever being declared, so sub_08073714 was the first caller to need
 * it.  Row index is UNSIGNED (`bhi` guard) and x is signed (`< 0` clamp). */
void sub_080736F4(int x, u32 y, u16 *row);

/* Wave 46, W46-G.  Same story as sub_080736F4 directly above: copied verbatim
 * from the promoted definition in src/decomp/c_08073974.c, which is
 * authoritative.  sub_08073998 is the first caller to need it. */
void sub_08073974(int x, u32 y, int c, u16 *base);

/* Wave 46, W46-G.  Copied verbatim from the promoted definition in
 * src/decomp/c_0806E4BC.c, which is authoritative -- it too was matched before
 * anything calling it was, so sub_0806E510 is the first caller to need it.
 * Seven parameters; the fourth is the only narrow one and the seventh is
 * Proc_Start's parent.  It RETURNS the proc (`pop {r1}; bx r1` with r0 live). */
ProcPtr sub_0806E4BC(int a1, int a2, int a3, u16 a4, int a5, int a6, ProcPtr parent);

/* Wave 46, W46-F.  Both copied from their promoted definitions, which are
 * authoritative: src/decomp/c_08065200.c and src/decomp/c_08065818.c.  Neither
 * had ever been declared -- they were matched before anything that calls them,
 * and sub_08065238 / sub_0806530C / sub_0806540C (for the first) and
 * sub_0806574C (for the second) are the first callers to need a prototype.
 * sub_08065200's `int` return is corroborated at all three call sites: the
 * result goes straight into an `strh` with no re-narrowing in between. */
int sub_08065200(int);
void sub_08065818(void);

/* Wave 46, W46-F.  Copied verbatim from the promoted definition in
 * src/decomp/c_08064D44.c, which is authoritative; sub_0806530C is the first
 * caller to need it. */
void sub_08064D44(struct Unk08580934_Obj *, int, int, int);

/* Wave 46, W46-F.  ONE parameter (`adds r5, r0, #0` is the only register read
 * before anything is written) and void -- the two exit paths leave 0x80 and
 * sub_08021810's result in r0 respectively, so nothing consistent is returned,
 * and sub_0806574C discards it.  The argument is gUnknown_08580934 at that
 * call site and the body writes the whole +0x00..+0x20 header through it, but
 * `void *` is the weakest type that fits and costs the caller nothing --
 * leaving the eventual definition of sub_0803BFBC free to name its own type
 * rather than being pinned to struct Unk08580934 by a caller.
 *   Wave 46, W46-E: agreed with, not changed -- sub_0806D850 is the second
 * caller and passes the same global, so `void *` still costs nothing. Worth
 * recording that the body CORROBORATES struct Unk08580934 independently of
 * everything that typed it: it writes +0x04 as a `strh` (the s16 unk04),
 * +0x18 as a whole word (the u8 *unk18), and fills exactly the four parallel
 * byte tables at +0x09, +0x11, +0x1c and +0x20 four entries at a time -- the
 * extents unk09/unk0d/unk11/unk20 were only ever guessed at from "whatever
 * fits below the next member". */
void sub_0803BFBC(void *);

/* ---- wave 47 (W47-G) ---- the 0x0805B4A8 / 0x0805BC7C cursor-target block.
 *
 * sub_0808B6C4, sub_0805B4A8 and sub_0805BD40 are already DEFINED in
 * src/decomp/ and were never declared; these publish those definitions
 * unchanged (sub_0808B6C4 is the tree's memset -- dst, fill byte, length).
 *
 * sub_0805B4D8 returns a BOOLEAN BYTE: its body only ever produces the
 * literals 0 and 1, and its caller sub_0805BDE4 re-narrows the result with
 * `lsls #0x18; lsrs #0x18` before `cmp #1` -- the call-site narrowing agbcc
 * emits for a byte-returning callee. The second and third arguments are
 * out-parameters written with a whole-word `str` (`str r4,[r6]` and
 * `str r0,[r1]`), so they are `int *` and not halfword pointers.
 *
 * The four x/y/out builders below all take the cell key as two unnarrowed
 * `int`s (`adds rN, r0, #0` with no shift pair in any prologue) and write the
 * pair back as two `strh` through the third argument, the same shape as the
 * promoted sub_0805BE10 / sub_0805BEF0 family. sub_0805BC7C returns 0 or 1. */
void *sub_0808B6C4(void *, int, int);            /* c_0808B6C4.c */
int sub_0805B4A8(void);                          /* c_0805B4A8.c */
u8 sub_0805B4D8(int, int *, int *);
int sub_0805BD40(int, int, int, int, s16 *);     /* c_0805BD40.c */
/* Wave 48, W48-F: u8, not int. Its one caller sub_0805B814 re-narrows the result
 * with `lsls #0x18; lsrs #0x18` before `cmp r0,#1`, which agbcc does not emit for
 * an int return. Byte-neutral in the body (only 0 and 1 are produced), verified
 * against src/decomp/c_0805BC7C.c after the change. */
u8 sub_0805BC7C(int, int, u16 *);
void sub_0805BDE4(int, int, u16 *);
void sub_0805BEA0(int, int, u16 *);
void sub_0805BF3C(int, int, u16 *);

/* ---- wave 48 (W48-B) ---- the 0x0805B744 AI-turn driver block.
 *
 * sub_0805B980 is already DEFINED in src/decomp/c_0805B980.c and was never
 * declared; this publishes that definition unchanged (it takes nothing and
 * computes no value after its final store).
 *
 * sub_0805B744 sets up ONE 4-byte stack slot, passes its address to
 * sub_0805B8F4 and then to sub_0805B814, and nothing else in the frame is
 * addressed -- so both take a single pointer to that slot.  Its neighbours
 * sub_0805BC7C / sub_0805BDE4 write the same-sized slot as an x/y pair of
 * `strh`, hence `u16 *`.  sub_0805B8F4's result is re-narrowed at the call
 * site with `lsls #0x18; lsrs #0x18` before `cmp #1`, the narrowing agbcc
 * emits for a byte-returning callee, so it returns u8. */
/* sub_08059C00 takes the scratch cell list (gUnknown_03003F20, dereferenced
 * from its own pointer global) and the address of the same 4-byte x/y pair
 * sub_080591E4 and sub_0805BAFC use; sub_0805B778 seeds +0 with 9999 before
 * the call and re-reads it with `ldrh` afterwards, so `u16 *`.  Its result is
 * never read at either call site.  sub_0805F7B8 is nullary and its result is
 * likewise never read. */
/* Wave 51, W51-K.  RETYPED from `void`: sub_08059C00 ends
 * `asrs r0, r3, #0x10; pop {r4-r7}; pop {r1}; bx r1`, where r3 is the winning
 * record's value shifted left 16 -- a sign-extended s16 computed on both arms
 * and left in r0.  A `void` body cannot emit that (it is dead code and agbcc
 * deletes it).  Byte-neutral at both call sites, which is why it went
 * unnoticed: neither reads the result, so no re-narrowing appears anywhere. */
s16 sub_08059C00(void *, u16 *);
void sub_0805F7B8(void);
/* sub_080581A4's first argument is a plane inside the gUnknown_08499590 map
 * (`gUnknown_08499590 + 0x3C72`, the same plane c_0805B980.c reads), passed as
 * a raw `u8 *` the way sub_0801F92C takes `gUnknown_08499590 + 0x2852` beside
 * it; the second is a bare `movs r1,#0` at the only call site, unnarrowed. */
void sub_080581A4(u8 *, int);
/* sub_0805B5BC and sub_0805B6A0 are sub_0805B4D8's twins over the same
 * gUnknown_02029ED8 record (see that symbol's comment). They differ from it
 * only in taking the two cursor indices BY POINTER instead of by value --
 * both are `ldr rN,[rN]` at entry and the second is written back with `str`
 * when the 0xFE branch advances it -- and, for sub_0805B6A0, in dropping the
 * terrain predicate entirely. Third and fourth arguments are the same pair of
 * whole-word out-parameters sub_0805B4D8 has, so `int *`.
 *   Return type is UNSETTLED: both bodies only ever produce 0 and 1, and no
 * caller was inspected. `int` is the weakest type that fits and is what they
 * were matched under; if a caller turns up that re-narrows with
 * `lsls #0x18; lsrs #0x18`, they are u8 like sub_0805B4D8 and this must
 * change. Changing it is byte-neutral in their own bodies.
 *   WAVE 49, W49-A: the callers turned up, and they say u8 -- but they say it
 * with a BARE `lsls r0,r0,#0x18` and no `lsrs`, which the wave brief classes as
 * a truth test rather than a value-kept narrowing, so it is one notch weaker
 * than the tell recorded above. It is still positive evidence: under an `int`
 * return a plain `if (f(...))` needs only `cmp r0,#0`, and the shift is exactly
 * agbcc re-narrowing a byte-returning callee. Five independent call sites agree
 * -- sub_0805AE88, sub_0805AF90, sub_0805B1CC (sub_0805B5BC) and sub_0805B0AC,
 * sub_0805B2EC (sub_0805B6A0) -- and all five matched with the declaration left
 * at `int` and an explicit `(u8)` cast written at the call. NOT changed here:
 * these are shared declarations under promoted definitions, and the cast is
 * byte-identical to retyping. If a sixth site keeps the value, retype then.
 *   sub_0805B3F4 is the turn-start entry beside them: it takes nothing,
 * computes nothing after its final indirect call, and dispatches through
 * gUnknown_08576890. */
void sub_0805B3F4(void);
int sub_0805B5BC(int *, int *, int *, int *);
int sub_0805B6A0(int *, int *, int *, int *);
/* Wave 49, W49-A. sub_0805AF90 is the sub_0805B5BC-driven map-redraw loop in
 * this same family; it returns nothing (its only exit is a bare early return)
 * and its two callers, sub_0805AE88 and sub_0805B2EC, both discard. */
void sub_0805AF90(void);
void sub_0805B980(void);
void sub_0805B744(void);
void sub_0805B778(void);
void sub_0805B814(u16 *);
u8 sub_0805B8F4(u16 *);
/* Wave 48, W48-F. Both are DEFINED in src/decomp/c_0805B980.c and were matched
 * there under `int`, which W45-F flagged as unsettled because no caller had been
 * inspected. The callers exist and they settle it as u8: sub_0805B814 re-narrows
 * sub_0805BA34's result with `lsls r0,#0x18; cmp r0,#0` before its truth test,
 * and sub_0805B8F4 re-narrows sub_0805BB8C's with `lsls #0x18; lsrs #0x18` before
 * `cmp r0,#1`. agbcc emits neither for an `int` return, and both call sites are
 * the plain `if (f(...))` / `if (f(...) == 1)` spelling with no cast in sight.
 * The definitions in c_0805B980.c were changed to match and both re-verified
 * byte-identical, as predicted -- the bodies only ever produce 0 and 1, so
 * narrowing the return is free there. */
u8 sub_0805BA34(int, int, u16 *);
u8 sub_0805BB8C(int, int);

/* Wave 50, W50-D. sub_08032484 is already PROMOTED (src/decomp/c_08032484.c)
 * but was never declared here; sub_080324C4 is its first caller outside its
 * own unit, and passes `gUnknown_08499578 + 0x221`. The promoted definition
 * names the parameter type, so this must agree with it. */
void sub_08032484(u16 *);
/* Wave 50, W50-D. All four are void/void: sub_080324C4 and sub_08032BCC call
 * them with no argument register set up and never read r0 afterwards. */
void sub_08034290(void);
void sub_080328EC(void);
void sub_08032AFC(void);
/* Wave 50, W50-D. sub_08032BCC calls it as sub_08031B6C(&gUnknown_02027C2C[i
 * * 0x13], <the 0x34-byte stack buffer sub_0803CCB8(int, u8 *) just filled>),
 * result unused. sub_08032BCC itself is NOT declared here: it is a proc
 * callback reached only through a ProcCmd table, its parameter is a proc
 * struct local to its own unit, and a `void *` declaration here would
 * conflict with that definition. */
void sub_08031B6C(u8 *, u8 *);
/* Wave 50, W50-D. All four read off sub_08031638's call sites, which narrow
 * every result: sub_0802F408 is tested with a BARE `lsls #24` (a truth test on
 * a byte), sub_0802F4A0 with `lsls #24; lsrs #24; cmp #1` (a value kept, and
 * sub_080312AC tests the same function both ways), sub_0802F504 with
 * `lsls #24; asrs #24; cmp #1; ble` -- signed, i.e. the same s8 its neighbour
 * sub_0802F534 is already declared to return. sub_08030D84 is a bare tail call
 * with no argument set up and its result unread. */
bool8 sub_0802F408(void);
bool8 sub_0802F4A0(void);
s8 sub_0802F504(void);
void sub_08030D84(void);

/* Wave 51, W51-O. Two functions that are ALREADY DEFINED in src/decomp but had
 * no declaration in this header, so nothing outside their own file could call
 * them. Both signatures are copied from the promoted definition, which wins:
 * src/decomp/c_08038368.c and src/decomp/c_0805CA24.c. */
int sub_0803840C(void);
int sub_0805CA24(void);
/* sub_0805E778 hands it the dereferenced gUnknown_03003F20 list pointer, the
 * same value sub_08059E3C and sub_08059C00 take, and both of those are declared
 * `void *` here. Result unused at the only call site. */
void sub_0805A0EC(void *);

/* Wave 51, W51-P. Three callees of this batch that had no declaration here.
 * Each signature is copied from a definition that already exists and therefore
 * wins: work/sub_0808488C (matched, returns one of two u16 palette bases),
 * src/decomp/c_0807F8E4.c (`return Proc_Find(...) != 0;`, so `int` -- its
 * caller sub_0807C9EC re-narrows the result with `lsls #24; lsrs #24`, which is
 * the caller storing it in a u8, not a u8 return) and src/decomp/c_08087B20.c
 * (four ints; the fourth is a sprite-id base added to each decimal digit). */
u16 *sub_0808488C(int);
int sub_0807F8E4(void);
void sub_08087B20(int, int, int, int);

#endif // UNKNOWN_FUNCS_H
