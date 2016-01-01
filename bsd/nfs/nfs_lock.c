/*
<<<<<<< HEAD
 * Copyright (c) 2002-2014 Apple Inc. All rights reserved.
 *
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
 * Copyright (c) 2002-2004 Apple Computer, Inc. All rights reserved.
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
>>>>>>> origin/10.3
 * EXPRESS OR IMPLIED, AND APPLE HEREBY DISCLAIMS ALL SUCH WARRANTIES,
 * INCLUDING WITHOUT LIMITATION, ANY WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE OR NON-INFRINGEMENT.  Please see the
 * License for the specific language governing rights and limitations
 * under the License.
 * 
 * @APPLE_OSREFERENCE_LICENSE_HEADER_END@
 */
/*-
 * Copyright (c) 1997 Berkeley Software Design, Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Berkeley Software Design Inc's name may not be used to endorse or
 *    promote products derived from this software without specific prior
 *    written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY BERKELEY SOFTWARE DESIGN INC ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL BERKELEY SOFTWARE DESIGN INC BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 *      from BSDI nfs_lock.c,v 2.4 1998/12/14 23:49:56 jch Exp
 */

#include <sys/cdefs.h>
#include <sys/param.h>
#include <sys/systm.h>
#include <sys/fcntl.h>
#include <sys/kernel.h>		/* for hz */
#include <sys/file_internal.h>
#include <sys/malloc.h>
#include <sys/lockf.h>		/* for hz */ /* Must come after sys/malloc.h */
#include <sys/kpi_mbuf.h>
#include <sys/mount_internal.h>
#include <sys/proc_internal.h>	/* for p_start */
#include <sys/kauth.h>
#include <sys/resourcevar.h>
#include <sys/socket.h>
#include <sys/unistd.h>
#include <sys/user.h>
#include <sys/vnode_internal.h>

#include <kern/thread.h>
#include <kern/host.h>

#include <machine/limits.h>

#include <net/if.h>

#include <nfs/rpcv2.h>
#include <nfs/nfsproto.h>
#include <nfs/nfs.h>
#include <nfs/nfs_gss.h>
#include <nfs/nfsmount.h>
#include <nfs/nfsnode.h>
#include <nfs/nfs_lock.h>

#include <mach/host_priv.h>
#include <mach/mig_errors.h>
#include <mach/host_special_ports.h>
#include <lockd/lockd_mach.h>

extern void ipc_port_release_send(ipc_port_t);

/*
 * pending lock request messages are kept in this queue which is
 * kept sorted by transaction ID (xid).
 */
static uint64_t nfs_lockxid = 0;
static LOCKD_MSG_QUEUE nfs_pendlockq;

/* list of mounts that are (potentially) making lockd requests */
TAILQ_HEAD(nfs_lockd_mount_list,nfsmount) nfs_lockd_mount_list;

static lck_grp_t *nfs_lock_lck_grp;
static lck_mtx_t *nfs_lock_mutex;

<<<<<<< HEAD
void nfs_lockdmsg_enqueue(LOCKD_MSG_REQUEST *);
void nfs_lockdmsg_dequeue(LOCKD_MSG_REQUEST *);
int nfs_lockdmsg_compare_to_answer(LOCKD_MSG_REQUEST *, struct lockd_ans *);
LOCKD_MSG_REQUEST *nfs_lockdmsg_find_by_answer(struct lockd_ans *);
LOCKD_MSG_REQUEST *nfs_lockdmsg_find_by_xid(uint64_t);
uint64_t nfs_lockxid_get(void);
int nfs_lockd_send_request(LOCKD_MSG *, int);

/*
 * initialize global nfs lock state
=======
/*
 * globals for managing the lockd fifo
 */
pid_t nfslockdpid = 0;
struct file *nfslockdfp = 0;
int nfslockdwaiting = 0;
int nfslockdfifowritten = 0;
int nfslockdfifolock = 0;
#define NFSLOCKDFIFOLOCK_LOCKED	1
#define NFSLOCKDFIFOLOCK_WANT	2

/*
 * pending lock request messages are kept in this queue which is
 * kept sorted by transaction ID (xid).
 */
uint64_t nfs_lockxid = 0;
LOCKD_MSG_QUEUE nfs_pendlockq;

/*
 * This structure is used to identify processes which have acquired NFS locks.
 * Knowing which processes have ever acquired locks allows us to short-circuit
 * unlock requests for processes that have never had an NFS file lock.  Thus
 * avoiding a costly and unnecessary lockd request.
 */
struct nfs_lock_pid {
	TAILQ_ENTRY(nfs_lock_pid)	lp_lru;		/* LRU list */
	LIST_ENTRY(nfs_lock_pid)	lp_hash;	/* hash chain */
	int				lp_valid;	/* valid entry? */
	int				lp_time;	/* last time seen valid */
	pid_t				lp_pid;		/* The process ID. */
	struct timeval			lp_pid_start;	/* Start time of process id */
};

#define NFS_LOCK_PID_HASH_SIZE		64	// XXX tune me
#define	NFS_LOCK_PID_HASH(pid)	\
	(&nfs_lock_pid_hash_tbl[(pid) & nfs_lock_pid_hash])
LIST_HEAD(, nfs_lock_pid) *nfs_lock_pid_hash_tbl;
TAILQ_HEAD(, nfs_lock_pid) nfs_lock_pid_lru;
u_long nfs_lock_pid_hash;
int nfs_lock_pid_lock;


/*
 * initialize global nfs lock state
 */
void
nfs_lockinit(void)
{
	TAILQ_INIT(&nfs_pendlockq);
	nfs_lock_pid_lock = 0;
	nfs_lock_pid_hash_tbl = hashinit(NFS_LOCK_PID_HASH_SIZE,
					 M_TEMP, &nfs_lock_pid_hash);
	TAILQ_INIT(&nfs_lock_pid_lru);
}

/*
 * insert a lock request message into the pending queue
 */
static inline void
nfs_lockdmsg_enqueue(LOCKD_MSG_REQUEST *msgreq)
{
	LOCKD_MSG_REQUEST *mr;

	mr = TAILQ_LAST(&nfs_pendlockq, nfs_lock_msg_queue);
	if (!mr || (msgreq->lmr_msg.lm_xid > mr->lmr_msg.lm_xid)) {
		/* fast path: empty queue or new largest xid */
		TAILQ_INSERT_TAIL(&nfs_pendlockq, msgreq, lmr_next);
		return;
	}
	/* slow path: need to walk list to find insertion point */
	while (mr && (msgreq->lmr_msg.lm_xid > mr->lmr_msg.lm_xid)) {
		mr = TAILQ_PREV(mr, nfs_lock_msg_queue, lmr_next);
	}
	if (mr) {
		TAILQ_INSERT_AFTER(&nfs_pendlockq, mr, msgreq, lmr_next);
	} else {
		TAILQ_INSERT_HEAD(&nfs_pendlockq, msgreq, lmr_next);
	}
}

/*
 * remove a lock request message from the pending queue
 */
static inline void
nfs_lockdmsg_dequeue(LOCKD_MSG_REQUEST *msgreq)
{
	TAILQ_REMOVE(&nfs_pendlockq, msgreq, lmr_next);
}

/*
 * find a pending lock request message by xid
 *
 * We search from the head of the list assuming that the message we're
 * looking for is for an older request (because we have an answer to it).
 * This assumes that lock request will be answered primarily in FIFO order.
 * However, this may not be the case if there are blocked requests.  We may
 * want to move blocked requests to a separate queue (but that'll complicate
 * duplicate xid checking).
 */
static inline LOCKD_MSG_REQUEST *
nfs_lockdmsg_find_by_xid(uint64_t lockxid)
{
	LOCKD_MSG_REQUEST *mr;

	TAILQ_FOREACH(mr, &nfs_pendlockq, lmr_next) {
		if (mr->lmr_msg.lm_xid == lockxid)
			return mr;
		if (mr->lmr_msg.lm_xid > lockxid)
			return NULL;
	}
	return mr;
}

/*
 * Because we can't depend on nlm_granted messages containing the same
 * cookie we sent with the original lock request, we need code test if
 * an nlm_granted answer matches the lock request.  We also need code
 * that can find a lockd message based solely on the nlm_granted answer.
>>>>>>> origin/10.3
 */
