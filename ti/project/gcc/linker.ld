MEMORY
{
    FLASH (rx)  : ORIGIN = 0x00000000, LENGTH = 128K
    SRAM  (rwx) : ORIGIN = 0x20200000, LENGTH = 32K
}

SECTIONS
{
    .intvecs :
    {
        KEEP(*(.intvecs))
    } > FLASH

    .text :
    {
        *(.text*)
        *(.rodata*)
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
        KEEP(*(SORT(.init_array.*)))
        KEEP(*(.init_array*))
        PROVIDE_HIDDEN(__init_array_end = .);
    } > FLASH

    .fini_array :
    {
        PROVIDE_HIDDEN(__fini_array_start = .);
        KEEP(*(SORT(.fini_array.*)))
        KEEP(*(.fini_array*))
        PROVIDE_HIDDEN(__fini_array_end = .);
    } > FLASH

    _data_load = LOADADDR(.data);
    .data :
    {
        _data_start = .;
        *(.data*)
        _data_end = .;
    } > SRAM AT > FLASH

    .bss (NOLOAD) :
    {
        _bss_start = .;
        *(.bss*)
        *(COMMON)
        _bss_end = .;
        end = .;
        _end = .;
    } > SRAM

    . = ALIGN(8);
    _stack_top = ORIGIN(SRAM) + LENGTH(SRAM);
}
