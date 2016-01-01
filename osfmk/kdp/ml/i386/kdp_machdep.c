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
 
#include <mach/mach_types.h>
#include <mach/machine.h>
#include <mach/exception_types.h>
#include <i386/trap.h>
#include <kdp/kdp_internal.h>

#define KDP_TEST_HARNESS 0
#if KDP_TEST_HARNESS
#define dprintf(x) printf x
#else
#define dprintf(x)
#endif

void print_saved_state(void *);
void kdp_call(void);
void kdp_i386_trap(unsigned int, struct i386_saved_state *, kern_return_t, vm_offset_t);
int kdp_getc(void);

void
kdp_exception(
    unsigned char	*pkt,
    int	*len,
    unsigned short	*remote_port,
    unsigned int	exception,
    unsigned int	code,
    unsigned int	subcode
)
{
    kdp_exception_t	*rq = (kdp_exception_t *)pkt;

    rq->hdr.request = KDP_EXCEPTION;
    rq->hdr.is_reply = 0;
    rq->hdr.seq = kdp.exception_seq;
    rq->hdr.key = 0;
    rq->hdr.len = sizeof (*rq);
    
    rq->n_exc_info = 1;
    rq->exc_info[0].cpu = 0;
    rq->exc_info[0].exception = exception;
    rq->exc_info[0].code = code;
    rq->exc_info[0].subcode = subcode;
    
    rq->hdr.len += rq->n_exc_info * sizeof (kdp_exc_info_t);
    
    bcopy((char *)rq, (char *)pkt, rq->hdr.len);

    kdp.exception_ack_needed = TRUE;
    
    *remote_port = kdp.exception_port;
    *len = rq->hdr.len;
}

boolean_t
kdp_exception_ack(
    unsigned char	*pkt,
    int			len
)
{
    kdp_exception_ack_t	*rq = (kdp_exception_ack_t *)pkt;

    if (len < sizeof (*rq))
	return(FALSE);
	
    if (!rq->hdr.is_reply || rq->hdr.request != KDP_EXCEPTION)
    	return(FALSE);
	
    dprintf(("kdp_exception_ack seq %x %x\n", rq->hdr.seq, kdp.exception_seq));
	
    if (rq->hdr.seq == kdp.exception_seq) {
	kdp.exception_ack_needed = FALSE;
	kdp.exception_seq++;
    }
    return(TRUE);
}

void
kdp_getstate(
    i386_thread_state_t		*state
)
{
    struct i386_saved_state	*saved_state;
    
    saved_state = (struct i386_saved_state *)kdp.saved_state;
    
    *state = (i386_thread_state_t) { 0 };	
    state->eax = saved_state->eax;
    state->ebx = saved_state->ebx;
    state->ecx = saved_state->ecx;
    state->edx = saved_state->edx;
    state->edi = saved_state->edi;
    state->esi = saved_state->esi;
    state->ebp = saved_state->ebp;

    if ((saved_state->cs & SEL_PL) == SEL_PL_K) { /* Kernel state? */
	    if (cpu_mode_is64bit())
		    state->esp = (uint32_t) saved_state->uesp;
	    else
		    state->esp = ((uint32_t)saved_state) + offsetof(x86_saved_state_t, ss_32) + sizeof(x86_saved_state32_t);
        state->ss = KERNEL_DS;
    } else {
    	state->esp = saved_state->uesp;
    	state->ss = saved_state->ss;
    }

    state->eflags = saved_state->efl;
    state->eip = saved_state->eip;
    state->cs = saved_state->cs;
    state->ds = saved_state->ds;
    state->es = saved_state->es;
    state->fs = saved_state->fs;
    state->gs = saved_state->gs;
}


void
kdp_setstate(
    i386_thread_state_t		*state
)
{
    struct i386_saved_state	*saved_state;
    
    saved_state = (struct i386_saved_state *)kdp.saved_state;

    saved_state->eax = state->eax;
    saved_state->ebx = state->ebx;
    saved_state->ecx = state->ecx;
    saved_state->edx = state->edx;
    saved_state->edi = state->edi;
    saved_state->esi = state->esi;
    saved_state->ebp = state->ebp;
    saved_state->efl = state->eflags;
#if	0
    saved_state->frame.eflags &= ~( EFL_VM | EFL_NT | EFL_IOPL | EFL_CLR );
    saved_state->frame.eflags |=  ( EFL_IF | EFL_SET );
#endif
    saved_state->eip = state->eip;
}


