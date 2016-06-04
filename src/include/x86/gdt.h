/**
 * \file include/x86/gdt.h
 * \brief Contains definitions related to segmentation on 
 *        the x86 architecture.
 */
#ifndef x86_GDT_H
#define x86_GDT_H

#include <types.h>

#define NBR_GDT_ENTRIES 16

/*The different segments used by the kernel:
  0 the null segment descriptor
  1 the kernel code segment
  2 the kernel data segment
  3 the user code segment
  4 the user data segment
  5 tss
*/

#define GDT_KERNEL_CS_INDEX 1
#define GDT_KERNEL_DS_INDEX 2
#define GDT_USER_CS_INDEX 3
#define GDT_USER_DS_INDEX 4
#define GDT_TSS_INDEX 5

/*DPLs*/
#define KERNEL_CS (GDT_KERNEL_CS_INDEX << 3)
#define KERNEL_DS (GDT_KERNEL_DS_INDEX << 3)
#define USER_CS ((GDT_USER_CS_INDEX << 3) + 3)
#define USER_DS ((GDT_USER_DS_INDEX << 3) + 3)

/*Used for flags1*/

#define GDT_DATA_SEGMENT ((1 << 4) + 0)        /**< \brief Flag for data segment*/
#define GDT_DATA_RO 0                          /**< \brief Flag for read-only data segment*/
#define GDT_DATA_RW (1 << 1)                   /**< \brief Flag for a read/write access on a data segment*/

#define GDT_CODE_SEGMENT ((1 << 4) + (1 << 3)) /**< \brief Flag for a code segment*/
#define GDT_CODE_XO 0                          /**< \brief Flag for an execute-only code segment*/ 
#define GDT_CODE_XR (1 << 1)                   /**< \brief Flag for an execute/read access on a code segment*/

#define GDT_TSS_SEGMENT 9
#define GDT_TSS_BUSY 2


#define GDT_DPL0 0
#define GDT_DPL1 (1 << 5)
#define GDT_DPL2 (2 << 5)
#define GDT_DPL3 (3 << 5)

#define GDT_PRESENT (1 << 7)

/*Used for flags2*/
#define GDT_32B (1 << 6)
#define GDT_GRANULARITY_BYTE 0
#define GDT_GRANULARITY_4KB (1 << 7)


#ifndef __ASM__

/**
 * \struct Gdt_reg
 * \brief This structure represents the GDT register's content
 */
struct Gdt_reg
{
  uint16_t limit;           /**< \brief The size in bytes of the current GDT*/
  vaddr_t linear_base_addr; /**< \brief The linear address of the current GDT*/
}__attribute__ ((packed));

/**
 * \struct Gdt_entry
 * \brief This structure represent a segment descriptor (a GDT's entry)
 *
 * NB: A segment's limit is a 20 bits long integer, the segment size (in byte)
 * is equal to its limit * its granularity (byte or 4kb-page)
 */
struct Gdt_entry
{
  uint16_t seg_limit_0_15;  /**< \brief The first 16 bits of the segment's limit*/
  uint16_t base_addr_0_15;  /**< \brief The first 16 bits of the segment's address*/
  uint8_t base_addr_16_23;  /**< \brief The 16 to 23 bits of the segment's address*/
  uint8_t flags1;           /**< \brief Flags related to the segment type and access rights*/
  uint8_t flags2;           /**< \brief The last 4 bits of the segment's limit and flags about segment's granularity*/
  uint8_t base_addr_24_31;  /**< \brief The 24 to 31 bits of the segment's address*/
}__attribute__ ((packed));


void gdt_init(void);
void set_gdt_entry(uint32_t n, vaddr_t base, uint32_t limit, uint8_t flags1, uint8_t flags2);
void gdt_flush(struct Gdt_reg*);

#endif //__ASM__

#endif
