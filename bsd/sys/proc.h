/*
<<<<<<< HEAD
 * Copyright (c) 2000-2006 Apple Computer, Inc. All rights reserved.
=======
 * Copyright (c) 2000-2004 Apple Computer, Inc. All rights reserved.
>>>>>>> origin/10.3
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
/* Copyright (c) 1995, 1997 Apple Computer, Inc. All Rights Reserved */
/*-
 * Copyright (c) 1986, 1989, 1991, 1993
 *	The Regents of the University of California.  All rights reserved.
 * (c) UNIX System Laboratories, Inc.
 * All or some portions of this file are derived from material licensed
 * to the University of California by American Telephone and Telegraph
 * Co. or Unix System Laboratories, Inc. and are reproduced herein with
 * the permission of UNIX System Laboratories, Inc.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *	This product includes software developed by the University of
 *	California, Berkeley and its contributors.
 * 4. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 *	@(#)proc.h	8.15 (Berkeley) 5/19/95
 */

#ifndef _SYS_PROC_H_
#define	_SYS_PROC_H_

#include <sys/appleapiopts.h>
#include <sys/cdefs.h>
#include <sys/select.h>			/* For struct selinfo. */
#include <sys/queue.h>
#include <sys/lock.h>
#include <sys/param.h>
#include <sys/event.h>
<<<<<<< HEAD
#ifdef KERNEL
#include <sys/kernel_types.h>
#include <uuid/uuid.h>
#endif
#include <mach/boolean.h>
=======

#ifdef __APPLE_API_PRIVATE

/*
 * One structure allocated per session.
 */
struct	session {
	int	s_count;		/* Ref cnt; pgrps in session. */
	struct	proc *s_leader;		/* Session leader. */
	struct	vnode *s_ttyvp;		/* Vnode of controlling terminal. */
	struct	tty *s_ttyp;		/* Controlling terminal. */
	pid_t	s_sid;		/* Session ID */
	char	s_login[MAXLOGNAME];	/* Setlogin() name. */
};

/*
 * One structure allocated per process group.
 */
struct	pgrp {
	LIST_ENTRY(pgrp) pg_hash;	/* Hash chain. */
	LIST_HEAD(, proc) pg_members;	/* Pointer to pgrp members. */
	struct	session *pg_session;	/* Pointer to session. */
	pid_t	pg_id;			/* Pgrp id. */
	int	pg_jobc;	/* # procs qualifying pgrp for job control */
};

/*
 * Description of a process.
 *
 * This structure contains the information needed to manage a thread of
 * control, known in UN*X as a process; it has references to substructures
 * containing descriptions of things that the process uses, but may share
 * with related processes.  The process structure and the substructures
 * are always addressible except for those marked "(PROC ONLY)" below,
 * which might be addressible only on a processor on which the process
 * is running.
 */
struct	proc {
	LIST_ENTRY(proc) p_list;	/* List of all processes. */

	/* substructures: */
	struct	pcred *p_cred;		/* Process owner's identity. */
	struct	filedesc *p_fd;		/* Ptr to open files structure. */
	struct	pstats *p_stats;	/* Accounting/statistics (PROC ONLY). */
	struct	plimit *p_limit;	/* Process limits. */
	struct	sigacts *p_sigacts;	/* Signal actions, state (PROC ONLY). */

#define	p_ucred		p_cred->pc_ucred
#define	p_rlimit	p_limit->pl_rlimit

	int	p_flag;			/* P_* flags. */
	char	p_stat;			/* S* process status. */
        char	p_shutdownstate;
	char	p_pad1[2];

	pid_t	p_pid;			/* Process identifier. */
	LIST_ENTRY(proc) p_pglist;	/* List of processes in pgrp. */
	struct	proc *p_pptr;	 	/* Pointer to parent process. */
	LIST_ENTRY(proc) p_sibling;	/* List of sibling processes. */
	LIST_HEAD(, proc) p_children;	/* Pointer to list of children. */

/* The following fields are all zeroed upon creation in fork. */
#define	p_startzero	p_oppid

