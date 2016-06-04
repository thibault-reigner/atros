#include <types.h>

#include <kernel/list.h>
#include <kernel/kprintf.h>
#include <kernel/panic.h>
#include <kernel/mm/slab.h>
#include <kernel/mm/virtual_pages.h>


/*************************************************
               Private data
*************************************************/

static vpn_t _boot_first_free_vpn; 
static vpn_t _boot_last_free_vpn;

//static vpn_t first_free_vpn; 
//static vpn_t last_free_vpn;


static Vregion *free_vregions = NULL;
static Vregion *used_vregions = NULL;

static Objs_cache *cache_Vregion;


static inline int32_t vregion_cmp(const Vregion *vregion1, const Vregion *vregion2)
{
  KASSERT(vregion1 != NULL);
  KASSERT(vregion2 != NULL);
  KASSERT((vregion_first_vpn(vregion1) <= vregion_first_vpn(vregion2) &&
	   vregion_last_vpn(vregion1) <= vregion_last_vpn(vregion2)) ||
	  (vregion_first_vpn(vregion2) <= vregion_first_vpn(vregion1) &&
	   vregion_last_vpn(vregion2) <= vregion_last_vpn(vregion1)));
  
  return (vregion_first_vpn(vregion2) == vregion_first_vpn(vregion1) ? 0 :
	  (vregion_first_vpn(vregion2) > vregion_first_vpn(vregion1) ? 1 : -1));
}

inline vaddr_t vpage_vaddr_of(vaddr_t vaddr)
{
  return (vaddr & (vaddr_t)~((1UL << VPAGE_SHIFT) - 1));
}

inline vaddr_t vpn_to_vaddr(vpn_t vpn)
{
  return (vpn << VPAGE_SHIFT);
}

inline vpn_t vaddr_to_vpn(vaddr_t vaddr)
{
  return (vaddr >> VPAGE_SHIFT);
}

inline size_t vregion_size(const Vregion *vregion)
{
  return (vregion->nbr_pages * VPAGE_SIZE);
}

inline vpn_t vregion_first_vpn(const Vregion *vregion)
{
  return vregion->first_vpn;
}

inline vpn_t vregion_last_vpn(const Vregion *vregion)
{
  return (vregion->first_vpn + vregion->nbr_pages - 1);
}



/*****************************************
          Private functions
******************************************/


/* Try to split the given Vregion in two :
 *  - the resulting lower part (vregion_to_split) will contain nbr_pages
 *  - the resulting upper part will contain vregion_to_split->nbr_pages - nbr_pages pages
 *
 * NB : this function assumes cache_Vregion has at least one free object.
 */
static  Vregion *vregion_split(Vregion *vregion_to_split, size_t nbr_pages)
{
  KASSERT(vregion_to_split != NULL);
  KASSERT(vregion_to_split->nbr_pages > nbr_pages);

  Vregion *vregion_upper_part = _retrieve_free_obj_from_objs_cache(cache_Vregion);

  if (vregion_upper_part == NULL)
    panic("retrieve_free_obj_from_objs_cache() failed to allocate a new Vregion in %s()\n", __func__);
  
  vregion_upper_part->first_vpn = vregion_first_vpn(vregion_to_split) + nbr_pages;
  vregion_upper_part->nbr_pages = vregion_to_split->nbr_pages - nbr_pages;

  vregion_to_split->nbr_pages = nbr_pages;

  return vregion_upper_part;
}


static Vregion *vregion_merge(Vregion *vregion1, Vregion *vregion2)
{
  Vregion *to_return = NULL;
  
  KASSERT(vregion1 != NULL);
  KASSERT(vregion2 != NULL);

  if (vregion_first_vpn(vregion1) > vregion_first_vpn(vregion2))
    {
      Vregion *tmp_vregion = vregion1;
      vregion1 = vregion2;
      vregion2 = tmp_vregion;
    }

  if ((vregion_last_vpn(vregion1) + 1) == vregion_first_vpn(vregion2))
    {
      vregion1->nbr_pages += vregion2->nbr_pages;
      objs_cache_free(cache_Vregion, vregion2);
      
      to_return = vregion1;
    }

  return to_return;
}


/*****************************************
          Public functions
******************************************/

