OUTPUT_FORMAT("binary")
OUTPUT_ARCH(i386)
STARTUP(crt0.o)

USER_BASE = 0x400000;

SECTIONS
{
  .text USER_BASE : AT(USER_BASE) {
    code = .;
    *(.text)
    *(.rodata)
  }
  .data : AT(USER_BASE + (data - code))
  {
    data = .;
    *(.data)
  }
  .bss : AT(USER_BASE + (bss - code))
  {
    bss = .;
    *(.bss)
  }
  end = .;
}
