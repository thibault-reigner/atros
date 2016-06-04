#include <types.h>
#include <string.h>
#include <math.h>

#include <kernel/panic.h>
#include <kernel/kprintf.h>
#include <kernel/list.h>

#include <kernel/mm/slab.h>
#include <kernel/mm/virtual_pages.h>
#include <kernel/mm/physical_pages.h>

#include <x86/paging.h>

static Objs_cache *caches_clist = NULL;

static Objs_cache *cache_Objs_cache;
static Objs_cache *cache_Slab;

static Objs_cache *cache_Vregion;

static inline uint32_t is_vaddr_in_slab(const Slab *slab, vaddr_t vaddr);
static inline uint32_t slab_free_objs_count(const Slab *slab);
static inline int32_t is_slab_full(const Slab *slab);
static inline int32_t is_slab_empty(const Slab *slab, size_t max_objs);
static Slab * initialize_slab(Slab *slab,
			      Vregion *slab_vregion,
			      size_t actual_obj_size,
			      uint32_t nbr_obj_already_used);
static void _create_slab_for_cache_Slab(void);

static inline uint32_t is_vaddr_in_slab(const Slab *slab, vaddr_t vaddr)
{
  KASSERT(slab != NULL);
  KASSERT(slab->vregion != NULL);
  return (vregion_first_vpn(slab->vregion) <= vaddr_to_vpn(vaddr)
	  && vaddr_to_vpn(vaddr) <= vregion_last_vpn(slab->vregion));
}

static inline uint32_t slab_free_objs_count(const Slab *slab)
{
  KASSERT(slab != NULL);
  return slab->free_objs_count;
}

static inline int32_t is_slab_full(const Slab *slab)
{
  KASSERT(slab != NULL);
  return (slab_free_objs_count(slab) == 0);
}

static inline int32_t is_slab_empty(const Slab *slab, size_t max_objs)
{
  KASSERT(slab != NULL);
  return (slab_free_objs_count(slab) == max_objs);
}



/**
 * \fn Slab * initialize_slab(Slab *slab,
 *		        Vregion *slab_vregion,
 *		        size_t actual_obj_size)
 * \brief Initialise an existing Slab struct.
 * \param slab Pointer to the Slab structure to initialise.
 * \param slab_vregion The virtual region used by the slab.
 * \param actual_obj_size The size in bytes of an object and its header in the slab.
 * \param nbr_obj_already_used Indicate the number of objects (all at the beginning of the  * slab) which are already in used and should therefore not be initialized as free.
 * Only for slab allocator initialization purpose.
 * \return Pointer to the given slab.
 */
static Slab * initialize_slab(Slab *slab,
			      Vregion *slab_vregion,
			      size_t actual_obj_size,
			      uint32_t nbr_obj_already_used)
{
  KASSERT(slab != NULL);
  KASSERT(slab_vregion != NULL);
  KASSERT(vpn_to_vaddr(vregion_first_vpn(slab_vregion)) != (vaddr_t)NULL); //NULL is a forbidden address

  slab->free_objs_count = vregion_size(slab_vregion) / actual_obj_size - nbr_obj_already_used;
  slab->vregion = slab_vregion;
  slab->free_objs_list = NULL;
  slab->prev = NULL;
  slab->next = NULL;

  if (vregion_size(slab_vregion) >= actual_obj_size)
    {
      //We set up the list of free objects

      vaddr_t slab_vregion_vaddr = vpn_to_vaddr(vregion_first_vpn(slab_vregion));
      Slab_object *current_obj = (Slab_object*)(slab_vregion_vaddr + nbr_obj_already_used * actual_obj_size);
      Slab_object *next_obj = NULL;

      slab->free_objs_list = current_obj;
      
      for (uint32_t i = 1; i < slab->free_objs_count; i++)
	{
	  next_obj = (Slab_object*)((vaddr_t)current_obj + actual_obj_size);

	  current_obj->next = next_obj;
	  current_obj = next_obj;
	}
      //The last object has no next element
      current_obj->next = NULL;

      KASSERT(slab->free_objs_count == list_length(slab->free_objs_list));
    }
  else
    {
      return NULL;
    }
  
  return slab;
}


/**
 * \fn Slab * create_slab(Vpages_block *slab_vpages,
 *		          size_t actual_obj_size)
 * \brief Create and initialise a Slab structure.
 * \param nbr_pages The number of virtual pages used by the slab.
 * \param actual_obj_size The size of an object and its header in the slab.
 * \return Pointer to the created slab, NULL if creation failed.
 */