void
nfs_lockinit(void)
{
	TAILQ_INIT(&nfs_pendlockq);
	TAILQ_INIT(&nfs_lockd_mount_list);

	nfs_lock_lck_grp = lck_grp_alloc_init("nfs_lock", LCK_GRP_ATTR_NULL);
	nfs_lock_mutex = lck_mtx_alloc_init(nfs_lock_lck_grp, LCK_ATTR_NULL);
}

/*
<<<<<<< HEAD
 * Register a mount as (potentially) making lockd requests.
=======
 * compare lockd message to answer
 *
 * returns 0 on equality and 1 if different
 */
static inline int
nfs_lockdmsg_compare_to_answer(LOCKD_MSG_REQUEST *msgreq, struct lockd_ans *ansp)
{
	if (!(ansp->la_flags & LOCKD_ANS_LOCK_INFO))
		return 1;
	if (msgreq->lmr_msg.lm_fl.l_pid != ansp->la_pid)
		return 1;
	if (msgreq->lmr_msg.lm_fl.l_start != ansp->la_start)
		return 1;
	if (msgreq->lmr_msg.lm_fl.l_len != ansp->la_len)
		return 1;
	if (msgreq->lmr_msg.lm_fh_len != ansp->la_fh_len)
		return 1;
	if (bcmp(msgreq->lmr_msg.lm_fh, ansp->la_fh, ansp->la_fh_len))
		return 1;
	return 0;
}

/*
 * find a pending lock request message based on the lock info provided
 * in the lockd_ans/nlm_granted data.  We need this because we can't
 * depend on nlm_granted messages containing the same cookie we sent
 * with the original lock request.
 *
 * We search from the head of the list assuming that the message we're
 * looking for is for an older request (because we have an answer to it).
 * This assumes that lock request will be answered primarily in FIFO order.
 * However, this may not be the case if there are blocked requests.  We may
 * want to move blocked requests to a separate queue (but that'll complicate
 * duplicate xid checking).
 */
static inline LOCKD_MSG_REQUEST *
nfs_lockdmsg_find_by_answer(struct lockd_ans *ansp)
{
	LOCKD_MSG_REQUEST *mr;

	if (!(ansp->la_flags & LOCKD_ANS_LOCK_INFO))
		return NULL;
	TAILQ_FOREACH(mr, &nfs_pendlockq, lmr_next) {
		if (!nfs_lockdmsg_compare_to_answer(mr, ansp))
			break;
	}
	return mr;
}

/*
 * return the next unique lock request transaction ID
 */
static inline uint64_t
nfs_lockxid_get(void)
{
	LOCKD_MSG_REQUEST *mr;

	/* derive initial lock xid from system time */
	if (!nfs_lockxid) {
		/*
		 * Note: it's OK if this code inits nfs_lockxid to 0 (for example,
		 * due to a broken clock) because we immediately increment it
		 * and we guarantee to never use xid 0.  So, nfs_lockxid should only
		 * ever be 0 the first time this function is called.
		 */
		struct timeval tv;
		microtime(&tv);
		nfs_lockxid = (uint64_t)tv.tv_sec << 12;
	}

	/* make sure we get a unique xid */
	do {
		/* Skip zero xid if it should ever happen.  */
		if (++nfs_lockxid == 0)
			nfs_lockxid++;
		if (!(mr = TAILQ_LAST(&nfs_pendlockq, nfs_lock_msg_queue)) ||
		     (mr->lmr_msg.lm_xid < nfs_lockxid)) {
			/* fast path: empty queue or new largest xid */
			break;
		}
		/* check if xid is already in use */
	} while (nfs_lockdmsg_find_by_xid(nfs_lockxid));

	return nfs_lockxid;
}


/*
 * Check the nfs_lock_pid hash table for an entry and, if requested,
 * add the entry if it is not found.
 *
 * (Also, if adding, try to clean up some stale entries.)
 */
static int
nfs_lock_pid_check(struct proc *p, int addflag, struct vnode *vp)
{
	struct nfs_lock_pid *lp, *lplru, *lplru_next;
	struct proc *plru;
	int error = 0;
	struct timeval now;

	/* lock hash */
loop:
	if (nfs_lock_pid_lock) {
		while (nfs_lock_pid_lock) {
			nfs_lock_pid_lock = -1;
			tsleep(&nfs_lock_pid_lock, PCATCH, "nfslockpid", 0);
			if ((error = nfs_sigintr(VFSTONFS(vp->v_mount), NULL, p)))
				return (error);
		}
		goto loop;
	}
	nfs_lock_pid_lock = 1;

	/* Search hash chain */
	error = ENOENT;
	lp = NFS_LOCK_PID_HASH(p->p_pid)->lh_first;
	for (; lp != NULL; lp = lp->lp_hash.le_next)
		if (lp->lp_pid == p->p_pid) {
			/* found pid... */
			if (timevalcmp(&lp->lp_pid_start, &p->p_stats->p_start, ==)) {
				/* ...and it's valid */
				/* move to tail of LRU */
				TAILQ_REMOVE(&nfs_lock_pid_lru, lp, lp_lru);
				microuptime(&now);
				lp->lp_time = now.tv_sec;
				TAILQ_INSERT_TAIL(&nfs_lock_pid_lru, lp, lp_lru);
				error = 0;
				break;
			}
			/* ...but it's no longer valid */
			/* remove from hash, invalidate, and move to lru head */
			LIST_REMOVE(lp, lp_hash);
			lp->lp_valid = 0;
			TAILQ_REMOVE(&nfs_lock_pid_lru, lp, lp_lru);
			TAILQ_INSERT_HEAD(&nfs_lock_pid_lru, lp, lp_lru);
			lp = NULL;
			break;
		}

	/* if we didn't find it (valid) and we've been asked to add it */
	if ((error == ENOENT) && addflag) {
		/* scan lru list for invalid, stale entries to reuse/free */
		int lrucnt = 0;
		microuptime(&now);
		for (lplru = TAILQ_FIRST(&nfs_lock_pid_lru); lplru; lplru = lplru_next) {
			lplru_next = TAILQ_NEXT(lplru, lp_lru);
			if (lplru->lp_valid && (lplru->lp_time >= (now.tv_sec - 2))) {
				/*
				 * If the oldest LRU entry is relatively new, then don't
				 * bother scanning any further.
				 */
				break;
			}
			/* remove entry from LRU, and check if it's still in use */
			TAILQ_REMOVE(&nfs_lock_pid_lru, lplru, lp_lru);
			if (!lplru->lp_valid || !(plru = pfind(lplru->lp_pid)) ||
			    timevalcmp(&lplru->lp_pid_start, &plru->p_stats->p_start, !=)) {
				/* no longer in use */
				LIST_REMOVE(lplru, lp_hash);
				if (!lp) {
					/* we'll reuse this one */
					lp = lplru;
				} else {
					/* we can free this one */
					FREE(lplru, M_TEMP);
				}
			} else {
				/* still in use */
				lplru->lp_time = now.tv_sec;
				TAILQ_INSERT_TAIL(&nfs_lock_pid_lru, lplru, lp_lru);
			}
			/* don't check too many entries at once */
			if (++lrucnt > 8)
				break;
		}
		if (!lp) {
			/* we need to allocate a new one */
			MALLOC(lp, struct nfs_lock_pid *, sizeof(struct nfs_lock_pid),
				M_TEMP, M_WAITOK | M_ZERO);
		}
		/* (re)initialize nfs_lock_pid info */
		lp->lp_pid = p->p_pid;
		lp->lp_pid_start = p->p_stats->p_start;
		/* insert pid in hash */
		LIST_INSERT_HEAD(NFS_LOCK_PID_HASH(lp->lp_pid), lp, lp_hash);
		lp->lp_valid = 1;
		lp->lp_time = now.tv_sec;
		TAILQ_INSERT_TAIL(&nfs_lock_pid_lru, lp, lp_lru);
		error = 0;
	}

	/* unlock hash */
	if (nfs_lock_pid_lock < 0) {
		nfs_lock_pid_lock = 0;
		wakeup(&nfs_lock_pid_lock);
	} else
		nfs_lock_pid_lock = 0;

	return (error);
}


/*
 * nfs_advlock --
 *      NFS advisory byte-level locks.
>>>>>>> origin/10.3
 */
void
nfs_lockd_mount_register(struct nfsmount *nmp)
{
<<<<<<< HEAD
	lck_mtx_lock(nfs_lock_mutex);
	TAILQ_INSERT_HEAD(&nfs_lockd_mount_list, nmp, nm_ldlink);
	nfs_lockd_mounts++;
	lck_mtx_unlock(nfs_lock_mutex);
}

