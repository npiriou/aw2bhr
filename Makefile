.SUFFIXES:

# ==================
# = PROJECT CONFIG =
# ==================

BUILD_NAME := aw2bhr

SRC_DIR = src
ASM_DIR = asm
BUILD_DIR = build

CLEAN_FILES :=
CLEAN_DIRS  :=

all: compare

mod:
	$(PYTHON) tools/spann_bridge/build_mod.py

.PHONY: mod

# ====================
# = TOOL DEFINITIONS =
# ====================

ifeq ($(OS),Windows_NT)
  EXE := .exe
else
  EXE :=
  UNAME_S := $(shell uname -s)
endif

TOOLCHAIN ?= $(DEVKITARM)
AGBCC_HOME ?= tools/agbcc

ifneq (,$(TOOLCHAIN))
  export PATH := $(TOOLCHAIN)/bin:$(PATH)
endif

PREFIX := arm-none-eabi-

ifeq ($(UNAME_S),Darwin)
  ifneq (,$(TOOLCHAIN))
    PREFIX := $(TOOLCHAIN)/bin/$(PREFIX)
  endif
  SHASUM ?= shasum
endif

export OBJCOPY := $(PREFIX)objcopy
export OBJDUMP := $(PREFIX)objdump
export AS := $(PREFIX)as
export CPP := $(PREFIX)cpp
export LD := $(PREFIX)ld
export STRIP := $(PREFIX)strip

CC1_OLD := $(AGBCC_HOME)/bin/old_agbcc$(EXE)
CC1     := $(AGBCC_HOME)/bin/agbcc$(EXE)

SHASUM ?= sha1sum
PERL := perl
PYTHON ?= python3

# ================
# = BUILD CONFIG =
# ================

CPPFLAGS := -I $(AGBCC_HOME)/include -iquote include -iquote . -nostdinc -undef
CFLAGS := -g -mthumb-interwork -Wimplicit -Wparentheses -Werror -fhex-asm -ffix-debug-line -fforce-addr -fprologue-bugfix -O2
ASFLAGS := -mcpu=arm7tdmi -I asm/include -I include
LDFLAGS :=

LDS := $(BUILD_NAME).lds
# src/decomp holds functions promoted out of assembly on this branch. They are
# excluded from the default build on purpose: that build is upstream's, still
# assembles the same functions from asm/*.s, and linking both would be a
# duplicate definition. Keeping it buildable means the ROM stays reproducible
# from upstream sources alone -- a second, independent check on our C.
#
# The `-name` patterns are quoted deliberately. Unquoted, the shell expands them
# against the repo root before find ever sees them, so a single stray .c file
# there turns `-name *.c` into `-name thatfile.c` and C_SRCS comes out EMPTY.
# Nothing errors: make just links whatever objects already exist in build/, so
# the ROM silently builds from stale code. This happened -- decomp-permuter's
# --debug mode drops a debug_source.c in the working directory.
C_SRCS := $(shell find $(SRC_DIR) -name '*.c' -not -path '*/decomp/*')
ASM_SRCS := $(shell find $(SRC_DIR) -name '*.s') $(shell find $(ASM_DIR) -name '*.s')
DATA_SRCS := $(shell find data -name '*.s')

