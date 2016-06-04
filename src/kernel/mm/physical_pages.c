#include <types.h>
#include <math.h>

#include <kernel/symbols.h>
#include <kernel/kprintf.h>
#include <kernel/list.h>
#include <kernel/panic.h>

#include <kernel/mm/slab.h>
#include <kernel/mm/virtual_pages.h>
#include <kernel/mm/physical_pages.h>

#include <x86/paging.h>

static inline void _ppage_set_free(Ppage *ppage);
static inline void _ppage_set_used(Ppage *ppage);
static inline int32_t is_buddy_block(ppn_t ppn_block, uint32_t order);
static inline void _ppage_set_mapping(Ppage *ppage, vaddr_t vaddr);
static inline void _ppage_set_slab(Ppage *ppage, Slab *slab);

static ppn_t  _boot_first_free_ppn;
static ppn_t  _boot_last_free_ppn;

/*TODO : replace the following variables by memory zones*/
static Ppage *ppages_dscrs;
static ppn_t first_ppn;
static ppn_t last_ppn;

static Ppage *free_ppages_blocks_clists[MAX_PPAGE_BLOCK_ORDER] = { NULL };
static Ppage *used_ppages_blocks_clists[MAX_PPAGE_BLOCK_ORDER] = { NULL };




static inline int32_t is_buddy_block(ppn_t ppn_block, uint32_t order)
{
  return (ppn_block % (1UL << order) == 0);
}

static inline void _ppage_set_free(Ppage *ppage)
{
  ppage->count = 0;
}

static inline void _ppage_set_used(Ppage *ppage)
{
  ppage->count = 1;
}

inline paddr_t ppage_paddr_of(paddr_t paddr)
{
  return ((paddr) & (paddr_t)~((1 << PPAGE_SHIFT) - 1));
}

inline ppn_t ppn_of(paddr_t paddr)
{
  return paddr_to_ppn(ppage_paddr_of(paddr));
}

inline paddr_t ppn_to_paddr(ppn_t ppn)
{
  return (paddr_t)(ppn << PPAGE_SHIFT);
}

inline ppn_t paddr_to_ppn(paddr_t paddr)
{
  return (ppn_t)(paddr >> PPAGE_SHIFT);
}

inline ppn_t ppage_to_ppn(const Ppage *ppage)
{
  KASSERT(ppage != NULL);
  return (((paddr_t)ppage - (paddr_t)ppages_dscrs) / sizeof(Ppage));
}

inline Ppage *ppn_to_ppage(ppn_t ppn)
{
  KASSERT(first_ppn <= ppn);
  KASSERT(last_ppn  >= ppn);
  return (&ppages_dscrs[ppn]);
}

inline uint32_t ppage_count(const Ppage *ppage)
{
  KASSERT(ppage != NULL);
  return ppage->count;
}

inline void ppage_ref(Ppage *ppage)
{
  KASSERT(ppage != NULL);
  ppage->count++;
}

inline void ppage_unref(Ppage *ppage)
{
  KASSERT(ppage != NULL);
  if (ppage->count > 1)
    ppage->count--;
  else if (ppage->count == 1)
    _ppage_block_free(ppage, ppage->block_order);
  else
    panic("Try to unreference a free physical page in %s\n", __func__);
}

inline uint32_t ppage_is_free(const Ppage *ppage)
{
  KASSERT(ppage != NULL);
  return (ppage_count(ppage) == 0);
}

inline paddr_t ppage_to_paddr(const Ppage *ppage)
{
  KASSERT(ppage != NULL);
  return (ppage_to_ppn(ppage) << PPAGE_SHIFT);
}

inline Ppage *paddr_to_ppage(paddr_t paddr)
{
  return ppn_to_ppage((paddr & (~((1UL << PPAGE_SHIFT) - 1))) >> PPAGE_SHIFT);
}

static inline void _ppage_set_mapping(Ppage *ppage, vaddr_t vaddr)
{
  KASSERT(ppage != NULL);
  ppage->mapping = vaddr;
}

