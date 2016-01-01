/*
 * Copyright (c) 2000 Apple Computer, Inc. All rights reserved.
 *
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
 * EXPRESS OR IMPLIED, AND APPLE HEREBY DISCLAIMS ALL SUCH WARRANTIES,
 * INCLUDING WITHOUT LIMITATION, ANY WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE OR NON-INFRINGEMENT.  Please see the
 * License for the specific language governing rights and limitations
 * under the License.
 * 
 * @APPLE_LICENSE_HEADER_END@
 */
/*
 * @OSF_COPYRIGHT@
 * 
 */

#include <debug.h>
#include <mach/vm_param.h>
#include <vm/vm_kern.h>
#include <vm/vm_map.h>
#include <vm/vm_page.h>
#include <ppc/pmap.h>
#include <ppc/io_map_entries.h>
#include <ppc/Firmware.h>
#include <ppc/mappings.h>
#include <ppc/proc_reg.h>

extern vm_offset_t	virtual_avail;

/*
 * Allocate and map memory for devices that may need to be mapped 
 * outside the usual physical memory. If phys_addr is NULL then
 * steal the appropriate number of physical pages from the vm
 * system and map them.
 */
vm_offset_t
<<<<<<< HEAD
io_map(phys_addr, size)
	vm_offset_t	phys_addr;
	vm_size_t	size;
=======
io_map(vm_map_offset_t phys_addr, vm_size_t size, unsigned int flags)
>>>>>>> origin/10.6
{
	vm_offset_t	start;
	int		i;
	unsigned int j;
	vm_page_t 	m;


#if DEBUG
	assert (kernel_map != VM_MAP_NULL);			/* VM must be initialised */
#endif

	if (phys_addr != 0) {
		/* make sure we map full contents of all the pages concerned */
		size = round_page(size + (phys_addr & PAGE_MASK));

		/* Steal some free virtual addresses */

		(void) kmem_alloc_pageable(kernel_map, &start, size);
		
		pmap_map_block(kernel_pmap, start, phys_addr, size, 
		   VM_PROT_READ|VM_PROT_WRITE, PTE_WIMG_IO, 0);	/* Set up a block mapped area */

<<<<<<< HEAD
		return (start + (phys_addr & PAGE_MASK));
=======
		return (start + (phys_addr & PAGE_MASK));	/* Pass back the virtual address */
>>>>>>> origin/10.6
	
	} else {
	
		/* Steal some free virtual addresses */
		(void) kmem_alloc_pageable(kernel_map, &start, size);

		mapping_prealloc(size);					/* Make sure there are enough free mappings */
		/* Steal some physical pages and map them one by one */
		for (i = 0; i < size ; i += PAGE_SIZE) {
			m = VM_PAGE_NULL;
			while ((m = vm_page_grab()) == VM_PAGE_NULL)
				VM_PAGE_WAIT();
			vm_page_gobble(m);
			(void) pmap_map_bd(start + i,
					   m->phys_addr,
					   m->phys_addr + PAGE_SIZE,
					   VM_PROT_READ|VM_PROT_WRITE);
		}

		mapping_relpre();						/* Allow mapping release */
		return start;
	}
}
<<<<<<< HEAD
=======


/*
 * Allocate and map memory for devices before the VM system comes alive.
 */

vm_offset_t io_map_spec(vm_map_offset_t phys_addr, vm_size_t size, unsigned int flags)
{
	vm_offset_t	start;
	unsigned int    mflags;

	if(kernel_map != VM_MAP_NULL) {				/* If VM system is up, redirect to normal routine */
		
		return io_map(phys_addr, size, flags);			/* Map the address */
	
	}

	mflags = mmFlgBlock | mmFlgUseAttr | (flags & VM_MEM_GUARDED) | ((flags & VM_MEM_NOT_CACHEABLE) >> 1);	/* Convert to our mapping_make flags */
	
	size = round_page(size + (phys_addr - (phys_addr & -PAGE_SIZE)));	/* Extend the length to include it all */
	start = pmap_boot_map(size);				/* Get me some virtual address */

	(void)mapping_make(kernel_pmap, (addr64_t)start, (ppnum_t)(phys_addr >> 12), 
		mflags,					/* Map with requested cache mode */
		(size >> 12), VM_PROT_READ|VM_PROT_WRITE);

	return (start + (phys_addr & PAGE_MASK));
}
>>>>>>> origin/10.6