kdp_error_t
kdp_machine_read_regs(
    unsigned int cpu,
    unsigned int flavor,
    char *data,
    int *size
)
{
    switch (flavor) {

    case i386_THREAD_STATE:
	dprintf(("kdp_readregs THREAD_STATE\n"));
	kdp_getstate((i386_thread_state_t *)data);
	*size = sizeof (i386_thread_state_t);
	return KDPERR_NO_ERROR;
	
    case i386_THREAD_FPSTATE:
	dprintf(("kdp_readregs THREAD_FPSTATE\n"));
	*(i386_thread_fpstate_t *)data = (i386_thread_fpstate_t) { 0 };	
	*size = sizeof (i386_thread_fpstate_t);
	return KDPERR_NO_ERROR;
	
    default:
	dprintf(("kdp_readregs bad flavor %d\n"));
	return KDPERR_BADFLAVOR;
    }
}

kdp_error_t
kdp_machine_write_regs(
    unsigned int cpu,
    unsigned int flavor,
    char *data,
    int *size
)
{
    switch (flavor) {

    case i386_THREAD_STATE:
	dprintf(("kdp_writeregs THREAD_STATE\n"));
	kdp_setstate((i386_thread_state_t *)data);
	return KDPERR_NO_ERROR;
	
    case i386_THREAD_FPSTATE:
	dprintf(("kdp_writeregs THREAD_FPSTATE\n"));
	return KDPERR_NO_ERROR;
	
    default:
	dprintf(("kdp_writeregs bad flavor %d\n"));
	return KDPERR_BADFLAVOR;
    }
}



void
kdp_machine_hostinfo(
    kdp_hostinfo_t *hostinfo
)
{
    machine_slot_t	m;
    int			i;

    hostinfo->cpus_mask = 0;

    for (i = 0; i < machine_info.max_cpus; i++) {
        m = &machine_slot[i];
        if (!m->is_cpu)
            continue;
	
        hostinfo->cpus_mask |= (1 << i);
    }

   /* FIXME?? */
    hostinfo->cpu_type = CPU_TYPE_I386;
    hostinfo->cpu_subtype = CPU_SUBTYPE_486;
}

void
kdp_panic(
    const char		*msg
)
{
    printf("kdp panic: %s\n", msg);    
    __asm__ volatile("hlt");	
}


void
kdp_reboot(void)
{
    kdreboot();
}

int
kdp_intr_disbl(void)
{
   return splhigh();
}

void
kdp_intr_enbl(int s)
{
	splx(s);
}

int
kdp_getc()
{
	return	cnmaygetc();
}

void
kdp_us_spin(int usec)
{
    extern void delay(int);

    delay(usec/100);
}

void print_saved_state(void *state)
{
    struct i386_saved_state	*saved_state;

    saved_state = state;

	printf("pc = 0x%x\n", saved_state->eip);
	printf("cr3= 0x%x\n", saved_state->cr2);
	printf("rp = TODO FIXME\n");
	printf("sp = 0x%x\n", saved_state->esp);

}

void
kdp_sync_cache()
{
	return;	/* No op here. */
}

void
kdp_call()
{
	__asm__ volatile ("int	$3");	/* Let the processor do the work */
}


typedef struct _cframe_t {
    struct _cframe_t	*prev;
    unsigned		caller;
    unsigned		args[0];
} cframe_t;


#define MAX_FRAME_DELTA		65536

void
kdp_i386_backtrace(void	*_frame, int nframes)
{
	cframe_t	*frame = (cframe_t *)_frame;
	int i;

	for (i=0; i<nframes; i++) {
	    if ((vm_offset_t)frame < VM_MIN_KERNEL_ADDRESS ||
	        (vm_offset_t)frame > VM_MAX_KERNEL_ADDRESS) {
		goto invalid;
	    }
	    printf("frame %x called by %x ",
		frame, frame->caller);
	    printf("args %x %x %x %x\n",
		frame->args[0], frame->args[1],
		frame->args[2], frame->args[3]);
	    if ((frame->prev < frame) ||	/* wrong direction */
	    	((frame->prev - frame) > MAX_FRAME_DELTA)) {
		goto invalid;
	    }
	    frame = frame->prev;
	}
	return;
invalid:
	printf("invalid frame pointer %x\n",frame->prev);
}