static inline void _ppage_set_slab(Ppage *ppage, Slab *slab)
{
  KASSERT(ppage != NULL);
  KASSERT(slab != NULL);
  
  ppage->slab = slab;
}

/***************************************
          Private functions
****************************************/


static void _range_ppages_set_status(ppn_t first_ppage, ppn_t last_ppage, uint32_t status)
{
  if (status != PPAGE_STATUS_FREE && status != PPAGE_STATUS_USED)
    panic("Incorrect status parameter in %s\n", __func__);
  
  if (first_ppage > last_ppage)
    {
#ifdef DEBUG
      kprintf("first_ppage > last_ppage in %s!\n", __func__);
#endif
    }
  
  for (ppn_t ppn = first_ppage; ppn <= last_ppage;)
    {
      /*We find the biggest buddy block which start at the page ppn 
	Note : this loop always ends since any page is at least the head of a block of order 0*/
      for (uint32_t order = MAX_PPAGE_BLOCK_ORDER - 1 ;; order--)
	{
	  if (is_buddy_block(ppn, order) &&
	      ppn + (1UL << order) - 1 <= last_ppage)
	    {
	      //we initialize the descriptor of the page which is the head of the block

	      if (status == PPAGE_STATUS_FREE)
		_ppage_set_free(&ppages_dscrs[ppn]);
	      else //if (status == PPAGE_STATUS_USED)
		_ppage_set_used(&ppages_dscrs[ppn]);
 
	      ppages_dscrs[ppn].block_head = TRUE;
	      ppages_dscrs[ppn].block_order = order;
	      
	      ppages_dscrs[ppn].slab = NULL;
	      ppages_dscrs[ppn].mapping = (vaddr_t)NULL;
		  
	      if (status == PPAGE_STATUS_FREE)
		clist_push_tail(free_ppages_blocks_clists[order], &ppages_dscrs[ppn]);
	      else //if (status == PPAGE_STATUS_USED)
		clist_push_tail(used_ppages_blocks_clists[order], &ppages_dscrs[ppn]);

	      //we initialize the descriptors of the pages in the block
	      for (ppn_t i = ppn + 1 ; i < ppn + (1UL << order) ; i++)
		{
		  if (status == PPAGE_STATUS_FREE)
		    _ppage_set_free(&ppages_dscrs[i]);
		  else //if (status == PPAGE_STATUS_USED)
		    _ppage_set_used(&ppages_dscrs[i]);
		  
		  ppages_dscrs[i].block_head = FALSE;
		  ppages_dscrs[i].block_order = order; //in fact the value of this field is meaningless in this case since block_head = FALSE
		  ppages_dscrs[i].slab = NULL;
		  ppages_dscrs[i].mapping = (vaddr_t)NULL;
		  
		  ppages_dscrs[i].prev = NULL;
		  ppages_dscrs[i].next = NULL;
		}
	      
	      ppn += (1UL << order);
	      
	      break;
	    }
	}
    }
}


/**************************************************
               Public functions
**************************************************/


void _boot_physical_pages_init(ppn_t first_free_ppage, ppn_t last_free_ppage)
{
  _boot_first_free_ppn = first_free_ppage;
  _boot_last_free_ppn  = last_free_ppage;
} 

ppn_t _boot_physical_pages_alloc(size_t nbr_ppages)
{
  ppn_t to_return = 0;
  
  if (_boot_first_free_ppn + nbr_ppages - 1 <= _boot_last_free_ppn)
    {
      to_return = _boot_first_free_ppn;
      _boot_first_free_ppn += nbr_ppages;
    }
  else
    {
      panic("Can't allocate %lu physical page(s) in %s!\n", nbr_ppages,  __func__);
    }

  return to_return;
}