/*
 * Unregister a mount as (potentially) making lockd requests.
 *
 * When the lockd mount count drops to zero, then send a shutdown request to
 * lockd if we've sent any requests to it.
 */
void
nfs_lockd_mount_unregister(struct nfsmount *nmp)
{
	int send_shutdown;
	mach_port_t lockd_port = IPC_PORT_NULL;
	kern_return_t kr;

	lck_mtx_lock(nfs_lock_mutex);
	if (nmp->nm_ldlink.tqe_next == NFSNOLIST) {
		lck_mtx_unlock(nfs_lock_mutex);
		return;
	}
	
	TAILQ_REMOVE(&nfs_lockd_mount_list, nmp, nm_ldlink);
	nmp->nm_ldlink.tqe_next = NFSNOLIST;

	nfs_lockd_mounts--;
=======
	LOCKD_MSG_REQUEST msgreq;
	LOCKD_MSG *msg;
	struct vnode *vp, *wvp;
	struct nfsnode *np;
	int error, error1;
	struct flock *fl;
	int fmode, ioflg;
	struct proc *p;
	struct nfsmount *nmp;
	struct vattr vattr;
	off_t start, end;
	struct timeval now;
	int timeo, endtime, lastmsg, wentdown = 0;
	int lockpidcheck;

	p = current_proc();
>>>>>>> origin/10.3

	/* send a shutdown request if there are no more lockd mounts */
	send_shutdown = ((nfs_lockd_mounts == 0) && nfs_lockd_request_sent);
	if (send_shutdown)
		nfs_lockd_request_sent = 0;

	lck_mtx_unlock(nfs_lock_mutex);

	if (!send_shutdown)
		return;

	/*
<<<<<<< HEAD
	 * Let lockd know that it is no longer needed for any NFS mounts
	 */
	kr = host_get_lockd_port(host_priv_self(), &lockd_port);
	if ((kr != KERN_SUCCESS) || !IPC_PORT_VALID(lockd_port)) {
		printf("nfs_lockd_mount_change: shutdown couldn't get port, kr %d, port %s\n",
			kr, (lockd_port == IPC_PORT_NULL) ? "NULL" :
			(lockd_port == IPC_PORT_DEAD) ? "DEAD" : "VALID");
		return;
	}

	kr = lockd_shutdown(lockd_port);
	if (kr != KERN_SUCCESS)
		printf("nfs_lockd_mount_change: shutdown %d\n", kr);

	ipc_port_release_send(lockd_port);
}

/*
 * insert a lock request message into the pending queue
 * (nfs_lock_mutex must be held)
 */
void
nfs_lockdmsg_enqueue(LOCKD_MSG_REQUEST *msgreq)
{
	LOCKD_MSG_REQUEST *mr;

	mr = TAILQ_LAST(&nfs_pendlockq, nfs_lock_msg_queue);
	if (!mr || (msgreq->lmr_msg.lm_xid > mr->lmr_msg.lm_xid)) {
		/* fast path: empty queue or new largest xid */
		TAILQ_INSERT_TAIL(&nfs_pendlockq, msgreq, lmr_next);
		return;
=======
	 * The NLM protocol doesn't allow the server to return an error
	 * on ranges, so we do it.  Pre LFS (Large File Summit)
	 * standards required EINVAL for the range errors.  More recent
	 * standards use EOVERFLOW, but their EINVAL wording still
	 * encompasses these errors.
	 * Any code sensitive to this is either:
	 *  1) written pre-LFS and so can handle only EINVAL, or
	 *  2) written post-LFS and thus ought to be tolerant of pre-LFS
	 *     implementations.
	 * Since returning EOVERFLOW certainly breaks 1), we return EINVAL.
	 */
	if (fl->l_whence != SEEK_END) {
		if ((fl->l_whence != SEEK_CUR && fl->l_whence != SEEK_SET) ||
		    fl->l_start < 0 ||
		    (fl->l_len > 0 && fl->l_len - 1 > OFF_MAX - fl->l_start) ||
		    (fl->l_len < 0 && fl->l_start + fl->l_len < 0))
			return (EINVAL);
	}
	/*
	 * If daemon is running take a ref on its fifo
	 */
	if (!nfslockdfp || !(wvp = (struct vnode *)nfslockdfp->f_data)) {
		if (!nfslockdwaiting)
			return (EOPNOTSUPP);
		/*
		 * Don't wake lock daemon if it hasn't been started yet and
		 * this is an unlock request (since we couldn't possibly
		 * actually have a lock on the file).  This could be an
		 * uninformed unlock request due to closef()'s behavior of doing
		 * unlocks on all files if a process has had a lock on ANY file.
		 */
		if (!nfslockdfp && (fl->l_type == F_UNLCK))
			return (EINVAL);
		/* wake up lock daemon */
		(void)wakeup((void *)&nfslockdwaiting);
		/* wait on nfslockdfp for a while to allow daemon to start */
		tsleep((void *)&nfslockdfp, PCATCH | PUSER, "lockd", 60*hz);
		/* check for nfslockdfp and f_data */
		if (!nfslockdfp || !(wvp = (struct vnode *)nfslockdfp->f_data))
			return (EOPNOTSUPP);
	}
	VREF(wvp);

	/*
	 * Need to check if this process has successfully acquired an NFS lock before.
	 * If not, and this is an unlock request we can simply return success here.
	 */
	lockpidcheck = nfs_lock_pid_check(p, 0, vp);
	if (lockpidcheck) {
		if (lockpidcheck != ENOENT)
			return (lockpidcheck);
		if (ap->a_op == F_UNLCK) {
			vrele(wvp);
			return (0);
		}
	}

	/*
	 * The NFS Lock Manager protocol doesn't directly handle
	 * negative lengths or SEEK_END, so we need to normalize
	 * things here where we have all the info.
	 * (Note: SEEK_CUR is already adjusted for at this point)
	 */
	/* Convert the flock structure into a start and end. */
	switch (fl->l_whence) {
	case SEEK_SET:
	case SEEK_CUR:
		/*
		 * Caller is responsible for adding any necessary offset
		 * to fl->l_start when SEEK_CUR is used.
		 */
		start = fl->l_start;
		break;
	case SEEK_END:
		/* need to flush, and refetch attributes to make */
		/* sure we have the correct end of file offset   */
		if (np->n_flag & NMODIFIED) {
			np->n_xid = 0;
			error = nfs_vinvalbuf(vp, V_SAVE, p->p_ucred, p, 1);
			if (error) {
				vrele(wvp);
				return (error);
			}
		}
		np->n_xid = 0;
		error = VOP_GETATTR(vp, &vattr, p->p_ucred, p);
		if (error) {
			vrele(wvp);
			return (error);
		}
		start = np->n_size + fl->l_start;
		break;
	default:
		vrele(wvp);
		return (EINVAL);
>>>>>>> origin/10.3
	}
	/* slow path: need to walk list to find insertion point */
	while (mr && (msgreq->lmr_msg.lm_xid > mr->lmr_msg.lm_xid)) {
		mr = TAILQ_PREV(mr, nfs_lock_msg_queue, lmr_next);
	}
	if (mr) {
		TAILQ_INSERT_AFTER(&nfs_pendlockq, mr, msgreq, lmr_next);
	} else {
		TAILQ_INSERT_HEAD(&nfs_pendlockq, msgreq, lmr_next);
	}
<<<<<<< HEAD
}

/*
 * remove a lock request message from the pending queue
 * (nfs_lock_mutex must be held)
 */
void
nfs_lockdmsg_dequeue(LOCKD_MSG_REQUEST *msgreq)
{
	TAILQ_REMOVE(&nfs_pendlockq, msgreq, lmr_next);
}

/*
 * find a pending lock request message by xid
 *
 * We search from the head of the list assuming that the message we're
 * looking for is for an older request (because we have an answer to it).
 * This assumes that lock request will be answered primarily in FIFO order.
 * However, this may not be the case if there are blocked requests.  We may
 * want to move blocked requests to a separate queue (but that'll complicate
 * duplicate xid checking).
 *
 * (nfs_lock_mutex must be held)
 */
LOCKD_MSG_REQUEST *
nfs_lockdmsg_find_by_xid(uint64_t lockxid)
{
	LOCKD_MSG_REQUEST *mr;
=======
	if (!NFS_ISV3(vp) &&
	    ((start >= 0x80000000) || (end >= 0x80000000))) {
		vrele(wvp);
		return (EINVAL);
	}

	/*
	 * Fill in the information structure.
	 */
	msgreq.lmr_answered = 0;
	msgreq.lmr_errno = 0;
	msgreq.lmr_saved_errno = 0;
	msg = &msgreq.lmr_msg;
	msg->lm_version = LOCKD_MSG_VERSION;
	msg->lm_flags = 0;

	msg->lm_fl = *fl;
	msg->lm_fl.l_start = start;
	if (end != -1)
		msg->lm_fl.l_len = end - start + 1;
	msg->lm_fl.l_pid = p->p_pid;

	if (ap->a_flags & F_WAIT)
		msg->lm_flags |= LOCKD_MSG_BLOCK;
	if (ap->a_op == F_GETLK)
		msg->lm_flags |= LOCKD_MSG_TEST;
>>>>>>> origin/10.3

	TAILQ_FOREACH(mr, &nfs_pendlockq, lmr_next) {
		if (mr->lmr_msg.lm_xid == lockxid)
			return mr;
		if (mr->lmr_msg.lm_xid > lockxid)
			return NULL;
	}
	return mr;
}

<<<<<<< HEAD
/*
 * Because we can't depend on nlm_granted messages containing the same
 * cookie we sent with the original lock request, we need code to test
 * if an nlm_granted answer matches the lock request.  We also need code
 * that can find a lockd message based solely on the nlm_granted answer.
 */

/*
 * compare lockd message to answer
 *
 * returns 0 on equality and 1 if different
 */
int
nfs_lockdmsg_compare_to_answer(LOCKD_MSG_REQUEST *msgreq, struct lockd_ans *ansp)
{
	if (!(ansp->la_flags & LOCKD_ANS_LOCK_INFO))
		return 1;
	if (msgreq->lmr_msg.lm_fl.l_pid != ansp->la_pid)
		return 1;
	if (msgreq->lmr_msg.lm_fl.l_start != ansp->la_start)
		return 1;
	if (msgreq->lmr_msg.lm_fl.l_len != ansp->la_len)
		return 1;
	if (msgreq->lmr_msg.lm_fh_len != ansp->la_fh_len)
		return 1;
	if (bcmp(msgreq->lmr_msg.lm_fh, ansp->la_fh, ansp->la_fh_len))
		return 1;
	return 0;
}
=======
	bcopy(mtod(nmp->nm_nam, struct sockaddr *), &msg->lm_addr,
	      min(sizeof msg->lm_addr,
		  mtod(nmp->nm_nam, struct sockaddr *)->sa_len));
	msg->lm_fh_len = NFS_ISV3(vp) ? VTONFS(vp)->n_fhsize : NFSX_V2FH;
	bcopy(VTONFS(vp)->n_fhp, msg->lm_fh, msg->lm_fh_len);
	if (NFS_ISV3(vp))
		msg->lm_flags |= LOCKD_MSG_NFSV3;
	cru2x(p->p_ucred, &msg->lm_cred);

	microuptime(&now);
	lastmsg = now.tv_sec - ((nmp->nm_tprintf_delay) - (nmp->nm_tprintf_initial_delay));
>>>>>>> origin/10.3

/*
 * find a pending lock request message based on the lock info provided
 * in the lockd_ans/nlm_granted data.  We need this because we can't
 * depend on nlm_granted messages containing the same cookie we sent
 * with the original lock request.
 *
 * We search from the head of the list assuming that the message we're
 * looking for is for an older request (because we have an answer to it).
 * This assumes that lock request will be answered primarily in FIFO order.
 * However, this may not be the case if there are blocked requests.  We may
 * want to move blocked requests to a separate queue (but that'll complicate
 * duplicate xid checking).
 *
 * (nfs_lock_mutex must be held)
 */
LOCKD_MSG_REQUEST *
nfs_lockdmsg_find_by_answer(struct lockd_ans *ansp)
{
	LOCKD_MSG_REQUEST *mr;

	if (!(ansp->la_flags & LOCKD_ANS_LOCK_INFO))
		return NULL;
	TAILQ_FOREACH(mr, &nfs_pendlockq, lmr_next) {
		if (!nfs_lockdmsg_compare_to_answer(mr, ansp))
			break;
	}
	return mr;
}

<<<<<<< HEAD
/*
 * return the next unique lock request transaction ID
 * (nfs_lock_mutex must be held)
 */
uint64_t
nfs_lockxid_get(void)
{
	LOCKD_MSG_REQUEST *mr;

	/* derive initial lock xid from system time */
	if (!nfs_lockxid) {
		/*
		 * Note: it's OK if this code inits nfs_lockxid to 0 (for example,
		 * due to a broken clock) because we immediately increment it
		 * and we guarantee to never use xid 0.  So, nfs_lockxid should only
		 * ever be 0 the first time this function is called.
		 */
		struct timeval tv;
		microtime(&tv);
		nfs_lockxid = (uint64_t)tv.tv_sec << 12;
	}

	/* make sure we get a unique xid */
	do {
		/* Skip zero xid if it should ever happen.  */
		if (++nfs_lockxid == 0)
			nfs_lockxid++;
		if (!(mr = TAILQ_LAST(&nfs_pendlockq, nfs_lock_msg_queue)) ||
		     (mr->lmr_msg.lm_xid < nfs_lockxid)) {
			/* fast path: empty queue or new largest xid */
			break;
		}
		/* check if xid is already in use */
	} while (nfs_lockdmsg_find_by_xid(nfs_lockxid));

	return nfs_lockxid;
}
=======
	/* allocate unique xid */
	msg->lm_xid = nfs_lockxid_get();
	nfs_lockdmsg_enqueue(&msgreq);

	timeo = 2*hz;