void
kdp_i386_trap(
    unsigned int		trapno,
    struct i386_saved_state	*saved_state,
    kern_return_t	result,
    vm_offset_t		va
)
{
    unsigned int exception, subcode = 0, code;

    if (trapno != T_INT3 && trapno != T_DEBUG)
    	printf("unexpected kernel trap %x eip %x\n", trapno, saved_state->eip);

    switch (trapno) {
    
    case T_DIVIDE_ERROR:
	exception = EXC_ARITHMETIC;
	code = EXC_I386_DIVERR;
	break;
    
    case T_OVERFLOW:
	exception = EXC_SOFTWARE;
	code = EXC_I386_INTOFLT;
	break;
    
    case T_OUT_OF_BOUNDS:
	exception = EXC_ARITHMETIC;
	code = EXC_I386_BOUNDFLT;
	break;
    
    case T_INVALID_OPCODE:
	exception = EXC_BAD_INSTRUCTION;
	code = EXC_I386_INVOPFLT;
	break;
    
    case T_SEGMENT_NOT_PRESENT:
	exception = EXC_BAD_INSTRUCTION;
	code = EXC_I386_SEGNPFLT;
	subcode	= saved_state->err;
	break;
    
    case T_STACK_FAULT:
	exception = EXC_BAD_INSTRUCTION;
	code = EXC_I386_STKFLT;
	subcode	= saved_state->err;
	break;
    
    case T_GENERAL_PROTECTION:
	exception = EXC_BAD_INSTRUCTION;
	code = EXC_I386_GPFLT;
	subcode	= saved_state->err;
	break;
	
    case T_PAGE_FAULT:
    	exception = EXC_BAD_ACCESS;
	code = result;
	subcode = va;
	break;
    
    case T_WATCHPOINT:
	exception = EXC_SOFTWARE;
	code = EXC_I386_ALIGNFLT;
	break;
	
    case T_DEBUG:
    case T_INT3:
	exception = EXC_BREAKPOINT;
	code = EXC_I386_BPTFLT;
	break;

    default:
    	exception = EXC_BAD_INSTRUCTION;
	code = trapno;
	break;
    }

    kdp_i386_backtrace((void *) saved_state->ebp, 10);

    kdp_raise_exception(exception, code, subcode, saved_state);
}

boolean_t 
kdp_call_kdb(
        void) 
{       
        return(FALSE);
}

unsigned int kdp_ml_get_breakinsn()
{
  return 0xcc;
}
<<<<<<< HEAD
=======

extern pmap_t kdp_pmap;
extern uint32_t kdp_src_high32;

#define RETURN_OFFSET 4
int
machine_trace_thread(thread_t thread, char *tracepos, char *tracebound, int nframes, boolean_t user_p)
{
	uint32_t *tracebuf = (uint32_t *)tracepos;
	uint32_t fence = 0;
	uint32_t stackptr = 0;
	uint32_t stacklimit = 0xfc000000;
	int framecount = 0;
	uint32_t init_eip = 0;
	uint32_t prevsp = 0;
	uint32_t framesize = 2 * sizeof(vm_offset_t);
	
	if (user_p) {
	        x86_saved_state32_t	*iss32;
		
		iss32 = USER_REGS32(thread);

			init_eip = iss32->eip;
			stackptr = iss32->ebp;

		/* This bound isn't useful, but it doesn't hinder us*/
		stacklimit = 0xffffffff;
		kdp_pmap = thread->task->map->pmap;
	}
	else {
		/*Examine the i386_saved_state at the base of the kernel stack*/
		stackptr = STACK_IKS(thread->kernel_stack)->k_ebp;
		init_eip = STACK_IKS(thread->kernel_stack)->k_eip;
	}

	*tracebuf++ = init_eip;

	for (framecount = 0; framecount < nframes; framecount++) {

		if ((uint32_t)(tracebound - ((char *)tracebuf)) < (4 * framesize)) {
			tracebuf--;
			break;
		}

		*tracebuf++ = stackptr;
/* Invalid frame, or hit fence */
		if (!stackptr || (stackptr == fence)) {
			break;
		}
		/* Stack grows downward */
		if (stackptr < prevsp) {
			break;
		}
		/* Unaligned frame */
		if (stackptr & 0x0000003) {
			break;
		}
		if (stackptr > stacklimit) {
			break;
		}

		if (kdp_vm_read((caddr_t) (stackptr + RETURN_OFFSET), (caddr_t) tracebuf, sizeof(caddr_t)) != sizeof(caddr_t)) {
			break;
		}
		tracebuf++;
		
		prevsp = stackptr;
		if (kdp_vm_read((caddr_t) stackptr, (caddr_t) &stackptr, sizeof(caddr_t)) != sizeof(caddr_t)) {
			*tracebuf++ = 0;
			break;
		}
	}

	kdp_pmap = 0;

	return (uint32_t) (((char *) tracebuf) - tracepos);
}

