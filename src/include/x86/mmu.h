/**
 * \file include/x86/mmu.h
 * \brief Contains definition related to MMU's contexts management,
 *        i.e: on the x86 architecture a page directory and its related
 *             page tables.
 *        MMU: Memory Management Unit
 */
#ifndef x86_MMU_H
#define x86_MMU_H

#include <kernel/mm/virtual_pages.h>
#include <kernel/mm/physical_pages.h>

#ifndef __ASM__

/**
 * \struct Mmu_context
 * \brief Describes a context of the MMU.
 */
typedef struct Mmu_context{
  
  Ppage *page_dir_ppage;
  paddr_t page_dir_paddr;
  //struct Vframe_descr *page_dir_vframe;
} Mmu_context;

void mmu_init(void);

Mmu_context *mmu_context_create(void);
void mmu_context_destroy(Mmu_context *a_mmu_context);
void mmu_context_load(Mmu_context *a_mmu_context);

#endif //__ASM__

#endif
