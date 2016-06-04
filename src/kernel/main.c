/**
 * kernel/main.c
 * \brief Contains the main() function, which is called after the early system 
 *        initialisation by the code from the .boot section.
 */
#include <stdarg.h>
#include <string.h>
#include <types.h>
#include <math.h>

#include <kernel/kernel.h>
#include <kernel/kprintf.h>
#include <kernel/panic.h>
#include <kernel/symbols.h>

#include <kernel/mm/physical_pages.h>
#include <kernel/mm/virtual_pages.h>
#include <kernel/mm/slab.h>

#include <x86/boot/bootloader_info.h>
#include <x86/boot/multiboot.h>
#include <x86/x86.h>
#include <x86/gdt.h>
#include <x86/paging.h>
#include <x86/mmu.h>
#include <x86/idt.h>
#include <x86/interrupts.h>
#include <x86/tss.h>
#include <x86/8259a.h>
#include <x86/pit.h>
#include <x86/cpucheck.h>





/**
 * \fn void main(uint32_t magic, void* mb_info)
 * \brief Entry point of the kernel, called by the boot code (from .boot section). 
 * \param magic A constant which identifies how the kernel was loaded.
 * \param mb_info Pointer to the information given by the bootloader.
 */
void main(uint32_t magic, void* mb_info)
{
  /* TODO : retrieve the memory map and the amount of available memory
   * from mb_info.
   * NB : for the moment, we assume there is 4MB of memory
   */
  
  ppn_t last_ppage_ppn = paddr_to_ppn(4*MB-1);

  /*Early memory management*/
  _boot_physical_pages_init(paddr_to_ppn(ROUNDUP(kernel_pa_end,PPAGE_SIZE)), last_ppage_ppn);  
  _boot_virtual_pages_init(vaddr_to_vpn(KERNEL_SPACE+ROUNDUP(kernel_pa_end,VPAGE_SIZE)), vaddr_to_vpn(KERNEL_SPACE+4*MB-1));

  /*Architecture initialisation*/      
  gdt_init();
  idt_init();
  tss_init();
  paging_boot_init();
  pic_8259a_init();
  pit_init();
  
  /*Kernel initialisation*/
  retrieve_bootloader_info(magic, mb_info);
  physical_page_boot_init(0, last_ppage_ppn);
  objs_cache_boot_init();

  /* Objs_cache *a_cache = objs_cache_create("test", */
  /* 					  sizeof(uint32_t), */
  /* 					  1); */

  /* KASSERT(a_cache != NULL); */
  /* DEBUG_dump_objs_cache(a_cache); */
  
  /* objs_cache_alloc(a_cache); */
  /* DEBUG_dump_objs_cache(a_cache); */
  //mmu_init();


  pit_set_frequency(100);
  irq_enable(0);

  //scheduler_launch();
  asm ("cli\t\n hlt\t\n");
  for (;;);
}