static Slab * create_slab(uint32_t nbr_pages, size_t actual_obj_size)
{
  Slab *new_slab = objs_cache_alloc(cache_Slab);

  if (new_slab != NULL)
    {
      Vregion *slab_vregion = vregion_alloc(nbr_pages);
      
      if (slab_vregion != NULL)
	{
	  ppn_t slab_ppages = ppages_alloc(nbr_pages);
	  
	  map_pages(slab_ppages,
		    vregion_first_vpn(slab_vregion),
		    nbr_pages,
		    PAGE_PRESENT | PAGE_READ_WRITE | PAGE_SUPERVISOR | PAGE_GLOBAL);
	  
	  initialize_slab(new_slab,
			  slab_vregion,
			  actual_obj_size,
			  0);
	  
	  ppages_set_slab(slab_ppages, nbr_pages, new_slab);
	}
      else
	{
	  objs_cache_free(cache_Slab, new_slab);
	  new_slab = NULL;
#ifdef DEBUG
	  kprintf("Failed to allocated a new virtual region for a new slab in %s\n", __func__);
#endif
	}
    }
  else
    {
#ifdef DEBUG
      kprintf("Failed to allocate a new Slab object in %s\n", __func__);
#endif
    }

  return new_slab;
}


/**
 * \fn void * alloc_obj_from_slab(Slab *slab)
 * \brief Allocate an object from a slab.
 * \param slab The slab from where to allocate an object.
 * \return Pointer to the allocated object, NULL if allocation failed.
 */
static void * alloc_obj_from_slab(Slab *slab)
{

  void *allocated_obj = NULL;

  KASSERT(slab != NULL);

  if (slab->free_objs_count > 0)
    {
      //kprintf("slab->free_objs_list %p  slab->free_objs_list->next : %p\n",slab->free_objs_list );
      allocated_obj = list_pop_head(slab->free_objs_list);
      KASSERT(allocated_obj != NULL);
      slab->free_objs_count--;
    }
  else
    {
#ifdef DEBUG
      kprintf("\nTried to allocate an object from an empty slab in %s\n", __func__);
#endif
    }
  return allocated_obj;
}


/**
 * \fn bool_t free_obj_from_slab(Slab *slab, void *obj)
 * \brief Free an object allocated in the given slab.
 * \param slab The slab to which belongs the object to free.
 * \param obj The object to free.
 * \return TRUE is the allocated object was freed, FALSE otherwise
 */
static bool_t free_obj_from_slab(Slab *slab, void *obj)
{
  KASSERT(slab != NULL);
  KASSERT(obj != NULL);

  bool_t to_return = FALSE;

  //Does this obj belong to the given slab ?
  if (is_vaddr_in_slab(slab, (vaddr_t)obj))
    {
      slab->free_objs_count++;
      list_push_head(slab->free_objs_list, (Slab_object*)obj);

      to_return = TRUE;
    }
  
  return to_return;
}







/*Add a slab to a given cache*/
static void objs_cache_add_slab(Objs_cache *cache, Slab *slab, uint32_t slab_status)
{
  KASSERT(cache != NULL);
  KASSERT(slab != NULL);
  KASSERT(slab_status != SLAB_STATUS_FREE || cache->objs_per_slab == slab->free_objs_count);
  
  switch (slab_status)
    {
    case SLAB_STATUS_FREE :
      cache->free_objs_count += cache->objs_per_slab;
      cache->free_slabs_count++;
      dlist_push_head_generic(cache->free_slabs, slab, prev, next);
      break;
    case SLAB_STATUS_PARTIAL :
      cache->free_objs_count += slab->free_objs_count;
      cache->used_objs_count += cache->objs_per_slab - slab->free_objs_count;
      cache->partial_slabs_count++;
      dlist_push_head_generic(cache->partial_slabs, slab, prev, next);
      break;
    case SLAB_STATUS_FULL :
      cache->used_objs_count += cache->objs_per_slab;
      cache->full_slabs_count++;
      dlist_push_head_generic(cache->full_slabs, slab, prev, next);
      break;
    default:
      panic("Incorrect slab status given as paramter in %s\n!",__func__);
    }

  cache->slabs_count++;
}


/**********************************************************
                   Public functions
**********************************************************/

