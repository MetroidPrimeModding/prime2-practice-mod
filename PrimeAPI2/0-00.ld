ENTRY(_prolog)
ENTRY(_earlyboot_memset)
PHDRS
{
  maindata PT_LOAD;
  bss PT_DYNAMIC;
}

SECTIONS
{
  . = _LINK_START;

    .text :
    {
      _INIT_START = .;
      . = ALIGN(4);
      *(.boot._prolog*)
      _earlyboot_memset = .;
      *(.boot._earlyboot_memset*)
      *(.boot*)
      *(.init)
      *(.init_array)
      *(.text._prolog)
      *(.text*)
      *(.text.*)
      *(.gnu.warning)
      *(.gnu.linkonce.t*)
      *(.glue_7)
      *(.glue_7t)
    } :maindata

    .data :
      {
        . = ALIGN(4);
        *(.rodata)
        *(.eh_frame)
        *all.rodata*(*)
        *(.roda)
        *(.rodata.*)
        *(.gnu.linkonce.r*)
        *(.data)
        *(.data.*)
        *(.gnu.linkonce.d*)
        . = ALIGN(4);
      } :maindata

    .bss :
    	{
        . = ALIGN(4);
    		*(.dynbss)
    		*(.gnu.linkonce.b*)
    		*(.bss*)
    		*(COMMON)
    		. = ALIGN(4);
    	} :bss

	_LINK_END = .;
	_LINK_SIZE = _LINK_END - _LINK_START;
}
