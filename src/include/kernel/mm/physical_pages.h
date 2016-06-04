/**
 * \file include/kernel/physical_pages.h
 * \brief Contains definitions related to the physical
 *        pages allocator algorithm.
 */
#ifndef KERNEL_PHYSICAL_PAGES_H
#define KERNEL_PHYSICAL_PAGES_H

#include <types.h>
#include <kernel/kernel.h>

#define MAX_PPAGE_BLOCK_ORDER 10
#define PPAGE_STATUS_USED 1
#define PPAGE_STATUS_FREE 0


#define PPAGE_SHIFT 12
#define PPAGE_SIZE  (1 << PPAGE_SHIFT)
#define PPAGE_MASK  (VPAGE_SIZE - 1)

#ifndef __ASM__

//Forward definition from kernel/mm/slab.h
struct Slab;
typedef struct Slab Slab;

//Physical page number
#ifdef __ARCH_x86__
typedef uint32_t ppn_t;
#define NULL_PPN MAX_UINT32

#else

#ifdef __ARCH_x86_64__
typedef uint64_t ppn_t;
#define NULL_PPN MAX_UINT64
#endif

#endif


typedef struct Physical_page_dscr{

  bool_t block_head:1;
  uint32_t block_order:8;
  uint32_t count; //references counter

  Slab *slab;
  
  vaddr_t mapping;
  
  struct Physical_page_dscr *prev, *next;
}Ppage;

#define find_buddy(addr, order) \
  ((addr) & (1UL << (order)))

/**
 * \struct Physical_memory_zone
 * \brief Describe a physical memory zone where we can allocate physical pages
 *
 * This structure contains useful information on a physical memory zone and
 * structures used to allocate/free its pages.
 */
/** \todo Implement used frames lists in Physical_memory_zone ?*/
typedef struct Physical_memory_zone
{
  uint32_t first_ppage_nbr;  /**< Number of the first physical page in the memory zone*/
  uint32_t pages_count;      /**< Counter of physical pages in this memory zone*/
  
  uint32_t free_pages_count; /**< Counter of free pages in the memory zone*/
  uint32_t used_pages_count; /**< Counter of used pages in the memory zone*/

  uint32_t free_frames_count;
  uint32_t used_frames_count;

  //Ppage_descr *ppages;       /**< Pointer to the array of the physical pages's descriptors*/
  //Ppage_descr *free_frames_lists[MAX_PPAGE_FRAME_ORDER + 1];

} Physical_memory_zone;



paddr_t  ppage_paddr_of(paddr_t paddr);
ppn_t    ppage_to_ppn(const Ppage *ppage);
paddr_t  ppage_to_paddr(const Ppage *ppage);

ppn_t    ppn_of(paddr_t paddr);
paddr_t  ppn_to_paddr(ppn_t ppn);
Ppage   *ppn_to_ppage(ppn_t ppn);

ppn_t    paddr_to_ppn(paddr_t paddr);
Ppage   *paddr_to_ppage(paddr_t paddr);

uint32_t ppage_count(const Ppage *ppage);
void     ppage_ref(Ppage *ppage);
void     ppage_unref(Ppage *ppage);
uint32_t ppage_is_free(const Ppage *ppage);

void physical_page_boot_init(ppn_t first_ppage, ppn_t last_ppage);

ppn_t ppage_block_alloc(uint32_t order);
Ppage *_ppage_block_alloc(uint32_t order);
void ppage_block_free(ppn_t ppn, uint32_t order);
void _ppage_block_free(Ppage *block, uint32_t order);

ppn_t ppage_alloc(void);
Ppage *_ppage_alloc(void);
void ppage_free(ppn_t ppn);
void _ppage_free(Ppage *ppage);
  
ppn_t ppages_alloc(size_t nbr_ppages);

void ppages_set_slab(ppn_t ppn, uint32_t nbr_pages, const Slab *slab);
  
//Relevant only during the booting phase of the kernel
void _boot_physical_pages_init(ppn_t first_free_ppage, ppn_t last_free_ppage);
ppn_t _boot_physical_pages_alloc(size_t nbr_ppages);

#endif //__ASM__

#endif