void objs_cache_boot_init(void)
{
  /* The initialization of the slab-based object allocator of the kernel is tricky.
   * Since the allocation of virtual pages relies on the the slab allocator (Vregion objects)
   * and the slab allocator relies on the virtual pages allocator (to create new slabs), we have
   * to initialize simultaneously both allocators.
   *
   * This is done in 4 steps :
   * 1) allocate the virtual pages (using the boot virtual pages allocator) 
   *    of the first slabs for each of the following objects caches :
   *     - the cache Objs_cache						\
   *	                        > both used by the slab allocator system
   *	- the cache Slab_cache /
   *	- the cache Vregion_cache > used by the virtual pages allocator
   * 2) allocate the physical pages used by the 3 slabs
   * 3) map the virtual pages to the physical pages
   * 4) initialize in each slabs the required objects which described the 3 caches, their 
   *    slabs and the virtual pages (Vregion) allocated in phase 2.
   */
  
  //Step 1:
  vpn_t cache_Objs_cache_slab_vpn = _boot_virtual_pages_alloc(VPAGES_PER_SLAB_CACHE_OBJS_CACHE);
  vpn_t cache_Slab_slab_vpn       = _boot_virtual_pages_alloc(VPAGES_PER_SLAB_CACHE_SLAB);
  vpn_t cache_Vregion_slab_vpn    = _boot_virtual_pages_alloc(VPAGES_PER_SLAB_CACHE_VREGION);

  //Steps 2
  ppn_t ppages_for_Objs_cache_slab = ppages_alloc(VPAGES_PER_SLAB_CACHE_OBJS_CACHE);
  ppn_t ppages_for_Slab_slab       = ppages_alloc(VPAGES_PER_SLAB_CACHE_SLAB);
  ppn_t ppages_for_Vregion_slab    = ppages_alloc(VPAGES_PER_SLAB_CACHE_VREGION);
  
  //Steps 3
  map_pages(ppages_for_Objs_cache_slab,
	    cache_Objs_cache_slab_vpn,
	    VPAGES_PER_SLAB_CACHE_OBJS_CACHE,
	    PAGE_PRESENT | PAGE_READ_WRITE | PAGE_SUPERVISOR | PAGE_GLOBAL);
  map_pages(ppages_for_Slab_slab,
	    cache_Slab_slab_vpn,
	    VPAGES_PER_SLAB_CACHE_SLAB,
	    PAGE_PRESENT | PAGE_READ_WRITE | PAGE_SUPERVISOR | PAGE_GLOBAL);
  map_pages(ppages_for_Vregion_slab,
	    cache_Vregion_slab_vpn,
	    VPAGES_PER_SLAB_CACHE_VREGION,
	    PAGE_PRESENT | PAGE_READ_WRITE | PAGE_SUPERVISOR | PAGE_GLOBAL);

  Objs_cache *a_Objs_cache = (Objs_cache*)vpn_to_vaddr(cache_Objs_cache_slab_vpn);
  Slab *a_Slab             = (Slab*)vpn_to_vaddr(cache_Slab_slab_vpn);
  Vregion *a_Vregion       = (Vregion*)vpn_to_vaddr(cache_Vregion_slab_vpn);

  //Step 4
  
  //We initialize the 3 Vregion objects which describe the allocated virtual regions for each slabs
  vregion_init(a_Vregion, cache_Objs_cache_slab_vpn, VPAGES_PER_SLAB_CACHE_OBJS_CACHE);
  vregion_init(a_Vregion + 1, cache_Slab_slab_vpn, VPAGES_PER_SLAB_CACHE_SLAB);
  vregion_init(a_Vregion + 2, cache_Vregion_slab_vpn, VPAGES_PER_SLAB_CACHE_VREGION);

  a_Vregion->next = (a_Vregion + 1);
  (a_Vregion + 1)->next = (a_Vregion + 2);
  (a_Vregion + 2)->next = NULL;
  
  //We initialize the 3 Slab objects.
  initialize_slab(a_Slab, a_Vregion, MAX(sizeof(Objs_cache), sizeof(void*)), 3);
  initialize_slab(a_Slab + 1, a_Vregion + 1, MAX(sizeof(Slab), sizeof(void*)), 3);
  initialize_slab(a_Slab + 2, a_Vregion + 2, MAX(sizeof(Vregion), sizeof(void*)), 3);

  //We link the physical pages allocated to the appropriate slab
  ppages_set_slab(ppages_for_Objs_cache_slab, VPAGES_PER_SLAB_CACHE_OBJS_CACHE, a_Slab);
  ppages_set_slab(ppages_for_Slab_slab, VPAGES_PER_SLAB_CACHE_SLAB, a_Slab + 1);
  ppages_set_slab(ppages_for_Vregion_slab, VPAGES_PER_SLAB_CACHE_VREGION, a_Slab + 2);
		  
  //We initialize the 3 Objs_cache objects
  objs_cache_init(a_Objs_cache, "Objs_cache", sizeof(Objs_cache), VPAGES_PER_SLAB_CACHE_OBJS_CACHE);
  objs_cache_init(a_Objs_cache + 1, "Slab", sizeof(Slab),  VPAGES_PER_SLAB_CACHE_SLAB);
  objs_cache_init(a_Objs_cache + 2, "Vregion", sizeof(Vregion), VPAGES_PER_SLAB_CACHE_VREGION);

  //We initialiaze the pointers to the 3 caches
  cache_Objs_cache = a_Objs_cache;
  cache_Slab       = a_Objs_cache + 1;
  cache_Vregion    = a_Objs_cache + 2;

  //We add each slab created to the appropriate cache
  objs_cache_add_slab(cache_Objs_cache, a_Slab, SLAB_STATUS_PARTIAL);
  objs_cache_add_slab(cache_Slab, a_Slab + 1, SLAB_STATUS_PARTIAL);
  objs_cache_add_slab(cache_Vregion, a_Slab + 2, SLAB_STATUS_PARTIAL);

  KASSERT(cache_Slab->free_objs_count > MIN_FREE_OBJS_CACHE_SLAB);
    
  //We initialize the virtual pages allocator
  virtual_page_boot_init(cache_Vregion, a_Vregion);
}


