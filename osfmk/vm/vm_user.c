/*
 * Copyright (c) 2000-2007 Apple Inc. All rights reserved.
 *
 * @APPLE_OSREFERENCE_LICENSE_HEADER_START@
 * 
<<<<<<< HEAD
<<<<<<< HEAD
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
=======
 * Copyright (c) 1999-2003 Apple Computer, Inc.  All Rights Reserved.
 * 
 * This file contains Original Code and/or Modifications of Original Code
 * as defined in and that are subject to the Apple Public Source License
 * Version 2.0 (the 'License'). You may not use this file except in
 * compliance with the License. Please obtain a copy of the License at
 * http://www.opensource.apple.com/apsl/ and read it before using this
 * file.
>>>>>>> origin/10.2
 * 
 * The Original Code and all software distributed under the License are
 * distributed on an 'AS IS' basis, WITHOUT WARRANTY OF ANY KIND, EITHER
=======
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
 * Copyright (c) 1991,1990,1989,1988 Carnegie Mellon University
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
/*
 */
/*
 *	File:	vm/vm_user.c
 *	Author:	Avadis Tevanian, Jr., Michael Wayne Young
 * 
 *	User-exported virtual memory functions.
 */

/*
 * There are three implementations of the "XXX_allocate" functionality in
 * the kernel: mach_vm_allocate (for any task on the platform), vm_allocate
 * (for a task with the same address space size, especially the current task),
 * and vm32_vm_allocate (for the specific case of a 32-bit task). vm_allocate
 * in the kernel should only be used on the kernel_task. vm32_vm_allocate only
 * makes sense on platforms where a user task can either be 32 or 64, or the kernel
 * task can be 32 or 64. mach_vm_allocate makes sense everywhere, and is preferred
 * for new code.
 *
 * The entrypoints into the kernel are more complex. All platforms support a
 * mach_vm_allocate-style API (subsystem 4800) which operates with the largest
 * size types for the platform. On platforms that only support U32/K32,
 * subsystem 4800 is all you need. On platforms that support both U32 and U64,
 * subsystem 3800 is used disambiguate the size of parameters, and they will
 * always be 32-bit and call into the vm32_vm_allocate APIs. On non-U32/K32 platforms,
 * the MIG glue should never call into vm_allocate directly, because the calling
 * task and kernel_task are unlikely to use the same size parameters
 *
 * New VM call implementations should be added here and to mach_vm.defs
 * (subsystem 4800), and use mach_vm_* "wide" types.
 */

#include <debug.h>

#include <vm_cpm.h>
#include <mach/boolean.h>
#include <mach/kern_return.h>
#include <mach/mach_types.h>	/* to get vm_address_t */
#include <mach/memory_object.h>
#include <mach/std_types.h>	/* to get pointer_t */
#include <mach/upl.h>
#include <mach/vm_attributes.h>
#include <mach/vm_param.h>
#include <mach/vm_statistics.h>
#include <mach/mach_syscalls.h>

#include <mach/host_priv_server.h>
#include <mach/mach_vm_server.h>
#include <mach/vm_map_server.h>

#include <kern/host.h>
#include <kern/kalloc.h>
#include <kern/task.h>
#include <kern/misc_protos.h>
#include <vm/vm_fault.h>
#include <vm/vm_map.h>
#include <vm/vm_object.h>
#include <vm/vm_page.h>
#include <vm/memory_object.h>
#include <vm/vm_pageout.h>
#include <vm/vm_protos.h>
#include <vm/vm_purgeable_internal.h>

vm_size_t        upl_offset_to_pagelist = 0;

#if	VM_CPM
#include <vm/cpm.h>
#endif	/* VM_CPM */

ipc_port_t	dynamic_pager_control_port=NULL;

/*
 *	mach_vm_allocate allocates "zero fill" memory in the specfied
 *	map.
 */
kern_return_t
mach_vm_allocate(
	vm_map_t		map,
	mach_vm_offset_t	*addr,
	mach_vm_size_t	size,
	int			flags)
{
	vm_map_offset_t map_addr;
	vm_map_size_t	map_size;
	kern_return_t	result;
	boolean_t	anywhere;

	/* filter out any kernel-only flags */
	if (flags & ~VM_FLAGS_USER_ALLOCATE)
		return KERN_INVALID_ARGUMENT;

	if (map == VM_MAP_NULL)
		return(KERN_INVALID_ARGUMENT);
	if (size == 0) {
		*addr = 0;
		return(KERN_SUCCESS);
	}

	anywhere = ((VM_FLAGS_ANYWHERE & flags) != 0);
	if (anywhere) {
		/*
		 * No specific address requested, so start candidate address
		 * search at the minimum address in the map.  However, if that
		 * minimum is 0, bump it up by PAGE_SIZE.  We want to limit
		 * allocations of PAGEZERO to explicit requests since its
		 * normal use is to catch dereferences of NULL and many
		 * applications also treat pointers with a value of 0 as
		 * special and suddenly having address 0 contain useable
		 * memory would tend to confuse those applications.
		 */
		map_addr = vm_map_min(map);
		if (map_addr == 0)
			map_addr += VM_MAP_PAGE_SIZE(map);
	} else
		map_addr = vm_map_trunc_page(*addr,
					     VM_MAP_PAGE_MASK(map));
	map_size = vm_map_round_page(size,
				     VM_MAP_PAGE_MASK(map));
	if (map_size == 0) {
	  return(KERN_INVALID_ARGUMENT);
	}

	result = vm_map_enter(
			map,
			&map_addr,
			map_size,
			(vm_map_offset_t)0,
			flags,
			VM_OBJECT_NULL,
			(vm_object_offset_t)0,
			FALSE,
			VM_PROT_DEFAULT,
			VM_PROT_ALL,
			VM_INHERIT_DEFAULT);

	*addr = map_addr;
	return(result);
}

/*
 *	vm_allocate 
 *	Legacy routine that allocates "zero fill" memory in the specfied
 *	map (which is limited to the same size as the kernel).
 */
kern_return_t
vm_allocate(
	vm_map_t	map,
	vm_offset_t	*addr,
	vm_size_t	size,
	int		flags)
{
	vm_map_offset_t map_addr;
	vm_map_size_t	map_size;
	kern_return_t	result;
	boolean_t	anywhere;

	/* filter out any kernel-only flags */
	if (flags & ~VM_FLAGS_USER_ALLOCATE)
		return KERN_INVALID_ARGUMENT;

	if (map == VM_MAP_NULL)
		return(KERN_INVALID_ARGUMENT);
	if (size == 0) {
		*addr = 0;
		return(KERN_SUCCESS);
	}

	anywhere = ((VM_FLAGS_ANYWHERE & flags) != 0);
	if (anywhere) {
		/*
		 * No specific address requested, so start candidate address
		 * search at the minimum address in the map.  However, if that
		 * minimum is 0, bump it up by PAGE_SIZE.  We want to limit
		 * allocations of PAGEZERO to explicit requests since its
		 * normal use is to catch dereferences of NULL and many
		 * applications also treat pointers with a value of 0 as
		 * special and suddenly having address 0 contain useable
		 * memory would tend to confuse those applications.
		 */
		map_addr = vm_map_min(map);
		if (map_addr == 0)
			map_addr += VM_MAP_PAGE_SIZE(map);
	} else
		map_addr = vm_map_trunc_page(*addr,
					     VM_MAP_PAGE_MASK(map));
	map_size = vm_map_round_page(size,
				     VM_MAP_PAGE_MASK(map));
	if (map_size == 0) {
	  return(KERN_INVALID_ARGUMENT);
	}

	result = vm_map_enter(
			map,
			&map_addr,
			map_size,
			(vm_map_offset_t)0,
			flags,
			VM_OBJECT_NULL,
			(vm_object_offset_t)0,
			FALSE,
			VM_PROT_DEFAULT,
			VM_PROT_ALL,
			VM_INHERIT_DEFAULT);

	*addr = CAST_DOWN(vm_offset_t, map_addr);
	return(result);
}

/*
 *	mach_vm_deallocate -
 *	deallocates the specified range of addresses in the
 *	specified address map.
 */
kern_return_t
mach_vm_deallocate(
	vm_map_t		map,
	mach_vm_offset_t	start,
	mach_vm_size_t	size)
{
	if ((map == VM_MAP_NULL) || (start + size < start))
		return(KERN_INVALID_ARGUMENT);

	if (size == (mach_vm_offset_t) 0)
		return(KERN_SUCCESS);

	return(vm_map_remove(map,
			     vm_map_trunc_page(start,
					       VM_MAP_PAGE_MASK(map)),
			     vm_map_round_page(start+size,
					       VM_MAP_PAGE_MASK(map)),
			     VM_MAP_NO_FLAGS));
}

/*
 *	vm_deallocate -
 *	deallocates the specified range of addresses in the
 *	specified address map (limited to addresses the same
 *	size as the kernel).
 */
kern_return_t
vm_deallocate(
	register vm_map_t	map,
	vm_offset_t		start,
	vm_size_t		size)
{
	if ((map == VM_MAP_NULL) || (start + size < start))
		return(KERN_INVALID_ARGUMENT);

	if (size == (vm_offset_t) 0)
		return(KERN_SUCCESS);

	return(vm_map_remove(map,
			     vm_map_trunc_page(start,
					       VM_MAP_PAGE_MASK(map)),
			     vm_map_round_page(start+size,
					       VM_MAP_PAGE_MASK(map)),
			     VM_MAP_NO_FLAGS));
}

/*
 *	mach_vm_inherit -
 *	Sets the inheritance of the specified range in the
 *	specified map.
 */
kern_return_t
mach_vm_inherit(
	vm_map_t		map,
	mach_vm_offset_t	start,
	mach_vm_size_t	size,
	vm_inherit_t		new_inheritance)
{
	if ((map == VM_MAP_NULL) || (start + size < start) ||
	    (new_inheritance > VM_INHERIT_LAST_VALID))
                return(KERN_INVALID_ARGUMENT);

	if (size == 0)
		return KERN_SUCCESS;

	return(vm_map_inherit(map,
			      vm_map_trunc_page(start,
						VM_MAP_PAGE_MASK(map)),
			      vm_map_round_page(start+size,
						VM_MAP_PAGE_MASK(map)),
			      new_inheritance));
}

/*
 *	vm_inherit -
 *	Sets the inheritance of the specified range in the
 *	specified map (range limited to addresses
 */
kern_return_t
vm_inherit(
	register vm_map_t	map,
	vm_offset_t		start,
	vm_size_t		size,
	vm_inherit_t		new_inheritance)
{
	if ((map == VM_MAP_NULL) || (start + size < start) ||
	    (new_inheritance > VM_INHERIT_LAST_VALID))
                return(KERN_INVALID_ARGUMENT);

	if (size == 0)
		return KERN_SUCCESS;

	return(vm_map_inherit(map,
			      vm_map_trunc_page(start,
						VM_MAP_PAGE_MASK(map)),
			      vm_map_round_page(start+size,
						VM_MAP_PAGE_MASK(map)),
			      new_inheritance));
}

/*
 *	mach_vm_protect -
 *	Sets the protection of the specified range in the
 *	specified map.
 */

kern_return_t
mach_vm_protect(
	vm_map_t		map,
	mach_vm_offset_t	start,
	mach_vm_size_t	size,
	boolean_t		set_maximum,
	vm_prot_t		new_protection)
{
	if ((map == VM_MAP_NULL) || (start + size < start) ||
	    (new_protection & ~(VM_PROT_ALL | VM_PROT_COPY)))
		return(KERN_INVALID_ARGUMENT);

	if (size == 0)
		return KERN_SUCCESS;

	return(vm_map_protect(map,
			      vm_map_trunc_page(start,
						VM_MAP_PAGE_MASK(map)),
			      vm_map_round_page(start+size,
						VM_MAP_PAGE_MASK(map)),
			      new_protection,
			      set_maximum));
}

/*
 *	vm_protect -
 *	Sets the protection of the specified range in the
 *	specified map. Addressability of the range limited
 *	to the same size as the kernel.
 */

kern_return_t
vm_protect(
	vm_map_t		map,
	vm_offset_t		start,
	vm_size_t		size,
	boolean_t		set_maximum,
	vm_prot_t		new_protection)
{
	if ((map == VM_MAP_NULL) || (start + size < start) ||
	    (new_protection & ~(VM_PROT_ALL | VM_PROT_COPY)))
		return(KERN_INVALID_ARGUMENT);

	if (size == 0)
		return KERN_SUCCESS;

	return(vm_map_protect(map,
			      vm_map_trunc_page(start,
						VM_MAP_PAGE_MASK(map)),
			      vm_map_round_page(start+size,
						VM_MAP_PAGE_MASK(map)),
			      new_protection,
			      set_maximum));
}

/*
 * mach_vm_machine_attributes -
 * Handle machine-specific attributes for a mapping, such
 * as cachability, migrability, etc.
 */
kern_return_t
mach_vm_machine_attribute(
	vm_map_t			map,
	mach_vm_address_t		addr,
	mach_vm_size_t		size,
	vm_machine_attribute_t	attribute,
	vm_machine_attribute_val_t* value)		/* IN/OUT */
{
	if ((map == VM_MAP_NULL) || (addr + size < addr))
		return(KERN_INVALID_ARGUMENT);

	if (size == 0)
		return KERN_SUCCESS;

	return vm_map_machine_attribute(
		map, 
		vm_map_trunc_page(addr,
				  VM_MAP_PAGE_MASK(map)),
		vm_map_round_page(addr+size,
				  VM_MAP_PAGE_MASK(map)),
		attribute,
		value);
}

/*
 * vm_machine_attribute -
 * Handle machine-specific attributes for a mapping, such
 * as cachability, migrability, etc. Limited addressability
 * (same range limits as for the native kernel map).
 */
kern_return_t
vm_machine_attribute(
	vm_map_t	map,
	vm_address_t	addr,
	vm_size_t	size,
	vm_machine_attribute_t	attribute,
	vm_machine_attribute_val_t* value)		/* IN/OUT */
{
	if ((map == VM_MAP_NULL) || (addr + size < addr))
		return(KERN_INVALID_ARGUMENT);

	if (size == 0)
		return KERN_SUCCESS;

	return vm_map_machine_attribute(
		map, 
		vm_map_trunc_page(addr,
				  VM_MAP_PAGE_MASK(map)),
		vm_map_round_page(addr+size,
				  VM_MAP_PAGE_MASK(map)),
		attribute,
		value);
}

/*
 * mach_vm_read -
 * Read/copy a range from one address space and return it to the caller.
 *
 * It is assumed that the address for the returned memory is selected by
 * the IPC implementation as part of receiving the reply to this call.
 * If IPC isn't used, the caller must deal with the vm_map_copy_t object
 * that gets returned.
 * 
 * JMM - because of mach_msg_type_number_t, this call is limited to a
 * single 4GB region at this time.
 *
 */