void physical_page_boot_init(ppn_t first_ppage, ppn_t last_ppage)
{
  first_ppn = first_ppage;
  last_ppn = last_ppage;

  size_t nbr_pages_used_by_ppages_dscrs = ROUNDUP((last_ppage - first_ppage + 1) * sizeof(Ppage), PPAGE_SIZE) / PPAGE_SIZE;

  ppn_t ppages_dscrs_ppn = _boot_physical_pages_alloc(nbr_pages_used_by_ppages_dscrs);
  vpn_t ppages_dscrs_vpn = _boot_virtual_pages_alloc(nbr_pages_used_by_ppages_dscrs);
  
  ppages_dscrs = (Ppage*)vpn_to_vaddr(ppages_dscrs_vpn);

  kprintf("%s()\n", __func__);
  kprintf("  ppages_dscrs physical size (in pages) : %u\n", nbr_pages_used_by_ppages_dscrs);
  kprintf("  ppages_dscrs physical location : %p\n", ppn_to_paddr(ppages_dscrs_ppn));
  kprintf("  ppages_dscrs virtual location : %p\n", ppages_dscrs);
  kprintf("  first_ppage : %u\n", first_ppage);
  kprintf("  last_ppage : %u\n", last_ppage);
  kprintf("  Size of struct Ppage : %u\n", sizeof(Ppage));
  
  map_pages(ppages_dscrs_ppn,
	    ppages_dscrs_vpn,
	    nbr_pages_used_by_ppages_dscrs,
	    PAGE_PRESENT | PAGE_READ_WRITE | PAGE_SUPERVISOR | PAGE_GLOBAL);

  //we set the first physical MB of the memory as reserved by the kernel
  _range_ppages_set_status(0, paddr_to_ppn(1*MB)-1, PPAGE_STATUS_USED);
  //mark as free the memory between the first MB and the kernel image
  _range_ppages_set_status(paddr_to_ppn(1*MB), paddr_to_ppn(boot_pa_start) - 1, PPAGE_STATUS_FREE);
  //we mark as used the image of the kernel
  _range_ppages_set_status(paddr_to_ppn(boot_pa_start), paddr_to_ppn(ROUNDUP(kernel_pa_end, PPAGE_SIZE)) - 1, PPAGE_STATUS_USED);
  //we mark as used the memory allocated by the boot physical memory allocator
  _range_ppages_set_status(paddr_to_ppn(kernel_pa_end), _boot_first_free_ppn - 1, PPAGE_STATUS_USED);
  //mark as free the memory after
  _range_ppages_set_status(_boot_first_free_ppn, last_ppage, PPAGE_STATUS_FREE);
  kprintf("  Kernel's pages : [%u , %u]\n",paddr_to_ppn(boot_pa_start), paddr_to_ppn(ROUNDUP(kernel_pa_end, PPAGE_SIZE)) - 1);
  kprintf("  Pages allocated during boot : [%u , %u]\n", paddr_to_ppn(kernel_pa_end), _boot_first_free_ppn - 1, PPAGE_STATUS_USED);
  kprintf("  Free pages : [%u , %u]\n",  _boot_first_free_ppn, last_ppage, PPAGE_STATUS_FREE);
}


ppn_t ppage_block_alloc(uint32_t order)
{
  Ppage *to_return = _ppage_block_alloc(order);

  if (to_return == NULL)
    panic("Failed to allocate a block of physical ppages in %s\n", __func__);

  return ppage_to_ppn(to_return);
}


/*Try to allocate a block of physical contiguous pages of a given order.
  Return a pointer to Ppage structure of the first ppage of the block if successful
  NULL otherwise*/
