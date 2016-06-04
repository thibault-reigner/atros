/**
 * \file arch/x86/boot/ multiboot.h 
 * \brief The header for Multiboot 
 *
 * Copyright (C) 1999  Free Software Foundation, Inc.
 *  
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *  
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *  
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA. 
 */

/** \todo Check if multiboot.h is obsolete (GRUB ? GRUB 2 ?)*/
#ifndef __MULTIBOOT_H__
#define __MULTIBOOT_H__

#include <types.h>



/* Macros.  */

/* The magic number for the Multiboot header.  */
#define MULTIBOOT_HEADER_MAGIC		0x1BADB002

/* The flags for the Multiboot header.  */
#define MULTIBOOT_HEADER_FLAG_MEM_INFO       1
#define MULTIBOOT_HEADER_FLAG_BOOT_DEV_INFO  2
#define MULTIBOOT_HEADER_FLAG_CMD_LINE_INFO  4
#define MULTIBOOT_HEADER_FLAG_MODULES_INFO   8
#define MULTIBOOT_HEADER_FLAG_MEM_MAP_INFO   64

#define MULTIBOOT_HEADER_FLAGS		MULTIBOOT_HEADER_FLAG_MEM_INFO      | \
                                        MULTIBOOT_HEADER_FLAG_BOOT_DEV_INFO 

// MULTIBOOT_HEADER_FLAG_MODULES_INFO  


/* The magic number passed by a Multiboot-compliant boot loader.  */
#define MULTIBOOT_BOOTLOADER_MAGIC	0x2BADB002

/* The size of our stack (16KB).  */
#define MULTIBOOT_STACK_SIZE	        0x4000

#define MULTIBOOT_CMDLINE 4
#define MULTIBOOT_MODS 8



#ifndef __ASM__

/* The Multiboot header.  */
struct Multiboot_header
{
  unsigned long magic;
  unsigned long flags;
  unsigned long checksum;
  unsigned long header_addr;
  unsigned long load_addr;
  unsigned long load_end_addr;
  unsigned long bss_end_addr;
  unsigned long entry_addr;
};

/* The symbol table for a.out.  */
struct Aout_symbol_table
{
  unsigned long tabsize;
  unsigned long strsize;
  unsigned long addr;
  unsigned long reserved;
};

/* The section header table for ELF.  */
struct Elf_section_header_table
{
  unsigned long num;
  unsigned long size;
  unsigned long addr;
  unsigned long shndx;
};

/* The Multiboot information.  */
struct Multiboot_info
{
  unsigned long flags;
  unsigned long mem_lower;
  unsigned long mem_upper;
  unsigned long boot_device;
  unsigned long cmdline;
  unsigned long mods_count;
  unsigned long mods_addr;
  union
  {
    struct Aout_symbol_table aout_sym;
    struct Elf_section_header_table elf_sec;
  } u;
  unsigned long mmap_length;
  unsigned long mmap_addr;
  unsigned long drives_length;
  unsigned long drives_addr;
};

/* The module structure.  */
struct Module
{
  unsigned long mod_start;
  unsigned long mod_end;
  unsigned long string;
  unsigned long reserved;
};

/* The memory map. Be careful that the offset 0 is base_addr_low
   but no size.  */
struct Memory_map
{
  unsigned long size;
  unsigned long base_addr_low;
  unsigned long base_addr_high;
  unsigned long length_low;
  unsigned long length_high;
  unsigned long type;
};

#endif


#endif /* __MULTIBOOT_H__ */