kern_return_t
mach_vm_read(
	vm_map_t		map,
	mach_vm_address_t	addr,
	mach_vm_size_t	size,
	pointer_t		*data,
	mach_msg_type_number_t	*data_size)
{
	kern_return_t	error;
	vm_map_copy_t	ipc_address;

	if (map == VM_MAP_NULL)
		return(KERN_INVALID_ARGUMENT);

	if ((mach_msg_type_number_t) size != size)
		return KERN_INVALID_ARGUMENT;
	
	error = vm_map_copyin(map,
			(vm_map_address_t)addr,
			(vm_map_size_t)size,
			FALSE,	/* src_destroy */
			&ipc_address);

	if (KERN_SUCCESS == error) {
		*data = (pointer_t) ipc_address;
		*data_size = (mach_msg_type_number_t) size;
		assert(*data_size == size);
	}
	return(error);
}

/*
 * vm_read -
 * Read/copy a range from one address space and return it to the caller.
 * Limited addressability (same range limits as for the native kernel map).
 * 
 * It is assumed that the address for the returned memory is selected by
 * the IPC implementation as part of receiving the reply to this call.
 * If IPC isn't used, the caller must deal with the vm_map_copy_t object
 * that gets returned.
 */
kern_return_t
vm_read(
	vm_map_t		map,
	vm_address_t		addr,
	vm_size_t		size,
	pointer_t		*data,
	mach_msg_type_number_t	*data_size)
{
	kern_return_t	error;
	vm_map_copy_t	ipc_address;

	if (map == VM_MAP_NULL)
		return(KERN_INVALID_ARGUMENT);

	if (size > (unsigned)(mach_msg_type_number_t) -1) {
		/*
		 * The kernel could handle a 64-bit "size" value, but
		 * it could not return the size of the data in "*data_size"
		 * without overflowing.
		 * Let's reject this "size" as invalid.
		 */
		return KERN_INVALID_ARGUMENT;
	}

	error = vm_map_copyin(map,
			(vm_map_address_t)addr,
			(vm_map_size_t)size,
			FALSE,	/* src_destroy */
			&ipc_address);

	if (KERN_SUCCESS == error) {
		*data = (pointer_t) ipc_address;
		*data_size = (mach_msg_type_number_t) size;
		assert(*data_size == size);
	}
	return(error);
}

/* 
 * mach_vm_read_list -
 * Read/copy a list of address ranges from specified map.
 *
 * MIG does not know how to deal with a returned array of
 * vm_map_copy_t structures, so we have to do the copyout
 * manually here.
 */
kern_return_t
mach_vm_read_list(
	vm_map_t			map,
	mach_vm_read_entry_t		data_list,
	natural_t			count)
{
	mach_msg_type_number_t	i;
	kern_return_t	error;
	vm_map_copy_t	copy;

	if (map == VM_MAP_NULL ||
	    count > VM_MAP_ENTRY_MAX)
		return(KERN_INVALID_ARGUMENT);

	error = KERN_SUCCESS;
	for(i=0; i<count; i++) {
		vm_map_address_t map_addr;
		vm_map_size_t map_size;

		map_addr = (vm_map_address_t)(data_list[i].address);
		map_size = (vm_map_size_t)(data_list[i].size);

		if(map_size != 0) {
			error = vm_map_copyin(map,
					map_addr,
					map_size,
					FALSE,	/* src_destroy */
					&copy);
			if (KERN_SUCCESS == error) {
				error = vm_map_copyout(
						current_task()->map, 
						&map_addr,
						copy);
				if (KERN_SUCCESS == error) {
					data_list[i].address = map_addr;
					continue;
				}
				vm_map_copy_discard(copy);
			}
		}
		data_list[i].address = (mach_vm_address_t)0;
		data_list[i].size = (mach_vm_size_t)0;
	}
	return(error);
}

/* 
 * vm_read_list -
 * Read/copy a list of address ranges from specified map.
 *
 * MIG does not know how to deal with a returned array of
 * vm_map_copy_t structures, so we have to do the copyout
 * manually here.
 *
 * The source and destination ranges are limited to those
 * that can be described with a vm_address_t (i.e. same
 * size map as the kernel).
 *
 * JMM - If the result of the copyout is an address range
 * that cannot be described with a vm_address_t (i.e. the
 * caller had a larger address space but used this call
 * anyway), it will result in a truncated address being
 * returned (and a likely confused caller).
 */

kern_return_t
vm_read_list(
	vm_map_t		map,
	vm_read_entry_t	data_list,
	natural_t		count)
{
	mach_msg_type_number_t	i;
	kern_return_t	error;
	vm_map_copy_t	copy;

	if (map == VM_MAP_NULL ||
	    count > VM_MAP_ENTRY_MAX)
		return(KERN_INVALID_ARGUMENT);

	error = KERN_SUCCESS;
	for(i=0; i<count; i++) {
		vm_map_address_t map_addr;
		vm_map_size_t map_size;

		map_addr = (vm_map_address_t)(data_list[i].address);
		map_size = (vm_map_size_t)(data_list[i].size);

		if(map_size != 0) {
			error = vm_map_copyin(map,
					map_addr,
					map_size,
					FALSE,	/* src_destroy */
					&copy);
			if (KERN_SUCCESS == error) {
				error = vm_map_copyout(current_task()->map, 
						&map_addr,
						copy);
				if (KERN_SUCCESS == error) {
					data_list[i].address =
						CAST_DOWN(vm_offset_t, map_addr);
					continue;
				}
				vm_map_copy_discard(copy);
			}
		}
		data_list[i].address = (mach_vm_address_t)0;
		data_list[i].size = (mach_vm_size_t)0;
	}
	return(error);
}

/*
 * mach_vm_read_overwrite -
 * Overwrite a range of the current map with data from the specified
 * map/address range.
 * 
 * In making an assumption that the current thread is local, it is
 * no longer cluster-safe without a fully supportive local proxy
 * thread/task (but we don't support cluster's anymore so this is moot).
 */

kern_return_t
mach_vm_read_overwrite(
	vm_map_t		map,
	mach_vm_address_t	address,
	mach_vm_size_t	size,
	mach_vm_address_t	data,
	mach_vm_size_t	*data_size)
{
	kern_return_t	error;
	vm_map_copy_t	copy;

	if (map == VM_MAP_NULL)
		return(KERN_INVALID_ARGUMENT);

	error = vm_map_copyin(map, (vm_map_address_t)address,
				(vm_map_size_t)size, FALSE, &copy);

	if (KERN_SUCCESS == error) {
		error = vm_map_copy_overwrite(current_thread()->map,
 					(vm_map_address_t)data, 
					copy, FALSE);
		if (KERN_SUCCESS == error) {
			*data_size = size;
			return error;
		}
		vm_map_copy_discard(copy);
	}
	return(error);
}

/*
 * vm_read_overwrite -
 * Overwrite a range of the current map with data from the specified
 * map/address range.
 * 
 * This routine adds the additional limitation that the source and
 * destination ranges must be describable with vm_address_t values
 * (i.e. the same size address spaces as the kernel, or at least the
 * the ranges are in that first portion of the respective address
 * spaces).
 */

kern_return_t
vm_read_overwrite(
	vm_map_t	map,
	vm_address_t	address,
	vm_size_t	size,
	vm_address_t	data,
	vm_size_t	*data_size)
{
	kern_return_t	error;
	vm_map_copy_t	copy;

	if (map == VM_MAP_NULL)
		return(KERN_INVALID_ARGUMENT);

	error = vm_map_copyin(map, (vm_map_address_t)address,
				(vm_map_size_t)size, FALSE, &copy);

	if (KERN_SUCCESS == error) {
		error = vm_map_copy_overwrite(current_thread()->map,
 					(vm_map_address_t)data, 
					copy, FALSE);
		if (KERN_SUCCESS == error) {
			*data_size = size;
			return error;
		}
		vm_map_copy_discard(copy);
	}
	return(error);
}


/*
 * mach_vm_write -
 * Overwrite the specified address range with the data provided
 * (from the current map).
 */
kern_return_t
mach_vm_write(
	vm_map_t			map,
	mach_vm_address_t		address,
	pointer_t			data,
	__unused mach_msg_type_number_t	size)
{
	if (map == VM_MAP_NULL)
		return KERN_INVALID_ARGUMENT;

	return vm_map_copy_overwrite(map, (vm_map_address_t)address,
		(vm_map_copy_t) data, FALSE /* interruptible XXX */);
}

/*
 * vm_write -
 * Overwrite the specified address range with the data provided
 * (from the current map).
 *
 * The addressability of the range of addresses to overwrite is
 * limited bu the use of a vm_address_t (same size as kernel map).
 * Either the target map is also small, or the range is in the
 * low addresses within it.
 */
kern_return_t
vm_write(
	vm_map_t			map,
	vm_address_t			address,
	pointer_t			data,
	__unused mach_msg_type_number_t	size)
{
	if (map == VM_MAP_NULL)
		return KERN_INVALID_ARGUMENT;

	return vm_map_copy_overwrite(map, (vm_map_address_t)address,
		(vm_map_copy_t) data, FALSE /* interruptible XXX */);
}

/*
 * mach_vm_copy -
 * Overwrite one range of the specified map with the contents of
 * another range within that same map (i.e. both address ranges
 * are "over there").
 */
kern_return_t
mach_vm_copy(
	vm_map_t		map,
	mach_vm_address_t	source_address,
	mach_vm_size_t	size,
	mach_vm_address_t	dest_address)
{
	vm_map_copy_t copy;
	kern_return_t kr;

	if (map == VM_MAP_NULL)
		return KERN_INVALID_ARGUMENT;

	kr = vm_map_copyin(map, (vm_map_address_t)source_address,
			   (vm_map_size_t)size, FALSE, &copy);

	if (KERN_SUCCESS == kr) {
		kr = vm_map_copy_overwrite(map,
				(vm_map_address_t)dest_address,
				copy, FALSE /* interruptible XXX */);

		if (KERN_SUCCESS != kr)
			vm_map_copy_discard(copy);
	}
	return kr;
}

kern_return_t
vm_copy(
	vm_map_t	map,
	vm_address_t	source_address,
	vm_size_t	size,
	vm_address_t	dest_address)
{
	vm_map_copy_t copy;
	kern_return_t kr;

	if (map == VM_MAP_NULL)
		return KERN_INVALID_ARGUMENT;

	kr = vm_map_copyin(map, (vm_map_address_t)source_address,
			   (vm_map_size_t)size, FALSE, &copy);

	if (KERN_SUCCESS == kr) {
		kr = vm_map_copy_overwrite(map,
				(vm_map_address_t)dest_address,
				copy, FALSE /* interruptible XXX */);

		if (KERN_SUCCESS != kr)
			vm_map_copy_discard(copy);
	}
	return kr;
}

/*
 * mach_vm_map -
 * Map some range of an object into an address space.
 *
 * The object can be one of several types of objects:
 *	NULL - anonymous memory
 *	a named entry - a range within another address space
 *	                or a range within a memory object
 *	a whole memory object
 *
 */
kern_return_t
mach_vm_map(
	vm_map_t		target_map,
	mach_vm_offset_t	*address,
	mach_vm_size_t	initial_size,
	mach_vm_offset_t	mask,
	int			flags,
	ipc_port_t		port,
	vm_object_offset_t	offset,
	boolean_t		copy,
	vm_prot_t		cur_protection,
	vm_prot_t		max_protection,
	vm_inherit_t		inheritance)
{
	kern_return_t		kr;
	vm_map_offset_t 	vmmaddr;

	vmmaddr = (vm_map_offset_t) *address;

	/* filter out any kernel-only flags */
	if (flags & ~VM_FLAGS_USER_MAP)
		return KERN_INVALID_ARGUMENT;

	kr = vm_map_enter_mem_object(target_map,
				       &vmmaddr,
				       initial_size,
				       mask,
				       flags,
				       port,
				       offset,
				       copy,
				       cur_protection,
				       max_protection,
				       inheritance);

	*address = vmmaddr;
	return kr;
}


/* legacy interface */
kern_return_t
vm_map_64(
	vm_map_t		target_map,
	vm_offset_t		*address,
	vm_size_t		size,
	vm_offset_t		mask,
	int			flags,
	ipc_port_t		port,
	vm_object_offset_t	offset,
	boolean_t		copy,
	vm_prot_t		cur_protection,
	vm_prot_t		max_protection,
	vm_inherit_t		inheritance)
{
	mach_vm_address_t map_addr;
	mach_vm_size_t map_size;
	mach_vm_offset_t map_mask;
	kern_return_t kr;

	map_addr = (mach_vm_address_t)*address;
	map_size = (mach_vm_size_t)size;
	map_mask = (mach_vm_offset_t)mask;

	kr = mach_vm_map(target_map, &map_addr, map_size, map_mask, flags,
			 port, offset, copy, 
			 cur_protection, max_protection, inheritance);
	*address = CAST_DOWN(vm_offset_t, map_addr);
	return kr;
}

/* temporary, until world build */
kern_return_t
vm_map(
	vm_map_t		target_map,
	vm_offset_t		*address,
	vm_size_t		size,
	vm_offset_t		mask,
	int			flags,
	ipc_port_t		port,
	vm_offset_t		offset,
	boolean_t		copy,
	vm_prot_t		cur_protection,
	vm_prot_t		max_protection,
	vm_inherit_t		inheritance)
{
	mach_vm_address_t map_addr;
	mach_vm_size_t map_size;
	mach_vm_offset_t map_mask;
	vm_object_offset_t obj_offset;
	kern_return_t kr;

	map_addr = (mach_vm_address_t)*address;
	map_size = (mach_vm_size_t)size;
	map_mask = (mach_vm_offset_t)mask;
	obj_offset = (vm_object_offset_t)offset;

	kr = mach_vm_map(target_map, &map_addr, map_size, map_mask, flags,
			 port, obj_offset, copy, 
			 cur_protection, max_protection, inheritance);
	*address = CAST_DOWN(vm_offset_t, map_addr);
	return kr;
}

/*
 * mach_vm_remap -
 * Remap a range of memory from one task into another,
 * to another address range within the same task, or
 * over top of itself (with altered permissions and/or
 * as an in-place copy of itself).
 */

kern_return_t
mach_vm_remap(
	vm_map_t		target_map,
	mach_vm_offset_t	*address,
	mach_vm_size_t	size,
	mach_vm_offset_t	mask,
	int			flags,
	vm_map_t		src_map,
	mach_vm_offset_t	memory_address,
	boolean_t		copy,
	vm_prot_t		*cur_protection,
	vm_prot_t		*max_protection,
	vm_inherit_t		inheritance)
{
	vm_map_offset_t		map_addr;
	kern_return_t		kr;

	if (VM_MAP_NULL == target_map || VM_MAP_NULL == src_map)
		return KERN_INVALID_ARGUMENT;

	/* filter out any kernel-only flags */
	if (flags & ~VM_FLAGS_USER_REMAP)
		return KERN_INVALID_ARGUMENT;

	map_addr = (vm_map_offset_t)*address;

	kr = vm_map_remap(target_map,
			  &map_addr,
			  size,
			  mask,
			  flags,
			  src_map,
			  memory_address,
			  copy,
			  cur_protection,
			  max_protection,
			  inheritance);
	*address = map_addr;
	return kr;
}

