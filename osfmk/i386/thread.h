/*
 * Copyright (c) 2000-2012 Apple Inc. All rights reserved.
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
 * Copyright (c) 1991,1990,1989 Carnegie Mellon University
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
 *	File:	machine/thread.h
 *
 *	This file contains the structure definitions for the thread
 *	state as applied to I386 processors.
 */

#ifndef	_I386_THREAD_H_
#define _I386_THREAD_H_

#include <mach/boolean.h>
#include <mach/i386/vm_types.h>
#include <mach/i386/fp_reg.h>
#include <mach/thread_status.h>

#include <kern/simple_lock.h>

#include <i386/iopb.h>
#include <i386/seg.h>
#include <i386/tss.h>
#include <i386/eflags.h>

<<<<<<< HEAD
#include <i386/cpu_data.h>
=======
/*
 *	x86_saved_state32/64:
 *
 *	Has been exported to servers.  See: mach/i386/thread_status.h
 *
 *	This structure corresponds to the state of user registers
 *	as saved upon kernel entry.  It lives in the pcb.
 *	It is also pushed onto the stack for exceptions in the kernel.
 *	For performance, it is also used directly in syscall exceptions
 *	if the server has requested i386_THREAD_STATE flavor for the exception
 *	port.
 */

/*
 *	Save area for user floating-point state.
 *	Allocated only when necessary.
 */

typedef	enum {
		FXSAVE32 = 1,
<<<<<<< HEAD
		FXSAVE64 = 2
	} fp_save_layout;
        struct x86_fx_save 	fx_save_state __attribute__ ((aligned (16)));
};
>>>>>>> origin/10.5
=======
		FXSAVE64 = 2,
		XSAVE32  = 3,
		XSAVE64  = 4,
		FP_UNUSED = 5
	} fp_save_layout_t;

>>>>>>> origin/10.6

#include <machine/pal_routines.h>

/*
 *	x86_kernel_state:
 *
 *	This structure corresponds to the state of kernel registers
 *	as saved in a context-switch.  It lives at the base of the stack.
 */

struct x86_kernel_state {
	uint64_t	k_rbx;	/* kernel context */
	uint64_t	k_rsp;
	uint64_t	k_rbp;
	uint64_t	k_r12;
	uint64_t	k_r13;
	uint64_t	k_r14;
	uint64_t	k_r15;
	uint64_t	k_rip;
};
<<<<<<< HEAD
=======
#endif

typedef struct pcb {
	void			*sf;
	x86_saved_state_t	*iss;
	void			*ifps;
#ifdef	MACH_BSD
	uint64_t	cthread_self;		/* for use of cthread package */
        struct real_descriptor cthread_desc;
	unsigned long  uldt_selector;          /* user ldt selector to set */
	struct real_descriptor uldt_desc;      /* the actual user setable ldt data */
#endif
	decl_simple_lock_data(,lock);
	uint64_t	iss_pte0;
	uint64_t	iss_pte1;
	void		*ids;
	uint32_t	arg_store_valid;
} *pcb_t;
>>>>>>> origin/10.6

/*
 * Maps state flavor to number of words in the state:
 */
extern unsigned int _MachineStateCount[];

/*
 * The machine-dependent thread state - registers and all platform-dependent
 * state - is saved in the machine thread structure which is embedded in
 * the thread data structure. For historical reasons this is also referred to
 * as the PCB.
 */
struct machine_thread {
	x86_saved_state_t	*iss;
	void			*ifps;
	void			*ids;
	decl_simple_lock_data(,lock);		/* protects ifps and ids */
	uint64_t		iss_pte0;
	uint64_t		iss_pte1;

#ifdef	MACH_BSD
	uint64_t		cthread_self;	/* for use of cthread package */
        struct real_descriptor	cthread_desc;
	unsigned long		uldt_selector;	/* user ldt selector to set */
	struct real_descriptor	uldt_desc;	/* actual user setable ldt */
#endif

	struct pal_pcb		pal_pcb;
	uint32_t		specFlags;
#define		OnProc		0x1
#define		CopyIOActive 	0x2 /* Checked to ensure DTrace actions do not re-enter copyio(). */
	uint64_t		thread_gpu_ns;
#if NCOPY_WINDOWS > 0
        struct {
	        user_addr_t	user_base;
	} copy_window[NCOPY_WINDOWS];
        int			nxt_window;
        int			copyio_state;
#define		WINDOWS_DIRTY	0
#define		WINDOWS_CLEAN	1
#define		WINDOWS_CLOSED	2
#define		WINDOWS_OPENED	3
        uint64_t		physwindow_pte;
        int			physwindow_busy;
#endif
};
typedef struct machine_thread *pcb_t;

#define	THREAD_TO_PCB(Thr)	(&(Thr)->machine)

#define USER_STATE(Thr)		((Thr)->machine.iss)
#define USER_REGS32(Thr)	(saved_state32(USER_STATE(Thr)))
#define USER_REGS64(Thr)	(saved_state64(USER_STATE(Thr)))

#define	user_pc(Thr)		(is_saved_state32(USER_STATE(Thr)) ?	\
					USER_REGS32(Thr)->eip :		\
					USER_REGS64(Thr)->isf.rip )

extern void *get_user_regs(thread_t);

extern void *act_thread_csave(void);
extern void act_thread_catt(void *ctx);
extern void act_thread_cfree(void *ctx);

#define FIND_PERFCONTROL_STATE(th)	(PERFCONTROL_STATE_NULL)

/*
 *	On the kernel stack is:
 *	stack:	...
 *		struct x86_kernel_state
 *	stack+kernel_stack_size
 */

#define STACK_IKS(stack)	\
	((struct x86_kernel_state *)((stack) + kernel_stack_size) - 1)

/*
 * Return the current stack depth including x86_kernel_state
 */
static inline vm_offset_t
current_stack_depth(void)
{
	vm_offset_t	stack_ptr;

	assert(get_preemption_level() > 0 || !ml_get_interrupts_enabled());

#if defined(__x86_64__)
       __asm__ volatile("mov %%rsp, %0" : "=m" (stack_ptr));
#else
       __asm__ volatile("mov %%esp, %0" : "=m" (stack_ptr));
#endif
	return (current_cpu_datap()->cpu_kernel_stack
		+ sizeof(struct x86_kernel_state)
		- stack_ptr); 
}

/*
 * Return address of the function that called current function, given
 *	address of the first parameter of current function.
 */
#define	GET_RETURN_PC(addr)	(__builtin_return_address(0))

#endif	/* _I386_THREAD_H_ */
