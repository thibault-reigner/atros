#include <types.h>
#include <kernel/kernel.h>
#include <kernel/symbols.h>
#include <kernel/kprintf.h>
#include <kernel/panic.h>
#include <kernel/mm/physical_pages.h>

#include <x86/boot/bootloader_info.h>
#include <x86/boot/multiboot.h>
#include <x86/boot/multiboot2.h>
#include <x86/paging.h>

#define __boot_get_virtual_address_of(paddr) ((paddr) + KERNEL_SPACE)

/****************************************************
                  Local variables
****************************************************/
static uint32_t bootloader_magic = 0;
static void *bootloader_info = NULL;

/***************************************************
                  Local functions
***************************************************/

/***************************************************
                  Global functions
***************************************************/

void retrieve_bootloader_info(uint32_t magic, void *boot_info)
{

  boot_info = __boot_get_virtual_address_of((paddr_t)boot_info);
  bootloader_magic = magic;
  bootloader_info = boot_info;

  if(magic == MULTIBOOT_BOOTLOADER_MAGIC)
    {
      kprintf("AttrOS was loaded by GRUB\n");
       
    }
  else if(magic == MULTIBOOT2_BOOTLOADER_MAGIC)
    {
      kprintf("AttrOS was loaded by GRUB2\n");
      panic("GRUB2 is not supported by AttrOS!\n");
     }
  else
    {
      panic("Attros was not loaded by a multiboot-compliant loader.\n");
    }

}


size_t boot_info_get_mem_upper(void)
{
  if(bootloader_magic == MULTIBOOT_BOOTLOADER_MAGIC)
    {
      struct Multiboot_info *mb_info = bootloader_info;
      return mb_info->mem_upper;
    }

  return 0;
}

uint32_t boot_info_get_modules_count(void)
{
  if(bootloader_magic == MULTIBOOT_BOOTLOADER_MAGIC)
    {
      struct Multiboot_info *mb_info = bootloader_info;
      return mb_info->mods_count;
    }

  return 0;
}

void boot_info_get_module(uint32_t mod_number, 
			  paddr_t *mod_start,
			  paddr_t *mod_end)
{
  *mod_start = (paddr_t)NULL;
  *mod_end   = (paddr_t)NULL;

  if(bootloader_magic == MULTIBOOT_BOOTLOADER_MAGIC)
    {
      struct Multiboot_info *mb_info = bootloader_info;
      
      if(mb_info->mods_count > mod_number)
	{
	  struct Module *modules = (struct Module*)__boot_get_virtual_address_of(mb_info->mods_addr);
	  *mod_start = modules[mod_number].mod_start;
	  *mod_end   = modules[mod_number].mod_end;
	}
    }
}
