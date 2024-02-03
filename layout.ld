/* SPDX-License-Identifier: MIT */

/*
 * Created by lebakassemmerl 2022
 * E-Mail: hotschi@gmx.at
 */

MEMORY
{
    rom (rx)  : ORIGIN = 0x00000000, LENGTH = 256K
    ram (rwx) : ORIGIN = 0x20000000, LENGTH = 64K
}

STACK_SIZE = 4K;

SECTIONS
{
    .text :
    {
        . = ALIGN(4);
        _stext = .;

        KEEP(*(.arm_vector))
        KEEP(*(.irq_vector))
        
        *(.text .text.* .gnu.linkonce.t.*)
        *(.rodata .rodata.* .gnu.linkonce.r.*)

        . = ALIGN(4); 
    } > rom

    .cpp :
    {
        . = ALIGN(4);
        _ctors_begin = .;
        KEEP(*(.init))
        KEEP (*(.preinit_array))
        KEEP (*(SORT(.init_array.*)))
        KEEP (*(.init_array))
        KEEP (*crtbegin.o(.ctors))
        KEEP (*(EXCLUDE_FILE (*crtend.o) .ctors))
        KEEP (*(SORT(.ctors.*)))
        KEEP (*crtend.o(.ctors))
        _ctors_end = .;

        . = ALIGN(4);
        _dtors_begin = .;
        KEEP (*(.fini_array))
        KEEP (*(SORT(.fini_array.*)))
        KEEP (*crtbegin.o(.dtors))
        KEEP (*(EXCLUDE_FILE (*crtend.o) .dtors))
        KEEP (*(SORT(.dtors.*)))
        KEEP (*crtend.o(.dtors))
        _dtors_end = .;

        . = ALIGN(4);
        _etext = .;
    } > rom
    
    .stack (NOLOAD) :
    {
        /* Place the stack at the beginning of the RAM in order to detect a 
         * stack overflow due to the hardfault-handler when writing beyound the
         * RAM. */
        . = ALIGN(4);
        _sstack = ADDR(.stack);
        /* KEEP(*(.stack_buffer)) */
        . = . + STACK_SIZE ;

        . = ALIGN(4);
        _estack = .;
        . = . + 4;
    } > ram
    
    .relocate : AT(_etext)
    {
        . = ALIGN(4);
        _sdata = .;

        *(.data .data.*)

        . = ALIGN(4);
        _edata = .;
    } > ram
    
    .bss (NOLOAD) :
    {
        . = ALIGN(4);
        _szero = .;

        *(.bss .bss.*)
        *(COMMON)

        . = ALIGN(4);
        _ezero = .;
    } > ram
}
