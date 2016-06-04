/**
* \file arch/x86/paging.c
* \brief Contains data and functions used for paging management
*
* The two main structured linked to paging management are: \n
* PGD: Page Directory \n
* PGT: Page Table \n
* These structures are used by x86 processors to translate virtual
* addresses to physical ones when paging is used.
*/

#include <types.h>
#include <math.h>
#include <string.h>

#include <kernel/kernel.h>
#include <kernel/kprintf.h>
#include <kernel/panic.h>
#include <kernel/symbols.h>

#include <kernel/mm/virtual_pages.h>
#include <kernel/mm/physical_pages.h>


#include <x86/paging.h>


extern uint32_t _kpgd[1024];
extern uint32_t _first_kpgt[1024];


/*******************************************************
                   Private functions
********************************************************/

static void load_pgd(void* pgd);
static void enable_paging(void);

/**
* \fn static void load_pgd(void* pgd)
* \brief Loads the address of the page directory in the register CR3
* \param pgd The physical address of the page directory, to load in 
*        the register CR3.
*/
static void load_pgd(void* pgd)
{
  asm volatile("movl %0, %%eax\n" \
	       "movl %%eax, %%cr3\n" \
	       :: "m" (pgd)
	       : "eax");
}

/**
* \fn static void enable_paging(void)
* \brief Enables the paging.
*/
static void enable_paging(void)
{
  //Set the paging-enable flag of the register CR0
  asm volatile ("movl %cr0, %eax \t\n" \
	       "orl $0x80000000, %eax \t\n" \
	       "movl %eax, %cr0 \t\n"
	       "jmp 1f \t\n" \
	       "1: \t\n");
}



/*****************************************************************
                      Public functions
****************************************************************/

/**
 * \fn static void invlpg(vaddr_t vaddr)
 * \brief Invalidate TLB entry for page that contains vaddr.
 * \param vaddr The virtual address whose translation should be
 *              be invalidated.
 */
void invlpg(vaddr_t vaddr)
{
#ifdef __SUBARCH_i386__
  /*On the 386 processor the only way to invalidate a TLB is to
    invalidate ALL the TLBs by reloading the CR3 register*/
  asm volatile ("mov %cr3, %eax\t\n"
	       "mov %eax, %cr3\t\n");
#else
  /*On newer architectures we can use the invlpg instruction*/
   asm volatile ("invlpg (%0)" ::"r" (vaddr) : "memory");
#endif
}


/**
 * \fn bool_t set_pde(uint32_t *pd,
 *                           Page_dir_entry pde)
 * \brief Set a value to the given page directory's entry.
 * \param pd Pointer to the page directory to modify.
 * \param pde_index The index of the entry to set.
 * \param pde Value to set.
 * \return TRUE if the value was set, FALSE otherwise.
 *
 * Set the given value to the entry of the current page directory, using
 * recursive paging through the recursive paging entry of the page directory.
 */
void set_pde(uint32_t pde_index,
	     pde_t a_pde)
{
  pde_t *rec_paging_vaddr = (pde_t*)(REC_PAGING_ENTRY * 4*MB + 
				     REC_PAGING_ENTRY * 4*KB +
				     pde_index * 4);
  *rec_paging_vaddr = a_pde;
}

/**
 * \fn Page_dir_entry get_pde(uint32_t pde_index)
 * \brief Return the value of the given entry of the current page directory.
 * \param pde_index The index of the entry to get.
 * \return The value of this entry.
 */
pde_t get_pde(uint32_t pde_index)
{
  pde_t *rec_paging_vaddr = (pde_t*)(REC_PAGING_ENTRY * 4*MB + 
				     REC_PAGING_ENTRY * 4*KB +
				     pde_index * 4);	
  return *rec_paging_vaddr;
}

/**
 * \fn bool_t set_pte(uint32_t pt_index_in_pd,
 *                           uint32_t pte_index,
 *	                     Page_table_entry pte)
 * \brief Set a value to an entry of the given page table
 * \param pt_index_in_pd Index of the entry in the page directory which is 
 *                       associated to the page table to modify.
 * \param pte_index      Index of the entry to set in the page table.
 * \param pte            Value to set to the page table entry.
 * \return TRUE if the value was set, FALSE otherwise
 *
 * Set the value in a page table, using the recursive paging entry of the
 * page directory entry.
 */
void set_pte(uint32_t pde_index,
	     uint32_t pte_index,
	     pte_t a_pte)
{

  pte_t *rec_paging_vaddr = (pte_t*)(REC_PAGING_ENTRY * 4*MB +
				    pde_index * 4*KB +
				    pte_index * 4);
  *rec_paging_vaddr = a_pte;
 
}

