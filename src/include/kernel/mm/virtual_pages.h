#ifndef KERNEL_VIRTUAL_PAGES_H
#define KERNEL_VIRTUAL_PAGES_H

#include <types.h>
#include <kernel/kernel.h>


#define VPAGES_PER_SLAB_CACHE_VREGION 1 //must be at least 1

#define MIN_FREE_OBJS_CACHE_VREGION 2

#define VPAGE_SHIFT 12UL
#define VPAGE_SIZE (1UL << VPAGE_SHIFT)
#define VPAGE_MASK (VPAGE_SIZE - 1)

#ifndef __ASM__

#ifdef __ARCH_x86__
typedef uint32_t vpn_t;
#define NULL_VPN MAX_UINT32

#else

#ifdef __ARCH_x86_64__
typedef uint64_t vpn_t;
#define NULL_VPN MAX_UINT64
#endif

#endif


//Forward declaration (from kernel/mm/slab.h)
struct Objs_cache;
typedef struct Objs_cache Objs_cache;



typedef struct Virtual_region{
  vpn_t first_vpn;
  size_t nbr_pages;

  struct Virtual_region *next;
}Vregion;

vaddr_t vpage_vaddr_of(vaddr_t vaddr);
vaddr_t vpn_to_vaddr(vpn_t vpn);
vpn_t vaddr_to_vpn(vaddr_t vaddr);

size_t vregion_size(const Vregion *vregion);
vpn_t vregion_first_vpn(const Vregion *vregion);
vpn_t vregion_last_vpn(const Vregion *vregion);


void vregion_init(Vregion *region, vpn_t first_vpn, uint32_t nbr_pages);

Vregion *vregion_alloc(uint32_t nbr_pages);
Vregion *_vregion_alloc(uint32_t nbr_pages);

void _boot_virtual_pages_init(vpn_t first_free_vpage, vpn_t last_free_vpage);
vpn_t _boot_virtual_pages_alloc(uint32_t nbr_vpages);


void virtual_page_boot_init(Objs_cache *cache, Vregion *first_used_vregion);

void DEBUG_dump_free_vregions(void);
void DEBUG_dump_used_vregions(void);

#endif

#endif