void _boot_virtual_pages_init(vpn_t first_free_vpage, vpn_t last_free_vpage)
{
  _boot_first_free_vpn = first_free_vpage;
  _boot_last_free_vpn  = last_free_vpage;
} 

vpn_t _boot_virtual_pages_alloc(uint32_t nbr_vpages)
{
  vpn_t to_return = 0;
  
  if (_boot_first_free_vpn + nbr_vpages - 1 <= _boot_last_free_vpn)
    {
      to_return = _boot_first_free_vpn;
      _boot_first_free_vpn += nbr_vpages;
    }
  else
    {
      panic("Can't allocate %u virtual page(s) in %s!\n", nbr_vpages,  __func__);
    }

  return to_return;
}


void virtual_page_boot_init(Objs_cache *cache, Vregion *first_used_vregion)
{
  KASSERT(cache != NULL);
  KASSERT(first_used_vregion != NULL);
  KASSERT(cache->free_objs_count > MIN_FREE_OBJS_CACHE_VREGION);
  
  cache_Vregion = cache;
  used_vregions = first_used_vregion;

  Vregion *current = first_used_vregion;
  Vregion *next    = NULL;

  //We check if the initial list of used vregions is properly orderd
  while (current != NULL)
    {
      next = current->next;
      if (next != NULL)
	{
	  if (vregion_first_vpn(current) > vregion_first_vpn(next))
	    {
	      panic("Used vregions are unproperly ordered in %s!\n", __func__);
	    }
	  else if (vregion_last_vpn(current) >= vregion_first_vpn(next))
	    {
	      panic("A virtual region is overlapping an other one in %s!\n", __func__);
	    }
	    
	}
      current = next;
    }
  
  free_vregions = objs_cache_alloc(cache_Vregion);

  if (free_vregions == NULL)
    panic("Failed to allocate a Vregion in %s()\n", __func__);

  vregion_init(free_vregions, _boot_first_free_vpn, _boot_last_free_vpn - _boot_first_free_vpn + 1);
}

void vregion_init(Vregion *region, vpn_t first_vpn, uint32_t nbr_pages)
{
  KASSERT(region != NULL);
  
  if (region != NULL)
    {
      region->first_vpn = first_vpn;
      region->nbr_pages = nbr_pages;
    }
}

Vregion *vregion_alloc(uint32_t nbr_pages)
{
  Vregion *allocated_vregion = NULL;

  KASSERT(cache_Vregion->free_objs_count >= MIN_FREE_OBJS_CACHE_VREGION);
  
  if (cache_Vregion->free_objs_count == MIN_FREE_OBJS_CACHE_VREGION)
    {
      _create_slab_for_cache_Vregion(); 
    }
  
  allocated_vregion = _vregion_alloc(nbr_pages);
  
  return allocated_vregion;
}

Vregion *_vregion_alloc(uint32_t nbr_pages)
{
  Vregion *prev = NULL;
  Vregion *current = free_vregions;

  KASSERT(current != NULL);
  //We look for the first region which has at least the required number of pages
  while (current != NULL)
    {
      if (current->nbr_pages >= nbr_pages)
	break;

      prev = current;
      current = current->next;
    }

  //If the virtual region found is too big we try to split it
  if (current != NULL)
    {
      list_delete_el(free_vregions, prev, current);
      
      if (current->nbr_pages > nbr_pages)
	{
	  Vregion *upper_part = NULL;

	  upper_part = vregion_split(current, nbr_pages);
	  KASSERT(upper_part != NULL);
	  list_insert_after(free_vregions, prev, upper_part);
	}

      list_insert_ordered(used_vregions, current, vregion_cmp);
    }

  return current;
}


void DEBUG_dump_free_vregions(void)
{
  Vregion *current = free_vregions;

  kprintf("%s()\n",__func__);
  kprintf("  Free virtual regions :\n  ");
  
  while (current != NULL)
    {
      kprintf("[%u , %u] - ", vregion_first_vpn(current), vregion_last_vpn(current));
      current = current->next;
    }
  kprintf("\n");
}

void DEBUG_dump_used_vregions(void)
{
  Vregion *current = used_vregions;

  kprintf("%s()\n",__func__);
  kprintf("  Used virtual regions :\n  ");
  
  while (current != NULL)
    {
      kprintf("[%u , %u] - ", vregion_first_vpn(current), vregion_last_vpn(current));
      current = current->next;
    }
  kprintf("\n");
}

