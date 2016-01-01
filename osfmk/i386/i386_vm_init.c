/*
 * Copyright (c) 2003-2012 Apple Inc. All rights reserved.
 *
<<<<<<< HEAD
 * @APPLE_OSREFERENCE_LICENSE_HEADER_START@
 * 
 * This file contains Original Code and/or Modifications of Original Code
 * as defined in and that are subject to the Apple Public Source License
 * Version 2.0 (the 'License'). You may not use this file except in
 * compliance with the License. The rights granted to you under the License
 * may not be used to create, or enable the creation or redistribution of,
 * unlawful or unlicensed copies of an Apple operating system, or to
 * circumvent, violate, or enable the circumvention or violation of, any
 * terms of an Apple operating system software license agreement.
 * 
 * Please obtain a copy of the License at
 * http://www.opensource.apple.com/apsl/ and read it before using this file.
 * 
 * The Original Code and all software distributed under the License are
 * distributed on an 'AS IS' basis, WITHOUT WARRANTY OF ANY KIND, EITHER
=======
 * @APPLE_LICENSE_HEADER_START@
 * 
 * The contents of this file constitute Original Code as defined in and
 * are subject to the Apple Public Source License Version 1.1 (the
 * "License").  You may not use this file except in compliance with the
 * License.  Please obtain a copy of the License at
 * http://www.apple.com/publicsource and read it before using this file.
 * 
 * This Original Code and all software distributed under the License are
 * distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY KIND, EITHER
>>>>>>> origin/10.3
 * EXPRESS OR IMPLIED, AND APPLE HEREBY DISCLAIMS ALL SUCH WARRANTIES,
 * INCLUDING WITHOUT LIMITATION, ANY WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE OR NON-INFRINGEMENT.  Please see the
 * License for the specific language governing rights and limitations
 * under the License.
 * 
 * @APPLE_OSREFERENCE_LICENSE_HEADER_END@
 */
/*
 * @OSF_COPYRIGHT@
 */
/* 
 * Mach Operating System
 * Copyright (c) 1991,1990,1989, 1988 Carnegie Mellon University
 * All Rights Reserved.
 * 
 * Permission to use, copy, modify and distribute this software and its
 * documentation is hereby granted, provided that both the copyright
 * notice and this permission notice appear in all copies of the
 * software, derivative works or modified versions, and any portions
 * thereof, and that both notices appear in supporting documentation.
 * 
 * CARNEGIE MELLON ALLOWS FREE USE OF THIS SOFTWARE IN ITS "AS IS"
 * CONDITION.  CARNEGIE MELLON DISCLAIMS ANY LIABILITY OF ANY KIND FOR
 * ANY DAMAGES WHATSOEVER RESULTING FROM THE USE OF THIS SOFTWARE.
 * 
 * Carnegie Mellon requests users of this software to return to
 * 
 *  Software Distribution Coordinator  or  Software.Distribution@CS.CMU.EDU
 *  School of Computer Science
 *  Carnegie Mellon University
 *  Pittsburgh PA 15213-3890
 * 
 * any improvements or extensions that they make and grant Carnegie Mellon
 * the rights to redistribute these changes.
 */


#include <mach/i386/vm_param.h>

#include <string.h>
#include <mach/vm_param.h>
#include <mach/vm_prot.h>
#include <mach/machine.h>
#include <mach/time_value.h>
#include <kern/spl.h>
#include <kern/assert.h>
#include <kern/debug.h>
#include <kern/misc_protos.h>
#include <kern/cpu_data.h>
#include <kern/processor.h>
#include <vm/vm_page.h>
#include <vm/pmap.h>
#include <vm/vm_kern.h>
#include <i386/pmap.h>
#include <i386/misc_protos.h>
#include <i386/cpuid.h>
#include <mach/thread_status.h>
#include <pexpert/i386/efi.h>
#include <i386/i386_lowmem.h>
#include <x86_64/lowglobals.h>
#include <i386/pal_routines.h>

#include <mach-o/loader.h>
#include <libkern/kernel_mach_header.h>


vm_size_t	mem_size = 0; 
pmap_paddr_t	first_avail = 0;/* first after page tables */

uint64_t	max_mem;        /* Size of physical memory (bytes), adjusted by maxmem */
uint64_t        mem_actual;
<<<<<<< HEAD
uint64_t	sane_size = 0;  /* Memory size for defaults calculations */

/*
 * KASLR parameters
 */
ppnum_t		vm_kernel_base_page;
vm_offset_t	vm_kernel_base;
vm_offset_t	vm_kernel_top;
vm_offset_t	vm_kernel_stext;
vm_offset_t	vm_kernel_etext;
vm_offset_t	vm_kernel_slide;
vm_offset_t vm_hib_base;
vm_offset_t	vm_kext_base = VM_MIN_KERNEL_AND_KEXT_ADDRESS;
vm_offset_t	vm_kext_top = VM_MIN_KERNEL_ADDRESS;

vm_offset_t vm_prelink_stext;
vm_offset_t vm_prelink_etext;
vm_offset_t vm_prelink_sinfo;
vm_offset_t vm_prelink_einfo;
vm_offset_t vm_slinkedit;
vm_offset_t vm_elinkedit;

#define MAXLORESERVE	(32 * 1024 * 1024)
=======
uint64_t	sane_size = 0;  /* Memory size to use for defaults calculations */

#define MAXLORESERVE	( 32 * 1024 * 1024)
>>>>>>> origin/10.6

ppnum_t		max_ppnum = 0;
ppnum_t		lowest_lo = 0;
ppnum_t		lowest_hi = 0;
ppnum_t		highest_hi = 0;

<<<<<<< HEAD
<<<<<<< HEAD
enum {PMAP_MAX_RESERVED_RANGES = 32};
uint32_t pmap_reserved_pages_allocated = 0;
uint32_t pmap_reserved_range_indices[PMAP_MAX_RESERVED_RANGES];
uint32_t pmap_last_reserved_range_index = 0;
=======
uint32_t pmap_reserved_pages_allocated = 0;
uint32_t pmap_last_reserved_range = 0xFFFFFFFF;
>>>>>>> origin/10.6
=======
enum {PMAP_MAX_RESERVED_RANGES = 32};
uint32_t pmap_reserved_pages_allocated = 0;
uint32_t pmap_reserved_range_indices[PMAP_MAX_RESERVED_RANGES];
uint32_t pmap_last_reserved_range_index = 0;
>>>>>>> origin/10.7
uint32_t pmap_reserved_ranges = 0;