#define IO_NOMACCHECK 0;
	ioflg = IO_UNIT | IO_NOMACCHECK;
	for (;;) {
		VOP_LEASE(wvp, p, kernproc->p_ucred, LEASE_WRITE);

		error = 0;
		while (nfslockdfifolock & NFSLOCKDFIFOLOCK_LOCKED) {
			nfslockdfifolock |= NFSLOCKDFIFOLOCK_WANT;
			error = tsleep((void *)&nfslockdfifolock,
					PCATCH | PUSER, "lockdfifo", 20*hz);
			if (error)
				break;
		}
		if (error)
			break;
		nfslockdfifolock |= NFSLOCKDFIFOLOCK_LOCKED;

		error = vn_rdwr(UIO_WRITE, wvp, (caddr_t)msg, sizeof(*msg), 0,
		    UIO_SYSSPACE, ioflg, kernproc->p_ucred, NULL, p);
>>>>>>> origin/10.3

#define MACH_MAX_TRIES 3

int
nfs_lockd_send_request(LOCKD_MSG *msg, int interruptable)
{
	kern_return_t kr;
	int retries = 0;
	mach_port_t lockd_port = IPC_PORT_NULL;

	kr = host_get_lockd_port(host_priv_self(), &lockd_port);
	if (kr != KERN_SUCCESS || !IPC_PORT_VALID(lockd_port))
		return (ENOTSUP);

	do {
		/* In the kernel all mach messaging is interruptable */
		do {
			kr = lockd_request(
				lockd_port,
				msg->lm_version,
				msg->lm_flags,
				msg->lm_xid,
				msg->lm_fl.l_start,
				msg->lm_fl.l_len,
				msg->lm_fl.l_pid,
				msg->lm_fl.l_type,
				msg->lm_fl.l_whence,
				(uint32_t *)&msg->lm_addr,
				(uint32_t *)&msg->lm_cred,
				msg->lm_fh_len,
				msg->lm_fh);
			if (kr != KERN_SUCCESS)
				printf("lockd_request received %d!\n", kr);
		} while (!interruptable && kr == MACH_SEND_INTERRUPTED);
	} while (kr == MIG_SERVER_DIED && retries++ < MACH_MAX_TRIES);

	ipc_port_release_send(lockd_port);
	switch (kr) {
	case MACH_SEND_INTERRUPTED: 
		return (EINTR);
	default:
		/*
		 * Other MACH or MIG errors we will retry. Eventually
		 * we will call nfs_down and allow the user to disable 
		 * locking.
		 */
		return (EAGAIN);
	}
	return (kr);
}
				

/*
 * NFS advisory byte-level locks (client)
 */
int
nfs3_lockd_request(
	nfsnode_t np,
	int type,
	LOCKD_MSG_REQUEST *msgreq,
	int flags,
	thread_t thd)
{
	LOCKD_MSG *msg = &msgreq->lmr_msg;
	int error, error2;
	int interruptable, slpflag;
	struct nfsmount *nmp;
	struct timeval now;
	int timeo, starttime, endtime, lastmsg, wentdown = 0;
	struct timespec ts;
	struct sockaddr *saddr;

	nmp = NFSTONMP(np);
	if (!nmp || !nmp->nm_saddr)
		return (ENXIO);

	lck_mtx_lock(&nmp->nm_lock);
	saddr = nmp->nm_saddr;
	bcopy(saddr, &msg->lm_addr, min(sizeof msg->lm_addr, saddr->sa_len));
	if (nmp->nm_vers == NFS_VER3)
		msg->lm_flags |= LOCKD_MSG_NFSV3;

	if (nmp->nm_sotype != SOCK_DGRAM)
		msg->lm_flags |= LOCKD_MSG_TCP;

	microuptime(&now);
	starttime = now.tv_sec;
	lastmsg = now.tv_sec - ((nmp->nm_tprintf_delay) - (nmp->nm_tprintf_initial_delay));
	interruptable = NMFLAG(nmp, INTR);
	lck_mtx_unlock(&nmp->nm_lock);

	lck_mtx_lock(nfs_lock_mutex);

	/* allocate unique xid */
	msg->lm_xid = nfs_lockxid_get();
	nfs_lockdmsg_enqueue(msgreq);

	timeo = 4;

	for (;;) {
		nfs_lockd_request_sent = 1;

		/* need to drop nfs_lock_mutex while calling nfs_lockd_send_request() */
		lck_mtx_unlock(nfs_lock_mutex);
		error = nfs_lockd_send_request(msg, interruptable);
		lck_mtx_lock(nfs_lock_mutex);
		if (error && error != EAGAIN)
			break;
<<<<<<< HEAD
=======
		}
>>>>>>> origin/10.3

		/*
		 * Always wait for an answer.  Not waiting for unlocks could
		 * cause a lock to be left if the unlock request gets dropped.
		 */

		/*
		 * Retry if it takes too long to get a response.
		 *
		 * The timeout numbers were picked out of thin air... they start
<<<<<<< HEAD
		 * at 4 and double each timeout with a max of 30 seconds.
		 *
		 * In order to maintain responsiveness, we pass a small timeout
		 * to msleep and calculate the timeouts ourselves.  This allows
=======
		 * at 2 and double each timeout with a max of 60 seconds.
		 *
		 * In order to maintain responsiveness, we pass a small timeout
		 * to tsleep and calculate the timeouts ourselves.  This allows
>>>>>>> origin/10.3
		 * us to pick up on mount changes quicker.
		 */
wait_for_granted:
		error = EWOULDBLOCK;
<<<<<<< HEAD
		slpflag = (interruptable && (type != F_UNLCK)) ? PCATCH : 0;
		ts.tv_sec = 2;
		ts.tv_nsec = 0;
		microuptime(&now);
		endtime = now.tv_sec + timeo;
		while (now.tv_sec < endtime) {
			error = error2 = 0;
			if (!msgreq->lmr_answered) {
				error = msleep(msgreq, nfs_lock_mutex, slpflag | PUSER, "lockd", &ts);
				slpflag = 0;
			}
			if (msgreq->lmr_answered) {
				/*
				 * Note: it's possible to have a lock granted at
				 * essentially the same time that we get interrupted.
				 * Since the lock may be granted, we can't return an
				 * error from this request or we might not unlock the
				 * lock that's been granted.
				 */
				nmp = NFSTONMP(np);
				if ((msgreq->lmr_errno == ENOTSUP) && nmp &&
				    (nmp->nm_state & NFSSTA_LOCKSWORK)) {
					/*
					 * We have evidence that locks work, yet lockd
					 * returned ENOTSUP.  This is probably because
					 * it was unable to contact the server's lockd
					 * to send it the request.
					 *
					 * Because we know locks work, we'll consider
					 * this failure to be a timeout.
					 */
					error = EWOULDBLOCK;
				} else {
					error = 0;
				}
				break;
			}
			if (error != EWOULDBLOCK)
				break;
			/* check that we still have our mount... */
			/* ...and that we still support locks */
			/* ...and that there isn't a recovery pending */
			nmp = NFSTONMP(np);
			if ((error2 = nfs_sigintr(nmp, NULL, NULL, 0))) {
				error = error2;
				if (type == F_UNLCK)
					printf("nfs3_lockd_request: aborting unlock request, error %d\n", error);
				break;
			}
			lck_mtx_lock(&nmp->nm_lock);
			if (nmp->nm_lockmode == NFS_LOCK_MODE_DISABLED) {
				lck_mtx_unlock(&nmp->nm_lock);
				break;
			}
			if ((nmp->nm_state & NFSSTA_RECOVER) && !(flags & R_RECOVER)) {
				/* recovery pending... return an error that'll get this operation restarted */
				error = NFSERR_GRACE;
				lck_mtx_unlock(&nmp->nm_lock);
				break;
			}
			interruptable = NMFLAG(nmp, INTR);
			lck_mtx_unlock(&nmp->nm_lock);
			microuptime(&now);
		}
		if (error) {
			/* check that we still have our mount... */
			nmp = NFSTONMP(np);
			if ((error2 = nfs_sigintr(nmp, NULL, NULL, 0))) {
				error = error2;
				if (error2 != EINTR) {
					if (type == F_UNLCK)
						printf("nfs3_lockd_request: aborting unlock request, error %d\n", error);
					break;
				}
			}
			/* ...and that we still support locks */
			lck_mtx_lock(&nmp->nm_lock);
			if (nmp->nm_lockmode == NFS_LOCK_MODE_DISABLED) {
				if (error == EWOULDBLOCK)
					error = ENOTSUP;
				lck_mtx_unlock(&nmp->nm_lock);
				break;
			}
			/* ...and that there isn't a recovery pending */
			if ((error == EWOULDBLOCK) && (nmp->nm_state & NFSSTA_RECOVER) && !(flags & R_RECOVER)) {
				/* recovery pending... return to allow recovery to occur */
				error = NFSERR_DENIED;
				lck_mtx_unlock(&nmp->nm_lock);
				break;
			}
			interruptable = NMFLAG(nmp, INTR);
			if ((error != EWOULDBLOCK) ||
			    ((nmp->nm_state & NFSSTA_RECOVER) && !(flags & R_RECOVER)) ||
			    ((flags & R_RECOVER) && ((now.tv_sec - starttime) > 30))) {
				if ((error == EWOULDBLOCK) && (flags & R_RECOVER)) {
					/* give up if this is for recovery and taking too long */
					error = ETIMEDOUT;
				} else if ((nmp->nm_state & NFSSTA_RECOVER) && !(flags & R_RECOVER)) {
					/* recovery pending... return an error that'll get this operation restarted */
					error = NFSERR_GRACE;
				}
				lck_mtx_unlock(&nmp->nm_lock);
=======
		microuptime(&now);
		if ((timeo/hz) > 0)
			endtime = now.tv_sec + timeo/hz;
		else
			endtime = now.tv_sec + 1;
		while (now.tv_sec < endtime) {
			error = tsleep((void *)&msgreq, PCATCH | PUSER, "lockd", 2*hz);
			if (msgreq.lmr_answered) {
				/*
				 * Note: it's possible to have a lock granted at
				 * essentially the same time that we get interrupted.
				 * Since the lock may be granted, we can't return an
				 * error from this request or we might not unlock the
				 * lock that's been granted.
				 */
				error = 0;
				break;
			}
			if (error != EWOULDBLOCK)
				break;
			/* check that we still have our mount... */
			/* ...and that we still support locks */
			nmp = VFSTONFS(vp->v_mount);
			if (!nmp || (nmp->nm_flag & NFSMNT_NOLOCKS))
				break;
			/*
			 * If the mount is hung and we've requested not to hang
			 * on remote filesystems, then bail now.
			 */
			if ((p != NULL) && ((p->p_flag & P_NOREMOTEHANG) != 0) &&
			    ((nmp->nm_state & (NFSSTA_TIMEO|NFSSTA_LOCKTIMEO)) != 0)) {
				if (fl->l_type == F_UNLCK)
					printf("nfs_dolock: aborting unlock request "
					    "due to timeout (noremotehang)\n");
				error = EIO;
				break;
			}
			microuptime(&now);
		}
		if (error) {
			/* check that we still have our mount... */
			nmp = VFSTONFS(vp->v_mount);
			if (!nmp) {
				if (error == EWOULDBLOCK)
					error = ENXIO;
				break;
			}
			/* ...and that we still support locks */
			if (nmp->nm_flag & NFSMNT_NOLOCKS) {
				if (error == EWOULDBLOCK)
					error = EOPNOTSUPP;
				break;
			}
			if ((error == EOPNOTSUPP) &&
			    (nmp->nm_state & NFSSTA_LOCKSWORK)) {
				/*
				 * We have evidence that locks work, yet lockd
				 * returned EOPNOTSUPP.  This is probably because
				 * it was unable to contact the server's lockd to
				 * send it the request.
				 *
				 * Because we know locks work, we'll consider
				 * this failure to be a timeout.
				 */
				error = EWOULDBLOCK;
			}
			if (error != EWOULDBLOCK) {
>>>>>>> origin/10.3
				/*
				 * We're going to bail on this request.
				 * If we were a blocked lock request, send a cancel.
				 */
<<<<<<< HEAD
				if ((msgreq->lmr_errno == EINPROGRESS) &&
				    !(msg->lm_flags & LOCKD_MSG_CANCEL)) {
					/* set this request up as a cancel */
					msg->lm_flags |= LOCKD_MSG_CANCEL;
					nfs_lockdmsg_dequeue(msgreq);
					msg->lm_xid = nfs_lockxid_get();
					nfs_lockdmsg_enqueue(msgreq);
					msgreq->lmr_saved_errno = error;
					msgreq->lmr_errno = 0;
					msgreq->lmr_answered = 0;
					/* reset timeout */
					timeo = 2;
=======
				if ((msgreq.lmr_errno == EINPROGRESS) &&
				    !(msg->lm_flags & LOCKD_MSG_CANCEL)) {
					/* set this request up as a cancel */
					msg->lm_flags |= LOCKD_MSG_CANCEL;
					nfs_lockdmsg_dequeue(&msgreq);
					msg->lm_xid = nfs_lockxid_get();
					nfs_lockdmsg_enqueue(&msgreq);
					msgreq.lmr_saved_errno = error;
					msgreq.lmr_errno = 0;
					msgreq.lmr_answered = 0;
					/* reset timeout */
					timeo = 2*hz;
>>>>>>> origin/10.3
					/* send cancel request */
					continue;
				}
				break;
			}

<<<<<<< HEAD
			/* warn if we're not getting any response */
			microuptime(&now);
			if ((msgreq->lmr_errno != EINPROGRESS) &&
			    !(msg->lm_flags & LOCKD_MSG_DENIED_GRACE) &&
			    (nmp->nm_tprintf_initial_delay != 0) &&
			    ((lastmsg + nmp->nm_tprintf_delay) < now.tv_sec)) {
				lck_mtx_unlock(&nmp->nm_lock);
				lastmsg = now.tv_sec;
				nfs_down(nmp, thd, 0, NFSSTA_LOCKTIMEO, "lockd not responding", 0);
				wentdown = 1;
			} else
				lck_mtx_unlock(&nmp->nm_lock);

			if (msgreq->lmr_errno == EINPROGRESS) {
=======
			/*
			 * If the mount is hung and we've requested not to hang
			 * on remote filesystems, then bail now.
			 */
			if ((p != NULL) && ((p->p_flag & P_NOREMOTEHANG) != 0) &&
			    ((nmp->nm_state & (NFSSTA_TIMEO|NFSSTA_LOCKTIMEO)) != 0)) {
				if (fl->l_type == F_UNLCK)
					printf("nfs_dolock: aborting unlock request "
					    "due to timeout (noremotehang)\n");
				error = EIO;
				break;
			}
			/* warn if we're not getting any response */
			microuptime(&now);
			if ((msgreq.lmr_errno != EINPROGRESS) &&
			    (nmp->nm_tprintf_initial_delay != 0) &&
			    ((lastmsg + nmp->nm_tprintf_delay) < now.tv_sec)) {
				lastmsg = now.tv_sec;
				nfs_down(NULL, nmp, p, "lockd not responding",
					0, NFSSTA_LOCKTIMEO);
				wentdown = 1;
			}
			if (msgreq.lmr_errno == EINPROGRESS) {
>>>>>>> origin/10.3
				/*
				 * We've got a blocked lock request that we are
				 * going to retry.  First, we'll want to try to
				 * send a cancel for the previous request.
				 *
				 * Clear errno so if we don't get a response
				 * to the resend we'll call nfs_down().
				 * Also reset timeout because we'll expect a
				 * quick response to the cancel/resend (even if
				 * it is NLM_BLOCKED).
				 */
				msg->lm_flags |= LOCKD_MSG_CANCEL;
<<<<<<< HEAD
				nfs_lockdmsg_dequeue(msgreq);
				msg->lm_xid = nfs_lockxid_get();
				nfs_lockdmsg_enqueue(msgreq);
				msgreq->lmr_saved_errno = msgreq->lmr_errno;
				msgreq->lmr_errno = 0;
				msgreq->lmr_answered = 0;
				timeo = 2;
=======
				nfs_lockdmsg_dequeue(&msgreq);
				msg->lm_xid = nfs_lockxid_get();
				nfs_lockdmsg_enqueue(&msgreq);
				msgreq.lmr_saved_errno = msgreq.lmr_errno;
				msgreq.lmr_errno = 0;
				msgreq.lmr_answered = 0;
				timeo = 2*hz;
>>>>>>> origin/10.3
				/* send cancel then resend request */
				continue;
			}
			/*
			 * We timed out, so we will rewrite the request
			 * to the fifo, but only if it isn't already full.
			 */
			ioflg |= IO_NDELAY;
			timeo *= 2;
			if (timeo > 60*hz)
				timeo = 60*hz;
			/* resend request */
			continue;
		}

<<<<<<< HEAD
			/*
			 * We timed out, so we will resend the request.
			 */
			if (!(flags & R_RECOVER))
				timeo *= 2;
			if (timeo > 30)
				timeo = 30;
			/* resend request */
			continue;
		}

		/* we got a reponse, so the server's lockd is OK */
		nfs_up(NFSTONMP(np), thd, NFSSTA_LOCKTIMEO,
			wentdown ? "lockd alive again" : NULL);
		wentdown = 0;

		if (msgreq->lmr_answered && (msg->lm_flags & LOCKD_MSG_DENIED_GRACE)) {
			/*
			 * The lock request was denied because the server lockd is
			 * still in its grace period.  So, we need to try the
			 * request again in a little bit.  Return the GRACE error so
			 * the higher levels can perform the retry.
			 */
			msgreq->lmr_saved_errno = msgreq->lmr_errno = error = NFSERR_GRACE;
		}

		if (msgreq->lmr_errno == EINPROGRESS) {
			/* got NLM_BLOCKED response */
			/* need to wait for NLM_GRANTED */
			timeo = 30;
			msgreq->lmr_answered = 0;
			goto wait_for_granted;
		}

		if ((msg->lm_flags & LOCKD_MSG_CANCEL) &&
		    (msgreq->lmr_saved_errno == EINPROGRESS)) {
			/*
			 * We just got a successful reply to the
			 * cancel of the previous blocked lock request.
			 * Now, go ahead and return a DENIED error so the
			 * higher levels can resend the request.
			 */
			msg->lm_flags &= ~LOCKD_MSG_CANCEL;
			nfs_lockdmsg_dequeue(msgreq);
			error = NFSERR_DENIED;
			break;
		}

		/*
		 * If the blocked lock request was cancelled.
		 * Restore the error condition from when we
		 * originally bailed on the request.
		 */
		if (msg->lm_flags & LOCKD_MSG_CANCEL) {
			msg->lm_flags &= ~LOCKD_MSG_CANCEL;
			error = msgreq->lmr_saved_errno;
		} else {
			error = msgreq->lmr_errno;
		}

		nmp = NFSTONMP(np);
		if ((error == ENOTSUP) && nmp && !(nmp->nm_state & NFSSTA_LOCKSWORK)) {
			/*
			 * We have NO evidence that locks work and lockd
			 * returned ENOTSUP.  Let's take this as a hint
			 * that locks aren't supported and disable them
			 * for this mount.
			 */
			nfs_lockdmsg_dequeue(msgreq);
			lck_mtx_unlock(nfs_lock_mutex);
			lck_mtx_lock(&nmp->nm_lock);
			if (nmp->nm_lockmode == NFS_LOCK_MODE_ENABLED) {
				nmp->nm_lockmode = NFS_LOCK_MODE_DISABLED;
				nfs_lockd_mount_unregister(nmp);
			}
			nmp->nm_state &= ~NFSSTA_LOCKTIMEO;
			lck_mtx_unlock(&nmp->nm_lock);
			printf("lockd returned ENOTSUP, disabling locks for nfs server: %s\n",
				vfs_statfs(nmp->nm_mountp)->f_mntfromname);
			return (error);
		}
		if (!error) {
			/* record that NFS file locking has worked on this mount */
			if (nmp) {
				lck_mtx_lock(&nmp->nm_lock);
				if (!(nmp->nm_state & NFSSTA_LOCKSWORK))
					nmp->nm_state |= NFSSTA_LOCKSWORK;
				lck_mtx_unlock(&nmp->nm_lock);
			}
		}
		break;
	}

	nfs_lockdmsg_dequeue(msgreq);

	lck_mtx_unlock(nfs_lock_mutex);

	return (error);
=======
		if (wentdown) {
			/* we got a reponse, so the server's lockd is OK */
			nfs_up(NULL, VFSTONFS(vp->v_mount), p, "lockd alive again",
				NFSSTA_LOCKTIMEO);
			wentdown = 0;
		}

		if (msgreq.lmr_errno == EINPROGRESS) {
			/* got NLM_BLOCKED response */
			/* need to wait for NLM_GRANTED */
			timeo = 60*hz;
			msgreq.lmr_answered = 0;
			goto wait_for_granted;
		}

		if ((msg->lm_flags & LOCKD_MSG_CANCEL) &&
		    (msgreq.lmr_saved_errno == EINPROGRESS)) {
			/*
			 * We just got a successful reply to the
			 * cancel of the previous blocked lock request.
			 * Now, go ahead and resend the request.
			 */
			msg->lm_flags &= ~LOCKD_MSG_CANCEL;
			nfs_lockdmsg_dequeue(&msgreq);
			msg->lm_xid = nfs_lockxid_get();
			nfs_lockdmsg_enqueue(&msgreq);
			msgreq.lmr_saved_errno = 0;
			msgreq.lmr_errno = 0;
			msgreq.lmr_answered = 0;
			timeo = 2*hz;
			/* resend request */
			continue;
		}

		if ((msg->lm_flags & LOCKD_MSG_TEST) && msgreq.lmr_errno == 0) {
			if (msg->lm_fl.l_type != F_UNLCK) {
				fl->l_type = msg->lm_fl.l_type;
				fl->l_pid = msg->lm_fl.l_pid;
				fl->l_start = msg->lm_fl.l_start;
				fl->l_len = msg->lm_fl.l_len;
				fl->l_whence = SEEK_SET;
			} else {
				fl->l_type = F_UNLCK;
			}
		}

		/*
		 * If the blocked lock request was cancelled.
		 * Restore the error condition from when we
		 * originally bailed on the request.
		 */
		if (msg->lm_flags & LOCKD_MSG_CANCEL) {
			msg->lm_flags &= ~LOCKD_MSG_CANCEL;
			error = msgreq.lmr_saved_errno;
		} else
			error = msgreq.lmr_errno;

		if (!error) {
			/* record that NFS file locking has worked on this mount */
			nmp = VFSTONFS(vp->v_mount);
			if (nmp && !(nmp->nm_state & NFSSTA_LOCKSWORK))
				nmp->nm_state |= NFSSTA_LOCKSWORK;
			/*
			 * If we successfully acquired a lock, make sure this pid
			 * is in the nfs_lock_pid hash table so we know we can't
			 * short-circuit unlock requests.
			 */
			if ((lockpidcheck == ENOENT) &&
			    ((ap->a_op == F_SETLK) || (ap->a_op == F_SETLKW)))
				nfs_lock_pid_check(p, 1, vp);
	
		}
		break;
	}
  
	nfs_lockdmsg_dequeue(&msgreq);

	error1 = vn_close(wvp, FWRITE, kernproc->p_ucred, p);
	/* prefer any previous 'error' to our vn_close 'error1'. */
	return (error != 0 ? error : error1);
>>>>>>> origin/10.3
}

/*
 * Send an NLM LOCK message to the server
 */
int
nfs3_setlock_rpc(
	nfsnode_t np,
	struct nfs_open_file *nofp,
	struct nfs_file_lock *nflp,
	int reclaim,
	int flags,
	thread_t thd,
	kauth_cred_t cred)
{
<<<<<<< HEAD
	struct nfs_lock_owner *nlop = nflp->nfl_owner;
	struct nfsmount *nmp;
=======
	LOCKD_MSG_REQUEST *msgreq;
>>>>>>> origin/10.3
	int error;
	LOCKD_MSG_REQUEST msgreq;
	LOCKD_MSG *msg;

	nmp = NFSTONMP(np);
	if (nfs_mount_gone(nmp))
		return (ENXIO);

	if (!nlop->nlo_open_owner) {
		nfs_open_owner_ref(nofp->nof_owner);
		nlop->nlo_open_owner = nofp->nof_owner;
	}
	if ((error = nfs_lock_owner_set_busy(nlop, thd)))
		return (error);

<<<<<<< HEAD
	/* set up lock message request structure */
	bzero(&msgreq, sizeof(msgreq));
	msg = &msgreq.lmr_msg;
	msg->lm_version = LOCKD_MSG_VERSION;
	if ((nflp->nfl_flags & NFS_FILE_LOCK_WAIT) && !reclaim)
		msg->lm_flags |= LOCKD_MSG_BLOCK;
	if (reclaim)
		msg->lm_flags |= LOCKD_MSG_RECLAIM;
	msg->lm_fh_len = (nmp->nm_vers == NFS_VER2) ? NFSX_V2FH : np->n_fhsize;
	bcopy(np->n_fhp, msg->lm_fh, msg->lm_fh_len);
	cru2x(cred, &msg->lm_cred);

	msg->lm_fl.l_whence = SEEK_SET;
	msg->lm_fl.l_start = nflp->nfl_start;
	msg->lm_fl.l_len = NFS_FLOCK_LENGTH(nflp->nfl_start, nflp->nfl_end);
	msg->lm_fl.l_type = nflp->nfl_type;
	msg->lm_fl.l_pid = nlop->nlo_pid;

	error = nfs3_lockd_request(np, 0, &msgreq, flags, thd);

	nfs_lock_owner_clear_busy(nlop);
	return (error);
}

/*
 * Send an NLM UNLOCK message to the server
 */
int
nfs3_unlock_rpc(
	nfsnode_t np,
	struct nfs_lock_owner *nlop,
	__unused int type,
	uint64_t start,
	uint64_t end,
	int flags,
	thread_t thd,
	kauth_cred_t cred)
{
	struct nfsmount *nmp;
	LOCKD_MSG_REQUEST msgreq;
	LOCKD_MSG *msg;

	nmp = NFSTONMP(np);
	if (!nmp)
		return (ENXIO);

	/* set up lock message request structure */
	bzero(&msgreq, sizeof(msgreq));
	msg = &msgreq.lmr_msg;
	msg->lm_version = LOCKD_MSG_VERSION;
	msg->lm_fh_len = (nmp->nm_vers == NFS_VER2) ? NFSX_V2FH : np->n_fhsize;
	bcopy(np->n_fhp, msg->lm_fh, msg->lm_fh_len);
	cru2x(cred, &msg->lm_cred);

	msg->lm_fl.l_whence = SEEK_SET;
	msg->lm_fl.l_start = start;
	msg->lm_fl.l_len = NFS_FLOCK_LENGTH(start, end);
	msg->lm_fl.l_type = F_UNLCK;
	msg->lm_fl.l_pid = nlop->nlo_pid;

	return (nfs3_lockd_request(np, F_UNLCK, &msgreq, flags, thd));
}

/*
 * Send an NLM LOCK TEST message to the server
 */
int
nfs3_getlock_rpc(
	nfsnode_t np,
	struct nfs_lock_owner *nlop,
	struct flock *fl,
	uint64_t start,
	uint64_t end,
	vfs_context_t ctx)
{
	struct nfsmount *nmp;
	int error;
	LOCKD_MSG_REQUEST msgreq;
	LOCKD_MSG *msg;
=======
	/* the version should match, or we're out of sync */
	if (ansp->la_version != LOCKD_ANS_VERSION)
		return (EINVAL);

	/* try to find the lockd message by transaction id (cookie) */
	msgreq = nfs_lockdmsg_find_by_xid(ansp->la_xid);
	if (ansp->la_flags & LOCKD_ANS_GRANTED) {
		/*
		 * We can't depend on the granted message having our cookie,
		 * so we check the answer against the lockd message found.
		 * If no message was found or it doesn't match the answer,
		 * we look for the lockd message by the answer's lock info.
		 */
		if (!msgreq || nfs_lockdmsg_compare_to_answer(msgreq, ansp))
			msgreq = nfs_lockdmsg_find_by_answer(ansp);
		/*
		 * We need to make sure this request isn't being cancelled
		 * If it is, we don't want to accept the granted message.
		 */
		if (msgreq && (msgreq->lmr_msg.lm_flags & LOCKD_MSG_CANCEL))
			msgreq = NULL;
	}
	if (!msgreq)
		return (EPIPE);

	msgreq->lmr_errno = ansp->la_errno;
	if ((msgreq->lmr_msg.lm_flags & LOCKD_MSG_TEST) && msgreq->lmr_errno == 0) {
		if (ansp->la_flags & LOCKD_ANS_LOCK_INFO) {
			if (ansp->la_flags & LOCKD_ANS_LOCK_EXCL)
				msgreq->lmr_msg.lm_fl.l_type = F_WRLCK;
			else
				msgreq->lmr_msg.lm_fl.l_type = F_RDLCK;
			msgreq->lmr_msg.lm_fl.l_pid = ansp->la_pid;
			msgreq->lmr_msg.lm_fl.l_start = ansp->la_start;
			msgreq->lmr_msg.lm_fl.l_len = ansp->la_len;
		} else {
			msgreq->lmr_msg.lm_fl.l_type = F_UNLCK;
		}
	}

	msgreq->lmr_answered = 1;
	(void)wakeup((void *)msgreq);
>>>>>>> origin/10.3

	nmp = NFSTONMP(np);
	if (nfs_mount_gone(nmp))
		return (ENXIO);

	/* set up lock message request structure */
	bzero(&msgreq, sizeof(msgreq));
	msg = &msgreq.lmr_msg;
	msg->lm_version = LOCKD_MSG_VERSION;
	msg->lm_flags |= LOCKD_MSG_TEST;
	msg->lm_fh_len = (nmp->nm_vers == NFS_VER2) ? NFSX_V2FH : np->n_fhsize;
	bcopy(np->n_fhp, msg->lm_fh, msg->lm_fh_len);
	cru2x(vfs_context_ucred(ctx), &msg->lm_cred);

	msg->lm_fl.l_whence = SEEK_SET;
	msg->lm_fl.l_start = start;
	msg->lm_fl.l_len = NFS_FLOCK_LENGTH(start, end);
	msg->lm_fl.l_type = fl->l_type;
	msg->lm_fl.l_pid = nlop->nlo_pid;

	error = nfs3_lockd_request(np, 0, &msgreq, 0, vfs_context_thread(ctx));

	if (!error && (msg->lm_flags & LOCKD_MSG_TEST) && !msgreq.lmr_errno) {
		if (msg->lm_fl.l_type != F_UNLCK) {
			fl->l_type = msg->lm_fl.l_type;
			fl->l_pid = msg->lm_fl.l_pid;
			fl->l_start = msg->lm_fl.l_start;
			fl->l_len = msg->lm_fl.l_len;
			fl->l_whence = SEEK_SET;
		} else
			fl->l_type = F_UNLCK;
	}

	return (error);
}

/*
 * nfslockdans --
 *      NFS advisory byte-level locks answer from the lock daemon.
 */
int
nfslockdans(proc_t p, struct lockd_ans *ansp)
{
	LOCKD_MSG_REQUEST *msgreq;
	int error;

	/* Let root make this call. */
	error = proc_suser(p);
	if (error)
		return (error);

	/* the version should match, or we're out of sync */
	if (ansp->la_version != LOCKD_ANS_VERSION)
		return (EINVAL);

	lck_mtx_lock(nfs_lock_mutex);

	/* try to find the lockd message by transaction id (cookie) */
	msgreq = nfs_lockdmsg_find_by_xid(ansp->la_xid);
	if (ansp->la_flags & LOCKD_ANS_GRANTED) {
		/*
		 * We can't depend on the granted message having our cookie,
		 * so we check the answer against the lockd message found.
		 * If no message was found or it doesn't match the answer,
		 * we look for the lockd message by the answer's lock info.
		 */
		if (!msgreq || nfs_lockdmsg_compare_to_answer(msgreq, ansp))
			msgreq = nfs_lockdmsg_find_by_answer(ansp);
		/*
		 * We need to make sure this request isn't being cancelled
		 * If it is, we don't want to accept the granted message.
		 */
		if (msgreq && (msgreq->lmr_msg.lm_flags & LOCKD_MSG_CANCEL))
			msgreq = NULL;
	}
	if (!msgreq) {
		lck_mtx_unlock(nfs_lock_mutex);
		return (EPIPE);
	}

	msgreq->lmr_errno = ansp->la_errno;
	if ((msgreq->lmr_msg.lm_flags & LOCKD_MSG_TEST) && msgreq->lmr_errno == 0) {
		if (ansp->la_flags & LOCKD_ANS_LOCK_INFO) {
			if (ansp->la_flags & LOCKD_ANS_LOCK_EXCL)
				msgreq->lmr_msg.lm_fl.l_type = F_WRLCK;
			else
				msgreq->lmr_msg.lm_fl.l_type = F_RDLCK;
			msgreq->lmr_msg.lm_fl.l_pid = ansp->la_pid;
			msgreq->lmr_msg.lm_fl.l_start = ansp->la_start;
			msgreq->lmr_msg.lm_fl.l_len = ansp->la_len;
		} else {
			msgreq->lmr_msg.lm_fl.l_type = F_UNLCK;
		}
	}
	if (ansp->la_flags & LOCKD_ANS_DENIED_GRACE)
		msgreq->lmr_msg.lm_flags |= LOCKD_MSG_DENIED_GRACE;

	msgreq->lmr_answered = 1;
	lck_mtx_unlock(nfs_lock_mutex);
	wakeup(msgreq);

	return (0);
}

/*
 * nfslockdnotify --
 *      NFS host restart notification from the lock daemon.
 *
 * Used to initiate reclaiming of held locks when a server we
 * have mounted reboots.
 */
int
nfslockdnotify(proc_t p, user_addr_t argp)
{
	int error, i, headsize;
	struct lockd_notify ln;
	struct nfsmount *nmp;
	struct sockaddr *saddr;

	/* Let root make this call. */
	error = proc_suser(p);
	if (error)
		return (error);

	headsize = (char*)&ln.ln_addr[0] - (char*)&ln.ln_version;
	error = copyin(argp, &ln, headsize);
	if (error)
		return (error);
	if (ln.ln_version != LOCKD_NOTIFY_VERSION)
		return (EINVAL);
	if ((ln.ln_addrcount < 1) || (ln.ln_addrcount > 128))
		return (EINVAL);
	argp += headsize;
	saddr = (struct sockaddr *)&ln.ln_addr[0];

	lck_mtx_lock(nfs_lock_mutex);

	for (i=0; i < ln.ln_addrcount; i++) {
		error = copyin(argp, &ln.ln_addr[0], sizeof(ln.ln_addr[0]));
		if (error)
			break;
		argp += sizeof(ln.ln_addr[0]);
		/* scan lockd mount list for match to this address */
		TAILQ_FOREACH(nmp, &nfs_lockd_mount_list, nm_ldlink) {
			/* check if address matches this mount's server address */
			if (!nmp->nm_saddr || nfs_sockaddr_cmp(saddr, nmp->nm_saddr))
				continue;
			/* We have a match!  Mark it as needing recovery. */
			lck_mtx_lock(&nmp->nm_lock);
			nfs_need_recover(nmp, 0);
			lck_mtx_unlock(&nmp->nm_lock);
		}
	}

	lck_mtx_unlock(nfs_lock_mutex);

	return (error);
}