/**
 * \fn Objs_cache * objs_cache_init(Objs_cache *cache,
 *			            const char *name,
 *			            size_t obj_size,
 *			            uint32_t pages_per_slab)
 * \brief Initialise a given Objs_cache structure.
 * \param cache The cache to initialise.
 * \param name The name of the cache.
 * \param obj_size The size in bytes of an object in the cache.
 * \param pages_per_slab Number of virtual pages occupied by a slab.
 * \return Pointer to cache.
 */
Objs_cache * objs_cache_init(Objs_cache *cache,
			     const char *name,
			     size_t obj_size,
			     uint32_t pages_per_slab)
{
  KASSERT(cache != NULL);
  KASSERT(obj_size > 0);
  KASSERT(pages_per_slab > 0);
  KASSERT(obj_size <= (pages_per_slab * VPAGE_SIZE));
 
  if (name != NULL)
    {
      strncpy(cache->name, name, CACHE_NAME_MAX_LENGTH);
      cache->name[CACHE_NAME_MAX_LENGTH] = '\0';
    }
  else
   {
     cache->name[0] = '\0';
   }

  cache->obj_size        = obj_size;
  cache->actual_obj_size = MAX(obj_size,sizeof(void*));
 
  cache->pages_per_slab  = pages_per_slab;
  cache->slab_size       = pages_per_slab * VPAGE_SIZE;
  cache->objs_per_slab   = pages_per_slab * VPAGE_SIZE / cache->actual_obj_size;
  cache->wasted_memory_per_slab = cache->slab_size - cache->objs_per_slab * cache->actual_obj_size;
  
  cache->free_objs_count = 0;
  cache->used_objs_count = 0;

  cache->slabs_count         = 0;
  cache->free_slabs_count    = 0;
  cache->partial_slabs_count = 0;
  cache->full_slabs_count    = 0;

  cache->free_slabs    = NULL;
  cache->partial_slabs = NULL;
  cache->full_slabs    = NULL;
  
  cache->prev     = NULL;
  cache->next     = NULL;

  clist_push_tail(caches_clist, cache);
  
  return cache;
}


/**
 * \fn Ojs_cache *objs_cache_create(const char *name,
 *			             size_t obj_size,
 *			             uint32_t pages_per_slab)
 * \brief Create and initialise a new Objs_cache structure.
 * \param name Name of the cache to create.
 * \param obj_size Size in byte of an object in the cache.
 * \param pages_per_slab Number of virtual pages occupied by a slab.
 * \param min_free_objs The number of free objects the cache must have at any time.
 * \return Pointer to the created cache, NULL if created failed.
 */