# `make SPLIT=1` builds from the per-function units in build/functions instead
# of the four monolithic asm/*.s, using the matching generated linker script.
# Both must produce the same ROM -- that equality is the acceptance test for
# the split, and it is stronger than the text-level check in verify_split.py
# because it also proves 4,505 separate objects land at the same addresses.
#
# Opt-in so the default build stays byte-for-byte upstream's, which keeps the
# comparison honest and merges clean.
#
# Regenerate the inputs first:
#     python tools/split_asm.py && python tools/gen_lds.py
FUNC_DIR := $(BUILD_DIR)/functions
RODATA_DIR := $(BUILD_DIR)/rodata
ifeq ($(SPLIT),1)
  FUNC_SRCS := $(shell find $(FUNC_DIR) -name '*.s')
  ASM_SRCS := $(shell find $(SRC_DIR) -name '*.s')
  C_SRCS += $(shell find $(SRC_DIR)/decomp -name '*.c' 2>/dev/null)
  LDS := $(BUILD_NAME).split.lds

  # data/rodata.s holds agbcc's -fforce-addr pool words as incbin'd ROM bytes.
  # A promoted unit whose C names the global directly EMITS its own copy of the
  # word it owns, so the blob must stop supplying those bytes or they would be
  # in the ROM twice. tools/split_rodata.py carves them out into generated
  # pieces under build/rodata, reading data/rodata.s and never modifying it --
  # the same read-only treatment split_asm.py gives asm/. With nothing carved
  # out the single generated piece is TEXTUALLY IDENTICAL to data/rodata.s, so
  # this substitution is a no-op until a unit claims a word.
  RODATA_SRCS := $(shell find $(RODATA_DIR) -name '*.s' 2>/dev/null)
  ifeq ($(strip $(RODATA_SRCS)),)
    $(error build/rodata is empty -- run `$(PYTHON) tools/split_rodata.py` first. \
            Without it the link would silently drop megabytes of ROM data)
  endif
  # SPLIT_BLOBS is written by split_rodata.py and names exactly the blobs its
  # generated pieces replace. Included rather than hardcoded so the two cannot
  # drift: a blob left in DATA_SRCS is linked twice, and one wrongly removed
  # vanishes from the ROM.
  include $(RODATA_DIR)/blobs.mk
  DATA_SRCS := $(filter-out $(SPLIT_BLOBS),$(DATA_SRCS))

  # Parts of the ROM were not built with the default toolchain -- the sound and
  # flash libraries came out of the SDK prebuilt with their own settings, which
  # is why CC1_OLD has been defined since the beginning. These are per-object
  # assignments, so nothing outside the named files is affected. Source of truth
  # is data/compiler-overrides.json, which tools/agbenv.py reads too so that
  # trymatch.py and compile_probe agree with this build.
  #
  # Generated rather than checked in, and generated BY MAKE rather than by hand:
  # build/ is gitignored, so on a fresh clone or after `make clean` a hand-run
  # step would be missing, `-include` would skip it silently, and the overridden
  # objects would build with the wrong toolchain. That surfaces as a bare SHA
  # mismatch with nothing pointing at the cause. GNU make remakes an included
  # makefile and restarts, so the rule below keeps the two in step by itself.
  $(BUILD_DIR)/compiler-overrides.mk: data/compiler-overrides.json data/promoted.json tools/gen_overrides_mk.py
	@$(PYTHON) tools/gen_overrides_mk.py

  -include $(BUILD_DIR)/compiler-overrides.mk
endif

C_GENERATED :=

# ===========
# = TARGETS =
# ===========

ROM := $(BUILD_NAME).gba
ELF := $(ROM:%.gba=%.elf)
MAP := $(ROM:%.gba=%.map)
SYM := $(ROM:%.gba=%.sym)

ifeq (,$(findstring $(C_GENERATED),$(C_SRCS)))
C_SRCS += $(C_GENERATED)
endif

# Source discovery failing silently is worse than any build error: make would
# link the objects already in build/ and `compare` would pass on stale code.
ifeq ($(strip $(C_SRCS)),)
  $(error C_SRCS is empty -- source discovery failed, see the note above it)
endif

C_OBJS := $(C_SRCS:%.c=$(BUILD_DIR)/%.o)
ASM_OBJS := $(ASM_SRCS:%.s=$(BUILD_DIR)/%.o)
DATA_OBJS := $(DATA_SRCS:%.s=$(BUILD_DIR)/%.o)

# Unit objects sit beside their sources, already under $(BUILD_DIR), so they
# take the %.s -> %.o rule below rather than the $(BUILD_DIR)/%.o one.
ASM_OBJS += $(FUNC_SRCS:%.s=%.o)
ASM_OBJS += $(RODATA_SRCS:%.s=%.o)

ALL_OBJS := $(C_OBJS) $(ASM_OBJS) $(DATA_OBJS)
ALL_DEPS := $(ALL_OBJS:%.o=%.d)

SUBDIRS := $(sort $(dir $(ALL_OBJS)))
$(shell mkdir -p $(SUBDIRS))

# ===========
# = RECIPES =
# ===========

