#define __ASM__

#include <x86/boot/multiboot2.h>

.align 8
_multiboot2_header_start:
	.long MULTIBOOT2_HEADER_MAGIC
	.long MULTIBOOT2_ARCHITECTURE_I386
	.long (_multiboot2_header_end - _multiboot2_header_start + 1)
	.long -(MULTIBOOT2_HEADER_MAGIC + MULTIBOOT2_ARCHITECTURE_I386 + _multiboot2_header_end - _multiboot2_header_start +1)	

.align 8
	/*Tag information request*/
_multiboot2_info_request_tag_start:	
	.word MULTIBOOT2_HEADER_TAG_INFORMATION_REQUEST
	.word 0
	.long (_multiboot2_info_request_tag_end - _multiboot2_info_request_tag_start + 1)
	/*Requests for information*/
	.long MULTIBOOT2_TAG_TYPE_CMDLINE
	.long MULTIBOOT2_TAG_TYPE_BOOT_LOADER_NAME
	.long MULTIBOOT2_TAG_TYPE_MODULE
	.long MULTIBOOT2_TAG_TYPE_BASIC_MEMINFO
	.long MULTIBOOT2_TAG_TYPE_MMAP  
_multiboot2_info_request_tag_end:

.align 8
	/*Tag kernel addresses*/
	.word MULTIBOOT2_HEADER_TAG_ADDRESS
	.word 0
	.long 24
	.long _multiboot2_header_start
	.long _boot_pa_start
	.long _rodata_pa_end   /*End of data sections*/
	.long _bss_pa_end      /*End of the .bss section*/

.align 8
	/*Tag kernel's entry address*/
	.word MULTIBOOT2_HEADER_TAG_ENTRY_ADDRESS
	.word 0
	.long 12
	.long _start

.align 8
	/*Modules must be page-aligned*/
	.word MULTIBOOT2_HEADER_TAG_MODULE_ALIGN
	.word 0
	.long 12
	
.align 8
	/*Tags' end*/
	.word MULTIBOOT2_HEADER_TAG_END
	.word 0
	.long 8
_multiboot2_header_end: