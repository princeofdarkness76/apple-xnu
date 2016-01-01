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
 * Copyright (c) 1999 Apple Computer, Inc. All rights reserved.
 */

#include <mach/mach_types.h>
#include <mach/exception_types.h>

#include <sys/param.h>
#include <sys/proc.h>
#include <sys/user.h>
#include <sys/ucontext.h>

#include <ppc/signal.h>
#include <sys/signalvar.h>
#include <sys/kdebug.h>
#include <sys/wait.h>
#include <kern/thread.h>
#include <kern/thread_act.h>
#include <mach/ppc/thread_status.h>
#define __ELF__ 0
#include <ppc/proc_reg.h>

#define	C_REDZONE_LEN		224
#define	C_STK_ALIGN			16
#define C_PARAMSAVE_LEN		64
#define	C_LINKAGE_LEN		48
#define TRUNC_DOWN(a,b,c)  (((((unsigned)a)-(b))/(c)) * (c))

/*
 * Arrange for this process to run a signal handler
 */

void
sendsig(p, catcher, sig, mask, code)
 	struct proc *p;
	sig_t catcher;
	int sig, mask;
	u_long code;
{
	struct mcontext mctx, *p_mctx;
	struct ucontext uctx, *p_uctx;
	siginfo_t sinfo, *p_sinfo;
	struct sigacts *ps = p->p_sigacts;
	int framesize;
	int oonstack;
	unsigned long sp;
	unsigned long state_count;
	thread_act_t th_act;
	struct uthread *ut;
	unsigned long paramp,linkp;
	int infostyle = 1;
	sig_t trampact;
	int vec_used = 0;
	int stack_size = 0;
	int stack_flags = 0;

	th_act = current_act();
	ut = get_bsdthread_info(th_act);

	state_count = PPC_EXCEPTION_STATE_COUNT;
	if (act_machine_get_state(th_act, PPC_EXCEPTION_STATE, &mctx.es, &state_count)  != KERN_SUCCESS) {
		goto bad;
	}	
	state_count = PPC_THREAD_STATE_COUNT;
	if (act_machine_get_state(th_act, PPC_THREAD_STATE, &mctx.ss, &state_count)  != KERN_SUCCESS) {
		goto bad;
	}	
	state_count = PPC_FLOAT_STATE_COUNT;
	if (act_machine_get_state(th_act, PPC_FLOAT_STATE, &mctx.fs, &state_count)  != KERN_SUCCESS) {
		goto bad;
	}	

	vec_save(th_act);
	if (find_user_vec(th_act)) {
		vec_used = 1;
<<<<<<< HEAD
		state_count = PPC_VECTOR_STATE_COUNT;
		if (act_machine_get_state(th_act, PPC_VECTOR_STATE, &mctx.vs, &state_count)  != KERN_SUCCESS) {
			goto bad;
		}	
		
	}
=======

                if ((ctx32 == 1) || dualcontext) {
                    flavor = PPC_VECTOR_STATE;
                    tstate = (void *)&mctx.vs;
                    state_count = PPC_VECTOR_STATE_COUNT;
                    if (thread_getstatus(th_act, flavor, (thread_state_t)tstate, &state_count)  != KERN_SUCCESS)
                    goto bad;
            } 
       
            if ((ctx32 == 0) || dualcontext) {
                    flavor = PPC_VECTOR_STATE;
                    tstate = (void *)&mctx64.vs;
                    state_count = PPC_VECTOR_STATE_COUNT;
                    if (thread_getstatus(th_act, flavor, (thread_state_t)tstate, &state_count)  != KERN_SUCCESS)
                        goto bad;
           }
        infostyle += 5;
	}  
