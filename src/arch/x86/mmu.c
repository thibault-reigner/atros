/**
 * \file arch/x86/mmu.c
 * \brief Contains functions to manage MMU context.
 *        The management of the current MMU's context is 
 *        done through the functions of paging.c.
 */
#include <types.h>
#include <string.h>


#include <kernel/kprintf.h>
#include <kernel/panic.h>

#include <kernel/mm/slab.h>
#include <kernel/mm/physical_pages.h>

#include <x86/mmu.h>
#include <x86/paging.h>

/*****************************************************
                    Local variables
******************************************************/

static struct Objs_cache *mmu_context_cache = NULL;

static struct Mmu_context *current_mmu_context = NULL;

/*****************************************************
                    Local functions
******************************************************/
static bool_t preserve_kernel_space(struct Mmu_context *a_mmu_context);

/**
 * \fn static bool_t preserve_kernel_space(struct Mmu_context *a_mmu_context)
 * \brief Copy the kernel space mapping from the current context to the given context
 * \param a_mmu_context The context where to copy the kernel space mapping.
 * \return TRUE if copy is successful, FALSE otherwise.
 */
static bool_t preserve_kernel_space(struct Mmu_context *a_mmu_context)
{
  bool_t to_return = FALSE;
  /*uint32_t i;*/

  /* if(a_mmu_context != NULL) */
  /*   { */
  /*     if(a_mmu_context != current_mmu_context) */
  /* 	{ */
  /* 	  if(a_mmu_context->page_dir_vframe != NULL) */
  /* 	    { */
  /* 	      /\* */
  /* 	       * Now we copy the entries of the current page directory which define */
  /* 	       * the kernel space to the new current page directory. */
  /* 	       * This way the kernel space is preserved whenever the mmu's context */
  /* 	       * is changed. */
  /* 	       *\/ */
  /* 	      pde_t *page_dir = (pde_t*)a_mmu_context->page_dir_vframe->addr; */

  /* 	      /\*Keep the kernel space in the address space defined by the new */
  /* 		page directory*\/ */
  /* 	      for(i = get_pde_index_of(KERNEL_SPACE); i < NBR_PD_ENTRIES; i++) */
  /* 		{ */
  /* 		  page_dir[i] = get_pde(i); */
  /* 		} */

  /* 	      //Set the recursive paging entry for the new page directory */
  /* 	      page_dir[REC_PAGING_ENTRY] = \ */
  /* 		a_mmu_context->page_dir_paddr  | */
  /* 		PAGE_SUPERVISOR | */
  /* 		PAGE_READ_WRITE | */
  /* 		PAGE_PRESENT; */

  /* 	      to_return = TRUE; */
  /* 	    } */
  /* 	  else */
  /* 	    { */
  /* 	      panic("The target page directory is not mapped in the kernel space in preserve_kernel_space()!\n"); */
  /* 	    } */

  /* 	} */
  /*     else */
  /* 	{ */
  /* 	  to_return = TRUE; */
  /* 	} */
  /*   } */
  return to_return;
}

/*****************************************************
                    Global functions
******************************************************/

/**
 * \fn void mmu_init(void)
 * \brief Initialises the management of MMU's contexts.
 */
void mmu_init(void)
{
  mmu_context_cache = objs_cache_create("Mmu_context",
					sizeof(struct Mmu_context),
					2);
  if (mmu_context_cache == NULL)
    {
      panic("Creation of a cache for Mmu_context structures failed in mmu_init()!\n");
    }
}

/**
 * \fn struct Mmu_context *mmu_context_create(void)
 * \brief Creates and initialises a new MMU context.
 * \return Pointer to the created context
 */
