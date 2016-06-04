#ifndef x86_BOOTLOADER_INFO_H
#define x86_BOOTlOADER_INFO_H

#ifndef __ASM__

void retrieve_bootloader_info(uint32_t magic, void *boot_info);
size_t boot_info_get_mem_upper(void);
uint32_t boot_info_get_modules_count(void);
void boot_info_get_module(uint32_t mod_number, 
			  paddr_t *mod_start,
			  paddr_t *mod_end);

#endif

#endif
