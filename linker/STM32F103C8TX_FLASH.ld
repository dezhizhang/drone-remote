/* STM32F103C8T6: use the documented 64 KiB Flash and 20 KiB SRAM. */
ENTRY(Reset_Handler)

MEMORY
{
    FLASH (rx)  : ORIGIN = 0x08000000, LENGTH = 64K
    RAM   (xrw) : ORIGIN = 0x20000000, LENGTH = 20K
}

_estack = ORIGIN(RAM) + LENGTH(RAM);
_Min_Stack_Size = 0x800; /* MSP: startup, interrupts and fault handlers. */

SECTIONS
{
    .isr_vector : { . = ALIGN(4); KEEP(*(.isr_vector)) . = ALIGN(4); } > FLASH
    .text :
    {
        . = ALIGN(4);
        *(.text .text.*) *(.rodata .rodata.*)
        KEEP(*(.init)) KEEP(*(.fini))
        . = ALIGN(4);
        _etext = .;
    } > FLASH
    .ARM.extab : { *(.ARM.extab* .gnu.linkonce.armextab.*) } > FLASH
    .ARM.exidx :
    {
        __exidx_start = .;
        *(.ARM.exidx* .gnu.linkonce.armexidx.*)
        __exidx_end = .;
    } > FLASH
    .preinit_array :
    {
        PROVIDE_HIDDEN(__preinit_array_start = .);
        KEEP(*(.preinit_array*))
        PROVIDE_HIDDEN(__preinit_array_end = .);
    } > FLASH
    .init_array :
    {
        PROVIDE_HIDDEN(__init_array_start = .);
        KEEP(*(SORT(.init_array.*))) KEEP(*(.init_array*))
        PROVIDE_HIDDEN(__init_array_end = .);
    } > FLASH
    .fini_array :
    {
        PROVIDE_HIDDEN(__fini_array_start = .);
        KEEP(*(SORT(.fini_array.*))) KEEP(*(.fini_array*))
        PROVIDE_HIDDEN(__fini_array_end = .);
    } > FLASH
    _sidata = LOADADDR(.data);
    .data :
    {
        . = ALIGN(4); _sdata = .;
        *(.data .data.*)
        . = ALIGN(4); _edata = .;
    } > RAM AT > FLASH
    .bss (NOLOAD) :
    {
        . = ALIGN(8); _sbss = .; __bss_start__ = .;
        *(.bss .bss.*) *(COMMON)
        . = ALIGN(8); _ebss = .; __bss_end__ = .;
    } > RAM
    .msp_stack (NOLOAD) :
    {
        . = ALIGN(8);
        _end = .; PROVIDE(end = .);
        . += _Min_Stack_Size;
        . = ALIGN(8);
    } > RAM
    ASSERT(_ebss + _Min_Stack_Size <= _estack, "RAM overflow: no room for MSP stack")
    .ARM.attributes 0 : { *(.ARM.attributes) }
    /DISCARD/ : { *(.note.GNU-stack) *(.comment) }
}
