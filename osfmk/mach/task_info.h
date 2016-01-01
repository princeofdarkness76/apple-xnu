/*
 * Copyright (c) 2000-2007, 2015 Apple Inc. All rights reserved.
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
 * Copyright (c) 1991,1990,1989,1988,1987 Carnegie Mellon University
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
 *	Machine-independent task information structures and definitions.
 *
 *	The definitions in this file are exported to the user.  The kernel
 *	will translate its internal data structures to these structures
 *	as appropriate.
 *
 */

#ifndef	_MACH_TASK_INFO_H_
#define	_MACH_TASK_INFO_H_

#include <mach/message.h>
#include <mach/machine/vm_types.h>
#include <mach/time_value.h>
#include <mach/policy.h>
#include <mach/vm_statistics.h> /* for vm_extmod_statistics_data_t */
#include <Availability.h>

#include <sys/cdefs.h>

/*
 *	Generic information structure to allow for expansion.
 */
typedef	natural_t	task_flavor_t;
typedef	integer_t	*task_info_t;		/* varying array of int */

/* Deprecated, use per structure _data_t's instead */
#define	TASK_INFO_MAX	(1024)		/* maximum array size */
typedef	integer_t	task_info_data_t[TASK_INFO_MAX];

/*
 *	Currently defined information structures.
 */

#pragma pack(4)

/* Don't use this, use MACH_TASK_BASIC_INFO instead */
#define TASK_BASIC_INFO_32      4       /* basic information */
#define TASK_BASIC2_INFO_32      6

struct task_basic_info_32 {
        integer_t       suspend_count;  /* suspend count for task */
        natural_t       virtual_size;   /* virtual memory size (bytes) */
        natural_t       resident_size;  /* resident memory size (bytes) */
        time_value_t    user_time;      /* total user run time for
                                           terminated threads */
        time_value_t    system_time;    /* total system run time for
                                           terminated threads */
	policy_t	policy;		/* default policy for new threads */
};
typedef struct task_basic_info_32       task_basic_info_32_data_t;
typedef struct task_basic_info_32       *task_basic_info_32_t;
#define TASK_BASIC_INFO_32_COUNT   \
                (sizeof(task_basic_info_32_data_t) / sizeof(natural_t))

/* Don't use this, use MACH_TASK_BASIC_INFO instead */
struct task_basic_info_64 {
        integer_t       suspend_count;  /* suspend count for task */
        mach_vm_size_t  virtual_size;   /* virtual memory size (bytes) */
        mach_vm_size_t  resident_size;  /* resident memory size (bytes) */
        time_value_t    user_time;      /* total user run time for
                                           terminated threads */
        time_value_t    system_time;    /* total system run time for
                                           terminated threads */
	policy_t	policy;		/* default policy for new threads */
};
typedef struct task_basic_info_64       task_basic_info_64_data_t;
typedef struct task_basic_info_64       *task_basic_info_64_t;

#define TASK_BASIC_INFO_64      5       /* 64-bit capable basic info */
#define TASK_BASIC_INFO_64_COUNT   \
                (sizeof(task_basic_info_64_data_t) / sizeof(natural_t))


/* localized structure - cannot be safely passed between tasks of differing sizes */
/* Don't use this, use MACH_TASK_BASIC_INFO instead */
struct task_basic_info {
        integer_t       suspend_count;  /* suspend count for task */
        vm_size_t       virtual_size;   /* virtual memory size (bytes) */
        vm_size_t       resident_size;  /* resident memory size (bytes) */
        time_value_t    user_time;      /* total user run time for
                                           terminated threads */
        time_value_t    system_time;    /* total system run time for
                                           terminated threads */
	policy_t	policy;		/* default policy for new threads */
};

typedef struct task_basic_info          task_basic_info_data_t;
typedef struct task_basic_info          *task_basic_info_t;
#define TASK_BASIC_INFO_COUNT   \
                (sizeof(task_basic_info_data_t) / sizeof(natural_t))
#if !defined(__LP64__)
#define TASK_BASIC_INFO TASK_BASIC_INFO_32
#else
#define TASK_BASIC_INFO TASK_BASIC_INFO_64
#endif



#define	TASK_EVENTS_INFO	2	/* various event counts */

struct task_events_info {
	integer_t	faults;		/* number of page faults */
	integer_t 	pageins;	/* number of actual pageins */
	integer_t 	cow_faults;	/* number of copy-on-write faults */
	integer_t 	messages_sent;	/* number of messages sent */
	integer_t 	messages_received; /* number of messages received */
        integer_t 	syscalls_mach;  /* number of mach system calls */
	integer_t 	syscalls_unix;  /* number of unix system calls */
	integer_t 	csw;            /* number of context switches */
};
typedef struct task_events_info		task_events_info_data_t;
typedef struct task_events_info		*task_events_info_t;
#define	TASK_EVENTS_INFO_COUNT		((mach_msg_type_number_t) \
		(sizeof(task_events_info_data_t) / sizeof(natural_t)))

#define	TASK_THREAD_TIMES_INFO	3	/* total times for live threads -
					   only accurate if suspended */

struct task_thread_times_info {
	time_value_t	user_time;	/* total user run time for
					   live threads */
	time_value_t	system_time;	/* total system run time for
					   live threads */
};

typedef struct task_thread_times_info	task_thread_times_info_data_t;
typedef struct task_thread_times_info	*task_thread_times_info_t;
#define	TASK_THREAD_TIMES_INFO_COUNT	((mach_msg_type_number_t) \
		(sizeof(task_thread_times_info_data_t) / sizeof(natural_t)))

#define TASK_ABSOLUTETIME_INFO	1

struct task_absolutetime_info {
	uint64_t		total_user;
	uint64_t		total_system;
	uint64_t		threads_user;	/* existing threads only */
	uint64_t		threads_system;
};

typedef struct task_absolutetime_info	task_absolutetime_info_data_t;
typedef struct task_absolutetime_info	*task_absolutetime_info_t;
#define TASK_ABSOLUTETIME_INFO_COUNT	((mach_msg_type_number_t) \
		(sizeof (task_absolutetime_info_data_t) / sizeof (natural_t)))

#define TASK_KERNELMEMORY_INFO	7

struct task_kernelmemory_info {
	uint64_t		total_palloc;	/* private kernel mem alloc'ed */
	uint64_t		total_pfree;	/* private kernel mem freed */
	uint64_t		total_salloc;	/* shared kernel mem alloc'ed */
	uint64_t		total_sfree;	/* shared kernel mem freed */
};

typedef struct task_kernelmemory_info	task_kernelmemory_info_data_t;
typedef struct task_kernelmemory_info	*task_kernelmemory_info_t;
#define TASK_KERNELMEMORY_INFO_COUNT	((mach_msg_type_number_t) \
		(sizeof (task_kernelmemory_info_data_t) / sizeof (natural_t)))

#define TASK_SECURITY_TOKEN		13
#define TASK_SECURITY_TOKEN_COUNT	((mach_msg_type_number_t) \
		(sizeof(security_token_t) / sizeof(natural_t)))

#define TASK_AUDIT_TOKEN		15
#define TASK_AUDIT_TOKEN_COUNT	\
		(sizeof(audit_token_t) / sizeof(natural_t))


#define TASK_AFFINITY_TAG_INFO		16 	/* This is experimental. */

struct task_affinity_tag_info {
	integer_t		set_count;
	integer_t		min;
	integer_t		max;
	integer_t		task_count;
};
typedef struct task_affinity_tag_info	task_affinity_tag_info_data_t;
typedef struct task_affinity_tag_info	*task_affinity_tag_info_t;
#define TASK_AFFINITY_TAG_INFO_COUNT	\
		(sizeof(task_affinity_tag_info_data_t) / sizeof(natural_t))

#define TASK_DYLD_INFO			17

struct task_dyld_info {
	mach_vm_address_t	all_image_info_addr;
	mach_vm_size_t		all_image_info_size;
	integer_t		all_image_info_format;		
};
typedef struct task_dyld_info	task_dyld_info_data_t;
typedef struct task_dyld_info	*task_dyld_info_t;
#define TASK_DYLD_INFO_COUNT	\
    		(sizeof(task_dyld_info_data_t) / sizeof(natural_t))
#define TASK_DYLD_ALL_IMAGE_INFO_32	0	/* format value */
#define TASK_DYLD_ALL_IMAGE_INFO_64	1	/* format value */


#define TASK_EXTMOD_INFO			19

struct task_extmod_info {
	unsigned char	task_uuid[16];
	vm_extmod_statistics_data_t		extmod_statistics;
};
typedef struct task_extmod_info	task_extmod_info_data_t;
typedef struct task_extmod_info	*task_extmod_info_t;
#define TASK_EXTMOD_INFO_COUNT	\
    		(sizeof(task_extmod_info_data_t) / sizeof(natural_t))


#define MACH_TASK_BASIC_INFO     20         /* always 64-bit basic info */
struct mach_task_basic_info {
        mach_vm_size_t  virtual_size;       /* virtual memory size (bytes) */
        mach_vm_size_t  resident_size;      /* resident memory size (bytes) */
        mach_vm_size_t  resident_size_max;  /* maximum resident memory size (bytes) */
        time_value_t    user_time;          /* total user run time for
                                               terminated threads */
        time_value_t    system_time;        /* total system run time for
                                               terminated threads */
        policy_t        policy;             /* default policy for new threads */
        integer_t       suspend_count;      /* suspend count for task */
};
typedef struct mach_task_basic_info       mach_task_basic_info_data_t;
typedef struct mach_task_basic_info       *mach_task_basic_info_t;
#define MACH_TASK_BASIC_INFO_COUNT   \
                (sizeof(mach_task_basic_info_data_t) / sizeof(natural_t))


#define TASK_POWER_INFO	21

struct task_power_info {
	uint64_t		total_user;
	uint64_t		total_system;
	uint64_t		task_interrupt_wakeups;
	uint64_t		task_platform_idle_wakeups;
	uint64_t		task_timer_wakeups_bin_1;
	uint64_t		task_timer_wakeups_bin_2;
};

typedef struct task_power_info	task_power_info_data_t;
typedef struct task_power_info	*task_power_info_t;
#define TASK_POWER_INFO_COUNT	((mach_msg_type_number_t) \
		(sizeof (task_power_info_data_t) / sizeof (natural_t)))



#define TASK_VM_INFO		22
#define TASK_VM_INFO_PURGEABLE	23
struct task_vm_info {
        mach_vm_size_t  virtual_size;	    /* virtual memory size (bytes) */
	integer_t	region_count;	    /* number of memory regions */
	integer_t	page_size;
        mach_vm_size_t  resident_size;	    /* resident memory size (bytes) */
        mach_vm_size_t  resident_size_peak; /* peak resident size (bytes) */

	mach_vm_size_t	device;
	mach_vm_size_t	device_peak;
	mach_vm_size_t	internal;
	mach_vm_size_t	internal_peak;
	mach_vm_size_t	external;
	mach_vm_size_t	external_peak;
	mach_vm_size_t	reusable;
	mach_vm_size_t	reusable_peak;
	mach_vm_size_t	purgeable_volatile_pmap;
	mach_vm_size_t	purgeable_volatile_resident;
	mach_vm_size_t	purgeable_volatile_virtual;
	mach_vm_size_t	compressed;
	mach_vm_size_t	compressed_peak;
	mach_vm_size_t	compressed_lifetime;

	/* added for rev1 */
	mach_vm_size_t	phys_footprint;
};
typedef struct task_vm_info	task_vm_info_data_t;
typedef struct task_vm_info	*task_vm_info_t;
#define TASK_VM_INFO_COUNT	((mach_msg_type_number_t) \
		(sizeof (task_vm_info_data_t) / sizeof (natural_t)))
#define TASK_VM_INFO_REV0_COUNT /* doesn't include phys_footprint */ \
		((mach_msg_type_number_t) \
		(TASK_VM_INFO_COUNT - 2))

typedef struct vm_purgeable_info	task_purgable_info_t;