struct Mmu_context *mmu_context_create(void)
{
  struct Mmu_context *mmu_context_created = NULL;


/*   //Allocate a new Mmu_context structure */
/*   mmu_context_created = objs_cache_alloc(mmu_context_cache); */

/*   if(mmu_context_created != NULL) */
/*     { */
/*       //Allocate a new physical page which will contain the page directory */
/*       mmu_context_created->page_dir_ppage = ppage_alloc(); */
/*       mmu_context_created->page_dir_paddr = ppage_to_paddr(mmu_context_created->page_dir_ppage); */
      
/*       if(mmu_context_created->page_dir_paddr != (paddr_t)NULL) */
/* 	{ */
/* 	  //Allocate a virtual page where the new page directory will be mapped */
/* 	  mmu_context_created->page_dir_vframe = vregion_alloc(0); */

/* 	  if(mmu_context_created->page_dir_vframe != NULL) */
/* 	    { */
/* 	      //Map the page directory in the kernel space */
/* 	      vpage_map(mmu_context_created->page_dir_vframe->addr, */
/* 			mmu_context_created->page_dir_paddr, */
/* 			PAGE_PRESENT | */
/* 			PAGE_SUPERVISOR | */
/* 			PAGE_READ_WRITE); */

/* 	      //Set as unpresent the whole address space */
/* 	      memset((void*)mmu_context_created->page_dir_vframe->addr,  */
/* 		     0, */
/* 		     NBR_PD_ENTRIES * sizeof(pde_t)); */
/* 	    } */
/* 	  else  */
/* 	    { */
/* #ifdef DEBUG */
/* 	      kprintf("Failed to allocate a new virtual page in mmu_context_create()!\n"); */
/* #endif */
/* 	      ppage_free(mmu_context_created->page_dir_ppage); */
/* 	      objs_cache_free(mmu_context_cache, mmu_context_created); */
/* 	    } */
/* 	} */
/*       else */
/* 	{ */
/* #ifdef DEBUG */
/* 	  kprintf("Failed to allocate a new physical page for a new page directory in mmu_context_create()!\n"); */
/* #endif */
/* 	  objs_cache_free(mmu_context_cache, mmu_context_created); */
/* 	} */
/*     } */
/*   else */
/*     { */
/* #ifdef DEBUG */
/*       kprintf("Failed to create a new MMU context in mmu_context_create()!\n"); */
/* #endif */
/*     } */

  return mmu_context_created;
}

/**
 * \fn void mmu_context_destroy(struct Mmu_context *a_mmu_context)
 * \brief Destroys a MMU context, i.e: releases all the ressources
 *        allocated during the creation and life of this context.
 * \param a_mmu_context Pointer to the MMU context to destroy.
 */
void mmu_context_destroy(struct Mmu_context *a_mmu_context)
{
  /* if(a_mmu_context != NULL) */
  /*   { */
  /*     /\** \todo Release all the physical pages used by the page */
  /* 	  tables used by this context.*\/ */
  /*     panic("mmu_context_destroy() is not implemented!\n"); */
      
  /*     vpage_unmap(a_mmu_context->page_dir_vframe->addr); */
  /*     //vregion_free(a_mmu_context->page_dir_vframe); */
  /*     ppage_free(a_mmu_context->page_dir_ppage); */
  /*     objs_cache_free(mmu_context_cache, a_mmu_context); */
  /*   } */
}

/**
 * \fn void mmu_context_load(struct Mmu_context *a_mmu_context)
 * \brief Loads the given MMU context.
 * \param a_mmu_context Pointer to the MMU context to load as the current
 *                      MMU context.
 */
void mmu_context_load(struct Mmu_context *a_mmu_context)
{
  if (a_mmu_context != NULL)
    {
      if (a_mmu_context->page_dir_paddr != (paddr_t)NULL)
  	{
  	  /*Before loading the new mmu context, we have to save
  	    the kernel space mapping from the current one.*/
  	  if (preserve_kernel_space(a_mmu_context) == TRUE)
  	    {
  	      current_mmu_context = a_mmu_context;

  	      /*Load the physical address of the new page directory in
  		the register CR3.*/
  	      asm volatile("movl %0, %%eax\n" \
  			   "movl %%eax, %%cr3\n" \
  			   :: "m" (a_mmu_context->page_dir_paddr)
  			   : "eax");

  	    }
  	  else{
  	    panic("Failed to save the current address space!\n");
  	  }
  	}
      else{
  	panic("Tried to load the null address in CR3!\n");
      }
    }
  else
    {
      panic("Tried to load a null MMU context!\n");
    }
}