>>>>>>> origin/10.3

	trampact = ps->ps_trampact[sig];
	oonstack = ps->ps_sigstk.ss_flags & SA_ONSTACK;
	if (p->p_sigacts->ps_siginfo & sigmask(sig))
		infostyle = 2;

	/* figure out where our new stack lives */
	if ((ps->ps_flags & SAS_ALTSTACK) && !oonstack &&
		(ps->ps_sigonstack & sigmask(sig))) {
		sp = (unsigned long)(ps->ps_sigstk.ss_sp);
		sp += ps->ps_sigstk.ss_size;
		stack_size = ps->ps_sigstk.ss_size;
		ps->ps_sigstk.ss_flags |= SA_ONSTACK;
	}
	else
		sp = mctx.ss.r1;

	/* preserve RED ZONE area */
	sp = TRUNC_DOWN(sp, C_REDZONE_LEN, C_STK_ALIGN);

	/* context goes first on stack */
	sp -= sizeof(*p_uctx);
	p_uctx = (struct ucontext *) sp;

	/* this is where siginfo goes on stack */
	sp -= sizeof(*p_sinfo);
	p_sinfo = (siginfo_t *) sp;

	/* next are the saved registers */
	sp -= sizeof(*p_mctx);
	p_mctx = (struct mcontext *)sp;

	/* C calling conventions, create param save and linkage
	 *  areas
	 */

	sp = TRUNC_DOWN(sp, C_PARAMSAVE_LEN, C_STK_ALIGN);
	paramp = sp;
	sp -= C_LINKAGE_LEN;
	linkp = sp;

	uctx.uc_onstack = oonstack;
	uctx.uc_sigmask = mask;
	uctx.uc_stack.ss_sp = (char *)sp;
	uctx.uc_stack.ss_size = stack_size;
	if (oonstack)
		uctx.uc_stack.ss_flags |= SS_ONSTACK;
		
	uctx.uc_link = 0;
	uctx.uc_mcsize = (size_t)((PPC_EXCEPTION_STATE_COUNT + PPC_THREAD_STATE_COUNT + PPC_FLOAT_STATE_COUNT) * sizeof(int));
	if (vec_used) 
		uctx.uc_mcsize += (size_t)(PPC_VECTOR_STATE_COUNT * sizeof(int));
	uctx.uc_mcontext = p_mctx;

	/* setup siginfo */
	bzero((caddr_t)&sinfo, sizeof(siginfo_t));
	sinfo.si_signo = sig;
	switch (sig) {
		case SIGCHLD:
			sinfo.si_pid = p->si_pid;
			p->si_pid =0;
			sinfo.si_status = p->si_status;
			p->si_status = 0;
			sinfo.si_uid = p->si_uid;
			p->si_uid =0;
			sinfo.si_code = p->si_code;
			p->si_code = 0;
			if (sinfo.si_code == CLD_EXITED) {
				if (WIFEXITED(sinfo.si_status)) 
					sinfo.si_code = CLD_EXITED;
				else if (WIFSIGNALED(sinfo.si_status)) {
					if (WCOREDUMP(sinfo.si_status))
						sinfo.si_code = CLD_DUMPED;
					else	
						sinfo.si_code = CLD_KILLED;
				}
			}
			break;
		case SIGILL:
			sinfo.si_addr = (void *)mctx.ss.srr0;
			if (mctx.ss.srr1 & (1 << (31 - SRR1_PRG_ILL_INS_BIT)))
				sinfo.si_code = ILL_ILLOPC;
			else if (mctx.ss.srr1 & (1 << (31 - SRR1_PRG_PRV_INS_BIT)))
				sinfo.si_code = ILL_PRVOPC;
			else if (mctx.ss.srr1 & (1 << (31 - SRR1_PRG_TRAP_BIT)))
				sinfo.si_code = ILL_ILLTRP;
			else
				sinfo.si_code = ILL_NOOP;
			break;
		case SIGFPE:
#define FPSCR_VX	2
#define FPSCR_OX	3
#define FPSCR_UX	4
#define FPSCR_ZX	5
#define FPSCR_XX	6
			sinfo.si_addr = (void *)mctx.ss.srr0;
			if (mctx.fs.fpscr & (1 << (31 - FPSCR_VX)))
				sinfo.si_code = FPE_FLTINV;
			else if (mctx.fs.fpscr & (1 << (31 - FPSCR_OX)))
				sinfo.si_code = FPE_FLTOVF;
			else if (mctx.fs.fpscr & (1 << (31 - FPSCR_UX)))
				sinfo.si_code = FPE_FLTUND;
			else if (mctx.fs.fpscr & (1 << (31 - FPSCR_ZX)))
				sinfo.si_code = FPE_FLTDIV;
			else if (mctx.fs.fpscr & (1 << (31 - FPSCR_XX)))
				sinfo.si_code = FPE_FLTRES;
			else
				sinfo.si_code = FPE_NOOP;
			break;

		case SIGBUS:
			sinfo.si_addr = (void *)mctx.ss.srr0;
			/* on ppc we generate only if EXC_PPC_UNALIGNED */
			sinfo.si_code = BUS_ADRALN;
			break;

		case SIGSEGV:
			sinfo.si_addr = (void *)mctx.ss.srr0;
			/* First check in srr1 and then in dsisr */
			if (mctx.ss.srr1 & (1 << (31 - DSISR_PROT_BIT)))
				sinfo.si_code = SEGV_ACCERR;
			else if (mctx.es.dsisr & (1 << (31 - DSISR_PROT_BIT)))
				sinfo.si_code = SEGV_ACCERR;
			else
				sinfo.si_code = SEGV_MAPERR;
			break;
		default:
			break;
	}

	/* copy info out to user space */
	if (copyout((caddr_t)&uctx, (caddr_t)p_uctx, sizeof(struct ucontext)))
		goto bad;
	if (copyout((caddr_t)&sinfo, (caddr_t)p_sinfo, sizeof(siginfo_t)))
		goto bad;