compare: $(ROM)
	@echo "[SHA]	$<"
	@$(SHASUM) -c $(BUILD_NAME).sha1

.PHONY: compare

syms: $(SYM)

%.gba: %.elf
	@echo "[GEN]	$@"
	@$(OBJCOPY) --strip-debug -O binary $< $@

CLEAN_FILES += $(ROM)

$(ELF): $(ALL_OBJS) $(LDS)
	@echo "[ LD]	$@"
	@cd $(BUILD_DIR) && $(LD) -T ../$(LDS) -Map ../$(MAP) -L../tools/agbcc/lib $(ALL_OBJS:$(BUILD_DIR)/%=%) -lc -lgcc -o ../$@ $(LDFLAGS)

CLEAN_FILES += $(ELF) $(MAP)

# C dependency files
$(BUILD_DIR)/%.d: %.c
	@$(CPP) $(CPPFLAGS) $< -o $@ -MM -MG -MT $@ -MT $(BUILD_DIR)/$*.o

# C object
#
# The overrides makefile is a prerequisite, not just an `-include`: a
# target-specific `CC1 :=` changes how an object is BUILT but does not make an
# existing .o out of date, so make happily keeps one compiled by the previous
# toolchain. Wave 38 added two old_agbcc entries, regenerated the .mk, rebuilt,
# and got a 12-byte ROM mismatch inside exactly those two functions -- the .o
# was simply never recompiled. gen_overrides_mk.py writes only when the CONTENT
# changes, so this costs a full rebuild when the override set really changes
# (four times in 38 waves) and nothing on an ordinary wave.
$(BUILD_DIR)/%.o: %.c $(BUILD_DIR)/%.d $(BUILD_DIR)/compiler-overrides.mk
	@echo "[ CC]	$<"
	@$(CPP) $(CPPFLAGS) $< | iconv -f UTF-8 -t CP932 | $(CC1) $(CFLAGS) -o $(BUILD_DIR)/$*.s
	@printf ".text\n\t.align\t2, 0\n" >> $(BUILD_DIR)/$*.s
	@$(AS) $(ASFLAGS) $(BUILD_DIR)/$*.s -o $@
	@$(STRIP) -N .gcc2_compiled. $@

# ASM dependency file (dummy, generated with the object)
$(BUILD_DIR)/%.d: $(BUILD_DIR)/%.o
	@touch $@

# ASM object
$(BUILD_DIR)/%.o: %.s
	@echo "[ AS]	$<"
	@$(AS) $(ASFLAGS) $< -o $@ --MD $(BUILD_DIR)/$*.d

# Per-function unit object (SPLIT=1). Source and object share a directory, so
# this cannot go through the rule above. Silent: 4,505 echo lines is noise.
$(FUNC_DIR)/%.o: $(FUNC_DIR)/%.s
	@$(AS) $(ASFLAGS) $< -o $@ --MD $(FUNC_DIR)/$*.d

# Generated rodata piece (SPLIT=1). Same reason as the rule above: source and
# object share a directory, so the $(BUILD_DIR)/%.o rule would nest it.
$(RODATA_DIR)/%.o: $(RODATA_DIR)/%.s
	@echo "[ AS]	$<"
	@$(AS) $(ASFLAGS) $< -o $@ --MD $(RODATA_DIR)/$*.d

ifneq (clean,$(MAKECMDGOALS))
  -include $(ALL_DEPS)
  .PRECIOUS: $(BUILD_DIR)/%.d
endif

CLEAN_DIRS += $(BUILD_DIR)

# ===============
# = Symbol file =
# ===============

$(SYM): $(ELF)
	@echo "[GEN]	$@"
	@$(OBJDUMP) -t $< | sort -u | grep -E "^0[2389]" | $(PERL) -p -e 's/^(\w{8}) (\w).{6} \S+\t(\w{8}) (\S+)$$/\1 \2 \3 \4/g' > $@

CLEAN_FILES += $(SYM)

# ==============
# = Make clean =
# ==============

CLEAN_DIRS += $(shell find . -type d -name "__pycache__")

clean:
	@rm -f $(CLEAN_FILES)
	@rm -rf $(CLEAN_DIRS)
	@echo "all cleaned..."

.PHONY: clean
