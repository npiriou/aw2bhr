OUTPUT_ARCH(arm)
ENTRY(spann_phase2_arm)
SECTIONS
{
    . = 0x08800000;
    .text : ALIGN(4)
    {
        KEEP(*(.text.spann_hook))
        *(.text*)
        *(.rodata*)
    }
    /DISCARD/ : { *(.comment*) *(.ARM.attributes*) }
    ASSERT(ADDR(.text) == 0x08800000, "Spann payload moved")
    ASSERT(SIZEOF(.text) <= 0x10000, "Spann payload exceeds reserved ROM window")
}