	pid_t	p_oppid;	 /* Save parent pid during ptrace. XXX */
	int	p_dupfd;	 /* Sideways return value from fdopen. XXX */

	/* scheduling */
	u_int	p_estcpu;	 /* Time averaged value of p_cpticks. */
	int	p_cpticks;	 /* Ticks of cpu time. */
	fixpt_t	p_pctcpu;	 /* %cpu for this process during p_swtime */
	void	*p_wchan;	 /* Sleep address. */
	char	*p_wmesg;	 /* Reason for sleep. */
	u_int	p_swtime;	 /* DEPRECATED (Time swapped in or out.) */
#define	p_argslen p_swtime	 /* Length of process arguments. */
	u_int	p_slptime;	 /* Time since last blocked. */

	struct	itimerval p_realtimer;	/* Alarm timer. */
	struct	timeval p_rtime;	/* Real time. */
	u_quad_t p_uticks;		/* Statclock hits in user mode. */
	u_quad_t p_sticks;		/* Statclock hits in system mode. */
	u_quad_t p_iticks;		/* Statclock hits processing intr. */

	int	p_traceflag;		/* Kernel trace points. */
	struct	vnode *p_tracep;	/* Trace to vnode. */

	sigset_t p_siglist;		/* DEPRECATED. */

	struct	vnode *p_textvp;	/* Vnode of executable. */

/* End area that is zeroed on creation. */
#define	p_endzero	p_hash.le_next

	/*
	 * Not copied, not zero'ed.
	 * Belongs after p_pid, but here to avoid shifting proc elements.
	 */
	LIST_ENTRY(proc) p_hash;	/* Hash chain. */
	TAILQ_HEAD( ,eventqelt) p_evlist;

/* The following fields are all copied upon creation in fork. */
#define	p_startcopy	p_sigmask

	sigset_t p_sigmask;		/* DEPRECATED */
	sigset_t p_sigignore;	/* Signals being ignored. */
	sigset_t p_sigcatch;	/* Signals being caught by user. */

	u_char	p_priority;	/* Process priority. */
	u_char	p_usrpri;	/* User-priority based on p_cpu and p_nice. */
	char	p_nice;		/* Process "nice" value. */
	char	p_comm[MAXCOMLEN+1];