#define RETURN_OFFSET64	8
/* Routine to encapsulate the 64-bit address read hack*/
unsigned
machine_read64(addr64_t srcaddr, caddr_t dstaddr, uint32_t len)
{
	uint32_t kdp_vm_read_low32;
	unsigned retval;
	
	kdp_src_high32 = srcaddr >> 32;
	kdp_vm_read_low32 = srcaddr & 0x00000000FFFFFFFFUL;
	retval = kdp_vm_read((caddr_t)kdp_vm_read_low32, dstaddr, len);
	kdp_src_high32 = 0;
	return retval;
}

int
machine_trace_thread64(thread_t thread, char *tracepos, char *tracebound, int nframes, boolean_t user_p)
{
	uint64_t *tracebuf = (uint64_t *)tracepos;
	uint32_t fence = 0;
	addr64_t stackptr = 0;
	uint64_t stacklimit = 0xfc000000;
	int framecount = 0;
	addr64_t init_rip = 0;
	addr64_t prevsp = 0;
	unsigned framesize = 2 * sizeof(addr64_t);
	
	if (user_p) {
		x86_saved_state64_t	*iss64;
		iss64 = USER_REGS64(thread);
		init_rip = iss64->isf.rip;
		stackptr = iss64->rbp;
		stacklimit = 0xffffffffffffffffULL;
		kdp_pmap = thread->task->map->pmap;
	}
	else {
		/* DRK: This would need to adapt for a 64-bit kernel, if any */
		stackptr = STACK_IKS(thread->kernel_stack)->k_ebp;
		init_rip = STACK_IKS(thread->kernel_stack)->k_eip;
	}

	*tracebuf++ = init_rip;

	for (framecount = 0; framecount < nframes; framecount++) {

		if ((uint32_t)(tracebound - ((char *)tracebuf)) < (4 * framesize)) {
			tracebuf--;
			break;
		}

		*tracebuf++ = stackptr;

		if (!stackptr || (stackptr == fence)){
			break;
		}
		if (stackptr < prevsp) {
			break;
		}
		if (stackptr & 0x0000003) {
			break;
		}
		if (stackptr > stacklimit) {
			break;
		}

		if (machine_read64(stackptr + RETURN_OFFSET64, (caddr_t) tracebuf, sizeof(addr64_t)) != sizeof(addr64_t)) {
			break;
		}
		tracebuf++;

		prevsp = stackptr;
		if (machine_read64(stackptr, (caddr_t) &stackptr, sizeof(addr64_t)) != sizeof(addr64_t)) {
			*tracebuf++ = 0;
			break;
		}
	}

	kdp_pmap = NULL;

	return (uint32_t) (((char *) tracebuf) - tracepos);
}

static struct kdp_callout {
	struct kdp_callout	*callout_next;
	kdp_callout_fn_t	callout_fn;
	void			*callout_arg;
} *kdp_callout_list = NULL;


/*
 * Called from kernel context to register a kdp event callout.
 */
void
kdp_register_callout(
	kdp_callout_fn_t	fn,
	void			*arg)
{
	struct kdp_callout	*kcp;
	struct kdp_callout	*list_head;

	kcp = kalloc(sizeof(*kcp));
	if (kcp == NULL)
		panic("kdp_register_callout() kalloc failed");

	kcp->callout_fn  = fn;
	kcp->callout_arg = arg;

	/* Lock-less list insertion using compare and exchange. */
	do {
		list_head = kdp_callout_list;
		kcp->callout_next = list_head;
	} while(!atomic_cmpxchg((uint32_t *) &kdp_callout_list,
				(uint32_t) list_head,
				(uint32_t) kcp));
}

/*
 * Called at exception/panic time when extering or exiting kdp.  
 * We are single-threaded at this time and so we don't use locks.
 */
static void
kdp_callouts(kdp_event_t event)
{
	struct kdp_callout	*kcp = kdp_callout_list;

	while (kcp) {
		kcp->callout_fn(kcp->callout_arg, event); 
		kcp = kcp->callout_next;
	}	
}

void
kdp_ml_enter_debugger(void)
{
	__asm__ __volatile__("int3");
}
>>>>>>> origin/10.5
