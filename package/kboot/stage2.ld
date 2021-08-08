ENTRY(main);
OUTPUT_FORMAT("binary")
SECTIONS
{
  . = 0x7e04;
  .text :
  {
    /* *(.boot); */
    *(.entry);
    *(.text16);
    /* *(.latetext16); */
    *(.text32);
    /* *(.latetext32); */
  }
  .rodata :
  {
    *(.rodata);
  }
  .data :
  {
    *(.data);
  }
  .bss :
  {
    *(.bss);
  }
} 