/*
 * vm_remap -
 * Remap a range of memory from one task into another,
 * to another address range within the same task, or
 * over top of itself (with altered permissions and/or
 * as an in-place copy of itself).
 *
 * The addressability of the source and target address
 * range is limited by the size of vm_address_t (in the
 * kernel context).
 */
kern_return_t
vm_remap(
	vm_map_t		target_map,
	vm_offset_t		*address,
	vm_size_t		size,
	vm_offset_t		mask,
	int			flags,
	vm_map_t		src_map,
	vm_offset_t		memory_address,
	boolean_t		copy,
	vm_prot_t		*cur_protection,
	vm_prot_t		*max_protection,
	vm_inherit_t		inheritance)
{
	vm_map_offset_t		map_addr;
	kern_return_t		kr;

	if (VM_MAP_NULL == target_map || VM_MAP_NULL == src_map)
		return KERN_INVALID_ARGUMENT;

	/* filter out any kernel-only flags */
	if (flags & ~VM_FLAGS_USER_REMAP)
		return KERN_INVALID_ARGUMENT;

	map_addr = (vm_map_offset_t)*address;

	kr = vm_map_remap(target_map,
			  &map_addr,
			  size,
			  mask,
			  flags,
			  src_map,
			  memory_address,
			  copy,
			  cur_protection,
			  max_protection,
			  inheritance);
	*address = CAST_DOWN(vm_offset_t, map_addr);
	return kr;
}

/*
 * NOTE: these routine (and this file) will no longer require mach_host_server.h
 * when mach_vm_wire and vm_wire are changed to use ledgers.
 */
#include <mach/mach_host_server.h>
/*
 *	mach_vm_wire
 *	Specify that the range of the virtual address space
 *	of the target task must not cause page faults for
 *	the indicated accesses.
 *
 *	[ To unwire the pages, specify VM_PROT_NONE. ]
 */
kern_return_t
mach_vm_wire(
	host_priv_t		host_priv,
	vm_map_t		map,
	mach_vm_offset_t	start,
	mach_vm_size_t	size,
	vm_prot_t		access)
{
	kern_return_t		rc;

	if (host_priv == HOST_PRIV_NULL)
		return KERN_INVALID_HOST;

	assert(host_priv == &realhost);

	if (map == VM_MAP_NULL)
		return KERN_INVALID_TASK;

	if (access & ~VM_PROT_ALL || (start + size < start))
		return KERN_INVALID_ARGUMENT;

	if (access != VM_PROT_NONE) {
		rc = vm_map_wire(map,
				 vm_map_trunc_page(start,
						   VM_MAP_PAGE_MASK(map)),
				 vm_map_round_page(start+size,
						   VM_MAP_PAGE_MASK(map)),
				 access | VM_PROT_MEMORY_TAG_MAKE(VM_KERN_MEMORY_MLOCK),
				 TRUE);
	} else {
		rc = vm_map_unwire(map,
				   vm_map_trunc_page(start,
						     VM_MAP_PAGE_MASK(map)),
				   vm_map_round_page(start+size,
						     VM_MAP_PAGE_MASK(map)),
				   TRUE);
	}
	return rc;
}

/*
 *	vm_wire -
 *	Specify that the range of the virtual address space
 *	of the target task must not cause page faults for
 *	the indicated accesses.
 *
 *	[ To unwire the pages, specify VM_PROT_NONE. ]
 */
kern_return_t
vm_wire(
	host_priv_t		host_priv,
	register vm_map_t	map,
	vm_offset_t		start,
	vm_size_t		size,
	vm_prot_t		access)
{
	kern_return_t		rc;

	if (host_priv == HOST_PRIV_NULL)
		return KERN_INVALID_HOST;

	assert(host_priv == &realhost);

	if (map == VM_MAP_NULL)
		return KERN_INVALID_TASK;

	if ((access & ~VM_PROT_ALL) || (start + size < start))
		return KERN_INVALID_ARGUMENT;

	if (size == 0) {
		rc = KERN_SUCCESS;
	} else if (access != VM_PROT_NONE) {
		rc = vm_map_wire(map,
				 vm_map_trunc_page(start,
						   VM_MAP_PAGE_MASK(map)),
				 vm_map_round_page(start+size,
						   VM_MAP_PAGE_MASK(map)),
				 access | VM_PROT_MEMORY_TAG_MAKE(VM_KERN_MEMORY_OSFMK),
				 TRUE);
	} else {
		rc = vm_map_unwire(map,
				   vm_map_trunc_page(start,
						     VM_MAP_PAGE_MASK(map)),
				   vm_map_round_page(start+size,
						     VM_MAP_PAGE_MASK(map)),
				   TRUE);
	}
	return rc;
}

/*
 *	vm_msync
 *
 *	Synchronises the memory range specified with its backing store
 *	image by either flushing or cleaning the contents to the appropriate
 *	memory manager.
 *
 *	interpretation of sync_flags
 *	VM_SYNC_INVALIDATE	- discard pages, only return precious
 *				  pages to manager.
 *
 *	VM_SYNC_INVALIDATE & (VM_SYNC_SYNCHRONOUS | VM_SYNC_ASYNCHRONOUS)
 *				- discard pages, write dirty or precious
 *				  pages back to memory manager.
 *
 *	VM_SYNC_SYNCHRONOUS | VM_SYNC_ASYNCHRONOUS
 *				- write dirty or precious pages back to
 *				  the memory manager.
 *
 *	VM_SYNC_CONTIGUOUS	- does everything normally, but if there
 *				  is a hole in the region, and we would
 *				  have returned KERN_SUCCESS, return
 *				  KERN_INVALID_ADDRESS instead.
 *
 *	RETURNS
 *	KERN_INVALID_TASK		Bad task parameter
 *	KERN_INVALID_ARGUMENT		both sync and async were specified.
 *	KERN_SUCCESS			The usual.
 *	KERN_INVALID_ADDRESS		There was a hole in the region.
 */

kern_return_t
mach_vm_msync(
	vm_map_t		map,
	mach_vm_address_t	address,
	mach_vm_size_t	size,
	vm_sync_t		sync_flags)
{

	if (map == VM_MAP_NULL)
		return(KERN_INVALID_TASK);

	return vm_map_msync(map, (vm_map_address_t)address,
			(vm_map_size_t)size, sync_flags);
}
      
/*
 *	vm_msync
 *
 *	Synchronises the memory range specified with its backing store
 *	image by either flushing or cleaning the contents to the appropriate
 *	memory manager.
 *
 *	interpretation of sync_flags
 *	VM_SYNC_INVALIDATE	- discard pages, only return precious
 *				  pages to manager.
 *
 *	VM_SYNC_INVALIDATE & (VM_SYNC_SYNCHRONOUS | VM_SYNC_ASYNCHRONOUS)
 *				- discard pages, write dirty or precious
 *				  pages back to memory manager.
 *
 *	VM_SYNC_SYNCHRONOUS | VM_SYNC_ASYNCHRONOUS
 *				- write dirty or precious pages back to
 *				  the memory manager.
 *
 *	VM_SYNC_CONTIGUOUS	- does everything normally, but if there
 *				  is a hole in the region, and we would
 *				  have returned KERN_SUCCESS, return
 *				  KERN_INVALID_ADDRESS instead.
 *
 *	The addressability of the range is limited to that which can
 *	be described by a vm_address_t.
 *
 *	RETURNS
 *	KERN_INVALID_TASK		Bad task parameter
 *	KERN_INVALID_ARGUMENT		both sync and async were specified.
 *	KERN_SUCCESS			The usual.
 *	KERN_INVALID_ADDRESS		There was a hole in the region.
 */

kern_return_t
vm_msync(
	vm_map_t	map,
	vm_address_t	address,
	vm_size_t	size,
	vm_sync_t	sync_flags)
{

	if (map == VM_MAP_NULL)
		return(KERN_INVALID_TASK);

	return vm_map_msync(map, (vm_map_address_t)address,
			(vm_map_size_t)size, sync_flags);
}


int
vm_toggle_entry_reuse(int toggle, int *old_value)
{
	vm_map_t map = current_map();
	
	if(toggle == VM_TOGGLE_GETVALUE && old_value != NULL){
		*old_value = map->disable_vmentry_reuse;
	} else if(toggle == VM_TOGGLE_SET){
		vm_map_entry_t map_to_entry;

		vm_map_lock(map);
		vm_map_disable_hole_optimization(map);
		map->disable_vmentry_reuse = TRUE;
		__IGNORE_WCASTALIGN(map_to_entry = vm_map_to_entry(map));
		if (map->first_free == map_to_entry) {
			map->highest_entry_end = vm_map_min(map);
		} else {
			map->highest_entry_end = map->first_free->vme_end;
		}
		vm_map_unlock(map);
	} else if (toggle == VM_TOGGLE_CLEAR){
		vm_map_lock(map);
		map->disable_vmentry_reuse = FALSE;
		vm_map_unlock(map);
	} else
		return KERN_INVALID_ARGUMENT;

	return KERN_SUCCESS;
}

/*
 *	mach_vm_behavior_set 
 *
 *	Sets the paging behavior attribute for the  specified range
 *	in the specified map.
 *
 *	This routine will fail with KERN_INVALID_ADDRESS if any address
 *	in [start,start+size) is not a valid allocated memory region.
 */
kern_return_t 
mach_vm_behavior_set(
	vm_map_t		map,
	mach_vm_offset_t	start,
	mach_vm_size_t	size,
	vm_behavior_t		new_behavior)
{
	if ((map == VM_MAP_NULL) || (start + size < start))
		return(KERN_INVALID_ARGUMENT);

	if (size == 0)
		return KERN_SUCCESS;

	return(vm_map_behavior_set(map,
				   vm_map_trunc_page(start,
						     VM_MAP_PAGE_MASK(map)), 
				   vm_map_round_page(start+size,
						     VM_MAP_PAGE_MASK(map)),
				   new_behavior));
}

/*
 *	vm_behavior_set 
 *
 *	Sets the paging behavior attribute for the  specified range
 *	in the specified map.
 *
 *	This routine will fail with KERN_INVALID_ADDRESS if any address
 *	in [start,start+size) is not a valid allocated memory region.
 *
 *	This routine is potentially limited in addressibility by the
 *	use of vm_offset_t (if the map provided is larger than the
 *	kernel's).
 */
kern_return_t 
vm_behavior_set(
	vm_map_t		map,
	vm_offset_t		start,
	vm_size_t		size,
	vm_behavior_t		new_behavior)
{
	if ((map == VM_MAP_NULL) || (start + size < start))
		return(KERN_INVALID_ARGUMENT);

	if (size == 0)
		return KERN_SUCCESS;

	return(vm_map_behavior_set(map,
				   vm_map_trunc_page(start,
						     VM_MAP_PAGE_MASK(map)), 
				   vm_map_round_page(start+size,
						     VM_MAP_PAGE_MASK(map)),
				   new_behavior));
}

/*
 *	mach_vm_region:
 *
 *	User call to obtain information about a region in
 *	a task's address map. Currently, only one flavor is
 *	supported.
 *
 *	XXX The reserved and behavior fields cannot be filled
 *	    in until the vm merge from the IK is completed, and
 *	    vm_reserve is implemented.
 *
 *	XXX Dependency: syscall_vm_region() also supports only one flavor.
 */

kern_return_t
mach_vm_region(
	vm_map_t		 map,
	mach_vm_offset_t	*address,		/* IN/OUT */
	mach_vm_size_t	*size,			/* OUT */
	vm_region_flavor_t	 flavor,		/* IN */
	vm_region_info_t	 info,			/* OUT */
	mach_msg_type_number_t	*count,			/* IN/OUT */
	mach_port_t		*object_name)		/* OUT */
{
	vm_map_offset_t 	map_addr;
	vm_map_size_t 		map_size;
	kern_return_t		kr;

	if (VM_MAP_NULL == map)
		return KERN_INVALID_ARGUMENT;

	map_addr = (vm_map_offset_t)*address;
	map_size = (vm_map_size_t)*size;

	/* legacy conversion */
	if (VM_REGION_BASIC_INFO == flavor)
		flavor = VM_REGION_BASIC_INFO_64;

	kr = vm_map_region(map,
			   &map_addr, &map_size,
			   flavor, info, count,
			   object_name);

	*address = map_addr;
	*size = map_size;
	return kr;
}

/*
 *	vm_region_64 and vm_region:
 *
 *	User call to obtain information about a region in
 *	a task's address map. Currently, only one flavor is
 *	supported.
 *
 *	XXX The reserved and behavior fields cannot be filled
 *	    in until the vm merge from the IK is completed, and
 *	    vm_reserve is implemented.
 *
 *	XXX Dependency: syscall_vm_region() also supports only one flavor.
 */

kern_return_t
vm_region_64(
	vm_map_t		 map,
	vm_offset_t	        *address,		/* IN/OUT */
	vm_size_t		*size,			/* OUT */
	vm_region_flavor_t	 flavor,		/* IN */
	vm_region_info_t	 info,			/* OUT */
	mach_msg_type_number_t	*count,			/* IN/OUT */
	mach_port_t		*object_name)		/* OUT */
{
	vm_map_offset_t 	map_addr;
	vm_map_size_t 		map_size;
	kern_return_t		kr;

	if (VM_MAP_NULL == map)
		return KERN_INVALID_ARGUMENT;

	map_addr = (vm_map_offset_t)*address;
	map_size = (vm_map_size_t)*size;

	/* legacy conversion */
	if (VM_REGION_BASIC_INFO == flavor)
		flavor = VM_REGION_BASIC_INFO_64;

	kr = vm_map_region(map,
			   &map_addr, &map_size,
			   flavor, info, count,
			   object_name);

	*address = CAST_DOWN(vm_offset_t, map_addr);
	*size = CAST_DOWN(vm_size_t, map_size);

	if (KERN_SUCCESS == kr && map_addr + map_size > VM_MAX_ADDRESS)
		return KERN_INVALID_ADDRESS;
	return kr;
}

kern_return_t
vm_region(
	vm_map_t			map,
	vm_address_t	      		*address,	/* IN/OUT */
	vm_size_t			*size,		/* OUT */
	vm_region_flavor_t	 	flavor,	/* IN */
	vm_region_info_t	 	info,		/* OUT */
	mach_msg_type_number_t	*count,	/* IN/OUT */
	mach_port_t			*object_name)	/* OUT */
{
	vm_map_address_t 	map_addr;
	vm_map_size_t 		map_size;
	kern_return_t		kr;

	if (VM_MAP_NULL == map)
		return KERN_INVALID_ARGUMENT;

	map_addr = (vm_map_address_t)*address;
	map_size = (vm_map_size_t)*size;

	kr = vm_map_region(map,
			   &map_addr, &map_size,
			   flavor, info, count,
			   object_name);

	*address = CAST_DOWN(vm_address_t, map_addr);
	*size = CAST_DOWN(vm_size_t, map_size);

	if (KERN_SUCCESS == kr && map_addr + map_size > VM_MAX_ADDRESS)
		return KERN_INVALID_ADDRESS;
	return kr;
}

/*
 *	vm_region_recurse: A form of vm_region which follows the
 *	submaps in a target map
 *
 */
kern_return_t
mach_vm_region_recurse(
	vm_map_t			map,
	mach_vm_address_t		*address,
	mach_vm_size_t		*size,
	uint32_t			*depth,
	vm_region_recurse_info_t	info,
	mach_msg_type_number_t 	*infoCnt)
{
	vm_map_address_t	map_addr;
	vm_map_size_t		map_size;
	kern_return_t		kr;

	if (VM_MAP_NULL == map)
		return KERN_INVALID_ARGUMENT;

	map_addr = (vm_map_address_t)*address;
	map_size = (vm_map_size_t)*size;

	kr = vm_map_region_recurse_64(
			map,
			&map_addr,
			&map_size,
			depth,
			(vm_region_submap_info_64_t)info,
			infoCnt);

	*address = map_addr;
	*size = map_size;
	return kr;
}

/*
 *	vm_region_recurse: A form of vm_region which follows the
 *	submaps in a target map
 *
 */
kern_return_t
vm_region_recurse_64(
	vm_map_t			map,
	vm_address_t			*address,
	vm_size_t			*size,
	uint32_t			*depth,
	vm_region_recurse_info_64_t	info,
	mach_msg_type_number_t 	*infoCnt)
{
	vm_map_address_t	map_addr;
	vm_map_size_t		map_size;
	kern_return_t		kr;

	if (VM_MAP_NULL == map)
		return KERN_INVALID_ARGUMENT;

	map_addr = (vm_map_address_t)*address;
	map_size = (vm_map_size_t)*size;

	kr = vm_map_region_recurse_64(
			map,
			&map_addr,
			&map_size,
			depth,
			(vm_region_submap_info_64_t)info,
			infoCnt);

	*address = CAST_DOWN(vm_address_t, map_addr);
	*size = CAST_DOWN(vm_size_t, map_size);

	if (KERN_SUCCESS == kr && map_addr + map_size > VM_MAX_ADDRESS)
		return KERN_INVALID_ADDRESS;
	return kr;
}

kern_return_t
vm_region_recurse(
	vm_map_t			map,
	vm_offset_t	       	*address,	/* IN/OUT */
	vm_size_t			*size,		/* OUT */
	natural_t	 		*depth,	/* IN/OUT */
	vm_region_recurse_info_t	info32,	/* IN/OUT */
	mach_msg_type_number_t	*infoCnt)	/* IN/OUT */
{
	vm_region_submap_info_data_64_t info64;
	vm_region_submap_info_t info;
	vm_map_address_t	map_addr;
	vm_map_size_t		map_size;
	kern_return_t		kr;

	if (VM_MAP_NULL == map || *infoCnt < VM_REGION_SUBMAP_INFO_COUNT)
		return KERN_INVALID_ARGUMENT;

	
	map_addr = (vm_map_address_t)*address;
	map_size = (vm_map_size_t)*size;
	info = (vm_region_submap_info_t)info32;
	*infoCnt = VM_REGION_SUBMAP_INFO_COUNT_64;

	kr = vm_map_region_recurse_64(map, &map_addr,&map_size,
				      depth, &info64, infoCnt);

	info->protection = info64.protection;
	info->max_protection = info64.max_protection;
	info->inheritance = info64.inheritance;
	info->offset = (uint32_t)info64.offset; /* trouble-maker */
        info->user_tag = info64.user_tag;
        info->pages_resident = info64.pages_resident;
        info->pages_shared_now_private = info64.pages_shared_now_private;
        info->pages_swapped_out = info64.pages_swapped_out;
        info->pages_dirtied = info64.pages_dirtied;
        info->ref_count = info64.ref_count;
        info->shadow_depth = info64.shadow_depth;
        info->external_pager = info64.external_pager;
        info->share_mode = info64.share_mode;
	info->is_submap = info64.is_submap;
	info->behavior = info64.behavior;
	info->object_id = info64.object_id;
	info->user_wired_count = info64.user_wired_count; 

	*address = CAST_DOWN(vm_address_t, map_addr);
	*size = CAST_DOWN(vm_size_t, map_size);
	*infoCnt = VM_REGION_SUBMAP_INFO_COUNT;

	if (KERN_SUCCESS == kr && map_addr + map_size > VM_MAX_ADDRESS)
		return KERN_INVALID_ADDRESS;
	return kr;
}

kern_return_t
mach_vm_purgable_control(
	vm_map_t		map,
	mach_vm_offset_t	address,
	vm_purgable_t		control,
	int			*state)
{
	if (VM_MAP_NULL == map)
		return KERN_INVALID_ARGUMENT;

	return vm_map_purgable_control(map,
				       vm_map_trunc_page(address, PAGE_MASK),
				       control,
				       state);
}

kern_return_t
vm_purgable_control(
	vm_map_t		map,
	vm_offset_t		address,
	vm_purgable_t		control,
	int			*state)
{
	if (VM_MAP_NULL == map)
		return KERN_INVALID_ARGUMENT;

	return vm_map_purgable_control(map,
				       vm_map_trunc_page(address, PAGE_MASK),
				       control,
				       state);
}
					

/*
 *	Ordinarily, the right to allocate CPM is restricted
 *	to privileged applications (those that can gain access
 *	to the host priv port).  Set this variable to zero if
 *	you want to let any application allocate CPM.
 */
unsigned int	vm_allocate_cpm_privileged = 0;

/*
 *	Allocate memory in the specified map, with the caveat that
 *	the memory is physically contiguous.  This call may fail
 *	if the system can't find sufficient contiguous memory.
 *	This call may cause or lead to heart-stopping amounts of
 *	paging activity.
 *
 *	Memory obtained from this call should be freed in the
 *	normal way, viz., via vm_deallocate.
 */
kern_return_t
vm_allocate_cpm(
	host_priv_t		host_priv,
	vm_map_t		map,
	vm_address_t		*addr,
	vm_size_t		size,
	int			flags)
{
	vm_map_address_t	map_addr;
	vm_map_size_t		map_size;
	kern_return_t		kr;

	if (vm_allocate_cpm_privileged && HOST_PRIV_NULL == host_priv)
		return KERN_INVALID_HOST;

	if (VM_MAP_NULL == map)
		return KERN_INVALID_ARGUMENT;

	map_addr = (vm_map_address_t)*addr;
	map_size = (vm_map_size_t)size;

	kr = vm_map_enter_cpm(map,
			      &map_addr,
			      map_size,
			      flags);

	*addr = CAST_DOWN(vm_address_t, map_addr);
	return kr;
}


kern_return_t
mach_vm_page_query(
	vm_map_t		map,
	mach_vm_offset_t	offset,
	int			*disposition,
	int			*ref_count)
{
	if (VM_MAP_NULL == map)
		return KERN_INVALID_ARGUMENT;

	return vm_map_page_query_internal(
		map,
		vm_map_trunc_page(offset, PAGE_MASK),
		disposition, ref_count);
}

kern_return_t
vm_map_page_query(
	vm_map_t		map,
	vm_offset_t		offset,
	int			*disposition,
	int			*ref_count)
{
	if (VM_MAP_NULL == map)
		return KERN_INVALID_ARGUMENT;

	return vm_map_page_query_internal(
		map,
		vm_map_trunc_page(offset, PAGE_MASK),
		disposition, ref_count);
}

kern_return_t
mach_vm_page_info(
	vm_map_t		map,
	mach_vm_address_t	address,
	vm_page_info_flavor_t	flavor,
	vm_page_info_t		info,
	mach_msg_type_number_t	*count)
{
	kern_return_t	kr;

	if (map == VM_MAP_NULL) {
		return KERN_INVALID_ARGUMENT;
	}

	kr = vm_map_page_info(map, address, flavor, info, count);
	return kr;
}

/* map a (whole) upl into an address space */
kern_return_t
vm_upl_map(
	vm_map_t		map, 
	upl_t			upl, 
	vm_address_t		*dst_addr)
{
	vm_map_offset_t		map_addr;
	kern_return_t		kr;

	if (VM_MAP_NULL == map)
		return KERN_INVALID_ARGUMENT;

	kr = vm_map_enter_upl(map, upl, &map_addr);
	*dst_addr = CAST_DOWN(vm_address_t, map_addr);
	return kr;
}

kern_return_t
vm_upl_unmap(
	vm_map_t		map,
	upl_t 			upl)
{
	if (VM_MAP_NULL == map)
		return KERN_INVALID_ARGUMENT;

	return (vm_map_remove_upl(map, upl));
}

/* Retrieve a upl for an object underlying an address range in a map */

kern_return_t
vm_map_get_upl(
	vm_map_t		map,
	vm_map_offset_t		map_offset,
	upl_size_t		*upl_size,
	upl_t			*upl,
	upl_page_info_array_t	page_list,
	unsigned int		*count,
	upl_control_flags_t	*flags,
	int             	force_data_sync)
{
	upl_control_flags_t map_flags;
	kern_return_t	    kr;

	if (VM_MAP_NULL == map)
		return KERN_INVALID_ARGUMENT;

	map_flags = *flags & ~UPL_NOZEROFILL;
	if (force_data_sync)
		map_flags |= UPL_FORCE_DATA_SYNC;

	kr = vm_map_create_upl(map,
			       map_offset,
			       upl_size,
			       upl,
			       page_list,
			       count,
			       &map_flags);

	*flags = (map_flags & ~UPL_FORCE_DATA_SYNC);
	return kr;
}

/*
 * mach_make_memory_entry_64
 *
 * Think of it as a two-stage vm_remap() operation.  First
 * you get a handle.  Second, you get map that handle in
 * somewhere else. Rather than doing it all at once (and
 * without needing access to the other whole map).
 */