<<<<<<< HEAD
	if (copyout((caddr_t)&mctx, (caddr_t)p_mctx, uctx.uc_mcsize))
		goto bad;
=======
	if ((ctx32 == 0) || dualcontext) {
		tstate = &mctx64;
		if (copyout((caddr_t)tstate, (caddr_t)p_mctx64, (vec_used? UC_FLAVOR64_VEC_SIZE: UC_FLAVOR64_SIZE)))
			goto bad;
	}
	if ((ctx32 == 1) || dualcontext) {
		tstate = &mctx;
		if (copyout((caddr_t)tstate, (caddr_t)p_mctx, uctx.uc_mcsize))
			goto bad;
	}    

>>>>>>> origin/10.3

	/* Place our arguments in arg registers: rtm dependent */

	mctx.ss.r3 = (unsigned long)catcher;
	mctx.ss.r4 = (unsigned long)infostyle;
	mctx.ss.r5 = (unsigned long)sig;
	mctx.ss.r6 = (unsigned long)p_sinfo;
	mctx.ss.r7 = (unsigned long)p_uctx;

	mctx.ss.srr0 = (unsigned long)trampact;
	mctx.ss.srr1 = get_msr_exportmask();	/* MSR_EXPORT_MASK_SET */
	mctx.ss.r1 = sp;
	state_count = PPC_THREAD_STATE_COUNT;
	if (act_machine_set_state(th_act, PPC_THREAD_STATE, &mctx.ss, &state_count)  != KERN_SUCCESS) {
		goto bad;
	}	

	return;

bad:
	SIGACTION(p, SIGILL) = SIG_DFL;
	sig = sigmask(SIGILL);
	p->p_sigignore &= ~sig;
	p->p_sigcatch &= ~sig;
	ut->uu_sigmask &= ~sig;
	/* sendsig is called with signal lock held */
	psignal_lock(p, SIGILL, 0);
	return;
}

/*
 * System call to cleanup state after a signal
 * has been taken.  Reset signal mask and
 * stack state from context left by sendsig (above).
 * Return to previous pc and psl as specified by
 * context left by sendsig. Check carefully to
 * make sure that the user has not modified the
 * psl to gain improper priviledges or to cause
 * a machine fault.
 */
<<<<<<< HEAD
=======

#define FOR64_TRANSITION 1


#ifdef FOR64_TRANSITION

struct osigreturn_args {
	struct ucontext *uctx;
};