Ppage *_ppage_block_alloc(uint32_t order)
{
  KASSERT(order < MAX_PPAGE_BLOCK_ORDER);
  
  Ppage *to_return = NULL;

  if (clist_is_empty(free_ppages_blocks_clists[order]))
    {
      Ppage *bigger_block = _ppage_block_alloc(order+1);

      if (bigger_block != NULL)
	{
	  //we split the bigger_block in two
	  
	  Ppage *upper_part = ppn_to_ppage(ppage_to_ppn(bigger_block) + (ppn_t)(1 << order));
	  upper_part->block_head = TRUE;
	  upper_part->block_order = order;

	  //we release the upper (unused) part
	  _ppage_set_free(upper_part);
	  clist_push_tail(free_ppages_blocks_clists[order], upper_part);
	  
	  bigger_block->block_order--;
	  clist_push_tail(used_ppages_blocks_clists[order], bigger_block);
	  to_return = bigger_block;
	}
    }
  else
    {
      to_return = clist_pop_head(free_ppages_blocks_clists[order]);
      clist_push_tail(used_ppages_blocks_clists[order], to_return);     
      _ppage_set_used(to_return);
    }


  return to_return;
  
}


void ppage_block_free(ppn_t ppn, uint32_t order)
{
  _ppage_block_free(ppn_to_ppage(ppn), order);
}

void _ppage_block_free(Ppage *block, uint32_t order)
{
  KASSERT(block != NULL);
  KASSERT(block->block_head == TRUE);
  KASSERT(block->block_order == order);
  KASSERT(block->block_order < MAX_PPAGE_BLOCK_ORDER);
  KASSERT(ppage_count(block) == 1);

  KASSERT(block->mapping == (vaddr_t)NULL);
  KASSERT(block->slab == NULL);
  
  clist_delete_el(used_ppages_blocks_clists[order], block);
  _ppage_set_free(block);

  //We try to merge as much as possible blocks
  while (order < (MAX_PPAGE_BLOCK_ORDER - 1))
    {
      ppn_t buddy_ppn = find_buddy(ppage_to_ppn(block), block->block_order);
      Ppage *buddy = ppn_to_ppage(buddy_ppn);

      if (ppage_is_free(buddy) &&
	  buddy->block_head == TRUE &&
	  buddy->block_order == order)
	{
	  clist_delete_el(free_ppages_blocks_clists[order], buddy);
	  block->block_head = FALSE;
	  buddy->block_head = FALSE;
	  ppn_t merged_block_ppn = ppage_to_ppn(block) & buddy_ppn;
	  Ppage *merged_block = ppn_to_ppage(merged_block_ppn);
	  merged_block->block_head = TRUE;
	  order++;
	  merged_block->block_order = order;

	  block = merged_block;
	}
      else
	{
	  break;
	}
    }
  
  clist_push_tail(free_ppages_blocks_clists[order], block);
}

ppn_t ppage_alloc(void)
{
  return ppage_block_alloc(0);
}

Ppage *_ppage_alloc(void)
{
  return _ppage_block_alloc(0);
}

void ppage_free(ppn_t ppn)
{
  _ppage_free(ppn_to_ppage(ppn));
}

void _ppage_free(Ppage *ppage)
{
  _ppage_block_free(ppage,0);
}

ppn_t ppages_alloc(size_t nbr_ppages)
{
  uint32_t order;
  
  if (nbr_ppages >= (1UL << MAX_PPAGE_BLOCK_ORDER))
    panic("Try to allocate more that (1 << MAX_PPAGE_BLOCK_ORDER) physical pages in %s\n", __func__);

  for (order = 0; order < MAX_PPAGE_BLOCK_ORDER; order++)
    {
      if (nbr_ppages <= (1UL << order))
	break;
    }
  
  return ppage_block_alloc(order);
}

void ppages_set_slab(ppn_t ppn, uint32_t nbr_pages, const Slab *slab)
{
  KASSERT(ppn <= last_ppn);
  KASSERT(ppn + nbr_pages - 1 <= last_ppn);
  KASSERT(slab != NULL);
  KASSERT(slab->vregion != NULL);
  KASSERT(slab->vregion->nbr_pages == nbr_pages);

  for(uint32_t i = 1; i <= nbr_pages; i++)
    {
      _ppage_set_slab(ppn_to_ppage(ppn), slab);
      ppn++;
    }
}