kern_return_t
mach_make_memory_entry_64(
	vm_map_t		target_map,
	memory_object_size_t	*size,
	memory_object_offset_t offset,
	vm_prot_t		permission,
	ipc_port_t		*object_handle,
	ipc_port_t		parent_handle)
{
	vm_map_version_t	version;
	vm_named_entry_t	parent_entry;
	vm_named_entry_t	user_entry;
	ipc_port_t		user_handle;
	kern_return_t		kr;
	vm_map_t		real_map;

	/* needed for call to vm_map_lookup_locked */
	boolean_t		wired;
	boolean_t		iskernel;
	vm_object_offset_t	obj_off;
	vm_prot_t		prot;
	struct vm_object_fault_info	fault_info;
	vm_object_t		object;
	vm_object_t		shadow_object;

	/* needed for direct map entry manipulation */
	vm_map_entry_t		map_entry;
	vm_map_entry_t		next_entry;
	vm_map_t		local_map;
	vm_map_t		original_map = target_map;
	vm_map_size_t		total_size, map_size;
	vm_map_offset_t		map_start, map_end;
	vm_map_offset_t		local_offset;
	vm_object_size_t	mappable_size;

	/* 
	 * Stash the offset in the page for use by vm_map_enter_mem_object()
	 * in the VM_FLAGS_RETURN_DATA_ADDR/MAP_MEM_USE_DATA_ADDR case.
	 */
	vm_object_offset_t	offset_in_page;

	unsigned int		access;
	vm_prot_t		protections;
	vm_prot_t		original_protections, mask_protections;
	unsigned int		wimg_mode;

	boolean_t		force_shadow = FALSE;
	boolean_t 		use_data_addr;
	boolean_t 		use_4K_compat;

	if (((permission & 0x00FF0000) &
	     ~(MAP_MEM_ONLY |
	       MAP_MEM_NAMED_CREATE |
	       MAP_MEM_PURGABLE | 
	       MAP_MEM_NAMED_REUSE |
	       MAP_MEM_USE_DATA_ADDR |
	       MAP_MEM_VM_COPY |
	       MAP_MEM_4K_DATA_ADDR |
	       MAP_MEM_VM_SHARE))) {
		/*
		 * Unknown flag: reject for forward compatibility.
		 */
		return KERN_INVALID_VALUE;
	}

	if (parent_handle != IP_NULL &&
	    ip_kotype(parent_handle) == IKOT_NAMED_ENTRY) {
		parent_entry = (vm_named_entry_t) parent_handle->ip_kobject;
	} else {
		parent_entry = NULL;
	}

	if (parent_entry && parent_entry->is_copy) {
		return KERN_INVALID_ARGUMENT;
	}

	original_protections = permission & VM_PROT_ALL;
	protections = original_protections;
	mask_protections = permission & VM_PROT_IS_MASK;
	access = GET_MAP_MEM(permission);
	use_data_addr = ((permission & MAP_MEM_USE_DATA_ADDR) != 0);
	use_4K_compat = ((permission & MAP_MEM_4K_DATA_ADDR) != 0);

	user_handle = IP_NULL;
	user_entry = NULL;

	map_start = vm_map_trunc_page(offset, PAGE_MASK);

	if (permission & MAP_MEM_ONLY) {
		boolean_t		parent_is_object;

		map_end = vm_map_round_page(offset + *size, PAGE_MASK);
		map_size = map_end - map_start;
		
		if (use_data_addr || use_4K_compat || parent_entry == NULL) {
			return KERN_INVALID_ARGUMENT;
		}

		parent_is_object = !(parent_entry->is_sub_map ||
				     parent_entry->is_pager);
		object = parent_entry->backing.object;
		if(parent_is_object && object != VM_OBJECT_NULL)
			wimg_mode = object->wimg_bits;
		else
			wimg_mode = VM_WIMG_USE_DEFAULT;
		if((access != GET_MAP_MEM(parent_entry->protection)) &&
				!(parent_entry->protection & VM_PROT_WRITE)) { 
			return KERN_INVALID_RIGHT;
		}
		if(access == MAP_MEM_IO) {
		   SET_MAP_MEM(access, parent_entry->protection);
		   wimg_mode = VM_WIMG_IO;
		} else if (access == MAP_MEM_COPYBACK) {
		   SET_MAP_MEM(access, parent_entry->protection);
		   wimg_mode = VM_WIMG_USE_DEFAULT;
		} else if (access == MAP_MEM_INNERWBACK) {
		   SET_MAP_MEM(access, parent_entry->protection);
		   wimg_mode = VM_WIMG_INNERWBACK;
		} else if (access == MAP_MEM_WTHRU) {
		   SET_MAP_MEM(access, parent_entry->protection);
		   wimg_mode = VM_WIMG_WTHRU;
		} else if (access == MAP_MEM_WCOMB) {
		   SET_MAP_MEM(access, parent_entry->protection);
		   wimg_mode = VM_WIMG_WCOMB;
		}
		if (parent_is_object && object &&
			(access != MAP_MEM_NOOP) && 
			(!(object->nophyscache))) {

			if (object->wimg_bits != wimg_mode) {
				vm_object_lock(object);
				vm_object_change_wimg_mode(object, wimg_mode);
				vm_object_unlock(object);
			}
		}
		if (object_handle)
			*object_handle = IP_NULL;
		return KERN_SUCCESS;
	} else if (permission & MAP_MEM_NAMED_CREATE) {
		map_end = vm_map_round_page(offset + *size, PAGE_MASK);
		map_size = map_end - map_start;

		if (use_data_addr || use_4K_compat) {
			return KERN_INVALID_ARGUMENT;
		}

		kr = mach_memory_entry_allocate(&user_entry, &user_handle);
		if (kr != KERN_SUCCESS) {
			return KERN_FAILURE;
		}

		/*
		 * Force the creation of the VM object now.
		 */
		if (map_size > (vm_map_size_t) ANON_MAX_SIZE) {
			/*
			 * LP64todo - for now, we can only allocate 4GB-4096
			 * internal objects because the default pager can't
			 * page bigger ones.  Remove this when it can.
			 */
			kr = KERN_FAILURE;
			goto make_mem_done;
		}

		object = vm_object_allocate(map_size);
		assert(object != VM_OBJECT_NULL);

		if (permission & MAP_MEM_PURGABLE) {
			if (! (permission & VM_PROT_WRITE)) {
				/* if we can't write, we can't purge */
				vm_object_deallocate(object);
				kr = KERN_INVALID_ARGUMENT;
				goto make_mem_done;
			}
			object->purgable = VM_PURGABLE_NONVOLATILE;
			assert(object->vo_purgeable_owner == NULL);
			assert(object->resident_page_count == 0);
			assert(object->wired_page_count == 0);
			vm_object_lock(object);
			vm_purgeable_nonvolatile_enqueue(object,
							 current_task());
			vm_object_unlock(object);
		}

		/*
		 * The VM object is brand new and nobody else knows about it,
		 * so we don't need to lock it.
		 */

		wimg_mode = object->wimg_bits;
		if (access == MAP_MEM_IO) {
			wimg_mode = VM_WIMG_IO;
		} else if (access == MAP_MEM_COPYBACK) {
			wimg_mode = VM_WIMG_USE_DEFAULT;
		} else if (access == MAP_MEM_INNERWBACK) {
			wimg_mode = VM_WIMG_INNERWBACK;
		} else if (access == MAP_MEM_WTHRU) {
			wimg_mode = VM_WIMG_WTHRU;
		} else if (access == MAP_MEM_WCOMB) {
			wimg_mode = VM_WIMG_WCOMB;
		}
		if (access != MAP_MEM_NOOP) {
			object->wimg_bits = wimg_mode;
		}
		/* the object has no pages, so no WIMG bits to update here */

		/*
		 * XXX
		 * We use this path when we want to make sure that
		 * nobody messes with the object (coalesce, for
		 * example) before we map it.
		 * We might want to use these objects for transposition via
		 * vm_object_transpose() too, so we don't want any copy or
		 * shadow objects either...
		 */
		object->copy_strategy = MEMORY_OBJECT_COPY_NONE;
		object->true_share = TRUE;

		user_entry->backing.object = object;
		user_entry->internal = TRUE;
		user_entry->is_sub_map = FALSE;
		user_entry->is_pager = FALSE;
		user_entry->offset = 0;
		user_entry->data_offset = 0;
		user_entry->protection = protections;
		SET_MAP_MEM(access, user_entry->protection);
		user_entry->size = map_size;

		/* user_object pager and internal fields are not used */
		/* when the object field is filled in.		      */

		*size = CAST_DOWN(vm_size_t, (user_entry->size -
					      user_entry->data_offset));
		*object_handle = user_handle;
		return KERN_SUCCESS;
	}

	if (permission & MAP_MEM_VM_COPY) {
		vm_map_copy_t	copy;

		if (target_map == VM_MAP_NULL) {
			return KERN_INVALID_TASK;
		}

		map_end = vm_map_round_page(offset + *size, PAGE_MASK);
		map_size = map_end - map_start;
		if (use_data_addr || use_4K_compat) {
			offset_in_page = offset - map_start;
			if (use_4K_compat)
				offset_in_page &= ~((signed)(0xFFF));
		} else {
			offset_in_page = 0;
		}

		kr = vm_map_copyin_internal(target_map,
					    map_start,
					    map_size,
					    VM_MAP_COPYIN_ENTRY_LIST,
					    &copy);
		if (kr != KERN_SUCCESS) {
			return kr;
		}
				   
		kr = mach_memory_entry_allocate(&user_entry, &user_handle);
		if (kr != KERN_SUCCESS) {
			vm_map_copy_discard(copy);
			return KERN_FAILURE;
		}

		user_entry->backing.copy = copy;
		user_entry->internal = FALSE;
		user_entry->is_sub_map = FALSE;
		user_entry->is_pager = FALSE;
		user_entry->is_copy = TRUE;
		user_entry->offset = 0;
		user_entry->protection = protections;
		user_entry->size = map_size;
		user_entry->data_offset = offset_in_page;

		*size = CAST_DOWN(vm_size_t, (user_entry->size -
					      user_entry->data_offset));
		*object_handle = user_handle;
		return KERN_SUCCESS;
	}

	if (permission & MAP_MEM_VM_SHARE) {
		vm_map_copy_t	copy;
		vm_prot_t	cur_prot, max_prot;

		if (target_map == VM_MAP_NULL) {
			return KERN_INVALID_TASK;
		}

		map_end = vm_map_round_page(offset + *size, PAGE_MASK);
		map_size = map_end - map_start;
		if (use_data_addr || use_4K_compat) {
			offset_in_page = offset - map_start;
			if (use_4K_compat)
				offset_in_page &= ~((signed)(0xFFF));
		} else {
			offset_in_page = 0;
		}

		kr = vm_map_copy_extract(target_map,
					 map_start,
					 map_size,
					 &copy,
					 &cur_prot,
					 &max_prot);
		if (kr != KERN_SUCCESS) {
			return kr;
		}

		if (mask_protections) {
			/*
			 * We just want as much of "original_protections" 
			 * as we can get out of the actual "cur_prot".
			 */
			protections &= cur_prot;
			if (protections == VM_PROT_NONE) {
				/* no access at all: fail */
				vm_map_copy_discard(copy);
				return KERN_PROTECTION_FAILURE;
			}
		} else {
			/*
			 * We want exactly "original_protections"
			 * out of "cur_prot".
			 */
			if ((cur_prot & protections) != protections) {
				vm_map_copy_discard(copy);
				return KERN_PROTECTION_FAILURE;
			}
		}

		kr = mach_memory_entry_allocate(&user_entry, &user_handle);
		if (kr != KERN_SUCCESS) {
			vm_map_copy_discard(copy);
			return KERN_FAILURE;
		}

		user_entry->backing.copy = copy;
		user_entry->internal = FALSE;
		user_entry->is_sub_map = FALSE;
		user_entry->is_pager = FALSE;
		user_entry->is_copy = TRUE;
		user_entry->offset = 0;
		user_entry->protection = protections;
		user_entry->size = map_size;
		user_entry->data_offset = offset_in_page;

		*size = CAST_DOWN(vm_size_t, (user_entry->size -
					      user_entry->data_offset));
		*object_handle = user_handle;
		return KERN_SUCCESS;
	}

	if (parent_entry == NULL ||
	    (permission & MAP_MEM_NAMED_REUSE)) {

		map_end = vm_map_round_page(offset + *size, PAGE_MASK);
		map_size = map_end - map_start;
		if (use_data_addr || use_4K_compat) {
			offset_in_page = offset - map_start;
			if (use_4K_compat)
				offset_in_page &= ~((signed)(0xFFF));
		} else {
			offset_in_page = 0;
		}

		/* Create a named object based on address range within the task map */
		/* Go find the object at given address */

		if (target_map == VM_MAP_NULL) {
			return KERN_INVALID_TASK;
		}

redo_lookup:
		protections = original_protections;
		vm_map_lock_read(target_map);

		/* get the object associated with the target address */
		/* note we check the permission of the range against */
		/* that requested by the caller */

		kr = vm_map_lookup_locked(&target_map, map_start, 
					  protections | mask_protections,
					  OBJECT_LOCK_EXCLUSIVE, &version,
					  &object, &obj_off, &prot, &wired,
					  &fault_info,
					  &real_map);
		if (kr != KERN_SUCCESS) {
			vm_map_unlock_read(target_map);
			goto make_mem_done;
		}
		if (mask_protections) {
			/*
			 * The caller asked us to use the "protections" as
			 * a mask, so restrict "protections" to what this
			 * mapping actually allows.
			 */
			protections &= prot;
		}
		if (((prot & protections) != protections) 
					|| (object == kernel_object)) {
			kr = KERN_INVALID_RIGHT;
			vm_object_unlock(object);
			vm_map_unlock_read(target_map);
			if(real_map != target_map)
				vm_map_unlock_read(real_map);
			if(object == kernel_object) {
				printf("Warning: Attempt to create a named"
					" entry from the kernel_object\n");
			}
			goto make_mem_done;
		}

		/* We have an object, now check to see if this object */
		/* is suitable.  If not, create a shadow and share that */

		/*
		 * We have to unlock the VM object to avoid deadlocking with
		 * a VM map lock (the lock ordering is map, the object), if we
		 * need to modify the VM map to create a shadow object.  Since
		 * we might release the VM map lock below anyway, we have
		 * to release the VM map lock now.
		 * XXX FBDP There must be a way to avoid this double lookup...
		 *
		 * Take an extra reference on the VM object to make sure it's
		 * not going to disappear.
		 */
		vm_object_reference_locked(object); /* extra ref to hold obj */
		vm_object_unlock(object);

		local_map = original_map;
		local_offset = map_start;
		if(target_map != local_map) {
			vm_map_unlock_read(target_map);
			if(real_map != target_map)
				vm_map_unlock_read(real_map);
			vm_map_lock_read(local_map);
			target_map = local_map;
			real_map = local_map;
		}
		while(TRUE) {
		   if(!vm_map_lookup_entry(local_map, 
						local_offset, &map_entry)) {
			kr = KERN_INVALID_ARGUMENT;
                        vm_map_unlock_read(target_map);
			if(real_map != target_map)
				vm_map_unlock_read(real_map);
                        vm_object_deallocate(object); /* release extra ref */
			object = VM_OBJECT_NULL;
                        goto make_mem_done;
		   }
		   iskernel = (local_map->pmap == kernel_pmap);
		   if(!(map_entry->is_sub_map)) {
		      if (VME_OBJECT(map_entry) != object) {
			 kr = KERN_INVALID_ARGUMENT;
                         vm_map_unlock_read(target_map);
			 if(real_map != target_map)
				vm_map_unlock_read(real_map);
                         vm_object_deallocate(object); /* release extra ref */
			 object = VM_OBJECT_NULL;
                         goto make_mem_done;
	              }
		      break;
		   } else {
			vm_map_t	tmap;
			tmap = local_map;
			local_map = VME_SUBMAP(map_entry);
			
			vm_map_lock_read(local_map);
			vm_map_unlock_read(tmap);
			target_map = local_map;
			real_map = local_map;
			local_offset = local_offset - map_entry->vme_start;
			local_offset += VME_OFFSET(map_entry);
		   }
		}

		/*
		 * We found the VM map entry, lock the VM object again.
		 */
		vm_object_lock(object);
		if(map_entry->wired_count) {
			 /* JMM - The check below should be reworked instead. */
			 object->true_share = TRUE;
		      }
		if (mask_protections) {
			/*
			 * The caller asked us to use the "protections" as
			 * a mask, so restrict "protections" to what this
			 * mapping actually allows.
			 */
			protections &= map_entry->max_protection;
		}
		if(((map_entry->max_protection) & protections) != protections) {
			 kr = KERN_INVALID_RIGHT;
                         vm_object_unlock(object);
                         vm_map_unlock_read(target_map);
			 if(real_map != target_map)
				vm_map_unlock_read(real_map);
			 vm_object_deallocate(object);
			 object = VM_OBJECT_NULL;
                         goto make_mem_done;
		}

		mappable_size = fault_info.hi_offset - obj_off;
		total_size = map_entry->vme_end - map_entry->vme_start;
		if(map_size > mappable_size) {
			/* try to extend mappable size if the entries */
			/* following are from the same object and are */
			/* compatible */
			next_entry = map_entry->vme_next;
			/* lets see if the next map entry is still   */
			/* pointing at this object and is contiguous */
			while(map_size > mappable_size) {
				if ((VME_OBJECT(next_entry) == object) &&
				    (next_entry->vme_start == 
				     next_entry->vme_prev->vme_end) &&
				    (VME_OFFSET(next_entry) == 
				     (VME_OFFSET(next_entry->vme_prev) + 
				      (next_entry->vme_prev->vme_end - 
				       next_entry->vme_prev->vme_start)))) {
					if (mask_protections) {
						/*
						 * The caller asked us to use
						 * the "protections" as a mask,
						 * so restrict "protections" to
						 * what this mapping actually
						 * allows.
						 */
						protections &= next_entry->max_protection;
					}
					if ((next_entry->wired_count) &&
					    (map_entry->wired_count == 0)) {
						break;
					}
					if(((next_entry->max_protection) 
						& protections) != protections) {
			 			break;
					}
					if (next_entry->needs_copy !=
					    map_entry->needs_copy)
						break;
					mappable_size += next_entry->vme_end
						- next_entry->vme_start;
					total_size += next_entry->vme_end
						- next_entry->vme_start;
					next_entry = next_entry->vme_next;
				} else {
					break;
				}
			
			}
		}

		/* vm_map_entry_should_cow_for_true_share() checks for malloc tags,
		 * never true in kernel */ 
		if (!iskernel && vm_map_entry_should_cow_for_true_share(map_entry) &&
		    object->vo_size > map_size &&
		    map_size != 0) {
			/*
			 * Set up the targeted range for copy-on-write to
			 * limit the impact of "true_share"/"copy_delay" to
			 * that range instead of the entire VM object...
			 */
			
			vm_object_unlock(object);
			if (vm_map_lock_read_to_write(target_map)) {
				vm_object_deallocate(object);
				target_map = original_map;
				goto redo_lookup;
			}

			vm_map_clip_start(target_map,
					  map_entry,
					  vm_map_trunc_page(map_start,
							    VM_MAP_PAGE_MASK(target_map)));
			vm_map_clip_end(target_map,
					map_entry,
					(vm_map_round_page(map_end,
							   VM_MAP_PAGE_MASK(target_map))));
			force_shadow = TRUE;

			if ((map_entry->vme_end - offset) < map_size) {
				map_size = map_entry->vme_end - map_start;
			}
			total_size = map_entry->vme_end - map_entry->vme_start;

			vm_map_lock_write_to_read(target_map);
			vm_object_lock(object);
		}

		if (object->internal) {
	   		/* vm_map_lookup_locked will create a shadow if   */
		 	/* needs_copy is set but does not check for the   */
			/* other two conditions shown. It is important to */ 
			/* set up an object which will not be pulled from */
			/* under us.  */

	      		if (force_shadow ||
			    ((map_entry->needs_copy  ||
			      object->shadowed ||
			      (object->vo_size > total_size &&
			       (VME_OFFSET(map_entry) != 0 ||
				object->vo_size >
				vm_map_round_page(total_size,
						  VM_MAP_PAGE_MASK(target_map)))))
			     && !object->true_share)) {
				/*
				 * We have to unlock the VM object before
				 * trying to upgrade the VM map lock, to
				 * honor lock ordering (map then object).
				 * Otherwise, we would deadlock if another
				 * thread holds a read lock on the VM map and
				 * is trying to acquire the VM object's lock.
				 * We still hold an extra reference on the
				 * VM object, guaranteeing that it won't
				 * disappear.
				 */
				vm_object_unlock(object);

		   		if (vm_map_lock_read_to_write(target_map)) {
					/*
					 * We couldn't upgrade our VM map lock
					 * from "read" to "write" and we lost
					 * our "read" lock.
					 * Start all over again...
					 */
					vm_object_deallocate(object); /* extra ref */
					target_map = original_map;
		            		goto redo_lookup;
		   		}
#if 00
				vm_object_lock(object);
#endif

				/* 
				 * JMM - We need to avoid coming here when the object
				 * is wired by anybody, not just the current map.  Why
				 * couldn't we use the standard vm_object_copy_quickly()
				 * approach here?
				 */
				 
		   		/* create a shadow object */
				VME_OBJECT_SHADOW(map_entry, total_size);
				shadow_object = VME_OBJECT(map_entry);
#if 00
				vm_object_unlock(object);
#endif

				prot = map_entry->protection & ~VM_PROT_WRITE;

				if (override_nx(target_map,
						VME_ALIAS(map_entry))
				    && prot)
				        prot |= VM_PROT_EXECUTE;

				vm_object_pmap_protect(
					object, VME_OFFSET(map_entry),
					total_size,
					((map_entry->is_shared 
					  || target_map->mapped_in_other_pmaps)
							? PMAP_NULL :
							target_map->pmap),
					map_entry->vme_start,
					prot);
				total_size -= (map_entry->vme_end 
						- map_entry->vme_start);
				next_entry = map_entry->vme_next;
				map_entry->needs_copy = FALSE;

				vm_object_lock(shadow_object);
				while (total_size) {
				    assert((next_entry->wired_count == 0) ||
					   (map_entry->wired_count));

				    if (VME_OBJECT(next_entry) == object) {
					vm_object_reference_locked(shadow_object);
					VME_OBJECT_SET(next_entry,
						       shadow_object);
					vm_object_deallocate(object);
					VME_OFFSET_SET(
						next_entry,
						(VME_OFFSET(next_entry->vme_prev) +
						 (next_entry->vme_prev->vme_end 
						  - next_entry->vme_prev->vme_start)));
						next_entry->needs_copy = FALSE;
					} else {
						panic("mach_make_memory_entry_64:"
						  " map entries out of sync\n");
					}
					total_size -= 
						next_entry->vme_end 
							- next_entry->vme_start;
					next_entry = next_entry->vme_next;
				}

				/*
				 * Transfer our extra reference to the
				 * shadow object.
				 */
				vm_object_reference_locked(shadow_object);
				vm_object_deallocate(object); /* extra ref */
				object = shadow_object;

				obj_off = ((local_offset - map_entry->vme_start)
					   + VME_OFFSET(map_entry));

				vm_map_lock_write_to_read(target_map);
	        	}
	   	}

		/* note: in the future we can (if necessary) allow for  */
		/* memory object lists, this will better support        */
		/* fragmentation, but is it necessary?  The user should */
		/* be encouraged to create address space oriented       */
		/* shared objects from CLEAN memory regions which have  */
		/* a known and defined history.  i.e. no inheritence    */
		/* share, make this call before making the region the   */
		/* target of ipc's, etc.  The code above, protecting    */
		/* against delayed copy, etc. is mostly defensive.      */

		wimg_mode = object->wimg_bits;
		if(!(object->nophyscache)) {
			if(access == MAP_MEM_IO) {
				wimg_mode = VM_WIMG_IO;
			} else if (access == MAP_MEM_COPYBACK) {
				wimg_mode = VM_WIMG_USE_DEFAULT;
			} else if (access == MAP_MEM_INNERWBACK) {
				wimg_mode = VM_WIMG_INNERWBACK;
			} else if (access == MAP_MEM_WTHRU) {
				wimg_mode = VM_WIMG_WTHRU;
			} else if (access == MAP_MEM_WCOMB) {
				wimg_mode = VM_WIMG_WCOMB;
			}
		}

#if VM_OBJECT_TRACKING_OP_TRUESHARE
		if (!object->true_share &&
		    vm_object_tracking_inited) {
			void *bt[VM_OBJECT_TRACKING_BTDEPTH];
			int num = 0;

			num = OSBacktrace(bt,
					  VM_OBJECT_TRACKING_BTDEPTH);
			btlog_add_entry(vm_object_tracking_btlog,
					object,
					VM_OBJECT_TRACKING_OP_TRUESHARE,
					bt,
					num);
		}
#endif /* VM_OBJECT_TRACKING_OP_TRUESHARE */

		object->true_share = TRUE;
		if (object->copy_strategy == MEMORY_OBJECT_COPY_SYMMETRIC)
			object->copy_strategy = MEMORY_OBJECT_COPY_DELAY;

		/*
		 * The memory entry now points to this VM object and we
		 * need to hold a reference on the VM object.  Use the extra
		 * reference we took earlier to keep the object alive when we
		 * had to unlock it.
		 */

		vm_map_unlock_read(target_map);
		if(real_map != target_map)
			vm_map_unlock_read(real_map);

		if (object->wimg_bits != wimg_mode)
			vm_object_change_wimg_mode(object, wimg_mode);

		/* the size of mapped entry that overlaps with our region */
		/* which is targeted for share.                           */
		/* (entry_end - entry_start) -                            */
		/*                   offset of our beg addr within entry  */
		/* it corresponds to this:                                */

		if(map_size > mappable_size)
			map_size = mappable_size;

		if (permission & MAP_MEM_NAMED_REUSE) {
			/*
			 * Compare what we got with the "parent_entry".
			 * If they match, re-use the "parent_entry" instead
			 * of creating a new one.
			 */
			if (parent_entry != NULL &&
			    parent_entry->backing.object == object &&
			    parent_entry->internal == object->internal &&
			    parent_entry->is_sub_map == FALSE &&
			    parent_entry->is_pager == FALSE &&
			    parent_entry->offset == obj_off &&
			    parent_entry->protection == protections &&
			    parent_entry->size == map_size &&
			    ((!(use_data_addr || use_4K_compat) &&
			      (parent_entry->data_offset == 0)) ||  
			     ((use_data_addr || use_4K_compat) &&
			      (parent_entry->data_offset == offset_in_page)))) {
				/*
				 * We have a match: re-use "parent_entry".
				 */
				/* release our extra reference on object */
				vm_object_unlock(object);
				vm_object_deallocate(object);
				/* parent_entry->ref_count++; XXX ? */
				/* Get an extra send-right on handle */
				ipc_port_copy_send(parent_handle);

				*size = CAST_DOWN(vm_size_t,
						  (parent_entry->size -
						   parent_entry->data_offset));
				*object_handle = parent_handle;
				return KERN_SUCCESS;
			} else {
				/*
				 * No match: we need to create a new entry.
				 * fall through...
				 */
			}
		}

		vm_object_unlock(object);
		if (mach_memory_entry_allocate(&user_entry, &user_handle)
		    != KERN_SUCCESS) {
			/* release our unused reference on the object */
			vm_object_deallocate(object);
			return KERN_FAILURE;
		}

		user_entry->backing.object = object;
		user_entry->internal = object->internal;
		user_entry->is_sub_map = FALSE;
		user_entry->is_pager = FALSE;
		user_entry->offset = obj_off;
		user_entry->data_offset = offset_in_page;
		user_entry->protection = protections;
		SET_MAP_MEM(GET_MAP_MEM(permission), user_entry->protection);
		user_entry->size = map_size;

		/* user_object pager and internal fields are not used */
		/* when the object field is filled in.		      */

		*size = CAST_DOWN(vm_size_t, (user_entry->size -
					      user_entry->data_offset));
		*object_handle = user_handle;
		return KERN_SUCCESS;

	} else {
		/* The new object will be base on an existing named object */
		if (parent_entry == NULL) {
			kr = KERN_INVALID_ARGUMENT;
			goto make_mem_done;
		}

		if (use_data_addr || use_4K_compat) {
			/*
			 * submaps and pagers should only be accessible from within
			 * the kernel, which shouldn't use the data address flag, so can fail here.
			 */
			if (parent_entry->is_pager || parent_entry->is_sub_map) {
				panic("Shouldn't be using data address with a parent entry that is a submap or pager.");
			}
			/*
			 * Account for offset to data in parent entry and
			 * compute our own offset to data.
			 */
			if((offset + *size + parent_entry->data_offset) > parent_entry->size) {
				kr = KERN_INVALID_ARGUMENT;
				goto make_mem_done;
			}

			map_start = vm_map_trunc_page(offset + parent_entry->data_offset, PAGE_MASK);
			offset_in_page = (offset + parent_entry->data_offset) - map_start;
			if (use_4K_compat)
				offset_in_page &= ~((signed)(0xFFF));
			map_end = vm_map_round_page(offset + parent_entry->data_offset + *size, PAGE_MASK);
			map_size = map_end - map_start;
		} else {
			map_end = vm_map_round_page(offset + *size, PAGE_MASK);
			map_size = map_end - map_start;
			offset_in_page = 0;

			if((offset + map_size) > parent_entry->size) {
				kr = KERN_INVALID_ARGUMENT;
				goto make_mem_done;
			}
		}

		if (mask_protections) {
			/*
			 * The caller asked us to use the "protections" as
			 * a mask, so restrict "protections" to what this
			 * mapping actually allows.
			 */
			protections &= parent_entry->protection;
		}
		if((protections & parent_entry->protection) != protections) {
			kr = KERN_PROTECTION_FAILURE;
			goto make_mem_done;
		}

		if (mach_memory_entry_allocate(&user_entry, &user_handle)
		    != KERN_SUCCESS) {
			kr = KERN_FAILURE;
			goto make_mem_done;
		}

		user_entry->size = map_size;
		user_entry->offset = parent_entry->offset + map_start;
		user_entry->data_offset = offset_in_page; 
		user_entry->is_sub_map = parent_entry->is_sub_map;
		user_entry->is_pager = parent_entry->is_pager;
		user_entry->is_copy = parent_entry->is_copy;
		user_entry->internal = parent_entry->internal;
		user_entry->protection = protections;

		if(access != MAP_MEM_NOOP) {
		   SET_MAP_MEM(access, user_entry->protection);
		}

		if(parent_entry->is_sub_map) {
		   user_entry->backing.map = parent_entry->backing.map;
		   vm_map_lock(user_entry->backing.map);
		   user_entry->backing.map->ref_count++;
		   vm_map_unlock(user_entry->backing.map);
		}
		else if (parent_entry->is_pager) {
		   user_entry->backing.pager = parent_entry->backing.pager;
		   /* JMM - don't we need a reference here? */
		} else {
		   object = parent_entry->backing.object;
		   assert(object != VM_OBJECT_NULL);
		   user_entry->backing.object = object;
		   /* we now point to this object, hold on */
		   vm_object_reference(object); 
		   vm_object_lock(object);
#if VM_OBJECT_TRACKING_OP_TRUESHARE
		if (!object->true_share &&
		    vm_object_tracking_inited) {
			void *bt[VM_OBJECT_TRACKING_BTDEPTH];
			int num = 0;

			num = OSBacktrace(bt,
					  VM_OBJECT_TRACKING_BTDEPTH);
			btlog_add_entry(vm_object_tracking_btlog,
					object,
					VM_OBJECT_TRACKING_OP_TRUESHARE,
					bt,
					num);
		}
#endif /* VM_OBJECT_TRACKING_OP_TRUESHARE */

		   object->true_share = TRUE;
		   if (object->copy_strategy == MEMORY_OBJECT_COPY_SYMMETRIC)
			object->copy_strategy = MEMORY_OBJECT_COPY_DELAY;
		   vm_object_unlock(object);
		}
		*size = CAST_DOWN(vm_size_t, (user_entry->size -
					      user_entry->data_offset));
		*object_handle = user_handle;
		return KERN_SUCCESS;
	}

make_mem_done:
	if (user_handle != IP_NULL) {
		/*
		 * Releasing "user_handle" causes the kernel object
		 * associated with it ("user_entry" here) to also be
		 * released and freed.
		 */
		mach_memory_entry_port_release(user_handle);
	}
	return kr;
}

