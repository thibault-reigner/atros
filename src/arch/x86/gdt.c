/**
 * \file /arch/x86/gdt.c
 * \brief Contains functions for the GDT (Global Descriptors Table) management
 */
#include <types.h>

#include <x86/gdt.h>

/**
 * \brief The kernel GDT
 */
static struct Gdt_entry kgdt[NBR_GDT_ENTRIES] __attribute__ ((__aligned__ (8)));

/**
 * \brief The value to load in the GDT register.
 *
 * Contains the physical address and the size of the GDT
 */
static struct Gdt_reg gdtr = {
  .limit = NBR_GDT_ENTRIES * sizeof(struct Gdt_entry) - 1,
  .linear_base_addr = (uint32_t)&kgdt
};



/**
 * \fn void set_gdt_entry(uint32_t n, uint32_t base, uint32_t limit, uint8_t flags1, uint8_t flags2)
 * \brief Specifies a new entry in the GDT (which describes a segment).
 *
 * \param n The index of the entry in the GDT. (Must be lower than NBR_GDT_ENTRIES)
 * \param base The base of the segment.
 * \param limit The size of the segment.
 * \param flags1 Flag related to segment type and the privilege level (DPL)
 * \param flags2 Flag related to the operation size (16 or 32 bits) and the granularity.
 *
 */
void set_gdt_entry(uint32_t n, vaddr_t base, uint32_t limit, uint8_t flags1, uint8_t flags2)
{
  if ( n < NBR_GDT_ENTRIES)
    {
      kgdt[n].seg_limit_0_15 = (uint16_t)(limit & 0xFFFF);
      kgdt[n].base_addr_0_15 = (uint16_t)(base & 0xFFFF);
      kgdt[n].base_addr_16_23 = (uint8_t)((base >> 16) & 0xFF);
      kgdt[n].flags1 = flags1;
      kgdt[n].flags2 = flags2 | (uint8_t)((limit >> 16) & 0xFF);
      kgdt[n].base_addr_24_31 = (uint8_t)((base >> 24) & 0xFF);
    }
}

/**
 * \fn void gdt_init(void)
 * \brief Initialise the kernel GDT
 * 
 * Defines the code and data segments for user and supervisor.
 */
void gdt_init(void)
{
  //The first segment descriptor must not be used (cf. Intel documentation)
  set_gdt_entry(0,0,0,0,0);
      
  //Kernel/user code and data segments
  //We use a flat-memory model
  set_gdt_entry(GDT_KERNEL_CS_INDEX,
		0,0xFFFFF, 
		GDT_CODE_SEGMENT + GDT_CODE_XR + GDT_PRESENT + GDT_DPL0,
		GDT_32B + GDT_GRANULARITY_4KB
		);
  set_gdt_entry(GDT_KERNEL_DS_INDEX,
		0,0xFFFFF,
		GDT_DATA_SEGMENT + GDT_DATA_RW + GDT_PRESENT + GDT_DPL0,
		GDT_32B + GDT_GRANULARITY_4KB
		);
  set_gdt_entry(GDT_USER_CS_INDEX,
		0,0xFFFFF,
		GDT_CODE_SEGMENT + GDT_CODE_XR + GDT_PRESENT + GDT_DPL3,
		GDT_32B + GDT_GRANULARITY_4KB
		);
  set_gdt_entry(GDT_USER_DS_INDEX,
		0,0xFFFFF,
		GDT_DATA_SEGMENT + GDT_DATA_RW + GDT_PRESENT + GDT_DPL3,
		GDT_32B + GDT_GRANULARITY_4KB
		);
  gdt_flush(&gdtr);
}