	struct 	pgrp *p_pgrp;	/* Pointer to process group. */
>>>>>>> origin/10.3

#ifdef XNU_KERNEL_PRIVATE
#include <mach/coalition.h>		/* COALITION_NUM_TYPES */
#endif

#if defined(XNU_KERNEL_PRIVATE) || !defined(KERNEL) 

struct session;
struct pgrp;
struct proc;

/* Exported fields for kern sysctls */
struct extern_proc {
	union {
		struct {
			struct	proc *__p_forw;	/* Doubly-linked run/sleep queue. */
			struct	proc *__p_back;
		} p_st1;
		struct timeval __p_starttime; 	/* process start time */
	} p_un;
#define p_forw p_un.p_st1.__p_forw
#define p_back p_un.p_st1.__p_back
#define p_starttime p_un.__p_starttime
	struct	vmspace *p_vmspace;	/* Address space. */
	struct	sigacts *p_sigacts;	/* Signal actions, state (PROC ONLY). */
	int	p_flag;			/* P_* flags. */
	char	p_stat;			/* S* process status. */
	pid_t	p_pid;			/* Process identifier. */
	pid_t	p_oppid;	 /* Save parent pid during ptrace. XXX */
	int	p_dupfd;	 /* Sideways return value from fdopen. XXX */
	/* Mach related  */
	caddr_t user_stack;	/* where user stack was allocated */
	void	*exit_thread;	/* XXX Which thread is exiting? */
	int		p_debugger;		/* allow to debug */
	boolean_t	sigwait;	/* indication to suspend */
	/* scheduling */
	u_int	p_estcpu;	 /* Time averaged value of p_cpticks. */
	int	p_cpticks;	 /* Ticks of cpu time. */
	fixpt_t	p_pctcpu;	 /* %cpu for this process during p_swtime */
	void	*p_wchan;	 /* Sleep address. */
	char	*p_wmesg;	 /* Reason for sleep. */
	u_int	p_swtime;	 /* Time swapped in or out. */
	u_int	p_slptime;	 /* Time since last blocked. */
	struct	itimerval p_realtimer;	/* Alarm timer. */
	struct	timeval p_rtime;	/* Real time. */
	u_quad_t p_uticks;		/* Statclock hits in user mode. */
	u_quad_t p_sticks;		/* Statclock hits in system mode. */
	u_quad_t p_iticks;		/* Statclock hits processing intr. */
	int	p_traceflag;		/* Kernel trace points. */
	struct	vnode *p_tracep;	/* Trace to vnode. */
	int	p_siglist;		/* DEPRECATED. */
	struct	vnode *p_textvp;	/* Vnode of executable. */
	int	p_holdcnt;		/* If non-zero, don't swap. */
	sigset_t p_sigmask;	/* DEPRECATED. */
	sigset_t p_sigignore;	/* Signals being ignored. */
	sigset_t p_sigcatch;	/* Signals being caught by user. */
	u_char	p_priority;	/* Process priority. */
	u_char	p_usrpri;	/* User-priority based on p_cpu and p_nice. */
	char	p_nice;		/* Process "nice" value. */
	char	p_comm[MAXCOMLEN+1];
	struct 	pgrp *p_pgrp;	/* Pointer to process group. */
	struct	user *p_addr;	/* Kernel virtual addr of u-area (PROC ONLY). */
	u_short	p_xstat;	/* Exit status for wait; also stop signal. */
	u_short	p_acflag;	/* Accounting flags. */
	struct	rusage *p_ru;	/* Exit information. XXX */
};


/* Status values. */
#define	SIDL	1		/* Process being created by fork. */
#define	SRUN	2		/* Currently runnable. */
#define	SSLEEP	3		/* Sleeping on an address. */
#define	SSTOP	4		/* Process debugging or suspension. */
#define	SZOMB	5		/* Awaiting collection by parent. */

/* These flags are kept in extern_proc.p_flag. */
#define	P_ADVLOCK	0x00000001	/* Process may hold POSIX adv. lock */
#define	P_CONTROLT	0x00000002	/* Has a controlling terminal */
#define	P_LP64		0x00000004	/* Process is LP64 */
#define	P_NOCLDSTOP	0x00000008	/* No SIGCHLD when children stop */

#define	P_PPWAIT	0x00000010	/* Parent waiting for chld exec/exit */
#define	P_PROFIL	0x00000020	/* Has started profiling */
#define	P_SELECT	0x00000040	/* Selecting; wakeup/waiting danger */
#define	P_CONTINUED	0x00000080	/* Process was stopped and continued */

#define	P_SUGID		0x00000100	/* Has set privileges since last exec */
#define	P_SYSTEM	0x00000200	/* Sys proc: no sigs, stats or swap */
#define	P_TIMEOUT	0x00000400	/* Timing out during sleep */
#define	P_TRACED	0x00000800	/* Debugged process being traced */

#define	P_DISABLE_ASLR	0x00001000	/* Disable address space layout randomization */
#define	P_WEXIT		0x00002000	/* Working on exiting */
#define	P_EXEC		0x00004000	/* Process called exec. */

/* Should be moved to machine-dependent areas. */
#define	P_OWEUPC	0x00008000	/* Owe process an addupc() call at next ast. */

#define	P_AFFINITY	0x00010000	/* xxx */
#define	P_TRANSLATED	0x00020000	/* xxx */
#define	P_CLASSIC	P_TRANSLATED	/* xxx */

#define	P_DELAYIDLESLEEP 0x00040000	/* Process is marked to delay idle sleep on disk IO */
#define	P_CHECKOPENEVT 	0x00080000	/* check if a vnode has the OPENEVT flag set on open */

#define	P_DEPENDENCY_CAPABLE	0x00100000	/* process is ok to call vfs_markdependency() */
#define	P_REBOOT	0x00200000	/* Process called reboot() */
#define	P_RESV6		0x00400000	/* used to be P_TBE */
#define	P_RESV7		0x00800000	/* (P_SIGEXC)signal exceptions */

#define	P_THCWD		0x01000000	/* process has thread cwd  */
#define	P_RESV9		0x02000000	/* (P_VFORK)process has vfork children */
#define	P_RESV10	0x04000000	/* reserved flag */
#define	P_RESV11	0x08000000	/* (P_INVFORK) proc in vfork */

#define	P_NOSHLIB	0x10000000	/* no shared libs are in use for proc */
					/* flag set on exec */
#define	P_FORCEQUOTA	0x20000000	/* Force quota for root */
#define	P_NOCLDWAIT	0x40000000	/* No zombies when chil procs exit */
#define	P_NOREMOTEHANG	0x80000000	/* Don't hang on remote FS ops */

#define	P_INMEM		0		/* Obsolete: retained for compilation */
#define	P_NOSWAP	0		/* Obsolete: retained for compilation */
#define	P_PHYSIO	0		/* Obsolete: retained for compilation */
#define	P_FSTRACE	0		/* Obsolete: retained for compilation */
#define	P_SSTEP		0		/* Obsolete: retained for compilation */

#define P_DIRTY_TRACK                           0x00000001      /* track dirty state */
#define P_DIRTY_ALLOW_IDLE_EXIT                 0x00000002      /* process can be idle-exited when clean */
#define P_DIRTY_DEFER                           0x00000004      /* defer initial opt-in to idle-exit */
#define P_DIRTY                                 0x00000008      /* process is dirty */
#define P_DIRTY_SHUTDOWN                        0x00000010      /* process is dirty during shutdown */
#define P_DIRTY_TERMINATED                      0x00000020      /* process has been marked for termination */
#define P_DIRTY_BUSY                            0x00000040      /* serialization flag */
#define P_DIRTY_MARKED                          0x00000080      /* marked dirty previously */
#define P_DIRTY_DEFER_IN_PROGRESS               0x00000100      /* deferral to idle-band in process */
#define P_DIRTY_LAUNCH_IN_PROGRESS              0x00000200      /* launch is in progress */

#define P_DIRTY_IS_DIRTY                        (P_DIRTY | P_DIRTY_SHUTDOWN)
#define P_DIRTY_IDLE_EXIT_ENABLED               (P_DIRTY_TRACK|P_DIRTY_ALLOW_IDLE_EXIT)

#endif /* XNU_KERNEL_PRIVATE || !KERNEL */

#ifdef KERNEL
__BEGIN_DECLS

extern proc_t kernproc;

extern int proc_is_classic(proc_t p);
proc_t current_proc_EXTERNAL(void);

extern int	msleep(void *chan, lck_mtx_t *mtx, int pri, const char *wmesg, struct timespec * ts );
extern void	wakeup(void *chan);
extern void wakeup_one(caddr_t chan);

/* proc kpis */
/* this routine returns the pid of the current process */
extern int proc_selfpid(void);
/* this routine returns the pid of the parent of the current process */
extern int proc_selfppid(void);
/* this routine returns sends a signal signum to the process identified by the pid */
extern void proc_signal(int pid, int signum);
/* this routine checks whether any signal identified by the mask are pending in the  process identified by the pid. The check is  on all threads of the process. */
extern int proc_issignal(int pid, sigset_t mask);
/* this routine returns 1 if the pid1 is inferior of pid2 */
extern int proc_isinferior(int pid1, int pid2);
/* this routine copies the process's name of the executable to the passed in buffer. It 
 * is always null terminated. The size of the buffer is to be passed in as well. This 
 * routine is to be used typically for debugging 
 */
void proc_name(int pid, char * buf, int size);
/* This routine is simillar to proc_name except it returns for current process */
void proc_selfname(char * buf, int size);

/* find a process with a given pid. This comes with a reference which needs to be dropped by proc_rele */
extern proc_t proc_find(int pid);
/* returns a handle to current process which is referenced. The reference needs to be dropped with proc_rele */
extern proc_t proc_self(void);
/* releases the held reference on the process */
extern int proc_rele(proc_t p);
/* returns the pid of the given process */
extern int proc_pid(proc_t);
/* returns the pid of the parent of a given process */
extern int proc_ppid(proc_t);
/* returns 1 if the process is marked for no remote hangs */
extern int proc_noremotehang(proc_t);
/* returns 1 if the process is marked for force quota */
extern int proc_forcequota(proc_t);
/* returns 1 if the process is chrooted */
extern int proc_chrooted(proc_t);

/* this routine returns 1 if the process is running with 64bit address space, else 0 */
extern int proc_is64bit(proc_t);
/* is this process exiting? */
extern int proc_exiting(proc_t);
/* this routine returns error if the process is not one with super user privileges */
int proc_suser(proc_t p);
/* returns the cred assicaited with the process; temporary api */
kauth_cred_t proc_ucred(proc_t p);

extern int proc_tbe(proc_t);

/*!
 @function proc_selfpgrpid
 @abstract Get the process group id for the current process, as with proc_pgrpid().
 @return pgrpid of current process.
 */
<<<<<<< HEAD
pid_t proc_selfpgrpid(void);
=======
extern int nprocs, maxproc;		/* Current and max number of procs. */
__private_extern__ int hard_maxproc;	/* hard limit */

#define	PID_MAX		30000
#define	NO_PID		30001

#define SESS_LEADER(p)	((p)->p_session->s_leader == (p))
#define	SESSHOLD(s)	((s)->s_count++)
#define	SESSRELE(s)	sessrele(s)

#define	PIDHASH(pid)	(&pidhashtbl[(pid) & pidhash])
extern LIST_HEAD(pidhashhead, proc) *pidhashtbl;
extern u_long pidhash;

#define	PGRPHASH(pgid)	(&pgrphashtbl[(pgid) & pgrphash])
extern LIST_HEAD(pgrphashhead, pgrp) *pgrphashtbl;
extern u_long pgrphash;

LIST_HEAD(proclist, proc);
extern struct proclist allproc;		/* List of all processes. */
extern struct proclist zombproc;	/* List of zombie processes. */
extern struct proc *initproc, *kernproc;
extern void	pgdelete __P((struct pgrp *pgrp));
extern void	sessrele __P((struct session *sess));
extern void	procinit __P((void));
__private_extern__ char *proc_core_name(const char *name, uid_t uid, pid_t pid);
extern int proc_is_classic(struct proc *p);
struct proc *current_proc_EXTERNAL(void);
#endif /* __APPLE_API_PRIVATE */

#ifdef __APPLE_API_UNSTABLE

extern int isinferior(struct proc *, struct proc *);
extern struct	proc *pfind __P((pid_t));	/* Find process by id. */
__private_extern__ struct proc *pzfind(pid_t);	/* Find zombie by id. */
extern struct	pgrp *pgfind __P((pid_t));	/* Find process group by id. */

extern int	chgproccnt __P((uid_t uid, int diff));
extern int	enterpgrp __P((struct proc *p, pid_t pgid, int mksess));
extern void	fixjobc __P((struct proc *p, struct pgrp *pgrp, int entering));
extern int	inferior __P((struct proc *p));
extern int	leavepgrp __P((struct proc *p));
#ifdef __APPLE_API_OBSOLETE
extern void	mi_switch __P((void));
#endif /* __APPLE_API_OBSOLETE */
extern void	resetpriority __P((struct proc *));
extern void	setrunnable __P((struct proc *));
extern void	setrunqueue __P((struct proc *));
extern int	sleep __P((void *chan, int pri));
extern int	tsleep __P((void *chan, int pri, char *wmesg, int timo));
extern int	tsleep0 __P((void *chan, int pri, char *wmesg, int timo, int (*continuation)(int)));
extern int	tsleep1 __P((void *chan, int pri, char *wmesg, u_int64_t abstime, int (*continuation)(int)));
extern void	unsleep __P((struct proc *));
extern void	wakeup __P((void *chan));
#endif /* __APPLE_API_UNSTABLE */
>>>>>>> origin/10.3

/*!
 @function proc_pgrpid
 @abstract Get the process group id for the passed-in process.
 @param p Process whose pgrpid to grab.
 @return pgrpid for "p".
 */
pid_t proc_pgrpid(proc_t);

#ifdef KERNEL_PRIVATE
// mark a process as being allowed to call vfs_markdependency()
void bsd_set_dependency_capable(task_t task);
extern int IS_64BIT_PROCESS(proc_t);

extern int	tsleep(void *chan, int pri, const char *wmesg, int timo);
extern int	msleep1(void *chan, lck_mtx_t *mtx, int pri, const char *wmesg, u_int64_t timo);

<<<<<<< HEAD
task_t proc_task(proc_t);
extern int proc_pidversion(proc_t);
extern int proc_getcdhash(proc_t, unsigned char *);

/*! 
 @function    proc_pidbackgrounded
 @abstract    KPI to determine if a process is currently backgrounded.
 @discussion  The process may move into or out of background state at any time, 
              so be prepared for this value to be outdated immediately. 
 @param pid   PID of the process to be queried.
 @param state Pointer to a value which will be set to 1 if the process
              is currently backgrounded, 0 otherwise. 
 @return      ESRCH if pid cannot be found or has started exiting.

              EINVAL if state is NULL.
 */
extern int proc_pidbackgrounded(pid_t pid, uint32_t* state);

/* 
 * This returns an unique 64bit id of a given process. 
 * Caller needs to hold proper reference on the 
 * passed in process strucutre.
 */
extern uint64_t proc_uniqueid(proc_t);

extern void proc_set_responsible_pid(proc_t target_proc, pid_t responsible_pid);

#endif /* KERNEL_PRIVATE */

#ifdef XNU_KERNEL_PRIVATE

/* unique 64bit id for process's original parent */
extern uint64_t proc_puniqueid(proc_t);

extern void proc_getexecutableuuid(proc_t, unsigned char *, unsigned long);
extern int proc_get_originatorbgstate(uint32_t *is_backgrounded);

/* Kernel interface to get the uuid of the originator of the work.*/
extern int proc_pidoriginatoruuid(uuid_t uuid_buf, uint32_t buffersize);

extern uint64_t proc_was_throttled(proc_t);
extern uint64_t proc_did_throttle(proc_t);

extern void proc_coalitionids(proc_t, uint64_t [COALITION_NUM_TYPES]);

#endif /* XNU_KERNEL_PRIVATE*/

#ifdef KERNEL_PRIVATE
extern vnode_t proc_getexecutablevnode(proc_t); /* Returned with iocount, use vnode_put() to drop */
#endif
=======
extern int proc_pidversion(proc_t);
extern int proc_getcdhash(proc_t, unsigned char *);
#endif /* KERNEL_PRIVATE */
>>>>>>> origin/10.5

__END_DECLS

#endif	/* KERNEL */

#ifdef PRIVATE

/* Values for pid_shutdown_sockets */
#ifdef KERNEL
#define SHUTDOWN_SOCKET_LEVEL_DISCONNECT_INTERNAL	0x0
#endif /* KERNEL */
#define SHUTDOWN_SOCKET_LEVEL_DISCONNECT_SVC		0x1
#define SHUTDOWN_SOCKET_LEVEL_DISCONNECT_ALL		0x2

#ifndef KERNEL

__BEGIN_DECLS

int pid_suspend(int pid);
int pid_resume(int pid);


__END_DECLS

#endif /* !KERNEL */
#endif /* PRIVATE */

#endif	/* !_SYS_PROC_H_ */