kern_return_t
_mach_make_memory_entry(
	vm_map_t		target_map,
	memory_object_size_t	*size,
	memory_object_offset_t	offset,
	vm_prot_t		permission,
	ipc_port_t		*object_handle,
	ipc_port_t		parent_entry)
{
	memory_object_size_t 	mo_size;
	kern_return_t		kr;
	
	mo_size = (memory_object_size_t)*size;
	kr = mach_make_memory_entry_64(target_map, &mo_size, 
			(memory_object_offset_t)offset, permission, object_handle,
			parent_entry);
	*size = mo_size;
	return kr;
}

kern_return_t
mach_make_memory_entry(
	vm_map_t		target_map,
	vm_size_t		*size,
	vm_offset_t		offset,
	vm_prot_t		permission,
	ipc_port_t		*object_handle,
	ipc_port_t		parent_entry)
{	
	memory_object_size_t 	mo_size;
	kern_return_t		kr;
	
	mo_size = (memory_object_size_t)*size;
	kr = mach_make_memory_entry_64(target_map, &mo_size, 
			(memory_object_offset_t)offset, permission, object_handle,
			parent_entry);
	*size = CAST_DOWN(vm_size_t, mo_size);
	return kr;
}

/*
 *	task_wire
 *
 *	Set or clear the map's wiring_required flag.  This flag, if set,
 *	will cause all future virtual memory allocation to allocate
 *	user wired memory.  Unwiring pages wired down as a result of
 *	this routine is done with the vm_wire interface.
 */
kern_return_t
task_wire(
	vm_map_t	map,
	boolean_t	must_wire)
{
	if (map == VM_MAP_NULL)
		return(KERN_INVALID_ARGUMENT);

	if (must_wire)
		map->wiring_required = TRUE;
	else
		map->wiring_required = FALSE;

	return(KERN_SUCCESS);
}

__private_extern__ kern_return_t
mach_memory_entry_allocate(
	vm_named_entry_t	*user_entry_p,
	ipc_port_t		*user_handle_p)
{
	vm_named_entry_t	user_entry;
	ipc_port_t		user_handle;
	ipc_port_t		previous;

	user_entry = (vm_named_entry_t) kalloc(sizeof *user_entry);
	if (user_entry == NULL)
		return KERN_FAILURE;

	named_entry_lock_init(user_entry);

	user_handle = ipc_port_alloc_kernel();
	if (user_handle == IP_NULL) {
		kfree(user_entry, sizeof *user_entry);
		return KERN_FAILURE;
	}
	ip_lock(user_handle);

	/* make a sonce right */
	user_handle->ip_sorights++;
	ip_reference(user_handle);

	user_handle->ip_destination = IP_NULL;
	user_handle->ip_receiver_name = MACH_PORT_NULL;
	user_handle->ip_receiver = ipc_space_kernel;

	/* make a send right */
        user_handle->ip_mscount++;
        user_handle->ip_srights++;
        ip_reference(user_handle);

	ipc_port_nsrequest(user_handle, 1, user_handle, &previous);
	/* nsrequest unlocks user_handle */

	user_entry->backing.pager = NULL;
	user_entry->is_sub_map = FALSE;
	user_entry->is_pager = FALSE;
	user_entry->is_copy = FALSE;
	user_entry->internal = FALSE;
	user_entry->size = 0;
	user_entry->offset = 0;
	user_entry->data_offset = 0;
	user_entry->protection = VM_PROT_NONE;
	user_entry->ref_count = 1;

	ipc_kobject_set(user_handle, (ipc_kobject_t) user_entry,
			IKOT_NAMED_ENTRY);

	*user_entry_p = user_entry;
	*user_handle_p = user_handle;

	return KERN_SUCCESS;
}

