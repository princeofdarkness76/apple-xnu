/*
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
 * Copyright (c) 1999-2015 Apple Inc. All rights reserved.
=======
 * Copyright (c) 1999-2008 Apple Inc. All rights reserved.
>>>>>>> origin/10.5
=======
 * Copyright (c) 1999-2010 Apple Inc. All rights reserved.
>>>>>>> origin/10.6
=======
 * Copyright (c) 1999-2012 Apple Inc. All rights reserved.
>>>>>>> origin/10.7
=======
 * Copyright (c) 1999-2013 Apple Inc. All rights reserved.
>>>>>>> origin/10.8
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
 * Copyright (c) 1991, 1993, 1994
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
 *      hfs_vfsops.c
 *  derived from	@(#)ufs_vfsops.c	8.8 (Berkeley) 5/20/95
 *
 *      (c) Copyright 1997-2002 Apple Computer, Inc. All rights reserved.
 *
 *      hfs_vfsops.c -- VFS layer for loadable HFS file system.
 *
 */
#include <sys/param.h>
#include <sys/systm.h>
#include <sys/kauth.h>

#include <sys/ubc.h>
#include <sys/ubc_internal.h>
#include <sys/vnode_internal.h>
#include <sys/mount_internal.h>
#include <sys/sysctl.h>
#include <sys/malloc.h>
#include <sys/stat.h>
<<<<<<< HEAD
#include <sys/quota.h>
#include <sys/disk.h>
#include <sys/paths.h>
#include <sys/utfconv.h>
#include <sys/kdebug.h>
#include <sys/fslog.h>
#include <sys/ubc.h>
#include <sys/buf_internal.h>

/* for parsing boot-args */
#include <pexpert/pexpert.h>

<<<<<<< HEAD

#include <kern/locks.h>

#include <vfs/vfs_journal.h>

=======
#include <sys/lock.h>
>>>>>>> origin/10.1
=======
// XXXdbg
#include <vfs/vfs_journal.h>

>>>>>>> origin/10.2
#include <miscfs/specfs/specdev.h>
#include <hfs/hfs_mount.h>

#include <libkern/crypto/md5.h>
#include <uuid/uuid.h>

#include "hfs.h"
#include "hfs_catalog.h"
#include "hfs_cnode.h"
#include "hfs_dbg.h"
#include "hfs_endian.h"
#include "hfs_hotfiles.h"
#include "hfs_quota.h"
#include "hfs_btreeio.h"
<<<<<<< HEAD
#include "hfs_kdebug.h"
#include "hfs_cprotect.h"
=======
>>>>>>> origin/10.7

#include "hfscommon/headers/FileMgrInternal.h"
#include "hfscommon/headers/BTreesInternal.h"

#define HFS_MOUNT_DEBUG 1

#if	HFS_DIAGNOSTIC
int hfs_dbg_all = 0;
int hfs_dbg_err = 0;
#endif

<<<<<<< HEAD
<<<<<<< HEAD
/* Enable/disable debugging code for live volume resizing, defined in hfs_resize.c */
extern int hfs_resize_debug;
=======

/*
 * These come from IOKit/storage/IOMediaBSDClient.h
 */
#define DKIOCGETBLOCKSIZE            _IOR('d', 24, u_int32_t)
#define DKIOCSETBLOCKSIZE            _IOW('d', 24, u_int32_t)
#define DKIOCGETBLOCKCOUNT           _IOR('d', 25, u_int64_t)

/*
 * HFS File System globals:
 */
Ptr					gBufferAddress[BUFFERPTRLISTSIZE];
struct buf			*gBufferHeaderPtr[BUFFERPTRLISTSIZE];
int					gBufferListIndex;
simple_lock_data_t	gBufferPtrListLock;

//static char hfs_fs_name[MFSNAMELEN] = "hfs";
>>>>>>> origin/10.1
=======
/* Enable/disable debugging code for live volume resizing */
int hfs_resize_debug = 0;
>>>>>>> origin/10.6

lck_grp_attr_t *  hfs_group_attr;
lck_attr_t *  hfs_lock_attr;
lck_grp_t *  hfs_mutex_group;
lck_grp_t *  hfs_rwlock_group;
lck_grp_t *  hfs_spinlock_group;

extern struct vnodeopv_desc hfs_vnodeop_opv_desc;
/* not static so we can re-use in hfs_readwrite.c for build_path */
int hfs_vfs_vget(struct mount *mp, ino64_t ino, struct vnode **vpp, vfs_context_t context);


#if CONFIG_HFS_STD
extern struct vnodeopv_desc hfs_std_vnodeop_opv_desc;
static int hfs_flushMDB(struct hfsmount *hfsmp, int waitfor, int altflush);
#endif

/* not static so we can re-use in hfs_readwrite.c for build_path calls */
int hfs_vfs_vget(struct mount *mp, ino64_t ino, struct vnode **vpp, vfs_context_t context);

static int hfs_changefs(struct mount *mp, struct hfs_mount_args *args);
static int hfs_fhtovp(struct mount *mp, int fhlen, unsigned char *fhp, struct vnode **vpp, vfs_context_t context);
static int hfs_flushfiles(struct mount *, int, struct proc *);
static int hfs_getmountpoint(struct vnode *vp, struct hfsmount **hfsmpp);
static int hfs_init(struct vfsconf *vfsp);
static void hfs_locks_destroy(struct hfsmount *hfsmp);
static int hfs_vfs_root(struct mount *mp, struct vnode **vpp, vfs_context_t context);
static int hfs_quotactl(struct mount *, int, uid_t, caddr_t, vfs_context_t context);
static int hfs_start(struct mount *mp, int flags, vfs_context_t context);
<<<<<<< HEAD
=======
static int hfs_statfs(struct mount *mp, register struct vfsstatfs *sbp, vfs_context_t context);
static int hfs_sync(struct mount *mp, int waitfor, vfs_context_t context);
static int hfs_sysctl(int *name, u_int namelen, user_addr_t oldp, size_t *oldlenp, 
                      user_addr_t newp, size_t newlen, vfs_context_t context);
static int hfs_unmount(struct mount *mp, int mntflags, vfs_context_t context);
>>>>>>> origin/10.5
static int hfs_vptofh(struct vnode *vp, int *fhlenp, unsigned char *fhp, vfs_context_t context);
<<<<<<< HEAD
=======

static int hfs_reclaimspace(struct hfsmount *hfsmp, u_int32_t startblk, u_int32_t reclaimblks, vfs_context_t context);
static int hfs_overlapped_overflow_extents(struct hfsmount *hfsmp, u_int32_t startblk, u_int32_t fileID);
>>>>>>> origin/10.6
static int hfs_journal_replay(vnode_t devvp, vfs_context_t context);
static void hfs_syncer_free(struct hfsmount *hfsmp);

void hfs_initialize_allocator (struct hfsmount *hfsmp);
int hfs_teardown_allocator (struct hfsmount *hfsmp);

int hfs_mount(struct mount *mp, vnode_t  devvp, user_addr_t data, vfs_context_t context);
int hfs_mountfs(struct vnode *devvp, struct mount *mp, struct hfs_mount_args *args, int journal_replay_only, vfs_context_t context);
int hfs_reload(struct mount *mp);
int hfs_statfs(struct mount *mp, register struct vfsstatfs *sbp, vfs_context_t context);
int hfs_sync(struct mount *mp, int waitfor, vfs_context_t context);
int hfs_sysctl(int *name, u_int namelen, user_addr_t oldp, size_t *oldlenp, 
                      user_addr_t newp, size_t newlen, vfs_context_t context);
int hfs_unmount(struct mount *mp, int mntflags, vfs_context_t context);

/*
 * Called by vfs_mountroot when mounting HFS Plus as root.
 */

int
hfs_mountroot(mount_t mp, vnode_t rvp, vfs_context_t context)
{
	struct hfsmount *hfsmp;
	ExtendedVCB *vcb;
	struct vfsstatfs *vfsp;
	int error;

	if ((error = hfs_mountfs(rvp, mp, NULL, 0, context))) {
		if (HFS_MOUNT_DEBUG) {
			printf("hfs_mountroot: hfs_mountfs returned %d, rvp (%p) name (%s) \n", 
					error, rvp, (rvp->v_name ? rvp->v_name : "unknown device"));
		}
		return (error);
	}

	/* Init hfsmp */
	hfsmp = VFSTOHFS(mp);

	hfsmp->hfs_uid = UNKNOWNUID;
	hfsmp->hfs_gid = UNKNOWNGID;
	hfsmp->hfs_dir_mask = (S_IRWXU | S_IRGRP|S_IXGRP | S_IROTH|S_IXOTH); /* 0755 */
	hfsmp->hfs_file_mask = (S_IRWXU | S_IRGRP|S_IXGRP | S_IROTH|S_IXOTH); /* 0755 */

	/* Establish the free block reserve. */
	vcb = HFSTOVCB(hfsmp);
	vcb->reserveBlocks = ((u_int64_t)vcb->totalBlocks * HFS_MINFREE) / 100;
	vcb->reserveBlocks = MIN(vcb->reserveBlocks, HFS_MAXRESERVE / vcb->blockSize);

	vfsp = vfs_statfs(mp);
	(void)hfs_statfs(mp, vfsp, NULL);

	return (0);
}


/*
 * VFS Operations.
 *
 * mount system call
 */

int
hfs_mount(struct mount *mp, vnode_t devvp, user_addr_t data, vfs_context_t context)
{
	struct proc *p = vfs_context_proc(context);
	struct hfsmount *hfsmp = NULL;
	struct hfs_mount_args args;
	int retval = E_NONE;
	u_int32_t cmdflags;

	if ((retval = copyin(data, (caddr_t)&args, sizeof(args)))) {
		if (HFS_MOUNT_DEBUG) {
			printf("hfs_mount: copyin returned %d for fs\n", retval);
		}
		return (retval);
	}
	cmdflags = (u_int32_t)vfs_flags(mp) & MNT_CMDFLAGS;
	if (cmdflags & MNT_UPDATE) {
		hfsmp = VFSTOHFS(mp);

		/* Reload incore data after an fsck. */
		if (cmdflags & MNT_RELOAD) {
			if (vfs_isrdonly(mp)) {
				int error = hfs_reload(mp);
				if (error && HFS_MOUNT_DEBUG) {
					printf("hfs_mount: hfs_reload returned %d on %s \n", error, hfsmp->vcbVN);
				}
				return error;
			}
			else {
				if (HFS_MOUNT_DEBUG) {
					printf("hfs_mount: MNT_RELOAD not supported on rdwr filesystem %s\n", hfsmp->vcbVN);
				}
				return (EINVAL);
			}
		}

		/* Change to a read-only file system. */
		if (((hfsmp->hfs_flags & HFS_READ_ONLY) == 0) &&
		    vfs_isrdonly(mp)) {
			int flags;

			/* Set flag to indicate that a downgrade to read-only
			 * is in progress and therefore block any further 
			 * modifications to the file system.
			 */
<<<<<<< HEAD
			hfs_lock_global (hfsmp, HFS_EXCLUSIVE_LOCK);
			hfsmp->hfs_flags |= HFS_RDONLY_DOWNGRADE;
			hfsmp->hfs_downgrading_thread = current_thread();
			hfs_unlock_global (hfsmp);
			hfs_syncer_free(hfsmp);
            
=======
			hfs_global_exclusive_lock_acquire(hfsmp);
			hfsmp->hfs_flags |= HFS_RDONLY_DOWNGRADE;
			hfsmp->hfs_downgrading_proc = current_thread();
			hfs_global_exclusive_lock_release(hfsmp);

>>>>>>> origin/10.5
			/* use VFS_SYNC to push out System (btree) files */
			retval = VFS_SYNC(mp, MNT_WAIT, context);
			if (retval && ((cmdflags & MNT_FORCE) == 0)) {
				hfsmp->hfs_flags &= ~HFS_RDONLY_DOWNGRADE;
<<<<<<< HEAD
				hfsmp->hfs_downgrading_thread = NULL;
				if (HFS_MOUNT_DEBUG) {
					printf("hfs_mount: VFS_SYNC returned %d during b-tree sync of %s \n", retval, hfsmp->vcbVN);
				}
=======
				hfsmp->hfs_downgrading_proc = NULL;
>>>>>>> origin/10.5
				goto out;
			}
		
			flags = WRITECLOSE;
			if (cmdflags & MNT_FORCE)
				flags |= FORCECLOSE;
				
			if ((retval = hfs_flushfiles(mp, flags, p))) {
				hfsmp->hfs_flags &= ~HFS_RDONLY_DOWNGRADE;
<<<<<<< HEAD
				hfsmp->hfs_downgrading_thread = NULL;
				if (HFS_MOUNT_DEBUG) {
					printf("hfs_mount: hfs_flushfiles returned %d on %s \n", retval, hfsmp->vcbVN);
				}
=======
				hfsmp->hfs_downgrading_proc = NULL;
				goto out;
			}

			/* mark the volume cleanly unmounted */
			hfsmp->vcbAtrb |= kHFSVolumeUnmountedMask;
			retval = hfs_flushvolumeheader(hfsmp, MNT_WAIT, 0);
			hfsmp->hfs_flags |= HFS_READ_ONLY;

			/* also get the volume bitmap blocks */
			if (!retval) {
				if (vnode_mount(hfsmp->hfs_devvp) == mp) {
					retval = hfs_fsync(hfsmp->hfs_devvp, MNT_WAIT, 0, p);
				} else {
					vnode_get(hfsmp->hfs_devvp);
					retval = VNOP_FSYNC(hfsmp->hfs_devvp, MNT_WAIT, context);
					vnode_put(hfsmp->hfs_devvp);
				}
			}
			if (retval) {
				hfsmp->hfs_flags &= ~HFS_RDONLY_DOWNGRADE;
				hfsmp->hfs_downgrading_proc = NULL;
				hfsmp->hfs_flags &= ~HFS_READ_ONLY;
>>>>>>> origin/10.5
				goto out;
			}

			/* mark the volume cleanly unmounted */
			hfsmp->vcbAtrb |= kHFSVolumeUnmountedMask;
			retval = hfs_flushvolumeheader(hfsmp, HFS_FVH_WAIT);
			hfsmp->hfs_flags |= HFS_READ_ONLY;

			/*
			 * Close down the journal. 
			 *
			 * NOTE: It is critically important to close down the journal
			 * and have it issue all pending I/O prior to calling VNOP_FSYNC below.
			 * In a journaled environment it is expected that the journal be
			 * the only actor permitted to issue I/O for metadata blocks in HFS.
			 * If we were to call VNOP_FSYNC prior to closing down the journal,
			 * we would inadvertantly issue (and wait for) the I/O we just 
			 * initiated above as part of the flushvolumeheader call.
			 * 
			 * To avoid this, we follow the same order of operations as in
			 * unmount and issue the journal_close prior to calling VNOP_FSYNC.
			 */
	
			if (hfsmp->jnl) {
				hfs_lock_global (hfsmp, HFS_EXCLUSIVE_LOCK);

			    journal_close(hfsmp->jnl);
			    hfsmp->jnl = NULL;

			    // Note: we explicitly don't want to shutdown
			    //       access to the jvp because we may need
			    //       it later if we go back to being read-write.

				hfs_unlock_global (hfsmp);

                vfs_clearflags(hfsmp->hfs_mp, MNT_JOURNALED);
			}

			/*
			 * Write out any pending I/O still outstanding against the device node
			 * now that the journal has been closed.
			 */
			if (retval == 0) {
				vnode_get(hfsmp->hfs_devvp);
				retval = VNOP_FSYNC(hfsmp->hfs_devvp, MNT_WAIT, context);
				vnode_put(hfsmp->hfs_devvp);
			}

			if (retval) {
				if (HFS_MOUNT_DEBUG) {
					printf("hfs_mount: FSYNC on devvp returned %d for fs %s\n", retval, hfsmp->vcbVN);
				}
				hfsmp->hfs_flags &= ~HFS_RDONLY_DOWNGRADE;
				hfsmp->hfs_downgrading_thread = NULL;
				hfsmp->hfs_flags &= ~HFS_READ_ONLY;
				goto out;
			}
		
			if (hfsmp->hfs_flags & HFS_SUMMARY_TABLE) {
				if (hfsmp->hfs_summary_table) {
					int err = 0;
					/* 
					 * Take the bitmap lock to serialize against a concurrent bitmap scan still in progress 
					 */
					if (hfsmp->hfs_allocation_vp) {
						err = hfs_lock (VTOC(hfsmp->hfs_allocation_vp), HFS_EXCLUSIVE_LOCK, HFS_LOCK_DEFAULT);
					}
					FREE (hfsmp->hfs_summary_table, M_TEMP);
					hfsmp->hfs_summary_table = NULL;
					hfsmp->hfs_flags &= ~HFS_SUMMARY_TABLE;
					if (err == 0 && hfsmp->hfs_allocation_vp){
						hfs_unlock (VTOC(hfsmp->hfs_allocation_vp));
					}
				}
			}

<<<<<<< HEAD
			hfsmp->hfs_downgrading_thread = NULL;
=======
			hfsmp->hfs_downgrading_proc = NULL;
>>>>>>> origin/10.5
		}

		/* Change to a writable file system. */
		if (vfs_iswriteupgrade(mp)) {
			/*
			 * On inconsistent disks, do not allow read-write mount
			 * unless it is the boot volume being mounted.
			 */
			if (!(vfs_flags(mp) & MNT_ROOTFS) &&
					(hfsmp->vcbAtrb & kHFSVolumeInconsistentMask)) {
				if (HFS_MOUNT_DEBUG) {
					printf("hfs_mount: attempting to mount inconsistent non-root volume %s\n",  (hfsmp->vcbVN));
				}
				retval = EINVAL;
				goto out;
			}

			// If the journal was shut-down previously because we were
			// asked to be read-only, let's start it back up again now
			
			if (   (HFSTOVCB(hfsmp)->vcbAtrb & kHFSVolumeJournaledMask)
			    && hfsmp->jnl == NULL
			    && hfsmp->jvp != NULL) {
			    int jflags;

			    if (hfsmp->hfs_flags & HFS_NEED_JNL_RESET) {
					jflags = JOURNAL_RESET;
				} else {
					jflags = 0;
<<<<<<< HEAD
				}

				hfs_lock_global (hfsmp, HFS_EXCLUSIVE_LOCK);

				/* We provide the mount point twice here: The first is used as
				 * an opaque argument to be passed back when hfs_sync_metadata
				 * is called.  The second is provided to the throttling code to
				 * indicate which mount's device should be used when accounting
				 * for metadata writes.
				 */
				hfsmp->jnl = journal_open(hfsmp->jvp,
						hfs_blk_to_bytes(hfsmp->jnl_start, HFSTOVCB(hfsmp)->blockSize) + (off_t)HFSTOVCB(hfsmp)->hfsPlusIOPosOffset,
						hfsmp->jnl_size,
						hfsmp->hfs_devvp,
						hfsmp->hfs_logical_block_size,
						jflags,
						0,
						hfs_sync_metadata, hfsmp->hfs_mp,
						hfsmp->hfs_mp);
				
				/*
				 * Set up the trim callback function so that we can add
				 * recently freed extents to the free extent cache once
				 * the transaction that freed them is written to the
				 * journal on disk.
				 */
				if (hfsmp->jnl)
					journal_trim_set_callback(hfsmp->jnl, hfs_trim_callback, hfsmp);
				
				hfs_unlock_global (hfsmp);

				if (hfsmp->jnl == NULL) {
					if (HFS_MOUNT_DEBUG) {
						printf("hfs_mount: journal_open == NULL; couldn't be opened on %s \n", (hfsmp->vcbVN));
					}
					retval = EINVAL;
					goto out;
				} else {
					hfsmp->hfs_flags &= ~HFS_NEED_JNL_RESET;
                    vfs_setflags(hfsmp->hfs_mp, MNT_JOURNALED);
				}
			}
=======
			    }
			    
			    hfs_global_exclusive_lock_acquire(hfsmp);

			    hfsmp->jnl = journal_open(hfsmp->jvp,
						      (hfsmp->jnl_start * HFSTOVCB(hfsmp)->blockSize) + (off_t)HFSTOVCB(hfsmp)->hfsPlusIOPosOffset,
						      hfsmp->jnl_size,
						      hfsmp->hfs_devvp,
						      hfsmp->hfs_logical_block_size,
						      jflags,
						      0,
						      hfs_sync_metadata, hfsmp->hfs_mp);

			    hfs_global_exclusive_lock_release(hfsmp);

			    if (hfsmp->jnl == NULL) {
				retval = EINVAL;
				goto out;
			    } else {
				hfsmp->hfs_flags &= ~HFS_NEED_JNL_RESET;
			    }
>>>>>>> origin/10.5

			/* See if we need to erase unused Catalog nodes due to <rdar://problem/6947811>. */
			retval = hfs_erase_unused_nodes(hfsmp);
			if (retval != E_NONE) {
				if (HFS_MOUNT_DEBUG) {
					printf("hfs_mount: hfs_erase_unused_nodes returned %d for fs %s\n", retval, hfsmp->vcbVN);
				}
				goto out;
			}

			/* If this mount point was downgraded from read-write 
			 * to read-only, clear that information as we are now 
			 * moving back to read-write.
			 */
			hfsmp->hfs_flags &= ~HFS_RDONLY_DOWNGRADE;
			hfsmp->hfs_downgrading_thread = NULL;

			/* mark the volume dirty (clear clean unmount bit) */
			hfsmp->vcbAtrb &= ~kHFSVolumeUnmountedMask;

			retval = hfs_flushvolumeheader(hfsmp, HFS_FVH_WAIT);
			if (retval != E_NONE) {
				if (HFS_MOUNT_DEBUG) {
					printf("hfs_mount: hfs_flushvolumeheader returned %d for fs %s\n", retval, hfsmp->vcbVN);
				}
				goto out;
			}
		
			/* Only clear HFS_READ_ONLY after a successful write */
			hfsmp->hfs_flags &= ~HFS_READ_ONLY;

<<<<<<< HEAD
=======
			/* If this mount point was downgraded from read-write 
			 * to read-only, clear that information as we are now 
			 * moving back to read-write.
			 */
			hfsmp->hfs_flags &= ~HFS_RDONLY_DOWNGRADE;
			hfsmp->hfs_downgrading_proc = NULL;

			/* mark the volume dirty (clear clean unmount bit) */
			hfsmp->vcbAtrb &= ~kHFSVolumeUnmountedMask;

			retval = hfs_flushvolumeheader(hfsmp, MNT_WAIT, 0);
			if (retval != E_NONE)
				goto out;
>>>>>>> origin/10.5

			if (!(hfsmp->hfs_flags & (HFS_READ_ONLY | HFS_STANDARD))) {
				/* Setup private/hidden directories for hardlinks. */
				hfs_privatedir_init(hfsmp, FILE_HARDLINKS);
				hfs_privatedir_init(hfsmp, DIR_HARDLINKS);

				hfs_remove_orphans(hfsmp);

				/*
				 * Since we're upgrading to a read-write mount, allow
				 * hot file clustering if conditions allow.
				 *
				 * Note: this normally only would happen if you booted
				 *       single-user and upgraded the mount to read-write
				 *
				 * Note: at this point we are not allowed to fail the
				 *       mount operation because the HotFile init code
				 *       in hfs_recording_init() will lookup vnodes with
				 *       VNOP_LOOKUP() which hangs vnodes off the mount
				 *       (and if we were to fail, VFS is not prepared to
				 *       clean that up at this point.  Since HotFiles are
				 *       optional, this is not a big deal.
				 */
<<<<<<< HEAD
				if ((hfsmp->hfs_flags & HFS_METADATA_ZONE) && 
				    (((hfsmp->hfs_mp->mnt_kern_flag & MNTK_SSD) == 0) || (hfsmp->hfs_flags & HFS_CS_HOTFILE_PIN)) ) {
=======
				if ((hfsmp->hfs_flags & HFS_METADATA_ZONE) &&
				    ((hfsmp->hfs_mp->mnt_kern_flag & MNTK_SSD) == 0)) {
>>>>>>> origin/10.6
					(void) hfs_recording_init(hfsmp);
				}					
				/* Force ACLs on HFS+ file systems. */
				if (vfs_extendedsecurity(HFSTOVFS(hfsmp)) == 0) {
					vfs_setextendedsecurity(HFSTOVFS(hfsmp));
				}
			}
		}

<<<<<<< HEAD
		/* Update file system parameters. */
		retval = hfs_changefs(mp, &args);
		if (retval &&  HFS_MOUNT_DEBUG) {
			printf("hfs_mount: hfs_changefs returned %d for %s\n", retval, hfsmp->vcbVN);
=======
		if ((hfsmp->hfs_fs_ronly == 0) &&
		    (HFSTOVCB(hfsmp)->vcbSigWord == kHFSPlusSigWord)) {
			/* setup private/hidden directory for unlinked files */
			hfsmp->hfs_private_metadata_dir = FindMetaDataDirectory(HFSTOVCB(hfsmp));
			if (hfsmp->jnl)
				hfs_remove_orphans(hfsmp);
>>>>>>> origin/10.2
		}

	} else /* not an update request */ {

		/* Set the mount flag to indicate that we support volfs  */
		vfs_setflags(mp, (u_int64_t)((unsigned int)MNT_DOVOLFS));

		retval = hfs_mountfs(devvp, mp, &args, 0, context);
		if (retval) { 
			const char *name = vnode_getname(devvp);
			printf("hfs_mount: hfs_mountfs returned error=%d for device %s\n", retval, (name ? name : "unknown-dev"));
			if (name) {
				vnode_putname(name);
			}
			goto out;
		}

		/* After hfs_mountfs succeeds, we should have valid hfsmp */
		hfsmp = VFSTOHFS(mp);

	}

out:
	if (retval == 0) {
		(void)hfs_statfs(mp, vfs_statfs(mp), context);
	}
	return (retval);
}


struct hfs_changefs_cargs {
	struct hfsmount *hfsmp;
        int		namefix;
        int		permfix;
        int		permswitch;
};

static int
hfs_changefs_callback(struct vnode *vp, void *cargs)
{
	ExtendedVCB *vcb;
	struct cnode *cp;
	struct cat_desc cndesc;
	struct cat_attr cnattr;
	struct hfs_changefs_cargs *args;
	int lockflags;
	int error;

	args = (struct hfs_changefs_cargs *)cargs;

	cp = VTOC(vp);
	vcb = HFSTOVCB(args->hfsmp);

	lockflags = hfs_systemfile_lock(args->hfsmp, SFL_CATALOG, HFS_SHARED_LOCK);
<<<<<<< HEAD
	error = cat_lookup(args->hfsmp, &cp->c_desc, 0, 0, &cndesc, &cnattr, NULL, NULL);
=======
	error = cat_lookup(args->hfsmp, &cp->c_desc, 0, &cndesc, &cnattr, NULL, NULL);
>>>>>>> origin/10.5
	hfs_systemfile_unlock(args->hfsmp, lockflags);
	if (error) {
	        /*
		 * If we couldn't find this guy skip to the next one
		 */
	        if (args->namefix)
		        cache_purge(vp);

		return (VNODE_RETURNED);
	}
	/*
	 * Get the real uid/gid and perm mask from disk.
	 */
	if (args->permswitch || args->permfix) {
	        cp->c_uid = cnattr.ca_uid;
		cp->c_gid = cnattr.ca_gid;
		cp->c_mode = cnattr.ca_mode;
	}
	/*
	 * If we're switching name converters then...
	 *   Remove the existing entry from the namei cache.
	 *   Update name to one based on new encoder.
	 */
	if (args->namefix) {
	        cache_purge(vp);
		replace_desc(cp, &cndesc);

		if (cndesc.cd_cnid == kHFSRootFolderID) {
		        strlcpy((char *)vcb->vcbVN, (const char *)cp->c_desc.cd_nameptr, NAME_MAX+1);
			cp->c_desc.cd_encoding = args->hfsmp->hfs_encoding;
		}
	} else {
	        cat_releasedesc(&cndesc);
	}
<<<<<<< HEAD
	return (VNODE_RETURNED);
=======

	if (retval != E_NONE) {
		goto error_exit;
	}

	/* Set the mount flag to indicate that we support volfs  */
	mp->mnt_flag |= MNT_DOVOLFS;
    if (VFSTOVCB(mp)->vcbSigWord == kHFSSigWord) {
    	/* HFS volumes only want roman-encoded names: */
    	mp->mnt_flag |= MNT_FIXEDSCRIPTENCODING;
    }
	(void) copyinstr(path, mp->mnt_stat.f_mntonname, MNAMELEN-1, &size);

	bzero(mp->mnt_stat.f_mntonname + size, MNAMELEN - size);
	(void) copyinstr(args.fspec, mp->mnt_stat.f_mntfromname, MNAMELEN - 1, &size);
	bzero(mp->mnt_stat.f_mntfromname + size, MNAMELEN - size);
	(void)hfs_statfs(mp, &mp->mnt_stat, p);
	return (E_NONE);

error_exit:

	return (retval);
>>>>>>> origin/10.2
}

/* Change fs mount parameters */
static int
hfs_changefs(struct mount *mp, struct hfs_mount_args *args)
{
	int retval = 0;
	int namefix, permfix, permswitch;
	struct hfsmount *hfsmp;
	ExtendedVCB *vcb;
	struct hfs_changefs_cargs cargs;
	u_int32_t mount_flags;

#if CONFIG_HFS_STD
	u_int32_t old_encoding = 0;
	hfs_to_unicode_func_t	get_unicode_func;
	unicode_to_hfs_func_t	get_hfsname_func;
#endif

	hfsmp = VFSTOHFS(mp);
	vcb = HFSTOVCB(hfsmp);
	mount_flags = (unsigned int)vfs_flags(mp);

	hfsmp->hfs_flags |= HFS_IN_CHANGEFS;
	
	permswitch = (((hfsmp->hfs_flags & HFS_UNKNOWN_PERMS) &&
	               ((mount_flags & MNT_UNKNOWNPERMISSIONS) == 0)) ||
	              (((hfsmp->hfs_flags & HFS_UNKNOWN_PERMS) == 0) &&
	               (mount_flags & MNT_UNKNOWNPERMISSIONS)));

	/* The root filesystem must operate with actual permissions: */
	if (permswitch && (mount_flags & MNT_ROOTFS) && (mount_flags & MNT_UNKNOWNPERMISSIONS)) {
		vfs_clearflags(mp, (u_int64_t)((unsigned int)MNT_UNKNOWNPERMISSIONS));	/* Just say "No". */
		retval = EINVAL;
		goto exit;
	}
	if (mount_flags & MNT_UNKNOWNPERMISSIONS)
		hfsmp->hfs_flags |= HFS_UNKNOWN_PERMS;
	else
		hfsmp->hfs_flags &= ~HFS_UNKNOWN_PERMS;

	namefix = permfix = 0;

	/*
	 * Tracking of hot files requires up-to-date access times.  So if
	 * access time updates are disabled, we must also disable hot files.
	 */
	if (mount_flags & MNT_NOATIME) {
		(void) hfs_recording_suspend(hfsmp);
	}
	
	/* Change the timezone (Note: this affects all hfs volumes and hfs+ volume create dates) */
	if (args->hfs_timezone.tz_minuteswest != VNOVAL) {
		gTimeZone = args->hfs_timezone;
	}

	/* Change the default uid, gid and/or mask */
	if ((args->hfs_uid != (uid_t)VNOVAL) && (hfsmp->hfs_uid != args->hfs_uid)) {
		hfsmp->hfs_uid = args->hfs_uid;
		if (vcb->vcbSigWord == kHFSPlusSigWord)
			++permfix;
	}
	if ((args->hfs_gid != (gid_t)VNOVAL) && (hfsmp->hfs_gid != args->hfs_gid)) {
		hfsmp->hfs_gid = args->hfs_gid;
		if (vcb->vcbSigWord == kHFSPlusSigWord)
			++permfix;
	}
	if (args->hfs_mask != (mode_t)VNOVAL) {
		if (hfsmp->hfs_dir_mask != (args->hfs_mask & ALLPERMS)) {
			hfsmp->hfs_dir_mask = args->hfs_mask & ALLPERMS;
			hfsmp->hfs_file_mask = args->hfs_mask & ALLPERMS;
			if ((args->flags != VNOVAL) && (args->flags & HFSFSMNT_NOXONFILES))
				hfsmp->hfs_file_mask = (args->hfs_mask & DEFFILEMODE);
			if (vcb->vcbSigWord == kHFSPlusSigWord)
				++permfix;
		}
	}
	
#if CONFIG_HFS_STD
	/* Change the hfs encoding value (hfs only) */
	if ((vcb->vcbSigWord == kHFSSigWord)	&&
	    (args->hfs_encoding != (u_int32_t)VNOVAL)              &&
	    (hfsmp->hfs_encoding != args->hfs_encoding)) {

		retval = hfs_getconverter(args->hfs_encoding, &get_unicode_func, &get_hfsname_func);
		if (retval)
			goto exit;

		/*
		 * Connect the new hfs_get_unicode converter but leave
		 * the old hfs_get_hfsname converter in place so that
		 * we can lookup existing vnodes to get their correctly
		 * encoded names.
		 *
		 * When we're all finished, we can then connect the new
		 * hfs_get_hfsname converter and release our interest
		 * in the old converters.
		 */
		hfsmp->hfs_get_unicode = get_unicode_func;
		old_encoding = hfsmp->hfs_encoding;
		hfsmp->hfs_encoding = args->hfs_encoding;
		++namefix;
	}
#endif

	if (!(namefix || permfix || permswitch))
		goto exit;

	/* XXX 3762912 hack to support HFS filesystem 'owner' */
	if (permfix)
		vfs_setowner(mp,
		    hfsmp->hfs_uid == UNKNOWNUID ? KAUTH_UID_NONE : hfsmp->hfs_uid,
		    hfsmp->hfs_gid == UNKNOWNGID ? KAUTH_GID_NONE : hfsmp->hfs_gid);
	
	/*
	 * For each active vnode fix things that changed
	 *
	 * Note that we can visit a vnode more than once
	 * and we can race with fsync.
	 *
	 * hfs_changefs_callback will be called for each vnode
	 * hung off of this mount point
	 *
	 * The vnode will be properly referenced and unreferenced 
	 * around the callback
	 */
	cargs.hfsmp = hfsmp;
	cargs.namefix = namefix;
	cargs.permfix = permfix;
	cargs.permswitch = permswitch;

	vnode_iterate(mp, 0, hfs_changefs_callback, (void *)&cargs);

#if CONFIG_HFS_STD
	/*
	 * If we're switching name converters we can now
	 * connect the new hfs_get_hfsname converter and
	 * release our interest in the old converters.
	 */
	if (namefix) {
		/* HFS standard only */
		hfsmp->hfs_get_hfsname = get_hfsname_func;
		vcb->volumeNameEncodingHint = args->hfs_encoding;
		(void) hfs_relconverter(old_encoding);
	}
#endif

exit:
	hfsmp->hfs_flags &= ~HFS_IN_CHANGEFS;
	return (retval);
}


struct hfs_reload_cargs {
	struct hfsmount *hfsmp;
        int		error;
};

static int
hfs_reload_callback(struct vnode *vp, void *cargs)
{
	struct cnode *cp;
	struct hfs_reload_cargs *args;
	int lockflags;

	args = (struct hfs_reload_cargs *)cargs;
	/*
	 * flush all the buffers associated with this node
	 */
	(void) buf_invalidateblks(vp, 0, 0, 0);

	cp = VTOC(vp);
	/* 
	 * Remove any directory hints
	 */
	if (vnode_isdir(vp))
	        hfs_reldirhints(cp, 0);

	/*
	 * Re-read cnode data for all active vnodes (non-metadata files).
	 */
	if (!vnode_issystem(vp) && !VNODE_IS_RSRC(vp) && (cp->c_fileid >= kHFSFirstUserCatalogNodeID)) {
	        struct cat_fork *datafork;
		struct cat_desc desc;

		datafork = cp->c_datafork ? &cp->c_datafork->ff_data : NULL;

		/* lookup by fileID since name could have changed */
		lockflags = hfs_systemfile_lock(args->hfsmp, SFL_CATALOG, HFS_SHARED_LOCK);
<<<<<<< HEAD
<<<<<<< HEAD
		args->error = cat_idlookup(args->hfsmp, cp->c_fileid, 0, 0, &desc, &cp->c_attr, datafork);
=======
		args->error = cat_idlookup(args->hfsmp, cp->c_fileid, 0, &desc, &cp->c_attr, datafork);
>>>>>>> origin/10.5
=======
		args->error = cat_idlookup(args->hfsmp, cp->c_fileid, 0, 0, &desc, &cp->c_attr, datafork);
>>>>>>> origin/10.8
		hfs_systemfile_unlock(args->hfsmp, lockflags);
		if (args->error) {
		        return (VNODE_RETURNED_DONE);
		}

		/* update cnode's catalog descriptor */
		(void) replace_desc(cp, &desc);
	}
	return (VNODE_RETURNED);
}

/*
 * Reload all incore data for a filesystem (used after running fsck on
 * the root filesystem and finding things to fix). The filesystem must
 * be mounted read-only.
 *
 * Things to do to update the mount:
 *	invalidate all cached meta-data.
 *	invalidate all inactive vnodes.
 *	invalidate all cached file data.
 *	re-read volume header from disk.
 *	re-load meta-file info (extents, file size).
 *	re-load B-tree header data.
 *	re-read cnode data for all active vnodes.
 */
int
hfs_reload(struct mount *mountp)
{
	register struct vnode *devvp;
	struct buf *bp;
<<<<<<< HEAD
<<<<<<< HEAD
=======
	int sectorsize;
>>>>>>> origin/10.1
=======
>>>>>>> origin/10.5
	int error, i;
	struct hfsmount *hfsmp;
	struct HFSPlusVolumeHeader *vhp;
	ExtendedVCB *vcb;
	struct filefork *forkp;
    	struct cat_desc cndesc;
	struct hfs_reload_cargs args;
	daddr64_t priIDSector;

    	hfsmp = VFSTOHFS(mountp);
	vcb = HFSTOVCB(hfsmp);

	if (vcb->vcbSigWord == kHFSSigWord)
		return (EINVAL);	/* rooting from HFS is not supported! */

	/*
	 * Invalidate all cached meta-data.
	 */
	devvp = hfsmp->hfs_devvp;
	if (buf_invalidateblks(devvp, 0, 0, 0))
		panic("hfs_reload: dirty1");

	args.hfsmp = hfsmp;
	args.error = 0;
	/*
	 * hfs_reload_callback will be called for each vnode
	 * hung off of this mount point that can't be recycled...
	 * vnode_iterate will recycle those that it can (the VNODE_RELOAD option)
	 * the vnode will be in an 'unbusy' state (VNODE_WAIT) and 
	 * properly referenced and unreferenced around the callback
	 */
	vnode_iterate(mountp, VNODE_RELOAD | VNODE_WAIT, hfs_reload_callback, (void *)&args);

	if (args.error)
	        return (args.error);

	/*
	 * Re-read VolumeHeader from disk.
	 */
<<<<<<< HEAD
<<<<<<< HEAD
=======
>>>>>>> origin/10.5
	priIDSector = (daddr64_t)((vcb->hfsPlusIOPosOffset / hfsmp->hfs_logical_block_size) + 
			HFS_PRI_SECTOR(hfsmp->hfs_logical_block_size));

	error = (int)buf_meta_bread(hfsmp->hfs_devvp,
			HFS_PHYSBLK_ROUNDDOWN(priIDSector, hfsmp->hfs_log_per_phys),
			hfsmp->hfs_physical_block_size, NOCRED, &bp);
<<<<<<< HEAD
=======
	sectorsize = hfsmp->hfs_phys_block_size;

	error = meta_bread(hfsmp->hfs_devvp,
			(vcb->hfsPlusIOPosOffset / sectorsize) + HFS_PRI_SECTOR(sectorsize),
			sectorsize, NOCRED, &bp);
>>>>>>> origin/10.1
=======
>>>>>>> origin/10.5
	if (error) {
        	if (bp != NULL)
        		buf_brelse(bp);
		return (error);
	}

<<<<<<< HEAD
<<<<<<< HEAD
	vhp = (HFSPlusVolumeHeader *) (buf_dataptr(bp) + HFS_PRI_OFFSET(hfsmp->hfs_physical_block_size));
=======
	vhp = (HFSPlusVolumeHeader *) (bp->b_data + HFS_PRI_OFFSET(sectorsize));
>>>>>>> origin/10.1
=======
	vhp = (HFSPlusVolumeHeader *) (buf_dataptr(bp) + HFS_PRI_OFFSET(hfsmp->hfs_physical_block_size));
>>>>>>> origin/10.5

	/* Do a quick sanity check */
	if ((SWAP_BE16(vhp->signature) != kHFSPlusSigWord &&
	     SWAP_BE16(vhp->signature) != kHFSXSigWord) ||
	    (SWAP_BE16(vhp->version) != kHFSPlusVersion &&
	     SWAP_BE16(vhp->version) != kHFSXVersion) ||
	    SWAP_BE32(vhp->blockSize) != vcb->blockSize) {
		buf_brelse(bp);
		return (EIO);
	}

	vcb->vcbLsMod		= to_bsd_time(SWAP_BE32(vhp->modifyDate));
<<<<<<< HEAD
	vcb->vcbAtrb		= SWAP_BE32 (vhp->attributes);
=======
	vcb->vcbAtrb		= (UInt16) SWAP_BE32 (vhp->attributes);	/* VCB only uses lower 16 bits */
>>>>>>> origin/10.2
	vcb->vcbJinfoBlock  = SWAP_BE32(vhp->journalInfoBlock);
	vcb->vcbClpSiz		= SWAP_BE32 (vhp->rsrcClumpSize);
	vcb->vcbNxtCNID		= SWAP_BE32 (vhp->nextCatalogID);
	vcb->vcbVolBkUp		= to_bsd_time(SWAP_BE32(vhp->backupDate));
	vcb->vcbWrCnt		= SWAP_BE32 (vhp->writeCount);
	vcb->vcbFilCnt		= SWAP_BE32 (vhp->fileCount);
	vcb->vcbDirCnt		= SWAP_BE32 (vhp->folderCount);
	HFS_UPDATE_NEXT_ALLOCATION(vcb, SWAP_BE32 (vhp->nextAllocation));
	vcb->totalBlocks	= SWAP_BE32 (vhp->totalBlocks);
	vcb->freeBlocks		= SWAP_BE32 (vhp->freeBlocks);
	vcb->encodingsBitmap	= SWAP_BE64 (vhp->encodingsBitmap);
	bcopy(vhp->finderInfo, vcb->vcbFndrInfo, sizeof(vhp->finderInfo));    
	vcb->localCreateDate	= SWAP_BE32 (vhp->createDate); /* hfs+ create date is in local time */ 

	/*
	 * Re-load meta-file vnode data (extent info, file size, etc).
	 */
	forkp = VTOF((struct vnode *)vcb->extentsRefNum);
	for (i = 0; i < kHFSPlusExtentDensity; i++) {
		forkp->ff_extents[i].startBlock =
			SWAP_BE32 (vhp->extentsFile.extents[i].startBlock);
		forkp->ff_extents[i].blockCount =
			SWAP_BE32 (vhp->extentsFile.extents[i].blockCount);
	}
	forkp->ff_size      = SWAP_BE64 (vhp->extentsFile.logicalSize);
	forkp->ff_blocks    = SWAP_BE32 (vhp->extentsFile.totalBlocks);
	forkp->ff_clumpsize = SWAP_BE32 (vhp->extentsFile.clumpSize);


	forkp = VTOF((struct vnode *)vcb->catalogRefNum);
	for (i = 0; i < kHFSPlusExtentDensity; i++) {
		forkp->ff_extents[i].startBlock	=
			SWAP_BE32 (vhp->catalogFile.extents[i].startBlock);
		forkp->ff_extents[i].blockCount	=
			SWAP_BE32 (vhp->catalogFile.extents[i].blockCount);
	}
	forkp->ff_size      = SWAP_BE64 (vhp->catalogFile.logicalSize);
	forkp->ff_blocks    = SWAP_BE32 (vhp->catalogFile.totalBlocks);
	forkp->ff_clumpsize = SWAP_BE32 (vhp->catalogFile.clumpSize);

	if (hfsmp->hfs_attribute_vp) {
		forkp = VTOF(hfsmp->hfs_attribute_vp);
		for (i = 0; i < kHFSPlusExtentDensity; i++) {
			forkp->ff_extents[i].startBlock	=
				SWAP_BE32 (vhp->attributesFile.extents[i].startBlock);
			forkp->ff_extents[i].blockCount	=
				SWAP_BE32 (vhp->attributesFile.extents[i].blockCount);
		}
		forkp->ff_size      = SWAP_BE64 (vhp->attributesFile.logicalSize);
		forkp->ff_blocks    = SWAP_BE32 (vhp->attributesFile.totalBlocks);
		forkp->ff_clumpsize = SWAP_BE32 (vhp->attributesFile.clumpSize);
	}

	forkp = VTOF((struct vnode *)vcb->allocationsRefNum);
	for (i = 0; i < kHFSPlusExtentDensity; i++) {
		forkp->ff_extents[i].startBlock	=
			SWAP_BE32 (vhp->allocationFile.extents[i].startBlock);
		forkp->ff_extents[i].blockCount	=
			SWAP_BE32 (vhp->allocationFile.extents[i].blockCount);
	}
	forkp->ff_size      = SWAP_BE64 (vhp->allocationFile.logicalSize);
	forkp->ff_blocks    = SWAP_BE32 (vhp->allocationFile.totalBlocks);
	forkp->ff_clumpsize = SWAP_BE32 (vhp->allocationFile.clumpSize);

	buf_brelse(bp);
	vhp = NULL;

	/*
	 * Re-load B-tree header data
	 */
	forkp = VTOF((struct vnode *)vcb->extentsRefNum);
	if ( (error = MacToVFSError( BTReloadData((FCB*)forkp) )) )
		return (error);

	forkp = VTOF((struct vnode *)vcb->catalogRefNum);
	if ( (error = MacToVFSError( BTReloadData((FCB*)forkp) )) )
		return (error);

	if (hfsmp->hfs_attribute_vp) {
		forkp = VTOF(hfsmp->hfs_attribute_vp);
		if ( (error = MacToVFSError( BTReloadData((FCB*)forkp) )) )
			return (error);
	}

	/* Reload the volume name */
	if ((error = cat_idlookup(hfsmp, kHFSRootFolderID, 0, 0, &cndesc, NULL, NULL)))
		return (error);
	vcb->volumeNameEncodingHint = cndesc.cd_encoding;
	bcopy(cndesc.cd_nameptr, vcb->vcbVN, min(255, cndesc.cd_namelen));
	cat_releasedesc(&cndesc);

	/* Re-establish private/hidden directories. */
	hfs_privatedir_init(hfsmp, FILE_HARDLINKS);
	hfs_privatedir_init(hfsmp, DIR_HARDLINKS);

	/* In case any volume information changed to trigger a notification */
	hfs_generate_volume_notifications(hfsmp);
    
	return (0);
}

<<<<<<< HEAD
__unused
static uint64_t tv_to_usecs(struct timeval *tv)
{
	return tv->tv_sec * 1000000ULL + tv->tv_usec;
}

<<<<<<< HEAD
// Returns TRUE if b - a >= usecs
static boolean_t hfs_has_elapsed (const struct timeval *a, 
                                  const struct timeval *b,
                                  uint64_t usecs)
=======
static int
get_raw_device(char *fspec, int is_user, int ronly, struct vnode **rvp, struct ucred *cred, struct proc *p)
{
	char            *rawbuf;
	char            *dp;
	size_t           namelen;
	struct nameidata nd;
	int               retval;

	*rvp = NULL;

	MALLOC(rawbuf, char *, MAXPATHLEN, M_HFSMNT, M_WAITOK);
	if (rawbuf == NULL) {
		retval = ENOMEM;
		goto error_exit;
	}

	if (is_user) {
		retval = copyinstr(fspec, rawbuf, MAXPATHLEN - 1, &namelen);
		if (retval != E_NONE) {
			FREE(rawbuf, M_HFSMNT);
			goto error_exit;
		}
	} else {
		strcpy(rawbuf, fspec);
		namelen = strlen(rawbuf);
	}

	/* make sure it's null terminated */
	rawbuf[MAXPATHLEN-1] = '\0';   

	dp = &rawbuf[namelen-1];
	while(dp >= rawbuf && *dp != '/') {
		dp--;
	}
			
	if (dp != NULL) {
		dp++;
	} else {
		dp = rawbuf;
	}
			
	/* make room for and insert the 'r' for the raw device */
	memmove(dp+1, dp, strlen(dp)+1);
	*dp = 'r';

	NDINIT(&nd, LOOKUP, FOLLOW, UIO_SYSSPACE, rawbuf, p);
	retval = namei(&nd);
	if (retval != E_NONE) {
		DBG_ERR(("hfs_mountfs: can't open raw device for journal: %s, %x\n", rawbuf, nd.ni_vp->v_rdev));
		FREE(rawbuf, M_HFSMNT);
		goto error_exit;
	}

	*rvp = nd.ni_vp;
	if ((retval = VOP_OPEN(*rvp, ronly ? FREAD : FREAD|FWRITE, FSCRED, p))) {
		*rvp = NULL;
		goto error_exit;
	}

	// don't need this any more
	FREE(rawbuf, M_HFSMNT);

	return 0;

  error_exit:
	if (*rvp) {
	    (void)VOP_CLOSE(*rvp, ronly ? FREAD : FREAD|FWRITE, cred, p);
	}

	if (rawbuf) {
		FREE(rawbuf, M_HFSMNT);
	}
	return retval;
}


=======
int hfs_last_io_wait_time = 125000;
SYSCTL_INT (_kern, OID_AUTO, hfs_last_io_wait_time, CTLFLAG_RW, &hfs_last_io_wait_time, 0, "number of usecs to wait after an i/o before syncing ejectable media");

static void
hfs_syncer(void *arg0, void *unused)
{
#pragma unused(unused)

    struct hfsmount *hfsmp = arg0;
    uint32_t secs, usecs, delay = HFS_META_DELAY;
    uint64_t now;
    struct timeval nowtv, last_io;

    clock_get_calendar_microtime(&secs, &usecs);
    now = ((uint64_t)secs * 1000000LL) + usecs;
    //
    // If we have put off the last sync for more than
    // 5 seconds, force it so that we don't let too
    // much i/o queue up (since flushing the journal
    // causes the i/o queue to drain)
    //
<<<<<<< HEAD
    if ((now - hfsmp->hfs_last_sync_time) >= 5000000LL) {
	    goto doit;
    }
=======
    if (hfsmp->hfs_mp->mnt_pending_write_size > hfsmp->hfs_max_pending_io) {
	    int counter=0;
	    uint64_t pending_io, start, rate = 0;
	    
	    no_max = 0;

	    hfs_start_transaction(hfsmp);   // so we hold off any new i/o's

	    pending_io = hfsmp->hfs_mp->mnt_pending_write_size;
	    
	    clock_get_calendar_microtime(&secs, &usecs);
	    start = ((uint64_t)secs * 1000000ULL) + (uint64_t)usecs;

	    while(hfsmp->hfs_mp->mnt_pending_write_size > (pending_io/3) && counter++ < 500) {
		    tsleep((caddr_t)hfsmp, PRIBIO, "hfs-wait-for-io-to-drain", 10);
	    }
>>>>>>> origin/10.7

    //
    // Find out when the last i/o was done to this device (read or write).  
    //
    throttle_info_get_last_io_time(hfsmp->hfs_mp, &last_io);
    microuptime(&nowtv);
    timevalsub(&nowtv, &last_io);

    //
    // If the last i/o was too recent, defer this sync until later.
    // The limit chosen (125 milli-seconds) was picked based on
    // some experiments copying data to an SD card and seems to
    // prevent us from issuing too many syncs.
    //
    if (nowtv.tv_sec >= 0 && nowtv.tv_usec > 0 && nowtv.tv_usec < hfs_last_io_wait_time) {
	    delay /= 2;
	    goto resched;
    }
    
    //
    // If there's pending i/o, also skip the sync.
    //
    if (hfsmp->hfs_devvp && hfsmp->hfs_devvp->v_numoutput > 0) {
	    goto resched;
    }

<<<<<<< HEAD
		
    //
    // Only flush the journal if we have not sync'ed recently
    // and the last sync request time was more than 100 milli
    // seconds ago and there is no one in the middle of a
    // transaction right now.  Else we defer the sync and
    // reschedule it for later.
    //
    if (  ((now - hfsmp->hfs_last_sync_time) >= 100000LL)
       && ((now - hfsmp->hfs_last_sync_request_time) >= 100000LL)
       && (hfsmp->hfs_active_threads == 0)
       && (hfsmp->hfs_global_lock_nesting == 0)) {

    doit:
	    OSAddAtomic(1, (SInt32 *)&hfsmp->hfs_active_threads);
=======
	    clock_get_calendar_microtime(&secs, &usecs);
	    now = ((uint64_t)secs * 1000000ULL) + (uint64_t)usecs;
	    hfsmp->hfs_last_sync_time = now;
	    if (now != start) {
		    rate = ((pending_io * 1000000ULL) / (now - start));     // yields bytes per second
	    }

	    hfs_end_transaction(hfsmp);
	    
	    //
	    // If a reasonable amount of time elapsed then check the
	    // i/o rate.  If it's taking less than 1 second or more
	    // than 2 seconds, adjust hfs_max_pending_io so that we
	    // will allow about 1.5 seconds of i/o to queue up.
	    //
	    if (((now - start) >= 300000) && (rate != 0)) {
		    uint64_t scale = (pending_io * 100) / rate;
		    
		    if (scale < 100 || scale > 200) {
			    // set it so that it should take about 1.5 seconds to drain
			    hfsmp->hfs_max_pending_io = (rate * 150ULL) / 100ULL;
		    }
	    }
  
    } else if (   ((now - hfsmp->hfs_last_sync_time) >= 5000000ULL)
	       || (((now - hfsmp->hfs_last_sync_time) >= 100000LL)
		   && ((now - hfsmp->hfs_last_sync_request_time) >= 100000LL)
		   && (hfsmp->hfs_active_threads == 0)
		   && (hfsmp->hfs_global_lock_nesting == 0))) {

	    //
	    // Flush the journal if more than 5 seconds elapsed since
	    // the last sync OR we have not sync'ed recently and the
	    // last sync request time was more than 100 milliseconds
	    // ago and no one is in the middle of a transaction right
	    // now.  Else we defer the sync and reschedule it.
	    //
>>>>>>> origin/10.7
	    if (hfsmp->jnl) {
		    journal_flush(hfsmp->jnl);
	    }
	    OSAddAtomic(-1, (SInt32 *)&hfsmp->hfs_active_threads);
		   
	    clock_get_calendar_microtime(&secs, &usecs);
	    hfsmp->hfs_last_sync_time = ((int64_t)secs * 1000000) + usecs;
	    
    } else if (hfsmp->hfs_active_threads == 0) {
	    uint64_t deadline;

    resched:
	    clock_interval_to_deadline(delay, HFS_MILLISEC_SCALE, &deadline);
	    thread_call_enter_delayed(hfsmp->hfs_syncer, deadline);
	    return;
    }
	    
    //
    // NOTE: we decrement these *after* we're done the journal_flush() since
    // it can take a significant amount of time and so we don't want more
    // callbacks scheduled until we're done this one.
    //
    OSDecrementAtomic((volatile SInt32 *)&hfsmp->hfs_sync_scheduled);
    OSDecrementAtomic((volatile SInt32 *)&hfsmp->hfs_sync_incomplete);
    wakeup((caddr_t)&hfsmp->hfs_sync_incomplete);
}

extern int IOBSDIsMediaEjectable( const char *cdev_name );
>>>>>>> origin/10.5

/*
 * Common code for mount and mountroot
 */
static int
hfs_mountfs(struct vnode *devvp, struct mount *mp, struct proc *p,
	struct hfs_mount_args *args)
>>>>>>> origin/10.2
{
<<<<<<< HEAD
    struct timeval diff;
    timersub(b, a, &diff);
    return diff.tv_sec * 1000000ULL + diff.tv_usec >= usecs;
}

static void
hfs_syncer(void *arg0, __unused void *unused)
{
    struct hfsmount *hfsmp = arg0;
    struct timeval   now;
=======
	struct proc *p = vfs_context_proc(context);
	int retval = E_NONE;
	struct hfsmount	*hfsmp = NULL;
	struct buf *bp;
	dev_t dev;
	HFSMasterDirectoryBlock *mdbp = NULL;
	int ronly;
#if QUOTA
	int i;
#endif
	int mntwrapper;
	kauth_cred_t cred;
	u_int64_t disksize;
	daddr64_t log_blkcnt;
	u_int32_t log_blksize;
	u_int32_t phys_blksize;
	u_int32_t minblksize;
	u_int32_t iswritable;
	daddr64_t mdb_offset;
	int isvirtual = 0;
	int isroot = 0;
	u_int32_t device_features = 0;
	
	if (args == NULL) {
		/* only hfs_mountroot passes us NULL as the 'args' argument */
		isroot = 1;
	}
>>>>>>> origin/10.6

    microuptime(&now);

    KERNEL_DEBUG_CONSTANT(HFSDBG_SYNCER | DBG_FUNC_START, hfsmp, 
                          tv_to_usecs(&now),
                          tv_to_usecs(&hfsmp->hfs_mp->mnt_last_write_completed_timestamp), 
                          hfsmp->hfs_mp->mnt_pending_write_size, 0);

    hfs_syncer_lock(hfsmp);

    if (!hfsmp->hfs_syncer) {
        // hfs_unmount is waiting for us leave now and let it do the sync
        hfsmp->hfs_sync_incomplete = FALSE;
        hfs_syncer_unlock(hfsmp);
        hfs_syncer_wakeup(hfsmp);
        return;
    }

    /* Check to see whether we should flush now: either the oldest is
       > HFS_MAX_META_DELAY or HFS_META_DELAY has elapsed since the
       request and there are no pending writes. */

    boolean_t flush_now = FALSE;

    if (hfs_has_elapsed(&hfsmp->hfs_sync_req_oldest, &now, HFS_MAX_META_DELAY))
        flush_now = TRUE;
    else if (!hfsmp->hfs_mp->mnt_pending_write_size) {
        /* N.B. accessing mnt_last_write_completed_timestamp is not thread safe, but
           it won't matter for what we're using it for. */
        if (hfs_has_elapsed(&hfsmp->hfs_mp->mnt_last_write_completed_timestamp,
                            &now,
                            HFS_META_DELAY)) {
            flush_now = TRUE;
        }
    }

    if (!flush_now) {
        thread_call_t syncer = hfsmp->hfs_syncer;

        hfs_syncer_unlock(hfsmp);

        hfs_syncer_queue(syncer);

        return;
    }

    timerclear(&hfsmp->hfs_sync_req_oldest);

    hfs_syncer_unlock(hfsmp);

    KERNEL_DEBUG_CONSTANT(HFSDBG_SYNCER_TIMED | DBG_FUNC_START, 
                          tv_to_usecs(&now),
                          tv_to_usecs(&hfsmp->hfs_mp->mnt_last_write_completed_timestamp),
                          tv_to_usecs(&hfsmp->hfs_mp->mnt_last_write_issued_timestamp), 
                          hfsmp->hfs_mp->mnt_pending_write_size, 0);

    if (hfsmp->hfs_syncer_thread) {
        printf("hfs: syncer already running!\n");
		return;
	}

    hfsmp->hfs_syncer_thread = current_thread();

    /*
     * We intentionally do a synchronous flush (of the journal or entire volume) here.
     * For journaled volumes, this means we wait until the metadata blocks are written
     * to both the journal and their final locations (in the B-trees, etc.).
     *
     * This tends to avoid interleaving the metadata writes with other writes (for
     * example, user data, or to the journal when a later transaction notices that
     * an earlier transaction has finished its async writes, and then updates the
     * journal start in the journal header).  Avoiding interleaving of writes is
     * very good for performance on simple flash devices like SD cards, thumb drives;
     * and on devices like floppies.  Since removable devices tend to be this kind of
     * simple device, doing a synchronous flush actually improves performance in
     * practice.
     *
     * NOTE: For non-journaled volumes, the call to hfs_sync will also cause dirty
     * user data to be written.
     */
    if (hfsmp->jnl) {
        hfs_flush(hfsmp, HFS_FLUSH_JOURNAL_META);
    } else {
        hfs_sync(hfsmp->hfs_mp, MNT_WAIT, vfs_context_kernel());
    }

    KERNEL_DEBUG_CONSTANT(HFSDBG_SYNCER_TIMED | DBG_FUNC_END, 
                          (microuptime(&now), tv_to_usecs(&now)),
                          tv_to_usecs(&hfsmp->hfs_mp->mnt_last_write_completed_timestamp), 
                          tv_to_usecs(&hfsmp->hfs_mp->mnt_last_write_issued_timestamp), 
                          hfsmp->hfs_mp->mnt_pending_write_size, 0);

    hfsmp->hfs_syncer_thread = NULL;

    hfs_syncer_lock(hfsmp);

    // If hfs_unmount lets us and we missed a sync, schedule again
    if (hfsmp->hfs_syncer && timerisset(&hfsmp->hfs_sync_req_oldest)) {
        thread_call_t syncer = hfsmp->hfs_syncer;

        hfs_syncer_unlock(hfsmp);

        hfs_syncer_queue(syncer);
    } else {
        hfsmp->hfs_sync_incomplete = FALSE;
        hfs_syncer_unlock(hfsmp);
        hfs_syncer_wakeup(hfsmp);
    }

    /* BE CAREFUL WHAT YOU ADD HERE: at this point hfs_unmount is free
       to continue and therefore hfsmp might be invalid. */

    KERNEL_DEBUG_CONSTANT(HFSDBG_SYNCER | DBG_FUNC_END, 0, 0, 0, 0, 0);
}


extern int IOBSDIsMediaEjectable( const char *cdev_name );

/*
 * Call into the allocator code and perform a full scan of the bitmap file.
 * 
 * This allows us to TRIM unallocated ranges if needed, and also to build up
 * an in-memory summary table of the state of the allocated blocks.
 */
void hfs_scan_blocks (struct hfsmount *hfsmp) {
	/*
	 * Take the allocation file lock.  Journal transactions will block until
	 * we're done here. 
	 */
	
	int flags = hfs_systemfile_lock(hfsmp, SFL_BITMAP, HFS_EXCLUSIVE_LOCK);
	
	/* 
	 * We serialize here with the HFS mount lock as we're mounting.
	 * 
	 * The mount can only proceed once this thread has acquired the bitmap 
	 * lock, since we absolutely do not want someone else racing in and 
	 * getting the bitmap lock, doing a read/write of the bitmap file, 
	 * then us getting the bitmap lock.
	 * 
	 * To prevent this, the mount thread takes the HFS mount mutex, starts us 
	 * up, then immediately msleeps on the scan_var variable in the mount 
	 * point as a condition variable.  This serialization is safe since 
	 * if we race in and try to proceed while they're still holding the lock, 
	 * we'll block trying to acquire the global lock.  Since the mount thread 
	 * acquires the HFS mutex before starting this function in a new thread, 
	 * any lock acquisition on our part must be linearizably AFTER the mount thread's. 
	 *
	 * Note that the HFS mount mutex is always taken last, and always for only
	 * a short time.  In this case, we just take it long enough to mark the
	 * scan-in-flight bit.
	 */
	(void) hfs_lock_mount (hfsmp);
	hfsmp->scan_var |= HFS_ALLOCATOR_SCAN_INFLIGHT;
	wakeup((caddr_t) &hfsmp->scan_var);
	hfs_unlock_mount (hfsmp);

	/* Initialize the summary table */
	if (hfs_init_summary (hfsmp)) {
		printf("hfs: could not initialize summary table for %s\n", hfsmp->vcbVN);
	}	

	/*
	 * ScanUnmapBlocks assumes that the bitmap lock is held when you 
	 * call the function. We don't care if there were any errors issuing unmaps.
	 *
	 * It will also attempt to build up the summary table for subsequent
	 * allocator use, as configured.
	 */
	(void) ScanUnmapBlocks(hfsmp);

	(void) hfs_lock_mount (hfsmp);
	hfsmp->scan_var &= ~HFS_ALLOCATOR_SCAN_INFLIGHT;
	hfsmp->scan_var |= HFS_ALLOCATOR_SCAN_COMPLETED;
	wakeup((caddr_t) &hfsmp->scan_var);
	hfs_unlock_mount (hfsmp);

	buf_invalidateblks(hfsmp->hfs_allocation_vp, 0, 0, 0);
	
	hfs_systemfile_unlock(hfsmp, flags);

}

static int hfs_root_unmounted_cleanly = 0;

SYSCTL_DECL(_vfs_generic);
SYSCTL_INT(_vfs_generic, OID_AUTO, root_unmounted_cleanly, CTLFLAG_RD, &hfs_root_unmounted_cleanly, 0, "Root filesystem was unmounted cleanly");

/*
 * Common code for mount and mountroot
 */
int
hfs_mountfs(struct vnode *devvp, struct mount *mp, struct hfs_mount_args *args,
            int journal_replay_only, vfs_context_t context)
{
<<<<<<< HEAD
	struct proc *p = vfs_context_proc(context);
	int retval = E_NONE;
	struct hfsmount	*hfsmp = NULL;
	struct buf *bp;
	dev_t dev;
	HFSMasterDirectoryBlock *mdbp = NULL;
	int ronly;
#if QUOTA
	int i;
#endif
	int mntwrapper;
	kauth_cred_t cred;
	u_int64_t disksize;
	daddr64_t log_blkcnt;
	u_int32_t log_blksize;
	u_int32_t phys_blksize;
	u_int32_t minblksize;
	u_int32_t iswritable;
<<<<<<< HEAD
	daddr64_t mdb_offset;
	int isvirtual = 0;
	int isroot = 0;
<<<<<<< HEAD
	u_int32_t device_features = 0;
	int isssd;
	
	if (args == NULL) {
		/* only hfs_mountroot passes us NULL as the 'args' argument */
		isroot = 1;
	}
=======
    int                         retval = E_NONE;
    register struct hfsmount	*hfsmp;
    struct buf					*bp;
    dev_t                       dev;
    HFSMasterDirectoryBlock		*mdbp;
    int                         ronly;
    struct ucred				*cred;
	u_int64_t disksize;
	u_int64_t blkcnt;
	u_int32_t blksize;
	u_int32_t minblksize;
    DBG_VFS(("hfs_mountfs: mp = 0x%lX\n", (u_long)mp));
=======
	daddr_t   mdb_offset;
>>>>>>> origin/10.2

    dev = devvp->v_rdev;
    cred = p ? p->p_ucred : NOCRED;
    /*
     * Disallow multiple mounts of the same device.
     * Disallow mounting of a device that is currently in use
     * (except for root, which might share swap device for miniroot).
     * Flush out any old buffers remaining from a previous use.
     */
    if ((retval = vfs_mountedon(devvp)))
        return (retval);
    if ((vcount(devvp) > 1) && (devvp != rootvp))
                return (EBUSY);
    if ((retval = vinvalbuf(devvp, V_SAVE, cred, p, 0, 0)))
        return (retval);
>>>>>>> origin/10.1
=======
>>>>>>> origin/10.5

	ronly = vfs_isrdonly(mp);
	dev = vnode_specrdev(devvp);
	cred = p ? vfs_context_ucred(context) : NOCRED;
	mntwrapper = 0;

	if (args == NULL) {
		/* only hfs_mountroot passes us NULL as the 'args' argument */
		isroot = 1;	
	}

	bp = NULL;
	hfsmp = NULL;
	mdbp = NULL;
	minblksize = kHFSBlockSize;

	/* Advisory locking should be handled at the VFS layer */
	vfs_setlocklocal(mp);

<<<<<<< HEAD
<<<<<<< HEAD
	/* Get the logical block size (treated as physical block size everywhere) */
	if (VNOP_IOCTL(devvp, DKIOCGETBLOCKSIZE, (caddr_t)&log_blksize, 0, context)) {
		if (HFS_MOUNT_DEBUG) {
			printf("hfs_mountfs: DKIOCGETBLOCKSIZE failed\n");
		}
		retval = ENXIO;
		goto error_exit;
	}
	if (log_blksize == 0 || log_blksize > 1024*1024*1024) {
		printf("hfs: logical block size 0x%x looks bad.  Not mounting.\n", log_blksize);
		retval = ENXIO;
		goto error_exit;
	}
	
	/* Get the physical block size. */
	retval = VNOP_IOCTL(devvp, DKIOCGETPHYSICALBLOCKSIZE, (caddr_t)&phys_blksize, 0, context);
	if (retval) {
		if ((retval != ENOTSUP) && (retval != ENOTTY)) {
			if (HFS_MOUNT_DEBUG) {
				printf("hfs_mountfs: DKIOCGETPHYSICALBLOCKSIZE failed\n");
			}
			retval = ENXIO;
			goto error_exit;
		}
		/* If device does not support this ioctl, assume that physical 
		 * block size is same as logical block size 
		 */
		phys_blksize = log_blksize;
	}
	if (phys_blksize == 0 || phys_blksize > MAXBSIZE) {
		printf("hfs: physical block size 0x%x looks bad.  Not mounting.\n", phys_blksize);
		retval = ENXIO;
		goto error_exit;
	}

=======
	/* Get the logical block size (treated as physical block size everywhere) */
	if (VNOP_IOCTL(devvp, DKIOCGETBLOCKSIZE, (caddr_t)&log_blksize, 0, context)) {
		retval = ENXIO;
		goto error_exit;
	}
	/* Get the physical block size. */
	retval = VNOP_IOCTL(devvp, DKIOCGETPHYSICALBLOCKSIZE, (caddr_t)&phys_blksize, 0, context);
	if (retval) {
		if ((retval != ENOTSUP) && (retval != ENOTTY)) {
			retval = ENXIO;
			goto error_exit;
		}
		/* If device does not support this ioctl, assume that physical 
		 * block size is same as logical block size 
		 */
		phys_blksize = log_blksize;
	}
>>>>>>> origin/10.5
	/* Switch to 512 byte sectors (temporarily) */
	if (log_blksize > 512) {
		u_int32_t size512 = 512;

		if (VNOP_IOCTL(devvp, DKIOCSETBLOCKSIZE, (caddr_t)&size512, FWRITE, context)) {
			if (HFS_MOUNT_DEBUG) {
				printf("hfs_mountfs: DKIOCSETBLOCKSIZE failed \n");
			}
			retval = ENXIO;
			goto error_exit;
		}
	}
	/* Get the number of 512 byte physical blocks. */
	if (VNOP_IOCTL(devvp, DKIOCGETBLOCKCOUNT, (caddr_t)&log_blkcnt, 0, context)) {
		/* resetting block size may fail if getting block count did */
		(void)VNOP_IOCTL(devvp, DKIOCSETBLOCKSIZE, (caddr_t)&log_blksize, FWRITE, context);
<<<<<<< HEAD
		if (HFS_MOUNT_DEBUG) {
			printf("hfs_mountfs: DKIOCGETBLOCKCOUNT failed\n");
		}
=======

>>>>>>> origin/10.5
		retval = ENXIO;
		goto error_exit;
	}
	/* Compute an accurate disk size (i.e. within 512 bytes) */
	disksize = (u_int64_t)log_blkcnt * (u_int64_t)512;

	/*
	 * On Tiger it is not necessary to switch the device 
	 * block size to be 4k if there are more than 31-bits
	 * worth of blocks but to insure compatibility with
	 * pre-Tiger systems we have to do it.
	 *
	 * If the device size is not a multiple of 4K (8 * 512), then
	 * switching the logical block size isn't going to help because
	 * we will be unable to write the alternate volume header.
	 * In this case, just leave the logical block size unchanged.
	 */
	if (log_blkcnt > 0x000000007fffffff && (log_blkcnt & 7) == 0) {
		minblksize = log_blksize = 4096;
		if (phys_blksize < log_blksize)
			phys_blksize = log_blksize;
	}
	
	/*
	 * The cluster layer is not currently prepared to deal with a logical
	 * block size larger than the system's page size.  (It can handle 
	 * blocks per page, but not multiple pages per block.)  So limit the
	 * logical block size to the page size.
	 */
	if (log_blksize > PAGE_SIZE) {
		log_blksize = PAGE_SIZE;
	}

	/* Now switch to our preferred physical block size. */
	if (log_blksize > 512) {
		if (VNOP_IOCTL(devvp, DKIOCSETBLOCKSIZE, (caddr_t)&log_blksize, FWRITE, context)) {
<<<<<<< HEAD
			if (HFS_MOUNT_DEBUG) { 
				printf("hfs_mountfs: DKIOCSETBLOCKSIZE (2) failed\n");
			}
=======
>>>>>>> origin/10.5
			retval = ENXIO;
			goto error_exit;
		}
		/* Get the count of physical blocks. */
		if (VNOP_IOCTL(devvp, DKIOCGETBLOCKCOUNT, (caddr_t)&log_blkcnt, 0, context)) {
<<<<<<< HEAD
			if (HFS_MOUNT_DEBUG) { 
				printf("hfs_mountfs: DKIOCGETBLOCKCOUNT (2) failed\n");
			}
=======
	bp = NULL;
	hfsmp = NULL;
	minblksize = kHFSBlockSize;

	/* Get the real physical block size. */
	if (VOP_IOCTL(devvp, DKIOCGETBLOCKSIZE, (caddr_t)&blksize, 0, cred, p)) {
		retval = ENXIO;
		goto error_exit;
	}
	/* Switch to 512 byte sectors (temporarily) */
	if (blksize > 512) {
		u_int32_t size512 = 512;

		if (VOP_IOCTL(devvp, DKIOCSETBLOCKSIZE, (caddr_t)&size512, FWRITE, cred, p)) {
>>>>>>> origin/10.1
=======
>>>>>>> origin/10.5
			retval = ENXIO;
			goto error_exit;
		}
	}
<<<<<<< HEAD
	/*
	 * At this point:
	 *   minblksize is the minimum physical block size
	 *   log_blksize has our preferred physical block size
	 *   log_blkcnt has the total number of physical blocks
<<<<<<< HEAD
=======
	/* Get the number of 512 byte physical blocks. */
	if (VOP_IOCTL(devvp, DKIOCGETBLOCKCOUNT, (caddr_t)&blkcnt, 0, cred, p)) {
		retval = ENXIO;
		goto error_exit;
	}
	/* Compute an accurate disk size (i.e. within 512 bytes) */
	disksize = blkcnt * (u_int64_t)512;

	/*
	 * For large volumes use a 4K physical block size.
>>>>>>> origin/10.1
=======
>>>>>>> origin/10.5
	 */
	if (blkcnt > (u_int64_t)0x000000007fffffff) {
		minblksize = blksize = 4096;
	}

<<<<<<< HEAD
<<<<<<< HEAD
=======
>>>>>>> origin/10.5
	mdb_offset = (daddr64_t)HFS_PRI_SECTOR(log_blksize);
	if ((retval = (int)buf_meta_bread(devvp, 
				HFS_PHYSBLK_ROUNDDOWN(mdb_offset, (phys_blksize/log_blksize)), 
				phys_blksize, cred, &bp))) {
<<<<<<< HEAD
		if (HFS_MOUNT_DEBUG) {
			printf("hfs_mountfs: buf_meta_bread failed with %d\n", retval);
		}
		goto error_exit;
	}
	MALLOC(mdbp, HFSMasterDirectoryBlock *, kMDBSize, M_TEMP, M_WAITOK);
	if (mdbp == NULL) {
		retval = ENOMEM;
		if (HFS_MOUNT_DEBUG) { 
			printf("hfs_mountfs: MALLOC failed\n");
		}
		goto error_exit;
	}
=======
		goto error_exit;
	}
	MALLOC(mdbp, HFSMasterDirectoryBlock *, kMDBSize, M_TEMP, M_WAITOK);
>>>>>>> origin/10.5
	bcopy((char *)buf_dataptr(bp) + HFS_PRI_OFFSET(phys_blksize), mdbp, kMDBSize);
	buf_brelse(bp);
	bp = NULL;

	MALLOC(hfsmp, struct hfsmount *, sizeof(struct hfsmount), M_HFSMNT, M_WAITOK);
	if (hfsmp == NULL) {
		if (HFS_MOUNT_DEBUG) { 
			printf("hfs_mountfs: MALLOC (2) failed\n");
		}
		retval = ENOMEM;
		goto error_exit;
	}
	bzero(hfsmp, sizeof(struct hfsmount));
	
	hfs_chashinit_finish(hfsmp);
	
<<<<<<< HEAD
	/* Init the ID lookup hashtable */
	hfs_idhash_init (hfsmp);

=======
>>>>>>> origin/10.6
	/*
	 * See if the disk supports unmap (trim).
	 *
	 * NOTE: vfs_init_io_attributes has not been called yet, so we can't use the io_flags field
	 * returned by vfs_ioattr.  We need to call VNOP_IOCTL ourselves.
	 */
	if (VNOP_IOCTL(devvp, DKIOCGETFEATURES, (caddr_t)&device_features, 0, context) == 0) {
		if (device_features & DK_FEATURE_UNMAP) {
			hfsmp->hfs_flags |= HFS_UNMAP;
		}
<<<<<<< HEAD

		if(device_features & DK_FEATURE_BARRIER)
			hfsmp->hfs_flags |= HFS_FEATURE_BARRIER;
	}

	/* 
	 * See if the disk is a solid state device, too.  We need this to decide what to do about 
	 * hotfiles.
	 */
	if (VNOP_IOCTL(devvp, DKIOCISSOLIDSTATE, (caddr_t)&isssd, 0, context) == 0) {
		if (isssd) {
			hfsmp->hfs_flags |= HFS_SSD;
		}
	}

	/* See if the underlying device is Core Storage or not */
	dk_corestorage_info_t cs_info;
	memset(&cs_info, 0, sizeof(dk_corestorage_info_t));
	if (VNOP_IOCTL(devvp, DKIOCCORESTORAGE, (caddr_t)&cs_info, 0, context) == 0) {
		hfsmp->hfs_flags |= HFS_CS;
		if (isroot && (cs_info.flags & DK_CORESTORAGE_PIN_YOUR_METADATA)) {
			hfsmp->hfs_flags |= HFS_CS_METADATA_PIN;
		}
		if (isroot && (cs_info.flags & DK_CORESTORAGE_ENABLE_HOTFILES)) {
			hfsmp->hfs_flags |= HFS_CS_HOTFILE_PIN;
			hfsmp->hfs_cs_hotfile_size = cs_info.hotfile_size;
		}
		if ((cs_info.flags & DK_CORESTORAGE_PIN_YOUR_SWAPFILE)) {
			hfsmp->hfs_flags |= HFS_CS_SWAPFILE_PIN;

			mp->mnt_ioflags |= MNT_IOFLAGS_SWAPPIN_SUPPORTED;
			mp->mnt_max_swappin_available = cs_info.swapfile_pinning;
		}
	}
=======
	/* Now switch to our prefered physical block size. */
	if (blksize > 512) {
		if (VOP_IOCTL(devvp, DKIOCSETBLOCKSIZE, (caddr_t)&blksize, FWRITE, cred, p)) {
			retval = ENXIO;
			goto error_exit;
		}
		/* Get the count of physical blocks. */
		if (VOP_IOCTL(devvp, DKIOCGETBLOCKCOUNT, (caddr_t)&blkcnt, 0, cred, p)) {
			retval = ENXIO;
			goto error_exit;
		}
	}

	/*
	 * At this point:
	 *   minblksize is the minimum physical block size
	 *   blksize has our prefered physical block size
	 *   blkcnt has the total number of physical blocks
	 */

	devvp->v_specsize = blksize;

	/* cache the IO attributes */
	if ((retval = vfs_init_io_attributes(devvp, mp))) {
		printf("hfs_mountfs: vfs_init_io_attributes returned %d\n",
			retval);
		return (retval);
	}

	mdb_offset = HFS_PRI_SECTOR(blksize);
	if ((retval = meta_bread(devvp, HFS_PRI_SECTOR(blksize), blksize, cred, &bp))) {
		goto error_exit;
	}
	mdbp = (HFSMasterDirectoryBlock*) (bp->b_data + HFS_PRI_OFFSET(blksize));

	MALLOC(hfsmp, struct hfsmount *, sizeof(struct hfsmount), M_HFSMNT, M_WAITOK);
	bzero(hfsmp, sizeof(struct hfsmount));
<<<<<<< HEAD
>>>>>>> origin/10.1

=======

	simple_lock_init(&hfsmp->hfs_renamelock);
	
>>>>>>> origin/10.2
=======
	}
	
>>>>>>> origin/10.6
	/*
	 *  Init the volume information structure
	 */
	
	lck_mtx_init(&hfsmp->hfs_mutex, hfs_mutex_group, hfs_lock_attr);
	lck_mtx_init(&hfsmp->hfc_mutex, hfs_mutex_group, hfs_lock_attr);
	lck_rw_init(&hfsmp->hfs_global_lock, hfs_rwlock_group, hfs_lock_attr);
	lck_spin_init(&hfsmp->vcbFreeExtLock, hfs_spinlock_group, hfs_lock_attr);
	
	vfs_setfsprivate(mp, hfsmp);
	hfsmp->hfs_mp = mp;			/* Make VFSTOHFS work */
	hfsmp->hfs_raw_dev = vnode_specrdev(devvp);
	hfsmp->hfs_devvp = devvp;
	vnode_ref(devvp);  /* Hold a ref on the device, dropped when hfsmp is freed. */
	hfsmp->hfs_logical_block_size = log_blksize;
	hfsmp->hfs_logical_block_count = log_blkcnt;
<<<<<<< HEAD
	hfsmp->hfs_logical_bytes = (uint64_t) log_blksize * (uint64_t) log_blkcnt;
=======
>>>>>>> origin/10.5
	hfsmp->hfs_physical_block_size = phys_blksize;
	hfsmp->hfs_log_per_phys = (phys_blksize / log_blksize);
	hfsmp->hfs_flags |= HFS_WRITEABLE_MEDIA;
	if (ronly)
		hfsmp->hfs_flags |= HFS_READ_ONLY;
	if (((unsigned int)vfs_flags(mp)) & MNT_UNKNOWNPERMISSIONS)
		hfsmp->hfs_flags |= HFS_UNKNOWN_PERMS;

#if QUOTA
	for (i = 0; i < MAXQUOTAS; i++)
		dqfileinit(&hfsmp->hfs_qfiles[i]);
#endif

<<<<<<< HEAD
=======
    /*
     *  Init the volume information structure
     */
    mp->mnt_data = (qaddr_t)hfsmp;
    hfsmp->hfs_mp = mp;						/* Make VFSTOHFS work */
    hfsmp->hfs_vcb.vcb_hfsmp = hfsmp;		/* Make VCBTOHFS work */
    hfsmp->hfs_raw_dev = devvp->v_rdev;
    hfsmp->hfs_devvp = devvp;
    hfsmp->hfs_phys_block_size = blksize;
    hfsmp->hfs_phys_block_count = blkcnt;
    hfsmp->hfs_fs_ronly = ronly;
    hfsmp->hfs_unknownpermissions = ((mp->mnt_flag & MNT_UNKNOWNPERMISSIONS) != 0);
>>>>>>> origin/10.1
	if (args) {
		hfsmp->hfs_uid = (args->hfs_uid == (uid_t)VNOVAL) ? UNKNOWNUID : args->hfs_uid;
		if (hfsmp->hfs_uid == 0xfffffffd) hfsmp->hfs_uid = UNKNOWNUID;
		hfsmp->hfs_gid = (args->hfs_gid == (gid_t)VNOVAL) ? UNKNOWNGID : args->hfs_gid;
		if (hfsmp->hfs_gid == 0xfffffffd) hfsmp->hfs_gid = UNKNOWNGID;
		vfs_setowner(mp, hfsmp->hfs_uid, hfsmp->hfs_gid);				/* tell the VFS */
		if (args->hfs_mask != (mode_t)VNOVAL) {
			hfsmp->hfs_dir_mask = args->hfs_mask & ALLPERMS;
			if (args->flags & HFSFSMNT_NOXONFILES) {
				hfsmp->hfs_file_mask = (args->hfs_mask & DEFFILEMODE);
			} else {
				hfsmp->hfs_file_mask = args->hfs_mask & ALLPERMS;
			}
		} else {
			hfsmp->hfs_dir_mask = UNKNOWNPERMISSIONS & ALLPERMS;		/* 0777: rwx---rwx */
			hfsmp->hfs_file_mask = UNKNOWNPERMISSIONS & DEFFILEMODE;	/* 0666: no --x by default? */
		}
		if ((args->flags != (int)VNOVAL) && (args->flags & HFSFSMNT_WRAPPER))
			mntwrapper = 1;
	} else {
		/* Even w/o explicit mount arguments, MNT_UNKNOWNPERMISSIONS requires setting up uid, gid, and mask: */
		if (((unsigned int)vfs_flags(mp)) & MNT_UNKNOWNPERMISSIONS) {
			hfsmp->hfs_uid = UNKNOWNUID;
			hfsmp->hfs_gid = UNKNOWNGID;
			vfs_setowner(mp, hfsmp->hfs_uid, hfsmp->hfs_gid);			/* tell the VFS */
			hfsmp->hfs_dir_mask = UNKNOWNPERMISSIONS & ALLPERMS;		/* 0777: rwx---rwx */
			hfsmp->hfs_file_mask = UNKNOWNPERMISSIONS & DEFFILEMODE;	/* 0666: no --x by default? */
<<<<<<< HEAD
		}
	}

	/* Find out if disk media is writable. */
	if (VNOP_IOCTL(devvp, DKIOCISWRITABLE, (caddr_t)&iswritable, 0, context) == 0) {
		if (iswritable)
			hfsmp->hfs_flags |= HFS_WRITEABLE_MEDIA;
		else
			hfsmp->hfs_flags &= ~HFS_WRITEABLE_MEDIA;
	}

	// Reservations
	rl_init(&hfsmp->hfs_reserved_ranges[0]);
	rl_init(&hfsmp->hfs_reserved_ranges[1]);

	// record the current time at which we're mounting this volume
	struct timeval tv;
	microtime(&tv);
	hfsmp->hfs_mount_time = tv.tv_sec;

	/* Mount a standard HFS disk */
	if ((SWAP_BE16(mdbp->drSigWord) == kHFSSigWord) &&
	    (mntwrapper || (SWAP_BE16(mdbp->drEmbedSigWord) != kHFSPlusSigWord))) {
#if CONFIG_HFS_STD 
		/* On 10.6 and beyond, non read-only mounts for HFS standard vols get rejected */
		if (vfs_isrdwr(mp)) {
			retval = EROFS;
			goto error_exit;
		}

		printf("hfs_mountfs: Mounting HFS Standard volumes was deprecated in Mac OS 10.7 \n");

		/* Treat it as if it's read-only and not writeable */
		hfsmp->hfs_flags |= HFS_READ_ONLY;
		hfsmp->hfs_flags &= ~HFS_WRITEABLE_MEDIA;

	   	/* If only journal replay is requested, exit immediately */
		if (journal_replay_only) {
			retval = 0;
			goto error_exit;
		}

	        if ((vfs_flags(mp) & MNT_ROOTFS)) {	
			retval = EINVAL;  /* Cannot root from HFS standard disks */
			goto error_exit;
		}
		/* HFS disks can only use 512 byte physical blocks */
		if (log_blksize > kHFSBlockSize) {
			log_blksize = kHFSBlockSize;
			if (VNOP_IOCTL(devvp, DKIOCSETBLOCKSIZE, (caddr_t)&log_blksize, FWRITE, context)) {
<<<<<<< HEAD
				retval = ENXIO;
				goto error_exit;
			}
			if (VNOP_IOCTL(devvp, DKIOCGETBLOCKCOUNT, (caddr_t)&log_blkcnt, 0, context)) {
				retval = ENXIO;
				goto error_exit;
			}
			hfsmp->hfs_logical_block_size = log_blksize;
			hfsmp->hfs_logical_block_count = log_blkcnt;
			hfsmp->hfs_logical_bytes = (uint64_t) log_blksize * (uint64_t) log_blkcnt;
			hfsmp->hfs_physical_block_size = log_blksize;
			hfsmp->hfs_log_per_phys = 1;
=======
		};
	};

	/* Mount a standard HFS disk */
	if ((SWAP_BE16(mdbp->drSigWord) == kHFSSigWord) &&
	    (SWAP_BE16(mdbp->drEmbedSigWord) != kHFSPlusSigWord)) {
		if (devvp == rootvp) {
			retval = EINVAL;  /* Cannot root from HFS standard disks */
			goto error_exit;
		}
		/* HFS disks can only use 512 byte physical blocks */
		if (blksize > kHFSBlockSize) {
			blksize = kHFSBlockSize;
			if (VOP_IOCTL(devvp, DKIOCSETBLOCKSIZE, (caddr_t)&blksize, FWRITE, cred, p)) {
				retval = ENXIO;
				goto error_exit;
			}
			if (VOP_IOCTL(devvp, DKIOCGETBLOCKCOUNT, (caddr_t)&blkcnt, 0, cred, p)) {
				retval = ENXIO;
				goto error_exit;
			}
			/* XXX do we need to call vfs_init_io_attributes again ? */
			devvp->v_specsize = blksize;
			hfsmp->hfs_phys_block_size = blksize;
			hfsmp->hfs_phys_block_count = blkcnt;
>>>>>>> origin/10.1
=======
				retval = ENXIO;
				goto error_exit;
			}
			if (VNOP_IOCTL(devvp, DKIOCGETBLOCKCOUNT, (caddr_t)&log_blkcnt, 0, context)) {
				retval = ENXIO;
				goto error_exit;
			}
			hfsmp->hfs_logical_block_size = log_blksize;
			hfsmp->hfs_logical_block_count = log_blkcnt;
			hfsmp->hfs_physical_block_size = log_blksize;
			hfsmp->hfs_log_per_phys = 1;
>>>>>>> origin/10.5
		}
		if (args) {
			hfsmp->hfs_encoding = args->hfs_encoding;
			HFSTOVCB(hfsmp)->volumeNameEncodingHint = args->hfs_encoding;

			/* establish the timezone */
			gTimeZone = args->hfs_timezone;
		}

<<<<<<< HEAD
		retval = hfs_getconverter(hfsmp->hfs_encoding, &hfsmp->hfs_get_unicode,
					&hfsmp->hfs_get_hfsname);
=======
		retval = hfs_getconverter(hfsmp->hfs_encoding, &hfsmp->hfs_get_unicode, &hfsmp->hfs_get_hfsname);
>>>>>>> origin/10.1
		if (retval)
			goto error_exit;

		retval = hfs_MountHFSVolume(hfsmp, mdbp, p);
		if (retval)
			(void) hfs_relconverter(hfsmp->hfs_encoding);
<<<<<<< HEAD
#else
		/* On platforms where HFS Standard is not supported, deny the mount altogether */
		retval = EINVAL;
=======

	} else /* Mount an HFS Plus disk */ {
		HFSPlusVolumeHeader *vhp;
		off_t embeddedOffset;
		int   jnl_disable = 0;
	
		/* Get the embedded Volume Header */
		if (SWAP_BE16(mdbp->drEmbedSigWord) == kHFSPlusSigWord) {
			embeddedOffset = SWAP_BE16(mdbp->drAlBlSt) * kHFSBlockSize;
			embeddedOffset += (u_int64_t)SWAP_BE16(mdbp->drEmbedExtent.startBlock) *
			                  (u_int64_t)SWAP_BE32(mdbp->drAlBlkSiz);

			disksize = (u_int64_t)SWAP_BE16(mdbp->drEmbedExtent.blockCount) *
			           (u_int64_t)SWAP_BE32(mdbp->drAlBlkSiz);

			hfsmp->hfs_phys_block_count = disksize / blksize;
	
			brelse(bp);
			bp = NULL;
			mdbp = NULL;

			/*
			 * If the embedded volume doesn't start on a block
			 * boundary, then switch the device to a 512-byte
			 * block size so everything will line up on a block
			 * boundary.
			 */
			if ((embeddedOffset % log_blksize) != 0) {
				printf("HFS Mount: embedded volume offset not"
				    " a multiple of physical block size (%d);"
<<<<<<< HEAD
				    " switching to 512\n", blksize);
				blksize = 512;
				if (VOP_IOCTL(devvp, DKIOCSETBLOCKSIZE,
				    (caddr_t)&blksize, FWRITE, cred, p)) {
					retval = ENXIO;
					goto error_exit;
				}
				if (VOP_IOCTL(devvp, DKIOCGETBLOCKCOUNT,
				    (caddr_t)&blkcnt, 0, cred, p)) {
=======
				    " switching to 512\n", log_blksize);
				log_blksize = 512;
				if (VNOP_IOCTL(devvp, DKIOCSETBLOCKSIZE,
				    (caddr_t)&log_blksize, FWRITE, context)) {
					retval = ENXIO;
					goto error_exit;
				}
				if (VNOP_IOCTL(devvp, DKIOCGETBLOCKCOUNT,
				    (caddr_t)&log_blkcnt, 0, context)) {
>>>>>>> origin/10.5
					retval = ENXIO;
					goto error_exit;
				}
				/* XXX do we need to call vfs_init_io_attributes again? */
				devvp->v_specsize = blksize;
				/* Note: relative block count adjustment */
				hfsmp->hfs_logical_block_count *=
				    hfsmp->hfs_logical_block_size / log_blksize;
				hfsmp->hfs_logical_block_size = log_blksize;
				
				/* Update logical/physical block size */
				hfsmp->hfs_physical_block_size = log_blksize;
				phys_blksize = log_blksize;
				hfsmp->hfs_log_per_phys = 1;
			}

<<<<<<< HEAD
			retval = meta_bread(devvp, (embeddedOffset / blksize) + HFS_PRI_SECTOR(blksize),
			               blksize, cred, &bp);
=======
			disksize = (u_int64_t)SWAP_BE16(mdbp->drEmbedExtent.blockCount) *
			           (u_int64_t)SWAP_BE32(mdbp->drAlBlkSiz);

			hfsmp->hfs_logical_block_count = disksize / log_blksize;
	
<<<<<<< HEAD
			mdb_offset = (embeddedOffset / blksize) + HFS_PRI_SECTOR(blksize);
			retval = meta_bread(devvp, mdb_offset, blksize, cred, &bp);
>>>>>>> origin/10.2
			if (retval)
				goto error_exit;
			vhp = (HFSPlusVolumeHeader*) (bp->b_data + HFS_PRI_OFFSET(blksize));
=======
			mdb_offset = (daddr64_t)((embeddedOffset / log_blksize) + HFS_PRI_SECTOR(log_blksize));
			retval = (int)buf_meta_bread(devvp, HFS_PHYSBLK_ROUNDDOWN(mdb_offset, hfsmp->hfs_log_per_phys),
					phys_blksize, cred, &bp);
			if (retval)
				goto error_exit;
			bcopy((char *)buf_dataptr(bp) + HFS_PRI_OFFSET(phys_blksize), mdbp, 512);
			buf_brelse(bp);
			bp = NULL;
			vhp = (HFSPlusVolumeHeader*) mdbp;
>>>>>>> origin/10.5

		} else /* pure HFS+ */ {
			embeddedOffset = 0;
			vhp = (HFSPlusVolumeHeader*) mdbp;
		}

		// XXXdbg
		//
		hfsmp->jnl = NULL;
		hfsmp->jvp = NULL;
		if (args != NULL && (args->flags & HFSFSMNT_EXTENDED_ARGS) && args->journal_disable) {
		    jnl_disable = 1;
		}
				
		//
		// We only initialize the journal here if the last person
		// to mount this volume was journaling aware.  Otherwise
		// we delay journal initialization until later at the end
		// of hfs_MountHFSPlusVolume() because the last person who
		// mounted it could have messed things up behind our back
		// (so we need to go find the .journal file, make sure it's
		// the right size, re-sync up if it was moved, etc).
		//
		if (   (SWAP_BE32(vhp->lastMountedVersion) == kHFSJMountVersion)
			&& (SWAP_BE32(vhp->attributes) & kHFSVolumeJournaledMask)
			&& !jnl_disable) {
			
			// if we're able to init the journal, mark the mount
			// point as journaled.
			//
			if (hfs_early_journal_init(hfsmp, vhp, args, embeddedOffset, mdb_offset, mdbp, cred) == 0) {
				mp->mnt_flag |= MNT_JOURNALED;
			} else {
<<<<<<< HEAD
				retval = EINVAL;
				goto error_exit;
=======
				// if the journal failed to open, then set the lastMountedVersion
				// to be "FSK!" which fsck_hfs will see and force the fsck instead
				// of just bailing out because the volume is journaled.
				if (!ronly) {
				    HFSPlusVolumeHeader *jvhp;

				    hfsmp->hfs_flags |= HFS_NEED_JNL_RESET;
				    
				    if (mdb_offset == 0) {
					mdb_offset = (daddr64_t)((embeddedOffset / log_blksize) + HFS_PRI_SECTOR(log_blksize));
				    }

				    bp = NULL;
				    retval = (int)buf_meta_bread(devvp, 
						    HFS_PHYSBLK_ROUNDDOWN(mdb_offset, hfsmp->hfs_log_per_phys), 
						    phys_blksize, cred, &bp);
				    if (retval == 0) {
					jvhp = (HFSPlusVolumeHeader *)(buf_dataptr(bp) + HFS_PRI_OFFSET(phys_blksize));
					    
					if (SWAP_BE16(jvhp->signature) == kHFSPlusSigWord || SWAP_BE16(jvhp->signature) == kHFSXSigWord) {
						printf ("hfs(1): Journal replay fail.  Writing lastMountVersion as FSK!\n");
					    jvhp->lastMountedVersion = SWAP_BE32(kFSKMountVersion);
					    buf_bwrite(bp);
					} else {
					    buf_brelse(bp);
					}
					bp = NULL;
				    } else if (bp) {
					buf_brelse(bp);
					// clear this so the error exit path won't try to use it
					bp = NULL;
				    }
				}

				// if this isn't the root device just bail out.
				// If it is the root device we just continue on
				// in the hopes that fsck_hfs will be able to
				// fix any damage that exists on the volume.
				if ( !(vfs_flags(mp) & MNT_ROOTFS)) {
				    retval = EINVAL;
				    goto error_exit;
				}
>>>>>>> origin/10.5
			}
		}
		// XXXdbg
	
		(void) hfs_getconverter(0, &hfsmp->hfs_get_unicode, &hfsmp->hfs_get_hfsname);

		retval = hfs_MountHFSPlusVolume(hfsmp, vhp, embeddedOffset, disksize, p, args);
		/*
		 * If the backend didn't like our physical blocksize
		 * then retry with physical blocksize of 512.
		 */
		if ((retval == ENXIO) && (log_blksize > 512) && (log_blksize != minblksize)) {
			printf("HFS Mount: could not use physical block size "
<<<<<<< HEAD
				"(%d) switching to 512\n", blksize);
			blksize = 512;
			if (VOP_IOCTL(devvp, DKIOCSETBLOCKSIZE, (caddr_t)&blksize, FWRITE, cred, p)) {
				retval = ENXIO;
				goto error_exit;
			}
			if (VOP_IOCTL(devvp, DKIOCGETBLOCKCOUNT, (caddr_t)&blkcnt, 0, cred, p)) {
				retval = ENXIO;
				goto error_exit;
			}
			/* XXX do we need to call vfs_init_io_attributes again ? */
			devvp->v_specsize = blksize;
=======
				"(%d) switching to 512\n", log_blksize);
			log_blksize = 512;
			if (VNOP_IOCTL(devvp, DKIOCSETBLOCKSIZE, (caddr_t)&log_blksize, FWRITE, context)) {
				retval = ENXIO;
				goto error_exit;
			}
			if (VNOP_IOCTL(devvp, DKIOCGETBLOCKCOUNT, (caddr_t)&log_blkcnt, 0, context)) {
				retval = ENXIO;
				goto error_exit;
			}
			devvp->v_specsize = log_blksize;
>>>>>>> origin/10.5
			/* Note: relative block count adjustment (in case this is an embedded volume). */
    			hfsmp->hfs_logical_block_count *= hfsmp->hfs_logical_block_size / log_blksize;
     			hfsmp->hfs_logical_block_size = log_blksize;
     			hfsmp->hfs_log_per_phys = hfsmp->hfs_physical_block_size / log_blksize;
 
<<<<<<< HEAD
=======
			if (hfsmp->jnl) {
			    // close and re-open this with the new block size
			    journal_close(hfsmp->jnl);
			    hfsmp->jnl = NULL;
			    if (hfs_early_journal_init(hfsmp, vhp, args, embeddedOffset, mdb_offset, mdbp, cred) == 0) {
					vfs_setflags(mp, (u_int64_t)((unsigned int)MNT_JOURNALED));
				} else {
					// if the journal failed to open, then set the lastMountedVersion
					// to be "FSK!" which fsck_hfs will see and force the fsck instead
					// of just bailing out because the volume is journaled.
					if (!ronly) {
				    	HFSPlusVolumeHeader *jvhp;

				    	hfsmp->hfs_flags |= HFS_NEED_JNL_RESET;
				    
				    	if (mdb_offset == 0) {
							mdb_offset = (daddr64_t)((embeddedOffset / log_blksize) + HFS_PRI_SECTOR(log_blksize));
				    	}

				   	 	bp = NULL;
				    	retval = (int)buf_meta_bread(devvp, HFS_PHYSBLK_ROUNDDOWN(mdb_offset, hfsmp->hfs_log_per_phys), 
							phys_blksize, cred, &bp);
				    	if (retval == 0) {
							jvhp = (HFSPlusVolumeHeader *)(buf_dataptr(bp) + HFS_PRI_OFFSET(phys_blksize));
					    
							if (SWAP_BE16(jvhp->signature) == kHFSPlusSigWord || SWAP_BE16(jvhp->signature) == kHFSXSigWord) {
								printf ("hfs(2): Journal replay fail.  Writing lastMountVersion as FSK!\n");
					    		jvhp->lastMountedVersion = SWAP_BE32(kFSKMountVersion);
					    		buf_bwrite(bp);
							} else {
					    		buf_brelse(bp);
							}
							bp = NULL;
				    	} else if (bp) {
							buf_brelse(bp);
							// clear this so the error exit path won't try to use it
							bp = NULL;
				    	}
					}

					// if this isn't the root device just bail out.
					// If it is the root device we just continue on
					// in the hopes that fsck_hfs will be able to
					// fix any damage that exists on the volume.
					if ( !(vfs_flags(mp) & MNT_ROOTFS)) {
				    	retval = EINVAL;
				    	goto error_exit;
					}
				}
			}

>>>>>>> origin/10.5
			/* Try again with a smaller block size... */
			retval = hfs_MountHFSPlusVolume(hfsmp, vhp, embeddedOffset, disksize, p, args);
		}
		if (retval)
			(void) hfs_relconverter(0);
	}

	if ( retval ) {
>>>>>>> origin/10.1
		goto error_exit;
#endif

<<<<<<< HEAD
	} 
	else { /* Mount an HFS Plus disk */
		HFSPlusVolumeHeader *vhp;
		off_t embeddedOffset;
		int   jnl_disable = 0;
	
		/* Get the embedded Volume Header */
		if (SWAP_BE16(mdbp->drEmbedSigWord) == kHFSPlusSigWord) {
			embeddedOffset = SWAP_BE16(mdbp->drAlBlSt) * kHFSBlockSize;
			embeddedOffset += (u_int64_t)SWAP_BE16(mdbp->drEmbedExtent.startBlock) *
			                  (u_int64_t)SWAP_BE32(mdbp->drAlBlkSiz);

			/* 
			 * Cooperative Fusion is not allowed on embedded HFS+ 
			 * filesystems (HFS+ inside HFS standard wrapper)
			 */
			hfsmp->hfs_flags &= ~HFS_CS_METADATA_PIN;

			/*
			 * If the embedded volume doesn't start on a block
			 * boundary, then switch the device to a 512-byte
			 * block size so everything will line up on a block
			 * boundary.
			 */
			if ((embeddedOffset % log_blksize) != 0) {
				printf("hfs_mountfs: embedded volume offset not"
				    " a multiple of physical block size (%d);"
				    " switching to 512\n", log_blksize);
				log_blksize = 512;
				if (VNOP_IOCTL(devvp, DKIOCSETBLOCKSIZE,
				    (caddr_t)&log_blksize, FWRITE, context)) {

					if (HFS_MOUNT_DEBUG) { 
						printf("hfs_mountfs: DKIOCSETBLOCKSIZE (3) failed\n");
					}				
					retval = ENXIO;
					goto error_exit;
				}
				if (VNOP_IOCTL(devvp, DKIOCGETBLOCKCOUNT,
				    (caddr_t)&log_blkcnt, 0, context)) {
					if (HFS_MOUNT_DEBUG) { 
						printf("hfs_mountfs: DKIOCGETBLOCKCOUNT (3) failed\n");
					}
					retval = ENXIO;
					goto error_exit;
				}
				/* Note: relative block count adjustment */
				hfsmp->hfs_logical_block_count *=
				    hfsmp->hfs_logical_block_size / log_blksize;
				
				/* Update logical /physical block size */
				hfsmp->hfs_logical_block_size = log_blksize;
				hfsmp->hfs_physical_block_size = log_blksize;
				
				phys_blksize = log_blksize;
				hfsmp->hfs_log_per_phys = 1;
			}

			disksize = (u_int64_t)SWAP_BE16(mdbp->drEmbedExtent.blockCount) *
			           (u_int64_t)SWAP_BE32(mdbp->drAlBlkSiz);

			hfsmp->hfs_logical_block_count = disksize / log_blksize;
	
			hfsmp->hfs_logical_bytes = (uint64_t) hfsmp->hfs_logical_block_count * (uint64_t) hfsmp->hfs_logical_block_size;
			
			mdb_offset = (daddr64_t)((embeddedOffset / log_blksize) + HFS_PRI_SECTOR(log_blksize));

			if (bp) {
				buf_markinvalid(bp);
				buf_brelse(bp);
				bp = NULL;
			}
			retval = (int)buf_meta_bread(devvp, HFS_PHYSBLK_ROUNDDOWN(mdb_offset, hfsmp->hfs_log_per_phys),
					phys_blksize, cred, &bp);
			if (retval) {
				if (HFS_MOUNT_DEBUG) { 
					printf("hfs_mountfs: buf_meta_bread (2) failed with %d\n", retval);
				}
				goto error_exit;
			}
			bcopy((char *)buf_dataptr(bp) + HFS_PRI_OFFSET(phys_blksize), mdbp, 512);
			buf_brelse(bp);
			bp = NULL;
			vhp = (HFSPlusVolumeHeader*) mdbp;

		} 
		else { /* pure HFS+ */ 
			embeddedOffset = 0;
			vhp = (HFSPlusVolumeHeader*) mdbp;
		}
<<<<<<< HEAD

		retval = hfs_ValidateHFSPlusVolumeHeader(hfsmp, vhp);
		if (retval)
			goto error_exit;
=======
		hfs_delete_chash(hfsmp);
		
		FREE(hfsmp, M_HFSMNT);
		vfs_setfsprivate(mp, NULL);
	}
        return (retval);
}
>>>>>>> origin/10.6

		/*
		 * If allocation block size is less than the physical block size,
		 * invalidate the buffer read in using native physical block size
		 * to ensure data consistency.
		 *
		 * HFS Plus reserves one allocation block for the Volume Header.
		 * If the physical size is larger, then when we read the volume header,
		 * we will also end up reading in the next allocation block(s).
		 * If those other allocation block(s) is/are modified, and then the volume
		 * header is modified, the write of the volume header's buffer will write
		 * out the old contents of the other allocation blocks.
		 *
		 * We assume that the physical block size is same as logical block size.
		 * The physical block size value is used to round down the offsets for
		 * reading and writing the primary and alternate volume headers.
		 *
		 * The same logic is also in hfs_MountHFSPlusVolume to ensure that
		 * hfs_mountfs, hfs_MountHFSPlusVolume and later are doing the I/Os
		 * using same block size.
		 */
		if (SWAP_BE32(vhp->blockSize) < hfsmp->hfs_physical_block_size) {
			phys_blksize = hfsmp->hfs_logical_block_size;
			hfsmp->hfs_physical_block_size = hfsmp->hfs_logical_block_size;
			hfsmp->hfs_log_per_phys = 1;
			// There should be one bp associated with devvp in buffer cache.
			retval = buf_invalidateblks(devvp, 0, 0, 0);
			if (retval)
				goto error_exit;
		}

		if (isroot) {
			hfs_root_unmounted_cleanly = ((SWAP_BE32(vhp->attributes) & kHFSVolumeUnmountedMask) != 0);
		}

		/*
		 * On inconsistent disks, do not allow read-write mount
		 * unless it is the boot volume being mounted.  We also
		 * always want to replay the journal if the journal_replay_only
		 * flag is set because that will (most likely) get the
		 * disk into a consistent state before fsck_hfs starts
		 * looking at it.
		 */
		if (  !(vfs_flags(mp) & MNT_ROOTFS)
		   && (SWAP_BE32(vhp->attributes) & kHFSVolumeInconsistentMask)
		   && !journal_replay_only
		   && !(hfsmp->hfs_flags & HFS_READ_ONLY)) {
			
			if (HFS_MOUNT_DEBUG) { 
				printf("hfs_mountfs: failed to mount non-root inconsistent disk\n");
			}
			retval = EINVAL;
			goto error_exit;
		}


		// XXXdbg
		//
		hfsmp->jnl = NULL;
		hfsmp->jvp = NULL;
		if (args != NULL && (args->flags & HFSFSMNT_EXTENDED_ARGS) && 
		    args->journal_disable) {
		    jnl_disable = 1;
		}
				
		//
		// We only initialize the journal here if the last person
		// to mount this volume was journaling aware.  Otherwise
		// we delay journal initialization until later at the end
		// of hfs_MountHFSPlusVolume() because the last person who
		// mounted it could have messed things up behind our back
		// (so we need to go find the .journal file, make sure it's
		// the right size, re-sync up if it was moved, etc).
		//
		if (   (SWAP_BE32(vhp->lastMountedVersion) == kHFSJMountVersion)
			&& (SWAP_BE32(vhp->attributes) & kHFSVolumeJournaledMask)
			&& !jnl_disable) {
			
			// if we're able to init the journal, mark the mount
			// point as journaled.
			//
			if ((retval = hfs_early_journal_init(hfsmp, vhp, args, embeddedOffset, mdb_offset, mdbp, cred)) == 0) {
				vfs_setflags(mp, (u_int64_t)((unsigned int)MNT_JOURNALED));
			} else {
				if (retval == EROFS) {
					// EROFS is a special error code that means the volume has an external
					// journal which we couldn't find.  in that case we do not want to
					// rewrite the volume header - we'll just refuse to mount the volume.
					if (HFS_MOUNT_DEBUG) { 
						printf("hfs_mountfs: hfs_early_journal_init indicated external jnl \n");
					}
					retval = EINVAL;
					goto error_exit;
				}

				// if the journal failed to open, then set the lastMountedVersion
				// to be "FSK!" which fsck_hfs will see and force the fsck instead
				// of just bailing out because the volume is journaled.
				if (!ronly) {
					if (HFS_MOUNT_DEBUG) { 
						printf("hfs_mountfs: hfs_early_journal_init failed, setting to FSK \n");
					}

					HFSPlusVolumeHeader *jvhp;

				    hfsmp->hfs_flags |= HFS_NEED_JNL_RESET;
				    
				    if (mdb_offset == 0) {
					mdb_offset = (daddr64_t)((embeddedOffset / log_blksize) + HFS_PRI_SECTOR(log_blksize));
				    }

				    bp = NULL;
				    retval = (int)buf_meta_bread(devvp, 
						    HFS_PHYSBLK_ROUNDDOWN(mdb_offset, hfsmp->hfs_log_per_phys), 
						    phys_blksize, cred, &bp);
				    if (retval == 0) {
					jvhp = (HFSPlusVolumeHeader *)(buf_dataptr(bp) + HFS_PRI_OFFSET(phys_blksize));
					    
					if (SWAP_BE16(jvhp->signature) == kHFSPlusSigWord || SWAP_BE16(jvhp->signature) == kHFSXSigWord) {
						printf ("hfs(1): Journal replay fail.  Writing lastMountVersion as FSK!\n");
					    jvhp->lastMountedVersion = SWAP_BE32(kFSKMountVersion);
					    buf_bwrite(bp);
					} else {
					    buf_brelse(bp);
					}
					bp = NULL;
				    } else if (bp) {
					buf_brelse(bp);
					// clear this so the error exit path won't try to use it
					bp = NULL;
				    }
				}

				// if this isn't the root device just bail out.
				// If it is the root device we just continue on
				// in the hopes that fsck_hfs will be able to
				// fix any damage that exists on the volume.
				if ( !(vfs_flags(mp) & MNT_ROOTFS)) {
					if (HFS_MOUNT_DEBUG) { 
						printf("hfs_mountfs: hfs_early_journal_init failed, erroring out \n");
					}
				    retval = EINVAL;
				    goto error_exit;
				}
			}
		}
		// XXXdbg
	
		/* Either the journal is replayed successfully, or there 
		 * was nothing to replay, or no journal exists.  In any case,
		 * return success.
		 */
		if (journal_replay_only) {
			retval = 0;
			goto error_exit;
		}

		(void) hfs_getconverter(0, &hfsmp->hfs_get_unicode, &hfsmp->hfs_get_hfsname);

		retval = hfs_MountHFSPlusVolume(hfsmp, vhp, embeddedOffset, disksize, p, args, cred);
		/*
		 * If the backend didn't like our physical blocksize
		 * then retry with physical blocksize of 512.
		 */
		if ((retval == ENXIO) && (log_blksize > 512) && (log_blksize != minblksize)) {
			printf("hfs_mountfs: could not use physical block size "
					"(%d) switching to 512\n", log_blksize);
			log_blksize = 512;
			if (VNOP_IOCTL(devvp, DKIOCSETBLOCKSIZE, (caddr_t)&log_blksize, FWRITE, context)) {
				if (HFS_MOUNT_DEBUG) { 
					printf("hfs_mountfs: DKIOCSETBLOCKSIZE (4) failed \n");
				}
				retval = ENXIO;
				goto error_exit;
			}
			if (VNOP_IOCTL(devvp, DKIOCGETBLOCKCOUNT, (caddr_t)&log_blkcnt, 0, context)) {
				if (HFS_MOUNT_DEBUG) { 
					printf("hfs_mountfs: DKIOCGETBLOCKCOUNT (4) failed \n");
				}
				retval = ENXIO;
				goto error_exit;
			}
			devvp->v_specsize = log_blksize;
			/* Note: relative block count adjustment (in case this is an embedded volume). */
			hfsmp->hfs_logical_block_count *= hfsmp->hfs_logical_block_size / log_blksize;
			hfsmp->hfs_logical_block_size = log_blksize;
			hfsmp->hfs_log_per_phys = hfsmp->hfs_physical_block_size / log_blksize;
	
			hfsmp->hfs_logical_bytes = (uint64_t) hfsmp->hfs_logical_block_count * (uint64_t) hfsmp->hfs_logical_block_size;

			if (hfsmp->jnl && hfsmp->jvp == devvp) {
			    // close and re-open this with the new block size
			    journal_close(hfsmp->jnl);
			    hfsmp->jnl = NULL;
			    if (hfs_early_journal_init(hfsmp, vhp, args, embeddedOffset, mdb_offset, mdbp, cred) == 0) {
					vfs_setflags(mp, (u_int64_t)((unsigned int)MNT_JOURNALED));
				} else {
					// if the journal failed to open, then set the lastMountedVersion
					// to be "FSK!" which fsck_hfs will see and force the fsck instead
					// of just bailing out because the volume is journaled.
					if (!ronly) {
						if (HFS_MOUNT_DEBUG) { 
							printf("hfs_mountfs: hfs_early_journal_init (2) resetting.. \n");
						}
				    	HFSPlusVolumeHeader *jvhp;

				    	hfsmp->hfs_flags |= HFS_NEED_JNL_RESET;
				    
				    	if (mdb_offset == 0) {
							mdb_offset = (daddr64_t)((embeddedOffset / log_blksize) + HFS_PRI_SECTOR(log_blksize));
				    	}

				   	 	bp = NULL;
				    	retval = (int)buf_meta_bread(devvp, HFS_PHYSBLK_ROUNDDOWN(mdb_offset, hfsmp->hfs_log_per_phys), 
							phys_blksize, cred, &bp);
				    	if (retval == 0) {
							jvhp = (HFSPlusVolumeHeader *)(buf_dataptr(bp) + HFS_PRI_OFFSET(phys_blksize));
					    
							if (SWAP_BE16(jvhp->signature) == kHFSPlusSigWord || SWAP_BE16(jvhp->signature) == kHFSXSigWord) {
								printf ("hfs(2): Journal replay fail.  Writing lastMountVersion as FSK!\n");
					    		jvhp->lastMountedVersion = SWAP_BE32(kFSKMountVersion);
					    		buf_bwrite(bp);
							} else {
					    		buf_brelse(bp);
							}
							bp = NULL;
				    	} else if (bp) {
							buf_brelse(bp);
							// clear this so the error exit path won't try to use it
							bp = NULL;
				    	}
					}

					// if this isn't the root device just bail out.
					// If it is the root device we just continue on
					// in the hopes that fsck_hfs will be able to
					// fix any damage that exists on the volume.
					if ( !(vfs_flags(mp) & MNT_ROOTFS)) {
						if (HFS_MOUNT_DEBUG) { 
							printf("hfs_mountfs: hfs_early_journal_init (2) failed \n");
						}
				    	retval = EINVAL;
				    	goto error_exit;
					}
				}
			}

			/* Try again with a smaller block size... */
			retval = hfs_MountHFSPlusVolume(hfsmp, vhp, embeddedOffset, disksize, p, args, cred);
			if (retval && HFS_MOUNT_DEBUG) {
				printf("hfs_MountHFSPlusVolume (late) returned %d\n",retval); 
			}
		}
		if (retval)
			(void) hfs_relconverter(0);
	}

	// save off a snapshot of the mtime from the previous mount
	// (for matador).
	hfsmp->hfs_last_mounted_mtime = hfsmp->hfs_mtime;

	if ( retval ) {
		if (HFS_MOUNT_DEBUG) { 
			printf("hfs_mountfs: encountered failure %d \n", retval);
		}
		goto error_exit;
	}

	mp->mnt_vfsstat.f_fsid.val[0] = dev;
	mp->mnt_vfsstat.f_fsid.val[1] = vfs_typenum(mp);
	vfs_setmaxsymlen(mp, 0);

	mp->mnt_vtable->vfc_vfsflags |= VFC_VFSNATIVEXATTR;
#if NAMEDSTREAMS
	mp->mnt_kern_flag |= MNTK_NAMED_STREAMS;
#endif
	if ((hfsmp->hfs_flags & HFS_STANDARD) == 0 ) {
		/* Tell VFS that we support directory hard links. */
		mp->mnt_vtable->vfc_vfsflags |= VFC_VFSDIRLINKS;
	} 
#if CONFIG_HFS_STD
	else {
		/* HFS standard doesn't support extended readdir! */
		mount_set_noreaddirext (mp);
	}
#endif

	if (args) {
		/*
		 * Set the free space warning levels for a non-root volume:
		 *
		 * Set the "danger" limit to 1% of the volume size or 100MB, whichever
		 * is less.  Set the "warning" limit to 2% of the volume size or 150MB,
		 * whichever is less.  And last, set the "desired" freespace level to
		 * to 3% of the volume size or 200MB, whichever is less.
		 */
		hfsmp->hfs_freespace_notify_dangerlimit =
			MIN(HFS_VERYLOWDISKTRIGGERLEVEL / HFSTOVCB(hfsmp)->blockSize,
				(HFSTOVCB(hfsmp)->totalBlocks / 100) * HFS_VERYLOWDISKTRIGGERFRACTION);
		hfsmp->hfs_freespace_notify_warninglimit =
			MIN(HFS_LOWDISKTRIGGERLEVEL / HFSTOVCB(hfsmp)->blockSize,
				(HFSTOVCB(hfsmp)->totalBlocks / 100) * HFS_LOWDISKTRIGGERFRACTION);
		hfsmp->hfs_freespace_notify_desiredlevel =
			MIN(HFS_LOWDISKSHUTOFFLEVEL / HFSTOVCB(hfsmp)->blockSize,
				(HFSTOVCB(hfsmp)->totalBlocks / 100) * HFS_LOWDISKSHUTOFFFRACTION);
	} else {
		/*
		 * Set the free space warning levels for the root volume:
		 *
		 * Set the "danger" limit to 5% of the volume size or 512MB, whichever
		 * is less.  Set the "warning" limit to 10% of the volume size or 1GB,
		 * whichever is less.  And last, set the "desired" freespace level to
		 * to 11% of the volume size or 1.25GB, whichever is less.
		 */
		hfsmp->hfs_freespace_notify_dangerlimit =
			MIN(HFS_ROOTVERYLOWDISKTRIGGERLEVEL / HFSTOVCB(hfsmp)->blockSize,
				(HFSTOVCB(hfsmp)->totalBlocks / 100) * HFS_ROOTVERYLOWDISKTRIGGERFRACTION);
		hfsmp->hfs_freespace_notify_warninglimit =
			MIN(HFS_ROOTLOWDISKTRIGGERLEVEL / HFSTOVCB(hfsmp)->blockSize,
				(HFSTOVCB(hfsmp)->totalBlocks / 100) * HFS_ROOTLOWDISKTRIGGERFRACTION);
		hfsmp->hfs_freespace_notify_desiredlevel =
			MIN(HFS_ROOTLOWDISKSHUTOFFLEVEL / HFSTOVCB(hfsmp)->blockSize,
				(HFSTOVCB(hfsmp)->totalBlocks / 100) * HFS_ROOTLOWDISKSHUTOFFFRACTION);
	};
	
	/* Check if the file system exists on virtual device, like disk image */
	if (VNOP_IOCTL(devvp, DKIOCISVIRTUAL, (caddr_t)&isvirtual, 0, context) == 0) {
		if (isvirtual) {
			hfsmp->hfs_flags |= HFS_VIRTUAL_DEVICE;
		}
	}

<<<<<<< HEAD
	/* do not allow ejectability checks on the root device */
=======
	/* ejectability checks will time out when the device is root_device, so skip them */
>>>>>>> origin/10.5
	if (isroot == 0) {
		if ((hfsmp->hfs_flags & HFS_VIRTUAL_DEVICE) == 0 && 
				IOBSDIsMediaEjectable(mp->mnt_vfsstat.f_mntfromname)) {
			hfsmp->hfs_syncer = thread_call_allocate(hfs_syncer, hfsmp);
			if (hfsmp->hfs_syncer == NULL) {
				printf("hfs: failed to allocate syncer thread callback for %s (%s)\n",
						mp->mnt_vfsstat.f_mntfromname, mp->mnt_vfsstat.f_mntonname);
			}
		}
	}

<<<<<<< HEAD
	printf("hfs: mounted %s on device %s\n", (hfsmp->vcbVN ? (const char*) hfsmp->vcbVN : "unknown"),
            (devvp->v_name ? devvp->v_name : (isroot ? "root_device": "unknown device")));

=======
>>>>>>> origin/10.5
	/*
	 * Start looking for free space to drop below this level and generate a
	 * warning immediately if needed:
	 */
	hfsmp->hfs_notification_conditions = 0;
	hfs_generate_volume_notifications(hfsmp);

	if (ronly == 0) {
		(void) hfs_flushvolumeheader(hfsmp, HFS_FVH_WAIT);
	}
	FREE(mdbp, M_TEMP);
	return (0);

error_exit:
	if (bp)
		buf_brelse(bp);
	if (mdbp)
		FREE(mdbp, M_TEMP);

	if (hfsmp && hfsmp->jvp && hfsmp->jvp != hfsmp->hfs_devvp) {
		vnode_clearmountedon(hfsmp->jvp);
		(void)VNOP_CLOSE(hfsmp->jvp, ronly ? FREAD : FREAD|FWRITE, vfs_context_kernel());
		hfsmp->jvp = NULL;
	}
	if (hfsmp) {
		if (hfsmp->hfs_devvp) {
			vnode_rele(hfsmp->hfs_devvp);
		}
		hfs_locks_destroy(hfsmp);
		hfs_delete_chash(hfsmp);
		hfs_idhash_destroy (hfsmp);

		FREE(hfsmp, M_HFSMNT);
		vfs_setfsprivate(mp, NULL);
=======
error_exit:
	if (bp)
		brelse(bp);
	if (mdbp)
		FREE(mdbp, M_TEMP);
	(void)VOP_CLOSE(devvp, ronly ? FREAD : FREAD|FWRITE, cred, p);
	if (hfsmp && hfsmp->jvp && hfsmp->jvp != hfsmp->hfs_devvp) {
	    (void)VOP_CLOSE(hfsmp->jvp, ronly ? FREAD : FREAD|FWRITE, cred, p);
		hfsmp->jvp = NULL;
	}
	if (hfsmp) {
		FREE(hfsmp, M_HFSMNT);
		mp->mnt_data = (qaddr_t)0;
>>>>>>> origin/10.2
	}
        return (retval);
}


/*
 * Make a filesystem operational.
 * Nothing to do at the moment.
 */
/* ARGSUSED */
static int
hfs_start(__unused struct mount *mp, __unused int flags, __unused vfs_context_t context)
{
	return (0);
}


/*
 * unmount system call
 */
int
hfs_unmount(struct mount *mp, int mntflags, vfs_context_t context)
{
	struct proc *p = vfs_context_proc(context);
	struct hfsmount *hfsmp = VFSTOHFS(mp);
	int retval = E_NONE;
	int flags;
	int force;
<<<<<<< HEAD
	int started_tr = 0;
=======
	int started_tr = 0, grabbed_lock = 0;
>>>>>>> origin/10.2

	flags = 0;
	force = 0;
	if (mntflags & MNT_FORCE) {
		flags |= FORCECLOSE;
		force = 1;
	}

	printf("hfs: unmount initiated on %s on device %s\n", 
			(hfsmp->vcbVN ? (const char*) hfsmp->vcbVN : "unknown"),
			(hfsmp->hfs_devvp ? ((hfsmp->hfs_devvp->v_name ? hfsmp->hfs_devvp->v_name : "unknown device")) : "unknown device"));

	if ((retval = hfs_flushfiles(mp, flags, p)) && !force)
 		return (retval);

	if (hfsmp->hfs_flags & HFS_METADATA_ZONE)
		(void) hfs_recording_suspend(hfsmp);
    
	hfs_syncer_free(hfsmp);
    
	if (hfsmp->hfs_flags & HFS_SUMMARY_TABLE) {
		if (hfsmp->hfs_summary_table) {
			int err = 0;
			/* 
		 	 * Take the bitmap lock to serialize against a concurrent bitmap scan still in progress 
			 */
			if (hfsmp->hfs_allocation_vp) {
				err = hfs_lock (VTOC(hfsmp->hfs_allocation_vp), HFS_EXCLUSIVE_LOCK, HFS_LOCK_DEFAULT);
			}
			FREE (hfsmp->hfs_summary_table, M_TEMP);
			hfsmp->hfs_summary_table = NULL;
			hfsmp->hfs_flags &= ~HFS_SUMMARY_TABLE;
			
			if (err == 0 && hfsmp->hfs_allocation_vp){
				hfs_unlock (VTOC(hfsmp->hfs_allocation_vp));
			}

		}
	}
	
	/*
	 * Cancel any pending timers for this volume.  Then wait for any timers
	 * which have fired, but whose callbacks have not yet completed.
	 */
	if (hfsmp->hfs_syncer)
	{
		struct timespec ts = {0, 100000000};	/* 0.1 seconds */
		
		/*
		 * Cancel any timers that have been scheduled, but have not
		 * fired yet.  NOTE: The kernel considers a timer complete as
		 * soon as it starts your callback, so the kernel does not
		 * keep track of the number of callbacks in progress.
		 */
		if (thread_call_cancel(hfsmp->hfs_syncer))
			OSDecrementAtomic((volatile SInt32 *)&hfsmp->hfs_sync_incomplete);
		thread_call_free(hfsmp->hfs_syncer);
		hfsmp->hfs_syncer = NULL;
		
		/*
		 * This waits for all of the callbacks that were entered before
		 * we did thread_call_cancel above, but have not completed yet.
		 */
		while(hfsmp->hfs_sync_incomplete > 0)
		{
			msleep((caddr_t)&hfsmp->hfs_sync_incomplete, NULL, PWAIT, "hfs_unmount", &ts);
		}
		
		if (hfsmp->hfs_sync_incomplete < 0)
			printf("hfs_unmount: pm_sync_incomplete underflow (%d)!\n", hfsmp->hfs_sync_incomplete);
	}
	
	/*
	 * Flush out the b-trees, volume bitmap and Volume Header
	 */
<<<<<<< HEAD
	if ((hfsmp->hfs_flags & HFS_READ_ONLY) == 0) {
		retval = hfs_start_transaction(hfsmp);
		if (retval == 0) {
		    started_tr = 1;
		} else if (!force) {
		    goto err_exit;
		}

		if (hfsmp->hfs_startup_vp) {
			(void) hfs_lock(VTOC(hfsmp->hfs_startup_vp), HFS_EXCLUSIVE_LOCK, HFS_LOCK_DEFAULT);
			retval = hfs_fsync(hfsmp->hfs_startup_vp, MNT_WAIT, 0, p);
			hfs_unlock(VTOC(hfsmp->hfs_startup_vp));
			if (retval && !force)
				goto err_exit;
		}

		if (hfsmp->hfs_attribute_vp) {
			(void) hfs_lock(VTOC(hfsmp->hfs_attribute_vp), HFS_EXCLUSIVE_LOCK, HFS_LOCK_DEFAULT);
			retval = hfs_fsync(hfsmp->hfs_attribute_vp, MNT_WAIT, 0, p);
			hfs_unlock(VTOC(hfsmp->hfs_attribute_vp));
			if (retval && !force)
=======
	if (hfsmp->hfs_fs_ronly == 0) {
		hfs_global_shared_lock_acquire(hfsmp);
		grabbed_lock = 1;
	    if (hfsmp->jnl) {
			journal_start_transaction(hfsmp->jnl);
			started_tr = 1;
		}
		
		retval = VOP_FSYNC(HFSTOVCB(hfsmp)->catalogRefNum, NOCRED, MNT_WAIT, p);
		if (retval && !force)
			goto err_exit;
		
		retval = VOP_FSYNC(HFSTOVCB(hfsmp)->extentsRefNum, NOCRED, MNT_WAIT, p);
		if (retval && !force)
			goto err_exit;
			
		// if we have an allocation file, sync it too so we don't leave dirty
		// blocks around
		if (HFSTOVCB(hfsmp)->allocationsRefNum) {
		    if (retval = VOP_FSYNC(HFSTOVCB(hfsmp)->allocationsRefNum, NOCRED, MNT_WAIT, p)) {
			if (!force)
			    goto err_exit;
		    }
		}

		if (retval = VOP_FSYNC(hfsmp->hfs_devvp, NOCRED, MNT_WAIT, p)) {
			if (!force)
>>>>>>> origin/10.2
				goto err_exit;
		}

		(void) hfs_lock(VTOC(hfsmp->hfs_catalog_vp), HFS_EXCLUSIVE_LOCK, HFS_LOCK_DEFAULT);
		retval = hfs_fsync(hfsmp->hfs_catalog_vp, MNT_WAIT, 0, p);
		hfs_unlock(VTOC(hfsmp->hfs_catalog_vp));
		if (retval && !force)
			goto err_exit;
		
		(void) hfs_lock(VTOC(hfsmp->hfs_extents_vp), HFS_EXCLUSIVE_LOCK, HFS_LOCK_DEFAULT);
		retval = hfs_fsync(hfsmp->hfs_extents_vp, MNT_WAIT, 0, p);
		hfs_unlock(VTOC(hfsmp->hfs_extents_vp));
		if (retval && !force)
			goto err_exit;
			
		if (hfsmp->hfs_allocation_vp) {
			(void) hfs_lock(VTOC(hfsmp->hfs_allocation_vp), HFS_EXCLUSIVE_LOCK, HFS_LOCK_DEFAULT);
			retval = hfs_fsync(hfsmp->hfs_allocation_vp, MNT_WAIT, 0, p);
			hfs_unlock(VTOC(hfsmp->hfs_allocation_vp));
			if (retval && !force)
				goto err_exit;
		}

		if (hfsmp->hfc_filevp && vnode_issystem(hfsmp->hfc_filevp)) {
			retval = hfs_fsync(hfsmp->hfc_filevp, MNT_WAIT, 0, p);
			if (retval && !force)
				goto err_exit;
		}

		/* If runtime corruption was detected, indicate that the volume
		 * was not unmounted cleanly.
		 */
		if (hfsmp->vcbAtrb & kHFSVolumeInconsistentMask) {
			HFSTOVCB(hfsmp)->vcbAtrb &= ~kHFSVolumeUnmountedMask;
		} else {
			HFSTOVCB(hfsmp)->vcbAtrb |= kHFSVolumeUnmountedMask;
		}

<<<<<<< HEAD
		if (hfsmp->hfs_flags & HFS_HAS_SPARSE_DEVICE) {
			int i;
			u_int32_t min_start = hfsmp->totalBlocks;
			
			// set the nextAllocation pointer to the smallest free block number
			// we've seen so on the next mount we won't rescan unnecessarily
			lck_spin_lock(&hfsmp->vcbFreeExtLock);
			for(i=0; i < (int)hfsmp->vcbFreeExtCnt; i++) {
				if (hfsmp->vcbFreeExt[i].startBlock < min_start) {
					min_start = hfsmp->vcbFreeExt[i].startBlock;
				}
			}
			lck_spin_unlock(&hfsmp->vcbFreeExtLock);
			if (min_start < hfsmp->nextAllocation) {
				hfsmp->nextAllocation = min_start;
			}
		}

		retval = hfs_flushvolumeheader(hfsmp, HFS_FVH_WAIT);
=======
		retval = hfs_flushvolumeheader(hfsmp, MNT_WAIT, 1);
>>>>>>> origin/10.2
		if (retval) {
			HFSTOVCB(hfsmp)->vcbAtrb &= ~kHFSVolumeUnmountedMask;
			if (!force)
				goto err_exit;	/* could not flush everything */
		}

<<<<<<< HEAD
		if (started_tr) {
		    hfs_end_transaction(hfsmp);
		    started_tr = 0;
=======
		if (hfsmp->jnl) {
			journal_end_transaction(hfsmp->jnl);
			started_tr = 0;
		}
		if (grabbed_lock) {
			hfs_global_shared_lock_release(hfsmp);
			grabbed_lock = 0;
>>>>>>> origin/10.2
		}
	}

	if (hfsmp->jnl) {
<<<<<<< HEAD
		hfs_flush(hfsmp, HFS_FLUSH_FULL);
=======
		journal_flush(hfsmp->jnl);
>>>>>>> origin/10.2
	}
	
	/*
	 *	Invalidate our caches and release metadata vnodes
	 */
	(void) hfsUnmount(hfsmp, p);

#if CONFIG_HFS_STD
	if (HFSTOVCB(hfsmp)->vcbSigWord == kHFSSigWord) {
		(void) hfs_relconverter(hfsmp->hfs_encoding);
	}
#endif

	// XXXdbg
	if (hfsmp->jnl) {
	    journal_close(hfsmp->jnl);
	    hfsmp->jnl = NULL;
	}

	VNOP_FSYNC(hfsmp->hfs_devvp, MNT_WAIT, context);

<<<<<<< HEAD
	if (hfsmp->jvp && hfsmp->jvp != hfsmp->hfs_devvp) {
	    vnode_clearmountedon(hfsmp->jvp);
	    retval = VNOP_CLOSE(hfsmp->jvp,
	                       hfsmp->hfs_flags & HFS_READ_ONLY ? FREAD : FREAD|FWRITE,
			       vfs_context_kernel());
	    vnode_put(hfsmp->jvp);
	    hfsmp->jvp = NULL;
	}
	// XXXdbg

	/*
	 * Last chance to dump unreferenced system files.
	 */
	(void) vflush(mp, NULLVP, FORCECLOSE);

#if HFS_SPARSE_DEV
	/* Drop our reference on the backing fs (if any). */
	if ((hfsmp->hfs_flags & HFS_HAS_SPARSE_DEVICE) && hfsmp->hfs_backingfs_rootvp) {
		struct vnode * tmpvp;

		hfsmp->hfs_flags &= ~HFS_HAS_SPARSE_DEVICE;
		tmpvp = hfsmp->hfs_backingfs_rootvp;
		hfsmp->hfs_backingfs_rootvp = NULLVP;
		vnode_rele(tmpvp);
	}
#endif /* HFS_SPARSE_DEV */

	vnode_rele(hfsmp->hfs_devvp);

	hfs_locks_destroy(hfsmp);
	hfs_delete_chash(hfsmp);
	hfs_idhash_destroy(hfsmp);

	assert(TAILQ_EMPTY(&hfsmp->hfs_reserved_ranges[HFS_TENTATIVE_BLOCKS])
		   && TAILQ_EMPTY(&hfsmp->hfs_reserved_ranges[HFS_LOCKED_BLOCKS]));
	assert(!hfsmp->lockedBlocks);
=======
	// XXXdbg
	if (hfsmp->jnl) {
	    journal_close(hfsmp->jnl);
	}

	if (hfsmp->jvp && hfsmp->jvp != hfsmp->hfs_devvp) {
	    retval = VOP_CLOSE(hfsmp->jvp, hfsmp->hfs_fs_ronly ? FREAD : FREAD|FWRITE,
			       NOCRED, p);
	    vrele(hfsmp->jvp);
		hfsmp->jvp = NULL;
	}
	// XXXdbg

	hfsmp->hfs_devvp->v_specflags &= ~SI_MOUNTEDON;
	retval = VOP_CLOSE(hfsmp->hfs_devvp,
		    hfsmp->hfs_fs_ronly ? FREAD : FREAD|FWRITE,
		    NOCRED, p);
	if (retval && !force)
		return(retval);
>>>>>>> origin/10.2

	FREE(hfsmp, M_HFSMNT);

	return (0);

  err_exit:
<<<<<<< HEAD
	if (started_tr) {
		hfs_end_transaction(hfsmp);
=======
	if (hfsmp->jnl && started_tr) {
		journal_end_transaction(hfsmp->jnl);
	}
	if (grabbed_lock) {
		hfs_global_shared_lock_release(hfsmp);
>>>>>>> origin/10.2
	}
	return retval;
}


/*
 * Return the root of a filesystem.
 */
static int
hfs_vfs_root(struct mount *mp, struct vnode **vpp, __unused vfs_context_t context)
{
	return hfs_vget(VFSTOHFS(mp), (cnid_t)kHFSRootFolderID, vpp, 1, 0);
}


/*
 * Do operations associated with quotas
 */
#if !QUOTA
static int
hfs_quotactl(__unused struct mount *mp, __unused int cmds, __unused uid_t uid, __unused caddr_t datap, __unused vfs_context_t context)
{
	return (ENOTSUP);
}
#else
static int
hfs_quotactl(struct mount *mp, int cmds, uid_t uid, caddr_t datap, vfs_context_t context)
{
	struct proc *p = vfs_context_proc(context);
	int cmd, type, error;

	if (uid == ~0U)
		uid = kauth_cred_getuid(vfs_context_ucred(context));
	cmd = cmds >> SUBCMDSHIFT;

	switch (cmd) {
	case Q_SYNC:
	case Q_QUOTASTAT:
		break;
	case Q_GETQUOTA:
		if (uid == kauth_cred_getuid(vfs_context_ucred(context)))
			break;
		/* fall through */
	default:
		if ( (error = vfs_context_suser(context)) )
			return (error);
	}

	type = cmds & SUBCMDMASK;
	if ((u_int)type >= MAXQUOTAS)
		return (EINVAL);
	if (vfs_busy(mp, LK_NOWAIT))
		return (0);

	switch (cmd) {

	case Q_QUOTAON:
		error = hfs_quotaon(p, mp, type, datap);
		break;

	case Q_QUOTAOFF:
		error = hfs_quotaoff(p, mp, type);
		break;

	case Q_SETQUOTA:
		error = hfs_setquota(mp, uid, type, datap);
		break;

	case Q_SETUSE:
		error = hfs_setuse(mp, uid, type, datap);
		break;

	case Q_GETQUOTA:
		error = hfs_getquota(mp, uid, type, datap);
		break;

	case Q_SYNC:
		error = hfs_qsync(mp);
		break;

	case Q_QUOTASTAT:
		error = hfs_quotastat(mp, type, datap);
		break;

	default:
		error = EINVAL;
		break;
	}
	vfs_unbusy(mp);

	return (error);
}
#endif /* QUOTA */

/* Subtype is composite of bits */
#define HFS_SUBTYPE_JOURNALED      0x01
#define HFS_SUBTYPE_CASESENSITIVE  0x02
/* bits 2 - 6 reserved */
#define HFS_SUBTYPE_STANDARDHFS    0x80



/*
 * Get file system statistics.
 */
int
hfs_statfs(struct mount *mp, register struct vfsstatfs *sbp, __unused vfs_context_t context)
{
	ExtendedVCB *vcb = VFSTOVCB(mp);
	struct hfsmount *hfsmp = VFSTOHFS(mp);
	u_int32_t freeCNIDs;
	u_int16_t subtype = 0;

	freeCNIDs = (u_int32_t)0xFFFFFFFF - (u_int32_t)vcb->vcbNxtCNID;

	sbp->f_bsize = (u_int32_t)vcb->blockSize;
	sbp->f_iosize = (size_t)cluster_max_io_size(mp, 0);
<<<<<<< HEAD
	sbp->f_blocks = (u_int64_t)((u_int32_t)vcb->totalBlocks);
	sbp->f_bfree = (u_int64_t)((u_int32_t )hfs_freeblks(hfsmp, 0));
	sbp->f_bavail = (u_int64_t)((u_int32_t )hfs_freeblks(hfsmp, 1));
	sbp->f_files = (u_int64_t)((u_int32_t )(vcb->totalBlocks - 2));  /* max files is constrained by total blocks */
	sbp->f_ffree = (u_int64_t)((u_int32_t )(MIN(freeCNIDs, sbp->f_bavail)));
=======
	sbp->f_blocks = (u_int64_t)((unsigned long)vcb->totalBlocks);
	sbp->f_bfree = (u_int64_t)((unsigned long )hfs_freeblks(hfsmp, 0));
	sbp->f_bavail = (u_int64_t)((unsigned long )hfs_freeblks(hfsmp, 1));
	sbp->f_files = (u_int64_t)((unsigned long )(vcb->totalBlocks - 2));  /* max files is constrained by total blocks */
	sbp->f_ffree = (u_int64_t)((unsigned long )(MIN(freeCNIDs, sbp->f_bavail)));
>>>>>>> origin/10.5

	/*
	 * Subtypes (flavors) for HFS
	 *   0:   Mac OS Extended
	 *   1:   Mac OS Extended (Journaled) 
	 *   2:   Mac OS Extended (Case Sensitive) 
	 *   3:   Mac OS Extended (Case Sensitive, Journaled) 
	 *   4 - 127:   Reserved
	 * 128:   Mac OS Standard
	 * 
	 */
	if ((hfsmp->hfs_flags & HFS_STANDARD) == 0) {
		/* HFS+ & variants */
		if (hfsmp->jnl) {
			subtype |= HFS_SUBTYPE_JOURNALED;
		}
		if (hfsmp->hfs_flags & HFS_CASE_SENSITIVE) {
			subtype |= HFS_SUBTYPE_CASESENSITIVE;
		}
	}
#if CONFIG_HFS_STD
	else {
		/* HFS standard */
		subtype = HFS_SUBTYPE_STANDARDHFS;
	} 
#endif
	sbp->f_fssubtype = subtype;

	return (0);
}


//
// XXXdbg -- this is a callback to be used by the journal to
//           get meta data blocks flushed out to disk.
//
// XXXdbg -- be smarter and don't flush *every* block on each
//           call.  try to only flush some so we don't wind up
//           being too synchronous.
//
__private_extern__
void
hfs_sync_metadata(void *arg)
<<<<<<< HEAD
=======
{
	struct mount *mp = (struct mount *)arg;
	struct cnode *cp;
	struct hfsmount *hfsmp;
	ExtendedVCB *vcb;
	struct vnode *meta_vp[3];
	struct buf *bp;
	int i, sectorsize, priIDSector, altIDSector, retval;
	int error, allerror = 0;

	hfsmp = VFSTOHFS(mp);
	vcb = HFSTOVCB(hfsmp);

	bflushq(BQ_META, mp);


#if 1     // XXXdbg - I do not believe this is necessary...
          //          but if I pull it out, then the journal
	      //          does not seem to get flushed properly
	      //          when it is closed....
	
	// now make sure the super block is flushed
	sectorsize = hfsmp->hfs_phys_block_size;
	priIDSector = (vcb->hfsPlusIOPosOffset / sectorsize) +
                  HFS_PRI_SECTOR(sectorsize);
	retval = meta_bread(hfsmp->hfs_devvp, priIDSector, sectorsize, NOCRED, &bp);
	if (retval != 0) {
		panic("hfs: sync_metadata: can't read super-block?! (retval 0x%x, priIDSector)\n",
			  retval, priIDSector);
	}

	if (retval == 0 && (bp->b_flags & B_DELWRI) && (bp->b_flags & B_LOCKED) == 0) {
	    bwrite(bp);
	} else if (bp) {
	    brelse(bp);
	}

	// the alternate super block...
	// XXXdbg - we probably don't need to do this each and every time.
	//          hfs_btreeio.c:FlushAlternate() should flag when it was
	//          written...
	altIDSector = (vcb->hfsPlusIOPosOffset / sectorsize) +
			HFS_ALT_SECTOR(sectorsize, hfsmp->hfs_phys_block_count);
	retval = meta_bread(hfsmp->hfs_devvp, altIDSector, sectorsize, NOCRED, &bp);
	if (retval == 0 && (bp->b_flags & B_DELWRI) && (bp->b_flags & B_LOCKED) == 0) {
	    bwrite(bp);
	} else if (bp) {
	    brelse(bp);
	}
#endif
	
}

/*
 * Go through the disk queues to initiate sandbagged IO;
 * go through the inodes to write those that have been modified;
 * initiate the writing of the super block if it has been modified.
 *
 * Note: we are always called with the filesystem marked `MPBUSY'.
 */
static int
hfs_sync(mp, waitfor, cred, p)
	struct mount *mp;
	int waitfor;
	struct ucred *cred;
	struct proc *p;
>>>>>>> origin/10.2
{
	struct mount *mp = (struct mount *)arg;
	struct hfsmount *hfsmp;
	ExtendedVCB *vcb;
	buf_t	bp;
	int  retval;
	daddr64_t priIDSector;
	hfsmp = VFSTOHFS(mp);
	vcb = HFSTOVCB(hfsmp);

<<<<<<< HEAD
	// now make sure the super block is flushed
	priIDSector = (daddr64_t)((vcb->hfsPlusIOPosOffset / hfsmp->hfs_logical_block_size) +
				  HFS_PRI_SECTOR(hfsmp->hfs_logical_block_size));

	retval = (int)buf_meta_bread(hfsmp->hfs_devvp, 
			HFS_PHYSBLK_ROUNDDOWN(priIDSector, hfsmp->hfs_log_per_phys),
			hfsmp->hfs_physical_block_size, NOCRED, &bp);
	if ((retval != 0 ) && (retval != ENXIO)) {
		printf("hfs_sync_metadata: can't read volume header at %d! (retval 0x%x)\n",
		       (int)priIDSector, retval);
	}

	if (retval == 0 && ((buf_flags(bp) & (B_DELWRI | B_LOCKED)) == B_DELWRI)) {
	    buf_bwrite(bp);
	} else if (bp) {
	    buf_brelse(bp);
	}
	
	/* Note that these I/Os bypass the journal (no calls to journal_start_modify_block) */

	// the alternate super block...
	// XXXdbg - we probably don't need to do this each and every time.
	//          hfs_btreeio.c:FlushAlternate() should flag when it was
	//          written...
<<<<<<< HEAD
	if (hfsmp->hfs_partition_avh_sector) {
		retval = (int)buf_meta_bread(hfsmp->hfs_devvp, 
				HFS_PHYSBLK_ROUNDDOWN(hfsmp->hfs_partition_avh_sector, hfsmp->hfs_log_per_phys),
				hfsmp->hfs_physical_block_size, NOCRED, &bp);
		if (retval == 0 && ((buf_flags(bp) & (B_DELWRI | B_LOCKED)) == B_DELWRI)) {
		    /* 
			 * note this I/O can fail if the partition shrank behind our backs! 
			 * So failure should be OK here.
			 */
			buf_bwrite(bp);
		} else if (bp) {
		    buf_brelse(bp);
=======
#if 0
	// XXXdbg first go through and flush out any modified
	//        meta data blocks so they go out in order...
	bflushq(BQ_META, mp);
	bflushq(BQ_LRU,  mp);
	// only flush locked blocks if we're not doing journaling
	if (hfsmp->jnl == NULL) {
	    bflushq(BQ_LOCKED, mp);
	}
#endif

	/*
	 * Write back each 'modified' vnode
	 */

loop:
	simple_lock(&mntvnode_slock);
	for (vp = mp->mnt_vnodelist.lh_first; vp != NULL; vp = nvp) {
		 int didhold;
		/*
		 * If the vnode that we are about to sync is no longer
		 * associated with this mount point, start over.
		 */
		if (vp->v_mount != mp) {
			simple_unlock(&mntvnode_slock);
			goto loop;
		}

		simple_lock(&vp->v_interlock);
		nvp = vp->v_mntvnodes.le_next;

		cp = VTOC(vp);

		// restart our whole search if this guy is locked
		// or being reclaimed.
		// XXXdbg - at some point this should go away or we
		//          need to change all file systems to have
		//          this same code.  vget() should never return
		//          success if either of these conditions is
		//          true.
		if (vp->v_tag != VT_HFS || cp == NULL) {
			simple_unlock(&vp->v_interlock);
			continue;
		}

		if ((vp->v_flag & VSYSTEM) || (vp->v_type == VNON) ||
		    (((cp->c_flag & (C_ACCESS | C_CHANGE | C_MODIFIED | C_UPDATE)) == 0) &&
		    (vp->v_dirtyblkhd.lh_first == NULL) && !(vp->v_flag & VHASDIRTY))) {
			simple_unlock(&vp->v_interlock);
			simple_unlock(&mntvnode_slock);
			simple_lock(&mntvnode_slock);
			continue;
>>>>>>> origin/10.2
		}
	}

<<<<<<< HEAD
	/* Is the FS's idea of the AVH different than the partition ? */
	if ((hfsmp->hfs_fs_avh_sector) && (hfsmp->hfs_partition_avh_sector != hfsmp->hfs_fs_avh_sector)) {
		retval = (int)buf_meta_bread(hfsmp->hfs_devvp, 
				HFS_PHYSBLK_ROUNDDOWN(hfsmp->hfs_fs_avh_sector, hfsmp->hfs_log_per_phys),
=======
	if (hfsmp->hfs_alt_id_sector) {
		retval = (int)buf_meta_bread(hfsmp->hfs_devvp, 
				HFS_PHYSBLK_ROUNDDOWN(hfsmp->hfs_alt_id_sector, hfsmp->hfs_log_per_phys),
>>>>>>> origin/10.5
				hfsmp->hfs_physical_block_size, NOCRED, &bp);
		if (retval == 0 && ((buf_flags(bp) & (B_DELWRI | B_LOCKED)) == B_DELWRI)) {
		    buf_bwrite(bp);
		} else if (bp) {
		    buf_brelse(bp);
=======
		simple_unlock(&mntvnode_slock);
		error = vget(vp, LK_EXCLUSIVE | LK_NOWAIT | LK_INTERLOCK, p);
		if (error) {
			if (error == ENOENT) {
				/*
				 * If vnode is being reclaimed, yield so
				 * that it can be removed from our list.
				 */
				if (UBCISVALID(vp))
					(void) tsleep((caddr_t)&lbolt, PINOD, "hfs_sync", 0);
				goto loop;
			}
			simple_lock(&mntvnode_slock);
			continue;
>>>>>>> origin/10.3
		}
	}

}


struct hfs_sync_cargs {
	kauth_cred_t  cred;
	struct proc	 *p;
	int			  waitfor;
	int			  error;
	int			  atime_only_syncs;
	time_t		  sync_start_time;
};


static int
hfs_sync_callback(struct vnode *vp, void *cargs)
{
	struct cnode *cp = VTOC(vp);
	struct hfs_sync_cargs *args;
	int error;

	args = (struct hfs_sync_cargs *)cargs;

	if (hfs_lock(cp, HFS_EXCLUSIVE_LOCK, HFS_LOCK_DEFAULT) != 0) {
		return (VNODE_RETURNED);
	}

	hfs_dirty_t dirty_state = hfs_is_dirty(cp);

	bool sync = dirty_state == HFS_DIRTY || vnode_hasdirtyblks(vp);

	if (!sync && dirty_state == HFS_DIRTY_ATIME
		&& args->atime_only_syncs < 256) {
		// We only update if the atime changed more than 60s ago
		if (args->sync_start_time - cp->c_attr.ca_atime > 60) {
			sync = true;
			++args->atime_only_syncs;
		}
	}

	if (sync) {
		error = hfs_fsync(vp, args->waitfor, 0, args->p);

		if (error)
		        args->error = error;
	} else if (cp->c_touch_acctime)
		hfs_touchtimes(VTOHFS(vp), cp);

	hfs_unlock(cp);
	return (VNODE_RETURNED);
}



/*
 * Go through the disk queues to initiate sandbagged IO;
 * go through the inodes to write those that have been modified;
 * initiate the writing of the super block if it has been modified.
 *
 * Note: we are always called with the filesystem marked `MPBUSY'.
 */
int
hfs_sync(struct mount *mp, int waitfor, vfs_context_t context)
{
	struct proc *p = vfs_context_proc(context);
	struct cnode *cp;
	struct hfsmount *hfsmp;
	ExtendedVCB *vcb;
	struct vnode *meta_vp[4];
	int i;
	int error, allerror = 0;
	struct hfs_sync_cargs args;

	hfsmp = VFSTOHFS(mp);
<<<<<<< HEAD
=======

	/*
	 * hfs_changefs might be manipulating vnodes so back off
	 */
	if (hfsmp->hfs_flags & HFS_IN_CHANGEFS)
		return (0);

	if (hfsmp->hfs_flags & HFS_READ_ONLY)
		return (EROFS);
>>>>>>> origin/10.5

	// Back off if hfs_changefs or a freeze is underway
	hfs_lock_mount(hfsmp);
	if ((hfsmp->hfs_flags & HFS_IN_CHANGEFS)
	    || hfsmp->hfs_freeze_state != HFS_THAWED) {
		hfs_unlock_mount(hfsmp);
		return 0;
	}

	if (hfsmp->hfs_flags & HFS_READ_ONLY) {
		hfs_unlock_mount(hfsmp);
		return (EROFS);
	}

	++hfsmp->hfs_syncers;
	hfs_unlock_mount(hfsmp);

	args.cred = kauth_cred_get();
	args.waitfor = waitfor;
	args.p = p;
	args.error = 0;
	args.atime_only_syncs = 0;

	struct timeval tv;
	microtime(&tv);

	args.sync_start_time = tv.tv_sec;

	/*
	 * hfs_sync_callback will be called for each vnode
	 * hung off of this mount point... the vnode will be
	 * properly referenced and unreferenced around the callback
	 */
	vnode_iterate(mp, 0, hfs_sync_callback, (void *)&args);

	if (args.error)
	        allerror = args.error;

	vcb = HFSTOVCB(hfsmp);

	meta_vp[0] = vcb->extentsRefNum;
	meta_vp[1] = vcb->catalogRefNum;
	meta_vp[2] = vcb->allocationsRefNum;  /* This is NULL for standard HFS */
	meta_vp[3] = hfsmp->hfs_attribute_vp; /* Optional file */

	/* Now sync our three metadata files */
	for (i = 0; i < 4; ++i) {
		struct vnode *btvp;

		btvp = meta_vp[i];;
		if ((btvp==0) || (vnode_mount(btvp) != mp))
			continue;

<<<<<<< HEAD
		/* XXX use hfs_systemfile_lock instead ? */
		(void) hfs_lock(VTOC(btvp), HFS_EXCLUSIVE_LOCK, HFS_LOCK_DEFAULT);
=======
		simple_lock(&btvp->v_interlock);
>>>>>>> origin/10.2
		cp = VTOC(btvp);

		if (!hfs_is_dirty(cp) && !vnode_hasdirtyblks(btvp)) {
			hfs_unlock(VTOC(btvp));
			continue;
		}
		error = vnode_get(btvp);
		if (error) {
			hfs_unlock(VTOC(btvp));
			continue;
		}
		if ((error = hfs_fsync(btvp, waitfor, 0, p)))
			allerror = error;

		hfs_unlock(cp);
		vnode_put(btvp);
	};


#if CONFIG_HFS_STD
	/*
	 * Force stale file system control information to be flushed.
	 */
	if (vcb->vcbSigWord == kHFSSigWord) {
		if ((error = VNOP_FSYNC(hfsmp->hfs_devvp, waitfor, context))) {
			allerror = error;
		}
	}
#endif

#if QUOTA
	hfs_qsync(mp);
#endif /* QUOTA */

	hfs_hotfilesync(hfsmp, vfs_context_kernel());

	/*
	 * Write back modified superblock.
	 */
	if (IsVCBDirty(vcb)) {
<<<<<<< HEAD
		error = hfs_flushvolumeheader(hfsmp, waitfor == MNT_WAIT ? HFS_FVH_WAIT : 0);
		if (error)
			allerror = error;
	}

	if (hfsmp->jnl) {
	    hfs_flush(hfsmp, HFS_FLUSH_JOURNAL);
	}

	hfs_lock_mount(hfsmp);
	boolean_t wake = (!--hfsmp->hfs_syncers
					  && hfsmp->hfs_freeze_state == HFS_WANT_TO_FREEZE);
	hfs_unlock_mount(hfsmp);
	if (wake)
		wakeup(&hfsmp->hfs_freeze_state);

=======
		// XXXdbg - debugging, remove
		if (hfsmp->jnl) {
			//printf("hfs: sync: strange, a journaled volume w/dirty VCB? jnl 0x%x hfsmp 0x%x\n",
			//	  hfsmp->jnl, hfsmp);
		}

		error = hfs_flushvolumeheader(hfsmp, waitfor, 0);
		if (error)
			allerror = error;
	}

	if (hfsmp->jnl) {
	    journal_flush(hfsmp->jnl);
	}
<<<<<<< HEAD
	
  err_exit:
>>>>>>> origin/10.2
=======

	{
		uint32_t secs, usecs;
		uint64_t now;

		clock_get_calendar_microtime(&secs, &usecs);
		now = ((uint64_t)secs * 1000000LL) + usecs;
		hfsmp->hfs_last_sync_time = now;
	}

	lck_rw_unlock_shared(&hfsmp->hfs_insync);	
>>>>>>> origin/10.5
	return (allerror);
}


/*
 * File handle to vnode
 *
 * Have to be really careful about stale file handles:
 * - check that the cnode id is valid
 * - call hfs_vget() to get the locked cnode
 * - check for an unallocated cnode (i_mode == 0)
 * - check that the given client host has export rights and return
 *   those rights via. exflagsp and credanonp
 */
static int
hfs_fhtovp(struct mount *mp, int fhlen, unsigned char *fhp, struct vnode **vpp, __unused vfs_context_t context)
{
	struct hfsfid *hfsfhp;
	struct vnode *nvp;
	int result;

	*vpp = NULL;
	hfsfhp = (struct hfsfid *)fhp;

	if (fhlen < (int)sizeof(struct hfsfid))
		return (EINVAL);

	result = hfs_vget(VFSTOHFS(mp), ntohl(hfsfhp->hfsfid_cnid), &nvp, 0, 0);
	if (result) {
		if (result == ENOENT)
			result = ESTALE;
		return result;
	}

	/* 
	 * We used to use the create time as the gen id of the file handle,
	 * but it is not static enough because it can change at any point 
	 * via system calls.  We still don't have another volume ID or other
	 * unique identifier to use for a generation ID across reboots that
	 * persists until the file is removed.  Using only the CNID exposes
	 * us to the potential wrap-around case, but as of 2/2008, it would take
	 * over 2 months to wrap around if the machine did nothing but allocate
	 * CNIDs.  Using some kind of wrap counter would only be effective if
	 * each file had the wrap counter associated with it.  For now, 
	 * we use only the CNID to identify the file as it's good enough.
	 */	 

	*vpp = nvp;

	hfs_unlock(VTOC(nvp));
	return (0);
}


/*
 * Vnode pointer to File handle
 */
/* ARGSUSED */
static int
hfs_vptofh(struct vnode *vp, int *fhlenp, unsigned char *fhp, __unused vfs_context_t context)
{
	struct cnode *cp;
	struct hfsfid *hfsfhp;

	if (ISHFS(VTOVCB(vp)))
		return (ENOTSUP);	/* hfs standard is not exportable */

	if (*fhlenp < (int)sizeof(struct hfsfid))
		return (EOVERFLOW);

	cp = VTOC(vp);
	hfsfhp = (struct hfsfid *)fhp;
	/* only the CNID is used to identify the file now */
	hfsfhp->hfsfid_cnid = htonl(cp->c_fileid);
	hfsfhp->hfsfid_gen = htonl(cp->c_fileid);
	*fhlenp = sizeof(struct hfsfid);
	
	return (0);
}


/*
 * Initialize HFS filesystems, done only once per boot.
 *
 * HFS is not a kext-based file system.  This makes it difficult to find 
 * out when the last HFS file system was unmounted and call hfs_uninit() 
 * to deallocate data structures allocated in hfs_init().  Therefore we 
 * never deallocate memory allocated by lock attribute and group initializations 
 * in this function.
 */
static int
hfs_init(__unused struct vfsconf *vfsp)
{
	static int done = 0;

	if (done)
		return (0);
	done = 1;
	hfs_chashinit();
	hfs_converterinit();

	BTReserveSetup();
	
	hfs_lock_attr    = lck_attr_alloc_init();
	hfs_group_attr   = lck_grp_attr_alloc_init();
	hfs_mutex_group  = lck_grp_alloc_init("hfs-mutex", hfs_group_attr);
	hfs_rwlock_group = lck_grp_alloc_init("hfs-rwlock", hfs_group_attr);
	hfs_spinlock_group = lck_grp_alloc_init("hfs-spinlock", hfs_group_attr);
	
#if HFS_COMPRESSION
	decmpfs_init();
#endif

	return (0);
}


// XXXdbg
#include <sys/filedesc.h>


/*
 * Destroy all locks, mutexes and spinlocks in hfsmp on unmount or failed mount
 */ 
static void 
hfs_locks_destroy(struct hfsmount *hfsmp)
{
<<<<<<< HEAD

	lck_mtx_destroy(&hfsmp->hfs_mutex, hfs_mutex_group);
	lck_mtx_destroy(&hfsmp->hfc_mutex, hfs_mutex_group);
	lck_rw_destroy(&hfsmp->hfs_global_lock, hfs_rwlock_group);
	lck_spin_destroy(&hfsmp->vcbFreeExtLock, hfs_spinlock_group);
=======
	extern u_int32_t hfs_encodingbias;

	/* all sysctl names at this level are terminal */

	if (name[0] == HFS_ENCODINGBIAS)
		return (sysctl_int(oldp, oldlenp, newp, newlen,
				&hfs_encodingbias));
	else if (name[0] == 0x082969) {
		// make the file system journaled...
		struct vnode *vp = p->p_fd->fd_cdir, *jvp;
		struct hfsmount *hfsmp;
		ExtendedVCB *vcb;
		int retval;
		struct cat_attr jnl_attr, jinfo_attr;
		struct cat_fork jnl_fork, jinfo_fork;
		void *jnl = NULL;

		/* Only root can enable journaling */
        if (current_proc()->p_ucred->cr_uid != 0) {
			return (EPERM);
		}
		hfsmp = VTOHFS(vp);
		if (hfsmp->hfs_fs_ronly) {
			return EROFS;
		}
		if (HFSTOVCB(hfsmp)->vcbSigWord == kHFSSigWord) {
			printf("hfs: can't make a plain hfs volume journaled.\n");
			return EINVAL;
		}

		if (hfsmp->jnl) {
		    printf("hfs: volume @ mp 0x%x is already journaled!\n", vp->v_mount);
		    return EAGAIN;
		}

		vcb = HFSTOVCB(hfsmp);
		if (BTHasContiguousNodes(VTOF(vcb->catalogRefNum)) == 0 ||
			BTHasContiguousNodes(VTOF(vcb->extentsRefNum)) == 0) {

			printf("hfs: volume has a btree w/non-contiguous nodes.  can not enable journaling.\n");
			return EINVAL;
		}

		// make sure these both exist!
		if (   GetFileInfo(vcb, kRootDirID, ".journal_info_block", &jinfo_attr, &jinfo_fork) == 0
			|| GetFileInfo(vcb, kRootDirID, ".journal", &jnl_attr, &jnl_fork) == 0) {

			return EINVAL;
		}

		hfs_sync(hfsmp->hfs_mp, MNT_WAIT, FSCRED, p);
		bflushq(BQ_META);

		printf("hfs: Initializing the journal (joffset 0x%llx sz 0x%llx)...\n",
			   (off_t)name[2], (off_t)name[3]);

		jvp = hfsmp->hfs_devvp;
		jnl = journal_create(jvp,
							 (off_t)name[2] * (off_t)HFSTOVCB(hfsmp)->blockSize
							 + HFSTOVCB(hfsmp)->hfsPlusIOPosOffset,
							 (off_t)name[3],
							 hfsmp->hfs_devvp,
							 hfsmp->hfs_logical_block_size,
							 0,
							 0,
							 hfs_sync_metadata, hfsmp->hfs_mp);

		if (jnl == NULL) {
			printf("hfs: FAILED to create the journal!\n");
			if (jvp && jvp != hfsmp->hfs_devvp) {
				VOP_CLOSE(jvp, hfsmp->hfs_fs_ronly ? FREAD : FREAD|FWRITE, FSCRED, p);
			}
			jvp = NULL;

			return EINVAL;
		} 

		hfs_global_exclusive_lock_acquire(hfsmp);
		
		HFSTOVCB(hfsmp)->vcbJinfoBlock = name[1];
		HFSTOVCB(hfsmp)->vcbAtrb |= kHFSVolumeJournaledMask;
		hfsmp->jvp = jvp;
		hfsmp->jnl = jnl;

		// save this off for the hack-y check in hfs_remove()
		hfsmp->jnl_start        = (u_int32_t)name[2];
		hfsmp->hfs_jnlinfoblkid = jinfo_attr.ca_fileid;
		hfsmp->hfs_jnlfileid    = jnl_attr.ca_fileid;

		hfsmp->hfs_mp->mnt_flag |= MNT_JOURNALED;

		hfs_global_exclusive_lock_release(hfsmp);
		hfs_flushvolumeheader(hfsmp, MNT_WAIT, 1);

		return 0;
	} else if (name[0] == 0x031272) {
		// clear the journaling bit 
		struct vnode *vp = p->p_fd->fd_cdir;
		struct hfsmount *hfsmp;
		void *jnl;
		int retval;
		
		/* Only root can disable journaling */
        if (current_proc()->p_ucred->cr_uid != 0) {
			return (EPERM);
		}
		hfsmp = VTOHFS(vp);
		if (hfsmp->jnl == NULL) {
			return EINVAL;
		}

		printf("hfs: disabling journaling for mount @ 0x%x\n", vp->v_mount);

		jnl = hfsmp->jnl;
		
		hfs_global_exclusive_lock_acquire(hfsmp);

		// Lights out for you buddy!
		hfsmp->jnl = NULL;
		journal_close(jnl);

		if (hfsmp->jvp && hfsmp->jvp != hfsmp->hfs_devvp) {
			VOP_CLOSE(hfsmp->jvp, hfsmp->hfs_fs_ronly ? FREAD : FREAD|FWRITE, FSCRED, p);
		}
		hfsmp->jnl = NULL;
		hfsmp->jvp = NULL;
		hfsmp->hfs_mp->mnt_flag &= ~MNT_JOURNALED;
		hfsmp->jnl_start        = 0;
		hfsmp->hfs_jnlinfoblkid = 0;
		hfsmp->hfs_jnlfileid    = 0;
		
		HFSTOVCB(hfsmp)->vcbAtrb &= ~kHFSVolumeJournaledMask;
		
		hfs_global_exclusive_lock_release(hfsmp);
		hfs_flushvolumeheader(hfsmp, MNT_WAIT, 1);

		return 0;
	}
>>>>>>> origin/10.2

	return;
}


static int
hfs_getmountpoint(struct vnode *vp, struct hfsmount **hfsmpp)
{
	struct hfsmount * hfsmp;
	char fstypename[MFSNAMELEN];

	if (vp == NULL)
		return (EINVAL);
	
	if (!vnode_isvroot(vp))
		return (EINVAL);

	vnode_vfsname(vp, fstypename);
	if (strncmp(fstypename, "hfs", sizeof(fstypename)) != 0)
		return (EINVAL);

	hfsmp = VTOHFS(vp);

	if (HFSTOVCB(hfsmp)->vcbSigWord == kHFSSigWord)
		return (EINVAL);

	*hfsmpp = hfsmp;

	return (0);
}

<<<<<<< HEAD
// XXXdbg
#include <sys/filedesc.h>

static hfsmount_t *hfs_mount_from_cwd(vfs_context_t ctx)
{
	vnode_t vp = vfs_context_cwd(ctx);

	if (!vp)
		return NULL;

	/*
	 * We could use vnode_tag, but it is probably more future proof to
	 * compare fstypename.
	 */
	char fstypename[MFSNAMELEN];
	vnode_vfsname(vp, fstypename);
=======
/* hfs_vfs_vget is not static since it is used in hfs_readwrite.c to support the
 * build_path ioctl.  We use it to leverage the code below that updates the origin
 * cache if necessary.
 */
int
hfs_vfs_vget(struct mount *mp, ino64_t ino, struct vnode **vpp, __unused vfs_context_t context)
{
	int error;
	int lockflags;
	struct hfsmount *hfsmp;

	hfsmp = VFSTOHFS(mp);

	error = hfs_vget(hfsmp, (cnid_t)ino, vpp, 1);
	if (error)
		return (error);

	/*
	 * ADLs may need to have their origin state updated
	 * since build_path needs a valid parent. The same is true
	 * for hardlinked files as well. There isn't a race window here in re-acquiring
	 * the cnode lock since we aren't pulling any data out of the cnode; instead, we're
	 * going back to the catalog.
	 */
	if ((VTOC(*vpp)->c_flag & C_HARDLINK) &&
	    (hfs_lock(VTOC(*vpp), HFS_EXCLUSIVE_LOCK) == 0)) {
		cnode_t *cp = VTOC(*vpp);
		struct cat_desc cdesc;
		
		if (!hfs_haslinkorigin(cp)) {
			lockflags = hfs_systemfile_lock(hfsmp, SFL_CATALOG, HFS_SHARED_LOCK);
		    error = cat_findname(hfsmp, (cnid_t)ino, &cdesc);
			hfs_systemfile_unlock(hfsmp, lockflags);
			if (error == 0) {
				if ((cdesc.cd_parentcnid !=
			    	hfsmp->hfs_private_desc[DIR_HARDLINKS].cd_cnid) && 
			   		(cdesc.cd_parentcnid != 
					hfsmp->hfs_private_desc[FILE_HARDLINKS].cd_cnid)) {
					hfs_savelinkorigin(cp, cdesc.cd_parentcnid);
				}
				cat_releasedesc(&cdesc);
			}
		}
		hfs_unlock(cp);
	}
	return (0);
}
>>>>>>> origin/10.5

	if (strcmp(fstypename, "hfs"))
		return NULL;

	return VTOHFS(vp);
}

/*
 * HFS filesystem related variables.
 */
int
hfs_sysctl(int *name, __unused u_int namelen, user_addr_t oldp, size_t *oldlenp, 
			user_addr_t newp, size_t newlen, vfs_context_t context)
{
	struct proc *p = vfs_context_proc(context);
	int error;
	struct hfsmount *hfsmp;

	/* all sysctl names at this level are terminal */

<<<<<<< HEAD
	if (name[0] == HFS_ENCODINGBIAS) {
		int bias;
=======
		lockflags = hfs_systemfile_lock(hfsmp, SFL_CATALOG, HFS_SHARED_LOCK);
		error = cat_idlookup(hfsmp, cnid, 0, 0, &cndesc, &cnattr, &cnfork);
		hfs_systemfile_unlock(hfsmp, lockflags);
>>>>>>> origin/10.8

		bias = hfs_getencodingbias();
		error = sysctl_int(oldp, oldlenp, newp, newlen, &bias);
		if (error == 0 && newp)
			hfs_setencodingbias(bias);
		return (error);

	} else if (name[0] == HFS_EXTEND_FS) {
		u_int64_t  newsize = 0;
		vnode_t vp = vfs_context_cwd(context);

		if (newp == USER_ADDR_NULL || vp == NULLVP)
			return (EINVAL);
		if ((error = hfs_getmountpoint(vp, &hfsmp)))
			return (error);

		/* Start with the 'size' set to the current number of bytes in the filesystem */
		newsize = ((uint64_t)hfsmp->totalBlocks) * ((uint64_t)hfsmp->blockSize);

		/* now get the new size from userland and over-write our stored value */
		error = sysctl_quad(oldp, oldlenp, newp, newlen, (quad_t *)&newsize);
		if (error)
			return (error);
	
		error = hfs_extendfs(hfsmp, newsize, context);		
		return (error);

	} else if (name[0] == HFS_ENCODINGHINT) {
		size_t bufsize;
		size_t bytes;
		u_int32_t hint;
		u_int16_t *unicode_name = NULL;
		char *filename = NULL;

<<<<<<< HEAD
		if ((newlen <= 0) || (newlen > MAXPATHLEN)) 
			return (EINVAL);
=======
		} else if ((pid == hfsmp->hfs_private_desc[FILE_HARDLINKS].cd_cnid) &&
		           (bcmp(nameptr, HFS_DELETE_PREFIX, HFS_DELETE_PREFIX_LEN) == 0)) {
			*vpp = NULL;
			cat_releasedesc(&cndesc);
			return (ENOENT);  /* open unlinked file */
		}
	}

	/*
	 * Finish initializing cnode descriptor for hardlinks.
	 *
	 * We need a valid name and parent for reverse lookups.
	 */
	if (linkref) {
		cnid_t nextlinkid;
		cnid_t prevlinkid;
		struct cat_desc linkdesc;
		int lockflags;

		cnattr.ca_linkref = linkref;

		/*
		 * Pick up the first link in the chain and get a descriptor for it.
		 * This allows blind volfs paths to work for hardlinks.
		 */
		if ((hfs_lookuplink(hfsmp, linkref, &prevlinkid,  &nextlinkid) == 0) &&
		    (nextlinkid != 0)) {
			lockflags = hfs_systemfile_lock(hfsmp, SFL_CATALOG, HFS_SHARED_LOCK);
			error = cat_findname(hfsmp, nextlinkid, &linkdesc);
			hfs_systemfile_unlock(hfsmp, lockflags);
			if (error == 0) {
				cat_releasedesc(&cndesc);
				bcopy(&linkdesc, &cndesc, sizeof(linkdesc));
			}
		}	
	}
>>>>>>> origin/10.5

		bufsize = MAX(newlen * 3, MAXPATHLEN);
		MALLOC(filename, char *, newlen, M_TEMP, M_WAITOK);
		if (filename == NULL) {
			error = ENOMEM;
			goto encodinghint_exit;
		}
<<<<<<< HEAD
		MALLOC(unicode_name, u_int16_t *, bufsize, M_TEMP, M_WAITOK);
		if (unicode_name == NULL) {
			error = ENOMEM;
			goto encodinghint_exit;
=======
	} else {
		struct componentname cn;

		/* Supply hfs_getnewvnode with a component name. */
		MALLOC_ZONE(cn.cn_pnbuf, caddr_t, MAXPATHLEN, M_NAMEI, M_WAITOK);
		cn.cn_nameiop = LOOKUP;
		cn.cn_flags = ISLASTCN | HASBUF;
		cn.cn_context = NULL;
		cn.cn_pnlen = MAXPATHLEN;
		cn.cn_nameptr = cn.cn_pnbuf;
		cn.cn_namelen = cndesc.cd_namelen;
		cn.cn_hash = 0;
		cn.cn_consume = 0;
		bcopy(cndesc.cd_nameptr, cn.cn_nameptr, cndesc.cd_namelen + 1);
	
		error = hfs_getnewvnode(hfsmp, NULLVP, &cn, &cndesc, 0, &cnattr, &cnfork, &vp);

		if ((error == 0) && (VTOC(vp)->c_flag & C_HARDLINK)) {
			hfs_savelinkorigin(VTOC(vp), cndesc.cd_parentcnid);
>>>>>>> origin/10.5
		}

		error = copyin(newp, (caddr_t)filename, newlen);
		if (error == 0) {
			error = utf8_decodestr((u_int8_t *)filename, newlen - 1, unicode_name,
			                       &bytes, bufsize, 0, UTF_DECOMPOSED);
			if (error == 0) {
				hint = hfs_pickencoding(unicode_name, bytes / 2);
				error = sysctl_int(oldp, oldlenp, USER_ADDR_NULL, 0, (int32_t *)&hint);
			}
		}

encodinghint_exit:
		if (unicode_name)
			FREE(unicode_name, M_TEMP);
		if (filename)
			FREE(filename, M_TEMP);
		return (error);

	} else if (name[0] == HFS_ENABLE_JOURNALING) {
		// make the file system journaled...
		vnode_t jvp;
		ExtendedVCB *vcb;
		struct cat_attr jnl_attr;
	    struct cat_attr	jinfo_attr;
		struct cat_fork jnl_fork;
		struct cat_fork jinfo_fork;
		buf_t jib_buf;
		uint64_t jib_blkno;
		uint32_t tmpblkno;
		uint64_t journal_byte_offset;
		uint64_t journal_size;
		vnode_t jib_vp = NULLVP;
		struct JournalInfoBlock local_jib;
		int err = 0;
		void *jnl = NULL;
		int lockflags;

		/* Only root can enable journaling */
		if (!kauth_cred_issuser(kauth_cred_get())) {
			return (EPERM);
		}

		hfsmp = hfs_mount_from_cwd(context);
		if (!hfsmp)
			return EINVAL;

		if (hfsmp->hfs_flags & HFS_READ_ONLY) {
			return EROFS;
		}
		if (HFSTOVCB(hfsmp)->vcbSigWord == kHFSSigWord) {
			printf("hfs: can't make a plain hfs volume journaled.\n");
			return EINVAL;
		}

		if (hfsmp->jnl) {
		    printf("hfs: volume %s is already journaled!\n", hfsmp->vcbVN);
		    return EAGAIN;
		}
		vcb = HFSTOVCB(hfsmp);

		/* Set up local copies of the initialization info */
		tmpblkno = (uint32_t) name[1];
		jib_blkno = (uint64_t) tmpblkno;
		journal_byte_offset = (uint64_t) name[2];
		journal_byte_offset *= hfsmp->blockSize;
		journal_byte_offset += hfsmp->hfsPlusIOPosOffset;
		journal_size = (uint64_t)((unsigned)name[3]);

		lockflags = hfs_systemfile_lock(hfsmp, SFL_CATALOG | SFL_EXTENTS, HFS_EXCLUSIVE_LOCK);
		if (BTHasContiguousNodes(VTOF(vcb->catalogRefNum)) == 0 ||
			BTHasContiguousNodes(VTOF(vcb->extentsRefNum)) == 0) {

			printf("hfs: volume has a btree w/non-contiguous nodes.  can not enable journaling.\n");
			hfs_systemfile_unlock(hfsmp, lockflags);
			return EINVAL;
		}
		hfs_systemfile_unlock(hfsmp, lockflags);

		// make sure these both exist!
		if (   GetFileInfo(vcb, kHFSRootFolderID, ".journal_info_block", &jinfo_attr, &jinfo_fork) == 0
			|| GetFileInfo(vcb, kHFSRootFolderID, ".journal", &jnl_attr, &jnl_fork) == 0) {

			return EINVAL;
		}

<<<<<<< HEAD
		/*
		 * At this point, we have a copy of the metadata that lives in the catalog for the
		 * journal info block.  Compare that the journal info block's single extent matches
		 * that which was passed into this sysctl.  
		 *
		 * If it is different, deny the journal enable call.
		 */
		if (jinfo_fork.cf_blocks > 1) {
			/* too many blocks */
			return EINVAL;
		}
=======
	switch (op) {
	case VOL_UPDATE:
		break;
	case VOL_MKDIR:
		if (vcb->vcbDirCnt != 0xFFFFFFFF)
			++vcb->vcbDirCnt;
		if (inroot && vcb->vcbNmRtDirs != 0xFFFF)
			++vcb->vcbNmRtDirs;
		break;
	case VOL_RMDIR:
		if (vcb->vcbDirCnt != 0)
			--vcb->vcbDirCnt;
		if (inroot && vcb->vcbNmRtDirs != 0xFFFF)
			--vcb->vcbNmRtDirs;
		break;
	case VOL_MKFILE:
		if (vcb->vcbFilCnt != 0xFFFFFFFF)
			++vcb->vcbFilCnt;
		if (inroot && vcb->vcbNmFls != 0xFFFF)
			++vcb->vcbNmFls;
		break;
	case VOL_RMFILE:
		if (vcb->vcbFilCnt != 0)
			--vcb->vcbFilCnt;
		if (inroot && vcb->vcbNmFls != 0xFFFF)
			--vcb->vcbNmFls;
		break;
	}

	if (hfsmp->jnl) {
		hfs_flushvolumeheader(hfsmp, 0, 0);
	}

	return (0);
}
>>>>>>> origin/10.2

		if (jinfo_fork.cf_extents[0].startBlock != jib_blkno) {
			/* Wrong block */
			return EINVAL;
		}

		/*   
		 * We want to immediately purge the vnode for the JIB.
		 * 
		 * Because it was written to from userland, there's probably 
		 * a vnode somewhere in the vnode cache (possibly with UBC backed blocks). 
		 * So we bring the vnode into core, then immediately do whatever 
		 * we can to flush/vclean it out.  This is because those blocks will be 
		 * interpreted as user data, which may be treated separately on some platforms
		 * than metadata.  If the vnode is gone, then there cannot be backing blocks
		 * in the UBC.
		 */
		if (hfs_vget (hfsmp, jinfo_attr.ca_fileid, &jib_vp, 1, 0)) {
			return EINVAL;
		} 
		/*
		 * Now we have a vnode for the JIB. recycle it. Because we hold an iocount
		 * on the vnode, we'll just mark it for termination when the last iocount
		 * (hopefully ours), is dropped.
		 */
		vnode_recycle (jib_vp);
		err = vnode_put (jib_vp);
		if (err) {
			return EINVAL;	
		}

<<<<<<< HEAD
<<<<<<< HEAD
		/* Initialize the local copy of the JIB (just like hfs.util) */
		memset (&local_jib, 'Z', sizeof(struct JournalInfoBlock));
		local_jib.flags = SWAP_BE32(kJIJournalInFSMask);
		/* Note that the JIB's offset is in bytes */
		local_jib.offset = SWAP_BE64(journal_byte_offset);
		local_jib.size = SWAP_BE64(journal_size);  

		/* 
		 * Now write out the local JIB.  This essentially overwrites the userland
		 * copy of the JIB.  Read it as BLK_META to treat it as a metadata read/write.
		 */
		jib_buf = buf_getblk (hfsmp->hfs_devvp, 
				jib_blkno * (hfsmp->blockSize / hfsmp->hfs_logical_block_size), 
				hfsmp->blockSize, 0, 0, BLK_META);
		char* buf_ptr = (char*) buf_dataptr (jib_buf);
=======
	sectorsize = hfsmp->hfs_phys_block_size;
	retval = bread(hfsmp->hfs_devvp, HFS_PRI_SECTOR(sectorsize), sectorsize, NOCRED, &bp);
=======
	sectorsize = hfsmp->hfs_logical_block_size;
	retval = (int)buf_bread(hfsmp->hfs_devvp, (daddr64_t)HFS_PRI_SECTOR(sectorsize), sectorsize, NOCRED, &bp);
>>>>>>> origin/10.5
	if (retval) {
		if (bp)
			brelse(bp);
		return retval;
	}
>>>>>>> origin/10.2

		/* Zero out the portion of the block that won't contain JIB data */
		memset (buf_ptr, 0, hfsmp->blockSize);

<<<<<<< HEAD
		bcopy(&local_jib, buf_ptr, sizeof(local_jib));
		if (buf_bwrite (jib_buf)) {
			return EIO;
		}
=======
	if (hfsmp->jnl) {
		panic("hfs: standard hfs volumes should not be journaled!\n");
	}

	mdb = (HFSMasterDirectoryBlock *)(bp->b_data + HFS_PRI_OFFSET(sectorsize));
    
	mdb->drCrDate	= SWAP_BE32 (UTCToLocal(to_hfs_time(vcb->vcbCrDate)));
	mdb->drLsMod	= SWAP_BE32 (UTCToLocal(to_hfs_time(vcb->vcbLsMod)));
	mdb->drAtrb	= SWAP_BE16 (vcb->vcbAtrb);
	mdb->drNmFls	= SWAP_BE16 (vcb->vcbNmFls);
	mdb->drAllocPtr	= SWAP_BE16 (vcb->nextAllocation);
	mdb->drClpSiz	= SWAP_BE32 (vcb->vcbClpSiz);
	mdb->drNxtCNID	= SWAP_BE32 (vcb->vcbNxtCNID);
	mdb->drFreeBks	= SWAP_BE16 (vcb->freeBlocks);
>>>>>>> origin/10.2

		/* Force a flush track cache */
		hfs_flush(hfsmp, HFS_FLUSH_CACHE);

		/* Now proceed with full volume sync */
		hfs_sync(hfsmp->hfs_mp, MNT_WAIT, context);

		printf("hfs: Initializing the journal (joffset 0x%llx sz 0x%llx)...\n",
			   (off_t)name[2], (off_t)name[3]);

		//
		// XXXdbg - note that currently (Sept, 08) hfs_util does not support
		//          enabling the journal on a separate device so it is safe
		//          to just copy hfs_devvp here.  If hfs_util gets the ability
		//          to dynamically enable the journal on a separate device then
		//          we will have to do the same thing as hfs_early_journal_init()
		//          to locate and open the journal device.
		//
		jvp = hfsmp->hfs_devvp;
		jnl = journal_create(jvp, journal_byte_offset, journal_size, 
							 hfsmp->hfs_devvp,
							 hfsmp->hfs_logical_block_size,
							 0,
							 0,
							 hfs_sync_metadata, hfsmp->hfs_mp,
							 hfsmp->hfs_mp);

		/*
		 * Set up the trim callback function so that we can add
		 * recently freed extents to the free extent cache once
		 * the transaction that freed them is written to the
		 * journal on disk.
		 */
		if (jnl)
			journal_trim_set_callback(jnl, hfs_trim_callback, hfsmp);

		if (jnl == NULL) {
			printf("hfs: FAILED to create the journal!\n");
			if (jvp && jvp != hfsmp->hfs_devvp) {
				vnode_clearmountedon(jvp);
				VNOP_CLOSE(jvp, hfsmp->hfs_flags & HFS_READ_ONLY ? FREAD : FREAD|FWRITE, vfs_context_kernel());
			}
			jvp = NULL;

			return EINVAL;
		} 

		hfs_lock_global (hfsmp, HFS_EXCLUSIVE_LOCK);

		/*
		 * Flush all dirty metadata buffers.
		 */
		buf_flushdirtyblks(hfsmp->hfs_devvp, TRUE, 0, "hfs_sysctl");
		buf_flushdirtyblks(hfsmp->hfs_extents_vp, TRUE, 0, "hfs_sysctl");
		buf_flushdirtyblks(hfsmp->hfs_catalog_vp, TRUE, 0, "hfs_sysctl");
		buf_flushdirtyblks(hfsmp->hfs_allocation_vp, TRUE, 0, "hfs_sysctl");
		if (hfsmp->hfs_attribute_vp)
			buf_flushdirtyblks(hfsmp->hfs_attribute_vp, TRUE, 0, "hfs_sysctl");

<<<<<<< HEAD
		HFSTOVCB(hfsmp)->vcbJinfoBlock = name[1];
		HFSTOVCB(hfsmp)->vcbAtrb |= kHFSVolumeJournaledMask;
		hfsmp->jvp = jvp;
		hfsmp->jnl = jnl;

		// save this off for the hack-y check in hfs_remove()
		hfsmp->jnl_start        = (u_int32_t)name[2];
		hfsmp->jnl_size         = (off_t)((unsigned)name[3]);
		hfsmp->hfs_jnlinfoblkid = jinfo_attr.ca_fileid;
		hfsmp->hfs_jnlfileid    = jnl_attr.ca_fileid;
=======
/*
 *  Flush any dirty in-memory mount data to the on-disk
 *  volume header.
 *
 *  Note: the on-disk volume signature is intentionally
 *  not flushed since the on-disk "H+" and "HX" signatures
 *  are always stored in-memory as "H+".
 */
__private_extern__
int
hfs_flushvolumeheader(struct hfsmount *hfsmp, int waitfor, int altflush)
{
	ExtendedVCB *vcb = HFSTOVCB(hfsmp);
	struct filefork *fp;
	HFSPlusVolumeHeader *volumeHeader;
	int retval;
	struct buf *bp;
	int i;
	daddr64_t priIDSector;
	int critical;
	u_int16_t  signature;
	u_int16_t  hfsversion;

	if (hfsmp->hfs_flags & HFS_READ_ONLY) {
		return(0);
	}
	if (hfsmp->hfs_flags & HFS_STANDARD) {
		return hfs_flushMDB(hfsmp, waitfor, altflush);
	}
	critical = altflush;
	priIDSector = (daddr64_t)((vcb->hfsPlusIOPosOffset / hfsmp->hfs_logical_block_size) +
				  HFS_PRI_SECTOR(hfsmp->hfs_logical_block_size));
>>>>>>> origin/10.5

		vfs_setflags(hfsmp->hfs_mp, (u_int64_t)((unsigned int)MNT_JOURNALED));

<<<<<<< HEAD
		hfs_unlock_global (hfsmp);
		hfs_flushvolumeheader(hfsmp, HFS_FVH_WAIT | HFS_FVH_WRITE_ALT);
=======
	retval = (int)buf_meta_bread(hfsmp->hfs_devvp, 
			HFS_PHYSBLK_ROUNDDOWN(priIDSector, hfsmp->hfs_log_per_phys),
			hfsmp->hfs_physical_block_size, NOCRED, &bp);
	if (retval) {
		if (bp)
			buf_brelse(bp);
>>>>>>> origin/10.5

		{
			fsid_t fsid;
		
			fsid.val[0] = (int32_t)hfsmp->hfs_raw_dev;
			fsid.val[1] = (int32_t)vfs_typenum(HFSTOVFS(hfsmp));
			vfs_event_signal(&fsid, VQ_UPDATE, (intptr_t)NULL);
		}
		return 0;
	} else if (name[0] == HFS_DISABLE_JOURNALING) {
		// clear the journaling bit 

		/* Only root can disable journaling */
		if (!kauth_cred_issuser(kauth_cred_get())) {
			return (EPERM);
		}

		hfsmp = hfs_mount_from_cwd(context);
		if (!hfsmp)
			return EINVAL;

<<<<<<< HEAD
		/* 
		 * Disabling journaling is disallowed on volumes with directory hard links
		 * because we have not tested the relevant code path.
		 */  
		if (hfsmp->hfs_private_attr[DIR_HARDLINKS].ca_entries != 0){
			printf("hfs: cannot disable journaling on volumes with directory hardlinks\n");
			return EPERM;
		}
=======
	volumeHeader = (HFSPlusVolumeHeader *)((char *)buf_dataptr(bp) + 
			HFS_PRI_OFFSET(hfsmp->hfs_physical_block_size));
>>>>>>> origin/10.5

		printf("hfs: disabling journaling for %s\n", hfsmp->vcbVN);

		hfs_lock_global (hfsmp, HFS_EXCLUSIVE_LOCK);

<<<<<<< HEAD
		// Lights out for you buddy!
		journal_close(hfsmp->jnl);
		hfsmp->jnl = NULL;
=======
		retval = (int)buf_meta_bread(hfsmp->hfs_devvp, 
				HFS_PHYSBLK_ROUNDDOWN(HFS_PRI_SECTOR(hfsmp->hfs_logical_block_size), hfsmp->hfs_log_per_phys),
				hfsmp->hfs_physical_block_size, NOCRED, &bp2);
		if (retval) {
			if (bp2)
				buf_brelse(bp2);
			retval = 0;
		} else {
			mdb = (HFSMasterDirectoryBlock *)(buf_dataptr(bp2) +
				HFS_PRI_OFFSET(hfsmp->hfs_physical_block_size));
>>>>>>> origin/10.5

		if (hfsmp->jvp && hfsmp->jvp != hfsmp->hfs_devvp) {
			vnode_clearmountedon(hfsmp->jvp);
			VNOP_CLOSE(hfsmp->jvp, hfsmp->hfs_flags & HFS_READ_ONLY ? FREAD : FREAD|FWRITE, vfs_context_kernel());
			vnode_put(hfsmp->jvp);
		}
		hfsmp->jvp = NULL;
		vfs_clearflags(hfsmp->hfs_mp, (u_int64_t)((unsigned int)MNT_JOURNALED));
		hfsmp->jnl_start        = 0;
		hfsmp->hfs_jnlinfoblkid = 0;
		hfsmp->hfs_jnlfileid    = 0;
		
		HFSTOVCB(hfsmp)->vcbAtrb &= ~kHFSVolumeJournaledMask;
		
		hfs_unlock_global (hfsmp);

		hfs_flushvolumeheader(hfsmp, HFS_FVH_WAIT | HFS_FVH_WRITE_ALT);

		{
			fsid_t fsid;
		
			fsid.val[0] = (int32_t)hfsmp->hfs_raw_dev;
			fsid.val[1] = (int32_t)vfs_typenum(HFSTOVFS(hfsmp));
			vfs_event_signal(&fsid, VQ_UPDATE, (intptr_t)NULL);
		}
		return 0;
	} else if (name[0] == HFS_SET_PKG_EXTENSIONS) {

	    return set_package_extensions_table((user_addr_t)((unsigned)name[1]), name[2], name[3]);
	    
	} else if (name[0] == VFS_CTL_QUERY) {
    	struct sysctl_req *req;
    	union union_vfsidctl vc;
    	struct mount *mp;
 	    struct vfsquery vq;
	
		req = CAST_DOWN(struct sysctl_req *, oldp);	/* we're new style vfs sysctl. */
		if (req == NULL) {
			return EFAULT;
		}
        
        error = SYSCTL_IN(req, &vc, proc_is64bit(p)? sizeof(vc.vc64):sizeof(vc.vc32));
		if (error) return (error);

		mp = vfs_getvfs(&vc.vc32.vc_fsid); /* works for 32 and 64 */
        if (mp == NULL) return (ENOENT);
        
		hfsmp = VFSTOHFS(mp);
		bzero(&vq, sizeof(vq));
		vq.vq_flags = hfsmp->hfs_notification_conditions;
		return SYSCTL_OUT(req, &vq, sizeof(vq));;
	} else if (name[0] == HFS_REPLAY_JOURNAL) {
		vnode_t devvp = NULL;
		int device_fd;
		if (namelen != 2) {
			return (EINVAL);
		}
		device_fd = name[1];
		error = file_vnode(device_fd, &devvp);
		if (error) {
			return error;
		}
		error = vnode_getwithref(devvp);
		if (error) {
			file_drop(device_fd);
			return error;
		}
		error = hfs_journal_replay(devvp, context);
		file_drop(device_fd);
		vnode_put(devvp);
		return error;
	} else if (name[0] == HFS_ENABLE_RESIZE_DEBUG) {
		hfs_resize_debug = 1;
		printf ("hfs_sysctl: Enabled volume resize debugging.\n");
		return 0;
	}

	return (ENOTSUP);
}

/* 
 * hfs_vfs_vget is not static since it is used in hfs_readwrite.c to support
 * the build_path ioctl.  We use it to leverage the code below that updates
 * the origin list cache if necessary
 */

int
hfs_vfs_vget(struct mount *mp, ino64_t ino, struct vnode **vpp, __unused vfs_context_t context)
{
	int error;
	int lockflags;
	struct hfsmount *hfsmp;

	hfsmp = VFSTOHFS(mp);

	error = hfs_vget(hfsmp, (cnid_t)ino, vpp, 1, 0);
	if (error)
		return error;

	/*
	 * If the look-up was via the object ID (rather than the link ID),
	 * then we make sure there's a parent here.  We can't leave this
	 * until hfs_vnop_getattr because if there's a problem getting the
	 * parent at that point, all the caller will do is call
	 * hfs_vfs_vget again and we'll end up in an infinite loop.
	 */

	cnode_t *cp = VTOC(*vpp);

	if (ISSET(cp->c_flag, C_HARDLINK) && ino == cp->c_fileid) {
		hfs_lock_always(cp, HFS_SHARED_LOCK);

		if (!hfs_haslinkorigin(cp)) {
			if (!hfs_lock_upgrade(cp))
				hfs_lock_always(cp, HFS_EXCLUSIVE_LOCK);

<<<<<<< HEAD
			if (cp->c_cnid == cp->c_fileid) {
				/*
				 * Descriptor is stale, so we need to refresh it.  We
				 * pick the first link.
				 */
				cnid_t link_id;

				error = hfs_first_link(hfsmp, cp, &link_id);
=======
		if (buf_meta_bread(hfsmp->hfs_devvp, 
				HFS_PHYSBLK_ROUNDDOWN(hfsmp->hfs_alt_id_sector, hfsmp->hfs_log_per_phys),
				hfsmp->hfs_physical_block_size, NOCRED, &alt_bp) == 0) {
			if (hfsmp->jnl) {
				journal_modify_block_start(hfsmp->jnl, alt_bp);
			}

			bcopy(volumeHeader, (char *)buf_dataptr(alt_bp) + 
					HFS_ALT_OFFSET(hfsmp->hfs_physical_block_size), 
					kMDBSize);
>>>>>>> origin/10.5

				if (!error) {
					lockflags = hfs_systemfile_lock(hfsmp, SFL_CATALOG, HFS_SHARED_LOCK);
					error = cat_findname(hfsmp, link_id, &cp->c_desc);
					hfs_systemfile_unlock(hfsmp, lockflags);
				}
			} else {
				// We'll use whatever link the descriptor happens to have
				error = 0;
			}
			if (!error)
				hfs_savelinkorigin(cp, cp->c_parentcnid);
		}

		hfs_unlock(cp);

		if (error) {
			vnode_put(*vpp);
			*vpp = NULL;
		}
	}

	return error;
}


/*
 * Look up an HFS object by ID.
 *
 * The object is returned with an iocount reference and the cnode locked.
 *
 * If the object is a file then it will represent the data fork.
 */
int
hfs_vget(struct hfsmount *hfsmp, cnid_t cnid, struct vnode **vpp, int skiplock, int allow_deleted)
{
<<<<<<< HEAD
	struct vnode *vp = NULLVP;
	struct cat_desc cndesc;
	struct cat_attr cnattr;
	struct cat_fork cnfork;
	u_int32_t linkref = 0;
	int error;
	
	/* Check for cnids that should't be exported. */
	if ((cnid < kHFSFirstUserCatalogNodeID) &&
	    (cnid != kHFSRootFolderID && cnid != kHFSRootParentID)) {
		return (ENOENT);
	}
	/* Don't export our private directories. */
	if (cnid == hfsmp->hfs_private_desc[FILE_HARDLINKS].cd_cnid ||
	    cnid == hfsmp->hfs_private_desc[DIR_HARDLINKS].cd_cnid) {
		return (ENOENT);
=======
	struct proc *p = vfs_context_proc(context);
	kauth_cred_t cred = vfs_context_ucred(context);
	struct  vnode *vp;
	struct  vnode *devvp;
	struct  buf *bp;
	struct  filefork *fp = NULL;
	ExtendedVCB  *vcb;
	struct  cat_fork forkdata;
	u_int64_t  oldsize;
	u_int64_t  newblkcnt;
	u_int64_t  prev_phys_block_count;
	u_int32_t  addblks;
	u_int64_t  sectorcnt;
	u_int32_t  sectorsize;
	u_int32_t  phys_sectorsize;
	daddr64_t  prev_alt_sector;
	daddr_t	   bitmapblks;
	int  lockflags = 0;
	int  error;
	int64_t oldBitmapSize;
	Boolean  usedExtendFileC = false;
	int transaction_begun = 0;
	
	devvp = hfsmp->hfs_devvp;
	vcb = HFSTOVCB(hfsmp);

	/*
	 * - HFS Plus file systems only. 
	 * - Journaling must be enabled.
	 * - No embedded volumes.
	 */
	if ((vcb->vcbSigWord == kHFSSigWord) ||
	     (hfsmp->jnl == NULL) ||
	     (vcb->hfsPlusIOPosOffset != 0)) {
		return (EPERM);
	}
	/*
	 * If extending file system by non-root, then verify
	 * ownership and check permissions.
	 */
	if (suser(cred, NULL)) {
		error = hfs_vget(hfsmp, kHFSRootFolderID, &vp, 0);

		if (error)
			return (error);
		error = hfs_owner_rights(hfsmp, VTOC(vp)->c_uid, cred, p, 0);
		if (error == 0) {
			error = hfs_write_access(vp, cred, p, false);
		}
		hfs_unlock(VTOC(vp));
		vnode_put(vp);
		if (error)
			return (error);

		error = vnode_authorize(devvp, NULL, KAUTH_VNODE_READ_DATA | KAUTH_VNODE_WRITE_DATA, context);
		if (error)
			return (error);
	}
	if (VNOP_IOCTL(devvp, DKIOCGETBLOCKSIZE, (caddr_t)&sectorsize, 0, context)) {
		return (ENXIO);
	}
	if (sectorsize != hfsmp->hfs_logical_block_size) {
		return (ENXIO);
	}
	if (VNOP_IOCTL(devvp, DKIOCGETBLOCKCOUNT, (caddr_t)&sectorcnt, 0, context)) {
		return (ENXIO);
	}
	if ((sectorsize * sectorcnt) < newsize) {
		printf("hfs_extendfs: not enough space on device\n");
		return (ENOSPC);
	}
	error = VNOP_IOCTL(devvp, DKIOCGETPHYSICALBLOCKSIZE, (caddr_t)&phys_sectorsize, 0, context);
	if (error) {
		if ((error != ENOTSUP) && (error != ENOTTY)) {
			return (ENXIO);
		}
		/* If ioctl is not supported, force physical and logical sector size to be same */
		phys_sectorsize = sectorsize;
	}
	oldsize = (u_int64_t)hfsmp->totalBlocks * (u_int64_t)hfsmp->blockSize;

	/*
	 * Validate new size.
	 */
	if ((newsize <= oldsize) || (newsize % sectorsize) || (newsize % phys_sectorsize)) {
		printf("hfs_extendfs: invalid size\n");
		return (EINVAL);
>>>>>>> origin/10.5
	}
<<<<<<< HEAD
=======
	newblkcnt = newsize / vcb->blockSize;
	if (newblkcnt > (u_int64_t)0xFFFFFFFF)
		return (EOVERFLOW);

	addblks = newblkcnt - vcb->totalBlocks;

	printf("hfs_extendfs: growing %s by %d blocks\n", vcb->vcbVN, addblks);

	HFS_MOUNT_LOCK(hfsmp, TRUE);
	if (hfsmp->hfs_flags & HFS_RESIZE_IN_PROGRESS) {
		HFS_MOUNT_UNLOCK(hfsmp, TRUE);
		error = EALREADY;
		goto out;
	}
	hfsmp->hfs_flags |= HFS_RESIZE_IN_PROGRESS;
	HFS_MOUNT_UNLOCK(hfsmp, TRUE);

<<<<<<< HEAD
	/* Invalidate the current free extent cache */
	invalidate_free_extent_cache(hfsmp);
	
>>>>>>> origin/10.6
=======
	/* Start with a clean journal. */
	hfs_journal_flush(hfsmp, TRUE);

>>>>>>> origin/10.7
	/*
	 * Check the hash first
	 */
<<<<<<< HEAD
	vp = hfs_chash_getvnode(hfsmp, cnid, 0, skiplock, allow_deleted);
	if (vp) {
		*vpp = vp;
		return(0);
=======
	if (hfs_start_transaction(hfsmp) != 0) {
		error = EINVAL;
		goto out;
>>>>>>> origin/10.6
	}
	transaction_begun = 1;

	bzero(&cndesc, sizeof(cndesc));
	bzero(&cnattr, sizeof(cnattr));
	bzero(&cnfork, sizeof(cnfork));

	/*
	 * Not in hash, lookup in catalog
	 */
	if (cnid == kHFSRootParentID) {
		static char hfs_rootname[] = "/";

		cndesc.cd_nameptr = (const u_int8_t *)&hfs_rootname[0];
		cndesc.cd_namelen = 1;
		cndesc.cd_parentcnid = kHFSRootParentID;
		cndesc.cd_cnid = kHFSRootFolderID;
		cndesc.cd_flags = CD_ISDIR;

		cnattr.ca_fileid = kHFSRootFolderID;
		cnattr.ca_linkcount = 1;
		cnattr.ca_entries = 1;
		cnattr.ca_dircount = 1;
		cnattr.ca_mode = (S_IFDIR | S_IRWXU | S_IRWXG | S_IRWXO);
	} else {
		int lockflags;
		cnid_t pid;
		const char *nameptr;

		lockflags = hfs_systemfile_lock(hfsmp, SFL_CATALOG, HFS_SHARED_LOCK);
		error = cat_idlookup(hfsmp, cnid, 0, 0, &cndesc, &cnattr, &cnfork);
		hfs_systemfile_unlock(hfsmp, lockflags);

		if (error) {
			*vpp = NULL;
			return (error);
		}

		/*
		 * Check for a raw hardlink inode and save its linkref.
		 */
		pid = cndesc.cd_parentcnid;
		nameptr = (const char *)cndesc.cd_nameptr;

		if ((pid == hfsmp->hfs_private_desc[FILE_HARDLINKS].cd_cnid) &&
		    (bcmp(nameptr, HFS_INODE_PREFIX, HFS_INODE_PREFIX_LEN) == 0)) {
			linkref = strtoul(&nameptr[HFS_INODE_PREFIX_LEN], NULL, 10);

		} else if ((pid == hfsmp->hfs_private_desc[DIR_HARDLINKS].cd_cnid) &&
		           (bcmp(nameptr, HFS_DIRINODE_PREFIX, HFS_DIRINODE_PREFIX_LEN) == 0)) {
			linkref = strtoul(&nameptr[HFS_DIRINODE_PREFIX_LEN], NULL, 10);

		} else if ((pid == hfsmp->hfs_private_desc[FILE_HARDLINKS].cd_cnid) &&
		           (bcmp(nameptr, HFS_DELETE_PREFIX, HFS_DELETE_PREFIX_LEN) == 0)) {
			*vpp = NULL;
			cat_releasedesc(&cndesc);
			return (ENOENT);  /* open unlinked file */
		}
	}

	/*
	 * Finish initializing cnode descriptor for hardlinks.
	 *
	 * We need a valid name and parent for reverse lookups.
	 */
<<<<<<< HEAD
	if (linkref) {
		cnid_t lastid;
		struct cat_desc linkdesc;
		int linkerr = 0;
		
		cnattr.ca_linkref = linkref;
		bzero (&linkdesc, sizeof (linkdesc));

		/* 
		 * If the caller supplied the raw inode value, then we don't know exactly
		 * which hardlink they wanted. It's likely that they acquired the raw inode
		 * value BEFORE the item became a hardlink, in which case, they probably
		 * want the oldest link.  So request the oldest link from the catalog.
		 * 
		 * Unfortunately, this requires that we iterate through all N hardlinks. On the plus
		 * side, since we know that we want the last linkID, we can also have this one
		 * call give us back the name of the last ID, since it's going to have it in-hand...
=======
	prev_phys_block_count = hfsmp->hfs_logical_block_count;
	prev_alt_sector = hfsmp->hfs_alt_id_sector;

	vcb->totalBlocks += addblks;
	vcb->freeBlocks += addblks;
	hfsmp->hfs_logical_block_count = newsize / sectorsize;
	hfsmp->hfs_alt_id_sector = (hfsmp->hfsPlusIOPosOffset / sectorsize) +
	                          HFS_ALT_SECTOR(sectorsize, hfsmp->hfs_logical_block_count);
	MarkVCBDirty(vcb);
	error = hfs_flushvolumeheader(hfsmp, MNT_WAIT, HFS_ALTFLUSH);
	if (error) {
		printf("hfs_extendfs: couldn't flush volume headers (%d)", error);
		/*
		 * Restore to old state.
>>>>>>> origin/10.5
		 */
		linkerr = hfs_lookup_lastlink (hfsmp, linkref, &lastid, &linkdesc);
		if ((linkerr == 0) && (lastid != 0)) {
			/* 
			 * Release any lingering buffers attached to our local descriptor.
			 * Then copy the name and other business into the cndesc 
			 */
<<<<<<< HEAD
			cat_releasedesc (&cndesc);
			bcopy (&linkdesc, &cndesc, sizeof(linkdesc));	
		}	
		/* If it failed, the linkref code will just use whatever it had in-hand below. */
=======
			vcb->freeBlocks += bitmapblks;
		}
		vcb->totalBlocks -= addblks;
		vcb->freeBlocks -= addblks;
		hfsmp->hfs_logical_block_count = prev_phys_block_count;
		hfsmp->hfs_alt_id_sector = prev_alt_sector;
		MarkVCBDirty(vcb);
		if (vcb->blockSize == 512)
			(void) BlockMarkAllocated(vcb, vcb->totalBlocks - 2, 2);
		else
			(void) BlockMarkAllocated(vcb, vcb->totalBlocks - 1, 1);
		goto out;
	}
	/*
	 * Invalidate the old alternate volume header.
	 */
	bp = NULL;
	if (prev_alt_sector) {
		if (buf_meta_bread(hfsmp->hfs_devvp, 
				HFS_PHYSBLK_ROUNDDOWN(prev_alt_sector, hfsmp->hfs_log_per_phys),
				hfsmp->hfs_physical_block_size, NOCRED, &bp) == 0) {
			journal_modify_block_start(hfsmp->jnl, bp);
	
			bzero((char *)buf_dataptr(bp) + HFS_ALT_OFFSET(hfsmp->hfs_physical_block_size), kMDBSize);
	
			journal_modify_block_end(hfsmp->jnl, bp, NULL, NULL);
		} else if (bp) {
			buf_brelse(bp);
		}
>>>>>>> origin/10.5
	}
<<<<<<< HEAD

	if (linkref) {
		int newvnode_flags = 0;
=======
	
	/* 
	 * Update the metadata zone size based on current volume size
	 */
	hfs_metadatazone_init(hfsmp);
	 
	/*
	 * Adjust the size of hfsmp->hfs_attrdata_vp
	 */
	if (hfsmp->hfs_attrdata_vp) {
		struct cnode *attr_cp;
		struct filefork *attr_fp;
>>>>>>> origin/10.6
		
		error = hfs_getnewvnode(hfsmp, NULL, NULL, &cndesc, 0, &cnattr,
								&cnfork, &vp, &newvnode_flags);
		if (error == 0) {
			VTOC(vp)->c_flag |= C_HARDLINK;
			vnode_setmultipath(vp);
		}
	} else {
		struct componentname cn;
		int newvnode_flags = 0;

		/* Supply hfs_getnewvnode with a component name. */
		MALLOC_ZONE(cn.cn_pnbuf, caddr_t, MAXPATHLEN, M_NAMEI, M_WAITOK);
		cn.cn_nameiop = LOOKUP;
		cn.cn_flags = ISLASTCN | HASBUF;
		cn.cn_context = NULL;
		cn.cn_pnlen = MAXPATHLEN;
		cn.cn_nameptr = cn.cn_pnbuf;
		cn.cn_namelen = cndesc.cd_namelen;
		cn.cn_hash = 0;
		cn.cn_consume = 0;
		bcopy(cndesc.cd_nameptr, cn.cn_nameptr, cndesc.cd_namelen + 1);
	
		error = hfs_getnewvnode(hfsmp, NULLVP, &cn, &cndesc, 0, &cnattr, 
								&cnfork, &vp, &newvnode_flags);

		if (error == 0 && (VTOC(vp)->c_flag & C_HARDLINK)) {
			hfs_savelinkorigin(VTOC(vp), cndesc.cd_parentcnid);
		}
		FREE_ZONE(cn.cn_pnbuf, cn.cn_pnlen, M_NAMEI);
	}
<<<<<<< HEAD
	cat_releasedesc(&cndesc);
=======
	/*
	   Regardless of whether or not the totalblocks actually increased,
	   we should reset the allocLimit field. If it changed, it will
	   get updated; if not, it will remain the same.
	*/
	HFS_MOUNT_LOCK(hfsmp, TRUE);	
	hfsmp->hfs_flags &= ~HFS_RESIZE_IN_PROGRESS;
	hfsmp->allocLimit = vcb->totalBlocks;
<<<<<<< HEAD
	hfs_systemfile_unlock(hfsmp, lockflags);
	hfs_end_transaction(hfsmp);
>>>>>>> origin/10.5
=======
	HFS_MOUNT_UNLOCK(hfsmp, TRUE);	
	if (lockflags) {
		hfs_systemfile_unlock(hfsmp, lockflags);
	}
	if (transaction_begun) {
		hfs_end_transaction(hfsmp);
		hfs_journal_flush(hfsmp, FALSE);
		/* Just to be sure, sync all data to the disk */
		(void) VNOP_IOCTL(hfsmp->hfs_devvp, DKIOCSYNCHRONIZECACHE, NULL, FWRITE, context);
	}
>>>>>>> origin/10.6

	*vpp = vp;
	if (vp && skiplock) {
		hfs_unlock(VTOC(vp));
	}
	return (error);
}


/*
 * Flush out all the files in a filesystem.
 */
static int
#if QUOTA
hfs_flushfiles(struct mount *mp, int flags, struct proc *p)
#else
hfs_flushfiles(struct mount *mp, int flags, __unused struct proc *p)
#endif /* QUOTA */
{
<<<<<<< HEAD
	struct hfsmount *hfsmp;
	struct vnode *skipvp = NULLVP;
=======
	struct  buf *bp = NULL;
	u_int64_t oldsize;
	u_int32_t newblkcnt;
	u_int32_t reclaimblks = 0;
	int lockflags = 0;
	int transaction_begun = 0;
	Boolean updateFreeBlocks = false;
>>>>>>> origin/10.6
	int error;
	int accounted_root_usecounts;
#if QUOTA
	int i;
#endif

<<<<<<< HEAD
	hfsmp = VFSTOHFS(mp);
=======
	HFS_MOUNT_LOCK(hfsmp, TRUE);	
	if (hfsmp->hfs_flags & HFS_RESIZE_IN_PROGRESS) {
		HFS_MOUNT_UNLOCK(hfsmp, TRUE);	
		return (EALREADY);
	}
	hfsmp->hfs_flags |= HFS_RESIZE_IN_PROGRESS;
	hfsmp->hfs_resize_filesmoved = 0;
	hfsmp->hfs_resize_totalfiles = 0;
	HFS_MOUNT_UNLOCK(hfsmp, TRUE);	
>>>>>>> origin/10.6

	accounted_root_usecounts = 0;
#if QUOTA
	/*
	 * The open quota files have an indirect reference on
	 * the root directory vnode.  We must account for this
	 * extra reference when doing the intial vflush.
	 */
	if (((unsigned int)vfs_flags(mp)) & MNT_QUOTA) {
		/* Find out how many quota files we have open. */
		for (i = 0; i < MAXQUOTAS; i++) {
			if (hfsmp->hfs_qfiles[i].qf_vp != NULLVP)
				++accounted_root_usecounts;
		}
	}
#endif /* QUOTA */

<<<<<<< HEAD
<<<<<<< HEAD
	if (accounted_root_usecounts > 0) {
		/* Obtain the root vnode so we can skip over it. */
		skipvp = hfs_chash_getvnode(hfsmp, kHFSRootFolderID, 0, 0, 0);
=======
=======
	if (hfs_resize_debug) {
		printf ("hfs_truncatefs: old: size=%qu, blkcnt=%u, freeblks=%u\n", oldsize, hfsmp->totalBlocks, hfs_freeblks(hfsmp, 1));
		printf ("hfs_truncatefs: new: size=%qu, blkcnt=%u, reclaimblks=%u\n", newsize, newblkcnt, reclaimblks);
	}

>>>>>>> origin/10.6
	/* Make sure new size is valid. */
	if ((newsize < HFS_MIN_SIZE) ||
	    (newsize >= oldsize) ||
	    (newsize % hfsmp->hfs_logical_block_size) ||
	    (newsize % hfsmp->hfs_physical_block_size)) {
		printf ("hfs_truncatefs: invalid size (newsize=%qu, oldsize=%qu)\n", newsize, oldsize);
		error = EINVAL;
		goto out;
	}
	/* Make sure that the file system has enough free blocks reclaim */
	if (reclaimblks >= hfs_freeblks(hfsmp, 1)) {
		printf("hfs_truncatefs: insufficient space (need %u blocks; have %u free blocks)\n", reclaimblks, hfs_freeblks(hfsmp, 1));
		error = ENOSPC;
		goto out;
	}
	
	/* Invalidate the current free extent cache */
	invalidate_free_extent_cache(hfsmp);
	
	/* Start with a clean journal. */
	journal_flush(hfsmp->jnl);
	
	if (hfs_start_transaction(hfsmp) != 0) {
		error = EINVAL;
		goto out;
>>>>>>> origin/10.5
	}

<<<<<<< HEAD
	error = vflush(mp, skipvp, SKIPSYSTEM | SKIPSWAP | flags);
	if (error != 0)
		return(error);

	error = vflush(mp, skipvp, SKIPSYSTEM | flags);

	if (skipvp) {
=======
	/*
	 * Prevent new allocations from using the part we're trying to truncate.
	 *
	 * NOTE: allocLimit is set to the allocation block number where the new
	 * alternate volume header will be.  That way there will be no files to
	 * interfere with allocating the new alternate volume header, and no files
	 * in the allocation blocks beyond (i.e. the blocks we're trying to
	 * truncate away.
	 */
	HFS_MOUNT_LOCK(hfsmp, TRUE);	
	if (hfsmp->blockSize == 512) 
		hfsmp->allocLimit = newblkcnt - 2;
	else
		hfsmp->allocLimit = newblkcnt - 1;
	/* 
	 * Update the volume free block count to reflect the total number 
	 * of free blocks that will exist after a successful resize.
	 * Relocation of extents will result in no net change in the total
	 * free space on the disk.  Therefore the code that allocates 
	 * space for new extent and deallocates the old extent explicitly 
	 * prevents updating the volume free block count.  It will also 
	 * prevent false disk full error when the number of blocks in 
	 * an extent being relocated is more than the free blocks that 
	 * will exist after the volume is resized.
	 */
	hfsmp->freeBlocks -= reclaimblks;
	updateFreeBlocks = true;
	HFS_MOUNT_UNLOCK(hfsmp, TRUE);	

	/*
	 * Update the metadata zone size, and, if required, disable it 
	 */
	hfs_metadatazone_init(hfsmp);

	/*
	 * Look for files that have blocks at or beyond the location of the
	 * new alternate volume header
	 */
	if (hfs_isallocated(hfsmp, hfsmp->allocLimit, reclaimblks)) {
>>>>>>> origin/10.6
		/*
		 * See if there are additional references on the
		 * root vp besides the ones obtained from the open
		 * quota files and CoreStorage.
		 */
<<<<<<< HEAD
		if ((error == 0) &&
		    (vnode_isinuse(skipvp,  accounted_root_usecounts))) {
			error = EBUSY;  /* root directory is still open */
=======
		hfs_end_transaction(hfsmp);
		transaction_begun = 0;

		/* Attempt to reclaim some space. */ 
		error = hfs_reclaimspace(hfsmp, hfsmp->allocLimit, reclaimblks, context);
		if (error != 0) {
			printf("hfs_truncatefs: couldn't reclaim space on %s (error=%d)\n", hfsmp->vcbVN, error);
			error = ENOSPC;
			goto out;
		}
		if (hfs_start_transaction(hfsmp) != 0) {
			error = EINVAL;
			goto out;
		}
		transaction_begun = 1;
		
		/* Check if we're clear now. */
		error = hfs_isallocated(hfsmp, hfsmp->allocLimit, reclaimblks);
		if (error != 0) {
			printf("hfs_truncatefs: didn't reclaim enough space on %s (error=%d)\n", hfsmp->vcbVN, error);
			error = EAGAIN;  /* tell client to try again */
			goto out;
>>>>>>> origin/10.6
		}
		hfs_unlock(VTOC(skipvp));
		/* release the iocount from the hfs_chash_getvnode call above. */
		vnode_put(skipvp);
	}
	if (error && (flags & FORCECLOSE) == 0)
		return (error);

<<<<<<< HEAD
#if QUOTA
	if (((unsigned int)vfs_flags(mp)) & MNT_QUOTA) {
		for (i = 0; i < MAXQUOTAS; i++) {
			if (hfsmp->hfs_qfiles[i].qf_vp == NULLVP)
				continue;
			hfs_quotaoff(p, mp, i);
=======
	/*
	 * Invalidate the existing alternate volume header.
	 *
	 * Don't include this in a transaction (don't call journal_modify_block)
	 * since this block will be outside of the truncated file system!
	 */
	if (hfsmp->hfs_alt_id_sector) {
		error = buf_meta_bread(hfsmp->hfs_devvp, 
				HFS_PHYSBLK_ROUNDDOWN(hfsmp->hfs_alt_id_sector, hfsmp->hfs_log_per_phys),
				hfsmp->hfs_physical_block_size, NOCRED, &bp);
		if (error == 0) {
			bzero((void*)((char *)buf_dataptr(bp) + HFS_ALT_OFFSET(hfsmp->hfs_physical_block_size)), kMDBSize);
			(void) VNOP_BWRITE(bp);
<<<<<<< HEAD
		} else if (bp) {
			buf_brelse(bp);
>>>>>>> origin/10.5
=======
		} else {
			if (bp) {
				buf_brelse(bp);
			}
>>>>>>> origin/10.6
		}
	}
#endif /* QUOTA */

<<<<<<< HEAD
	if (skipvp) {
		error = vflush(mp, NULLVP, SKIPSYSTEM | flags);
=======
	/* Log successful shrinking. */
	printf("hfs_truncatefs: shrank \"%s\" to %d blocks (was %d blocks)\n",
	       hfsmp->vcbVN, newblkcnt, hfsmp->totalBlocks);

	/*
	 * Adjust file system variables and flush them to disk.
	 */
	hfsmp->totalBlocks = newblkcnt;
	hfsmp->hfs_logical_block_count = newsize / hfsmp->hfs_logical_block_size;
	hfsmp->hfs_alt_id_sector = HFS_ALT_SECTOR(hfsmp->hfs_logical_block_size, hfsmp->hfs_logical_block_count);
	MarkVCBDirty(hfsmp);
	error = hfs_flushvolumeheader(hfsmp, MNT_WAIT, HFS_ALTFLUSH);
	if (error)
		panic("hfs_truncatefs: unexpected error flushing volume header (%d)\n", error);
	
	/*
	 * Adjust the size of hfsmp->hfs_attrdata_vp
	 */
	if (hfsmp->hfs_attrdata_vp) {
		struct cnode *cp;
		struct filefork *fp;
		
		if (vnode_get(hfsmp->hfs_attrdata_vp) == 0) {
			cp = VTOC(hfsmp->hfs_attrdata_vp);
			fp = VTOF(hfsmp->hfs_attrdata_vp);
			
			cp->c_blocks = newblkcnt;
			fp->ff_blocks = newblkcnt;
			fp->ff_extents[0].blockCount = newblkcnt;
			fp->ff_size = (off_t) newblkcnt * hfsmp->blockSize;
			ubc_setsize(hfsmp->hfs_attrdata_vp, fp->ff_size);
			vnode_put(hfsmp->hfs_attrdata_vp);
		}
	}
	
out:
	lck_mtx_lock(&hfsmp->hfs_mutex);
	if (error && (updateFreeBlocks == true)) 
		hfsmp->freeBlocks += reclaimblks;
	hfsmp->allocLimit = hfsmp->totalBlocks;
	if (hfsmp->nextAllocation >= hfsmp->allocLimit)
		hfsmp->nextAllocation = hfsmp->hfs_metazone_end + 1;
	hfsmp->hfs_flags &= ~HFS_RESIZE_IN_PROGRESS;
	HFS_MOUNT_UNLOCK(hfsmp, TRUE);	
	/* On error, reset the metadata zone for original volume size */
	if (error && (updateFreeBlocks == true)) {
		hfs_metadatazone_init(hfsmp);
	}
	
	if (lockflags) {
		hfs_systemfile_unlock(hfsmp, lockflags);
	}
	if (transaction_begun) {
		hfs_end_transaction(hfsmp);
<<<<<<< HEAD
		journal_flush(hfsmp->jnl);
>>>>>>> origin/10.5
=======
		hfs_journal_flush(hfsmp);
		/* Just to be sure, sync all data to the disk */
		(void) VNOP_IOCTL(hfsmp->hfs_devvp, DKIOCSYNCHRONIZECACHE, NULL, FWRITE, context);
>>>>>>> origin/10.6
	}

	return (error);
}

/*
 * Update volume encoding bitmap (HFS Plus only)
 * 
 * Mark a legacy text encoding as in-use (as needed)
 * in the volume header of this HFS+ filesystem.
 */
__private_extern__
void
hfs_setencodingbits(struct hfsmount *hfsmp, u_int32_t encoding)
{
<<<<<<< HEAD
#define  kIndexMacUkrainian	48  /* MacUkrainian encoding is 152 */
#define  kIndexMacFarsi		49  /* MacFarsi encoding is 140 */
=======
	int err = 0;
	size_t bufferSize;
	void *buffer = NULL;
	struct vfsioattr ioattr;
	buf_t bp = NULL;
	off_t resid;
	size_t ioSize;
	u_int32_t ioSizeSectors;	/* Device sectors in this I/O */
	daddr64_t srcSector, destSector;
	u_int32_t sectorsPerBlock = hfsmp->blockSize / hfsmp->hfs_logical_block_size;
>>>>>>> origin/10.5

	u_int32_t	index;

<<<<<<< HEAD
	switch (encoding) {
	case kTextEncodingMacUkrainian:
		index = kIndexMacUkrainian;
		break;
	case kTextEncodingMacFarsi:
		index = kIndexMacFarsi;
		break;
	default:
		index = encoding;
		break;
=======
	/*
	 * Determine the I/O size to use
	 *
	 * NOTE: Many external drives will result in an ioSize of 128KB.
	 * TODO: Should we use a larger buffer, doing several consecutive
	 * reads, then several consecutive writes?
	 */
	vfs_ioattr(hfsmp->hfs_mp, &ioattr);
	bufferSize = MIN(ioattr.io_maxreadcnt, ioattr.io_maxwritecnt);
	if (kmem_alloc(kernel_map, (vm_offset_t*) &buffer, bufferSize))
		return ENOMEM;

	/* Get a buffer for doing the I/O */
	bp = buf_alloc(hfsmp->hfs_devvp);
	buf_setdataptr(bp, (uintptr_t)buffer);
	
	resid = (off_t) blockCount * (off_t) hfsmp->blockSize;
	srcSector = (daddr64_t) oldStart * hfsmp->blockSize / hfsmp->hfs_logical_block_size;
	destSector = (daddr64_t) newStart * hfsmp->blockSize / hfsmp->hfs_logical_block_size;
	while (resid > 0) {
		ioSize = MIN(bufferSize, resid);
		ioSizeSectors = ioSize / hfsmp->hfs_logical_block_size;
		
		/* Prepare the buffer for reading */
		buf_reset(bp, B_READ);
		buf_setsize(bp, ioSize);
		buf_setcount(bp, ioSize);
		buf_setblkno(bp, srcSector);
		buf_setlblkno(bp, srcSector);
		
		/* Do the read */
		err = VNOP_STRATEGY(bp);
		if (!err)
			err = buf_biowait(bp);
		if (err) {
			printf("hfs_copy_extent: Error %d from VNOP_STRATEGY (read)\n", err);
			break;
		}
		
		/* Prepare the buffer for writing */
		buf_reset(bp, B_WRITE);
		buf_setsize(bp, ioSize);
		buf_setcount(bp, ioSize);
		buf_setblkno(bp, destSector);
		buf_setlblkno(bp, destSector);
		if (vnode_issystem(vp) && journal_uses_fua(hfsmp->jnl))
			buf_markfua(bp);
			
		/* Do the write */
		vnode_startwrite(hfsmp->hfs_devvp);
		err = VNOP_STRATEGY(bp);
		if (!err)
			err = buf_biowait(bp);
		if (err) {
			printf("hfs_copy_extent: Error %d from VNOP_STRATEGY (write)\n", err);
			break;
		}
		
		resid -= ioSize;
		srcSector += ioSizeSectors;
		destSector += ioSizeSectors;
>>>>>>> origin/10.5
	}

<<<<<<< HEAD
	/* Only mark the encoding as in-use if it wasn't already set */
	if (index < 64 && (hfsmp->encodingsBitmap & (u_int64_t)(1ULL << index)) == 0) {
		hfs_lock_mount (hfsmp);
		hfsmp->encodingsBitmap |= (u_int64_t)(1ULL << index);
		MarkVCBDirty(hfsmp);
		hfs_unlock_mount(hfsmp);
=======
	/* Make sure all writes have been flushed to disk. */
	if (vnode_issystem(vp) && !journal_uses_fua(hfsmp->jnl)) {
		err = VNOP_IOCTL(hfsmp->hfs_devvp, DKIOCSYNCHRONIZECACHE, NULL, FWRITE, context);
		if (err) {
			printf("hfs_copy_extent: DKIOCSYNCHRONIZECACHE failed (%d)\n", err);
			err = 0;	/* Don't fail the copy. */
		}
>>>>>>> origin/10.6
	}
<<<<<<< HEAD
=======

	if (!err)
		hfs_invalidate_sectors(vp, (daddr64_t)oldStart*sectorsPerBlock, (daddr64_t)blockCount*sectorsPerBlock);

	return err;
}


/* Structure to store state of reclaiming extents from a 
 * given file.  hfs_reclaim_file()/hfs_reclaim_xattr() 
 * initializes the values in this structure which are then 
 * used by code that reclaims and splits the extents.
 */
struct hfs_reclaim_extent_info {
	struct vnode *vp;
	u_int32_t fileID;
	u_int8_t forkType;
	u_int8_t is_dirlink;                 /* Extent belongs to directory hard link */
	u_int8_t is_sysfile;                 /* Extent belongs to system file */
	u_int8_t is_xattr;                   /* Extent belongs to extent-based xattr */
	u_int8_t extent_index;
	int lockflags;                       /* Locks that reclaim and split code should grab before modifying the extent record */
	u_int32_t blocks_relocated;          /* Total blocks relocated for this file till now */
	u_int32_t recStartBlock;             /* File allocation block number (FABN) for current extent record */
	u_int32_t cur_blockCount;            /* Number of allocation blocks that have been checked for reclaim */
	struct filefork *catalog_fp;         /* If non-NULL, extent is from catalog record */
	union record {
		HFSPlusExtentRecord overflow;/* Extent record from overflow extents btree */
		HFSPlusAttrRecord xattr;     /* Attribute record for large EAs */
	} record;
	HFSPlusExtentDescriptor *extents;    /* Pointer to current extent record being processed.
					      * For catalog extent record, points to the correct 
					      * extent information in filefork.  For overflow extent 
					      * record, or xattr record, points to extent record 
					      * in the structure above
					      */
	struct cat_desc *dirlink_desc;	
	struct cat_attr *dirlink_attr;
	struct filefork *dirlink_fork;	      /* For directory hard links, fp points actually to this */
	struct BTreeIterator *iterator;       /* Shared read/write iterator, hfs_reclaim_file/xattr() 
                                               * use it for reading and hfs_reclaim_extent()/hfs_split_extent() 
					       * use it for writing updated extent record 
					       */ 
	struct FSBufferDescriptor btdata;     /* Shared btdata for reading/writing extent record, same as iterator above */
	u_int16_t recordlen;
	int overflow_count;                   /* For debugging, counter for overflow extent record */
	FCB *fcb;                             /* Pointer to the current btree being traversed */
};

/* 
 * Split the current extent into two extents, with first extent 
 * to contain given number of allocation blocks.  Splitting of 
 * extent creates one new extent entry which can result in 
 * shifting of many entries through all the extent records of a 
 * file, and/or creating a new extent record in the overflow 
 * extent btree. 
 *
 * Example:
 * The diagram below represents two consecutive extent records, 
 * for simplicity, lets call them record X and X+1 respectively.
 * Interesting extent entries have been denoted by letters.  
 * If the letter is unchanged before and after split, it means 
 * that the extent entry was not modified during the split.  
 * A '.' means that the entry remains unchanged after the split 
 * and is not relevant for our example.  A '0' means that the 
 * extent entry is empty.  
 *
 * If there isn't sufficient contiguous free space to relocate 
 * an extent (extent "C" below), we will have to break the one 
 * extent into multiple smaller extents, and relocate each of 
 * the smaller extents individually.  The way we do this is by 
 * finding the largest contiguous free space that is currently 
 * available (N allocation blocks), and then convert extent "C" 
 * into two extents, C1 and C2, that occupy exactly the same 
 * allocation blocks as extent C.  Extent C1 is the first 
 * N allocation blocks of extent C, and extent C2 is the remainder 
 * of extent C.  Then we can relocate extent C1 since we know 
 * we have enough contiguous free space to relocate it in its 
 * entirety.  We then repeat the process starting with extent C2. 
 *
 * In record X, only the entries following entry C are shifted, and 
 * the original entry C is replaced with two entries C1 and C2 which
 * are actually two extent entries for contiguous allocation blocks.
 *
 * Note that the entry E from record X is shifted into record X+1 as 
 * the new first entry.  Since the first entry of record X+1 is updated, 
 * the FABN will also get updated with the blockCount of entry E.  
 * This also results in shifting of all extent entries in record X+1.  
 * Note that the number of empty entries after the split has been 
 * changed from 3 to 2. 
 *
 * Before:
 *               record X                           record X+1
 *  ---------------------===---------     ---------------------------------
 *  | A | . | . | . | B | C | D | E |     | F | . | . | . | G | 0 | 0 | 0 |
 *  ---------------------===---------     ---------------------------------    
 *
 * After:
 *  ---------------------=======-----     ---------------------------------
 *  | A | . | . | . | B | C1| C2| D |     | E | F | . | . | . | G | 0 | 0 |
 *  ---------------------=======-----     ---------------------------------    
 *
 *  C1.startBlock = C.startBlock          
 *  C1.blockCount = N
 *
 *  C2.startBlock = C.startBlock + N
 *  C2.blockCount = C.blockCount - N
 *
 *                                        FABN = old FABN - E.blockCount
 *
 * Inputs: 
 *	extent_info -   This is the structure that contains state about 
 *	                the current file, extent, and extent record that 
 *	                is being relocated.  This structure is shared 
 *	                among code that traverses through all the extents 
 *	                of the file, code that relocates extents, and 
 *	                code that splits the extent. 
 *	newBlockCount - The blockCount of the extent to be split after 
 *	                successfully split operation.
 * Output:
 * 	Zero on success, non-zero on failure.
 */
static int 
hfs_split_extent(struct hfs_reclaim_extent_info *extent_info, uint32_t newBlockCount)
{
	int error = 0;
	int index = extent_info->extent_index;
	int i;
	HFSPlusExtentDescriptor shift_extent; /* Extent entry that should be shifted into next extent record */
	HFSPlusExtentDescriptor last_extent;
	HFSPlusExtentDescriptor *extents; /* Pointer to current extent record being manipulated */
	HFSPlusExtentRecord *extents_rec = NULL;
	HFSPlusExtentKey *extents_key = NULL;
	HFSPlusAttrRecord *xattr_rec = NULL;
	HFSPlusAttrKey *xattr_key = NULL;
	struct BTreeIterator iterator;
	struct FSBufferDescriptor btdata;
	uint16_t reclen;
	uint32_t read_recStartBlock;	/* Starting allocation block number to read old extent record */
	uint32_t write_recStartBlock;	/* Starting allocation block number to insert newly updated extent record */
	Boolean create_record = false;
	Boolean is_xattr;
	struct cnode *cp;
       
	is_xattr = extent_info->is_xattr;
	extents = extent_info->extents;
	cp = VTOC(extent_info->vp);

	if (newBlockCount == 0) {
		if (hfs_resize_debug) {
			printf ("hfs_split_extent: No splitting required for newBlockCount=0\n");
		}
		return error;
	}

	if (hfs_resize_debug) {
		printf ("hfs_split_extent: Split record:%u recStartBlock=%u %u:(%u,%u) for %u blocks\n", extent_info->overflow_count, extent_info->recStartBlock, index, extents[index].startBlock, extents[index].blockCount, newBlockCount);
	}

	/* Extents overflow btree can not have more than 8 extents.  
	 * No split allowed if the 8th extent is already used. 
	 */
	if ((extent_info->fileID == kHFSExtentsFileID) && (extents[kHFSPlusExtentDensity - 1].blockCount != 0)) {
		printf ("hfs_split_extent: Maximum 8 extents allowed for extents overflow btree, cannot split further.\n");
		error = ENOSPC;
		goto out;
	}

	/* Determine the starting allocation block number for the following
	 * overflow extent record, if any, before the current record 
	 * gets modified. 
	 */
	read_recStartBlock = extent_info->recStartBlock;
	for (i = 0; i < kHFSPlusExtentDensity; i++) {
		if (extents[i].blockCount == 0) {
			break;
		}
		read_recStartBlock += extents[i].blockCount;
	}

	/* Shift and split */
	if (index == kHFSPlusExtentDensity-1) {
		/* The new extent created after split will go into following overflow extent record */
		shift_extent.startBlock = extents[index].startBlock + newBlockCount;
		shift_extent.blockCount = extents[index].blockCount - newBlockCount;

		/* Last extent in the record will be split, so nothing to shift */
	} else {
		/* Splitting of extents can result in at most of one 
		 * extent entry to be shifted into following overflow extent 
		 * record.  So, store the last extent entry for later. 
		 */
		shift_extent = extents[kHFSPlusExtentDensity-1];
		if ((hfs_resize_debug) && (shift_extent.blockCount != 0)) {
			printf ("hfs_split_extent: Save 7:(%u,%u) to shift into overflow record\n", shift_extent.startBlock, shift_extent.blockCount);
		}

		/* Start shifting extent information from the end of the extent 
		 * record to the index where we want to insert the new extent.
		 * Note that kHFSPlusExtentDensity-1 is already saved above, and 
		 * does not need to be shifted.  The extent entry that is being 
		 * split does not get shifted.
		 */
		for (i = kHFSPlusExtentDensity-2; i > index; i--) {
			if (hfs_resize_debug) {
				if (extents[i].blockCount) {
					printf ("hfs_split_extent: Shift %u:(%u,%u) to %u:(%u,%u)\n", i, extents[i].startBlock, extents[i].blockCount, i+1, extents[i].startBlock, extents[i].blockCount);
				}
			}
			extents[i+1] = extents[i];
		}
	}

	if (index == kHFSPlusExtentDensity-1) {
		/* The second half of the extent being split will be the overflow 
		 * entry that will go into following overflow extent record.  The
		 * value has been stored in 'shift_extent' above, so there is 
		 * nothing to be done here.
		 */
	} else {
		/* Update the values in the second half of the extent being split 
		 * before updating the first half of the split.  Note that the 
		 * extent to split or first half of the split is at index 'index' 
		 * and a new extent or second half of the split will be inserted at 
		 * 'index+1' or into following overflow extent record. 
		 */ 
		extents[index+1].startBlock = extents[index].startBlock + newBlockCount;
		extents[index+1].blockCount = extents[index].blockCount - newBlockCount;
	}
	/* Update the extent being split, only the block count will change */
	extents[index].blockCount = newBlockCount;

	if (hfs_resize_debug) {
		printf ("hfs_split_extent: Split %u:(%u,%u) and ", index, extents[index].startBlock, extents[index].blockCount);
		if (index != kHFSPlusExtentDensity-1) {
			printf ("%u:(%u,%u)\n", index+1, extents[index+1].startBlock, extents[index+1].blockCount);
		} else {
			printf ("overflow:(%u,%u)\n", shift_extent.startBlock, shift_extent.blockCount);
		}
	}

	/* Write out information about the newly split extent to the disk */
	if (extent_info->catalog_fp) {
		/* (extent_info->catalog_fp != NULL) means the newly split 
		 * extent exists in the catalog record.  This means that 
		 * the cnode was updated.  Therefore, to write out the changes,
		 * mark the cnode as modified.   We cannot call hfs_update()
		 * in this function because the caller hfs_reclaim_extent() 
		 * is holding the catalog lock currently.
		 */
		cp->c_flag |= C_MODIFIED;
	} else {
		/* The newly split extent is for large EAs or is in overflow 
		 * extent record, so update it directly in the btree using the 
		 * iterator information from the shared extent_info structure
	 	 */
		error = BTReplaceRecord(extent_info->fcb, extent_info->iterator, 
				&(extent_info->btdata), extent_info->recordlen);
		if (error) {
			printf ("hfs_split_extent: fileID=%u BTReplaceRecord returned error=%d\n", extent_info->fileID, error);
			goto out;
		}
	}
		
	/* No extent entry to be shifted into another extent overflow record */
	if (shift_extent.blockCount == 0) {
		if (hfs_resize_debug) {
			printf ("hfs_split_extent: No extent entry to be shifted into overflow records\n");
		}
		error = 0;
		goto out;
	}

	/* The overflow extent entry has to be shifted into an extent 
	 * overflow record.  This means that we might have to shift 
	 * extent entries from all subsequent overflow records by one. 
	 * We start iteration from the first record to the last record, 
	 * and shift the extent entry from one record to another.  
	 * We might have to create a new extent record for the last 
	 * extent entry for the file. 
	 */
	
	/* Initialize iterator to search the next record */
	bzero(&iterator, sizeof(iterator));
	if (is_xattr) {
		/* Copy the key from the iterator that was used to update the modified attribute record. */
		xattr_key = (HFSPlusAttrKey *)&(iterator.key);
		bcopy((HFSPlusAttrKey *)&(extent_info->iterator->key), xattr_key, sizeof(HFSPlusAttrKey));
		/* Note: xattr_key->startBlock will be initialized later in the iteration loop */

		MALLOC(xattr_rec, HFSPlusAttrRecord *, 
				sizeof(HFSPlusAttrRecord), M_TEMP, M_WAITOK);
		if (xattr_rec == NULL) {
			error = ENOMEM;
			goto out;
		}
		btdata.bufferAddress = xattr_rec;
		btdata.itemSize = sizeof(HFSPlusAttrRecord);
		btdata.itemCount = 1;
		extents = xattr_rec->overflowExtents.extents;
	} else {
		/* Initialize the extent key for the current file */
		extents_key = (HFSPlusExtentKey *) &(iterator.key);
		extents_key->keyLength = kHFSPlusExtentKeyMaximumLength;
		extents_key->forkType = extent_info->forkType;
		extents_key->fileID = extent_info->fileID;
		/* Note: extents_key->startBlock will be initialized later in the iteration loop */
		
		MALLOC(extents_rec, HFSPlusExtentRecord *, 
				sizeof(HFSPlusExtentRecord), M_TEMP, M_WAITOK);
		if (extents_rec == NULL) {
			error = ENOMEM;
			goto out;
		}
		btdata.bufferAddress = extents_rec;
		btdata.itemSize = sizeof(HFSPlusExtentRecord);
		btdata.itemCount = 1;
		extents = extents_rec[0];
	}

	/* The overflow extent entry has to be shifted into an extent 
	 * overflow record.  This means that we might have to shift 
	 * extent entries from all subsequent overflow records by one. 
	 * We start iteration from the first record to the last record, 
	 * examine one extent record in each iteration and shift one 
	 * extent entry from one record to another.  We might have to 
	 * create a new extent record for the last extent entry for the 
	 * file. 
	 *
	 * If shift_extent.blockCount is non-zero, it means that there is 
	 * an extent entry that needs to be shifted into the next 
	 * overflow extent record.  We keep on going till there are no such 
	 * entries left to be shifted.  This will also change the starting 
	 * allocation block number of the extent record which is part of 
	 * the key for the extent record in each iteration.  Note that 
	 * because the extent record key is changing while we are searching, 
	 * the record can not be updated directly, instead it has to be 
	 * deleted and inserted again.
	 */
	while (shift_extent.blockCount) {
		if (hfs_resize_debug) {
			printf ("hfs_split_extent: Will shift (%u,%u) into overflow record with startBlock=%u\n", shift_extent.startBlock, shift_extent.blockCount, read_recStartBlock);
		}

		/* Search if there is any existing overflow extent record
		 * that matches the current file and the logical start block 
		 * number.
		 *
		 * For this, the logical start block number in the key is 
		 * the value calculated based on the logical start block 
		 * number of the current extent record and the total number 
		 * of blocks existing in the current extent record.  
		 */
		if (is_xattr) {
			xattr_key->startBlock = read_recStartBlock;
		} else {
			extents_key->startBlock = read_recStartBlock;
		}
		error = BTSearchRecord(extent_info->fcb, &iterator, &btdata, &reclen, &iterator);
		if (error) {
			if (error != btNotFound) {
				printf ("hfs_split_extent: fileID=%u startBlock=%u BTSearchRecord error=%d\n", extent_info->fileID, read_recStartBlock, error);
				goto out;
			}
			/* No matching record was found, so create a new extent record.
			 * Note:  Since no record was found, we can't rely on the 
			 * btree key in the iterator any longer.  This will be initialized
			 * later before we insert the record.
			 */
			create_record = true;
		}
	
		/* The extra extent entry from the previous record is being inserted
		 * as the first entry in the current extent record.  This will change 
		 * the file allocation block number (FABN) of the current extent 
		 * record, which is the startBlock value from the extent record key.
		 * Since one extra entry is being inserted in the record, the new 
		 * FABN for the record will less than old FABN by the number of blocks 
		 * in the new extent entry being inserted at the start.  We have to 
		 * do this before we update read_recStartBlock to point at the 
		 * startBlock of the following record.
		 */
		write_recStartBlock = read_recStartBlock - shift_extent.blockCount;
		if (hfs_resize_debug) {
			if (create_record) {
				printf ("hfs_split_extent: No records found for startBlock=%u, will create new with startBlock=%u\n", read_recStartBlock, write_recStartBlock);
			}
		}

		/* Now update the read_recStartBlock to account for total number 
		 * of blocks in this extent record.  It will now point to the 
		 * starting allocation block number for the next extent record.
		 */
		for (i = 0; i < kHFSPlusExtentDensity; i++) {
			if (extents[i].blockCount == 0) {
				break;
			}
			read_recStartBlock += extents[i].blockCount;
		}

		if (create_record == true) {
			/* Initialize new record content with only one extent entry */
			bzero(extents, sizeof(HFSPlusExtentRecord));
			/* The new record will contain only one extent entry */
			extents[0] = shift_extent;
			/* There are no more overflow extents to be shifted */
			shift_extent.startBlock = shift_extent.blockCount = 0;

			if (is_xattr) {
				/* BTSearchRecord above returned btNotFound,
				 * but since the attribute btree is never empty
				 * if we are trying to insert new overflow 
				 * record for the xattrs, the extents_key will
				 * contain correct data.  So we don't need to 
				 * re-initialize it again like below. 
				 */

				/* Initialize the new xattr record */
				xattr_rec->recordType = kHFSPlusAttrExtents; 
				xattr_rec->overflowExtents.reserved = 0;
				reclen = sizeof(HFSPlusAttrExtents);
			} else {
				/* BTSearchRecord above returned btNotFound, 
				 * which means that extents_key content might 
				 * not correspond to the record that we are 
				 * trying to create, especially when the extents 
				 * overflow btree is empty.  So we reinitialize 
				 * the extents_key again always. 
				 */
				extents_key->keyLength = kHFSPlusExtentKeyMaximumLength;
				extents_key->forkType = extent_info->forkType;
				extents_key->fileID = extent_info->fileID;

				/* Initialize the new extent record */
				reclen = sizeof(HFSPlusExtentRecord);
			}
		} else {
			/* The overflow extent entry from previous record will be 
			 * the first entry in this extent record.  If the last 
			 * extent entry in this record is valid, it will be shifted 
			 * into the following extent record as its first entry.  So 
			 * save the last entry before shifting entries in current 
			 * record.
			 */
			last_extent = extents[kHFSPlusExtentDensity-1];
			
			/* Shift all entries by one index towards the end */
			for (i = kHFSPlusExtentDensity-2; i >= 0; i--) {
				extents[i+1] = extents[i];
			}

			/* Overflow extent entry saved from previous record 
			 * is now the first entry in the current record.
			 */
			extents[0] = shift_extent;

			if (hfs_resize_debug) {
				printf ("hfs_split_extent: Shift overflow=(%u,%u) to record with updated startBlock=%u\n", shift_extent.startBlock, shift_extent.blockCount, write_recStartBlock);
			}

			/* The last entry from current record will be the 
			 * overflow entry which will be the first entry for 
			 * the following extent record.
			 */
			shift_extent = last_extent;

			/* Since the key->startBlock is being changed for this record, 
			 * it should be deleted and inserted with the new key.
			 */
			error = BTDeleteRecord(extent_info->fcb, &iterator);
			if (error) {
				printf ("hfs_split_extent: fileID=%u startBlock=%u BTDeleteRecord error=%d\n", extent_info->fileID, read_recStartBlock, error);
				goto out;
			}
			if (hfs_resize_debug) {
				printf ("hfs_split_extent: Deleted extent record with startBlock=%u\n", (is_xattr ? xattr_key->startBlock : extents_key->startBlock));
			}
		}

		/* Insert the newly created or modified extent record */
		bzero(&iterator.hint, sizeof(iterator.hint));
		if (is_xattr) {
			xattr_key->startBlock = write_recStartBlock;
		} else {
			extents_key->startBlock = write_recStartBlock;
		}
		error = BTInsertRecord(extent_info->fcb, &iterator, &btdata, reclen);
		if (error) {
			printf ("hfs_split_extent: fileID=%u, startBlock=%u BTInsertRecord error=%d\n", extent_info->fileID, write_recStartBlock, error);
			goto out;
		}
		if (hfs_resize_debug) {
			printf ("hfs_split_extent: Inserted extent record with startBlock=%u\n", write_recStartBlock);
		}
	}

out:
	/* 
	 * Extents overflow btree or attributes btree headers might have 
	 * been modified during the split/shift operation, so flush the 
	 * changes to the disk while we are inside journal transaction.  
	 * We should only be able to generate I/O that modifies the B-Tree 
	 * header nodes while we're in the middle of a journal transaction.  
	 * Otherwise it might result in panic during unmount.
	 */
	BTFlushPath(extent_info->fcb);

	if (extents_rec) {
		FREE (extents_rec, M_TEMP);
	}
	if (xattr_rec) {
		FREE (xattr_rec, M_TEMP);
	}
	return error;
>>>>>>> origin/10.7
}

<<<<<<< HEAD
/*
 * Update volume stats
 *
 * On journal volumes this will cause a volume header flush
 */
int
hfs_volupdate(struct hfsmount *hfsmp, enum volop op, int inroot)
{
<<<<<<< HEAD
	struct timeval tv;
=======
	int error = 0;
	int index;
	struct cnode *cp;
	u_int32_t oldStartBlock;
	u_int32_t oldBlockCount;
	u_int32_t newStartBlock;
	u_int32_t newBlockCount;
	u_int32_t roundedBlockCount;
	uint16_t node_size;
	uint32_t remainder_blocks;
	u_int32_t alloc_flags;
	int blocks_allocated = false;

	index = extent_info->extent_index;
	cp = VTOC(extent_info->vp);

	oldStartBlock = extent_info->extents[index].startBlock;
	oldBlockCount = extent_info->extents[index].blockCount;

	if (0 && hfs_resize_debug) {
		printf ("hfs_reclaim_extent: Examine record:%u recStartBlock=%u, %u:(%u,%u)\n", extent_info->overflow_count, extent_info->recStartBlock, index, oldStartBlock, oldBlockCount);
	}

	/* If the current extent lies completely within allocLimit, 
	 * it does not require any relocation. 
	 */
	if ((oldStartBlock + oldBlockCount) <= allocLimit) {
		extent_info->cur_blockCount += oldBlockCount;
		return error;
	} 

	/* Every extent should be relocated in its own transaction
	 * to make sure that we don't overflow the journal buffer.
	 */
	error = hfs_start_transaction(hfsmp);
	if (error) {
		return error;
	}
	extent_info->lockflags = hfs_systemfile_lock(hfsmp, extent_info->lockflags, HFS_EXCLUSIVE_LOCK);

	/* Check if the extent lies partially in the area to reclaim, 
	 * i.e. it starts before allocLimit and ends beyond allocLimit.  
	 * We have already skipped extents that lie completely within 
	 * allocLimit in the check above, so we only check for the 
	 * startBlock.  If it lies partially, split it so that we 
	 * only relocate part of the extent.
	 */
	if (oldStartBlock < allocLimit) {
		newBlockCount = allocLimit - oldStartBlock;

		if (hfs_resize_debug) {
			int idx = extent_info->extent_index;
			printf ("hfs_reclaim_extent: Split straddling extent %u:(%u,%u) for %u blocks\n", idx, extent_info->extents[idx].startBlock, extent_info->extents[idx].blockCount, newBlockCount);
		}

		/* If the extent belongs to a btree, check and trim 
		 * it to be multiple of the node size. 
		 */
		if (extent_info->is_sysfile) {
			node_size = get_btree_nodesize(extent_info->vp);
			/* If the btree node size is less than the block size, 
			 * splitting this extent will not split a node across 
			 * different extents.  So we only check and trim if 
			 * node size is more than the allocation block size. 
			 */ 
			if (node_size > hfsmp->blockSize) {
				remainder_blocks = newBlockCount % (node_size / hfsmp->blockSize);
				if (remainder_blocks) {
					newBlockCount -= remainder_blocks;
					if (hfs_resize_debug) {
						printf ("hfs_reclaim_extent: Round-down newBlockCount to be multiple of nodeSize, node_allocblks=%u, old=%u, new=%u\n", node_size/hfsmp->blockSize, newBlockCount + remainder_blocks, newBlockCount);
					}
				}
			}
			/* The newBlockCount is zero because of rounding-down so that
			 * btree nodes are not split across extents.  Therefore this
			 * straddling extent across resize-boundary does not require 
			 * splitting.  Skip over to relocating of complete extent.
			 */
			if (newBlockCount == 0) {
				if (hfs_resize_debug) {
					printf ("hfs_reclaim_extent: After round-down newBlockCount=0, skip split, relocate full extent\n");
				}
				goto relocate_full_extent;
			}
		}

		/* Split the extents into two parts --- the first extent lies
		 * completely within allocLimit and therefore does not require
		 * relocation.  The second extent will require relocation which
		 * will be handled when the caller calls this function again 
		 * for the next extent. 
		 */
		error = hfs_split_extent(extent_info, newBlockCount);
		if (error == 0) {
			/* Split success, no relocation required */
			goto out;
		}
		/* Split failed, so try to relocate entire extent */
		if (hfs_resize_debug) {
			int idx = extent_info->extent_index;
			printf ("hfs_reclaim_extent: Split straddling extent %u:(%u,%u) for %u blocks failed, relocate full extent\n", idx, extent_info->extents[idx].startBlock, extent_info->extents[idx].blockCount, newBlockCount);
		}
	}

relocate_full_extent:
	/* At this point, the current extent requires relocation.  
	 * We will try to allocate space equal to the size of the extent 
	 * being relocated first to try to relocate it without splitting.  
	 * If the allocation fails, we will try to allocate contiguous 
	 * blocks out of metadata zone.  If that allocation also fails, 
	 * then we will take a whatever contiguous block run is returned 
	 * by the allocation, split the extent into two parts, and then 
	 * relocate the first splitted extent. 
	 */
	alloc_flags = HFS_ALLOC_FORCECONTIG | HFS_ALLOC_SKIPFREEBLKS; 
	if (extent_info->is_sysfile) {
		alloc_flags |= HFS_ALLOC_METAZONE;
	}

	error = BlockAllocate(hfsmp, 1, oldBlockCount, oldBlockCount, alloc_flags, 
			&newStartBlock, &newBlockCount);
	if ((extent_info->is_sysfile == false) && 
	    ((error == dskFulErr) || (error == ENOSPC))) {
		/* For non-system files, try reallocating space in metadata zone */
		alloc_flags |= HFS_ALLOC_METAZONE;
		error = BlockAllocate(hfsmp, 1, oldBlockCount, oldBlockCount, 
				alloc_flags, &newStartBlock, &newBlockCount);
	} 
	if ((error == dskFulErr) || (error == ENOSPC)) {
		/* We did not find desired contiguous space for this extent.  
		 * So try to allocate the maximum contiguous space available.
		 */
		alloc_flags &= ~HFS_ALLOC_FORCECONTIG;

		error = BlockAllocate(hfsmp, 1, oldBlockCount, oldBlockCount, 
				alloc_flags, &newStartBlock, &newBlockCount);
		if (error) {
			printf ("hfs_reclaim_extent: fileID=%u start=%u, %u:(%u,%u) BlockAllocate error=%d\n", extent_info->fileID, extent_info->recStartBlock, index, oldStartBlock, oldBlockCount, error);
			goto out;
		}
		blocks_allocated = true;

		/* The number of blocks allocated is less than the requested 
		 * number of blocks.  For btree extents, check and trim the 
		 * extent to be multiple of the node size. 
		 */
		if (extent_info->is_sysfile) {
			node_size = get_btree_nodesize(extent_info->vp);
			if (node_size > hfsmp->blockSize) {
				remainder_blocks = newBlockCount % (node_size / hfsmp->blockSize);
				if (remainder_blocks) {
					roundedBlockCount = newBlockCount - remainder_blocks;
					/* Free tail-end blocks of the newly allocated extent */
					BlockDeallocate(hfsmp, newStartBlock + roundedBlockCount,
							       newBlockCount - roundedBlockCount,
							       HFS_ALLOC_SKIPFREEBLKS);
					newBlockCount = roundedBlockCount;
					if (hfs_resize_debug) {
						printf ("hfs_reclaim_extent: Fixing extent block count, node_blks=%u, old=%u, new=%u\n", node_size/hfsmp->blockSize, newBlockCount + remainder_blocks, newBlockCount);
					}
					if (newBlockCount == 0) {
						printf ("hfs_reclaim_extent: Not enough contiguous blocks available to relocate fileID=%d\n", extent_info->fileID);
						error = ENOSPC;
						goto out;
					}
				}
			}
		}

		/* The number of blocks allocated is less than the number of 
		 * blocks requested, so split this extent --- the first extent 
		 * will be relocated as part of this function call and the caller
		 * will handle relocating the second extent by calling this 
		 * function again for the second extent. 
		 */
		error = hfs_split_extent(extent_info, newBlockCount);
		if (error) {
			printf ("hfs_reclaim_extent: fileID=%u start=%u, %u:(%u,%u) split error=%d\n", extent_info->fileID, extent_info->recStartBlock, index, oldStartBlock, oldBlockCount, error);
			goto out;
		}
		oldBlockCount = newBlockCount;
	}
	if (error) {
		printf ("hfs_reclaim_extent: fileID=%u start=%u, %u:(%u,%u) contig BlockAllocate error=%d\n", extent_info->fileID, extent_info->recStartBlock, index, oldStartBlock, oldBlockCount, error);
		goto out;
	}
	blocks_allocated = true;

	/* Copy data from old location to new location */
	error = hfs_copy_extent(hfsmp, extent_info->vp, oldStartBlock, 
			newStartBlock, newBlockCount, context);
	if (error) {
		printf ("hfs_reclaim_extent: fileID=%u start=%u, %u:(%u,%u)=>(%u,%u) hfs_copy_extent error=%d\n", extent_info->fileID, extent_info->recStartBlock, index, oldStartBlock, oldBlockCount, newStartBlock, newBlockCount, error);
		goto out;
	}

	/* Update the extent record with the new start block information */
	extent_info->extents[index].startBlock = newStartBlock;

	/* Sync the content back to the disk */
	if (extent_info->catalog_fp) {
		/* Update the extents in catalog record */
		if (extent_info->is_dirlink) {
			error = cat_update_dirlink(hfsmp, extent_info->forkType, 
					extent_info->dirlink_desc, extent_info->dirlink_attr, 
					&(extent_info->dirlink_fork->ff_data));
		} else {
			cp->c_flag |= C_MODIFIED;
			/* If this is a system file, sync volume headers on disk */
			if (extent_info->is_sysfile) {
				error = hfs_flushvolumeheader(hfsmp, MNT_WAIT, HFS_ALTFLUSH);
			}
		}
	} else {
		/* Replace record for extents overflow or extents-based xattrs */
		error = BTReplaceRecord(extent_info->fcb, extent_info->iterator, 
				&(extent_info->btdata), extent_info->recordlen);
	}
	if (error) {
		printf ("hfs_reclaim_extent: fileID=%u, update record error=%u\n", extent_info->fileID, error);
		goto out;
	}

	/* Deallocate the old extent */
	error = BlockDeallocate(hfsmp, oldStartBlock, oldBlockCount, HFS_ALLOC_SKIPFREEBLKS);
	if (error) {
		printf ("hfs_reclaim_extent: fileID=%u start=%u, %u:(%u,%u) BlockDeallocate error=%d\n", extent_info->fileID, extent_info->recStartBlock, index, oldStartBlock, oldBlockCount, error);
		goto out;
	}
	extent_info->blocks_relocated += newBlockCount;
>>>>>>> origin/10.7

	microtime(&tv);

	hfs_lock_mount (hfsmp);

	MarkVCBDirty(hfsmp);
	hfsmp->hfs_mtime = tv.tv_sec;

<<<<<<< HEAD
	switch (op) {
	case VOL_UPDATE:
		break;
	case VOL_MKDIR:
		if (hfsmp->hfs_dircount != 0xFFFFFFFF)
			++hfsmp->hfs_dircount;
		if (inroot && hfsmp->vcbNmRtDirs != 0xFFFF)
			++hfsmp->vcbNmRtDirs;
		break;
	case VOL_RMDIR:
		if (hfsmp->hfs_dircount != 0)
			--hfsmp->hfs_dircount;
		if (inroot && hfsmp->vcbNmRtDirs != 0xFFFF)
			--hfsmp->vcbNmRtDirs;
		break;
	case VOL_MKFILE:
		if (hfsmp->hfs_filecount != 0xFFFFFFFF)
			++hfsmp->hfs_filecount;
		if (inroot && hfsmp->vcbNmFls != 0xFFFF)
			++hfsmp->vcbNmFls;
		break;
	case VOL_RMFILE:
		if (hfsmp->hfs_filecount != 0)
			--hfsmp->hfs_filecount;
		if (inroot && hfsmp->vcbNmFls != 0xFFFF)
			--hfsmp->vcbNmFls;
		break;
=======
	/* For a non-system file, if an extent entry from catalog record 
	 * was modified, sync the in-memory changes to the catalog record
	 * on disk before ending the transaction.
	 */
	 if ((extent_info->catalog_fp) && 
	     (extent_info->is_sysfile == false)) {
		(void) hfs_update(extent_info->vp, MNT_WAIT);
>>>>>>> origin/10.7
	}
=======

static int
hfs_relocate_callback(__unused HFSPlusExtentKey *key, HFSPlusExtentRecord *record, HFSPlusExtentRecord *state)
{
	bcopy(state, record, sizeof(HFSPlusExtentRecord));
	return 0;
}

/*
 * Reclaim space at the end of a volume, used by a given file.
 *
 * This routine attempts to move any extent which contains allocation blocks
 * at or after "startblk."  A separate transaction is used to do the move.
 * The contents of any moved extents are read and written via the volume's
 * device vnode -- NOT via "vp."  During the move, moved blocks which are part
 * of a transaction have their physical block numbers invalidated so they will
 * eventually be written to their new locations.
 *
 * Inputs:
 *    hfsmp       The volume being resized.
 *    startblk    Blocks >= this allocation block need to be moved.
 *    locks       Which locks need to be taken for the given system file.
 *    vp          The vnode for the system file.
 *
 *    The caller of this function, hfs_reclaimspace(), grabs cnode lock 
 *    for non-system files before calling this function.  
 *
 * Outputs:
 *    blks_moved  Total number of allocation blocks moved by this routine.
 */
static int
hfs_reclaim_file(struct hfsmount *hfsmp, struct vnode *vp, u_long startblk, int locks, u_int32_t *blks_moved, vfs_context_t context)
{
	int error;
	int lockflags;
	int i;
	u_long datablks;
	u_long end_block;
	u_int32_t oldStartBlock;
	u_int32_t newStartBlock;
	u_int32_t oldBlockCount;
	u_int32_t newBlockCount;
	struct filefork *fp;
	struct cnode *cp;
	int is_sysfile;
	int took_truncate_lock = 0;
	struct BTreeIterator *iterator = NULL;
	u_int8_t forktype;
	u_int32_t fileID;
	u_int32_t alloc_flags;
		
	/* If there is no vnode for this file, then there's nothing to do. */	
	if (vp == NULL)
		return 0;

	cp = VTOC(vp);
	fileID = cp->c_cnid;
	is_sysfile = vnode_issystem(vp);
	forktype = VNODE_IS_RSRC(vp) ? 0xFF : 0;

	/* Flush all the buffer cache blocks and cluster pages associated with 
	 * this vnode.  
	 *
	 * If the current vnode is a system vnode, all the buffer cache blocks 
	 * associated with it should already be sync'ed to the disk as part of 
	 * journal flush in hfs_truncatefs().  Normally there should not be 
	 * buffer cache blocks for regular files, but for objects like symlinks,
	 * we can have buffer cache blocks associated with the vnode.  Therefore
	 * we call buf_flushdirtyblks() always.  Resource fork data for directory 
	 * hard links are directly written using buffer cache for device vnode, 
	 * which should also be sync'ed as part of journal flush in hfs_truncatefs().
	 * 
	 * Flushing cluster pages should be the normal case for regular files, 
	 * and really should not do anything for system files.  But just to be 
	 * sure that all blocks associated with this vnode is sync'ed to the 
	 * disk, we call both buffer cache and cluster layer functions.  
	 */
	buf_flushdirtyblks(vp, MNT_NOWAIT, 0, "hfs_reclaim_file");
	
	if (!is_sysfile) {
		/* The caller grabs cnode lock for non-system files only, therefore 
		 * we unlock only non-system files before calling cluster layer.
		 */
		hfs_unlock(cp);
		hfs_lock_truncate(cp, TRUE);
		took_truncate_lock = 1;
	}
	(void) cluster_push(vp, 0);
	if (!is_sysfile) {
		error = hfs_lock(cp, HFS_FORCE_LOCK);
		if (error) {
			hfs_unlock_truncate(cp, TRUE);
			return error;
		}

		/* If the file no longer exists, nothing left to do */
		if (cp->c_flag & C_NOEXISTS) {
			hfs_unlock_truncate(cp, TRUE);
			return 0;
		}
	}

	/* Wait for any in-progress writes to this vnode to complete, so that we'll
	 * be copying consistent bits.  (Otherwise, it's possible that an async
	 * write will complete to the old extent after we read from it.  That
	 * could lead to corruption.)
	 */
	error = vnode_waitforwrites(vp, 0, 0, 0, "hfs_reclaim_file");
	if (error) {
		printf("hfs_reclaim_file: Error %d from vnode_waitforwrites\n", error);
		return error;
	}

	if (hfs_resize_debug) {
		printf("hfs_reclaim_file: Start relocating %sfork for fileid=%u name=%.*s\n", (forktype ? "rsrc" : "data"), fileID, cp->c_desc.cd_namelen, cp->c_desc.cd_nameptr);
	}

	/* We always need the allocation bitmap and extents B-tree */
	locks |= SFL_BITMAP | SFL_EXTENTS;
	
	error = hfs_start_transaction(hfsmp);
	if (error) {
		printf("hfs_reclaim_file: hfs_start_transaction returned %d\n", error);
		if (took_truncate_lock) {
			hfs_unlock_truncate(cp, TRUE);
		}
		return error;
	}
	lockflags = hfs_systemfile_lock(hfsmp, locks, HFS_EXCLUSIVE_LOCK);
	fp = VTOF(vp);
	datablks = 0;
	*blks_moved = 0;

	/* Relocate non-overflow extents */
	for (i = 0; i < kHFSPlusExtentDensity; ++i) {
		if (fp->ff_extents[i].blockCount == 0)
			break;
		oldStartBlock = fp->ff_extents[i].startBlock;
		oldBlockCount = fp->ff_extents[i].blockCount;
		datablks += oldBlockCount;
		end_block = oldStartBlock + oldBlockCount;
		/* Check if the file overlaps the target space */
		if (end_block > startblk) {
			alloc_flags = HFS_ALLOC_FORCECONTIG | HFS_ALLOC_SKIPFREEBLKS; 
			if (is_sysfile) {
				alloc_flags |= HFS_ALLOC_METAZONE;
			}
			error = BlockAllocate(hfsmp, 1, oldBlockCount, oldBlockCount, alloc_flags, &newStartBlock, &newBlockCount);
			if (error) {
				if (!is_sysfile && ((error == dskFulErr) || (error == ENOSPC))) {
					/* Try allocating again using the metadata zone */
					alloc_flags |= HFS_ALLOC_METAZONE;
					error = BlockAllocate(hfsmp, 1, oldBlockCount, oldBlockCount, alloc_flags, &newStartBlock, &newBlockCount);
				}
				if (error) {
					printf("hfs_reclaim_file: BlockAllocate(metazone) (error=%d) for fileID=%u %u:(%u,%u)\n", error, fileID, i, oldStartBlock, oldBlockCount);
					goto fail;
				} else {
					if (hfs_resize_debug) {
						printf("hfs_reclaim_file: BlockAllocate(metazone) success for fileID=%u %u:(%u,%u)\n", fileID, i, newStartBlock, newBlockCount);
					}
				}
			}

			/* Copy data from old location to new location */
			error = hfs_copy_extent(hfsmp, vp, oldStartBlock, newStartBlock, newBlockCount, context);
			if (error) {
				printf("hfs_reclaim_file: hfs_copy_extent error=%d for fileID=%u %u:(%u,%u) to %u:(%u,%u)\n", error, fileID, i, oldStartBlock, oldBlockCount, i, newStartBlock, newBlockCount);
				if (BlockDeallocate(hfsmp, newStartBlock, newBlockCount, HFS_ALLOC_SKIPFREEBLKS)) {
					hfs_mark_volume_inconsistent(hfsmp);
				}
				goto fail;
			}
			fp->ff_extents[i].startBlock = newStartBlock;
			cp->c_flag |= C_MODIFIED;
			*blks_moved += newBlockCount;

			/* Deallocate the old extent */
			error = BlockDeallocate(hfsmp, oldStartBlock, oldBlockCount, HFS_ALLOC_SKIPFREEBLKS);
			if (error) {
				printf("hfs_reclaim_file: BlockDeallocate returned %d\n", error);
				hfs_mark_volume_inconsistent(hfsmp);
				goto fail;
			}

			/* If this is a system file, sync the volume header on disk */
			if (is_sysfile) {
				error = hfs_flushvolumeheader(hfsmp, MNT_WAIT, HFS_ALTFLUSH);
				if (error) {
					printf("hfs_reclaim_file: hfs_flushvolumeheader returned %d\n", error);
					hfs_mark_volume_inconsistent(hfsmp);
					goto fail;
				}
			}

			if (hfs_resize_debug) {
				printf ("hfs_reclaim_file: Relocated %u:(%u,%u) to %u:(%u,%u)\n", i, oldStartBlock, oldBlockCount, i, newStartBlock, newBlockCount);
			}
		}
	}

	/* Relocate overflow extents (if any) */
	if (i == kHFSPlusExtentDensity && fp->ff_blocks > datablks) {
		struct FSBufferDescriptor btdata;
		HFSPlusExtentRecord record;
		HFSPlusExtentKey *key;
		FCB *fcb;
		int overflow_count = 0;

		if (kmem_alloc(kernel_map, (vm_offset_t*) &iterator, sizeof(*iterator))) {
			printf("hfs_reclaim_file: kmem_alloc failed!\n");
			error = ENOMEM;
			goto fail;
		}
>>>>>>> origin/10.6

	hfs_unlock_mount (hfsmp);

<<<<<<< HEAD
	if (hfsmp->jnl) {
		hfs_flushvolumeheader(hfsmp, 0);
	}

	return (0);
=======
		error = BTSearchRecord(fcb, iterator, &btdata, NULL, iterator);
		while (error == 0) {
			/* Stop when we encounter a different file or fork. */
			if ((key->fileID != fileID) || 
			    (key->forkType != forktype)) {
				break;
			}
		
			/* Just track the overflow extent record number for debugging... */
			if (hfs_resize_debug) {
				overflow_count++;
			}

			/* 
			 * Check if the file overlaps target space.
			 */
			for (i = 0; i < kHFSPlusExtentDensity; ++i) {
				if (record[i].blockCount == 0) {
					goto fail;
				}
				oldStartBlock = record[i].startBlock;
				oldBlockCount = record[i].blockCount;
				end_block = oldStartBlock + oldBlockCount;
				if (end_block > startblk) {
					alloc_flags = HFS_ALLOC_FORCECONTIG | HFS_ALLOC_SKIPFREEBLKS; 
					if (is_sysfile) {
						alloc_flags |= HFS_ALLOC_METAZONE;
					}
					error = BlockAllocate(hfsmp, 1, oldBlockCount, oldBlockCount, alloc_flags, &newStartBlock, &newBlockCount);
					if (error) {
						if (!is_sysfile && ((error == dskFulErr) || (error == ENOSPC))) {
							/* Try allocating again using the metadata zone */
							alloc_flags |= HFS_ALLOC_METAZONE; 
							error = BlockAllocate(hfsmp, 1, oldBlockCount, oldBlockCount, alloc_flags, &newStartBlock, &newBlockCount);
						} 
						if (error) {
							printf("hfs_reclaim_file: BlockAllocate(metazone) (error=%d) for fileID=%u %u:(%u,%u)\n", error, fileID, i, oldStartBlock, oldBlockCount);
							goto fail;
						} else {
							if (hfs_resize_debug) {
								printf("hfs_reclaim_file: BlockAllocate(metazone) success for fileID=%u %u:(%u,%u)\n", fileID, i, newStartBlock, newBlockCount);
							}
						}
					}
					error = hfs_copy_extent(hfsmp, vp, oldStartBlock, newStartBlock, newBlockCount, context);
					if (error) {
						printf("hfs_reclaim_file: hfs_copy_extent error=%d for fileID=%u (%u,%u) to (%u,%u)\n", error, fileID, oldStartBlock, oldBlockCount, newStartBlock, newBlockCount);
						if (BlockDeallocate(hfsmp, newStartBlock, newBlockCount, HFS_ALLOC_SKIPFREEBLKS)) {
							hfs_mark_volume_inconsistent(hfsmp);
						}
						goto fail;
					}
					record[i].startBlock = newStartBlock;
					cp->c_flag |= C_MODIFIED;
					*blks_moved += newBlockCount;

					/*
					 * NOTE: To support relocating overflow extents of the
					 * allocation file, we must update the BTree record BEFORE
					 * deallocating the old extent so that BlockDeallocate will
					 * use the extent's new location to calculate physical block
					 * numbers.  (This is for the case where the old extent's
					 * bitmap bits actually reside in the extent being moved.)
					 */
					error = BTUpdateRecord(fcb, iterator, (IterateCallBackProcPtr) hfs_relocate_callback, &record);
					if (error) {
						printf("hfs_reclaim_file: BTUpdateRecord returned %d\n", error);
						hfs_mark_volume_inconsistent(hfsmp);
						goto fail;
					}
					error = BlockDeallocate(hfsmp, oldStartBlock, oldBlockCount, HFS_ALLOC_SKIPFREEBLKS);
					if (error) {
						printf("hfs_reclaim_file: BlockDeallocate returned %d\n", error);
						hfs_mark_volume_inconsistent(hfsmp);
						goto fail;
					}
					if (hfs_resize_debug) {
						printf ("hfs_reclaim_file: Relocated overflow#%d %u:(%u,%u) to %u:(%u,%u)\n", overflow_count, i, oldStartBlock, oldBlockCount, i, newStartBlock, newBlockCount);
					}
				}
			}
			/* Look for more records. */
			error = BTIterateRecord(fcb, kBTreeNextRecord, iterator, &btdata, NULL);
			if (error == btNotFound) {
				error = 0;
				break;
			}
		}
	}
	
fail:
	if (iterator) {
		kmem_free(kernel_map, (vm_offset_t)iterator, sizeof(*iterator));
	}

	(void) hfs_systemfile_unlock(hfsmp, lockflags);

	if ((*blks_moved != 0) && (is_sysfile == false)) {
		(void) hfs_update(vp, MNT_WAIT);
	}

	(void) hfs_end_transaction(hfsmp);

	if (took_truncate_lock) {
		hfs_unlock_truncate(cp, TRUE);
	}

	if (hfs_resize_debug) {
		printf("hfs_reclaim_file: Finished relocating %sfork for fileid=%u (error=%d)\n", (forktype ? "rsrc" : "data"), fileID, error);
	}

	return error;
>>>>>>> origin/10.6
}


#if CONFIG_HFS_STD
/* HFS Standard MDB flush */
static int
hfs_flushMDB(struct hfsmount *hfsmp, int waitfor, int altflush)
{
<<<<<<< HEAD
	ExtendedVCB *vcb = HFSTOVCB(hfsmp);
	struct filefork *fp;
	HFSMasterDirectoryBlock	*mdb;
	struct buf *bp = NULL;
	int retval;
	int sector_size;
	ByteCount namelen;

	sector_size = hfsmp->hfs_logical_block_size;
	retval = (int)buf_bread(hfsmp->hfs_devvp, (daddr64_t)HFS_PRI_SECTOR(sector_size), sector_size, NOCRED, &bp);
	if (retval) {
		if (bp)
			buf_brelse(bp);
		return retval;
=======
	int error;
	struct hfs_journal_relocate_args *args = _args;
	struct hfsmount *hfsmp = args->hfsmp;
	buf_t bp;
	JournalInfoBlock *jibp;

	error = buf_meta_bread(hfsmp->hfs_devvp,
		hfsmp->vcbJinfoBlock * (hfsmp->blockSize/hfsmp->hfs_logical_block_size),
		hfsmp->blockSize, vfs_context_ucred(args->context), &bp);
	if (error) {
		printf("hfs_reclaim_journal_file: failed to read JIB (%d)\n", error);
		return error;
	}
	jibp = (JournalInfoBlock*) buf_dataptr(bp);
	jibp->offset = SWAP_BE64((u_int64_t)args->newStartBlock * hfsmp->blockSize);
	jibp->size = SWAP_BE64(hfsmp->jnl_size);
	if (journal_uses_fua(hfsmp->jnl))
		buf_markfua(bp);
	error = buf_bwrite(bp);
	if (error) {
		printf("hfs_reclaim_journal_file: failed to write JIB (%d)\n", error);
		return error;
	}
	if (!journal_uses_fua(hfsmp->jnl)) {
		error = VNOP_IOCTL(hfsmp->hfs_devvp, DKIOCSYNCHRONIZECACHE, NULL, FWRITE, args->context);
		if (error) {
			printf("hfs_reclaim_journal_file: DKIOCSYNCHRONIZECACHE failed (%d)\n", error);
			error = 0;		/* Don't fail the operation. */
		}
>>>>>>> origin/10.5
	}

	hfs_lock_mount (hfsmp);

	mdb = (HFSMasterDirectoryBlock *)(buf_dataptr(bp) + HFS_PRI_OFFSET(sector_size));
    
	mdb->drCrDate	= SWAP_BE32 (UTCToLocal(to_hfs_time(vcb->hfs_itime)));
	mdb->drLsMod	= SWAP_BE32 (UTCToLocal(to_hfs_time(vcb->vcbLsMod)));
	mdb->drAtrb	= SWAP_BE16 (vcb->vcbAtrb);
	mdb->drNmFls	= SWAP_BE16 (vcb->vcbNmFls);
	mdb->drAllocPtr	= SWAP_BE16 (vcb->nextAllocation);
	mdb->drClpSiz	= SWAP_BE32 (vcb->vcbClpSiz);
	mdb->drNxtCNID	= SWAP_BE32 (vcb->vcbNxtCNID);
	mdb->drFreeBks	= SWAP_BE16 (vcb->freeBlocks);

<<<<<<< HEAD
	namelen = strlen((char *)vcb->vcbVN);
	retval = utf8_to_hfs(vcb, namelen, vcb->vcbVN, mdb->drVN);
	/* Retry with MacRoman in case that's how it was exported. */
	if (retval)
		retval = utf8_to_mac_roman(namelen, vcb->vcbVN, mdb->drVN);
	
	mdb->drVolBkUp	= SWAP_BE32 (UTCToLocal(to_hfs_time(vcb->vcbVolBkUp)));
	mdb->drWrCnt	= SWAP_BE32 (vcb->vcbWrCnt);
	mdb->drNmRtDirs	= SWAP_BE16 (vcb->vcbNmRtDirs);
	mdb->drFilCnt	= SWAP_BE32 (vcb->vcbFilCnt);
	mdb->drDirCnt	= SWAP_BE32 (vcb->vcbDirCnt);
	
	bcopy(vcb->vcbFndrInfo, mdb->drFndrInfo, sizeof(mdb->drFndrInfo));
=======
static int
hfs_reclaim_journal_file(struct hfsmount *hfsmp, vfs_context_t context)
{
	int error;
	int lockflags;
	u_int32_t oldStartBlock;
	u_int32_t newStartBlock;
	u_int32_t oldBlockCount;
	u_int32_t newBlockCount;
	struct cat_desc journal_desc;
	struct cat_attr journal_attr;
	struct cat_fork journal_fork;
	struct hfs_journal_relocate_args callback_args;

	error = hfs_start_transaction(hfsmp);
	if (error) {
		printf("hfs_reclaim_journal_file: hfs_start_transaction returned %d\n", error);
		return error;
	}
	lockflags = hfs_systemfile_lock(hfsmp, SFL_CATALOG | SFL_BITMAP, HFS_EXCLUSIVE_LOCK);
	
	oldBlockCount = hfsmp->jnl_size / hfsmp->blockSize;
	
	/* TODO: Allow the journal to change size based on the new volume size. */
	error = BlockAllocate(hfsmp, 1, oldBlockCount, oldBlockCount, 
			HFS_ALLOC_METAZONE | HFS_ALLOC_FORCECONTIG | HFS_ALLOC_SKIPFREEBLKS, 
			 &newStartBlock, &newBlockCount);
	if (error) {
		printf("hfs_reclaim_journal_file: BlockAllocate returned %d\n", error);
		goto fail;
	}
	if (newBlockCount != oldBlockCount) {
		printf("hfs_reclaim_journal_file: newBlockCount != oldBlockCount (%u, %u)\n", newBlockCount, oldBlockCount);
		goto free_fail;
	}
	
<<<<<<< HEAD
	error = BlockDeallocate(hfsmp, hfsmp->jnl_start, oldBlockCount, HFS_ALLOC_SKIPFREEBLKS);
=======
	error = cat_idlookup(hfsmp, hfsmp->hfs_jnlfileid, 1, 0, &journal_desc, &journal_attr, &journal_fork);
>>>>>>> origin/10.8
	if (error) {
		printf("hfs_reclaim_journal_file: BlockDeallocate returned %d\n", error);
		goto free_fail;
	}
>>>>>>> origin/10.6

<<<<<<< HEAD
	fp = VTOF(vcb->extentsRefNum);
	mdb->drXTExtRec[0].startBlock = SWAP_BE16 (fp->ff_extents[0].startBlock);
	mdb->drXTExtRec[0].blockCount = SWAP_BE16 (fp->ff_extents[0].blockCount);
	mdb->drXTExtRec[1].startBlock = SWAP_BE16 (fp->ff_extents[1].startBlock);
	mdb->drXTExtRec[1].blockCount = SWAP_BE16 (fp->ff_extents[1].blockCount);
	mdb->drXTExtRec[2].startBlock = SWAP_BE16 (fp->ff_extents[2].startBlock);
	mdb->drXTExtRec[2].blockCount = SWAP_BE16 (fp->ff_extents[2].blockCount);
	mdb->drXTFlSize	= SWAP_BE32 (fp->ff_blocks * vcb->blockSize);
	mdb->drXTClpSiz	= SWAP_BE32 (fp->ff_clumpsize);
	FTOC(fp)->c_flag &= ~C_MODIFIED;
=======
	/* Update the catalog record for .journal */
	error = cat_idlookup(hfsmp, hfsmp->hfs_jnlfileid, 1, &journal_desc, &journal_attr, &journal_fork);
	if (error) {
		printf("hfs_reclaim_journal_file: cat_idlookup returned %d\n", error);
		goto free_fail;
	}
	oldStartBlock = journal_fork.cf_extents[0].startBlock;
	journal_fork.cf_size = newBlockCount * hfsmp->blockSize;
	journal_fork.cf_extents[0].startBlock = newStartBlock;
	journal_fork.cf_extents[0].blockCount = newBlockCount;
	journal_fork.cf_blocks = newBlockCount;
	error = cat_update(hfsmp, &journal_desc, &journal_attr, &journal_fork, NULL);
	cat_releasedesc(&journal_desc);  /* all done with cat descriptor */
	if (error) {
		printf("hfs_reclaim_journal_file: cat_update returned %d\n", error);
		goto free_fail;
	}
	callback_args.hfsmp = hfsmp;
	callback_args.context = context;
	callback_args.newStartBlock = newStartBlock;
>>>>>>> origin/10.5
	
	fp = VTOF(vcb->catalogRefNum);
	mdb->drCTExtRec[0].startBlock = SWAP_BE16 (fp->ff_extents[0].startBlock);
	mdb->drCTExtRec[0].blockCount = SWAP_BE16 (fp->ff_extents[0].blockCount);
	mdb->drCTExtRec[1].startBlock = SWAP_BE16 (fp->ff_extents[1].startBlock);
	mdb->drCTExtRec[1].blockCount = SWAP_BE16 (fp->ff_extents[1].blockCount);
	mdb->drCTExtRec[2].startBlock = SWAP_BE16 (fp->ff_extents[2].startBlock);
	mdb->drCTExtRec[2].blockCount = SWAP_BE16 (fp->ff_extents[2].blockCount);
	mdb->drCTFlSize	= SWAP_BE32 (fp->ff_blocks * vcb->blockSize);
	mdb->drCTClpSiz	= SWAP_BE32 (fp->ff_clumpsize);
	FTOC(fp)->c_flag &= ~C_MODIFIED;

	MarkVCBClean( vcb );

	hfs_unlock_mount (hfsmp);

	/* If requested, flush out the alternate MDB */
	if (altflush) {
		struct buf *alt_bp = NULL;

		if (buf_meta_bread(hfsmp->hfs_devvp, hfsmp->hfs_partition_avh_sector, sector_size, NOCRED, &alt_bp) == 0) {
			bcopy(mdb, (char *)buf_dataptr(alt_bp) + HFS_ALT_OFFSET(sector_size), kMDBSize);

<<<<<<< HEAD
			(void) VNOP_BWRITE(alt_bp);
=======
		if (meta_bread(hfsmp->hfs_devvp, altIDSector, sectorsize, NOCRED, &alt_bp) == 0) {
			bcopy(mdb, alt_bp->b_data + HFS_ALT_OFFSET(sectorsize), kMDBSize);

			(void) VOP_BWRITE(alt_bp);
>>>>>>> origin/10.2
		} else if (alt_bp)
			buf_brelse(alt_bp);
	}
<<<<<<< HEAD

	if (waitfor != MNT_WAIT)
<<<<<<< HEAD
		buf_bawrite(bp);
	else 
		retval = VNOP_BWRITE(bp);
=======
		bawrite(bp);
	else 
		retval = VOP_BWRITE(bp);
 
	MarkVCBClean( vcb );
>>>>>>> origin/10.2
=======
	
	if (!error && hfs_resize_debug) {
		printf ("hfs_reclaim_journal_file: Successfully relocated journal from (%u,%u) to (%u,%u)\n", oldStartBlock, oldBlockCount, newStartBlock, newBlockCount);
	}
	return error;

free_fail:
	(void) BlockDeallocate(hfsmp, newStartBlock, newBlockCount, HFS_ALLOC_SKIPFREEBLKS);
fail:
	hfs_systemfile_unlock(hfsmp, lockflags);
	(void) hfs_end_transaction(hfsmp);
	if (hfs_resize_debug) {
		printf ("hfs_reclaim_journal_file: Error relocating journal file (error=%d)\n", error);
	}
	return error;
}
>>>>>>> origin/10.6

	return (retval);
}
#endif

/*
 *  Flush any dirty in-memory mount data to the on-disk
 *  volume header.
 *
 *  Note: the on-disk volume signature is intentionally
 *  not flushed since the on-disk "H+" and "HX" signatures
 *  are always stored in-memory as "H+".
 */
int
hfs_flushvolumeheader(struct hfsmount *hfsmp, 
					  hfs_flush_volume_header_options_t options)
{
<<<<<<< HEAD
	ExtendedVCB *vcb = HFSTOVCB(hfsmp);
	struct filefork *fp;
	HFSPlusVolumeHeader *volumeHeader, *altVH;
	int retval;
	struct buf *bp, *alt_bp;
	int i;
	daddr64_t priIDSector;
	bool critical = false;
	u_int16_t  signature;
	u_int16_t  hfsversion;
	daddr64_t avh_sector;
	bool altflush = ISSET(options, HFS_FVH_WRITE_ALT);

	if (ISSET(options, HFS_FVH_FLUSH_IF_DIRTY)
		&& !hfs_header_needs_flushing(hfsmp)) {
		return 0;
	}

	if (hfsmp->hfs_flags & HFS_READ_ONLY) {
		return(0);
=======
	int error;
	int lockflags;
	u_int32_t oldBlock;
	u_int32_t newBlock;
	u_int32_t blockCount;
	struct cat_desc jib_desc;
	struct cat_attr jib_attr;
	struct cat_fork jib_fork;
	buf_t old_bp, new_bp;
	
	error = hfs_start_transaction(hfsmp);
	if (error) {
		printf("hfs_reclaim_journal_info_block: hfs_start_transaction returned %d\n", error);
		return error;
	}
	lockflags = hfs_systemfile_lock(hfsmp, SFL_CATALOG | SFL_BITMAP, HFS_EXCLUSIVE_LOCK);
	
	error = BlockAllocate(hfsmp, 1, 1, 1, 
			HFS_ALLOC_METAZONE | HFS_ALLOC_FORCECONTIG | HFS_ALLOC_SKIPFREEBLKS, 
			&newBlock, &blockCount);
	if (error) {
		printf("hfs_reclaim_journal_info_block: BlockAllocate returned %d\n", error);
		goto fail;
>>>>>>> origin/10.6
	}
#if CONFIG_HFS_STD
	if (hfsmp->hfs_flags & HFS_STANDARD) {
		return hfs_flushMDB(hfsmp, ISSET(options, HFS_FVH_WAIT) ? MNT_WAIT : 0, altflush);
	}
<<<<<<< HEAD
#endif
	priIDSector = (daddr64_t)((vcb->hfsPlusIOPosOffset / hfsmp->hfs_logical_block_size) +
				  HFS_PRI_SECTOR(hfsmp->hfs_logical_block_size));

	if (hfs_start_transaction(hfsmp) != 0) {
	    return EINVAL;
=======
	error = BlockDeallocate(hfsmp, hfsmp->vcbJinfoBlock, 1, HFS_ALLOC_SKIPFREEBLKS);
	if (error) {
		printf("hfs_reclaim_journal_info_block: BlockDeallocate returned %d\n", error);
		goto free_fail;
>>>>>>> origin/10.6
	}
<<<<<<< HEAD

	bp = NULL;
	alt_bp = NULL;

	retval = (int)buf_meta_bread(hfsmp->hfs_devvp, 
			HFS_PHYSBLK_ROUNDDOWN(priIDSector, hfsmp->hfs_log_per_phys),
			hfsmp->hfs_physical_block_size, NOCRED, &bp);
	if (retval) {
		printf("hfs: err %d reading VH blk (vol=%s)\n", retval, vcb->vcbVN);
		goto err_exit;
=======
	
	/* Copy the old journal info block content to the new location */
	error = buf_meta_bread(hfsmp->hfs_devvp,
		hfsmp->vcbJinfoBlock * (hfsmp->blockSize/hfsmp->hfs_logical_block_size),
		hfsmp->blockSize, vfs_context_ucred(context), &old_bp);
	if (error) {
		printf("hfs_reclaim_journal_info_block: failed to read JIB (%d)\n", error);
		goto free_fail;
	}
	new_bp = buf_getblk(hfsmp->hfs_devvp,
		newBlock * (hfsmp->blockSize/hfsmp->hfs_logical_block_size),
		hfsmp->blockSize, 0, 0, BLK_META);
	bcopy((char*)buf_dataptr(old_bp), (char*)buf_dataptr(new_bp), hfsmp->blockSize);
	buf_brelse(old_bp);
	if (journal_uses_fua(hfsmp->jnl))
		buf_markfua(new_bp);
	error = buf_bwrite(new_bp);
	if (error) {
		printf("hfs_reclaim_journal_info_block: failed to write new JIB (%d)\n", error);
		goto free_fail;
>>>>>>> origin/10.5
	}

	volumeHeader = (HFSPlusVolumeHeader *)((char *)buf_dataptr(bp) + 
			HFS_PRI_OFFSET(hfsmp->hfs_physical_block_size));

	/*
	 * Sanity check what we just read.  If it's bad, try the alternate
	 * instead.
	 */
	signature = SWAP_BE16 (volumeHeader->signature);
	hfsversion   = SWAP_BE16 (volumeHeader->version);
	if ((signature != kHFSPlusSigWord && signature != kHFSXSigWord) ||
	    (hfsversion < kHFSPlusVersion) || (hfsversion > 100) ||
	    (SWAP_BE32 (volumeHeader->blockSize) != vcb->blockSize)) {
		printf("hfs: corrupt VH on %s, sig 0x%04x, ver %d, blksize %d\n",
			       	vcb->vcbVN, signature, hfsversion, 
				SWAP_BE32 (volumeHeader->blockSize));
		hfs_mark_inconsistent(hfsmp, HFS_INCONSISTENCY_DETECTED);

		/* Almost always we read AVH relative to the partition size */
		avh_sector = hfsmp->hfs_partition_avh_sector;

		if (hfsmp->hfs_partition_avh_sector != hfsmp->hfs_fs_avh_sector) {
			/* 
			 * The two altVH offsets do not match --- which means that a smaller file 
			 * system exists in a larger partition.  Verify that we have the correct 
			 * alternate volume header sector as per the current parititon size.  
			 * The GPT device that we are mounted on top could have changed sizes 
			 * without us knowing. 
			 *
			 * We're in a transaction, so it's safe to modify the partition_avh_sector 
			 * field if necessary.
			 */

			uint64_t sector_count;

			/* Get underlying device block count */
			if ((retval = VNOP_IOCTL(hfsmp->hfs_devvp, DKIOCGETBLOCKCOUNT, 
							(caddr_t)&sector_count, 0, vfs_context_current()))) {
				printf("hfs_flushVH: err %d getting block count (%s) \n", retval, vcb->vcbVN);
				retval = ENXIO;
				goto err_exit;
			}
			
			/* Partition size was changed without our knowledge */
			if (sector_count != (uint64_t)hfsmp->hfs_logical_block_count) {
				hfsmp->hfs_partition_avh_sector = (hfsmp->hfsPlusIOPosOffset / hfsmp->hfs_logical_block_size) + 
					HFS_ALT_SECTOR(hfsmp->hfs_logical_block_size, sector_count);
				/* Note: hfs_fs_avh_sector will remain unchanged */
				printf ("hfs_flushVH: partition size changed, partition_avh_sector=%qu, fs_avh_sector=%qu\n",
						hfsmp->hfs_partition_avh_sector, hfsmp->hfs_fs_avh_sector);

				/* 
				 * We just updated the offset for AVH relative to 
				 * the partition size, so the content of that AVH
				 * will be invalid.  But since we are also maintaining 
				 * a valid AVH relative to the file system size, we 
				 * can read it since primary VH and partition AVH 
				 * are not valid. 
				 */
				avh_sector = hfsmp->hfs_fs_avh_sector;
			}
		}

		printf ("hfs: trying alternate (for %s) avh_sector=%qu\n", 
				(avh_sector == hfsmp->hfs_fs_avh_sector) ? "file system" : "partition", avh_sector);

<<<<<<< HEAD
		if (avh_sector) {
			retval = buf_meta_bread(hfsmp->hfs_devvp, 
			    HFS_PHYSBLK_ROUNDDOWN(avh_sector, hfsmp->hfs_log_per_phys),
			    hfsmp->hfs_physical_block_size, NOCRED, &alt_bp);
			if (retval) {
				printf("hfs: err %d reading alternate VH (%s)\n", retval, vcb->vcbVN);
				goto err_exit;
			}
			
			altVH = (HFSPlusVolumeHeader *)((char *)buf_dataptr(alt_bp) + 
				HFS_ALT_OFFSET(hfsmp->hfs_physical_block_size));
			signature = SWAP_BE16(altVH->signature);
			hfsversion = SWAP_BE16(altVH->version);
			
			if ((signature != kHFSPlusSigWord && signature != kHFSXSigWord) ||
			    (hfsversion < kHFSPlusVersion) || (kHFSPlusVersion > 100) ||
			    (SWAP_BE32(altVH->blockSize) != vcb->blockSize)) {
				printf("hfs: corrupt alternate VH on %s, sig 0x%04x, ver %d, blksize %d\n",
				    vcb->vcbVN, signature, hfsversion,
				    SWAP_BE32(altVH->blockSize));
				retval = EIO;
				goto err_exit;
			}
			
			/* The alternate is plausible, so use it. */
			bcopy(altVH, volumeHeader, kMDBSize);
			buf_brelse(alt_bp);
			alt_bp = NULL;
		} else {
			/* No alternate VH, nothing more we can do. */
			retval = EIO;
			goto err_exit;
		}
=======
	// XXXdbg
	hfs_global_shared_lock_acquire(hfsmp);
	if (hfsmp->jnl) {
		if (journal_start_transaction(hfsmp->jnl) != 0) {
			hfs_global_shared_lock_release(hfsmp);
		    return EINVAL;
	    }
	}
<<<<<<< HEAD

	retval = meta_bread(hfsmp->hfs_devvp, priIDSector, sectorsize, NOCRED, &bp);
	if (retval) {
		if (bp)
			brelse(bp);

		if (hfsmp->jnl) {
			journal_end_transaction(hfsmp->jnl);
		}
		hfs_global_shared_lock_release(hfsmp);

		return (retval);
>>>>>>> origin/10.2
=======
	
	/* Update the catalog record for .journal_info_block */
	error = cat_idlookup(hfsmp, hfsmp->hfs_jnlinfoblkid, 1, 0, &jib_desc, &jib_attr, &jib_fork);
	if (error) {
		printf("hfs_reclaim_journal_file: cat_idlookup returned %d\n", error);
		goto fail;
	}
	oldBlock = jib_fork.cf_extents[0].startBlock;
	jib_fork.cf_size = hfsmp->blockSize;
	jib_fork.cf_extents[0].startBlock = newBlock;
	jib_fork.cf_extents[0].blockCount = 1;
	jib_fork.cf_blocks = 1;
	error = cat_update(hfsmp, &jib_desc, &jib_attr, &jib_fork, NULL);
	cat_releasedesc(&jib_desc);  /* all done with cat descriptor */
	if (error) {
		printf("hfs_reclaim_journal_info_block: cat_update returned %d\n", error);
		goto fail;
	}
	
	/* Update the pointer to the journal info block in the volume header. */
	hfsmp->vcbJinfoBlock = newBlock;
	error = hfs_flushvolumeheader(hfsmp, MNT_WAIT, HFS_ALTFLUSH);
	if (error) {
		printf("hfs_reclaim_journal_info_block: hfs_flushvolumeheader returned %d\n", error);
		goto fail;
>>>>>>> origin/10.5
	}

	if (hfsmp->jnl) {
		journal_modify_block_start(hfsmp->jnl, bp);
	}
<<<<<<< HEAD
=======

	volumeHeader = (HFSPlusVolumeHeader *)((char *)bp->b_data + HFS_PRI_OFFSET(sectorsize));
>>>>>>> origin/10.2

	/*
	 * For embedded HFS+ volumes, update create date if it changed
	 * (ie from a setattrlist call)
	 */
	if ((vcb->hfsPlusIOPosOffset != 0) &&
	    (SWAP_BE32 (volumeHeader->createDate) != vcb->localCreateDate)) {
		struct buf *bp2;
		HFSMasterDirectoryBlock	*mdb;

		retval = (int)buf_meta_bread(hfsmp->hfs_devvp, 
				HFS_PHYSBLK_ROUNDDOWN(HFS_PRI_SECTOR(hfsmp->hfs_logical_block_size), hfsmp->hfs_log_per_phys),
				hfsmp->hfs_physical_block_size, NOCRED, &bp2);
		if (retval) {
			if (bp2)
				buf_brelse(bp2);
			retval = 0;
		} else {
			mdb = (HFSMasterDirectoryBlock *)(buf_dataptr(bp2) +
				HFS_PRI_OFFSET(hfsmp->hfs_physical_block_size));

			if ( SWAP_BE32 (mdb->drCrDate) != vcb->localCreateDate )
			  {
<<<<<<< HEAD
=======
				// XXXdbg
>>>>>>> origin/10.2
				if (hfsmp->jnl) {
				    journal_modify_block_start(hfsmp->jnl, bp2);
				}

				mdb->drCrDate = SWAP_BE32 (vcb->localCreateDate);	/* pick up the new create date */

<<<<<<< HEAD
				if (hfsmp->jnl) {
					journal_modify_block_end(hfsmp->jnl, bp2, NULL, NULL);
				} else {
					(void) VNOP_BWRITE(bp2);		/* write out the changes */
=======
				// XXXdbg
				if (hfsmp->jnl) {
					journal_modify_block_end(hfsmp->jnl, bp2);
				} else {
					(void) VOP_BWRITE(bp2);		/* write out the changes */
>>>>>>> origin/10.2
				}
			  }
			else
			  {
				buf_brelse(bp2);						/* just release it */
			  }
		  }	
	}
<<<<<<< HEAD

<<<<<<< HEAD
	hfs_lock_mount (hfsmp);
=======

	if (!error && hfs_resize_debug) {
		printf ("hfs_reclaim_journal_info_block: Successfully relocated journal info block from (%u,%u) to (%u,%u)\n", oldBlock, blockCount, newBlock, blockCount);
	}
	return error;

free_fail:
	(void) BlockDeallocate(hfsmp, newBlock, blockCount, HFS_ALLOC_SKIPFREEBLKS);
fail:
	hfs_systemfile_unlock(hfsmp, lockflags);
	(void) hfs_end_transaction(hfsmp);
	if (hfs_resize_debug) {
		printf ("hfs_reclaim_journal_info_block: Error relocating journal info block (error=%d)\n", error);
	}
	return error;
}
>>>>>>> origin/10.6

	/* Note: only update the lower 16 bits worth of attributes */
	volumeHeader->attributes       = SWAP_BE32 (vcb->vcbAtrb);
	volumeHeader->journalInfoBlock = SWAP_BE32 (vcb->vcbJinfoBlock);
=======
// XXXdbg - only monkey around with the volume signature on non-root volumes
//
#if 0
	if (hfsmp->jnl &&
		hfsmp->hfs_fs_ronly == 0 &&
		(HFSTOVFS(hfsmp)->mnt_flag & MNT_ROOTFS) == 0) {
		
		int old_sig = volumeHeader->signature;

<<<<<<< HEAD
		if (vcb->vcbAtrb & kHFSVolumeUnmountedMask) {
			volumeHeader->signature = kHFSPlusSigWord;
		} else {
			volumeHeader->signature = kHFSJSigWord;
		}

		if (old_sig != volumeHeader->signature) {
			altflush = 1;
		}
	}
#endif
// XXXdbg

	/* Note: only update the lower 16 bits worth of attributes */
	volumeHeader->attributes	= SWAP_BE32 ((SWAP_BE32 (volumeHeader->attributes) & 0xFFFF0000) + (UInt16) vcb->vcbAtrb);
	volumeHeader->journalInfoBlock = SWAP_BE32(vcb->vcbJinfoBlock);
>>>>>>> origin/10.2
	if (hfsmp->jnl) {
		volumeHeader->lastMountedVersion = SWAP_BE32 (kHFSJMountVersion);
	} else {
		volumeHeader->lastMountedVersion = SWAP_BE32 (kHFSPlusMountVersion);
	}
	volumeHeader->createDate	= SWAP_BE32 (vcb->localCreateDate);  /* volume create date is in local time */
	volumeHeader->modifyDate	= SWAP_BE32 (to_hfs_time(vcb->vcbLsMod));
	volumeHeader->backupDate	= SWAP_BE32 (to_hfs_time(vcb->vcbVolBkUp));
	volumeHeader->fileCount		= SWAP_BE32 (vcb->vcbFilCnt);
	volumeHeader->folderCount	= SWAP_BE32 (vcb->vcbDirCnt);
	volumeHeader->totalBlocks	= SWAP_BE32 (vcb->totalBlocks);
	volumeHeader->freeBlocks	= SWAP_BE32 (vcb->freeBlocks + vcb->reclaimBlocks);
	volumeHeader->nextAllocation	= SWAP_BE32 (vcb->nextAllocation);
	volumeHeader->rsrcClumpSize	= SWAP_BE32 (vcb->vcbClpSiz);
	volumeHeader->dataClumpSize	= SWAP_BE32 (vcb->vcbClpSiz);
	volumeHeader->nextCatalogID	= SWAP_BE32 (vcb->vcbNxtCNID);
	volumeHeader->writeCount	= SWAP_BE32 (vcb->vcbWrCnt);
	volumeHeader->encodingsBitmap	= SWAP_BE64 (vcb->encodingsBitmap);

	if (bcmp(vcb->vcbFndrInfo, volumeHeader->finderInfo, sizeof(volumeHeader->finderInfo)) != 0) {
		bcopy(vcb->vcbFndrInfo, volumeHeader->finderInfo, sizeof(volumeHeader->finderInfo));
		critical = true;
	}

	if (!altflush && !ISSET(options, HFS_FVH_FLUSH_IF_DIRTY)) {
		goto done;
	}

	/* Sync Extents over-flow file meta data */
	fp = VTOF(vcb->extentsRefNum);
	if (FTOC(fp)->c_flag & C_MODIFIED) {
		for (i = 0; i < kHFSPlusExtentDensity; i++) {
			volumeHeader->extentsFile.extents[i].startBlock	=
				SWAP_BE32 (fp->ff_extents[i].startBlock);
			volumeHeader->extentsFile.extents[i].blockCount	=
				SWAP_BE32 (fp->ff_extents[i].blockCount);
		}
		volumeHeader->extentsFile.logicalSize = SWAP_BE64 (fp->ff_size);
		volumeHeader->extentsFile.totalBlocks = SWAP_BE32 (fp->ff_blocks);
		volumeHeader->extentsFile.clumpSize   = SWAP_BE32 (fp->ff_clumpsize);
		FTOC(fp)->c_flag &= ~C_MODIFIED;
		altflush = true;
	}
=======
/*
 * Reclaim space at the end of a file system.
 *
 * Inputs - 
 * 	startblk 	- start block of the space being reclaimed
 * 	reclaimblks 	- number of allocation blocks to reclaim
 */
static int
hfs_reclaimspace(struct hfsmount *hfsmp, u_int32_t startblk, u_int32_t reclaimblks, vfs_context_t context)
{
	struct vnode *vp = NULL;
	FCB *fcb;
	struct BTreeIterator * iterator = NULL;
	struct FSBufferDescriptor btdata;
	struct HFSPlusCatalogFile filerec;
	u_int32_t  saved_next_allocation;
	cnid_t * cnidbufp;
	size_t cnidbufsize;
	int filecnt = 0;
	int maxfilecnt;
	u_int32_t block;
	int lockflags;
	int i, j;
	int error;
	int lastprogress = 0;
	u_int32_t blks_moved = 0;
	u_int32_t total_blks_moved = 0;
	Boolean need_relocate;

	/* Relocate extents of the Allocation file if they're in the way. */
	error = hfs_reclaim_file(hfsmp, hfsmp->hfs_allocation_vp, startblk, SFL_BITMAP, &blks_moved, context);
	if (error) {
		printf("hfs_reclaimspace: reclaim allocation file returned %d\n", error);
		return error;
	}
	total_blks_moved += blks_moved;

	/* Relocate extents of the Extents B-tree if they're in the way. */
	error = hfs_reclaim_file(hfsmp, hfsmp->hfs_extents_vp, startblk, SFL_EXTENTS, &blks_moved, context);
	if (error) {
		printf("hfs_reclaimspace: reclaim extents b-tree returned %d\n", error);
		return error;
	}
	total_blks_moved += blks_moved;

	/* Relocate extents of the Catalog B-tree if they're in the way. */
	error = hfs_reclaim_file(hfsmp, hfsmp->hfs_catalog_vp, startblk, SFL_CATALOG, &blks_moved, context);
	if (error) {
		printf("hfs_reclaimspace: reclaim catalog b-tree returned %d\n", error);
		return error;
	}
	total_blks_moved += blks_moved;

	/* Relocate extents of the Attributes B-tree if they're in the way. */
	error = hfs_reclaim_file(hfsmp, hfsmp->hfs_attribute_vp, startblk, SFL_ATTRIBUTE, &blks_moved, context);
	if (error) {
		printf("hfs_reclaimspace: reclaim attribute b-tree returned %d\n", error);
		return error;
	}
	total_blks_moved += blks_moved;

	/* Relocate extents of the Startup File if there is one and they're in the way. */
	error = hfs_reclaim_file(hfsmp, hfsmp->hfs_startup_vp, startblk, SFL_STARTUP, &blks_moved, context);
	if (error) {
		printf("hfs_reclaimspace: reclaim startup file returned %d\n", error);
		return error;
	}
	total_blks_moved += blks_moved;
	
	/*
	 * We need to make sure the alternate volume header gets flushed if we moved
	 * any extents in the volume header.  But we need to do that before
	 * shrinking the size of the volume, or else the journal code will panic
	 * with an invalid (too large) block number.
	 *
	 * Note that total_blks_moved will be set if ANY extent was moved, even
	 * if it was just an overflow extent.  In this case, the journal_flush isn't
	 * strictly required, but shouldn't hurt.
	 */
	if (total_blks_moved) {
		hfs_journal_flush(hfsmp);
	}
>>>>>>> origin/10.6

	/* Sync Catalog file meta data */
	fp = VTOF(vcb->catalogRefNum);
	if (FTOC(fp)->c_flag & C_MODIFIED) {
		for (i = 0; i < kHFSPlusExtentDensity; i++) {
			volumeHeader->catalogFile.extents[i].startBlock	=
				SWAP_BE32 (fp->ff_extents[i].startBlock);
			volumeHeader->catalogFile.extents[i].blockCount	=
				SWAP_BE32 (fp->ff_extents[i].blockCount);
		}
		volumeHeader->catalogFile.logicalSize = SWAP_BE64 (fp->ff_size);
		volumeHeader->catalogFile.totalBlocks = SWAP_BE32 (fp->ff_blocks);
		volumeHeader->catalogFile.clumpSize   = SWAP_BE32 (fp->ff_clumpsize);
		FTOC(fp)->c_flag &= ~C_MODIFIED;
		altflush = true;
	}

	/* Sync Allocation file meta data */
	fp = VTOF(vcb->allocationsRefNum);
	if (FTOC(fp)->c_flag & C_MODIFIED) {
		for (i = 0; i < kHFSPlusExtentDensity; i++) {
			volumeHeader->allocationFile.extents[i].startBlock =
				SWAP_BE32 (fp->ff_extents[i].startBlock);
			volumeHeader->allocationFile.extents[i].blockCount =
				SWAP_BE32 (fp->ff_extents[i].blockCount);
		}
		volumeHeader->allocationFile.logicalSize = SWAP_BE64 (fp->ff_size);
		volumeHeader->allocationFile.totalBlocks = SWAP_BE32 (fp->ff_blocks);
		volumeHeader->allocationFile.clumpSize   = SWAP_BE32 (fp->ff_clumpsize);
		FTOC(fp)->c_flag &= ~C_MODIFIED;
		altflush = true;
	}

	/* Sync Attribute file meta data */
	if (hfsmp->hfs_attribute_vp) {
		fp = VTOF(hfsmp->hfs_attribute_vp);
		for (i = 0; i < kHFSPlusExtentDensity; i++) {
			volumeHeader->attributesFile.extents[i].startBlock =
				SWAP_BE32 (fp->ff_extents[i].startBlock);
			volumeHeader->attributesFile.extents[i].blockCount =
				SWAP_BE32 (fp->ff_extents[i].blockCount);
		}
		if (ISSET(FTOC(fp)->c_flag, C_MODIFIED)) {
			FTOC(fp)->c_flag &= ~C_MODIFIED;
			altflush = true;
		}
		volumeHeader->attributesFile.logicalSize = SWAP_BE64 (fp->ff_size);
		volumeHeader->attributesFile.totalBlocks = SWAP_BE32 (fp->ff_blocks);
		volumeHeader->attributesFile.clumpSize   = SWAP_BE32 (fp->ff_clumpsize);
	}

	/* Sync Startup file meta data */
	if (hfsmp->hfs_startup_vp) {
		fp = VTOF(hfsmp->hfs_startup_vp);
		if (FTOC(fp)->c_flag & C_MODIFIED) {
			for (i = 0; i < kHFSPlusExtentDensity; i++) {
				volumeHeader->startupFile.extents[i].startBlock =
					SWAP_BE32 (fp->ff_extents[i].startBlock);
				volumeHeader->startupFile.extents[i].blockCount =
					SWAP_BE32 (fp->ff_extents[i].blockCount);
			}
			volumeHeader->startupFile.logicalSize = SWAP_BE64 (fp->ff_size);
			volumeHeader->startupFile.totalBlocks = SWAP_BE32 (fp->ff_blocks);
			volumeHeader->startupFile.clumpSize   = SWAP_BE32 (fp->ff_clumpsize);
			FTOC(fp)->c_flag &= ~C_MODIFIED;
			altflush = true;
		}
	}

<<<<<<< HEAD
	if (altflush)
		critical = true;
 
done:
	MarkVCBClean(hfsmp);
	hfs_unlock_mount (hfsmp);

	/* If requested, flush out the alternate volume header */
	if (altflush) {
		/* 
		 * The two altVH offsets do not match --- which means that a smaller file 
		 * system exists in a larger partition.  Verify that we have the correct 
		 * alternate volume header sector as per the current parititon size.  
		 * The GPT device that we are mounted on top could have changed sizes 
		 * without us knowning. 
		 *
		 * We're in a transaction, so it's safe to modify the partition_avh_sector 
		 * field if necessary.
		 */
		if (hfsmp->hfs_partition_avh_sector != hfsmp->hfs_fs_avh_sector) {
			uint64_t sector_count;

			/* Get underlying device block count */
			if ((retval = VNOP_IOCTL(hfsmp->hfs_devvp, DKIOCGETBLOCKCOUNT, 
							(caddr_t)&sector_count, 0, vfs_context_current()))) {
				printf("hfs_flushVH: err %d getting block count (%s) \n", retval, vcb->vcbVN);
				retval = ENXIO;
				goto err_exit;
			}
			
			/* Partition size was changed without our knowledge */
			if (sector_count != (uint64_t)hfsmp->hfs_logical_block_count) {
				hfsmp->hfs_partition_avh_sector = (hfsmp->hfsPlusIOPosOffset / hfsmp->hfs_logical_block_size) + 
					HFS_ALT_SECTOR(hfsmp->hfs_logical_block_size, sector_count);
				/* Note: hfs_fs_avh_sector will remain unchanged */
				printf ("hfs_flushVH: altflush: partition size changed, partition_avh_sector=%qu, fs_avh_sector=%qu\n",
						hfsmp->hfs_partition_avh_sector, hfsmp->hfs_fs_avh_sector);
			}
		}

<<<<<<< HEAD
		/*
		 * First see if we need to write I/O to the "secondary" AVH 
		 * located at FS Size - 1024 bytes, because this one will 
		 * always go into the journal.  We put this AVH into the journal
		 * because even if the filesystem size has shrunk, this LBA should be 
		 * reachable after the partition-size modification has occurred.  
		 * The one where we need to be careful is partitionsize-1024, since the
		 * partition size should hopefully shrink. 
		 *
		 * Most of the time this block will not execute.
		 */
		if ((hfsmp->hfs_fs_avh_sector) && 
				(hfsmp->hfs_partition_avh_sector != hfsmp->hfs_fs_avh_sector)) {
			if (buf_meta_bread(hfsmp->hfs_devvp, 
						HFS_PHYSBLK_ROUNDDOWN(hfsmp->hfs_fs_avh_sector, hfsmp->hfs_log_per_phys),
						hfsmp->hfs_physical_block_size, NOCRED, &alt_bp) == 0) {
				if (hfsmp->jnl) {
					journal_modify_block_start(hfsmp->jnl, alt_bp);
				}

				bcopy(volumeHeader, (char *)buf_dataptr(alt_bp) + 
						HFS_ALT_OFFSET(hfsmp->hfs_physical_block_size), 
						kMDBSize);

				if (hfsmp->jnl) {
					journal_modify_block_end(hfsmp->jnl, alt_bp, NULL, NULL);
				} else {
					(void) VNOP_BWRITE(alt_bp);
				}
			} else if (alt_bp) {
				buf_brelse(alt_bp);
=======
	saved_next_allocation = hfsmp->nextAllocation;
	/* Always try allocating new blocks after the metadata zone */
	HFS_UPDATE_NEXT_ALLOCATION(hfsmp, hfsmp->hfs_metazone_start);

	fcb = VTOF(hfsmp->hfs_catalog_vp);
	bzero(iterator, sizeof(*iterator));

	btdata.bufferAddress = &filerec;
	btdata.itemSize = sizeof(filerec);
	btdata.itemCount = 1;

	/* Keep the Catalog and extents files locked during iteration. */
	lockflags = hfs_systemfile_lock(hfsmp, SFL_CATALOG | SFL_EXTENTS, HFS_SHARED_LOCK);

	error = BTIterateRecord(fcb, kBTreeFirstRecord, iterator, NULL, NULL);
	if (error) {
		goto end_iteration;
	}
	/*
	 * Iterate over all the catalog records looking for files
	 * that overlap into the space we're trying to free up and 
	 * the total number of blocks that will require relocation.
	 */
	for (filecnt = 0; filecnt < maxfilecnt; ) {
		error = BTIterateRecord(fcb, kBTreeNextRecord, iterator, &btdata, NULL);
		if (error) {
			if (error == fsBTRecordNotFoundErr || error == fsBTEndOfIterationErr) {
				error = 0;				
			}
			break;
		}
		if (filerec.recordType != kHFSPlusFileRecord) {
			continue;
		}

		need_relocate = false;
		/* Check if data fork overlaps the target space */
		for (i = 0; i < kHFSPlusExtentDensity; ++i) {
			if (filerec.dataFork.extents[i].blockCount == 0) {
				break;
			}
			block = filerec.dataFork.extents[i].startBlock +
				filerec.dataFork.extents[i].blockCount;
			if (block >= startblk) {
				if ((filerec.fileID == hfsmp->hfs_jnlfileid) ||
				    (filerec.fileID == hfsmp->hfs_jnlinfoblkid)) {
					printf("hfs_reclaimspace: cannot move active journal\n");
					error = EPERM;
					goto end_iteration;
				}
				need_relocate = true;
				goto save_fileid;
			}
		}

		/* Check if resource fork overlaps the target space */
		for (j = 0; j < kHFSPlusExtentDensity; ++j) {
			if (filerec.resourceFork.extents[j].blockCount == 0) {
				break;
			}
			block = filerec.resourceFork.extents[j].startBlock +
				filerec.resourceFork.extents[j].blockCount;
			if (block >= startblk) {
				need_relocate = true;
				goto save_fileid;
			}
		}

		/* Check if any forks' overflow extents overlap the target space */
		if ((i == kHFSPlusExtentDensity) || (j == kHFSPlusExtentDensity)) {
			if (hfs_overlapped_overflow_extents(hfsmp, startblk, filerec.fileID)) {
				need_relocate = true;
				goto save_fileid;
			}
		}

save_fileid:
		if (need_relocate == true) {
			cnidbufp[filecnt++] = filerec.fileID;
			if (hfs_resize_debug) {
				printf ("hfs_reclaimspace: Will relocate extents for fileID=%u\n", filerec.fileID);
>>>>>>> origin/10.6
			}
		}
<<<<<<< HEAD
		
		/* 
		 * Flush out alternate volume header located at 1024 bytes before
		 * end of the partition as part of journal transaction.  In 
		 * most cases, this will be the only alternate volume header 
		 * that we need to worry about because the file system size is 
		 * same as the partition size, therefore hfs_fs_avh_sector is 
		 * same as hfs_partition_avh_sector. This is the "priority" AVH. 
		 *
		 * However, do not always put this I/O into the journal.  If we skipped the
		 * FS-Size AVH write above, then we will put this I/O into the journal as 
		 * that indicates the two were in sync.  However, if the FS size is
		 * not the same as the partition size, we are tracking two.  We don't
		 * put it in the journal in that case, since if the partition
		 * size changes between uptimes, and we need to replay the journal,
		 * this I/O could generate an EIO if during replay it is now trying 
		 * to access blocks beyond the device EOF.  
		 */
		if (hfsmp->hfs_partition_avh_sector) {
			if (buf_meta_bread(hfsmp->hfs_devvp, 
						HFS_PHYSBLK_ROUNDDOWN(hfsmp->hfs_partition_avh_sector, hfsmp->hfs_log_per_phys),
						hfsmp->hfs_physical_block_size, NOCRED, &alt_bp) == 0) {

				/* only one AVH, put this I/O in the journal. */
				if ((hfsmp->jnl) && (hfsmp->hfs_partition_avh_sector == hfsmp->hfs_fs_avh_sector)) {
					journal_modify_block_start(hfsmp->jnl, alt_bp);
				}
=======
	}

end_iteration:
	/* If no regular file was found to be relocated and 
	 * no system file was moved, we probably do not have 
	 * enough space to relocate the system files, or 
	 * something else went wrong.
	 */
	if ((filecnt == 0) && (total_blks_moved == 0)) {
		printf("hfs_reclaimspace: no files moved\n");
		error = ENOSPC;
	}
	/* All done with catalog. */
	hfs_systemfile_unlock(hfsmp, lockflags);
	if (error || filecnt == 0)
		goto out;

<<<<<<< HEAD
	/*
	 * Double check space requirements to make sure
	 * there is enough space to relocate any files
	 * that reside in the reclaim area.
	 *
	 *                                          Blocks To Move --------------
	 *                                                            |    |    |
	 *                                                            V    V    V
	 * ------------------------------------------------------------------------
	 * |                                                        | /   ///  // |
	 * |                                                        | /   ///  // |
	 * |                                                        | /   ///  // |
	 * ------------------------------------------------------------------------
	 *
	 * <------------------- New Total Blocks ------------------><-- Reclaim -->
	 *
	 * <------------------------ Original Total Blocks ----------------------->
	 *
	 */
	if (blkstomove >= hfs_freeblks(hfsmp, 1)) {
		printf("hfs_truncatefs: insufficient space (need %lu blocks; have %u blocks)\n", blkstomove, hfs_freeblks(hfsmp, 1));
		error = ENOSPC;
		goto out;
	}
=======
>>>>>>> origin/10.6
	hfsmp->hfs_resize_filesmoved = 0;
	hfsmp->hfs_resize_totalfiles = filecnt;
	
	/* Now move any files that are in the way. */
	for (i = 0; i < filecnt; ++i) {
<<<<<<< HEAD
		struct vnode * rvp;
        struct cnode * cp;
>>>>>>> origin/10.5

				bcopy(volumeHeader, (char *)buf_dataptr(alt_bp) + 
						HFS_ALT_OFFSET(hfsmp->hfs_physical_block_size), 
						kMDBSize);

<<<<<<< HEAD
				/* If journaled and we only have one AVH to track */
				if ((hfsmp->jnl) && (hfsmp->hfs_partition_avh_sector == hfsmp->hfs_fs_avh_sector)) {
					journal_modify_block_end (hfsmp->jnl, alt_bp, NULL, NULL);
				} else {
					/* 
					 * If we don't have a journal or there are two AVH's at the
					 * moment, then this one doesn't go in the journal.  Note that 
					 * this one may generate I/O errors, since the partition
					 * can be resized behind our backs at any moment and this I/O 
					 * may now appear to be beyond the device EOF.
					 */
					(void) VNOP_BWRITE(alt_bp);
					hfs_flush(hfsmp, HFS_FLUSH_CACHE);
				}		
			} else if (alt_bp) {
				buf_brelse(alt_bp);
			}
=======
        /* Relocating directory hard links is not supported, so we
         * punt (see radar 6217026). */
        cp = VTOC(vp);
        if ((cp->c_flag & C_HARDLINK) && vnode_isdir(vp)) {
            printf("hfs_reclaimspace: unable to relocate directory hard link %d\n", cp->c_cnid);
            error = EINVAL;
            goto out;
        }

		/* Relocate any data fork blocks. */
		if (VTOF(vp) && VTOF(vp)->ff_blocks > 0) {
			error = hfs_relocate(vp, hfsmp->hfs_metazone_end + 1, kauth_cred_get(), current_proc());
>>>>>>> origin/10.5
		}
	}

<<<<<<< HEAD
	/* Finish modifying the block for the primary VH */
	if (hfsmp->jnl) {
		journal_modify_block_end(hfsmp->jnl, bp, NULL, NULL);
	} else {
		if (!ISSET(options, HFS_FVH_WAIT)) {
			buf_bawrite(bp);
		} else {
			retval = VNOP_BWRITE(bp);
			/* When critical data changes, flush the device cache */
			if (critical && (retval == 0)) {
				hfs_flush(hfsmp, HFS_FLUSH_CACHE);
			}
		}
	}
	hfs_end_transaction(hfsmp);
=======
		if (meta_bread(hfsmp->hfs_devvp, altIDSector, sectorsize, NOCRED, &alt_bp) == 0) {
			if (hfsmp->jnl) {
				journal_modify_block_start(hfsmp->jnl, alt_bp);
			}
=======
		/* Relocate any resource fork blocks. */
		if ((cp->c_blocks - (VTOF(vp) ? VTOF((vp))->ff_blocks : 0)) > 0) {
			error = hfs_vgetrsrc(hfsmp, vp, &rvp, TRUE);
			if (error)
=======
		struct vnode *rvp;
		struct cnode *cp;
		struct filefork *datafork;

		if (hfs_vget(hfsmp, cnidbufp[i], &vp, 0) != 0)
			continue;
		
		cp = VTOC(vp);
		datafork = VTOF(vp);

		/* Relocating directory hard links is not supported, so we punt (see radar 6217026). */
		if ((cp->c_flag & C_HARDLINK) && vnode_isdir(vp)) {
			printf("hfs_reclaimspace: Unable to relocate directory hard link id=%d\n", cp->c_cnid);
			error = EINVAL;
		       	goto out;
		}

		/* Relocate any overlapping data fork blocks. */
		if (datafork && datafork->ff_blocks > 0) {
			error = hfs_reclaim_file(hfsmp, vp, startblk, 0, &blks_moved, context);
			if (error)  {
				printf ("hfs_reclaimspace: Error reclaiming datafork blocks of fileid=%u (error=%d)\n", cnidbufp[i], error);
				break;
			}
			total_blks_moved += blks_moved;
		}

		/* Relocate any overlapping resource fork blocks. */
		if ((cp->c_blocks - (datafork ? datafork->ff_blocks : 0)) > 0) {
			error = hfs_vgetrsrc(hfsmp, vp, &rvp, TRUE, TRUE);
			if (error) {
				printf ("hfs_reclaimspace: Error looking up rvp for fileid=%u (error=%d)\n", cnidbufp[i], error);
>>>>>>> origin/10.6
				break;
			}
			error = hfs_reclaim_file(hfsmp, rvp, startblk, 0, &blks_moved, context);
			VTOC(rvp)->c_flag |= C_NEED_RVNODE_PUT;
			if (error) {
				printf ("hfs_reclaimspace: Error reclaiming rsrcfork blocks of fileid=%u (error=%d)\n", cnidbufp[i], error);
				break;
			}
			total_blks_moved += blks_moved;
		}
		hfs_unlock(cp);
		vnode_put(vp);
		vp = NULL;

		++hfsmp->hfs_resize_filesmoved;
>>>>>>> origin/10.5

			bcopy(volumeHeader, alt_bp->b_data + HFS_ALT_OFFSET(sectorsize), kMDBSize);

<<<<<<< HEAD
			if (hfsmp->jnl) {
				journal_modify_block_end(hfsmp->jnl, alt_bp);
			} else {
				(void) VOP_BWRITE(alt_bp);
			}
<<<<<<< HEAD
		} else if (alt_bp)
			brelse(alt_bp);
	}

	// XXXdbg
	if (hfsmp->jnl) {
		journal_modify_block_end(hfsmp->jnl, bp);
		journal_end_transaction(hfsmp->jnl);
	} else {
		if (waitfor != MNT_WAIT)
			bawrite(bp);
		else {
		    retval = VOP_BWRITE(bp);
		    /* When critical data changes, flush the device cache */
		    if (critical && (retval == 0)) {
			(void) VOP_IOCTL(hfsmp->hfs_devvp, DKIOCSYNCHRONIZECACHE,
					 NULL, FWRITE, NOCRED, current_proc());
		    }
		}
=======
		}
	}
	if (vp) {
		hfs_unlock(VTOC(vp));
		vnode_put(vp);
		vp = NULL;
=======
	/* Just to be safe, sync the content of the journal to the disk before we proceed */
	hfs_journal_flush(hfsmp, TRUE);

	/* First, relocate journal file blocks if they're in the way.  
	 * Doing this first will make sure that journal relocate code 
	 * gets access to contiguous blocks on disk first.  The journal
	 * file has to be contiguous on the disk, otherwise resize will 
	 * fail. 
	 */
	error = hfs_reclaim_journal_file(hfsmp, allocLimit, context);
	if (error) {
		printf("hfs_reclaimspace: hfs_reclaim_journal_file failed (%d)\n", error);
		return error;
	}
	
	/* Relocate journal info block blocks if they're in the way. */
	error = hfs_reclaim_journal_info_block(hfsmp, allocLimit, context);
	if (error) {
		printf("hfs_reclaimspace: hfs_reclaim_journal_info_block failed (%d)\n", error);
		return error;
	}

	/* Relocate extents of the Extents B-tree if they're in the way.
	 * Relocating extents btree before other btrees is important as 
	 * this will provide access to largest contiguous block range on 
	 * the disk for relocating extents btree.  Note that extents btree 
	 * can only have maximum of 8 extents.
	 */
	error = hfs_reclaim_file(hfsmp, hfsmp->hfs_extents_vp, kHFSExtentsFileID, 
			kHFSDataForkType, allocLimit, context);
	if (error) {
		printf("hfs_reclaimspace: reclaim extents b-tree returned %d\n", error);
		return error;
	}

	/* Relocate extents of the Allocation file if they're in the way. */
	error = hfs_reclaim_file(hfsmp, hfsmp->hfs_allocation_vp, kHFSAllocationFileID, 
			kHFSDataForkType, allocLimit, context);
	if (error) {
		printf("hfs_reclaimspace: reclaim allocation file returned %d\n", error);
		return error;
	}

	/* Relocate extents of the Catalog B-tree if they're in the way. */
	error = hfs_reclaim_file(hfsmp, hfsmp->hfs_catalog_vp, kHFSCatalogFileID, 
			kHFSDataForkType, allocLimit, context);
	if (error) {
		printf("hfs_reclaimspace: reclaim catalog b-tree returned %d\n", error);
		return error;
>>>>>>> origin/10.7
	}
	if (hfsmp->hfs_resize_filesmoved != 0) {
		printf("hfs_reclaimspace: relocated %u blocks from %d files on \"%s\"\n",
			total_blks_moved, (int)hfsmp->hfs_resize_filesmoved, hfsmp->vcbVN);
	}
out:
	kmem_free(kernel_map, (vm_offset_t)iterator, sizeof(*iterator));
	kmem_free(kernel_map, (vm_offset_t)cnidbufp, cnidbufsize);

	/*
	 * Restore the roving allocation pointer on errors.
	 * (but only if we didn't move any files)
	 */
<<<<<<< HEAD
	if (error && hfsmp->hfs_resize_filesmoved == 0) {
		HFS_UPDATE_NEXT_ALLOCATION(hfsmp, saved_next_allocation);
	}
	return (error);
=======
	if (hfsmp->hfs_resize_blocksmoved) {
		hfs_journal_flush(hfsmp, TRUE);
	}

	/* Reclaim extents from catalog file records */
	error = hfs_reclaim_filespace(hfsmp, allocLimit, context);
	if (error) {
		printf ("hfs_reclaimspace: hfs_reclaim_filespace returned error=%d\n", error);
		return error;
	}

	/* Reclaim extents from extent-based extended attributes, if any */
	error = hfs_reclaim_xattrspace(hfsmp, allocLimit, context);
	if (error) {
		printf ("hfs_reclaimspace: hfs_reclaim_xattrspace returned error=%d\n", error);
		return error;
	}

	return error;
>>>>>>> origin/10.7
}


/*
 * Check if there are any overflow data or resource fork extents that overlap 
 * into the disk space that is being reclaimed.  
 *
 * Output - 
 * 	1 - One of the overflow extents need to be relocated
 * 	0 - No overflow extents need to be relocated, or there was an error
 */
static int
hfs_overlapped_overflow_extents(struct hfsmount *hfsmp, u_int32_t startblk, u_int32_t fileID)
{
	struct BTreeIterator * iterator = NULL;
	struct FSBufferDescriptor btdata;
	HFSPlusExtentRecord extrec;
	HFSPlusExtentKey *extkeyptr;
	FCB *fcb;
	int overlapped = 0;
	int i;
	int error;

	if (kmem_alloc(kernel_map, (vm_offset_t *)&iterator, sizeof(*iterator))) {
		return 0;
	}	
	bzero(iterator, sizeof(*iterator));
	extkeyptr = (HFSPlusExtentKey *)&iterator->key;
	extkeyptr->keyLength = kHFSPlusExtentKeyMaximumLength;
	extkeyptr->forkType = 0;
	extkeyptr->fileID = fileID;
	extkeyptr->startBlock = 0;

	btdata.bufferAddress = &extrec;
	btdata.itemSize = sizeof(extrec);
	btdata.itemCount = 1;
	
	fcb = VTOF(hfsmp->hfs_extents_vp);

	/* This will position the iterator just before the first overflow 
	 * extent record for given fileID.  It will always return btNotFound, 
	 * so we special case the error code.
	 */
	error = BTSearchRecord(fcb, iterator, &btdata, NULL, iterator);
	if (error && (error != btNotFound)) {
		goto out;
	}

	/* BTIterateRecord() might return error if the btree is empty, and 
	 * therefore we return that the extent does not overflow to the caller
	 */
	error = BTIterateRecord(fcb, kBTreeNextRecord, iterator, &btdata, NULL);
	while (error == 0) {
		/* Stop when we encounter a different file. */
		if (extkeyptr->fileID != fileID) {
			break;
		}
		/* Check if any of the forks exist in the target space. */
		for (i = 0; i < kHFSPlusExtentDensity; ++i) {
			if (extrec[i].blockCount == 0) {
				break;
			}
			if ((extrec[i].startBlock + extrec[i].blockCount) >= startblk) {
				overlapped = 1;
				goto out;
			}
		}
		/* Look for more records. */
		error = BTIterateRecord(fcb, kBTreeNextRecord, iterator, &btdata, NULL);
	}

out:
	kmem_free(kernel_map, (vm_offset_t)iterator, sizeof(*iterator));
	return overlapped;
}


/*
 * Calculate the progress of a file system resize operation.
 */
__private_extern__
int
hfs_resize_progress(struct hfsmount *hfsmp, u_int32_t *progress)
{
	if ((hfsmp->hfs_flags & HFS_RESIZE_IN_PROGRESS) == 0) {
		return (ENXIO);
>>>>>>> origin/10.6
	}
	hfs_global_shared_lock_release(hfsmp);
>>>>>>> origin/10.2
 
	return (retval);

err_exit:
	if (alt_bp)
		buf_brelse(alt_bp);
	if (bp)
		buf_brelse(bp);
	hfs_end_transaction(hfsmp);
	return retval;
}


/*
 * Creates a UUID from a unique "name" in the HFS UUID Name space.
 * See version 3 UUID.
 */
static void
hfs_getvoluuid(struct hfsmount *hfsmp, uuid_t result)
{
	MD5_CTX  md5c;
	uint8_t  rawUUID[8];

	((uint32_t *)rawUUID)[0] = hfsmp->vcbFndrInfo[6];
	((uint32_t *)rawUUID)[1] = hfsmp->vcbFndrInfo[7];

	MD5Init( &md5c );
	MD5Update( &md5c, HFS_UUID_NAMESPACE_ID, sizeof( uuid_t ) );
	MD5Update( &md5c, rawUUID, sizeof (rawUUID) );
	MD5Final( result, &md5c );

	result[6] = 0x30 | ( result[6] & 0x0F );
	result[8] = 0x80 | ( result[8] & 0x3F );
}

/*
 * Get file system attributes.
 */
static int
hfs_vfs_getattr(struct mount *mp, struct vfs_attr *fsap, __unused vfs_context_t context)
{
#define HFS_ATTR_CMN_VALIDMASK ATTR_CMN_VALIDMASK
#define HFS_ATTR_FILE_VALIDMASK (ATTR_FILE_VALIDMASK & ~(ATTR_FILE_FILETYPE | ATTR_FILE_FORKCOUNT | ATTR_FILE_FORKLIST))
#define HFS_ATTR_CMN_VOL_VALIDMASK (ATTR_CMN_VALIDMASK & ~(ATTR_CMN_ACCTIME))

	ExtendedVCB *vcb = VFSTOVCB(mp);
	struct hfsmount *hfsmp = VFSTOHFS(mp);
	u_int32_t freeCNIDs;

	int searchfs_on = 0;
	int exchangedata_on = 1;

#if CONFIG_SEARCHFS
	searchfs_on = 1;
#endif

#if CONFIG_PROTECT
	if (cp_fs_protected(mp)) {
		exchangedata_on = 0;
	}
<<<<<<< HEAD
#endif
=======
	return (0);
}


/*
 * Go through the disk queues to initiate sandbagged IO;
 * go through the inodes to write those that have been modified;
 * initiate the writing of the super block if it has been modified.
 *
 * Note: we are always called with the filesystem marked `MPBUSY'.
 */
static int hfs_sync(mp, waitfor, cred, p)
struct mount *mp;
int waitfor;
struct ucred *cred;
struct proc *p;
{
    struct vnode 		*nvp, *vp;
    struct hfsnode 		*hp;
    struct hfsmount		*hfsmp = VFSTOHFS(mp);
    ExtendedVCB			*vcb;
    struct vnode 		*meta_vp[3];
    int i;
    int error, allerror = 0;
>>>>>>> origin/10.1

	freeCNIDs = (u_int32_t)0xFFFFFFFF - (u_int32_t)hfsmp->vcbNxtCNID;

	VFSATTR_RETURN(fsap, f_objcount, (u_int64_t)hfsmp->vcbFilCnt + (u_int64_t)hfsmp->vcbDirCnt);
	VFSATTR_RETURN(fsap, f_filecount, (u_int64_t)hfsmp->vcbFilCnt);
	VFSATTR_RETURN(fsap, f_dircount, (u_int64_t)hfsmp->vcbDirCnt);
	VFSATTR_RETURN(fsap, f_maxobjcount, (u_int64_t)0xFFFFFFFF);
	VFSATTR_RETURN(fsap, f_iosize, (size_t)cluster_max_io_size(mp, 0));
	VFSATTR_RETURN(fsap, f_blocks, (u_int64_t)hfsmp->totalBlocks);
	VFSATTR_RETURN(fsap, f_bfree, (u_int64_t)hfs_freeblks(hfsmp, 0));
	VFSATTR_RETURN(fsap, f_bavail, (u_int64_t)hfs_freeblks(hfsmp, 1));
	VFSATTR_RETURN(fsap, f_bsize, (u_int32_t)vcb->blockSize);
	/* XXX needs clarification */
	VFSATTR_RETURN(fsap, f_bused, hfsmp->totalBlocks - hfs_freeblks(hfsmp, 1));
	/* Maximum files is constrained by total blocks. */
	VFSATTR_RETURN(fsap, f_files, (u_int64_t)(hfsmp->totalBlocks - 2));
	VFSATTR_RETURN(fsap, f_ffree, MIN((u_int64_t)freeCNIDs, (u_int64_t)hfs_freeblks(hfsmp, 1)));

	fsap->f_fsid.val[0] = hfsmp->hfs_raw_dev;
	fsap->f_fsid.val[1] = vfs_typenum(mp);
	VFSATTR_SET_SUPPORTED(fsap, f_fsid);

	VFSATTR_RETURN(fsap, f_signature, vcb->vcbSigWord);
	VFSATTR_RETURN(fsap, f_carbon_fsid, 0);

	if (VFSATTR_IS_ACTIVE(fsap, f_capabilities)) {
		vol_capabilities_attr_t *cap;
	
		cap = &fsap->f_capabilities;

		if ((hfsmp->hfs_flags & HFS_STANDARD) == 0) {
			/* HFS+ & variants */
			cap->capabilities[VOL_CAPABILITIES_FORMAT] =
				VOL_CAP_FMT_PERSISTENTOBJECTIDS |
				VOL_CAP_FMT_SYMBOLICLINKS |
				VOL_CAP_FMT_HARDLINKS |
				VOL_CAP_FMT_JOURNAL |
				VOL_CAP_FMT_ZERO_RUNS |
				(hfsmp->jnl ? VOL_CAP_FMT_JOURNAL_ACTIVE : 0) |
				(hfsmp->hfs_flags & HFS_CASE_SENSITIVE ? VOL_CAP_FMT_CASE_SENSITIVE : 0) |
				VOL_CAP_FMT_CASE_PRESERVING |
				VOL_CAP_FMT_FAST_STATFS | 
				VOL_CAP_FMT_2TB_FILESIZE |
				VOL_CAP_FMT_HIDDEN_FILES |
#if HFS_COMPRESSION
				VOL_CAP_FMT_PATH_FROM_ID |
				VOL_CAP_FMT_DECMPFS_COMPRESSION;
#else
				VOL_CAP_FMT_PATH_FROM_ID;
#endif
		}
#if CONFIG_HFS_STD
		else {
			/* HFS standard */
			cap->capabilities[VOL_CAPABILITIES_FORMAT] =
				VOL_CAP_FMT_PERSISTENTOBJECTIDS |
				VOL_CAP_FMT_CASE_PRESERVING |
				VOL_CAP_FMT_FAST_STATFS |
				VOL_CAP_FMT_HIDDEN_FILES |
				VOL_CAP_FMT_PATH_FROM_ID;
		}
#endif

		/*
		 * The capabilities word in 'cap' tell you whether or not 
		 * this particular filesystem instance has feature X enabled.
		 */

		cap->capabilities[VOL_CAPABILITIES_INTERFACES] =
			VOL_CAP_INT_ATTRLIST |
			VOL_CAP_INT_NFSEXPORT |
			VOL_CAP_INT_READDIRATTR |
			VOL_CAP_INT_ALLOCATE |
			VOL_CAP_INT_VOL_RENAME |
			VOL_CAP_INT_ADVLOCK |
			VOL_CAP_INT_FLOCK |
#if NAMEDSTREAMS
			VOL_CAP_INT_EXTENDED_ATTR |
			VOL_CAP_INT_NAMEDSTREAMS;
#else
			VOL_CAP_INT_EXTENDED_ATTR;
#endif
		
		/* HFS may conditionally support searchfs and exchangedata depending on the runtime */

		if (searchfs_on) {
			cap->capabilities[VOL_CAPABILITIES_INTERFACES] |= VOL_CAP_INT_SEARCHFS;
		}
		if (exchangedata_on) {
			cap->capabilities[VOL_CAPABILITIES_INTERFACES] |= VOL_CAP_INT_EXCHANGEDATA;
		}

<<<<<<< HEAD
		cap->capabilities[VOL_CAPABILITIES_RESERVED1] = 0;
		cap->capabilities[VOL_CAPABILITIES_RESERVED2] = 0;

		cap->valid[VOL_CAPABILITIES_FORMAT] =
			VOL_CAP_FMT_PERSISTENTOBJECTIDS |
			VOL_CAP_FMT_SYMBOLICLINKS |
			VOL_CAP_FMT_HARDLINKS |
			VOL_CAP_FMT_JOURNAL |
			VOL_CAP_FMT_JOURNAL_ACTIVE |
			VOL_CAP_FMT_NO_ROOT_TIMES |
			VOL_CAP_FMT_SPARSE_FILES |
			VOL_CAP_FMT_ZERO_RUNS |
			VOL_CAP_FMT_CASE_SENSITIVE |
			VOL_CAP_FMT_CASE_PRESERVING |
			VOL_CAP_FMT_FAST_STATFS |
			VOL_CAP_FMT_2TB_FILESIZE |
			VOL_CAP_FMT_OPENDENYMODES |
			VOL_CAP_FMT_HIDDEN_FILES |
#if HFS_COMPRESSION
			VOL_CAP_FMT_PATH_FROM_ID |
			VOL_CAP_FMT_DECMPFS_COMPRESSION;
#else
			VOL_CAP_FMT_PATH_FROM_ID;
#endif
=======
loop:;
    simple_lock(&mntvnode_slock);
    for (vp = mp->mnt_vnodelist.lh_first;
         vp != NULL;
         vp = nvp) {
		 int didhold;
        /*
         * If the vnode that we are about to sync is no longer
         * associated with this mount point, start over.
         */
        if (vp->v_mount != mp) {
	    simple_unlock(&mntvnode_slock);
            goto loop;
	}
        simple_lock(&vp->v_interlock);
        nvp = vp->v_mntvnodes.le_next;
        hp = VTOH(vp);

        if ((vp->v_flag & VSYSTEM) || (vp->v_type == VNON) ||
            (((hp->h_nodeflags & (IN_ACCESS | IN_CHANGE | IN_MODIFIED | IN_UPDATE)) == 0) &&
            (vp->v_dirtyblkhd.lh_first == NULL) && !(vp->v_flag & VHASDIRTY))) {
            simple_unlock(&vp->v_interlock);
	    simple_unlock(&mntvnode_slock);
    	    simple_lock(&mntvnode_slock);
            continue;
        }
>>>>>>> origin/10.1

		/*
		 * Bits in the "valid" field tell you whether or not the on-disk
		 * format supports feature X.
		 */

<<<<<<< HEAD
		cap->valid[VOL_CAPABILITIES_INTERFACES] =
			VOL_CAP_INT_ATTRLIST |
			VOL_CAP_INT_NFSEXPORT |
			VOL_CAP_INT_READDIRATTR |
			VOL_CAP_INT_COPYFILE |
			VOL_CAP_INT_ALLOCATE |
			VOL_CAP_INT_VOL_RENAME |
			VOL_CAP_INT_ADVLOCK |
			VOL_CAP_INT_FLOCK |
			VOL_CAP_INT_MANLOCK |
#if NAMEDSTREAMS
			VOL_CAP_INT_EXTENDED_ATTR |
			VOL_CAP_INT_NAMEDSTREAMS;
#else
			VOL_CAP_INT_EXTENDED_ATTR;
#endif

		/* HFS always supports exchangedata and searchfs in the on-disk format natively */
		cap->valid[VOL_CAPABILITIES_INTERFACES] |= (VOL_CAP_INT_SEARCHFS | VOL_CAP_INT_EXCHANGEDATA);


		cap->valid[VOL_CAPABILITIES_RESERVED1] = 0;
		cap->valid[VOL_CAPABILITIES_RESERVED2] = 0;
		VFSATTR_SET_SUPPORTED(fsap, f_capabilities);
	}
	if (VFSATTR_IS_ACTIVE(fsap, f_attributes)) {
		vol_attributes_attr_t *attrp = &fsap->f_attributes;

        	attrp->validattr.commonattr = HFS_ATTR_CMN_VOL_VALIDMASK;
        	attrp->validattr.volattr = ATTR_VOL_VALIDMASK & ~ATTR_VOL_INFO;
        	attrp->validattr.dirattr = ATTR_DIR_VALIDMASK;
        	attrp->validattr.fileattr = HFS_ATTR_FILE_VALIDMASK;
        	attrp->validattr.forkattr = 0;

        	attrp->nativeattr.commonattr = HFS_ATTR_CMN_VOL_VALIDMASK;
        	attrp->nativeattr.volattr = ATTR_VOL_VALIDMASK & ~ATTR_VOL_INFO;
        	attrp->nativeattr.dirattr = ATTR_DIR_VALIDMASK;
        	attrp->nativeattr.fileattr = HFS_ATTR_FILE_VALIDMASK;
        	attrp->nativeattr.forkattr = 0;
		VFSATTR_SET_SUPPORTED(fsap, f_attributes);
	}	
	fsap->f_create_time.tv_sec = hfsmp->hfs_itime;
	fsap->f_create_time.tv_nsec = 0;
	VFSATTR_SET_SUPPORTED(fsap, f_create_time);
	fsap->f_modify_time.tv_sec = hfsmp->vcbLsMod;
	fsap->f_modify_time.tv_nsec = 0;
	VFSATTR_SET_SUPPORTED(fsap, f_modify_time);

	fsap->f_backup_time.tv_sec = hfsmp->vcbVolBkUp;
	fsap->f_backup_time.tv_nsec = 0;
	VFSATTR_SET_SUPPORTED(fsap, f_backup_time);
	if (VFSATTR_IS_ACTIVE(fsap, f_fssubtype)) {
		u_int16_t subtype = 0;

		/*
		 * Subtypes (flavors) for HFS
		 *   0:   Mac OS Extended
		 *   1:   Mac OS Extended (Journaled) 
		 *   2:   Mac OS Extended (Case Sensitive) 
		 *   3:   Mac OS Extended (Case Sensitive, Journaled) 
		 *   4 - 127:   Reserved
		 * 128:   Mac OS Standard
		 * 
		 */
		if ((hfsmp->hfs_flags & HFS_STANDARD) == 0) {
			if (hfsmp->jnl) {
				subtype |= HFS_SUBTYPE_JOURNALED;
			}
			if (hfsmp->hfs_flags & HFS_CASE_SENSITIVE) {
				subtype |= HFS_SUBTYPE_CASESENSITIVE;
			}
		}
#if CONFIG_HFS_STD
		else {
			subtype = HFS_SUBTYPE_STANDARDHFS;
		} 
#endif
		fsap->f_fssubtype = subtype;
		VFSATTR_SET_SUPPORTED(fsap, f_fssubtype);
	}
=======
    vcb = HFSTOVCB(hfsmp);
    meta_vp[0] = vcb->extentsRefNum;
    meta_vp[1] = vcb->catalogRefNum;
    meta_vp[2] = vcb->allocationsRefNum;  /* This is NULL for standard HFS */

    /* Now sync our three metadata files */
    for (i = 0; i < 3; ++i) {
	struct vnode *btvp;
  
        btvp = meta_vp[i];

        if ((btvp==0) || (btvp->v_type == VNON) || (btvp->v_mount != mp))
            continue;
        simple_lock(&btvp->v_interlock);
        hp = VTOH(btvp);
        if (((hp->h_nodeflags & (IN_ACCESS | IN_CHANGE | IN_MODIFIED | IN_UPDATE)) == 0) &&
            (btvp->v_dirtyblkhd.lh_first == NULL) && !(btvp->v_flag & VHASDIRTY)) {
            simple_unlock(&btvp->v_interlock);
            continue;
        }
        simple_unlock(&mntvnode_slock);
        error = vget(btvp, LK_EXCLUSIVE | LK_NOWAIT | LK_INTERLOCK, p);
        if (error) {
            simple_lock(&mntvnode_slock);
            continue;
        }
        if ((error = VOP_FSYNC(btvp, cred, waitfor, p)))
            allerror = error;
        VOP_UNLOCK(btvp, 0, p);
        vrele(btvp);
        simple_lock(&mntvnode_slock);
    };

    simple_unlock(&mntvnode_slock);

    /*
     * Force stale file system control information to be flushed.
     */
    if (vcb->vcbSigWord == kHFSSigWord) {
        if ((error = VOP_FSYNC(hfsmp->hfs_devvp, cred, waitfor, p)))
            allerror = error;
    }
    /*
     * Write back modified superblock.
     */
>>>>>>> origin/10.1

	if (VFSATTR_IS_ACTIVE(fsap, f_vol_name)) {
		strlcpy(fsap->f_vol_name, (char *) hfsmp->vcbVN, MAXPATHLEN);
		VFSATTR_SET_SUPPORTED(fsap, f_vol_name);
	}
	if (VFSATTR_IS_ACTIVE(fsap, f_uuid)) {
		hfs_getvoluuid(hfsmp, fsap->f_uuid);
		VFSATTR_SET_SUPPORTED(fsap, f_uuid);
	}
	return (0);
}

/*
 * Perform a volume rename.  Requires the FS' root vp.
 */
static int
hfs_rename_volume(struct vnode *vp, const char *name, proc_t p)
{
	ExtendedVCB *vcb = VTOVCB(vp);
	struct cnode *cp = VTOC(vp);
	struct hfsmount *hfsmp = VTOHFS(vp);
	struct cat_desc to_desc;
	struct cat_desc todir_desc;
	struct cat_desc new_desc;
	cat_cookie_t cookie;
	int lockflags;
	int error = 0;
	char converted_volname[256];
	size_t volname_length = 0;
	size_t conv_volname_length = 0;
	

	/*
	 * Ignore attempts to rename a volume to a zero-length name.
	 */
	if (name[0] == 0)
		return(0);

	bzero(&to_desc, sizeof(to_desc));
	bzero(&todir_desc, sizeof(todir_desc));
	bzero(&new_desc, sizeof(new_desc));
	bzero(&cookie, sizeof(cookie));

	todir_desc.cd_parentcnid = kHFSRootParentID;
	todir_desc.cd_cnid = kHFSRootFolderID;
	todir_desc.cd_flags = CD_ISDIR;

	to_desc.cd_nameptr = (const u_int8_t *)name;
	to_desc.cd_namelen = strlen(name);
	to_desc.cd_parentcnid = kHFSRootParentID;
	to_desc.cd_cnid = cp->c_cnid;
	to_desc.cd_flags = CD_ISDIR;

	if ((error = hfs_lock(cp, HFS_EXCLUSIVE_LOCK, HFS_LOCK_DEFAULT)) == 0) {
		if ((error = hfs_start_transaction(hfsmp)) == 0) {
			if ((error = cat_preflight(hfsmp, CAT_RENAME, &cookie, p)) == 0) {
				lockflags = hfs_systemfile_lock(hfsmp, SFL_CATALOG, HFS_EXCLUSIVE_LOCK);

				error = cat_rename(hfsmp, &cp->c_desc, &todir_desc, &to_desc, &new_desc);

				/*
				 * If successful, update the name in the VCB, ensure it's terminated.
				 */
				if (error == 0) {
					strlcpy((char *)vcb->vcbVN, name, sizeof(vcb->vcbVN));

					volname_length = strlen ((const char*)vcb->vcbVN);
					/* Send the volume name down to CoreStorage if necessary */	
					error = utf8_normalizestr(vcb->vcbVN, volname_length, (u_int8_t*)converted_volname, &conv_volname_length, 256, UTF_PRECOMPOSED);
					if (error == 0) {
						(void) VNOP_IOCTL (hfsmp->hfs_devvp, _DKIOCCSSETLVNAME, converted_volname, 0, vfs_context_current());
					}
					error = 0;
				}
				
				hfs_systemfile_unlock(hfsmp, lockflags);
				cat_postflight(hfsmp, &cookie, p);
			
				if (error)
					MarkVCBDirty(vcb);
				(void) hfs_flushvolumeheader(hfsmp, HFS_FVH_WAIT);
			}
			hfs_end_transaction(hfsmp);
		}			
		if (!error) {
			/* Release old allocated name buffer */
			if (cp->c_desc.cd_flags & CD_HASBUF) {
				const char *tmp_name = (const char *)cp->c_desc.cd_nameptr;
		
				cp->c_desc.cd_nameptr = 0;
				cp->c_desc.cd_namelen = 0;
				cp->c_desc.cd_flags &= ~CD_HASBUF;
				vfs_removename(tmp_name);
			}			
			/* Update cnode's catalog descriptor */
			replace_desc(cp, &new_desc);
			vcb->volumeNameEncodingHint = new_desc.cd_encoding;
			cp->c_touch_chgtime = TRUE;
		}

		hfs_unlock(cp);
	}
	
	return(error);
}

/*
 * Get file system attributes.
 */
static int
hfs_vfs_setattr(struct mount *mp, struct vfs_attr *fsap, __unused vfs_context_t context)
{
	kauth_cred_t cred = vfs_context_ucred(context);
	int error = 0;

	/*
	 * Must be superuser or owner of filesystem to change volume attributes
	 */
	if (!kauth_cred_issuser(cred) && (kauth_cred_getuid(cred) != vfs_statfs(mp)->f_owner))
		return(EACCES);

	if (VFSATTR_IS_ACTIVE(fsap, f_vol_name)) {
		vnode_t root_vp;
		
		error = hfs_vfs_root(mp, &root_vp, context);
		if (error)
			goto out;

		error = hfs_rename_volume(root_vp, fsap->f_vol_name, vfs_context_proc(context));
		(void) vnode_put(root_vp);
		if (error)
			goto out;

		VFSATTR_SET_SUPPORTED(fsap, f_vol_name);
	}

out:
	return error;
}

/* If a runtime corruption is detected, set the volume inconsistent 
 * bit in the volume attributes.  The volume inconsistent bit is a persistent
 * bit which represents that the volume is corrupt and needs repair.  
 * The volume inconsistent bit can be set from the kernel when it detects
 * runtime corruption or from file system repair utilities like fsck_hfs when
 * a repair operation fails.  The bit should be cleared only from file system 
 * verify/repair utility like fsck_hfs when a verify/repair succeeds.
 */
__private_extern__
void hfs_mark_inconsistent(struct hfsmount *hfsmp,
								  hfs_inconsistency_reason_t reason)
{
	hfs_lock_mount (hfsmp);
	if ((hfsmp->vcbAtrb & kHFSVolumeInconsistentMask) == 0) {
		hfsmp->vcbAtrb |= kHFSVolumeInconsistentMask;
		MarkVCBDirty(hfsmp);
	}
	if ((hfsmp->hfs_flags & HFS_READ_ONLY)==0) {
		switch (reason) {
		case HFS_INCONSISTENCY_DETECTED:
			printf("hfs_mark_inconsistent: Runtime corruption detected on %s, fsck will be forced on next mount.\n", 
				   hfsmp->vcbVN);
			break;
		case HFS_ROLLBACK_FAILED:
			printf("hfs_mark_inconsistent: Failed to roll back; volume `%s' might be inconsistent; fsck will be forced on next mount.\n", 
				   hfsmp->vcbVN);
			break;
		case HFS_OP_INCOMPLETE:
			printf("hfs_mark_inconsistent: Failed to complete operation; volume `%s' might be inconsistent; fsck will be forced on next mount.\n", 
				   hfsmp->vcbVN);
			break;
		case HFS_FSCK_FORCED:
			printf("hfs_mark_inconsistent: fsck requested for `%s'; fsck will be forced on next mount.\n",
				   hfsmp->vcbVN);
			break;	
		}
	}
	hfs_unlock_mount (hfsmp);
}

/* Replay the journal on the device node provided.  Returns zero if 
 * journal replay succeeded or no journal was supposed to be replayed.
 */
static int hfs_journal_replay(vnode_t devvp, vfs_context_t context)
{
	int retval = 0;
	int error = 0;
	struct mount *mp = NULL;
	struct hfs_mount_args *args = NULL;

	/* Replay allowed only on raw devices */
	if (!vnode_ischr(devvp) && !vnode_isblk(devvp)) {
		retval = EINVAL;
		goto out;
	}

	/* Create dummy mount structures */
	MALLOC(mp, struct mount *, sizeof(struct mount), M_TEMP, M_WAITOK);
	if (mp == NULL) {
		retval = ENOMEM;
		goto out;
	}
	bzero(mp, sizeof(struct mount));
	mount_lock_init(mp);

	MALLOC(args, struct hfs_mount_args *, sizeof(struct hfs_mount_args), M_TEMP, M_WAITOK);
	if (args == NULL) {
		retval = ENOMEM;
		goto out;
	}
	bzero(args, sizeof(struct hfs_mount_args));

	retval = hfs_mountfs(devvp, mp, args, 1, context);
	buf_flushdirtyblks(devvp, TRUE, 0, "hfs_journal_replay");
	
	/* FSYNC the devnode to be sure all data has been flushed */
	error = VNOP_FSYNC(devvp, MNT_WAIT, context);
	if (error) {
		retval = error;
	}

out:
	if (mp) {
		mount_lock_destroy(mp);
		FREE(mp, M_TEMP);
	}
	if (args) {
		FREE(args, M_TEMP);
	}
	return retval;
}


/* 
 * Cancel the syncer
 */
static void
hfs_syncer_free(struct hfsmount *hfsmp)
{
<<<<<<< HEAD
    if (hfsmp && hfsmp->hfs_syncer) {
        hfs_syncer_lock(hfsmp);
		
        /*
         * First, make sure everything else knows we don't want any more
         * requests queued.
         */
        thread_call_t syncer = hfsmp->hfs_syncer;
        hfsmp->hfs_syncer = NULL;
=======
    ExtendedVCB 			*vcb = HFSTOVCB(hfsmp);
    FCB						*fcb;
    HFSPlusVolumeHeader		*volumeHeader;
    int						retval;
    struct buf 				*bp;
    int						i;
	int sectorsize;
	int priIDSector;

	if (vcb->vcbSigWord != kHFSPlusSigWord)
		return EINVAL;

	sectorsize = hfsmp->hfs_phys_block_size;
	priIDSector = (vcb->hfsPlusIOPosOffset / sectorsize) +
			HFS_PRI_SECTOR(sectorsize);

	retval = meta_bread(hfsmp->hfs_devvp, priIDSector, sectorsize, NOCRED, &bp);
	if (retval) {
	    DBG_VFS((" hfs_flushvolumeheader bread return error! (%d)\n", retval));
		if (bp) brelse(bp);
		return retval;
	}

    DBG_ASSERT(bp != NULL);
    DBG_ASSERT(bp->b_data != NULL);
    DBG_ASSERT(bp->b_bcount == size);

	volumeHeader = (HFSPlusVolumeHeader *)((char *)bp->b_data + HFS_PRI_OFFSET(sectorsize));

	/*
	 * For embedded HFS+ volumes, update create date if it changed
	 * (ie from a setattrlist call)
	 */
	if ((vcb->hfsPlusIOPosOffset != 0) && (SWAP_BE32 (volumeHeader->createDate) != vcb->localCreateDate))
	  {
		struct buf 				*bp2;
		HFSMasterDirectoryBlock	*mdb;

		retval = meta_bread(hfsmp->hfs_devvp, HFS_PRI_SECTOR(sectorsize), sectorsize, NOCRED, &bp2);
		if (retval != E_NONE) {
			if (bp2) brelse(bp2);
		} else {
			mdb = (HFSMasterDirectoryBlock *)(bp2->b_data + HFS_PRI_OFFSET(sectorsize));
>>>>>>> origin/10.1

        hfs_syncer_unlock(hfsmp);

        // Now deal with requests that are outstanding
        if (hfsmp->hfs_sync_incomplete) {
            if (thread_call_cancel(syncer)) {
                // We managed to cancel the timer so we're done
                hfsmp->hfs_sync_incomplete = FALSE;
            } else {
                // Syncer must be running right now so we have to wait
                hfs_syncer_lock(hfsmp);
                while (hfsmp->hfs_sync_incomplete)
                    hfs_syncer_wait(hfsmp);
                hfs_syncer_unlock(hfsmp);
            }
        }

        // Now we're safe to free the syncer
        thread_call_free(syncer);
    }
}

/*
 * hfs vfs operations.
 */
struct vfsops hfs_vfsops = {
	hfs_mount,
	hfs_start,
	hfs_unmount,
	hfs_vfs_root,
	hfs_quotactl,
	hfs_vfs_getattr, 	/* was hfs_statfs */
	hfs_sync,
	hfs_vfs_vget,
	hfs_fhtovp,
	hfs_vptofh,
	hfs_init,
	hfs_sysctl,
	hfs_vfs_setattr,
	{NULL}
};