/**
 * \fn Page_dir_entry get_pte(uint32_t pt_index_in_pd, uint32_t pte_index)
 * \brief Return the value of an entry in a page table
 * \param pt_index_in_pd The index of the page table in the page directory.
 * \param pte_index The index of the entry to get in the page table.
 * \return The value of this entry.
 */
pte_t get_pte(uint32_t pde_index,
	      uint32_t pte_index)
{
  pte_t *rec_paging_vaddr = (pte_t*)(REC_PAGING_ENTRY * 4*MB +
				     pde_index * 4*KB +
				     pte_index * 4);
  return *rec_paging_vaddr;
}

/**
 * \fn paddr_t virt_to_phys_addr(vaddr_t vaddr)
 * \brief Return the physical address associated to the given virtual
 *        address.
 * \param vaddr Virtual address to translate in a physical address
 * \return The associated physical address, NULL if none.
 */
paddr_t virt_to_phys_addr(vaddr_t vaddr)
{
  paddr_t to_return = (paddr_t)NULL;
  uint32_t pde_index, pte_index;
  pde_t pde;
  pte_t pte;
  
  pde_index = get_pde_index_of(vaddr);
  pte_index = get_pte_index_of(vaddr);

  pde = get_pde(pde_index);
  
  //Is the page table present ?
  if (pde & PAGE_PRESENT)
    {
      //Is the page a 4Mb page ?
      if (!(pde & PAGE_4MB))
	{
	  pte = get_pte(pde_index, pte_index);
	  //Is the virtual page present ?
	  if (pte & PAGE_PRESENT)
	    {
	      //We compute the associated physical address
	      to_return = get_addr_of_pte(pte);
	      to_return += vaddr & ((1 << 12) - 1);
	    }
	}
      //4Mb-page, defined in the page directory's entry
      else
	{
	  to_return = get_addr_of_pde(pde);
	  to_return += vaddr & ((1 << 22) - 1);
	}
    }
  return to_return;
}



/**
 * \fn void vpage_unmap(vaddr_t vaddr)
 * \brief Unmap the virtual address vaddr
 * \param vaddr The virtual address to unmap
 */
void vpage_unmap(vaddr_t vaddr)
{
  /* //We compute the page directory and page table entries associated to */
  /* //the virtual address to map. */
  /* uint32_t pde_index = get_pde_index_of(vaddr); */
  /* uint32_t pte_index = get_pte_index_of(vaddr); */
  
  /* pde_t pde = get_pde(pde_index); */

  /* if (pde & PAGE_PRESENT) */
  /*   { */
  /*     if (!(pde & PAGE_4MB)) */
  /* 	{ */
  /* 	  pte_t pte = get_pte(pde_index, pte_index); */
  /* 	  pte = pte & (~PAGE_PRESENT); */
  /* 	  set_pte(pde_index, pte_index, pte); */
  /* 	} */
  /*     else */
  /* 	{ */
  /* 	  //We have to unmap a 4Mb page */
  /* 	  pde  = pde & (~PAGE_PRESENT); */
  /* 	  set_pde(pde_index,pde); */
  /* 	  /\** \todo Unreferences the physical pages*\/ */
  /* 	} */
  /*     invlpg(vaddr); */
  /*   } */
}

/* void vpage_map_area(vaddr_t vaddr, uint32_t nbr_pages) */
/* { */
/*   paddr_t paddr; */

/*   while(nbr_pages > 0) */
/*     { */
/*       nbr_pages--; */
/*       paddr = ppage_to_paddr(ppage_alloc()); */

/*       if(paddr != (paddr_t)NULL) */
/* 	{ */
/* 	  vpage_map(vaddr, */
/* 		    paddr, */
/* 		    PAGE_PRESENT | PAGE_READ_WRITE | PAGE_SUPERVISOR); */
/* 	} */
/*       else */
/* 	{ */
/* 	  panic("Failed to allocate a physical page in vpage_map_area()!\n"); */
/* 	} */
/*       vaddr += VPAGE_SIZE; */
/*     } */
/* } */

/* void vpage_unmap_area(vaddr_t vaddr, uint32_t nbr_pages) */
/* { */
/*   while(nbr_pages > 0) */
/*     { */
/*       vpage_unmap(vaddr); */
/*       vaddr += VPAGE_SIZE; */
/*     } */
/* } */

