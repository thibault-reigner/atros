#define __ASM__

#include <x86/boot/multiboot.h>


/*
 Multiboot header
 Expected to be at the beginning of the kernel image by
 a multiboot-compliant boot loader.
*/

_multiboot_header_start:
	.long MULTIBOOT_HEADER_MAGIC
	.long MULTIBOOT_HEADER_FLAGS
	.long -(MULTIBOOT_HEADER_MAGIC + MULTIBOOT_HEADER_FLAGS)  //Checksum
	.long _multiboot_header_start
	.long _boot_pa_start
	.long _kernel_pa_end
	.long _bss_pa_end
	.long _start
_multiboot_header_end:
