/*----------------------------------------------------------------------------*/
/* AlgLink.ld                                                                 */
/*                                                                            */
/* (c) Texas Instruments 2009-2013, All rights reserved.                      */
/*                                                                            */
/*----------------------------------------------------------------------------*/
/* Highest address of the stack */
_estack = 0x8040000;    /* end of 256K RAM */

/* Generate a link error if heap and stack don't fit into RAM */
_Min_Heap_Size = 0x400;      /* required amount of heap  */

/* Specify the memory areas */
MEMORY
{
    VECTORS         (rx)    : ORIGIN = 0x00080000, LENGTH = 0x00000040
    FLASH           (rx)    : ORIGIN = 0x00080040, LENGTH = (0x001FFFC0 + 0x00200000)

    RAM             (xrw)   : ORIGIN = 0x08040000, LENGTH = 0x0003F000    /* 64K*7 -4K */

    RAM_SL          (xrw)   : ORIGIN = 0x0807F000, LENGTH = 0x00001000    /* 4K flash 自诊断函数区   */
    MEMORY_B1       (rx)    : ORIGIN = 0x60000000, LENGTH = 0K
}

/* Define output sections */
SECTIONS
{
    /* The ISR vector goes first into RAM */
    .intvecs :
    {
        . = ALIGN(4);
        KEEP(*(.intvecs)) /* ISR vector */
        . = ALIGN(4);
    } >VECTORS

    /* The program code and other data goes into RAM */
    .text :
    {
        . = ALIGN(4);
        *(.text)           /* .text sections (code) */
        *(.text*)          /* .text* sections (code) */
        *(.glue_7)         /* glue arm to thumb code */
        *(.glue_7t)        /* glue thumb to arm code */
        *(.eh_frame)

        KEEP (*(.init))
        KEEP (*(.fini))

        . = ALIGN(4);
        _etext = .;        /* define a global symbols at end of code */
    } >FLASH

    /* Constant data goes into RAM */
    .rodata :
    {
        . = ALIGN(4);
        *(.rodata)         /* .rodata sections (constants, strings, etc.) */
        *(.rodata*)        /* .rodata* sections (constants, strings, etc.) */
        . = ALIGN(4);
    } >FLASH

    /* used by the startup to initialize data */
    _sidata = LOADADDR(.data);

    /* Initialized data sections goes into RAM, load LMA copy after code */
    .data :
    {
        . = ALIGN(4);
        _sdata = .;        /* create a global symbol at data start */
        *(.data)           /* .data sections */
        *(.data*)          /* .data* sections */
        *(.usr_par_head*)
        *(.usr_par*)
        *(.usr_cfg_head*)
        *(.usr_cfg*)
        *(.usr_img_head*)
        *(.usr_img*)
        *(.usr_for_head*)
        *(.usr_for*)
        . = ALIGN(4);
        _edata = .;        /* define a global symbol at data end */
    } >RAM AT> FLASH

    /* Uninitialized data section */
    . = ALIGN(4);
    .bss :
    {
        /* This is used by the startup in order to initialize the .bss secion */
        _sbss = .;         /* define a global symbol at bss start */
        __bss_start__ = _sbss;
        *(.bss)
        *(.bss*)
        *(COMMON)

        . = ALIGN(4);
        _ebss = .;         /* define a global symbol at bss end */
        __bss_end__ = _ebss;
    } >RAM

    PROVIDE ( end = _ebss );
    PROVIDE ( _end = _ebss );

    /* MEMORY_bank1 section, code must be located here explicitly            */
    /* Example: extern int foo(void) __attribute__ ((section (".mb1text"))); */
    .memory_b1_text :
    {
        *(.mb1text)        /* .mb1text sections (code) */
        *(.mb1text*)       /* .mb1text* sections (code)  */
        *(.mb1rodata)      /* read-only data (constants) */
        *(.mb1rodata*)
    } >MEMORY_B1

    /* Remove information from the standard libraries */
    /DISCARD/ :
    {
        libc.a ( * )
        libm.a ( * )
        libgcc.a ( * )
    }

    .ARM.attributes 0 : { *(.ARM.attributes) }
}
