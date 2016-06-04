#ifndef KERNEL_MM_SLAB_H
#define KERNEL_MM_SLAB_H

#include <types.h>

#include <kernel/mm/virtual_pages.h>

#define VPAGES_PER_SLAB_CACHE_OBJS_CACHE 1
#define VPAGES_PER_SLAB_CACHE_SLAB 1

#define MIN_FREE_OBJS_CACHE_SLAB 2 //must be at least 2

#define CACHE_NAME_MAX_LENGTH 20

#define SLAB_STATUS_FREE 0
#define SLAB_STATUS_PARTIAL 1
#define SLAB_STATUS_FULL 2

#ifndef __ASM__


/*The following structure is tricky.
  The actual size of an object in a slab is at
  least the size of a pointer.
  Hence when the object is free, we can use the first bytes
  to keep a pointer to the next free object.
  When the object is used, the pointer next is meaningless since
  its bytes are used to store some data.
*/
typedef struct Slab_object{
  struct Slab_object *next;
} Slab_object;


typedef struct Slab{
  uint32_t free_objs_count;
  Vregion *vregion;
  Slab_object *free_objs_list;

  struct Slab *prev, *next;
} Slab;


typedef struct Objs_cache{
  char name[CACHE_NAME_MAX_LENGTH + 1];
  size_t obj_size;
  size_t actual_obj_size;

  void (*constructor)(void *, size_t);
  void (*destructor)(void *, size_t);
  
  uint32_t pages_per_slab;
  size_t slab_size;
  uint32_t objs_per_slab;
  size_t wasted_memory_per_slab;

  uint32_t free_objs_count;
  uint32_t used_objs_count;
  
  uint32_t slabs_count;
  uint32_t free_slabs_count;
  uint32_t partial_slabs_count;
  uint32_t full_slabs_count;
  
  Slab *free_slabs;
  Slab *partial_slabs;
  Slab *full_slabs;
  
  struct Objs_cache *prev;
  struct Objs_cache *next;
} Objs_cache;


void objs_cache_boot_init(void);
Objs_cache *objs_cache_init(Objs_cache *cache,
			    const char *name,
			    size_t obj_size,
			    uint32_t pages_per_slab);
Objs_cache *objs_cache_create(const char *name,
			      size_t obj_size,
			      uint32_t pages_per_slab);
void *objs_cache_alloc(Objs_cache *cache);
void objs_cache_free(Objs_cache *cache, void *obj);

void *_retrieve_free_obj_from_objs_cache(Objs_cache *cache);
void _create_slab_for_cache_Vregion(void);

void DEBUG_dump_objs_cache(Objs_cache *cache);

#endif //__ASM__

#endif