/*
 *	mach_memory_object_memory_entry_64
 *
 *	Create a named entry backed by the provided pager.
 *
 *	JMM - we need to hold a reference on the pager -
 *	and release it when the named entry is destroyed.
 */
kern_return_t
mach_memory_object_memory_entry_64(
	host_t			host,
	boolean_t		internal,
	vm_object_offset_t	size,
	vm_prot_t		permission,
 	memory_object_t		pager,
	ipc_port_t		*entry_handle)
{
	unsigned int		access;
	vm_named_entry_t	user_entry;
	ipc_port_t		user_handle;

        if (host == HOST_NULL)
                return(KERN_INVALID_HOST);

	if (mach_memory_entry_allocate(&user_entry, &user_handle)
	    != KERN_SUCCESS) {
		return KERN_FAILURE;
	}

	user_entry->backing.pager = pager;
	user_entry->size = size;
	user_entry->offset = 0;
	user_entry->protection = permission & VM_PROT_ALL;
	access = GET_MAP_MEM(permission);
	SET_MAP_MEM(access, user_entry->protection);
	user_entry->internal = internal;
	user_entry->is_sub_map = FALSE;
	user_entry->is_pager = TRUE;
	assert(user_entry->ref_count == 1);

	*entry_handle = user_handle;
	return KERN_SUCCESS;
}	

kern_return_t
mach_memory_object_memory_entry(
	host_t		host,
	boolean_t	internal,
	vm_size_t	size,
	vm_prot_t	permission,
 	memory_object_t	pager,
	ipc_port_t	*entry_handle)
{
	return mach_memory_object_memory_entry_64( host, internal, 
		(vm_object_offset_t)size, permission, pager, entry_handle);
}


kern_return_t
mach_memory_entry_purgable_control(
	ipc_port_t	entry_port,
	vm_purgable_t	control,
	int		*state)
{
	kern_return_t		kr;
	vm_named_entry_t	mem_entry;
	vm_object_t		object;

	if (entry_port == IP_NULL ||
	    ip_kotype(entry_port) != IKOT_NAMED_ENTRY) {
		return KERN_INVALID_ARGUMENT;
	}
	if (control != VM_PURGABLE_SET_STATE &&
	    control != VM_PURGABLE_GET_STATE)
		return(KERN_INVALID_ARGUMENT);

	if (control == VM_PURGABLE_SET_STATE &&
	    (((*state & ~(VM_PURGABLE_ALL_MASKS)) != 0) ||
	     ((*state & VM_PURGABLE_STATE_MASK) > VM_PURGABLE_STATE_MASK)))
		return(KERN_INVALID_ARGUMENT);

	mem_entry = (vm_named_entry_t) entry_port->ip_kobject;

	named_entry_lock(mem_entry);

	if (mem_entry->is_sub_map ||
	    mem_entry->is_pager ||
	    mem_entry->is_copy) {
		named_entry_unlock(mem_entry);
		return KERN_INVALID_ARGUMENT;
	}

	object = mem_entry->backing.object;
	if (object == VM_OBJECT_NULL) {
		named_entry_unlock(mem_entry);
		return KERN_INVALID_ARGUMENT;
	}

	vm_object_lock(object);

	/* check that named entry covers entire object ? */
	if (mem_entry->offset != 0 || object->vo_size != mem_entry->size) {
		vm_object_unlock(object);
		named_entry_unlock(mem_entry);
		return KERN_INVALID_ARGUMENT;
	}

	named_entry_unlock(mem_entry);

	kr = vm_object_purgable_control(object, control, state);

	vm_object_unlock(object);

	return kr;
}

kern_return_t
mach_memory_entry_get_page_counts(
	ipc_port_t	entry_port,
	unsigned int	*resident_page_count,
	unsigned int	*dirty_page_count)
{
<<<<<<< HEAD
	kern_return_t		kr;
	vm_named_entry_t	mem_entry;
	vm_object_t		object;
	vm_object_offset_t	offset;
	vm_object_size_t	size;

	if (entry_port == IP_NULL ||
	    ip_kotype(entry_port) != IKOT_NAMED_ENTRY) {
		return KERN_INVALID_ARGUMENT;
	}
=======
	vm_map_entry_t	entry;
	int		caller_flags;
	int		sync_cow_data = FALSE;
	vm_object_t	local_object;
	vm_offset_t	local_offset;
	vm_offset_t	local_start;
	kern_return_t	ret;

	caller_flags = *flags;
	if (!(caller_flags & UPL_COPYOUT_FROM)) {
		sync_cow_data = TRUE;
	}
	if(upl == NULL)
		return KERN_INVALID_ARGUMENT;
REDISCOVER_ENTRY:
	vm_map_lock(map);
	if (vm_map_lookup_entry(map, offset, &entry)) {
		if((entry->vme_end - offset) < *upl_size) {
			*upl_size = entry->vme_end - offset;
		}
		/*
		 *      Create an object if necessary.
		 */
		if (entry->object.vm_object == VM_OBJECT_NULL) {
			entry->object.vm_object = vm_object_allocate(
				(vm_size_t)(entry->vme_end - entry->vme_start));
			entry->offset = 0;
		}
		if (!(caller_flags & UPL_COPYOUT_FROM)) {
			if (entry->needs_copy
				    || entry->object.vm_object->copy) {
				vm_map_t		local_map;
				vm_object_t		object;
				vm_object_offset_t	offset_hi;
				vm_object_offset_t	offset_lo;
				vm_object_offset_t	new_offset;
				vm_prot_t		prot;
				boolean_t		wired;
				vm_behavior_t		behavior;
				vm_map_version_t	 version;
				vm_map_t		pmap_map;

				local_map = map;
				vm_map_lock_write_to_read(map);
				if(vm_map_lookup_locked(&local_map,
					offset, VM_PROT_WRITE,
					&version, &object,
					&new_offset, &prot, &wired,
					&behavior, &offset_lo,
					&offset_hi, &pmap_map)) {
					vm_map_unlock(local_map);
					return KERN_FAILURE;
				}
				if (pmap_map != map) {
					vm_map_unlock(pmap_map);
				}
				vm_object_unlock(object);
				vm_map_unlock(local_map);

				goto REDISCOVER_ENTRY;
			}
		}
		if (entry->is_sub_map) {
			vm_map_t	submap;

			submap = entry->object.sub_map;
			local_start = entry->vme_start;
			local_offset = entry->offset;
			vm_map_reference(submap);
			vm_map_unlock(map);

			ret = (vm_map_get_upl(submap, 
				local_offset + (offset - local_start), 
				upl_size, upl, page_list, count, 
				flags, force_data_sync));

			vm_map_deallocate(submap);
			return ret;
		}
					
		if (sync_cow_data) {
			if (entry->object.vm_object->shadow) {
				int		flags;

				local_object = entry->object.vm_object;
				local_start = entry->vme_start;
				local_offset = entry->offset;
				vm_object_reference(local_object);
				vm_map_unlock(map);

				if(local_object->copy == NULL) {
					flags = MEMORY_OBJECT_DATA_SYNC;
				} else {
					flags = MEMORY_OBJECT_COPY_SYNC;
				}

				if((local_object->paging_offset) &&
						(local_object->pager == 0)) {
				   /* 
				    * do a little clean-up for our unorthodox
				    * entry into a pager call from a non-pager
				    * context.  Normally the pager code 
				    * assumes that an object it has been called
				    * with has a backing pager and so does
				    * not bother to check the pager field
				    * before relying on the paging_offset
				    */
				    vm_object_lock(local_object);
				    if (local_object->pager == 0) {
					local_object->paging_offset = 0;
				    }
				    vm_object_unlock(local_object);
				}
					
				memory_object_lock_request(
					local_object, ((offset - local_start) 
						+ local_offset) +
						local_object->paging_offset,
					(vm_object_size_t)*upl_size, FALSE, 
					flags,
					VM_PROT_NO_CHANGE, NULL, 0);
				sync_cow_data = FALSE;
				goto REDISCOVER_ENTRY;
			}
		}

		if (force_data_sync) {

			local_object = entry->object.vm_object;
			local_start = entry->vme_start;
			local_offset = entry->offset;
			vm_object_reference(local_object);
		        vm_map_unlock(map);

			if((local_object->paging_offset) && 
					(local_object->pager == 0)) {
			   /* 
			    * do a little clean-up for our unorthodox
			    * entry into a pager call from a non-pager
			    * context.  Normally the pager code 
			    * assumes that an object it has been called
			    * with has a backing pager and so does
			    * not bother to check the pager field
			    * before relying on the paging_offset
			    */
			    vm_object_lock(local_object);
			    if (local_object->pager == 0) {
				local_object->paging_offset = 0;
			    }
			    vm_object_unlock(local_object);
			}
					
			memory_object_lock_request(
				   local_object, ((offset - local_start) 
					   + local_offset) + 
					   local_object->paging_offset,
				   (vm_object_size_t)*upl_size, FALSE, 
				   MEMORY_OBJECT_DATA_SYNC,
				   VM_PROT_NO_CHANGE, 
				   NULL, 0);
			force_data_sync = FALSE;
			goto REDISCOVER_ENTRY;
		}

		if(!(entry->object.vm_object->private)) {
			if(*upl_size > (MAX_UPL_TRANSFER*PAGE_SIZE))
				*upl_size = (MAX_UPL_TRANSFER*PAGE_SIZE);
			if(entry->object.vm_object->phys_contiguous) {
				*flags = UPL_PHYS_CONTIG;
			} else {
				*flags = 0;
			}
		} else {
			*flags = UPL_DEV_MEMORY | UPL_PHYS_CONTIG;
		}
		local_object = entry->object.vm_object;
		local_offset = entry->offset;
		local_start = entry->vme_start;
		vm_object_reference(local_object);
		vm_map_unlock(map);
		ret = (vm_fault_list_request(local_object, 
			((offset - local_start) + local_offset),
			*upl_size,
			upl,
			page_list,
			*count,
			caller_flags));
		vm_object_deallocate(local_object);
		return(ret);
	} 
>>>>>>> origin/10.0

	mem_entry = (vm_named_entry_t) entry_port->ip_kobject;

	named_entry_lock(mem_entry);

	if (mem_entry->is_sub_map ||
	    mem_entry->is_pager ||
	    mem_entry->is_copy) {
		named_entry_unlock(mem_entry);
		return KERN_INVALID_ARGUMENT;
	}

	object = mem_entry->backing.object;
	if (object == VM_OBJECT_NULL) {
		named_entry_unlock(mem_entry);
		return KERN_INVALID_ARGUMENT;
	}

	vm_object_lock(object);

	offset = mem_entry->offset;
	size = mem_entry->size;

	named_entry_unlock(mem_entry);

	kr = vm_object_get_page_counts(object, offset, size, resident_page_count, dirty_page_count);

	vm_object_unlock(object);

	return kr;
}

/*
 * mach_memory_entry_port_release:
 *
 * Release a send right on a named entry port.  This is the correct
 * way to destroy a named entry.  When the last right on the port is
 * released, ipc_kobject_destroy() will call mach_destroy_memory_entry().
 */
void
mach_memory_entry_port_release(
	ipc_port_t	port)
{
	assert(ip_kotype(port) == IKOT_NAMED_ENTRY);
	ipc_port_release_send(port);
}

/*
 * mach_destroy_memory_entry:
 *
 * Drops a reference on a memory entry and destroys the memory entry if
 * there are no more references on it.
 * NOTE: This routine should not be called to destroy a memory entry from the
 * kernel, as it will not release the Mach port associated with the memory
 * entry.  The proper way to destroy a memory entry in the kernel is to
 * call mach_memort_entry_port_release() to release the kernel's send-right on
 * the memory entry's port.  When the last send right is released, the memory
 * entry will be destroyed via ipc_kobject_destroy().
 */
void
mach_destroy_memory_entry(
	ipc_port_t	port)
{
	vm_named_entry_t	named_entry;
#if MACH_ASSERT
	assert(ip_kotype(port) == IKOT_NAMED_ENTRY);
#endif /* MACH_ASSERT */
	named_entry = (vm_named_entry_t)port->ip_kobject;

	named_entry_lock(named_entry);
	named_entry->ref_count -= 1;

	if(named_entry->ref_count == 0) {
		if (named_entry->is_sub_map) {
			vm_map_deallocate(named_entry->backing.map);
		} else if (named_entry->is_pager) {
			/* JMM - need to drop reference on pager in that case */
		} else if (named_entry->is_copy) {
			vm_map_copy_discard(named_entry->backing.copy);
		} else {
			/* release the VM object we've been pointing to */
			vm_object_deallocate(named_entry->backing.object);
		}

		named_entry_unlock(named_entry);
		named_entry_lock_destroy(named_entry);

		kfree((void *) port->ip_kobject,
		      sizeof (struct vm_named_entry));
	} else
		named_entry_unlock(named_entry);
}

/* Allow manipulation of individual page state.  This is actually part of */
/* the UPL regimen but takes place on the memory entry rather than on a UPL */

kern_return_t
mach_memory_entry_page_op(
	ipc_port_t		entry_port,
	vm_object_offset_t	offset,
	int			ops,
	ppnum_t			*phys_entry,
	int			*flags)
{
	vm_named_entry_t	mem_entry;
	vm_object_t		object;
	kern_return_t		kr;

	if (entry_port == IP_NULL ||
	    ip_kotype(entry_port) != IKOT_NAMED_ENTRY) {
		return KERN_INVALID_ARGUMENT;
	}

	mem_entry = (vm_named_entry_t) entry_port->ip_kobject;

	named_entry_lock(mem_entry);

	if (mem_entry->is_sub_map ||
	    mem_entry->is_pager ||
	    mem_entry->is_copy) {
		named_entry_unlock(mem_entry);
		return KERN_INVALID_ARGUMENT;
	}

	object = mem_entry->backing.object;
	if (object == VM_OBJECT_NULL) {
		named_entry_unlock(mem_entry);
		return KERN_INVALID_ARGUMENT;
	}

	vm_object_reference(object);
	named_entry_unlock(mem_entry);

	kr = vm_object_page_op(object, offset, ops, phys_entry, flags);

	vm_object_deallocate(object);	

	return kr;
}