/* ARGSUSED */
int
osigreturn(p, uap, retval)
	struct proc *p;
	struct osigreturn_args *uap;
	int *retval;
{
	struct ucontext uctx;
	struct ucontext *p_uctx;
	struct mcontext64 mctx64;
	struct mcontext64 *p_64mctx;
	struct mcontext  *p_mctx;
	int error;
	thread_act_t th_act;
	struct sigacts *ps = p->p_sigacts;
	sigset_t mask;	
	register sig_t action;
	unsigned long state_count;
	unsigned int state_flavor;
	struct uthread * ut;
	int vec_used = 0;
	void *tsptr, *fptr, *vptr, *mactx;
	void ppc_checkthreadstate(void *, int);

	th_act = current_act();
	/* lets use the larger one */
	mactx = (void *)&mctx64;

	ut = (struct uthread *)get_bsdthread_info(th_act);
	if (error = copyin(uap->uctx, &uctx, sizeof(struct ucontext))) {
		return(error);
	}
	
	/* validate the machine context size */
	switch (uctx.uc_mcsize)  {
		case UC_FLAVOR64_VEC_SIZE :
		case UC_FLAVOR64_SIZE : 
		case UC_FLAVOR_VEC_SIZE :
		case UC_FLAVOR_SIZE:
			break;
		default: 
			return(EINVAL);
	}

	if (error = copyin(uctx.uc_mcontext, mactx, uctx.uc_mcsize)) {
		return(error);
	}
	
	if (uctx.uc_onstack & 01)
			p->p_sigacts->ps_sigstk.ss_flags |= SA_ONSTACK;
	else
		p->p_sigacts->ps_sigstk.ss_flags &= ~SA_ONSTACK;

	ut->uu_sigmask = uctx.uc_sigmask & ~sigcantmask;
	if (ut->uu_siglist & ~ut->uu_sigmask)
		signal_setast(current_act());	

	vec_used = 0;
	switch (uctx.uc_mcsize)  {
		case UC_FLAVOR64_VEC_SIZE :
			vec_used = 1;
		case UC_FLAVOR64_SIZE : {
			p_64mctx = (struct mcontext64 *)mactx;	
			tsptr = (void *)&p_64mctx->ss;
			fptr = (void *)&p_64mctx->fs;
			vptr = (void *)&p_64mctx->vs;
			state_flavor = PPC_THREAD_STATE64;
			state_count = PPC_THREAD_STATE64_COUNT;
			}
			break;
		case UC_FLAVOR_VEC_SIZE :
			vec_used = 1;
		case UC_FLAVOR_SIZE:
		default: {
			p_mctx = (struct mcontext *)mactx;	
			tsptr = (void *)&p_mctx->ss;
			fptr = (void *)&p_mctx->fs;
			vptr = (void *)&p_mctx->vs;
			state_flavor = PPC_THREAD_STATE;
			state_count = PPC_THREAD_STATE_COUNT;
		}
		break;
	} /* switch () */

	/* validate the thread state, set/reset appropriate mode bits in srr1 */
	(void)ppc_checkthreadstate(tsptr, state_flavor);

	if (thread_setstatus(th_act, state_flavor, tsptr, &state_count)  != KERN_SUCCESS) {
		return(EINVAL);
	}	

	state_count = PPC_FLOAT_STATE_COUNT;
	if (thread_setstatus(th_act, PPC_FLOAT_STATE, fptr, &state_count)  != KERN_SUCCESS) {
		return(EINVAL);
	}	

	mask = sigmask(SIGFPE);
	if (((ut->uu_sigmask & mask) == 0) && (p->p_sigcatch & mask) && ((p->p_sigignore & mask) == 0)) {
		action = ps->ps_sigact[SIGFPE];
		if((action != SIG_DFL) && (action != SIG_IGN)) {
			thread_enable_fpe(th_act, 1);
		}
	}

	if (vec_used) {
		state_count = PPC_VECTOR_STATE_COUNT;
		if (thread_setstatus(th_act, PPC_VECTOR_STATE, vptr, &state_count)  != KERN_SUCCESS) {
			return(EINVAL);
		}	
	}
	return (EJUSTRETURN);
}

#endif /* FOR64_TRANSITION */

>>>>>>> origin/10.3
struct sigreturn_args {
	struct ucontext *uctx;
};

