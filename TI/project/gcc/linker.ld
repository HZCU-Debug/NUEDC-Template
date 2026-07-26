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