extern unsigned int bsd_mbuf_cluster_reserve(boolean_t *);

pmap_paddr_t     avail_start, avail_end;
vm_offset_t	virtual_avail, virtual_end;
static pmap_paddr_t	avail_remaining;
vm_offset_t     static_memory_end = 0;

vm_offset_t	sHIB, eHIB, stext, etext, sdata, edata, sconstdata, econstdata, end;

/*
 * _mh_execute_header is the mach_header for the currently executing kernel
 */
<<<<<<< HEAD
vm_offset_t segTEXTB; unsigned long segSizeTEXT;
vm_offset_t segDATAB; unsigned long segSizeDATA;
vm_offset_t segLINKB; unsigned long segSizeLINK;
vm_offset_t segPRELINKB; unsigned long segSizePRELINK;
vm_offset_t segPRELINKINFOB; unsigned long segSizePRELINKINFO;
vm_offset_t segHIBB; unsigned long segSizeHIB;
vm_offset_t sectCONSTB; unsigned long sectSizeConst;

boolean_t doconstro_override = FALSE;

static kernel_segment_command_t *segTEXT, *segDATA;
static kernel_section_t *cursectTEXT, *lastsectTEXT;
static kernel_section_t *sectDCONST;
=======
void *sectTEXTB; unsigned long sectSizeTEXT;
void *sectDATAB; unsigned long sectSizeDATA;
void *sectOBJCB; unsigned long sectSizeOBJC;
void *sectLINKB; unsigned long sectSizeLINK;
void *sectPRELINKB; unsigned long sectSizePRELINK;
void *sectHIBB; unsigned long sectSizeHIB;
void *sectINITPTB; unsigned long sectSizeINITPT;
>>>>>>> origin/10.6

extern uint64_t firmware_Conventional_bytes;
extern uint64_t firmware_RuntimeServices_bytes;
extern uint64_t firmware_ACPIReclaim_bytes;
extern uint64_t firmware_ACPINVS_bytes;
extern uint64_t firmware_PalCode_bytes;
extern uint64_t firmware_Reserved_bytes;
extern uint64_t firmware_Unusable_bytes;
extern uint64_t firmware_other_bytes;
uint64_t firmware_MMIO_bytes;

/*
 * Linker magic to establish the highest address in the kernel.
 */
extern void 	*last_kernel_symbol;

#if	DEBUG
#define	PRINT_PMAP_MEMORY_TABLE
#define DBG(x...)       kprintf(x)
#else
#define DBG(x...)
#endif /* DEBUG */
/*
 * Basic VM initialization.
 */