void map_page(ppn_t ppn, vpn_t vpn, uint32_t flags)
{
  paddr_t paddr = ppn_to_paddr(ppn);
  vaddr_t vaddr = vpn_to_vaddr(vpn);
  
  if (vaddr == 0)
    panic("Try to map NULL in %s!\n", __func__);

  size_t pde_index = get_pde_index_of(vaddr);
  size_t pte_index = get_pte_index_of(vaddr);

  if (!(flags & PAGE_4MB) )
    {
      pde_t pde = get_pde(pde_index);

      if ( !(pde & PAGE_PRESENT) )
	{
	  panic("(%s()) Page table not present !\n", __func__);
	  /* /\** \todo If we map a page in the user space, allocate a physical pages */
	  /*  *        from the user's physical pages list ?*\/ */
	  /* paddr_t new_pgt_paddr = ppage_to_paddr(ppage_alloc()); */

	  /* if(new_pgt_paddr != (paddr_t)NULL) */
	  /*   { */
	  /*     //The new value to load in the page directory entry */
	  /*     pde = new_pgt_paddr | flags; */

	  /*     //We associate this new page table to the page directory entry */
	  /*     set_pde(pde_index, pde); */

	  /*     //Clear the allocated page table. */
	  /*     /\** \todo Optimise with a memset()*\/ */
	  /*     uint32_t i; */
	  /*     for(i=0; i<NBR_PT_ENTRIES ;i++) */
	  /* 	{ */
	  /* 	  set_pte(pde_index,i,0); */
	  /* 	} */
	}

   
      pte_t pte = paddr | flags;
      set_pte(pde_index, pte_index, pte);

      invlpg(vaddr);
    }
  else
    {
      panic("4MB pages not handled !\n");
    }
}

void map_pages(ppn_t ppn, vpn_t vpn, size_t nbr_pages, uint32_t flags)
{

  if (vpn == 0)
    panic("Try to map NULL in %s!\n", __func__);
  
  while (nbr_pages > 0)
    {
      map_page(ppn, vpn, flags);

      ppn++;
      vpn++;
      nbr_pages--;
    }
}

/**
* \fn void paging_boot_init(void)
* \brief Set up paging and the required structures
*
* This functions maps the first 4Mb of the kernel space to the first
* 4Mb of the physical memory.
*/
void paging_boot_init(void)
{

  /*We set up the kernel page directory*/

  //Since _kpgd and _first_kpgt are in the .bss section, we could assume they are
  //already initialised with 0 values.
  memset(_kpgd,0, NBR_PD_ENTRIES * sizeof(pde_t));
  memset(_first_kpgt, 0, NBR_PT_ENTRIES * sizeof(pte_t));

  //We map some pages of the first 4Mb of the kernel space
   _kpgd[KERNEL_SPACE >> 22] =					\
    ((vaddr_t)_first_kpgt - KERNEL_SPACE) | //physical address of the first page table
    PAGE_SUPERVISOR |
    PAGE_READ_WRITE |
    PAGE_PRESENT;

  //We initialise the recursive paging entry of the page directory
  _kpgd[REC_PAGING_ENTRY] = \
    ((uint32_t)_kpgd - KERNEL_SPACE) | //physical address of the page directory
    PAGE_SUPERVISOR |
    PAGE_READ_WRITE |
    PAGE_PRESENT;

  /*We set up the first page table*/

  //We set up the entries which map the kernel image
  ppn_t first_ppage_ppn = paddr_to_ppn(boot_pa_start);
  ppn_t last_ppage_ppn = paddr_to_ppn(ROUNDUP(kernel_pa_end,PPAGE_SIZE))-1;

  for (ppn_t i = first_ppage_ppn; i <= last_ppage_ppn; i++)
    {
      _first_kpgt[i] = \
	(i << 12) |
	PAGE_SUPERVISOR |
	PAGE_READ_WRITE |
	PAGE_PRESENT;
    }

  //We set up the entries which map the first Mb of the kernel space to the
  //first Mb of the physical memory.
  for (uint32_t i = 0; i < (MB >> VPAGE_SHIFT); i++)
    {
      _first_kpgt[i] = \
	(i << 12) |
	PAGE_SUPERVISOR |
	PAGE_READ_WRITE |
	PAGE_PRESENT;
    }

  //We load the new kernel page directory into the CR3 register
  //NB: at this stage we can't use virt_to_phys_addr() since the
  //current page directory doesn't have a recursive paging entry.
  load_pgd((void*)((vaddr_t)_kpgd - KERNEL_SPACE));
}