Objs_cache *objs_cache_create(const char *name,
			       size_t obj_size,
			       uint32_t pages_per_slab)
{
  Objs_cache *new_cache = NULL;

  //Can a slab contain at least one object ?
  if (MAX(obj_size,sizeof(void*))  <= pages_per_slab * VPAGE_SIZE)
    {
      new_cache = objs_cache_alloc(cache_Objs_cache);

      if (new_cache != NULL)
	{
	  objs_cache_init(new_cache,
			  name,
			  obj_size,
			  pages_per_slab);
	}
      else
	{
#ifdef DEBUG
	  kprintf("Can't allocate a Objs_cache structure for cache %s in %s\n", name, __func__);
#endif
	}
    }
  
  return new_cache;
}



void *objs_cache_alloc(Objs_cache *cache)
{
  void *allocated_obj = NULL;
  KASSERT(cache != NULL);
  KASSERT(cache_Slab->free_objs_count >= MIN_FREE_OBJS_CACHE_SLAB);


  if (cache_Slab->free_objs_count == MIN_FREE_OBJS_CACHE_SLAB)
    {
      _create_slab_for_cache_Slab();
    }

  if (cache_Vregion->free_objs_count == MIN_FREE_OBJS_CACHE_VREGION)
    {
      _create_slab_for_cache_Vregion();
    }
  
  if (cache->free_objs_count == 0)
    {
      Slab *new_slab = create_slab(cache->pages_per_slab, cache->actual_obj_size);

      if (new_slab == NULL)
	panic("Can't create a new slab for a cache (%s) in %s\n", cache->name, __func__);

      objs_cache_add_slab(cache, new_slab, SLAB_STATUS_FREE);
    }

  allocated_obj = _retrieve_free_obj_from_objs_cache(cache);

  if(allocated_obj == NULL)
    panic("Failed to allocate an object from cache %s in %s()\n", cache->name, __func__);
  
  return allocated_obj;
}

void objs_cache_free(Objs_cache *cache, void *obj)
{
  KASSERT(cache != NULL);
  KASSERT(obj != NULL);

  panic("objs_cache_free() not implemented yet!\n");
}


/**
 * Try to retrieve a free object from a cache, panic if the cache has no free object.
 * Only used by the slab allocator.
 * For a normal allocation of object use objs_cache_alloc().
 */
void *_retrieve_free_obj_from_objs_cache(Objs_cache *cache)
{
  KASSERT(cache != NULL);

  void *allocated_obj = NULL;


  if ( !dlist_is_empty(cache->partial_slabs))
    {
      //we try to allocate a new object from a partially used slab
      Slab *slab = cache->partial_slabs;
      KASSERT(slab != NULL);
      
      allocated_obj = alloc_obj_from_slab(slab);

      if (allocated_obj == NULL)
	panic("alloc_obj_from_slab() failed to allocate an object in %s()!\n", __func__);

      cache->free_objs_count--;
      cache->used_objs_count++;

      if (is_slab_full(slab))
	{
	  //the slab is now full
	  dlist_delete_head(cache->partial_slabs, slab);
	  dlist_push_head(cache->full_slabs, slab);
	  
	  cache->partial_slabs_count--;
	  cache->full_slabs_count++;
	}
    }
  else if ( !dlist_is_empty(cache->free_slabs))
    {
      //we try to allocate a new object from a free slab
      
      Slab *slab = cache->free_slabs;

      allocated_obj = alloc_obj_from_slab(slab);

      if (allocated_obj == NULL)
	panic("alloc_obj_from_slab() failed to allocate an object in %s()!\n", __func__);
	      
      cache->free_objs_count--;
      cache->used_objs_count++;

      if (!is_slab_full(slab))
	{
	  //the slab is at least partially used but not full
	  dlist_delete_head(cache->free_slabs, slab);
	  dlist_push_head(cache->partial_slabs, slab);

	  cache->free_slabs_count--;
	  cache->partial_slabs_count++;
	}
      else
	{
	  //NB : this case only occurs when a slab can contain only one object

	  dlist_delete_head(cache->free_slabs, slab);
	  dlist_push_head(cache->full_slabs, slab);

	  cache->free_slabs_count--;
	  cache->full_slabs_count++;
	}
    }
  else
    {
#ifdef DEBUG
      kprintf("No free object in the cache %s in %s()\n", cache->name, __func__);
#endif
    }

  return allocated_obj;
}