void
i386_vm_init(uint64_t	maxmem,
	     boolean_t	IA32e,
	     boot_args	*args)
{
	pmap_memory_region_t *pmptr;
        pmap_memory_region_t *prev_pmptr;
	EfiMemoryRange *mptr;
        unsigned int mcount;
        unsigned int msize;
	ppnum_t fap;
	unsigned int i;
	ppnum_t maxpg = 0;
        uint32_t pmap_type;
<<<<<<< HEAD
	uint32_t maxloreserve;
=======
>>>>>>> origin/10.6
	uint32_t maxdmaaddr;
	uint32_t  mbuf_reserve = 0;
	boolean_t mbuf_override = FALSE;
	boolean_t coalescing_permitted;
<<<<<<< HEAD
	vm_kernel_base_page = i386_btop(args->kaddr);
	vm_offset_t base_address;
	vm_offset_t static_base_address;
    
	/*
	 * Establish the KASLR parameters.
	 */
	static_base_address = ml_static_ptovirt(KERNEL_BASE_OFFSET);
	base_address        = ml_static_ptovirt(args->kaddr);
	vm_kernel_slide     = base_address - static_base_address;
	if (args->kslide) {
		kprintf("KASLR slide: 0x%016lx dynamic\n", vm_kernel_slide);
		if (vm_kernel_slide != ((vm_offset_t)args->kslide))
			panic("Kernel base inconsistent with slide - rebased?");
	} else {
		/* No slide relative to on-disk symbols */
		kprintf("KASLR slide: 0x%016lx static and ignored\n",
			vm_kernel_slide);
		vm_kernel_slide = 0;
	}

	/*
	 * Zero out local relocations to avoid confusing kxld.
	 * TODO: might be better to move this code to OSKext::initialize
	 */
	if (_mh_execute_header.flags & MH_PIE) {
		struct load_command *loadcmd;
		uint32_t cmd;

		loadcmd = (struct load_command *)((uintptr_t)&_mh_execute_header +
						  sizeof (_mh_execute_header));

		for (cmd = 0; cmd < _mh_execute_header.ncmds; cmd++) {
			if (loadcmd->cmd == LC_DYSYMTAB) {
				struct dysymtab_command *dysymtab;

				dysymtab = (struct dysymtab_command *)loadcmd;
				dysymtab->nlocrel = 0;
				dysymtab->locreloff = 0;
				kprintf("Hiding local relocations\n");
				break;
			}
			loadcmd = (struct load_command *)((uintptr_t)loadcmd + loadcmd->cmdsize);
		}
	}
=======
#if DEBUG
	kprintf("Boot args revision: %d version: %d",
		args->Revision, args->Version);
	kprintf("  commandline: \"");
	for(i=0; i<BOOT_LINE_LENGTH; i++)
		kprintf("%c", args->CommandLine[i]);
	kprintf("\"\n");
#endif
>>>>>>> origin/10.7

	/*
	 * Now retrieve addresses for end, edata, and etext 
	 * from MACH-O headers.
	 */
	segTEXTB = (vm_offset_t) getsegdatafromheader(&_mh_execute_header,
					"__TEXT", &segSizeTEXT);
	segDATAB = (vm_offset_t) getsegdatafromheader(&_mh_execute_header,
					"__DATA", &segSizeDATA);
	segLINKB = (vm_offset_t) getsegdatafromheader(&_mh_execute_header,
					"__LINKEDIT", &segSizeLINK);
	segHIBB  = (vm_offset_t) getsegdatafromheader(&_mh_execute_header,
					"__HIB", &segSizeHIB);
<<<<<<< HEAD
    segPRELINKB = (vm_offset_t) getsegdatafromheader(&_mh_execute_header,
                                                     "__PRELINK_TEXT", &segSizePRELINK);
    segPRELINKINFOB = (vm_offset_t) getsegdatafromheader(&_mh_execute_header,
                                                     "__PRELINK_INFO", &segSizePRELINKINFO);
=======
	segPRELINKB = (vm_offset_t) getsegdatafromheader(&_mh_execute_header,
					"__PRELINK_TEXT", &segSizePRELINK);
    segPRELINKINFOB = (vm_offset_t) getsegdatafromheader(&_mh_execute_header,
                    "__PRELINK_INFO", &segSizePRELINKINFO);
>>>>>>> origin/10.10
	segTEXT = getsegbynamefromheader(&_mh_execute_header,
					"__TEXT");
	segDATA = getsegbynamefromheader(&_mh_execute_header,
					"__DATA");
	sectDCONST = getsectbynamefromheader(&_mh_execute_header,
					"__DATA", "__const");
	cursectTEXT = lastsectTEXT = firstsect(segTEXT);
	/* Discover the last TEXT section within the TEXT segment */
	while ((cursectTEXT = nextsect(segTEXT, cursectTEXT)) != NULL) {
		lastsectTEXT = cursectTEXT;
	}

	sHIB  = segHIBB;
	eHIB  = segHIBB + segSizeHIB;
	vm_hib_base = sHIB;
	/* Zero-padded from ehib to stext if text is 2M-aligned */
	stext = segTEXTB;
	lowGlo.lgStext = stext;
	etext = (vm_offset_t) round_page_64(lastsectTEXT->addr + lastsectTEXT->size);
	/* Zero-padded from etext to sdata if text is 2M-aligned */
	sdata = segDATAB;
	edata = segDATAB + segSizeDATA;

	sectCONSTB = (vm_offset_t) sectDCONST->addr;
	sectSizeConst = sectDCONST->size;
	sconstdata = sectCONSTB;
	econstdata = sectCONSTB + sectSizeConst;

	if (sectSizeConst & PAGE_MASK) {
		kernel_section_t *ns = nextsect(segDATA, sectDCONST);
		if (ns && !(ns->addr & PAGE_MASK))
			doconstro_override = TRUE;
	} else
		doconstro_override = TRUE;

	DBG("segTEXTB    = %p\n", (void *) segTEXTB);
	DBG("segDATAB    = %p\n", (void *) segDATAB);
	DBG("segLINKB    = %p\n", (void *) segLINKB);
	DBG("segHIBB     = %p\n", (void *) segHIBB);
	DBG("segPRELINKB = %p\n", (void *) segPRELINKB);
<<<<<<< HEAD
	DBG("segPRELINKINFOB = %p\n", (void *) segPRELINKINFOB);
=======
    DBG("segPRELINKINFOB = %p\n", (void *) segPRELINKINFOB);
>>>>>>> origin/10.10
	DBG("sHIB        = %p\n", (void *) sHIB);
	DBG("eHIB        = %p\n", (void *) eHIB);
	DBG("stext       = %p\n", (void *) stext);
	DBG("etext       = %p\n", (void *) etext);
	DBG("sdata       = %p\n", (void *) sdata);
	DBG("edata       = %p\n", (void *) edata);
	DBG("sconstdata  = %p\n", (void *) sconstdata);
	DBG("econstdata  = %p\n", (void *) econstdata);
	DBG("kernel_top  = %p\n", (void *) &last_kernel_symbol);

	vm_kernel_base  = sHIB;
	vm_kernel_top   = (vm_offset_t) &last_kernel_symbol;
	vm_kernel_stext = stext;
	vm_kernel_etext = etext;
    vm_prelink_stext = segPRELINKB;
    vm_prelink_etext = segPRELINKB + segSizePRELINK;
    vm_prelink_sinfo = segPRELINKINFOB;
    vm_prelink_einfo = segPRELINKINFOB + segSizePRELINKINFO;
    vm_slinkedit = segLINKB;
    vm_elinkedit = segLINKB + segSizePRELINK;

    vm_prelink_stext = segPRELINKB;
    vm_prelink_etext = segPRELINKB + segSizePRELINK;
    vm_prelink_sinfo = segPRELINKINFOB;
    vm_prelink_einfo = segPRELINKINFOB + segSizePRELINKINFO;
    vm_slinkedit = segLINKB;
    vm_elinkedit = segLINKB + segSizePRELINK;

	vm_set_page_size();

	/*
	 * Compute the memory size.
	 */

<<<<<<< HEAD
=======
	if ((1 == vm_himemory_mode) || PE_parse_boot_argn("-x", &safeboot, sizeof (safeboot))) {
	        maxpg = 1 << (32 - I386_PGSHIFT);
	}
>>>>>>> origin/10.5
	avail_remaining = 0;
	avail_end = 0;
	pmptr = pmap_memory_regions;
        prev_pmptr = 0;
	pmap_memory_region_count = pmap_memory_region_current = 0;
	fap = (ppnum_t) i386_btop(first_avail);

	mptr = (EfiMemoryRange *)ml_static_ptovirt((vm_offset_t)args->MemoryMap);
        if (args->MemoryMapDescriptorSize == 0)
	        panic("Invalid memory map descriptor size");
        msize = args->MemoryMapDescriptorSize;
        mcount = args->MemoryMapSize / msize;

#define FOURGIG 0x0000000100000000ULL
#define ONEGIG  0x0000000040000000ULL

	for (i = 0; i < mcount; i++, mptr = (EfiMemoryRange *)(((vm_offset_t)mptr) + msize)) {
	        ppnum_t base, top;
		uint64_t region_bytes = 0;

		if (pmap_memory_region_count >= PMAP_MEMORY_REGIONS_SIZE) {
		        kprintf("WARNING: truncating memory region count at %d\n", pmap_memory_region_count);
			break;
		}
		base = (ppnum_t) (mptr->PhysicalStart >> I386_PGSHIFT);
		top = (ppnum_t) (((mptr->PhysicalStart) >> I386_PGSHIFT) + mptr->NumberOfPages - 1);

<<<<<<< HEAD
		if (base == 0) {
			/*
			 * Avoid having to deal with the edge case of the 
			 * very first possible physical page and the roll-over
			 * to -1; just ignore that page.
			 */
			kprintf("WARNING: ignoring first page in [0x%llx:0x%llx]\n", (uint64_t) base, (uint64_t) top);
			base++;
		}
		if (top + 1 == 0) {
			/*
			 * Avoid having to deal with the edge case of the 
			 * very last possible physical page and the roll-over
			 * to 0; just ignore that page.
			 */
			kprintf("WARNING: ignoring last page in [0x%llx:0x%llx]\n", (uint64_t) base, (uint64_t) top);
			top--;
		}
		if (top < base) {
			/*
			 * That was the only page in that region, so
			 * ignore the whole region.
			 */
			continue;
		}

=======
>>>>>>> origin/10.7
#if	MR_RSV_TEST
		static uint32_t nmr = 0;
		if ((base > 0x20000) && (nmr++ < 4))
			mptr->Attribute |= EFI_MEMORY_KERN_RESERVED;
#endif
		region_bytes = (uint64_t)(mptr->NumberOfPages << I386_PGSHIFT);
		pmap_type = mptr->Type;

		switch (mptr->Type) {
		case kEfiLoaderCode:
		case kEfiLoaderData:
		case kEfiBootServicesCode:
		case kEfiBootServicesData:
		case kEfiConventionalMemory:
		        /*
			 * Consolidate usable memory types into one.
			 */
		        pmap_type = kEfiConventionalMemory;
		        sane_size += region_bytes;
			firmware_Conventional_bytes += region_bytes;
			break;
			/*
			 * sane_size should reflect the total amount of physical
			 * RAM in the system, not just the amount that is
			 * available for the OS to use.
			 * We now get this value from SMBIOS tables
			 * rather than reverse engineering the memory map.
			 * But the legacy computation of "sane_size" is kept
			 * for diagnostic information.
			 */

		case kEfiRuntimeServicesCode:
		case kEfiRuntimeServicesData:
			firmware_RuntimeServices_bytes += region_bytes;
			sane_size += region_bytes;
			break;
		case kEfiACPIReclaimMemory:
			firmware_ACPIReclaim_bytes += region_bytes;
			sane_size += region_bytes;
			break;
		case kEfiACPIMemoryNVS:
			firmware_ACPINVS_bytes += region_bytes;
			sane_size += region_bytes;
			break;
		case kEfiPalCode:
			firmware_PalCode_bytes += region_bytes;
		        sane_size += region_bytes;
			break;

		case kEfiReservedMemoryType:
			firmware_Reserved_bytes += region_bytes;
			break;
		case kEfiUnusableMemory:
			firmware_Unusable_bytes += region_bytes;
			break;
		case kEfiMemoryMappedIO:
		case kEfiMemoryMappedIOPortSpace:
			firmware_MMIO_bytes += region_bytes;
			break;
		default:
			firmware_other_bytes += region_bytes;
			break;
		}

		DBG("EFI region %d: type %u/%d, base 0x%x, top 0x%x %s\n",
		    i, mptr->Type, pmap_type, base, top,
		    (mptr->Attribute&EFI_MEMORY_KERN_RESERVED)? "RESERVED" :
		    (mptr->Attribute&EFI_MEMORY_RUNTIME)? "RUNTIME" : "");

		if (maxpg) {
		        if (base >= maxpg)
				break;
		        top = (top > maxpg) ? maxpg : top;
		}

		/*
		 * handle each region
		 */
		if ((mptr->Attribute & EFI_MEMORY_RUNTIME) == EFI_MEMORY_RUNTIME ||
		    pmap_type != kEfiConventionalMemory) {
		        prev_pmptr = 0;
			continue;
		} else {
		        /*
			 * Usable memory region
			 */
		        if (top < I386_LOWMEM_RESERVED ||
			    !pal_is_usable_memory(base, top)) {
			        prev_pmptr = 0;
				continue;
			}
			/*
			 * A range may be marked with with the
			 * EFI_MEMORY_KERN_RESERVED attribute
			 * on some systems, to indicate that the range
			 * must not be made available to devices.
			 */

			if (mptr->Attribute & EFI_MEMORY_KERN_RESERVED) {
				if (++pmap_reserved_ranges > PMAP_MAX_RESERVED_RANGES) {
					panic("Too many reserved ranges %u\n", pmap_reserved_ranges);
				}
			}

			if (top < fap) {
			        /*
				 * entire range below first_avail
			         * salvage some low memory pages
				 * we use some very low memory at startup
				 * mark as already allocated here
				 */
			        if (base >= I386_LOWMEM_RESERVED)
				        pmptr->base = base;
				else
				        pmptr->base = I386_LOWMEM_RESERVED;

				pmptr->end = top;

<<<<<<< HEAD
<<<<<<< HEAD

				if ((mptr->Attribute & EFI_MEMORY_KERN_RESERVED) &&
				    (top < vm_kernel_base_page)) {
					pmptr->alloc_up = pmptr->base;
					pmptr->alloc_down = pmptr->end;
					pmap_reserved_range_indices[pmap_last_reserved_range_index++] = pmap_memory_region_count;
=======
				/*
				 * A range may be marked with with the
				 * EFI_MEMORY_KERN_RESERVED attribute
				 * on some systems, to indicate that the range
				 * must not be made available to devices.
				 * Simplifying assumptions are made regarding
				 * the placement of the range.
				 */
				if (mptr->Attribute & EFI_MEMORY_KERN_RESERVED)
					pmap_reserved_ranges++;
=======
>>>>>>> origin/10.7

				if ((mptr->Attribute & EFI_MEMORY_KERN_RESERVED) &&
				    (top < I386_KERNEL_IMAGE_BASE_PAGE)) {
					pmptr->alloc = pmptr->base;
<<<<<<< HEAD
					pmap_last_reserved_range = pmap_memory_region_count;
>>>>>>> origin/10.6
=======
					pmap_reserved_range_indices[pmap_last_reserved_range_index++] = pmap_memory_region_count;
>>>>>>> origin/10.7
				}
				else {
					/*
					 * mark as already mapped
					 */
<<<<<<< HEAD
					pmptr->alloc_up = top + 1;
					pmptr->alloc_down = top;
=======
					pmptr->alloc = top;
>>>>>>> origin/10.6
				}
				pmptr->type = pmap_type;
				pmptr->attribute = mptr->Attribute;
			}
			else if ( (base < fap) && (top > fap) ) {
			        /*
				 * spans first_avail
				 * put mem below first avail in table but
				 * mark already allocated
				 */
			        pmptr->base = base;
				pmptr->end = (fap - 1);
				pmptr->alloc_up = pmptr->end + 1;
				pmptr->alloc_down = pmptr->end;
				pmptr->type = pmap_type;
				pmptr->attribute = mptr->Attribute;
				/*
				 * we bump these here inline so the accounting
				 * below works correctly
				 */
				pmptr++;
				pmap_memory_region_count++;

<<<<<<< HEAD
				pmptr->alloc_up = pmptr->base = fap;
				pmptr->type = pmap_type;
				pmptr->attribute = mptr->Attribute;
				pmptr->alloc_down = pmptr->end = top;
=======
				pmptr->alloc = pmptr->base = fap;
				pmptr->type = pmap_type;
				pmptr->attribute = mptr->Attribute;
				pmptr->end = top;
>>>>>>> origin/10.7

				if (mptr->Attribute & EFI_MEMORY_KERN_RESERVED)
					pmap_reserved_range_indices[pmap_last_reserved_range_index++] = pmap_memory_region_count;
			} else {
			        /*
				 * entire range useable
				 */
			        pmptr->alloc_up = pmptr->base = base;
				pmptr->type = pmap_type;
				pmptr->attribute = mptr->Attribute;
<<<<<<< HEAD
				pmptr->alloc_down = pmptr->end = top;
=======
				pmptr->end = top;
>>>>>>> origin/10.7
				if (mptr->Attribute & EFI_MEMORY_KERN_RESERVED)
					pmap_reserved_range_indices[pmap_last_reserved_range_index++] = pmap_memory_region_count;
			}

			if (i386_ptob(pmptr->end) > avail_end )
			        avail_end = i386_ptob(pmptr->end);

			avail_remaining += (pmptr->end - pmptr->base);
			coalescing_permitted = (prev_pmptr && (pmptr->attribute == prev_pmptr->attribute) && ((pmptr->attribute & EFI_MEMORY_KERN_RESERVED) == 0));
			/*
			 * Consolidate contiguous memory regions, if possible
			 */
			if (prev_pmptr &&
			    (pmptr->type == prev_pmptr->type) &&
			    (coalescing_permitted) &&
<<<<<<< HEAD
			    (pmptr->base == pmptr->alloc_up) &&
			    (prev_pmptr->end == prev_pmptr->alloc_down) &&
			    (pmptr->base == (prev_pmptr->end + 1)))
			{
=======
			    (pmptr->base == pmptr->alloc) &&
			    (pmptr->base == (prev_pmptr->end + 1)))
			{
				if (prev_pmptr->end == prev_pmptr->alloc)
					prev_pmptr->alloc = pmptr->base;
>>>>>>> origin/10.7
				prev_pmptr->end = pmptr->end;
				prev_pmptr->alloc_down = pmptr->alloc_down;
			} else {
			        pmap_memory_region_count++;
				prev_pmptr = pmptr;
				pmptr++;
			}
		}
	}

#ifdef PRINT_PMAP_MEMORY_TABLE
	{
        unsigned int j;
        pmap_memory_region_t *p = pmap_memory_regions;
        addr64_t region_start, region_end;
        addr64_t efi_start, efi_end;
        for (j=0;j<pmap_memory_region_count;j++, p++) {
            kprintf("pmap region %d type %d base 0x%llx alloc_up 0x%llx alloc_down 0x%llx top 0x%llx\n",
		    j, p->type,
                    (addr64_t) p->base  << I386_PGSHIFT,
		    (addr64_t) p->alloc_up << I386_PGSHIFT,
		    (addr64_t) p->alloc_down << I386_PGSHIFT,
		    (addr64_t) p->end   << I386_PGSHIFT);
            region_start = (addr64_t) p->base << I386_PGSHIFT;
            region_end = ((addr64_t) p->end << I386_PGSHIFT) - 1;
	    mptr = (EfiMemoryRange *) ml_static_ptovirt((vm_offset_t)args->MemoryMap);
            for (i=0; i<mcount; i++, mptr = (EfiMemoryRange *)(((vm_offset_t)mptr) + msize)) {
                if (mptr->Type != kEfiLoaderCode &&
                    mptr->Type != kEfiLoaderData &&
                    mptr->Type != kEfiBootServicesCode &&
                    mptr->Type != kEfiBootServicesData &&
                    mptr->Type != kEfiConventionalMemory) {
                efi_start = (addr64_t)mptr->PhysicalStart;
                efi_end = efi_start + ((vm_offset_t)mptr->NumberOfPages << I386_PGSHIFT) - 1;
                if ((efi_start >= region_start && efi_start <= region_end) ||
                    (efi_end >= region_start && efi_end <= region_end)) {
                    kprintf(" *** Overlapping region with EFI runtime region %d\n", i);
                }
              }
            }
          }
	}
#endif

	avail_start = first_avail;
<<<<<<< HEAD
	mem_actual = args->PhysicalMemorySize;
=======
	mem_actual = sane_size;

#define MEG		(1024*1024ULL)
#define GIG		(1024*MEG)
>>>>>>> origin/10.5

	/*
	 * For user visible memory size, round up to 128 Mb
	 * - accounting for the various stolen memory not reported by EFI.
	 * This is maintained for historical, comparison purposes but
	 * we now use the memory size reported by EFI/Booter.
	 */
	sane_size = (sane_size + 128 * MB - 1) & ~((uint64_t)(128 * MB - 1));
	if (sane_size != mem_actual)
		printf("mem_actual: 0x%llx\n legacy sane_size: 0x%llx\n",
			mem_actual, sane_size);
	sane_size = mem_actual;

	/*
	 * We cap at KERNEL_MAXMEM bytes (currently 32GB for K32, 96GB for K64).
	 * Unless overriden by the maxmem= boot-arg
	 * -- which is a non-zero maxmem argument to this function.
	 */
	if (maxmem == 0 && sane_size > KERNEL_MAXMEM) {
		maxmem = KERNEL_MAXMEM;
		printf("Physical memory %lld bytes capped at %dGB\n",
			sane_size, (uint32_t) (KERNEL_MAXMEM/GB));
	}

#if defined(__i386__)
#define K32_MAXMEM	(32*GIG)
	/*
	 * For K32 we cap at K32_MAXMEM GB (currently 32GB).
	 * Unless overriden by the maxmem= boot-arg
	 * -- which is a non-zero maxmem argument to this function.
	 */
	if (maxmem == 0 && sane_size > K32_MAXMEM) {
		maxmem = K32_MAXMEM;
		printf("Physical memory %lld bytes capped at %dGB for 32-bit kernel\n",
			sane_size, (uint32_t) (K32_MAXMEM/GIG));
	}
#endif
	/*
	 * if user set maxmem, reduce memory sizes
	 */
	if ( (maxmem > (uint64_t)first_avail) && (maxmem < sane_size)) {
		ppnum_t discarded_pages  = (ppnum_t)((sane_size - maxmem) >> I386_PGSHIFT);
		ppnum_t	highest_pn = 0;
		ppnum_t	cur_end  = 0;
		uint64_t	pages_to_use;
		unsigned	cur_region = 0;

		sane_size = maxmem;

		if (avail_remaining > discarded_pages)
			avail_remaining -= discarded_pages;
		else
			avail_remaining = 0;
		
		pages_to_use = avail_remaining;

		while (cur_region < pmap_memory_region_count && pages_to_use) {
		        for (cur_end = pmap_memory_regions[cur_region].base;
			     cur_end < pmap_memory_regions[cur_region].end && pages_to_use;
			     cur_end++) {
			        if (cur_end > highest_pn)
				        highest_pn = cur_end;
				pages_to_use--;
			}
			if (pages_to_use == 0) {
			        pmap_memory_regions[cur_region].end = cur_end;
			        pmap_memory_regions[cur_region].alloc_down = cur_end;
			}

			cur_region++;
		}
		pmap_memory_region_count = cur_region;

		avail_end = i386_ptob(highest_pn + 1);
	}

	/*
	 * mem_size is only a 32 bit container... follow the PPC route
	 * and pin it to a 2 Gbyte maximum
	 */
	if (sane_size > (FOURGIG >> 1))
	        mem_size = (vm_size_t)(FOURGIG >> 1);
	else
	        mem_size = (vm_size_t)sane_size;
	max_mem = sane_size;

	kprintf("Physical memory %llu MB\n", sane_size/MB);

<<<<<<< HEAD
<<<<<<< HEAD
	max_valid_low_ppnum = (2 * GB) / PAGE_SIZE;

	if (!PE_parse_boot_argn("max_valid_dma_addr", &maxdmaaddr, sizeof (maxdmaaddr))) {
	        max_valid_dma_address = (uint64_t)4 * (uint64_t)GB;
	} else {
	        max_valid_dma_address = ((uint64_t) maxdmaaddr) * MB;

		if ((max_valid_dma_address / PAGE_SIZE) < max_valid_low_ppnum)
			max_valid_low_ppnum = (ppnum_t)(max_valid_dma_address / PAGE_SIZE);
	}
	if (avail_end >= max_valid_dma_address) {
=======
	if (!PE_parse_boot_argn("max_valid_dma_addr", &maxdmaaddr, sizeof (maxdmaaddr)))
	        max_valid_dma_address = 1024ULL * 1024ULL * 4096ULL;
	else
	        max_valid_dma_address = ((uint64_t) maxdmaaddr) * 1024ULL * 1024ULL;

	if (!PE_parse_boot_argn("maxbouncepool", &maxbouncepoolsize, sizeof (maxbouncepoolsize)))
	        maxbouncepoolsize = MAXBOUNCEPOOL;
	else
	        maxbouncepoolsize = maxbouncepoolsize * (1024 * 1024);

	/*
	 * bsd_mbuf_cluster_reserve depends on sane_size being set
	 * in order to correctly determine the size of the mbuf pool
	 * that will be reserved
	 */
	if (!PE_parse_boot_argn("maxloreserve", &maxloreserve, sizeof (maxloreserve)))
	        maxloreserve = MAXLORESERVE + bsd_mbuf_cluster_reserve();
	else
	        maxloreserve = maxloreserve * (1024 * 1024);
>>>>>>> origin/10.5

		if (!PE_parse_boot_argn("maxloreserve", &maxloreserve, sizeof (maxloreserve))) {

=======
	max_valid_low_ppnum = (2 * GB) / PAGE_SIZE;

	if (!PE_parse_boot_argn("max_valid_dma_addr", &maxdmaaddr, sizeof (maxdmaaddr))) {
	        max_valid_dma_address = (uint64_t)4 * (uint64_t)GB;
	} else {
	        max_valid_dma_address = ((uint64_t) maxdmaaddr) * MB;

		if ((max_valid_dma_address / PAGE_SIZE) < max_valid_low_ppnum)
			max_valid_low_ppnum = (ppnum_t)(max_valid_dma_address / PAGE_SIZE);
	}
	if (avail_end >= max_valid_dma_address) {
		uint32_t  maxloreserve;
		uint32_t  mbuf_reserve = 0;
		boolean_t mbuf_override = FALSE;

		if (!PE_parse_boot_argn("maxloreserve", &maxloreserve, sizeof (maxloreserve))) {

>>>>>>> origin/10.6
			if (sane_size >= (ONEGIG * 15))
				maxloreserve = (MAXLORESERVE / PAGE_SIZE) * 4;
			else if (sane_size >= (ONEGIG * 7))
				maxloreserve = (MAXLORESERVE / PAGE_SIZE) * 2;
			else
				maxloreserve = MAXLORESERVE / PAGE_SIZE;

<<<<<<< HEAD
#if SOCKETS
			mbuf_reserve = bsd_mbuf_cluster_reserve(&mbuf_override) / PAGE_SIZE;
#endif
=======
			mbuf_reserve = bsd_mbuf_cluster_reserve(&mbuf_override) / PAGE_SIZE;
>>>>>>> origin/10.6
		} else
			maxloreserve = (maxloreserve * (1024 * 1024)) / PAGE_SIZE;

		if (maxloreserve) {
		        vm_lopage_free_limit = maxloreserve;
			
			if (mbuf_override == TRUE) {
				vm_lopage_free_limit += mbuf_reserve;
				vm_lopage_lowater = 0;
			} else
				vm_lopage_lowater = vm_lopage_free_limit / 16;

			vm_lopage_refill = TRUE;
			vm_lopage_needed = TRUE;
		}
	}
	/*
	 *	Initialize kernel physical map.
	 *	Kernel virtual address starts at VM_KERNEL_MIN_ADDRESS.
	 */
	kprintf("avail_remaining = 0x%lx\n", (unsigned long)avail_remaining);
	pmap_bootstrap(0, IA32e);
}


unsigned int
pmap_free_pages(void)
{
	return (unsigned int)avail_remaining;
}

<<<<<<< HEAD

=======
>>>>>>> origin/10.6
boolean_t pmap_next_page_reserved(ppnum_t *);

/*
 * Pick a page from a "kernel private" reserved range; works around
 * errata on some hardware.
 */
<<<<<<< HEAD
boolean_t
pmap_next_page_reserved(ppnum_t *pn) {
	if (pmap_reserved_ranges) {
		uint32_t n;
		pmap_memory_region_t *region;
		for (n = 0; n < pmap_last_reserved_range_index; n++) {
			uint32_t reserved_index = pmap_reserved_range_indices[n];
			region = &pmap_memory_regions[reserved_index];
			if (region->alloc_up <= region->alloc_down) {
				*pn = region->alloc_up++;
				avail_remaining--;

				if (*pn > max_ppnum)
					max_ppnum = *pn;

				if (lowest_lo == 0 || *pn < lowest_lo)
					lowest_lo = *pn;

				pmap_reserved_pages_allocated++;
#if DEBUG
				if (region->alloc_up > region->alloc_down) {
					kprintf("Exhausted reserved range index: %u, base: 0x%x end: 0x%x, type: 0x%x, attribute: 0x%llx\n", reserved_index, region->base, region->end, region->type, region->attribute);
				}
#endif
				return TRUE;
			}
		}
	}
	return FALSE;
}


boolean_t
=======
boolean_t
pmap_next_page_reserved(ppnum_t *pn) {
	if (pmap_reserved_ranges) {
		uint32_t n;
		pmap_memory_region_t *region;
		for (n = 0; n < pmap_last_reserved_range_index; n++) {
			uint32_t reserved_index = pmap_reserved_range_indices[n];
			region = &pmap_memory_regions[reserved_index];
			if (region->alloc < region->end) {
				*pn = region->alloc++;
				avail_remaining--;

				if (*pn > max_ppnum)
					max_ppnum = *pn;

				if (lowest_lo == 0 || *pn < lowest_lo)
					lowest_lo = *pn;

				pmap_reserved_pages_allocated++;
#if DEBUG
				if (region->alloc == region->end) {
					kprintf("Exhausted reserved range index: %u, base: 0x%x end: 0x%x, type: 0x%x, attribute: 0x%llx\n", reserved_index, region->base, region->end, region->type, region->attribute);
				}
#endif
				return TRUE;
			}
		}
	}
	return FALSE;
}


boolean_t
>>>>>>> origin/10.6
pmap_next_page_hi(
	          ppnum_t *pn)
{
	pmap_memory_region_t *region;
	int	n;

	if (pmap_next_page_reserved(pn))
		return TRUE;

	if (avail_remaining) {
		for (n = pmap_memory_region_count - 1; n >= 0; n--) {
			region = &pmap_memory_regions[n];

<<<<<<< HEAD
			if (region->alloc_down >= region->alloc_up) {
				*pn = region->alloc_down--;
=======
			if (region->alloc != region->end) {
				*pn = region->alloc++;
>>>>>>> origin/10.6
				avail_remaining--;

				if (*pn > max_ppnum)
					max_ppnum = *pn;

                                if (lowest_lo == 0 || *pn < lowest_lo)
                                        lowest_lo = *pn;

                                if (lowest_hi == 0 || *pn < lowest_hi)
                                        lowest_hi = *pn;

                                if (*pn > highest_hi)
                                        highest_hi = *pn;

				return TRUE;
			}
		}
	}
	return FALSE;
}


boolean_t
pmap_next_page(
	       ppnum_t *pn)
{
	if (avail_remaining) while (pmap_memory_region_current < pmap_memory_region_count) {
		if (pmap_memory_regions[pmap_memory_region_current].alloc_up >
		    pmap_memory_regions[pmap_memory_region_current].alloc_down) {
			pmap_memory_region_current++;
			continue;
		}
		*pn = pmap_memory_regions[pmap_memory_region_current].alloc_up++;
		avail_remaining--;

		if (*pn > max_ppnum)
			max_ppnum = *pn;

		if (lowest_lo == 0 || *pn < lowest_lo)
			lowest_lo = *pn;

		return TRUE;
	}
	return FALSE;
}


boolean_t
pmap_valid_page(
	ppnum_t pn)
{
        unsigned int i;
	pmap_memory_region_t *pmptr = pmap_memory_regions;

	for (i = 0; i < pmap_memory_region_count; i++, pmptr++) {
	        if ( (pn >= pmptr->base) && (pn <= pmptr->end) )
	                return TRUE;
	}
	return FALSE;
}

<<<<<<< HEAD
=======
/*
 * Called once VM is fully initialized so that we can release unused
 * sections of low memory to the general pool.
 * Also complete the set-up of identity-mapped sections of the kernel:
 *  1) write-protect kernel text
 *  2) map kernel text using large pages if possible
 *  3) read and write-protect page zero (for K32)
 *  4) map the global page at the appropriate virtual address.
 *
 * Use of large pages
 * ------------------
 * To effectively map and write-protect all kernel text pages, the text
 * must be 2M-aligned at the base, and the data section above must also be
 * 2M-aligned. That is, there's padding below and above. This is achieved
 * through linker directives. Large pages are used only if this alignment
 * exists (and not overriden by the -kernel_text_page_4K boot-arg). The
 * memory layout is:
 * 
 *                       :                :
 *                       |     __DATA     |
 *               sdata:  ==================  2Meg
 *                       |                |
 *                       |  zero-padding  |
 *                       |                |
 *               etext:  ------------------ 
 *                       |                |
 *                       :                :
 *                       |                |
 *                       |     __TEXT     |
 *                       |                |
 *                       :                :
 *                       |                |
 *               stext:  ==================  2Meg
 *                       |                |
 *                       |  zero-padding  |
 *                       |                |
 *               eHIB:   ------------------ 
 *                       |     __HIB      |
 *                       :                :
 *
 * Prior to changing the mapping from 4K to 2M, the zero-padding pages
 * [eHIB,stext] and [etext,sdata] are ml_static_mfree()'d. Then all the
 * 4K pages covering [stext,etext] are coalesced as 2M large pages.
 * The now unused level-1 PTE pages are also freed.
 */
void
pmap_lowmem_finalize(void)
{
	spl_t           spl;
	int		i;

	/* Check the kernel is linked at the expected base address */
	if (i386_btop(kvtophys((vm_offset_t) &IdlePML4)) !=
	    I386_KERNEL_IMAGE_BASE_PAGE)
		panic("pmap_lowmem_finalize() unexpected kernel base address");

	/*
	 * Free all pages in pmap regions below the base:
	 * rdar://6332712
	 *	We can't free all the pages to VM that EFI reports available.
	 *	Pages in the range 0xc0000-0xff000 aren't safe over sleep/wake.
	 *	There's also a size miscalculation here: pend is one page less
	 *	than it should be but this is not fixed to be backwards
	 *	compatible.
	 *	Due to this current EFI limitation, we take only the first
	 *	entry in the memory region table. However, the loop is retained
	 * 	(with the intended termination criteria commented out) in the
	 *	hope that some day we can free all low-memory ranges.
	 *	This loop assumes the first range does not span the kernel
	 *	image base & avail_start. We skip this process on systems
	 *	with "kernel reserved" ranges, as the low memory reclamation
	 *	is handled in the initial memory map processing loop on
	 *	such systems.
	 */
	for (i = 0;
//	     pmap_memory_regions[i].end <= I386_KERNEL_IMAGE_BASE_PAGE;
	     i < 1 && (pmap_reserved_ranges == 0);
	     i++) {
		vm_offset_t	pbase = (vm_offset_t)i386_ptob(pmap_memory_regions[i].base);
		vm_offset_t	pend  = (vm_offset_t)i386_ptob(pmap_memory_regions[i].end);
//		vm_offset_t	pend  = i386_ptob(pmap_memory_regions[i].end+1);

		DBG("ml_static_mfree(%p,%p) for pmap region %d\n",
		    (void *) ml_static_ptovirt(pbase),
		    (void *) (pend - pbase), i);
		ml_static_mfree(ml_static_ptovirt(pbase), pend - pbase);
	}

	/*
	 * If text and data are both 2MB-aligned,
	 * we can map text with large-pages,
	 * unless the -kernel_text_ps_4K boot-arg overrides.
	 */
	if ((stext & I386_LPGMASK) == 0 && (sdata & I386_LPGMASK) == 0) {
		kprintf("Kernel text is 2MB aligned");
		kernel_text_ps_4K = FALSE;
		if (PE_parse_boot_argn("-kernel_text_ps_4K",
				       &kernel_text_ps_4K,
				       sizeof (kernel_text_ps_4K)))
			kprintf(" but will be mapped with 4K pages\n");
		else
			kprintf(" and will be mapped with 2M pages\n");
	}

	(void) PE_parse_boot_argn("wpkernel", &wpkernel, sizeof (wpkernel));
	if (wpkernel)
		kprintf("Kernel text %p-%p to be write-protected\n",
			(void *) stext, (void *) etext);

	spl = splhigh();

	/*
	 * Scan over text if mappings are to be changed:
	 * - Remap kernel text readonly unless the "wpkernel" boot-arg is 0 
 	 * - Change to large-pages if possible and not overriden.
	 */
	if (kernel_text_ps_4K && wpkernel) {
		vm_offset_t     myva;
		for (myva = stext; myva < etext; myva += PAGE_SIZE) {
			pt_entry_t     *ptep;

			ptep = pmap_pte(kernel_pmap, (vm_map_offset_t)myva);
			if (ptep)
				pmap_store_pte(ptep, *ptep & ~INTEL_PTE_RW);
		}
	}

	if (!kernel_text_ps_4K) {
		vm_offset_t     myva;

		/*
		 * Release zero-filled page padding used for 2M-alignment.
		 */
		DBG("ml_static_mfree(%p,%p) for padding below text\n",
			(void *) eHIB, (void *) (stext - eHIB));
		ml_static_mfree(eHIB, stext - eHIB);
		DBG("ml_static_mfree(%p,%p) for padding above text\n",
			(void *) etext, (void *) (sdata - etext));
		ml_static_mfree(etext, sdata - etext);

		/*
		 * Coalesce text pages into large pages.
		 */
		for (myva = stext; myva < sdata; myva += I386_LPGBYTES) {
			pt_entry_t	*ptep;
			vm_offset_t	pte_phys;
			pt_entry_t	*pdep;
			pt_entry_t	pde;

			pdep = pmap_pde(kernel_pmap, (vm_map_offset_t)myva);
			ptep = pmap_pte(kernel_pmap, (vm_map_offset_t)myva);
			DBG("myva: %p pdep: %p ptep: %p\n",
				(void *) myva, (void *) pdep, (void *) ptep);
			if ((*ptep & INTEL_PTE_VALID) == 0)
				continue;
			pte_phys = (vm_offset_t)(*ptep & PG_FRAME);
			pde = *pdep & PTMASK;	/* page attributes from pde */
			pde |= INTEL_PTE_PS;	/* make it a 2M entry */
			pde |= pte_phys;	/* take page frame from pte */

			if (wpkernel)
				pde &= ~INTEL_PTE_RW;
			DBG("pmap_store_pte(%p,0x%llx)\n",
				(void *)pdep, pde);
			pmap_store_pte(pdep, pde);

			/*
			 * Free the now-unused level-1 pte.
			 * Note: ptep is a virtual address to the pte in the
			 *   recursive map. We can't use this address to free
			 *   the page. Instead we need to compute its address
			 *   in the Idle PTEs in "low memory".
			 */
			vm_offset_t vm_ptep = (vm_offset_t) KPTphys
						+ (pte_phys >> PTPGSHIFT);
			DBG("ml_static_mfree(%p,0x%x) for pte\n",
				(void *) vm_ptep, PAGE_SIZE);
			ml_static_mfree(vm_ptep, PAGE_SIZE);
		}

		/* Change variable read by sysctl machdep.pmap */
		pmap_kernel_text_ps = I386_LPGBYTES;
	}

#if defined(__i386__)
	/* no matter what,  kernel page zero is not accessible */
	pmap_store_pte(pmap_pte(kernel_pmap, 0), INTEL_PTE_INVALID);
#endif

	/* map lowmem global page into fixed addr */
	pt_entry_t *pte = NULL;
	if (0 == (pte = pmap_pte(kernel_pmap,
				 VM_MIN_KERNEL_LOADED_ADDRESS + 0x2000)))
		panic("lowmem pte");
	/* make sure it is defined on page boundary */
	assert(0 == ((vm_offset_t) &lowGlo & PAGE_MASK));
	pmap_store_pte(pte, kvtophys((vm_offset_t)&lowGlo)
				| INTEL_PTE_REF
				| INTEL_PTE_MOD
				| INTEL_PTE_WIRED
				| INTEL_PTE_VALID
				| INTEL_PTE_RW);
	splx(spl);
	flush_tlb();
}

>>>>>>> origin/10.6