/*
 * mach_memory_entry_range_op offers performance enhancement over 
 * mach_memory_entry_page_op for page_op functions which do not require page 
 * level state to be returned from the call.  Page_op was created to provide 
 * a low-cost alternative to page manipulation via UPLs when only a single 
 * page was involved.  The range_op call establishes the ability in the _op 
 * family of functions to work on multiple pages where the lack of page level
 * state handling allows the caller to avoid the overhead of the upl structures.
 */

kern_return_t
mach_memory_entry_range_op(
	ipc_port_t		entry_port,
	vm_object_offset_t	offset_beg,
	vm_object_offset_t	offset_end,
	int                     ops,
	int                     *range)
{
	vm_named_entry_t	mem_entry;
	vm_object_t		object;
	kern_return_t		kr;

	if (entry_port == IP_NULL ||
	    ip_kotype(entry_port) != IKOT_NAMED_ENTRY) {
		return KERN_INVALID_ARGUMENT;
	}

	mem_entry = (vm_named_entry_t) entry_port->ip_kobject;

	named_entry_lock(mem_entry);

	if (mem_entry->is_sub_map ||
	    mem_entry->is_pager ||
	    mem_entry->is_copy) {
		named_entry_unlock(mem_entry);
		return KERN_INVALID_ARGUMENT;
	}

	object = mem_entry->backing.object;
	if (object == VM_OBJECT_NULL) {
		named_entry_unlock(mem_entry);
		return KERN_INVALID_ARGUMENT;
	}

	vm_object_reference(object);
	named_entry_unlock(mem_entry);

	kr = vm_object_range_op(object,
				offset_beg,
				offset_end,
				ops,
				(uint32_t *) range);

	vm_object_deallocate(object);

	return kr;
}


kern_return_t
set_dp_control_port(
	host_priv_t	host_priv,
	ipc_port_t	control_port)	
{
        if (host_priv == HOST_PRIV_NULL)
                return (KERN_INVALID_HOST);

	if (IP_VALID(dynamic_pager_control_port))
		ipc_port_release_send(dynamic_pager_control_port);

	dynamic_pager_control_port = control_port;
	return KERN_SUCCESS;
}

kern_return_t
get_dp_control_port(
	host_priv_t	host_priv,
	ipc_port_t	*control_port)	
{
        if (host_priv == HOST_PRIV_NULL)
                return (KERN_INVALID_HOST);

	*control_port = ipc_port_copy_send(dynamic_pager_control_port);
	return KERN_SUCCESS;
	
}

/* ******* Temporary Internal calls to UPL for BSD ***** */

extern int kernel_upl_map(
	vm_map_t        map,
	upl_t           upl,
	vm_offset_t     *dst_addr);

extern int kernel_upl_unmap(
	vm_map_t        map,
	upl_t           upl);

extern int kernel_upl_commit(
	upl_t                   upl,
	upl_page_info_t         *pl,
	mach_msg_type_number_t	 count);

extern int kernel_upl_commit_range(
	upl_t                   upl,
	upl_offset_t             offset,
	upl_size_t		size,
	int			flags,
	upl_page_info_array_t	pl,
	mach_msg_type_number_t	count);

extern int kernel_upl_abort(
	upl_t                   upl,
	int                     abort_type);

extern int kernel_upl_abort_range(
	upl_t                   upl,
	upl_offset_t             offset,
	upl_size_t               size,
	int                     abort_flags);


kern_return_t
kernel_upl_map(
	vm_map_t	map,
	upl_t		upl,
	vm_offset_t	*dst_addr)
{
	return vm_upl_map(map, upl, dst_addr);
}


kern_return_t
kernel_upl_unmap(
	vm_map_t	map,
	upl_t		upl)
{
	return vm_upl_unmap(map, upl);
}

kern_return_t
kernel_upl_commit(
	upl_t                   upl,
	upl_page_info_t        *pl,
	mach_msg_type_number_t  count)
{
	kern_return_t 	kr;

	kr = upl_commit(upl, pl, count);
	upl_deallocate(upl);
	return kr;
}


kern_return_t
kernel_upl_commit_range(
	upl_t 			upl,
	upl_offset_t		offset,
	upl_size_t		size,
	int			flags,
	upl_page_info_array_t   pl,
	mach_msg_type_number_t  count)
{
	boolean_t		finished = FALSE;
	kern_return_t 		kr;

	if (flags & UPL_COMMIT_FREE_ON_EMPTY)
		flags |= UPL_COMMIT_NOTIFY_EMPTY;

	if (flags & UPL_COMMIT_KERNEL_ONLY_FLAGS) {
		return KERN_INVALID_ARGUMENT;
	}

	kr = upl_commit_range(upl, offset, size, flags, pl, count, &finished);

	if ((flags & UPL_COMMIT_NOTIFY_EMPTY) && finished)
		upl_deallocate(upl);

	return kr;
}
	
kern_return_t
kernel_upl_abort_range(
	upl_t			upl,
	upl_offset_t		offset,
	upl_size_t		size,
	int			abort_flags)
{
	kern_return_t 		kr;
	boolean_t		finished = FALSE;

	if (abort_flags & UPL_COMMIT_FREE_ON_EMPTY)
		abort_flags |= UPL_COMMIT_NOTIFY_EMPTY;

	kr = upl_abort_range(upl, offset, size, abort_flags, &finished);

	if ((abort_flags & UPL_COMMIT_FREE_ON_EMPTY) && finished)
		upl_deallocate(upl);

	return kr;
}

kern_return_t
kernel_upl_abort(
	upl_t			upl,
	int			abort_type)
{
	kern_return_t	kr;

	kr = upl_abort(upl, abort_type);
	upl_deallocate(upl);
	return kr;
}

/*
 * Now a kernel-private interface (for BootCache
 * use only).  Need a cleaner way to create an
 * empty vm_map() and return a handle to it.
 */

kern_return_t
vm_region_object_create(
	__unused vm_map_t	target_map,
	vm_size_t		size,
	ipc_port_t		*object_handle)
{
	vm_named_entry_t	user_entry;
	ipc_port_t		user_handle;

	vm_map_t	new_map;
	
	if (mach_memory_entry_allocate(&user_entry, &user_handle)
	    != KERN_SUCCESS) {
		return KERN_FAILURE;
	}

	/* Create a named object based on a submap of specified size */

	new_map = vm_map_create(PMAP_NULL, VM_MAP_MIN_ADDRESS,
				vm_map_round_page(size,
						  VM_MAP_PAGE_MASK(target_map)),
				TRUE);
	vm_map_set_page_shift(new_map, VM_MAP_PAGE_SHIFT(target_map));

	user_entry->backing.map = new_map;
	user_entry->internal = TRUE;
	user_entry->is_sub_map = TRUE;
	user_entry->offset = 0;
	user_entry->protection = VM_PROT_ALL;
	user_entry->size = size;
	assert(user_entry->ref_count == 1);

<<<<<<< HEAD
	*object_handle = user_handle;
=======
	if(shared_region == NULL)
		return KERN_SUCCESS;
	shared_region_mapping_lock(shared_region);

	if((--shared_region->ref_count) == 0) {

		sm_info.text_region = shared_region->text_region;
		sm_info.text_size = shared_region->text_size;
		sm_info.data_region = shared_region->data_region;
		sm_info.data_size = shared_region->data_size;
		sm_info.region_mappings = shared_region->region_mappings;
		sm_info.client_base = shared_region->client_base;
		sm_info.alternate_base = shared_region->alternate_base;
		sm_info.alternate_next = shared_region->alternate_next;
		sm_info.flags = shared_region->flags;
		sm_info.self = (vm_offset_t)shared_region;

		lsf_remove_regions_mappings(shared_region, &sm_info);
		pmap_remove(((vm_named_entry_t)
			(shared_region->text_region->ip_kobject))
						->backing.map->pmap, 
			sm_info.client_base, 
			sm_info.client_base + sm_info.text_size);
		ipc_port_release_send(shared_region->text_region);
		ipc_port_release_send(shared_region->data_region);
		if(shared_region->object_chain) {
			shared_region_mapping_dealloc(
			     shared_region->object_chain->object_chain_region);
			kfree((vm_offset_t)shared_region->object_chain,
				sizeof (struct shared_region_object_chain));
		}
		kfree((vm_offset_t)shared_region,
				sizeof (struct shared_region_mapping));
		return KERN_SUCCESS;
	}
	shared_region_mapping_unlock(shared_region);
>>>>>>> origin/10.0
	return KERN_SUCCESS;

}

ppnum_t vm_map_get_phys_page(		/* forward */
	vm_map_t	map,
	vm_offset_t	offset);

ppnum_t
vm_map_get_phys_page(
	vm_map_t		map,
	vm_offset_t		addr)
{
	vm_object_offset_t	offset;
	vm_object_t		object;
	vm_map_offset_t 	map_offset;
	vm_map_entry_t		entry;
	ppnum_t			phys_page = 0;

	map_offset = vm_map_trunc_page(addr, PAGE_MASK);

	vm_map_lock(map);
	while (vm_map_lookup_entry(map, map_offset, &entry)) {

		if (VME_OBJECT(entry) == VM_OBJECT_NULL) {
			vm_map_unlock(map);
			return (ppnum_t) 0;
		}
		if (entry->is_sub_map) {
			vm_map_t	old_map;
			vm_map_lock(VME_SUBMAP(entry));
			old_map = map;
			map = VME_SUBMAP(entry);
			map_offset = (VME_OFFSET(entry) +
				      (map_offset - entry->vme_start));
			vm_map_unlock(old_map);
			continue;
		}
		if (VME_OBJECT(entry)->phys_contiguous) {
			/* These are  not standard pageable memory mappings */
			/* If they are not present in the object they will  */
			/* have to be picked up from the pager through the  */
			/* fault mechanism.  */
			if (VME_OBJECT(entry)->vo_shadow_offset == 0) {
				/* need to call vm_fault */
				vm_map_unlock(map);
				vm_fault(map, map_offset, VM_PROT_NONE, 
					FALSE, THREAD_UNINT, NULL, 0);
				vm_map_lock(map);
				continue;
			}
			offset = (VME_OFFSET(entry) +
				  (map_offset - entry->vme_start));
			phys_page = (ppnum_t)
				((VME_OBJECT(entry)->vo_shadow_offset 
				  + offset) >> PAGE_SHIFT);
			break;
			
		}
		offset = (VME_OFFSET(entry) + (map_offset - entry->vme_start));
		object = VME_OBJECT(entry);
		vm_object_lock(object);
		while (TRUE) {
			vm_page_t dst_page = vm_page_lookup(object,offset);
	                if(dst_page == VM_PAGE_NULL) {
				if(object->shadow) {
					vm_object_t old_object;
					vm_object_lock(object->shadow);
					old_object = object;
					offset = offset + object->vo_shadow_offset;
					object = object->shadow;
					vm_object_unlock(old_object);
				} else {
					vm_object_unlock(object);
					break;
				}
			} else {
				phys_page = (ppnum_t)(dst_page->phys_page);
				vm_object_unlock(object);
				break;
			}
		}
		break;

	} 

	vm_map_unlock(map);
	return phys_page;
}


#if 0
kern_return_t kernel_object_iopl_request(	/* forward */
	vm_named_entry_t	named_entry,
	memory_object_offset_t	offset,
	upl_size_t		*upl_size,
	upl_t			*upl_ptr,
	upl_page_info_array_t	user_page_list,
	unsigned int		*page_list_count,
	int			*flags);

kern_return_t
kernel_object_iopl_request(
	vm_named_entry_t	named_entry,
	memory_object_offset_t	offset,
	upl_size_t		*upl_size,
	upl_t			*upl_ptr,
	upl_page_info_array_t	user_page_list,
	unsigned int		*page_list_count,
	int			*flags)
{
	vm_object_t		object;
	kern_return_t		ret;

	int			caller_flags;

	caller_flags = *flags;

	if (caller_flags & ~UPL_VALID_FLAGS) {
		/*
		 * For forward compatibility's sake,
		 * reject any unknown flag.
		 */
		return KERN_INVALID_VALUE;
	}

	/* a few checks to make sure user is obeying rules */
	if(*upl_size == 0) {
		if(offset >= named_entry->size)
			return(KERN_INVALID_RIGHT);
		*upl_size = (upl_size_t) (named_entry->size - offset);
		if (*upl_size != named_entry->size - offset)
			return KERN_INVALID_ARGUMENT;
	}
	if(caller_flags & UPL_COPYOUT_FROM) {
		if((named_entry->protection & VM_PROT_READ) 
					!= VM_PROT_READ) {
			return(KERN_INVALID_RIGHT);
		}
	} else {
		if((named_entry->protection & 
			(VM_PROT_READ | VM_PROT_WRITE)) 
			!= (VM_PROT_READ | VM_PROT_WRITE)) {
			return(KERN_INVALID_RIGHT);
		}
	}
	if(named_entry->size < (offset + *upl_size))
		return(KERN_INVALID_ARGUMENT);

	/* the callers parameter offset is defined to be the */
	/* offset from beginning of named entry offset in object */
	offset = offset + named_entry->offset;

	if (named_entry->is_sub_map ||
	    named_entry->is_copy)
		return KERN_INVALID_ARGUMENT;
		
	named_entry_lock(named_entry);

	if (named_entry->is_pager) {
		object = vm_object_enter(named_entry->backing.pager, 
				named_entry->offset + named_entry->size, 
				named_entry->internal, 
				FALSE,
				FALSE);
		if (object == VM_OBJECT_NULL) {
			named_entry_unlock(named_entry);
			return(KERN_INVALID_OBJECT);
		}

		/* JMM - drop reference on the pager here? */

		/* create an extra reference for the object */
		vm_object_lock(object);
		vm_object_reference_locked(object);
		named_entry->backing.object = object;
		named_entry->is_pager = FALSE;
		named_entry_unlock(named_entry);

		/* wait for object (if any) to be ready */
		if (!named_entry->internal) {
			while (!object->pager_ready) {
				vm_object_wait(object,
					       VM_OBJECT_EVENT_PAGER_READY,
					       THREAD_UNINT);
				vm_object_lock(object);
			}
		}
		vm_object_unlock(object);

	} else {
		/* This is the case where we are going to operate */
		/* an an already known object.  If the object is */
		/* not ready it is internal.  An external     */
		/* object cannot be mapped until it is ready  */
		/* we can therefore avoid the ready check     */
		/* in this case.  */
		object = named_entry->backing.object;
		vm_object_reference(object);
		named_entry_unlock(named_entry);
	}

	if (!object->private) {
		if (*upl_size > MAX_UPL_TRANSFER_BYTES)
			*upl_size = MAX_UPL_TRANSFER_BYTES;
		if (object->phys_contiguous) {
			*flags = UPL_PHYS_CONTIG;
		} else {
			*flags = 0;
		}
	} else {
		*flags = UPL_DEV_MEMORY | UPL_PHYS_CONTIG;
	}

	ret = vm_object_iopl_request(object,
				     offset,
				     *upl_size,
				     upl_ptr,
				     user_page_list,
				     page_list_count,
				     (upl_control_flags_t)(unsigned int)caller_flags);
	vm_object_deallocate(object);
	return ret;
}
#endif