#define TASK_TRACE_MEMORY_INFO  24
struct task_trace_memory_info {
	uint64_t  user_memory_address; 	/* address of start of trace memory buffer */
	uint64_t  buffer_size;			/* size of buffer in bytes */
	uint64_t  mailbox_array_size;	/* size of mailbox area in bytes */
};
typedef struct task_trace_memory_info task_trace_memory_info_data_t;
typedef struct task_trace_memory_info * task_trace_memory_info_t;
#define TASK_TRACE_MEMORY_INFO_COUNT  ((mach_msg_type_number_t) \
		(sizeof(task_trace_memory_info_data_t) / sizeof(natural_t)))

#define TASK_WAIT_STATE_INFO  25    /* deprecated. */
struct task_wait_state_info {
	uint64_t  total_wait_state_time;	/* Time that all threads past and present have been in a wait state */
	uint64_t  total_wait_sfi_state_time;	/* Time that threads have been in SFI wait (should be a subset of total wait state time */
	uint32_t  _reserved[4];
};
typedef struct task_wait_state_info task_wait_state_info_data_t;
typedef struct task_wait_state_info * task_wait_state_info_t;
#define TASK_WAIT_STATE_INFO_COUNT  ((mach_msg_type_number_t) \
		(sizeof(task_wait_state_info_data_t) / sizeof(natural_t)))

#define TASK_POWER_INFO_V2	26

typedef struct {
	uint64_t		task_gpu_utilisation;
	uint64_t		task_gpu_stat_reserved0;
	uint64_t		task_gpu_stat_reserved1;
	uint64_t		task_gpu_stat_reserved2;
} gpu_energy_data;

typedef gpu_energy_data *gpu_energy_data_t;
struct task_power_info_v2 {
	task_power_info_data_t	cpu_energy;
	gpu_energy_data gpu_energy;
};

typedef struct task_power_info_v2	task_power_info_v2_data_t;
typedef struct task_power_info_v2	*task_power_info_v2_t;
#define TASK_POWER_INFO_V2_COUNT	((mach_msg_type_number_t) \
		(sizeof (task_power_info_v2_data_t) / sizeof (natural_t)))


#define TASK_VM_INFO_PURGEABLE_ACCOUNT 27 /* Used for xnu purgeable vm unit tests */

#ifdef PRIVATE
struct pvm_account_info {
	uint64_t pvm_volatile_count; /* Number of volatile bytes associated with a task */
	uint64_t pvm_volatile_compressed_count; /* Number of volatile compressed bytes associated with a task */
	uint64_t pvm_nonvolatile_count; /* Number of nonvolatile bytes associated with a task */
	uint64_t pvm_nonvolatile_compressed_count; /* Number of nonvolatile compressed bytes associated with a task */
};

typedef struct pvm_account_info *pvm_account_info_t;
typedef struct pvm_account_info pvm_account_info_data_t;

#define PVM_ACCOUNT_INFO_COUNT ((mach_msg_type_number_t) \
		(sizeof (pvm_account_info_data_t) / sizeof (natural_t)))
#endif /* PRIVATE */

#define TASK_FLAGS_INFO  28			/* return t_flags field */
struct task_flags_info {
	uint32_t	flags;				/* task flags */
};
typedef struct task_flags_info task_flags_info_data_t;
typedef struct task_flags_info * task_flags_info_t;
#define TASK_FLAGS_INFO_COUNT  ((mach_msg_type_number_t) \
		(sizeof(task_flags_info_data_t) / sizeof (natural_t)))

#define TF_LP64                 0x00000001                              /* task has 64-bit addressing */

#define TASK_DEBUG_INFO_INTERNAL    29 /* Used for kernel internal development tests. */

#ifdef PRIVATE
struct task_debug_info_internal {
	uint64_t ipc_space_size;
};
typedef struct task_debug_info_internal *task_debug_info_internal_t;
typedef struct task_debug_info_internal task_debug_info_internal_data_t;
#define TASK_DEBUG_INFO_INTERNAL_COUNT  ((mach_msg_type_number_t) \
		(sizeof (task_debug_info_internal_data_t) / sizeof(natural_t)))

#endif /* PRIVATE */

/*
 * Obsolete interfaces.
 */

#define TASK_SCHED_TIMESHARE_INFO	10
#define TASK_SCHED_RR_INFO		11
#define TASK_SCHED_FIFO_INFO		12

#define TASK_SCHED_INFO			14

#pragma pack()

#endif	/* _MACH_TASK_INFO_H_ */
