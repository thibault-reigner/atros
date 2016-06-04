/**
 * \file include/x86/paging.h
 * \brief Contains structure and constants definition related to paging
 *        on the x86 architecture.
 */
#ifndef x86_PAGING_H
#define x86_PAGING_H

#include <types.h>
#include <kernel/mm/physical_pages.h>
#include <kernel/mm/virtual_pages.h>

#define NBR_PD_ENTRIES 1024
#define NBR_PT_ENTRIES 1024
#define PD_SIZE 4096
#define PT_SIZE 4096

/** \brief The entry's number for recursive paging.*/
#define REC_PAGING_ENTRY 1023

/*
  Flags used for Page-Directory and Page-Table Entries (cf. Intel documentation)
*/

#define PAGE_PRESENT 1

#define PAGE_READ_ONLY 0
#define PAGE_READ_WRITE 2

#define PAGE_SUPERVISOR 0
#define PAGE_USER 4

#define PAGE_WRITE_THROUGH 8
//This flag prevents the caching of the 
//associated page or page table
#define PAGE_CACHE_DISABLED 16


/*Features added with the Pentium Pro Processor*/
#define PAGE_GLOBAL 256 //a global page's TLB is not invalidated when CR3 is reloaded
#define PAGE_4MB 128

#ifndef __ASM__

typedef uint32_t pde_t;
typedef uint32_t pte_t;

#define get_pde_index_of(vaddr) ((uint32_t)(vaddr) >> 22)
#define get_pte_index_of(vaddr) ((uint32_t)((vaddr) >> 12) & ((1UL << 10) - 1))

#define get_addr_of_pte(pte) ((paddr_t)(~((1UL << 12) - 1) & (pte)))
#define get_addr_of_pde(pde) ((paddr_t)(~((1UL << 12) - 1) & (pde)))

void paging_boot_init(void);
paddr_t virt_to_phys_addr(vaddr_t vaddr);

void map_page(ppn_t ppn, vpn_t vpn, uint32_t flags);
void map_pages(ppn_t ppn, vpn_t vpn, size_t nbr_pages, uint32_t flags);

void set_pde(uint32_t pde_index, pde_t a_pde);
pde_t get_pde(uint32_t pde_index);
void set_pte(uint32_t pde_index, uint32_t pte_index, pte_t a_pte);
pte_t get_pte(uint32_t pde_index, uint32_t pte_index);

void invlpg(vaddr_t vaddr);

#endif //__ASM__

#endif