/* ARGSUSED */
int
sigreturn(p, uap, retval)
	struct proc *p;
	struct sigreturn_args *uap;
	int *retval;
{
	struct ucontext uctx, *p_uctx;
	struct mcontext mctx, *p_mctx;
	int error;
	thread_act_t th_act;
	struct ppc_float_state fs;
	struct ppc_exception_state es;
	struct sigacts *ps = p->p_sigacts;
	sigset_t mask;	
	register sig_t action;
	unsigned long state_count;
	unsigned int nbits, rbits;
	struct uthread * ut;
	int vec_used = 0;

	th_act = current_act();

	ut = (struct uthread *)get_bsdthread_info(th_act);
	if (error = copyin(uap->uctx, &uctx, sizeof(struct ucontext))) {
		return(error);
	}
<<<<<<< HEAD
	if (error = copyin(uctx.uc_mcontext, &mctx, sizeof(struct mcontext))) {
=======
        
	/* validate the machine context size */
	switch (uctx.uc_mcsize) {
		case UC_FLAVOR64_VEC_SIZE:
		case UC_FLAVOR64_SIZE:
		case UC_FLAVOR_VEC_SIZE:
		case UC_FLAVOR_SIZE:
			break;
		default:
			return(EINVAL);
	}
	if (error = copyin(uctx.uc_mcontext, mactx, uctx.uc_mcsize)) {
>>>>>>> origin/10.3
		return(error);
	}
	
	if (uctx.uc_onstack & 01)
			p->p_sigacts->ps_sigstk.ss_flags |= SA_ONSTACK;
	else
		p->p_sigacts->ps_sigstk.ss_flags &= ~SA_ONSTACK;
	ut->uu_sigmask = uctx.uc_sigmask & ~sigcantmask;


	if (ut->uu_siglist & ~ut->uu_sigmask)
		signal_setast(current_act());	

	nbits = get_msr_nbits();
	rbits = get_msr_rbits();
	/* adjust the critical fields */
	/* make sure naughty bits are off */
	mctx.ss.srr1 &= ~(nbits);
	/* make sure necessary bits are on */
	mctx.ss.srr1 |= (rbits);

	state_count = (size_t)((PPC_EXCEPTION_STATE_COUNT + PPC_THREAD_STATE_COUNT + PPC_FLOAT_STATE_COUNT) * sizeof(int));

	if (uctx.uc_mcsize > state_count)
		vec_used = 1;

	state_count = PPC_THREAD_STATE_COUNT;
	if (act_machine_set_state(th_act, PPC_THREAD_STATE, &mctx.ss, &state_count)  != KERN_SUCCESS) {
		return(EINVAL);
	}	

	state_count = PPC_FLOAT_STATE_COUNT;
	if (act_machine_set_state(th_act, PPC_FLOAT_STATE, &mctx.fs, &state_count)  != KERN_SUCCESS) {
		return(EINVAL);
	}	

	mask = sigmask(SIGFPE);
	if (((ut->uu_sigmask & mask) == 0) && (p->p_sigcatch & mask) && ((p->p_sigignore & mask) == 0)) {
		action = ps->ps_sigact[SIGFPE];
		if((action != SIG_DFL) && (action != SIG_IGN)) {
			thread_enable_fpe(th_act, 1);
		}
	}

	if (vec_used) {
		state_count = PPC_VECTOR_STATE_COUNT;
		if (act_machine_set_state(th_act, PPC_VECTOR_STATE, &mctx.vs, &state_count)  != KERN_SUCCESS) {
			return(EINVAL);
		}	
	}

	return (EJUSTRETURN);
}

/*
 * machine_exception() performs MD translation
 * of a mach exception to a unix signal and code.
 */

boolean_t
machine_exception(
    int		exception,
    int		code,
    int		subcode,
    int		*unix_signal,
    int		*unix_code
)
{
    switch(exception) {

    case EXC_BAD_INSTRUCTION:
	*unix_signal = SIGILL;
	*unix_code = code;
	break;

    case EXC_ARITHMETIC:
	*unix_signal = SIGFPE;
	*unix_code = code;
	break;

    case EXC_SOFTWARE:
	if (code == EXC_PPC_TRAP) {
		*unix_signal = SIGTRAP;
		*unix_code = code;
		break;
	} else
		return(FALSE);

    default:
	return(FALSE);
    }
   
    return(TRUE);
}

