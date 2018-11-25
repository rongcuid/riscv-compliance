OUTPUT_ARCH( "riscv" )
ENTRY(_start)

/*
MEMORY
{
  rom : ORIGIN = 0, length = 1k;
  ram : ORIGIN = 0x10000000, length = 1k;
}
*/

SECTIONS
{
  .text.init 0 : { *(.text.init); }
  .text : { 
    *(.text);
    _etext = .;
   }
  .data : { 
    _data = .;
    *(.data);
  }
  . = ALIGN(4);
  .data.string : { *(.data.string);_edata = .;}
  . = ALIGN(4);
  .bss : { 
    _bstart = .;
    *(.bss); 
    _bend = .;
  }
  _end = .;
}