/* This function adds a new slab to the object cache for Vregion objects without
 * relying on obj_cache_alloc() and create_slab()
 */
void _create_slab_for_cache_Vregion(void)
{
  KASSERT(cache_Vregion->free_objs_count >= (MIN_FREE_OBJS_CACHE_VREGION - 1));
  KASSERT(cache_Vregion->pages_per_slab == VPAGES_PER_SLAB_CACHE_VREGION);
  
  Vregion *a_vregion = _vregion_alloc(VPAGES_PER_SLAB_CACHE_VREGION);
  Slab *a_slab       = _retrieve_free_obj_from_objs_cache(cache_Slab);

  KASSERT(a_vregion != NULL);
  KASSERT(a_slab != NULL);
  
  ppn_t slab_ppages = ppages_alloc(VPAGES_PER_SLAB_CACHE_VREGION);
  map_pages(slab_ppages,
	    vregion_first_vpn(a_vregion),
	    VPAGES_PER_SLAB_CACHE_VREGION,
	    PAGE_PRESENT | PAGE_READ_WRITE | PAGE_SUPERVISOR | PAGE_GLOBAL);
  
  kprintf("TODO : reference the slab in the Ppage structures in %s()\n", __func__);
  
  initialize_slab(a_slab, a_vregion, cache_Vregion->actual_obj_size, 0);

  objs_cache_add_slab(cache_Vregion, a_slab, SLAB_STATUS_FREE);

  if (cache_Slab->free_objs_count == MIN_FREE_OBJS_CACHE_SLAB - 1)
    _create_slab_for_cache_Slab();
}

/* This function adds a new slab to the object cache for Slab objects without
 * relying on obj_cache_alloc() and create_slab()
 */
static void _create_slab_for_cache_Slab(void)
{
  KASSERT(cache_Slab->free_objs_count >= (MIN_FREE_OBJS_CACHE_SLAB - 1));
  KASSERT(cache_Slab->pages_per_slab == VPAGES_PER_SLAB_CACHE_SLAB);
  
  Vregion *a_vregion = _vregion_alloc(VPAGES_PER_SLAB_CACHE_VREGION);
  Slab *a_slab       = _retrieve_free_obj_from_objs_cache(cache_Slab);

  KASSERT(a_vregion != NULL);
  KASSERT(a_slab != NULL);
  
  ppn_t slab_ppages = ppages_alloc(VPAGES_PER_SLAB_CACHE_SLAB);
  map_pages(slab_ppages,
	    vregion_first_vpn(a_vregion),
	    VPAGES_PER_SLAB_CACHE_SLAB,
	    PAGE_PRESENT | PAGE_READ_WRITE | PAGE_SUPERVISOR | PAGE_GLOBAL);
  
  kprintf("TODO : reference the slab in the Ppage structures in %s()\n", __func__);
  
  initialize_slab(a_slab, a_vregion, cache_Slab->actual_obj_size, 0);

  objs_cache_add_slab(cache_Slab, a_slab, SLAB_STATUS_FREE);

  if (cache_Vregion->free_objs_count == MIN_FREE_OBJS_CACHE_VREGION - 1)
    _create_slab_for_cache_Vregion();
}

void DEBUG_dump_objs_cache(Objs_cache *cache)
{
  KASSERT(cache != NULL);

  kprintf("%s()\n", __func__);
  kprintf("  cache name : %s\n"\
	  "  obj_size : %u\n"\
	  "  actual_obj_size : %u\n"
	  "  pages_per_slab : %u\n"
	  "  slab_size : %u\n"\
	  "  objs_per_slab : %u\n"\
	  "  wasted_memory_per_slab : %u\n"\
	  "  free_objs_count : %u\n"\
	  "  used_objs_count : %u\n"\
	  "  slabs_count : %u\n"\
	  "  free_slabs_count : %u\n"\
	  "  partial_slabs_count : %u\n"\
	  "  full_slabs_count : %u\n",
	  cache->name,
	  cache->obj_size,
	  cache->actual_obj_size,
	  cache->pages_per_slab,
	  cache->slab_size,
	  cache->objs_per_slab,
	  cache->wasted_memory_per_slab,
	  cache->free_objs_count,
	  cache->used_objs_count,
	  cache->slabs_count,
	  cache->free_slabs_count,
	  cache->partial_slabs_count,
	  cache->full_slabs_count);
}
    
