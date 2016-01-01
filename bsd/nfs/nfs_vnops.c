/*
<<<<<<< HEAD
 * Copyright (c) 2000-2015 Apple Inc. All rights reserved.
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
/* Copyright (c) 1995 NeXT Computer, Inc. All Rights Reserved */
/*
 * Copyright (c) 1989, 1993
 *	The Regents of the University of California.  All rights reserved.
 *
 * This code is derived from software contributed to Berkeley by
 * Rick Macklem at The University of Guelph.
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
 *	@(#)nfs_vnops.c	8.16 (Berkeley) 5/27/95
 * FreeBSD-Id: nfs_vnops.c,v 1.72 1997/11/07 09:20:48 phk Exp $
 */


/*
 * vnode op calls for Sun NFS version 2 and 3
 */
#include <sys/param.h>
#include <sys/kernel.h>
#include <sys/systm.h>
#include <sys/resourcevar.h>
#include <sys/proc_internal.h>
#include <sys/kauth.h>
#include <sys/mount_internal.h>
#include <sys/malloc.h>
#include <sys/kpi_mbuf.h>
#include <sys/conf.h>
#include <sys/vnode_internal.h>
#include <sys/dirent.h>
#include <sys/fcntl.h>
#include <sys/lockf.h>
#include <sys/ubc_internal.h>
#include <sys/attr.h>
#include <sys/signalvar.h>
#include <sys/uio_internal.h>

#include <vfs/vfs_support.h>

#include <sys/vm.h>

#include <sys/time.h>
#include <kern/clock.h>
#include <libkern/OSAtomic.h>

#include <miscfs/fifofs/fifo.h>
#include <miscfs/specfs/specdev.h>

#include <nfs/rpcv2.h>
#include <nfs/nfsproto.h>
#include <nfs/nfs.h>
#include <nfs/nfsnode.h>
#include <nfs/nfs_gss.h>
#include <nfs/nfsmount.h>
#include <nfs/nfs_lock.h>
#include <nfs/xdr_subs.h>
#include <nfs/nfsm_subs.h>

#include <net/if.h>
#include <netinet/in.h>
#include <netinet/in_var.h>

#include <vm/vm_kern.h>
#include <vm/vm_pageout.h>

#include <kern/task.h>
#include <kern/sched_prim.h>

<<<<<<< HEAD
#define NFS_VNOP_DBG(...) NFS_DBG(NFS_FAC_VNOP, 7, ## __VA_ARGS__)
#define DEFAULT_READLINK_NOCACHE 0
=======
#include <sys/kdebug.h>

#define FSDBG(A, B, C, D, E) \
	KERNEL_DEBUG((FSDBG_CODE(DBG_FSRW, (A))) | DBG_FUNC_NONE, \
		(int)(B), (int)(C), (int)(D), (int)(E), 0)
#define FSDBG_TOP(A, B, C, D, E) \
	KERNEL_DEBUG((FSDBG_CODE(DBG_FSRW, (A))) | DBG_FUNC_START, \
		(int)(B), (int)(C), (int)(D), (int)(E), 0)
#define FSDBG_BOT(A, B, C, D, E) \
	KERNEL_DEBUG((FSDBG_CODE(DBG_FSRW, (A))) | DBG_FUNC_END, \
		(int)(B), (int)(C), (int)(D), (int)(E), 0)

#define	TRUE	1
#define	FALSE	0

static int	nfsspec_read __P((struct vop_read_args *));
static int	nfsspec_write __P((struct vop_write_args *));
static int	nfsfifo_read __P((struct vop_read_args *));
static int	nfsfifo_write __P((struct vop_write_args *));
static int	nfsspec_close __P((struct vop_close_args *));
static int	nfsfifo_close __P((struct vop_close_args *));
#define nfs_poll vop_nopoll
static int	nfs_ioctl __P((struct vop_ioctl_args *));
static int	nfs_select __P((struct vop_select_args *));
static int	nfs_flush __P((struct vnode *,struct ucred *,int,struct proc *));
static int	nfs_setattrrpc __P((struct vnode *,struct vattr *,struct ucred *,struct proc *));
static	int	nfs_lookup __P((struct vop_lookup_args *));
static	int	nfs_create __P((struct vop_create_args *));
static	int	nfs_mknod __P((struct vop_mknod_args *));
static	int	nfs_open __P((struct vop_open_args *));
static	int	nfs_close __P((struct vop_close_args *));
static	int	nfs_access __P((struct vop_access_args *));
static	int	nfs_getattr __P((struct vop_getattr_args *));
static	int	nfs_setattr __P((struct vop_setattr_args *));
static	int	nfs_read __P((struct vop_read_args *));
static	int	nfs_mmap __P((struct vop_mmap_args *));
static	int	nfs_fsync __P((struct vop_fsync_args *));
static	int	nfs_remove __P((struct vop_remove_args *));
static	int	nfs_link __P((struct vop_link_args *));
static	int	nfs_rename __P((struct vop_rename_args *));
static	int	nfs_mkdir __P((struct vop_mkdir_args *));
static	int	nfs_rmdir __P((struct vop_rmdir_args *));
static	int	nfs_symlink __P((struct vop_symlink_args *));
static	int	nfs_readdir __P((struct vop_readdir_args *));
static	int	nfs_bmap __P((struct vop_bmap_args *));
static	int	nfs_strategy __P((struct vop_strategy_args *));
static	int	nfs_lookitup __P((struct vnode *,char *,int,struct ucred *,struct proc *,struct nfsnode **));
static	int	nfs_sillyrename __P((struct vnode *,struct vnode *,struct componentname *));
static int	nfsspec_access __P((struct vop_access_args *));
static int	nfs_readlink __P((struct vop_readlink_args *));
static int	nfs_print __P((struct vop_print_args *));
static int	nfs_pathconf __P((struct vop_pathconf_args *));
static int	nfs_advlock __P((struct vop_advlock_args *));
static int	nfs_blkatoff __P((struct vop_blkatoff_args *));
static int	nfs_bwrite __P((struct vop_bwrite_args *));
static int	nfs_valloc __P((struct vop_valloc_args *));
static int	nfs_vfree __P((struct vop_vfree_args *));
static int	nfs_truncate __P((struct vop_truncate_args *));
static int	nfs_update __P((struct vop_update_args *));
static	int	nfs_pagein __P((struct vop_pagein_args *));
static	int	nfs_pageout __P((struct vop_pageout_args *));
static	int nfs_blktooff __P((struct vop_blktooff_args *));
static	int nfs_offtoblk __P((struct vop_offtoblk_args *));
static	int nfs_cmap __P((struct vop_cmap_args *));
>>>>>>> origin/10.1

/*
 * NFS vnode ops
 */
int	nfs_vnop_lookup(struct vnop_lookup_args *);
int	nfsspec_vnop_read(struct vnop_read_args *);
int	nfsspec_vnop_write(struct vnop_write_args *);
int	nfsspec_vnop_close(struct vnop_close_args *);
#if FIFO
int	nfsfifo_vnop_read(struct vnop_read_args *);
int	nfsfifo_vnop_write(struct vnop_write_args *);
int	nfsfifo_vnop_close(struct vnop_close_args *);
#endif
int	nfs_vnop_ioctl(struct vnop_ioctl_args *);
int	nfs_vnop_select(struct vnop_select_args *);
int	nfs_vnop_setattr(struct vnop_setattr_args *);
int	nfs_vnop_fsync(struct vnop_fsync_args *);
int	nfs_vnop_rename(struct vnop_rename_args *);
int	nfs_vnop_readdir(struct vnop_readdir_args *);
int	nfs_vnop_readlink(struct vnop_readlink_args *);
int	nfs_vnop_pathconf(struct vnop_pathconf_args *);
int	nfs_vnop_pagein(struct vnop_pagein_args *);
int	nfs_vnop_pageout(struct vnop_pageout_args *);
int	nfs_vnop_blktooff(struct vnop_blktooff_args *);
int	nfs_vnop_offtoblk(struct vnop_offtoblk_args *);
int	nfs_vnop_blockmap(struct vnop_blockmap_args *);
int	nfs_vnop_monitor(struct vnop_monitor_args *);

int	nfs3_vnop_create(struct vnop_create_args *);
int	nfs3_vnop_mknod(struct vnop_mknod_args *);
int	nfs3_vnop_getattr(struct vnop_getattr_args *);
int	nfs3_vnop_link(struct vnop_link_args *);
int	nfs3_vnop_mkdir(struct vnop_mkdir_args *);
int	nfs3_vnop_rmdir(struct vnop_rmdir_args *);
int	nfs3_vnop_symlink(struct vnop_symlink_args *);

vnop_t **nfsv2_vnodeop_p;
static struct vnodeopv_entry_desc nfsv2_vnodeop_entries[] = {
	{ &vnop_default_desc, (vnop_t *)vn_default_error },
	{ &vnop_lookup_desc, (vnop_t *)nfs_vnop_lookup },	/* lookup */
	{ &vnop_create_desc, (vnop_t *)nfs3_vnop_create },	/* create */
	{ &vnop_mknod_desc, (vnop_t *)nfs3_vnop_mknod },	/* mknod */
	{ &vnop_open_desc, (vnop_t *)nfs_vnop_open },		/* open */
	{ &vnop_close_desc, (vnop_t *)nfs_vnop_close },		/* close */
	{ &vnop_access_desc, (vnop_t *)nfs_vnop_access },	/* access */
	{ &vnop_getattr_desc, (vnop_t *)nfs3_vnop_getattr },	/* getattr */
	{ &vnop_setattr_desc, (vnop_t *)nfs_vnop_setattr },	/* setattr */
	{ &vnop_read_desc, (vnop_t *)nfs_vnop_read },		/* read */
	{ &vnop_write_desc, (vnop_t *)nfs_vnop_write },		/* write */
	{ &vnop_ioctl_desc, (vnop_t *)nfs_vnop_ioctl },		/* ioctl */
	{ &vnop_select_desc, (vnop_t *)nfs_vnop_select },	/* select */
	{ &vnop_revoke_desc, (vnop_t *)nfs_vnop_revoke },	/* revoke */
	{ &vnop_mmap_desc, (vnop_t *)nfs_vnop_mmap },		/* mmap */
	{ &vnop_mnomap_desc, (vnop_t *)nfs_vnop_mnomap },	/* mnomap */
	{ &vnop_fsync_desc, (vnop_t *)nfs_vnop_fsync },		/* fsync */
	{ &vnop_remove_desc, (vnop_t *)nfs_vnop_remove },	/* remove */
	{ &vnop_link_desc, (vnop_t *)nfs3_vnop_link },		/* link */
	{ &vnop_rename_desc, (vnop_t *)nfs_vnop_rename },	/* rename */
	{ &vnop_mkdir_desc, (vnop_t *)nfs3_vnop_mkdir },	/* mkdir */
	{ &vnop_rmdir_desc, (vnop_t *)nfs3_vnop_rmdir },	/* rmdir */
	{ &vnop_symlink_desc, (vnop_t *)nfs3_vnop_symlink },	/* symlink */
	{ &vnop_readdir_desc, (vnop_t *)nfs_vnop_readdir },	/* readdir */
	{ &vnop_readlink_desc, (vnop_t *)nfs_vnop_readlink },	/* readlink */
	{ &vnop_inactive_desc, (vnop_t *)nfs_vnop_inactive },	/* inactive */
	{ &vnop_reclaim_desc, (vnop_t *)nfs_vnop_reclaim },	/* reclaim */
	{ &vnop_strategy_desc, (vnop_t *)err_strategy },	/* strategy */
	{ &vnop_pathconf_desc, (vnop_t *)nfs_vnop_pathconf },	/* pathconf */
	{ &vnop_advlock_desc, (vnop_t *)nfs_vnop_advlock },	/* advlock */
	{ &vnop_bwrite_desc, (vnop_t *)err_bwrite },		/* bwrite */
	{ &vnop_pagein_desc, (vnop_t *)nfs_vnop_pagein },	/* Pagein */
	{ &vnop_pageout_desc, (vnop_t *)nfs_vnop_pageout },	/* Pageout */
	{ &vnop_copyfile_desc, (vnop_t *)err_copyfile },	/* Copyfile */
	{ &vnop_blktooff_desc, (vnop_t *)nfs_vnop_blktooff },	/* blktooff */
	{ &vnop_offtoblk_desc, (vnop_t *)nfs_vnop_offtoblk },	/* offtoblk */
	{ &vnop_blockmap_desc, (vnop_t *)nfs_vnop_blockmap },	/* blockmap */
	{ &vnop_monitor_desc, (vnop_t *)nfs_vnop_monitor },	/* monitor */
	{ NULL, NULL }
};
struct vnodeopv_desc nfsv2_vnodeop_opv_desc =
	{ &nfsv2_vnodeop_p, nfsv2_vnodeop_entries };

vnop_t **nfsv4_vnodeop_p;
static struct vnodeopv_entry_desc nfsv4_vnodeop_entries[] = {
	{ &vnop_default_desc, (vnop_t *)vn_default_error },
	{ &vnop_lookup_desc, (vnop_t *)nfs_vnop_lookup },	/* lookup */
	{ &vnop_create_desc, (vnop_t *)nfs4_vnop_create },	/* create */
	{ &vnop_mknod_desc, (vnop_t *)nfs4_vnop_mknod },	/* mknod */
	{ &vnop_open_desc, (vnop_t *)nfs_vnop_open },		/* open */
	{ &vnop_close_desc, (vnop_t *)nfs_vnop_close },		/* close */
	{ &vnop_access_desc, (vnop_t *)nfs_vnop_access },	/* access */
	{ &vnop_getattr_desc, (vnop_t *)nfs4_vnop_getattr },	/* getattr */
	{ &vnop_setattr_desc, (vnop_t *)nfs_vnop_setattr },	/* setattr */
	{ &vnop_read_desc, (vnop_t *)nfs_vnop_read },		/* read */
	{ &vnop_write_desc, (vnop_t *)nfs_vnop_write },		/* write */
	{ &vnop_ioctl_desc, (vnop_t *)nfs_vnop_ioctl },		/* ioctl */
	{ &vnop_select_desc, (vnop_t *)nfs_vnop_select },	/* select */
	{ &vnop_revoke_desc, (vnop_t *)nfs_vnop_revoke },	/* revoke */
	{ &vnop_mmap_desc, (vnop_t *)nfs_vnop_mmap },		/* mmap */
	{ &vnop_mnomap_desc, (vnop_t *)nfs_vnop_mnomap },	/* mnomap */
	{ &vnop_fsync_desc, (vnop_t *)nfs_vnop_fsync },		/* fsync */
	{ &vnop_remove_desc, (vnop_t *)nfs_vnop_remove },	/* remove */
	{ &vnop_link_desc, (vnop_t *)nfs4_vnop_link },		/* link */
	{ &vnop_rename_desc, (vnop_t *)nfs_vnop_rename },	/* rename */
	{ &vnop_mkdir_desc, (vnop_t *)nfs4_vnop_mkdir },	/* mkdir */
	{ &vnop_rmdir_desc, (vnop_t *)nfs4_vnop_rmdir },	/* rmdir */
	{ &vnop_symlink_desc, (vnop_t *)nfs4_vnop_symlink },	/* symlink */
	{ &vnop_readdir_desc, (vnop_t *)nfs_vnop_readdir },	/* readdir */
	{ &vnop_readlink_desc, (vnop_t *)nfs_vnop_readlink },	/* readlink */
	{ &vnop_inactive_desc, (vnop_t *)nfs_vnop_inactive },	/* inactive */
	{ &vnop_reclaim_desc, (vnop_t *)nfs_vnop_reclaim },	/* reclaim */
	{ &vnop_strategy_desc, (vnop_t *)err_strategy },	/* strategy */
	{ &vnop_pathconf_desc, (vnop_t *)nfs_vnop_pathconf },	/* pathconf */
	{ &vnop_advlock_desc, (vnop_t *)nfs_vnop_advlock },	/* advlock */
	{ &vnop_bwrite_desc, (vnop_t *)err_bwrite },		/* bwrite */
	{ &vnop_pagein_desc, (vnop_t *)nfs_vnop_pagein },	/* Pagein */
	{ &vnop_pageout_desc, (vnop_t *)nfs_vnop_pageout },	/* Pageout */
	{ &vnop_copyfile_desc, (vnop_t *)err_copyfile },	/* Copyfile */
	{ &vnop_blktooff_desc, (vnop_t *)nfs_vnop_blktooff },	/* blktooff */
	{ &vnop_offtoblk_desc, (vnop_t *)nfs_vnop_offtoblk },	/* offtoblk */
	{ &vnop_blockmap_desc, (vnop_t *)nfs_vnop_blockmap },	/* blockmap */
	{ &vnop_getxattr_desc, (vnop_t *)nfs4_vnop_getxattr },	/* getxattr */
	{ &vnop_setxattr_desc, (vnop_t *)nfs4_vnop_setxattr },	/* setxattr */
	{ &vnop_removexattr_desc, (vnop_t *)nfs4_vnop_removexattr },/* removexattr */
	{ &vnop_listxattr_desc, (vnop_t *)nfs4_vnop_listxattr },/* listxattr */
#if NAMEDSTREAMS
	{ &vnop_getnamedstream_desc, (vnop_t *)nfs4_vnop_getnamedstream },	/* getnamedstream */
	{ &vnop_makenamedstream_desc, (vnop_t *)nfs4_vnop_makenamedstream },	/* makenamedstream */
	{ &vnop_removenamedstream_desc, (vnop_t *)nfs4_vnop_removenamedstream },/* removenamedstream */
#endif
	{ &vnop_monitor_desc, (vnop_t *)nfs_vnop_monitor },	/* monitor */
	{ NULL, NULL }
};
struct vnodeopv_desc nfsv4_vnodeop_opv_desc =
	{ &nfsv4_vnodeop_p, nfsv4_vnodeop_entries };

/*
 * Special device vnode ops
 */
vnop_t **spec_nfsv2nodeop_p;
static struct vnodeopv_entry_desc spec_nfsv2nodeop_entries[] = {
	{ &vnop_default_desc, (vnop_t *)vn_default_error },
	{ &vnop_lookup_desc, (vnop_t *)spec_lookup },		/* lookup */
	{ &vnop_create_desc, (vnop_t *)spec_create },		/* create */
	{ &vnop_mknod_desc, (vnop_t *)spec_mknod },		/* mknod */
	{ &vnop_open_desc, (vnop_t *)spec_open },		/* open */
	{ &vnop_close_desc, (vnop_t *)nfsspec_vnop_close },	/* close */
	{ &vnop_getattr_desc, (vnop_t *)nfs3_vnop_getattr },	/* getattr */
	{ &vnop_setattr_desc, (vnop_t *)nfs_vnop_setattr },	/* setattr */
	{ &vnop_read_desc, (vnop_t *)nfsspec_vnop_read },	/* read */
	{ &vnop_write_desc, (vnop_t *)nfsspec_vnop_write },	/* write */
	{ &vnop_ioctl_desc, (vnop_t *)spec_ioctl },		/* ioctl */
	{ &vnop_select_desc, (vnop_t *)spec_select },		/* select */
	{ &vnop_revoke_desc, (vnop_t *)spec_revoke },		/* revoke */
	{ &vnop_mmap_desc, (vnop_t *)spec_mmap },		/* mmap */
	{ &vnop_fsync_desc, (vnop_t *)nfs_vnop_fsync },		/* fsync */
	{ &vnop_remove_desc, (vnop_t *)spec_remove },		/* remove */
	{ &vnop_link_desc, (vnop_t *)spec_link },		/* link */
	{ &vnop_rename_desc, (vnop_t *)spec_rename },		/* rename */
	{ &vnop_mkdir_desc, (vnop_t *)spec_mkdir },		/* mkdir */
	{ &vnop_rmdir_desc, (vnop_t *)spec_rmdir },		/* rmdir */
	{ &vnop_symlink_desc, (vnop_t *)spec_symlink },		/* symlink */
	{ &vnop_readdir_desc, (vnop_t *)spec_readdir },		/* readdir */
	{ &vnop_readlink_desc, (vnop_t *)spec_readlink },	/* readlink */
	{ &vnop_inactive_desc, (vnop_t *)nfs_vnop_inactive },	/* inactive */
	{ &vnop_reclaim_desc, (vnop_t *)nfs_vnop_reclaim },	/* reclaim */
	{ &vnop_strategy_desc, (vnop_t *)spec_strategy },	/* strategy */
	{ &vnop_pathconf_desc, (vnop_t *)spec_pathconf },	/* pathconf */
	{ &vnop_advlock_desc, (vnop_t *)spec_advlock },		/* advlock */
	{ &vnop_bwrite_desc, (vnop_t *)vn_bwrite },		/* bwrite */
	{ &vnop_pagein_desc, (vnop_t *)nfs_vnop_pagein },	/* Pagein */
	{ &vnop_pageout_desc, (vnop_t *)nfs_vnop_pageout },	/* Pageout */
	{ &vnop_blktooff_desc, (vnop_t *)nfs_vnop_blktooff },	/* blktooff */
	{ &vnop_offtoblk_desc, (vnop_t *)nfs_vnop_offtoblk },	/* offtoblk */
	{ &vnop_blockmap_desc, (vnop_t *)nfs_vnop_blockmap },	/* blockmap */
	{ &vnop_monitor_desc, (vnop_t *)nfs_vnop_monitor },	/* monitor */
	{ NULL, NULL }
};
struct vnodeopv_desc spec_nfsv2nodeop_opv_desc =
	{ &spec_nfsv2nodeop_p, spec_nfsv2nodeop_entries };
vnop_t **spec_nfsv4nodeop_p;
static struct vnodeopv_entry_desc spec_nfsv4nodeop_entries[] = {
	{ &vnop_default_desc, (vnop_t *)vn_default_error },
	{ &vnop_lookup_desc, (vnop_t *)spec_lookup },		/* lookup */
	{ &vnop_create_desc, (vnop_t *)spec_create },		/* create */
	{ &vnop_mknod_desc, (vnop_t *)spec_mknod },		/* mknod */
	{ &vnop_open_desc, (vnop_t *)spec_open },		/* open */
	{ &vnop_close_desc, (vnop_t *)nfsspec_vnop_close },	/* close */
	{ &vnop_getattr_desc, (vnop_t *)nfs4_vnop_getattr },	/* getattr */
	{ &vnop_setattr_desc, (vnop_t *)nfs_vnop_setattr },	/* setattr */
	{ &vnop_read_desc, (vnop_t *)nfsspec_vnop_read },	/* read */
	{ &vnop_write_desc, (vnop_t *)nfsspec_vnop_write },	/* write */
	{ &vnop_ioctl_desc, (vnop_t *)spec_ioctl },		/* ioctl */
	{ &vnop_select_desc, (vnop_t *)spec_select },		/* select */
	{ &vnop_revoke_desc, (vnop_t *)spec_revoke },		/* revoke */
	{ &vnop_mmap_desc, (vnop_t *)spec_mmap },		/* mmap */
	{ &vnop_fsync_desc, (vnop_t *)nfs_vnop_fsync },		/* fsync */
	{ &vnop_remove_desc, (vnop_t *)spec_remove },		/* remove */
	{ &vnop_link_desc, (vnop_t *)spec_link },		/* link */
	{ &vnop_rename_desc, (vnop_t *)spec_rename },		/* rename */
	{ &vnop_mkdir_desc, (vnop_t *)spec_mkdir },		/* mkdir */
	{ &vnop_rmdir_desc, (vnop_t *)spec_rmdir },		/* rmdir */
	{ &vnop_symlink_desc, (vnop_t *)spec_symlink },		/* symlink */
	{ &vnop_readdir_desc, (vnop_t *)spec_readdir },		/* readdir */
	{ &vnop_readlink_desc, (vnop_t *)spec_readlink },	/* readlink */
	{ &vnop_inactive_desc, (vnop_t *)nfs_vnop_inactive },	/* inactive */
	{ &vnop_reclaim_desc, (vnop_t *)nfs_vnop_reclaim },	/* reclaim */
	{ &vnop_strategy_desc, (vnop_t *)spec_strategy },	/* strategy */
	{ &vnop_pathconf_desc, (vnop_t *)spec_pathconf },	/* pathconf */
	{ &vnop_advlock_desc, (vnop_t *)spec_advlock },		/* advlock */
	{ &vnop_bwrite_desc, (vnop_t *)vn_bwrite },		/* bwrite */
	{ &vnop_pagein_desc, (vnop_t *)nfs_vnop_pagein },	/* Pagein */
	{ &vnop_pageout_desc, (vnop_t *)nfs_vnop_pageout },	/* Pageout */
	{ &vnop_blktooff_desc, (vnop_t *)nfs_vnop_blktooff },	/* blktooff */
	{ &vnop_offtoblk_desc, (vnop_t *)nfs_vnop_offtoblk },	/* offtoblk */
	{ &vnop_blockmap_desc, (vnop_t *)nfs_vnop_blockmap },	/* blockmap */
	{ &vnop_getxattr_desc, (vnop_t *)nfs4_vnop_getxattr },	/* getxattr */
	{ &vnop_setxattr_desc, (vnop_t *)nfs4_vnop_setxattr },	/* setxattr */
	{ &vnop_removexattr_desc, (vnop_t *)nfs4_vnop_removexattr },/* removexattr */
	{ &vnop_listxattr_desc, (vnop_t *)nfs4_vnop_listxattr },/* listxattr */
#if NAMEDSTREAMS
	{ &vnop_getnamedstream_desc, (vnop_t *)nfs4_vnop_getnamedstream },	/* getnamedstream */
	{ &vnop_makenamedstream_desc, (vnop_t *)nfs4_vnop_makenamedstream },	/* makenamedstream */
	{ &vnop_removenamedstream_desc, (vnop_t *)nfs4_vnop_removenamedstream },/* removenamedstream */
#endif
	{ &vnop_monitor_desc, (vnop_t *)nfs_vnop_monitor },	/* monitor */
	{ NULL, NULL }
};
struct vnodeopv_desc spec_nfsv4nodeop_opv_desc =
	{ &spec_nfsv4nodeop_p, spec_nfsv4nodeop_entries };

#if FIFO
vnop_t **fifo_nfsv2nodeop_p;
static struct vnodeopv_entry_desc fifo_nfsv2nodeop_entries[] = {
	{ &vnop_default_desc, (vnop_t *)vn_default_error },
	{ &vnop_lookup_desc, (vnop_t *)fifo_lookup },		/* lookup */
	{ &vnop_create_desc, (vnop_t *)fifo_create },		/* create */
	{ &vnop_mknod_desc, (vnop_t *)fifo_mknod },		/* mknod */
	{ &vnop_open_desc, (vnop_t *)fifo_open },		/* open */
	{ &vnop_close_desc, (vnop_t *)nfsfifo_vnop_close },	/* close */
	{ &vnop_getattr_desc, (vnop_t *)nfs3_vnop_getattr },	/* getattr */
	{ &vnop_setattr_desc, (vnop_t *)nfs_vnop_setattr },	/* setattr */
	{ &vnop_read_desc, (vnop_t *)nfsfifo_vnop_read },	/* read */
	{ &vnop_write_desc, (vnop_t *)nfsfifo_vnop_write },	/* write */
	{ &vnop_ioctl_desc, (vnop_t *)fifo_ioctl },		/* ioctl */
	{ &vnop_select_desc, (vnop_t *)fifo_select },		/* select */
	{ &vnop_revoke_desc, (vnop_t *)fifo_revoke },		/* revoke */
	{ &vnop_mmap_desc, (vnop_t *)fifo_mmap },		/* mmap */
	{ &vnop_fsync_desc, (vnop_t *)nfs_vnop_fsync },		/* fsync */
	{ &vnop_remove_desc, (vnop_t *)fifo_remove },		/* remove */
	{ &vnop_link_desc, (vnop_t *)fifo_link },		/* link */
	{ &vnop_rename_desc, (vnop_t *)fifo_rename },		/* rename */
	{ &vnop_mkdir_desc, (vnop_t *)fifo_mkdir },		/* mkdir */
	{ &vnop_rmdir_desc, (vnop_t *)fifo_rmdir },		/* rmdir */
	{ &vnop_symlink_desc, (vnop_t *)fifo_symlink },		/* symlink */
	{ &vnop_readdir_desc, (vnop_t *)fifo_readdir },		/* readdir */
	{ &vnop_readlink_desc, (vnop_t *)fifo_readlink },	/* readlink */
	{ &vnop_inactive_desc, (vnop_t *)nfs_vnop_inactive },	/* inactive */
	{ &vnop_reclaim_desc, (vnop_t *)nfs_vnop_reclaim },	/* reclaim */
	{ &vnop_strategy_desc, (vnop_t *)fifo_strategy },	/* strategy */
	{ &vnop_pathconf_desc, (vnop_t *)fifo_pathconf },	/* pathconf */
	{ &vnop_advlock_desc, (vnop_t *)fifo_advlock },		/* advlock */
	{ &vnop_bwrite_desc, (vnop_t *)vn_bwrite },		/* bwrite */
	{ &vnop_pagein_desc, (vnop_t *)nfs_vnop_pagein },	/* Pagein */
	{ &vnop_pageout_desc, (vnop_t *)nfs_vnop_pageout },	/* Pageout */
	{ &vnop_blktooff_desc, (vnop_t *)nfs_vnop_blktooff },	/* blktooff */
	{ &vnop_offtoblk_desc, (vnop_t *)nfs_vnop_offtoblk },	/* offtoblk */
	{ &vnop_blockmap_desc, (vnop_t *)nfs_vnop_blockmap },	/* blockmap */
	{ &vnop_monitor_desc, (vnop_t *)nfs_vnop_monitor },	/* monitor */
	{ NULL, NULL }
};
struct vnodeopv_desc fifo_nfsv2nodeop_opv_desc =
	{ &fifo_nfsv2nodeop_p, fifo_nfsv2nodeop_entries };

vnop_t **fifo_nfsv4nodeop_p;
static struct vnodeopv_entry_desc fifo_nfsv4nodeop_entries[] = {
	{ &vnop_default_desc, (vnop_t *)vn_default_error },
	{ &vnop_lookup_desc, (vnop_t *)fifo_lookup },		/* lookup */
	{ &vnop_create_desc, (vnop_t *)fifo_create },		/* create */
	{ &vnop_mknod_desc, (vnop_t *)fifo_mknod },		/* mknod */
	{ &vnop_open_desc, (vnop_t *)fifo_open },		/* open */
	{ &vnop_close_desc, (vnop_t *)nfsfifo_vnop_close },	/* close */
	{ &vnop_getattr_desc, (vnop_t *)nfs4_vnop_getattr },	/* getattr */
	{ &vnop_setattr_desc, (vnop_t *)nfs_vnop_setattr },	/* setattr */
	{ &vnop_read_desc, (vnop_t *)nfsfifo_vnop_read },	/* read */
	{ &vnop_write_desc, (vnop_t *)nfsfifo_vnop_write },	/* write */
	{ &vnop_ioctl_desc, (vnop_t *)fifo_ioctl },		/* ioctl */
	{ &vnop_select_desc, (vnop_t *)fifo_select },		/* select */
	{ &vnop_revoke_desc, (vnop_t *)fifo_revoke },		/* revoke */
	{ &vnop_mmap_desc, (vnop_t *)fifo_mmap },		/* mmap */
	{ &vnop_fsync_desc, (vnop_t *)nfs_vnop_fsync },		/* fsync */
	{ &vnop_remove_desc, (vnop_t *)fifo_remove },		/* remove */
	{ &vnop_link_desc, (vnop_t *)fifo_link },		/* link */
	{ &vnop_rename_desc, (vnop_t *)fifo_rename },		/* rename */
	{ &vnop_mkdir_desc, (vnop_t *)fifo_mkdir },		/* mkdir */
	{ &vnop_rmdir_desc, (vnop_t *)fifo_rmdir },		/* rmdir */
	{ &vnop_symlink_desc, (vnop_t *)fifo_symlink },		/* symlink */
	{ &vnop_readdir_desc, (vnop_t *)fifo_readdir },		/* readdir */
	{ &vnop_readlink_desc, (vnop_t *)fifo_readlink },	/* readlink */
	{ &vnop_inactive_desc, (vnop_t *)nfs_vnop_inactive },	/* inactive */
	{ &vnop_reclaim_desc, (vnop_t *)nfs_vnop_reclaim },	/* reclaim */
	{ &vnop_strategy_desc, (vnop_t *)fifo_strategy },	/* strategy */
	{ &vnop_pathconf_desc, (vnop_t *)fifo_pathconf },	/* pathconf */
	{ &vnop_advlock_desc, (vnop_t *)fifo_advlock },		/* advlock */
	{ &vnop_bwrite_desc, (vnop_t *)vn_bwrite },		/* bwrite */
	{ &vnop_pagein_desc, (vnop_t *)nfs_vnop_pagein },	/* Pagein */
	{ &vnop_pageout_desc, (vnop_t *)nfs_vnop_pageout },	/* Pageout */
	{ &vnop_blktooff_desc, (vnop_t *)nfs_vnop_blktooff },	/* blktooff */
	{ &vnop_offtoblk_desc, (vnop_t *)nfs_vnop_offtoblk },	/* offtoblk */
	{ &vnop_blockmap_desc, (vnop_t *)nfs_vnop_blockmap },	/* blockmap */
	{ &vnop_getxattr_desc, (vnop_t *)nfs4_vnop_getxattr },	/* getxattr */
	{ &vnop_setxattr_desc, (vnop_t *)nfs4_vnop_setxattr },	/* setxattr */
	{ &vnop_removexattr_desc, (vnop_t *)nfs4_vnop_removexattr },/* removexattr */
	{ &vnop_listxattr_desc, (vnop_t *)nfs4_vnop_listxattr },/* listxattr */
#if NAMEDSTREAMS
	{ &vnop_getnamedstream_desc, (vnop_t *)nfs4_vnop_getnamedstream },	/* getnamedstream */
	{ &vnop_makenamedstream_desc, (vnop_t *)nfs4_vnop_makenamedstream },	/* makenamedstream */
	{ &vnop_removenamedstream_desc, (vnop_t *)nfs4_vnop_removenamedstream },/* removenamedstream */
#endif
	{ &vnop_monitor_desc, (vnop_t *)nfs_vnop_monitor },	/* monitor */
	{ NULL, NULL }
};
struct vnodeopv_desc fifo_nfsv4nodeop_opv_desc =
	{ &fifo_nfsv4nodeop_p, fifo_nfsv4nodeop_entries };
#endif /* FIFO */

int	nfs_sillyrename(nfsnode_t,nfsnode_t,struct componentname *,vfs_context_t);
int	nfs_getattr_internal(nfsnode_t, struct nfs_vattr *, vfs_context_t, int);
int	nfs_refresh_fh(nfsnode_t, vfs_context_t);

/*
 * Find the slot in the access cache for this UID.
 * If adding and no existing slot is found, reuse slots in FIFO order.
 * The index of the next slot to use is kept in the last entry of the n_access array.
 */
int
nfs_node_access_slot(nfsnode_t np, uid_t uid, int add)
{
	int slot;

	for (slot=0; slot < NFS_ACCESS_CACHE_SIZE; slot++)
		if (np->n_accessuid[slot] == uid)
			break;
	if (slot == NFS_ACCESS_CACHE_SIZE) {
		if (!add)
			return (-1);
		slot = np->n_access[NFS_ACCESS_CACHE_SIZE];
		np->n_access[NFS_ACCESS_CACHE_SIZE] = (slot + 1) % NFS_ACCESS_CACHE_SIZE;
	}
	return (slot);
}

int
nfs3_access_rpc(nfsnode_t np, u_int32_t *access, int rpcflags, vfs_context_t ctx)
{
<<<<<<< HEAD
	int error = 0, lockerror = ENOENT, status, slot;
	uint32_t access_result = 0;
	u_int64_t xid;
	struct nfsm_chain nmreq, nmrep;
	struct timeval now;
	uid_t uid;

	nfsm_chain_null(&nmreq);
	nfsm_chain_null(&nmrep);

	nfsm_chain_build_alloc_init(error, &nmreq, NFSX_FH(NFS_VER3) + NFSX_UNSIGNED);
	nfsm_chain_add_fh(error, &nmreq, NFS_VER3, np->n_fhp, np->n_fhsize);
	nfsm_chain_add_32(error, &nmreq, *access);
	nfsm_chain_build_done(error, &nmreq);
	nfsmout_if(error);
	error = nfs_request2(np, NULL, &nmreq, NFSPROC_ACCESS,
		vfs_context_thread(ctx), vfs_context_ucred(ctx),
		NULL, rpcflags, &nmrep, &xid, &status);
	if ((lockerror = nfs_node_lock(np)))
		error = lockerror;
	nfsm_chain_postop_attr_update(error, &nmrep, np, &xid);
	if (!error)
		error = status;
	nfsm_chain_get_32(error, &nmrep, access_result);
	nfsmout_if(error);

	uid = kauth_cred_getuid(vfs_context_ucred(ctx));
	slot = nfs_node_access_slot(np, uid, 1);
	np->n_accessuid[slot] = uid;
	microuptime(&now);
	np->n_accessstamp[slot] = now.tv_sec;
	np->n_access[slot] = access_result;

	/*
	 * If we asked for DELETE but didn't get it, the server
	 * may simply not support returning that bit (possible
	 * on UNIX systems).  So, we'll assume that it is OK,
	 * and just let any subsequent delete action fail if it
	 * really isn't deletable.
	 */
	if ((*access & NFS_ACCESS_DELETE) &&
	    !(np->n_access[slot] & NFS_ACCESS_DELETE))
		np->n_access[slot] |= NFS_ACCESS_DELETE;
	/* ".zfs" subdirectories may erroneously give a denied answer for add/remove */
	if (nfs_access_dotzfs && (np->n_flag & NISDOTZFSCHILD))
		np->n_access[slot] |= (NFS_ACCESS_MODIFY|NFS_ACCESS_EXTEND|NFS_ACCESS_DELETE);
	/* pass back the access returned with this request */
	*access = np->n_access[slot];
nfsmout:
	if (!lockerror)
		nfs_node_unlock(np);
	nfsm_chain_cleanup(&nmreq);
	nfsm_chain_cleanup(&nmrep);
	return (error);
=======
	const int v3 = 1;
	u_int32_t *tl;
	int error = 0, attrflag;

	struct mbuf *mreq, *mrep, *md, *mb, *mb2;
	caddr_t bpos, dpos, cp2;
	register int32_t t1, t2;
	register caddr_t cp;
	u_int32_t rmode;
	struct nfsnode *np = VTONFS(vp);
	u_int64_t xid;

	nfsstats.rpccnt[NFSPROC_ACCESS]++;   
	nfsm_reqhead(vp, NFSPROC_ACCESS, NFSX_FH(v3) + NFSX_UNSIGNED);
	nfsm_fhtom(vp, v3);
	nfsm_build(tl, u_int32_t *, NFSX_UNSIGNED);
	*tl = txdr_unsigned(wmode);
	nfsm_request(vp, NFSPROC_ACCESS, p, cred, &xid);
	if (mrep) {
		nfsm_postop_attr(vp, attrflag, &xid);
	}
	if (!error) {
		nfsm_dissect(tl, u_int32_t *, NFSX_UNSIGNED);
		rmode = fxdr_unsigned(u_int32_t, *tl);
		np->n_mode = rmode;
		np->n_modeuid = cred->cr_uid;
		np->n_modestamp = time_second;
	}
	nfsm_reqdone;
	return error;
>>>>>>> origin/10.1
}

/*
 * NFS access vnode op.
 * For NFS version 2, just return ok. File accesses may fail later.
 * For NFS version 3+, use the access RPC to check accessibility. If file
 * permissions are changed on the server, accesses might still fail later.
 */
int
nfs_vnop_access(
	struct vnop_access_args /* {
		struct vnodeop_desc *a_desc;
		vnode_t a_vp;
		int a_action;
		vfs_context_t a_context;
	} */ *ap)
{
	vfs_context_t ctx = ap->a_context;
	vnode_t vp = ap->a_vp;
	int error = 0, slot, dorpc, rpcflags = 0;
	u_int32_t access, waccess;
	nfsnode_t np = VTONFS(vp);
	struct nfsmount *nmp;
	int nfsvers;
	struct timeval now;
	uid_t uid;

	nmp = VTONMP(vp);
	if (nfs_mount_gone(nmp))
		return (ENXIO);
	nfsvers = nmp->nm_vers;

	if (nfsvers == NFS_VER2) {
		if ((ap->a_action & KAUTH_VNODE_WRITE_RIGHTS) &&
		    vfs_isrdonly(vnode_mount(vp)))
			return (EROFS);
		return (0);
	}

	/*
	 * For NFS v3, do an access rpc, otherwise you are stuck emulating
	 * ufs_access() locally using the vattr. This may not be correct,
	 * since the server may apply other access criteria such as
	 * client uid-->server uid mapping that we do not know about, but
	 * this is better than just returning anything that is lying about
	 * in the cache.
	 */
<<<<<<< HEAD

=======
	if (v3) {
		if (ap->a_mode & VREAD)
			mode = NFSV3ACCESS_READ;
		else
			mode = 0;
		if (vp->v_type == VDIR) {
			if (ap->a_mode & VWRITE)
				mode |= NFSV3ACCESS_MODIFY |
					NFSV3ACCESS_EXTEND | NFSV3ACCESS_DELETE;
			if (ap->a_mode & VEXEC)
				mode |= NFSV3ACCESS_LOOKUP;
		} else {
			if (ap->a_mode & VWRITE)
				mode |= NFSV3ACCESS_MODIFY | NFSV3ACCESS_EXTEND;
			if (ap->a_mode & VEXEC)
				mode |= NFSV3ACCESS_EXECUTE;
		}
		/* XXX safety belt, only make blanket request if caching */
		if (nfsaccess_cache_timeout > 0) {
			wmode = NFSV3ACCESS_READ | NFSV3ACCESS_MODIFY |
				NFSV3ACCESS_EXTEND | NFSV3ACCESS_EXECUTE |
				NFSV3ACCESS_DELETE | NFSV3ACCESS_LOOKUP;
		} else
			wmode = mode;
                
		/*
		 * Does our cached result allow us to give a definite yes to
		 * this request?
		 */     
		if (time_second < np->n_modestamp + nfsaccess_cache_timeout &&
		    ap->a_cred->cr_uid == np->n_modeuid &&
		    (np->n_mode & mode) == mode) {
			/* nfsstats.accesscache_hits++; */
		} else {
			/*
			 * Either a no, or a don't know.  Go to the wire.
			 */
			/* nfsstats.accesscache_misses++; */
			error = nfs3_access_otw(vp, wmode, ap->a_p,ap->a_cred);
			if (!error) {
				if ((np->n_mode & mode) != mode)
					error = EACCES;
			}
		}
	} else
		return (nfsspec_access(ap)); /* NFSv2 case checks for EROFS here */
>>>>>>> origin/10.1
	/*
	 * Convert KAUTH primitives to NFS access rights.
	 */
<<<<<<< HEAD
	access = 0;
	if (vnode_isdir(vp)) {
		/* directory */
		if (ap->a_action &
		    (KAUTH_VNODE_LIST_DIRECTORY |
		    KAUTH_VNODE_READ_EXTATTRIBUTES))
			access |= NFS_ACCESS_READ;
		if (ap->a_action & KAUTH_VNODE_SEARCH)
			access |= NFS_ACCESS_LOOKUP;
		if (ap->a_action &
		    (KAUTH_VNODE_ADD_FILE |
		    KAUTH_VNODE_ADD_SUBDIRECTORY))
			access |= NFS_ACCESS_MODIFY | NFS_ACCESS_EXTEND;
		if (ap->a_action & KAUTH_VNODE_DELETE_CHILD)
			access |= NFS_ACCESS_MODIFY;
	} else {
		/* file */
		if (ap->a_action &
		    (KAUTH_VNODE_READ_DATA |
		    KAUTH_VNODE_READ_EXTATTRIBUTES))
			access |= NFS_ACCESS_READ;
		if (ap->a_action & KAUTH_VNODE_WRITE_DATA)
			access |= NFS_ACCESS_MODIFY | NFS_ACCESS_EXTEND;
		if (ap->a_action & KAUTH_VNODE_APPEND_DATA)
			access |= NFS_ACCESS_EXTEND;
		if (ap->a_action & KAUTH_VNODE_EXECUTE)
			access |= NFS_ACCESS_EXECUTE;
	}
	/* common */
	if (ap->a_action & KAUTH_VNODE_DELETE)
		access |= NFS_ACCESS_DELETE;
	if (ap->a_action &
	    (KAUTH_VNODE_WRITE_ATTRIBUTES |
	    KAUTH_VNODE_WRITE_EXTATTRIBUTES |
	    KAUTH_VNODE_WRITE_SECURITY))
		access |= NFS_ACCESS_MODIFY;
	/* XXX this is pretty dubious */
	if (ap->a_action & KAUTH_VNODE_CHANGE_OWNER)
		access |= NFS_ACCESS_MODIFY;

	/* if caching, always ask for every right */
	if (nfs_access_cache_timeout > 0) {
		waccess = NFS_ACCESS_READ | NFS_ACCESS_MODIFY |
			NFS_ACCESS_EXTEND | NFS_ACCESS_EXECUTE |
			NFS_ACCESS_DELETE | NFS_ACCESS_LOOKUP;
	} else {
		waccess = access;
	}

	if ((error = nfs_node_lock(np)))
		return (error);

=======
	if (!error && (ap->a_mode & VWRITE) && (vp->v_mount->mnt_flag & MNT_RDONLY)) {
		switch (vp->v_type) {
			case VREG: case VDIR: case VLNK:
				error = EROFS;
			default: 
				break;
		}
	}
	return (error);
}

/*
 * nfs open vnode op
 * Check to see if the type is ok
 * and that deletion is not in progress.
 * For paged in text files, you will need to flush the page cache
 * if consistency is lost.
 */
/* ARGSUSED */

static int
nfs_open(ap)
	struct vop_open_args /* {
		struct vnode *a_vp;
		int  a_mode;
		struct ucred *a_cred;
		struct proc *a_p;
	} */ *ap;
{
	register struct vnode *vp = ap->a_vp;
	struct nfsnode *np = VTONFS(vp);
	struct nfsmount *nmp = VFSTONFS(vp->v_mount);
	struct vattr vattr;
	int error;

	if (vp->v_type != VREG && vp->v_type != VDIR && vp->v_type != VLNK) {
		return (EACCES);
	}
>>>>>>> origin/10.1
	/*
	 * Does our cached result allow us to give a definite yes to
	 * this request?
	 */
<<<<<<< HEAD
	if (auth_is_kerberized(np->n_auth) || auth_is_kerberized(nmp->nm_auth))
		uid = nfs_cred_getasid2uid(vfs_context_ucred(ctx));
	else
		uid = kauth_cred_getuid(vfs_context_ucred(ctx));
	slot = nfs_node_access_slot(np, uid, 0);
	dorpc = 1;
	if (access == 0) {
		/* not asking for any rights understood by NFS, so don't bother doing an RPC */
		/* OSAddAtomic(1, &nfsstats.accesscache_hits); */
		dorpc = 0;
		waccess = 0;
	} else if (NACCESSVALID(np, slot)) {
		microuptime(&now);
		if (((now.tv_sec < (np->n_accessstamp[slot] + nfs_access_cache_timeout)) &&
		    ((np->n_access[slot] & access) == access)) || nfs_use_cache(nmp)) {
			/* OSAddAtomic(1, &nfsstats.accesscache_hits); */
			dorpc = 0;
			waccess = np->n_access[slot];
		}
	}
	nfs_node_unlock(np);
	if (dorpc) {
		/* Either a no, or a don't know.  Go to the wire. */
		/* OSAddAtomic(1, &nfsstats.accesscache_misses); */

		/*
		 * Allow an access call to timeout if we have it cached
		 * so we won't hang if the server isn't responding.
		 */
		if (NACCESSVALID(np, slot))
			rpcflags |= R_SOFT;

<<<<<<< HEAD
		error = nmp->nm_funcs->nf_access_rpc(np, &waccess, rpcflags, ctx);
=======
	if (nmp->nm_flag & NFSMNT_NQNFS) {
		if (NQNFS_CKINVALID(vp, np, ND_READ)) {
		    do {
			error = nqnfs_getlease(vp, ND_READ, ap->a_cred,
			    ap->a_p);
		    } while (error == NQNFS_EXPIRED);
		    if (error)
			return (error);
		    if (np->n_lrev != np->n_brev ||
			(np->n_flag & NQNFSNONCACHE)) {
			if ((error = nfs_vinvalbuf(vp, V_SAVE, ap->a_cred,
				ap->a_p, 1)) == EINTR)
				return (error);
			np->n_brev = np->n_lrev;
		    }
		}
	} else {
		if (np->n_flag & NMODIFIED) {
			if ((error = nfs_vinvalbuf(vp, V_SAVE, ap->a_cred,
				ap->a_p, 1)) == EINTR)
				return (error);
			np->n_xid = 0;
			if (vp->v_type == VDIR)
				np->n_direofoffset = 0;
			error = VOP_GETATTR(vp, &vattr, ap->a_cred, ap->a_p);
			if (error)
				return (error);
			if (vp->v_type == VDIR) {
				/* if directory changed, purge any name cache entries */
				if (np->n_ncmtime != vattr.va_mtime.tv_sec)
					cache_purge(vp);
				np->n_ncmtime = vattr.va_mtime.tv_sec;
			}
			np->n_mtime = vattr.va_mtime.tv_sec;
		} else {
			error = VOP_GETATTR(vp, &vattr, ap->a_cred, ap->a_p);
			if (error)
				return (error);
			if (np->n_mtime != vattr.va_mtime.tv_sec) {
				if (vp->v_type == VDIR) {
					np->n_direofoffset = 0;
					nfs_invaldir(vp);
					/* purge name cache entries */
					if (np->n_ncmtime != vattr.va_mtime.tv_sec)
						cache_purge(vp);
				}
				if ((error = nfs_vinvalbuf(vp, V_SAVE,
					ap->a_cred, ap->a_p, 1)) == EINTR)
					return (error);
				if (vp->v_type == VDIR)
					np->n_ncmtime = vattr.va_mtime.tv_sec;
				np->n_mtime = vattr.va_mtime.tv_sec;
			}
		}
	}
	if ((nmp->nm_flag & NFSMNT_NQNFS) == 0)
		np->n_xid = 0; /* For Open/Close consistency */
	return (0);
}

/*
 * nfs close vnode op
 * What an NFS client should do upon close after writing is a debatable issue.
 * Most NFS clients push delayed writes to the server upon close, basically for
 * two reasons:
 * 1 - So that any write errors may be reported back to the client process
 *     doing the close system call. By far the two most likely errors are
 *     NFSERR_NOSPC and NFSERR_DQUOT to indicate space allocation failure.
 * 2 - To put a worst case upper bound on cache inconsistency between
 *     multiple clients for the file.
 * There is also a consistency problem for Version 2 of the protocol w.r.t.
 * not being able to tell if other clients are writing a file concurrently,
 * since there is no way of knowing if the changed modify time in the reply
 * is only due to the write for this client.
 * (NFS Version 3 provides weak cache consistency data in the reply that
 *  should be sufficient to detect and handle this case.)
 *
 * The current code does the following:
 * for NFS Version 2 - play it safe and flush/invalidate all dirty buffers
 * for NFS Version 3 - flush dirty buffers to the server but don't invalidate
 *                     them.
 * for NQNFS         - do nothing now, since 2 is dealt with via leases and
 *                     1 should be dealt with via an fsync() system call for
 *                     cases where write errors are important.
 */
/* ARGSUSED */
static int
nfs_close(ap)
	struct vop_close_args /* {
		struct vnodeop_desc *a_desc;
		struct vnode *a_vp;
		int  a_fflag;
		struct ucred *a_cred;
		struct proc *a_p;
	} */ *ap;
{
	register struct vnode *vp = ap->a_vp;
	register struct nfsnode *np = VTONFS(vp);
	struct nfsmount *nmp;
	int error = 0;
>>>>>>> origin/10.3

		/*
		 * If the server didn't respond return the cached access.
		 */
		if ((error == ETIMEDOUT) && (rpcflags & R_SOFT)) {
			error = 0;
			waccess = np->n_access[slot];
		}
=======
	if (vp->v_type == VREG) {
#if DIAGNOSTIC
	    register struct sillyrename *sp = np->n_sillyrename;
	    if (sp)
                kprintf("nfs_close: %s, dvp=%x, vp=%x, ap=%x, np=%x, sp=%x\n",
                	&sp->s_name[0], (unsigned)(sp->s_dvp), (unsigned)vp,
                	(unsigned)ap, (unsigned)np, (unsigned)sp);
#endif
	    if ((VFSTONFS(vp->v_mount)->nm_flag & NFSMNT_NQNFS) == 0 &&
		(np->n_flag & NMODIFIED)) {
		if (NFS_ISV3(vp)) {
		    error = nfs_flush(vp, ap->a_cred, MNT_WAIT, ap->a_p);
                    /*
                     * We cannot clear the NMODIFIED bit in np->n_flag due to
                     * potential races with other processes (and because
                     * the commit arg is 0 in the nfs_flush call above.)
		     * NMODIFIED is a hint
                     */
		    /* np->n_flag &= ~NMODIFIED; */
		} else
		    error = nfs_vinvalbuf(vp, V_SAVE, ap->a_cred, ap->a_p, 1);
<<<<<<< HEAD
		np->n_attrstamp = 0;
=======
		}
		np->n_xid = 0;
		if (getlock)
			VOP_UNLOCK(vp, 0, ap->a_p);
>>>>>>> origin/10.3
	    }
	    if (np->n_flag & NWRITEERR) {
		np->n_flag &= ~NWRITEERR;
		error = np->n_error;
	    }
>>>>>>> origin/10.1
	}
	if (!error && ((waccess & access) != access))
		error = EACCES;

	return (error);
}


/*
 * NFS open vnode op
 *
 * Perform various update/invalidation checks and then add the
 * open to the node.  Regular files will have an open file structure
 * on the node and, for NFSv4, perform an OPEN request on the server.
 */
int
nfs_vnop_open(
	struct vnop_open_args /* {
		struct vnodeop_desc *a_desc;
		vnode_t a_vp;
		int a_mode;
		vfs_context_t a_context;
	} */ *ap)
{
<<<<<<< HEAD
	vfs_context_t ctx = ap->a_context;
	vnode_t vp = ap->a_vp;
	nfsnode_t np = VTONFS(vp);
	struct nfsmount *nmp;
	int error, accessMode, denyMode, opened = 0;
	struct nfs_open_owner *noop = NULL;
	struct nfs_open_file *nofp = NULL;
	enum vtype vtype;

	if (!(ap->a_mode & (FREAD|FWRITE)))
		return (EINVAL);

	nmp = VTONMP(vp);
	if (nfs_mount_gone(nmp))
		return (ENXIO);
	if (np->n_flag & NREVOKE)
		return (EIO);

	vtype = vnode_vtype(vp);
	if ((vtype != VREG) && (vtype != VDIR) && (vtype != VLNK))
		return (EACCES);

	/* First, check if we need to update/invalidate */
	if (ISSET(np->n_flag, NUPDATESIZE))
		nfs_data_update_size(np, 0);
	if ((error = nfs_node_lock(np)))
		return (error);
	if (np->n_flag & NNEEDINVALIDATE) {
		np->n_flag &= ~NNEEDINVALIDATE;
		if (vtype == VDIR)
			nfs_invaldir(np);
		nfs_node_unlock(np);
		nfs_vinvalbuf(vp, V_SAVE|V_IGNORE_WRITEERR, ctx, 1);
		if ((error = nfs_node_lock(np)))
=======
	register struct vnode *vp = ap->a_vp;
	register struct nfsnode *np = VTONFS(vp);
	register caddr_t cp;
	register u_long *tl;
	register int t1, t2;
	caddr_t bpos, dpos;
	int error = 0;
	struct mbuf *mreq, *mrep, *md, *mb, *mb2;
	int v3 = NFS_ISV3(vp);
	u_int64_t xid;
	int avoidfloods;
	
	FSDBG_TOP(513, np->n_size, np, np->n_vattr.va_size, np->n_flag);
	/*
	 * Update local times for special files.
	 */
	if (np->n_flag & (NACC | NUPD))
		np->n_flag |= NCHG;
	/*
	 * First look in the cache.
	 */
	if ((error = nfs_getattrcache(vp, ap->a_vap)) == 0) {
		FSDBG_BOT(513, np->n_size, 0, np->n_vattr.va_size, np->n_flag);
		return (0);
	}
	if (error != ENOENT) {
		FSDBG_BOT(513, np->n_size, error, np->n_vattr.va_size,
			  np->n_flag);
		return (error);
	}
	error = 0;

	if (v3 && nfsaccess_cache_timeout > 0) {
		/*  nfsstats.accesscache_misses++; */
		if (error = nfs3_access_otw(vp, NFSV3ACCESS_ALL, ap->a_p,
					    ap->a_cred))
			return (error);
		if ((error = nfs_getattrcache(vp, ap->a_vap)) == 0)
			return (0);
		if (error != ENOENT)
>>>>>>> origin/10.1
			return (error);
	}
<<<<<<< HEAD
	if (vtype == VREG)
		np->n_lastrahead = -1;
	if (np->n_flag & NMODIFIED) {
		if (vtype == VDIR)
			nfs_invaldir(np);
		nfs_node_unlock(np);
		if ((error = nfs_vinvalbuf(vp, V_SAVE|V_IGNORE_WRITEERR, ctx, 1)))
			return (error);
	} else {
		nfs_node_unlock(np);
=======
	avoidfloods = 0;
tryagain:
	nfsstats.rpccnt[NFSPROC_GETATTR]++;
	nfsm_reqhead(vp, NFSPROC_GETATTR, NFSX_FH(v3));
	nfsm_fhtom(vp, v3);
	nfsm_request(vp, NFSPROC_GETATTR, ap->a_p, ap->a_cred, &xid);
	if (!error) {
		nfsm_loadattr(vp, ap->a_vap, &xid);
		if (!xid) { /* out-of-order rpc - attributes were dropped */
			m_freem(mrep);
			mrep = NULL;
			FSDBG(513, -1, np, np->n_xid << 32, np->n_xid);
			if (avoidfloods++ < 100)
				goto tryagain;
			/*
			 * avoidfloods>1 is bizarre.  at 100 pull the plug
			 */
			panic("nfs_getattr: getattr flood\n");
		}
		if (np->n_mtime != ap->a_vap->va_mtime.tv_sec) {
			FSDBG(513, -1, np, -1, vp);
			if (vp->v_type == VDIR)
				nfs_invaldir(vp);
<<<<<<< HEAD
			error = nfs_vinvalbuf(vp, V_SAVE, ap->a_cred,
					      ap->a_p, 1);
			FSDBG(513, -1, np, -2, error);
			if (!error)
=======
				/* purge name cache entries */
				if (np->n_ncmtime != ap->a_vap->va_mtime.tv_sec)
					cache_purge(vp);
			}
			error = nfs_vinvalbuf(vp, V_SAVE, ap->a_cred,
					      ap->a_p, 1);
			FSDBG(513, -1, np, -2, error);
			if (!error) {
				if (vp->v_type == VDIR)
					np->n_ncmtime = ap->a_vap->va_mtime.tv_sec;
>>>>>>> origin/10.3
				np->n_mtime = ap->a_vap->va_mtime.tv_sec;
		}
>>>>>>> origin/10.1
	}

<<<<<<< HEAD
	/* nfs_getattr() will check changed and purge caches */
	if ((error = nfs_getattr(np, NULL, ctx, NGA_UNCACHED)))
		return (error);

	if (vtype != VREG) {
		/* Just mark that it was opened */
		lck_mtx_lock(&np->n_openlock);
		np->n_openrefcnt++;
		lck_mtx_unlock(&np->n_openlock);
		return (0);
	}

	/* mode contains some combination of: FREAD, FWRITE, O_SHLOCK, O_EXLOCK */
	accessMode = 0;
	if (ap->a_mode & FREAD)
		accessMode |= NFS_OPEN_SHARE_ACCESS_READ;
	if (ap->a_mode & FWRITE)
		accessMode |= NFS_OPEN_SHARE_ACCESS_WRITE;
	if (ap->a_mode & O_EXLOCK)
		denyMode = NFS_OPEN_SHARE_DENY_BOTH;
	else if (ap->a_mode & O_SHLOCK)
		denyMode = NFS_OPEN_SHARE_DENY_WRITE;
	else
		denyMode = NFS_OPEN_SHARE_DENY_NONE;
	// XXX don't do deny modes just yet (and never do it for !v4)
	denyMode = NFS_OPEN_SHARE_DENY_NONE;

	noop = nfs_open_owner_find(nmp, vfs_context_ucred(ctx), 1);
	if (!noop)
		return (ENOMEM);

restart:
	error = nfs_mount_state_in_use_start(nmp, vfs_context_thread(ctx));
	if (error) {
		nfs_open_owner_rele(noop);
		return (error);
	}
	if (np->n_flag & NREVOKE) {
		error = EIO;
		nfs_mount_state_in_use_end(nmp, 0);
		nfs_open_owner_rele(noop);
		return (error);
	}

	error = nfs_open_file_find(np, noop, &nofp, accessMode, denyMode, 1);
	if (!error && (nofp->nof_flags & NFS_OPEN_FILE_LOST)) {
		NP(np, "nfs_vnop_open: LOST %d", kauth_cred_getuid(nofp->nof_owner->noo_cred));
		error = EIO;
	}
	if (!error && (nofp->nof_flags & NFS_OPEN_FILE_REOPEN)) {
		nfs_mount_state_in_use_end(nmp, 0);
		error = nfs4_reopen(nofp, vfs_context_thread(ctx));
		nofp = NULL;
		if (!error)
			goto restart;
	}
	if (!error)
		error = nfs_open_file_set_busy(nofp, vfs_context_thread(ctx));
	if (error) {
		nofp = NULL;
		goto out;
	}

	if (nmp->nm_vers < NFS_VER4) {
		/*
		 * NFS v2/v3 opens are always allowed - so just add it.
		 */
		nfs_open_file_add_open(nofp, accessMode, denyMode, 0);
		goto out;
	}

	/*
	 * If we just created the file and the modes match, then we simply use
	 * the open performed in the create.  Otherwise, send the request.
	 */
	if ((nofp->nof_flags & NFS_OPEN_FILE_CREATE) &&
	    (nofp->nof_creator == current_thread()) &&
	    (accessMode == NFS_OPEN_SHARE_ACCESS_BOTH) &&
	    (denyMode == NFS_OPEN_SHARE_DENY_NONE)) {
		nofp->nof_flags &= ~NFS_OPEN_FILE_CREATE;
		nofp->nof_creator = NULL;
	} else {
		if (!opened)
			error = nfs4_open(np, nofp, accessMode, denyMode, ctx);
		if ((error == EACCES) && (nofp->nof_flags & NFS_OPEN_FILE_CREATE) &&
		    (nofp->nof_creator == current_thread())) {
			/*
			 * Ugh.  This can happen if we just created the file with read-only
			 * perms and we're trying to open it for real with different modes
			 * (e.g. write-only or with a deny mode) and the server decides to
			 * not allow the second open because of the read-only perms.
			 * The best we can do is to just use the create's open.
			 * We may have access we don't need or we may not have a requested
			 * deny mode.  We may log complaints later, but we'll try to avoid it.
			 */
			if (denyMode != NFS_OPEN_SHARE_DENY_NONE)
				NP(np, "nfs_vnop_open: deny mode foregone on create, %d", kauth_cred_getuid(nofp->nof_owner->noo_cred));
			nofp->nof_creator = NULL;
			error = 0;
		}
		if (error)
			goto out;
		opened = 1;
		/*
		 * If we had just created the file, we already had it open.
		 * If the actual open mode is less than what we grabbed at
		 * create time, then we'll downgrade the open here.
		 */
		if ((nofp->nof_flags & NFS_OPEN_FILE_CREATE) &&
		    (nofp->nof_creator == current_thread())) {
			error = nfs_close(np, nofp, NFS_OPEN_SHARE_ACCESS_BOTH, NFS_OPEN_SHARE_DENY_NONE, ctx);
			if (error)
				NP(np, "nfs_vnop_open: create close error %d, %d", error, kauth_cred_getuid(nofp->nof_owner->noo_cred));
			if (!nfs_mount_state_error_should_restart(error)) {
				error = 0;
				nofp->nof_flags &= ~NFS_OPEN_FILE_CREATE;
			}
		}
	}

out:
	if (nofp)
		nfs_open_file_clear_busy(nofp);
	if (nfs_mount_state_in_use_end(nmp, error)) {
		nofp = NULL;
		goto restart;
	}
	if (error)
		NP(np, "nfs_vnop_open: error %d, %d", error, kauth_cred_getuid(noop->noo_cred));
	if (noop)
		nfs_open_owner_rele(noop);
	if (!error && vtype == VREG && (ap->a_mode & FWRITE)) {
		lck_mtx_lock(&nmp->nm_lock);
		nmp->nm_state &= ~NFSSTA_SQUISHY;
		nmp->nm_curdeadtimeout = nmp->nm_deadtimeout;
		if (nmp->nm_curdeadtimeout <= 0)
			nmp->nm_deadto_start = 0;
		nmp->nm_writers++;
		lck_mtx_unlock(&nmp->nm_lock);
	}
		
	return (error);
}

static uint32_t
nfs_no_of_open_file_writers(nfsnode_t np)
{
	uint32_t writers = 0;
	struct nfs_open_file *nofp;

	TAILQ_FOREACH(nofp,  &np->n_opens, nof_link) {
		writers += nofp->nof_w + nofp->nof_rw + nofp->nof_w_dw + nofp->nof_rw_dw +
			nofp->nof_w_drw + nofp->nof_rw_drw + nofp->nof_d_w_dw +
			nofp->nof_d_rw_dw + nofp->nof_d_w_drw + nofp->nof_d_rw_drw +
			nofp->nof_d_w + nofp->nof_d_rw;
	}
	
	return (writers);
}
=======
	FSDBG_BOT(513, np->n_size, -1, np->n_vattr.va_size, error);
	return (error);
}
>>>>>>> origin/10.1

/*
 * NFS close vnode op
 *
 * What an NFS client should do upon close after writing is a debatable issue.
 * Most NFS clients push delayed writes to the server upon close, basically for
 * two reasons:
 * 1 - So that any write errors may be reported back to the client process
 *     doing the close system call. By far the two most likely errors are
 *     NFSERR_NOSPC and NFSERR_DQUOT to indicate space allocation failure.
 * 2 - To put a worst case upper bound on cache inconsistency between
 *     multiple clients for the file.
 * There is also a consistency problem for Version 2 of the protocol w.r.t.
 * not being able to tell if other clients are writing a file concurrently,
 * since there is no way of knowing if the changed modify time in the reply
 * is only due to the write for this client.
 * (NFS Version 3 provides weak cache consistency data in the reply that
 *  should be sufficient to detect and handle this case.)
 *
 * The current code does the following:
 * for NFS Version 2 - play it safe and flush/invalidate all dirty buffers
 * for NFS Version 3 - flush dirty buffers to the server but don't invalidate them.
 * for NFS Version 4 - basically the same as NFSv3
 */
int
nfs_vnop_close(
	struct vnop_close_args /* {
		struct vnodeop_desc *a_desc;
		vnode_t a_vp;
		int a_fflag;
		vfs_context_t a_context;
	} */ *ap)
{
	vfs_context_t ctx = ap->a_context;
	vnode_t vp = ap->a_vp;
	nfsnode_t np = VTONFS(vp);
	struct nfsmount *nmp;
	int error = 0, error1, nfsvers;
	int fflag = ap->a_fflag;
	enum vtype vtype;
	int accessMode, denyMode;
	struct nfs_open_owner *noop = NULL;
	struct nfs_open_file *nofp = NULL;

	nmp = VTONMP(vp);
	if (!nmp)
		return (ENXIO);
	nfsvers = nmp->nm_vers;
	vtype = vnode_vtype(vp);

	/* First, check if we need to update/flush/invalidate */
	if (ISSET(np->n_flag, NUPDATESIZE))
		nfs_data_update_size(np, 0);
	nfs_node_lock_force(np);
	if (np->n_flag & NNEEDINVALIDATE) {
		np->n_flag &= ~NNEEDINVALIDATE;
		nfs_node_unlock(np);
		nfs_vinvalbuf(vp, V_SAVE|V_IGNORE_WRITEERR, ctx, 1);
		nfs_node_lock_force(np);
	}
	if ((vtype == VREG) && (np->n_flag & NMODIFIED) && (fflag & FWRITE)) {
		/* we're closing an open for write and the file is modified, so flush it */
		nfs_node_unlock(np);
		if (nfsvers != NFS_VER2)
			error = nfs_flush(np, MNT_WAIT, vfs_context_thread(ctx), 0);
		else
			error = nfs_vinvalbuf(vp, V_SAVE, ctx, 1);
		nfs_node_lock_force(np);
		NATTRINVALIDATE(np);
	}
	if (np->n_flag & NWRITEERR) {
		np->n_flag &= ~NWRITEERR;
		error = np->n_error;
	}
	nfs_node_unlock(np);

	if (vtype != VREG) {
		/* Just mark that it was closed */
		lck_mtx_lock(&np->n_openlock);
		if (np->n_openrefcnt == 0) {
			if (fflag & (FREAD|FWRITE)) {
				NP(np, "nfs_vnop_close: open reference underrun");
				error = EINVAL;
			}
		} else if (fflag & (FREAD|FWRITE)) {
			np->n_openrefcnt--;
		} else {
			/* No FREAD/FWRITE set - probably the final close */
			np->n_openrefcnt = 0;
		}
		lck_mtx_unlock(&np->n_openlock);
		return (error);
	}
	error1 = error;

	/* fflag should contain some combination of: FREAD, FWRITE, FHASLOCK */
	accessMode = 0;
	if (fflag & FREAD)
		accessMode |= NFS_OPEN_SHARE_ACCESS_READ;
	if (fflag & FWRITE)
		accessMode |= NFS_OPEN_SHARE_ACCESS_WRITE;
// XXX It would be nice if we still had the O_EXLOCK/O_SHLOCK flags that were on the open
//	if (fflag & O_EXLOCK)
//		denyMode = NFS_OPEN_SHARE_DENY_BOTH;
//	else if (fflag & O_SHLOCK)
//		denyMode = NFS_OPEN_SHARE_DENY_WRITE;
//	else
//		denyMode = NFS_OPEN_SHARE_DENY_NONE;
#if 0  // Not yet
	if (fflag & FHASLOCK) {
		/* XXX assume FHASLOCK is for the deny mode and not flock */
		/* FHASLOCK flock will be unlocked in the close path, but the flag is not cleared. */
		if (nofp->nof_deny & NFS_OPEN_SHARE_DENY_READ)
			denyMode = NFS_OPEN_SHARE_DENY_BOTH;
		else if (nofp->nof_deny & NFS_OPEN_SHARE_DENY_WRITE)
			denyMode = NFS_OPEN_SHARE_DENY_WRITE;
		else
			denyMode = NFS_OPEN_SHARE_DENY_NONE;
	} else {
			denyMode = NFS_OPEN_SHARE_DENY_NONE;
	}
#else
	// XXX don't do deny modes just yet (and never do it for !v4)
	denyMode = NFS_OPEN_SHARE_DENY_NONE;
#endif

<<<<<<< HEAD
	if (!accessMode) {
		/*
		 * No mode given to close?
		 * Guess this is the final close.
		 * We should unlock all locks and close all opens.
		 */
		uint32_t writers;
		mount_t mp = vnode_mount(vp);
		int force = (!mp || vfs_isforce(mp));

		writers = nfs_no_of_open_file_writers(np);
		nfs_release_open_state_for_node(np, force);
		if (writers) {
			lck_mtx_lock(&nmp->nm_lock);
			if (writers > nmp->nm_writers) {
				NP(np, "nfs_vnop_close: number of write opens for mount underrun. Node has %d"
				   " opens for write. Mount has total of %d opens for write\n", 
				   writers, nmp->nm_writers);
				nmp->nm_writers = 0;
			} else {
				nmp->nm_writers -= writers;
			}
			lck_mtx_unlock(&nmp->nm_lock);
		}
		
		return (error);
	} else if (fflag & FWRITE) {
		lck_mtx_lock(&nmp->nm_lock);
		if (nmp->nm_writers == 0) {
			NP(np, "nfs_vnop_close: removing open writer from mount, but mount has no files open for writing");
		} else {
			nmp->nm_writers--;
		}
		lck_mtx_unlock(&nmp->nm_lock);
	}
	

	noop = nfs_open_owner_find(nmp, vfs_context_ucred(ctx), 0);
	if (!noop) {
		// printf("nfs_vnop_close: can't get open owner!\n");
		return (EIO);
	}

restart:
	error = nfs_mount_state_in_use_start(nmp, NULL);
	if (error) {
		nfs_open_owner_rele(noop);
		return (error);
	}

	error = nfs_open_file_find(np, noop, &nofp, 0, 0, 0);
	if (!error && (nofp->nof_flags & NFS_OPEN_FILE_REOPEN)) {
		nfs_mount_state_in_use_end(nmp, 0);
		error = nfs4_reopen(nofp, NULL);
		nofp = NULL;
		if (!error)
			goto restart;
	}
	if (error) {
		NP(np, "nfs_vnop_close: no open file for owner, error %d, %d", error, kauth_cred_getuid(noop->noo_cred));
		error = EBADF;
		goto out;
	}
	error = nfs_open_file_set_busy(nofp, NULL);
	if (error) {
		nofp = NULL;
		goto out;
	}

	error = nfs_close(np, nofp, accessMode, denyMode, ctx);
	if (error)
		NP(np, "nfs_vnop_close: close error %d, %d", error, kauth_cred_getuid(noop->noo_cred));

out:
	if (nofp)
		nfs_open_file_clear_busy(nofp);
	if (nfs_mount_state_in_use_end(nmp, error)) {
		nofp = NULL;
		goto restart;
	}
	if (!error)
		error = error1;
	if (error)
		NP(np, "nfs_vnop_close: error %d, %d", error, kauth_cred_getuid(noop->noo_cred));
	if (noop)
		nfs_open_owner_rele(noop);
	return (error);
}

/*
 * nfs_close(): common function that does all the heavy lifting of file closure
 *
 * Takes an open file structure and a set of access/deny modes and figures out how
 * to update the open file structure (and the state on the server) appropriately.
 */
int
nfs_close(
	nfsnode_t np,
	struct nfs_open_file *nofp,
	uint32_t accessMode,
	uint32_t denyMode,
	vfs_context_t ctx)
{
	struct nfs_lock_owner *nlop;
	int error = 0, changed = 0, delegated = 0, closed = 0, downgrade = 0;
	uint32_t newAccessMode, newDenyMode;
	
	/* warn if modes don't match current state */
	if (((accessMode & nofp->nof_access) != accessMode) || ((denyMode & nofp->nof_deny) != denyMode))
		NP(np, "nfs_close: mode mismatch %d %d, current %d %d, %d",
			accessMode, denyMode, nofp->nof_access, nofp->nof_deny,
			kauth_cred_getuid(nofp->nof_owner->noo_cred));
=======
#ifdef XXX /* enable this code soon! (but test it first) */
	/*		  
	 * Setting of flags is not supported.
	 */
	if (vap->va_flags != VNOVAL)
		return (EOPNOTSUPP);
#endif
>>>>>>> origin/10.1

	/*
	 * If we're closing a write-only open, we may not have a write-only count
	 * if we also grabbed read access.  So, check the read-write count.
	 */
	if (denyMode == NFS_OPEN_SHARE_DENY_NONE) {
		if ((accessMode == NFS_OPEN_SHARE_ACCESS_WRITE) &&
		    (nofp->nof_w == 0) && (nofp->nof_d_w == 0) &&
		    (nofp->nof_rw || nofp->nof_d_rw))
			accessMode = NFS_OPEN_SHARE_ACCESS_BOTH;
	} else if (denyMode == NFS_OPEN_SHARE_DENY_WRITE) {
		if ((accessMode == NFS_OPEN_SHARE_ACCESS_WRITE) &&
		    (nofp->nof_w_dw == 0) && (nofp->nof_d_w_dw == 0) &&
		    (nofp->nof_rw_dw || nofp->nof_d_rw_dw))
			accessMode = NFS_OPEN_SHARE_ACCESS_BOTH;
	} else { /* NFS_OPEN_SHARE_DENY_BOTH */
		if ((accessMode == NFS_OPEN_SHARE_ACCESS_WRITE) &&
		    (nofp->nof_w_drw == 0) && (nofp->nof_d_w_drw == 0) &&
		    (nofp->nof_rw_drw || nofp->nof_d_rw_drw))
			accessMode = NFS_OPEN_SHARE_ACCESS_BOTH;
	}

	nfs_open_file_remove_open_find(nofp, accessMode, denyMode, &newAccessMode, &newDenyMode, &delegated);
	if ((newAccessMode != nofp->nof_access) || (newDenyMode != nofp->nof_deny))
		changed = 1;
	else
		changed = 0;

	if (NFSTONMP(np)->nm_vers < NFS_VER4) /* NFS v2/v3 closes simply need to remove the open. */
		goto v3close;

	if ((newAccessMode == 0) || (nofp->nof_opencnt == 1)) {
		/*
		 * No more access after this close, so clean up and close it.
		 * Don't send a close RPC if we're closing a delegated open.
		 */
		nfs_wait_bufs(np);
		closed = 1;
		if (!delegated && !(nofp->nof_flags & NFS_OPEN_FILE_LOST))
			error = nfs4_close_rpc(np, nofp, vfs_context_thread(ctx), vfs_context_ucred(ctx), 0);
		if (error == NFSERR_LOCKS_HELD) {
			/*
			 * Hmm... the server says we have locks we need to release first
			 * Find the lock owner and try to unlock everything.
			 */
<<<<<<< HEAD
<<<<<<< HEAD
			nlop = nfs_lock_owner_find(np, vfs_context_proc(ctx), 0);
			if (nlop) {
				nfs4_unlock_rpc(np, nlop, F_WRLCK, 0, UINT64_MAX,
					0, vfs_context_thread(ctx), vfs_context_ucred(ctx));
				nfs_lock_owner_rele(nlop);
=======
			if (vp->v_mount->mnt_flag & MNT_RDONLY)
				return (EROFS);
			FSDBG_TOP(512, np->n_size, vap->va_size,
				  np->n_vattr.va_size, np->n_flag);
			if (np->n_flag & NMODIFIED) {
 				if (vap->va_size == 0)
 					error = nfs_vinvalbuf(vp, 0,
 						ap->a_cred, ap->a_p, 1);
 				else
 					error = nfs_vinvalbuf(vp, V_SAVE,
 						ap->a_cred, ap->a_p, 1);
	 			if (error) {
					printf("nfs_setattr: nfs_vinvalbuf %d\n", error);
					FSDBG_BOT(512, np->n_size, vap->va_size,
						  np->n_vattr.va_size, -1);
 					return (error);
				}
			} else if (np->n_size > vap->va_size) { /* shrinking? */
				daddr_t obn, bn;
				int biosize;
				struct nfsbuf *bp;

				biosize = vp->v_mount->mnt_stat.f_iosize;
				obn = (np->n_size - 1) / biosize;
				bn = vap->va_size / biosize; 
				for ( ; obn >= bn; obn--)
				    if (nfs_buf_incore(vp, obn)) {
					bp = nfs_buf_get(vp, obn, biosize, 0, BLK_READ);
					if (!bp)
						continue;
					if (obn == bn) {
					    int neweofoff, mustwrite;
					    mustwrite = 0;
					    neweofoff = vap->va_size - NBOFF(bp);
					    /* check for any dirty data before the new EOF */
					    if (bp->nb_dirtyend && bp->nb_dirtyoff < neweofoff) {
						/* clip dirty range to EOF */
						if (bp->nb_dirtyend > neweofoff)
						    bp->nb_dirtyend = neweofoff;
						mustwrite++;
					    }
					    bp->nb_dirty &= (1 << round_page_32(neweofoff)/PAGE_SIZE) - 1;
					    if (bp->nb_dirty)
						mustwrite++;
					    if (mustwrite) {
						/* gotta write out dirty data before invalidating */
						/* (NB_STABLE indicates that data writes should be FILESYNC) */
						/* (NB_NOCACHE indicates buffer should be discarded) */
						CLR(bp->nb_flags, (NB_DONE | NB_ERROR | NB_INVAL | NB_ASYNC | NB_READ));
						SET(bp->nb_flags, NB_STABLE | NB_NOCACHE);
						/*
						 * NFS has embedded ucred so crhold() risks zone corruption
						 */
						if (bp->nb_wcred == NOCRED)
						    bp->nb_wcred = crdup(ap->a_cred);
						error = nfs_buf_write(bp);
						// Note: bp has been released
						if (error) {
						    FSDBG(512, bp, 0xd00dee, 0xbad, error);
						    np->n_error = error;
						    np->n_flag |= NWRITEERR;
						    error = 0;
						}
						bp = NULL;
					    }
					}
					if (bp) {
					    FSDBG(512, bp, bp->nb_flags, 0, obn);
					    SET(bp->nb_flags, NB_INVAL);
					    nfs_buf_release(bp, 1);
					}
				    }
>>>>>>> origin/10.3
			}
			error = nfs4_close_rpc(np, nofp, vfs_context_thread(ctx), vfs_context_ucred(ctx), 0);
		}
	} else if (changed) {
		/*
		 * File is still open but with less access, so downgrade the open.
		 * Don't send a downgrade RPC if we're closing a delegated open.
		 */
		if (!delegated && !(nofp->nof_flags & NFS_OPEN_FILE_LOST)) {
			downgrade = 1;
			/*
			 * If we have delegated opens, we should probably claim them before sending
			 * the downgrade because the server may not know the open we are downgrading to.
			 */
			if (nofp->nof_d_rw_drw || nofp->nof_d_w_drw || nofp->nof_d_r_drw ||
			    nofp->nof_d_rw_dw || nofp->nof_d_w_dw || nofp->nof_d_r_dw ||
			    nofp->nof_d_rw || nofp->nof_d_w || nofp->nof_d_r)
				nfs4_claim_delegated_state_for_open_file(nofp, 0);
			/* need to remove the open before sending the downgrade */
			nfs_open_file_remove_open(nofp, accessMode, denyMode);
			error = nfs4_open_downgrade_rpc(np, nofp, ctx);
			if (error) /* Hmm.. that didn't work. Add the open back in. */
				nfs_open_file_add_open(nofp, accessMode, denyMode, delegated);
		}
	}

	if (error) {
		NP(np, "nfs_close: error %d, %d", error, kauth_cred_getuid(nofp->nof_owner->noo_cred));
		return (error);
	}

v3close:
	if (!downgrade)
		nfs_open_file_remove_open(nofp, accessMode, denyMode);

	if (closed) {
		lck_mtx_lock(&nofp->nof_lock);
		if (nofp->nof_r || nofp->nof_d_r || nofp->nof_w || nofp->nof_d_w || nofp->nof_d_rw ||
		    (nofp->nof_rw && !((nofp->nof_flags & NFS_OPEN_FILE_CREATE) && !nofp->nof_creator && (nofp->nof_rw == 1))) ||
		    nofp->nof_r_dw || nofp->nof_d_r_dw || nofp->nof_w_dw || nofp->nof_d_w_dw ||
		    nofp->nof_rw_dw || nofp->nof_d_rw_dw || nofp->nof_r_drw || nofp->nof_d_r_drw ||
		    nofp->nof_w_drw || nofp->nof_d_w_drw || nofp->nof_rw_drw || nofp->nof_d_rw_drw)
			NP(np, "nfs_close: unexpected count: %u.%u %u.%u %u.%u dw %u.%u %u.%u %u.%u drw %u.%u %u.%u %u.%u flags 0x%x, %d",
				nofp->nof_r, nofp->nof_d_r, nofp->nof_w, nofp->nof_d_w,
				nofp->nof_rw, nofp->nof_d_rw, nofp->nof_r_dw, nofp->nof_d_r_dw,
				nofp->nof_w_dw, nofp->nof_d_w_dw, nofp->nof_rw_dw, nofp->nof_d_rw_dw,
				nofp->nof_r_drw, nofp->nof_d_r_drw, nofp->nof_w_drw, nofp->nof_d_w_drw,
				nofp->nof_rw_drw, nofp->nof_d_rw_drw, nofp->nof_flags,
				kauth_cred_getuid(nofp->nof_owner->noo_cred));
		/* clear out all open info, just to be safe */
		nofp->nof_access = nofp->nof_deny = 0;
		nofp->nof_mmap_access = nofp->nof_mmap_deny = 0;
		nofp->nof_r = nofp->nof_d_r = 0;
		nofp->nof_w = nofp->nof_d_w = 0;
		nofp->nof_rw = nofp->nof_d_rw = 0;
		nofp->nof_r_dw = nofp->nof_d_r_dw = 0;
		nofp->nof_w_dw = nofp->nof_d_w_dw = 0;
		nofp->nof_rw_dw = nofp->nof_d_rw_dw = 0;
		nofp->nof_r_drw = nofp->nof_d_r_drw = 0;
		nofp->nof_w_drw = nofp->nof_d_w_drw = 0;
		nofp->nof_rw_drw = nofp->nof_d_rw_drw = 0;
		nofp->nof_flags &= ~NFS_OPEN_FILE_CREATE;
		lck_mtx_unlock(&nofp->nof_lock);
		/* XXX we may potentially want to clean up idle/unused open file structures */
	}
	if (nofp->nof_flags & NFS_OPEN_FILE_LOST) {
		error = EIO;
		NP(np, "nfs_close: LOST%s, %d", !nofp->nof_opencnt ? " (last)" : "",
			kauth_cred_getuid(nofp->nof_owner->noo_cred));
	}
		
	return (error);
}


int
nfs3_getattr_rpc(
	nfsnode_t np,
	mount_t mp,
	u_char *fhp,
	size_t fhsize,
	int flags,
	vfs_context_t ctx,
	struct nfs_vattr *nvap,
	u_int64_t *xidp)
{
	struct nfsmount *nmp = mp ? VFSTONFS(mp) : NFSTONMP(np);
	int error = 0, status, nfsvers, rpcflags = 0;
	struct nfsm_chain nmreq, nmrep;

	if (nfs_mount_gone(nmp))
		return (ENXIO);
	nfsvers = nmp->nm_vers;

	if (flags & NGA_MONITOR) /* vnode monitor requests should be soft */
		rpcflags = R_RECOVER;

	if (flags & NGA_SOFT) /* Return ETIMEDOUT if server not responding */
		rpcflags |= R_SOFT;

	nfsm_chain_null(&nmreq);
	nfsm_chain_null(&nmrep);

	nfsm_chain_build_alloc_init(error, &nmreq, NFSX_FH(nfsvers));
	if (nfsvers != NFS_VER2)
		nfsm_chain_add_32(error, &nmreq, fhsize);
	nfsm_chain_add_opaque(error, &nmreq, fhp, fhsize);
	nfsm_chain_build_done(error, &nmreq);
	nfsmout_if(error);
	error = nfs_request2(np, mp, &nmreq, NFSPROC_GETATTR, 
			vfs_context_thread(ctx), vfs_context_ucred(ctx),
			NULL, rpcflags, &nmrep, xidp, &status);
	if (!error)
		error = status;
	nfsmout_if(error);
	error = nfs_parsefattr(&nmrep, nfsvers, nvap);
nfsmout:
	nfsm_chain_cleanup(&nmreq);
	nfsm_chain_cleanup(&nmrep);
	return (error);
}

/*
 * nfs_refresh_fh will attempt to update the file handle for the node.
 *
 * It only does this for symbolic links and regular files that are not currently opened.
 *
 * On Success returns 0 and the nodes file handle is updated, or ESTALE on failure.
 */ 
int
nfs_refresh_fh(nfsnode_t np, vfs_context_t ctx)
{
	vnode_t dvp, vp = NFSTOV(np);
	nfsnode_t dnp;
	const char *v_name = vnode_getname(vp);
	char *name;
	int namelen, fhsize, refreshed;
	int error, wanted = 0;
	uint8_t *fhp;
	struct timespec ts = {2, 0};

	NFS_VNOP_DBG("vnode is %d\n", vnode_vtype(vp));

	dvp = vnode_parent(vp);
	if ((vnode_vtype(vp) != VREG && vnode_vtype(vp) != VLNK) ||
	    v_name == NULL || *v_name == '\0' || dvp == NULL) {
		if (v_name != NULL)
			vnode_putname(v_name);
		return (ESTALE);
	}
	dnp = VTONFS(dvp);
	
	namelen = strlen(v_name);
	MALLOC(name, char *, namelen + 1, M_TEMP, M_WAITOK);
	if (name == NULL) {
		vnode_putname(v_name);
		return (ESTALE);
	}
	bcopy(v_name, name, namelen+1);
	NFS_VNOP_DBG("Trying to refresh %s : %s\n", v_name, name);
	vnode_putname(v_name);

	/* Allocate the maximum size file handle */
	MALLOC(fhp, uint8_t *, NFS4_FHSIZE, M_TEMP, M_WAITOK);
	if (fhp == NULL) {
		FREE(name, M_TEMP);
		return (ESTALE);
	}
	
	if ((error = nfs_node_lock(np))) {
		FREE(name, M_TEMP);
		FREE(fhp, M_TEMP);
		return (ESTALE);
	}
	
	fhsize = np->n_fhsize;
	bcopy(np->n_fhp, fhp, fhsize);
	while (ISSET(np->n_flag, NREFRESH)) {
		SET(np->n_flag, NREFRESHWANT);
		NFS_VNOP_DBG("Waiting for refresh of %s\n", name);
		msleep(np, &np->n_lock, PZERO-1, "nfsrefreshwant", &ts);
		if ((error = nfs_sigintr(NFSTONMP(np), NULL,  vfs_context_thread(ctx), 0)))
			break;
	}
	refreshed = error ? 0 : !NFS_CMPFH(np, fhp, fhsize);
	SET(np->n_flag, NREFRESH);
	nfs_node_unlock(np);

	NFS_VNOP_DBG("error = %d, refreshed = %d\n", error, refreshed);
	if (error || refreshed)
		goto nfsmout;
	
	/* Check that there are no open references for this file */
	lck_mtx_lock(&np->n_openlock);
	if (np->n_openrefcnt || !TAILQ_EMPTY(&np->n_opens) || !TAILQ_EMPTY(&np->n_lock_owners)) {
		int cnt = 0;
		struct nfs_open_file *ofp;
		
		TAILQ_FOREACH(ofp, &np->n_opens, nof_link) {
			cnt += ofp->nof_opencnt;
		}
		if (cnt) {
			lck_mtx_unlock(&np->n_openlock);
			NFS_VNOP_DBG("Can not refresh file handle for %s with open state\n", name);
			NFS_VNOP_DBG("\topenrefcnt = %d, opens = %d lock_owners = %d\n", 
					  np->n_openrefcnt, cnt, !TAILQ_EMPTY(&np->n_lock_owners));
			error = ESTALE;
			goto nfsmout;
		}
	}
	lck_mtx_unlock(&np->n_openlock);
	/* 
	 * Since the FH is currently stale we should not be able to
	 * establish any open state until the FH is refreshed.
	 */

	error = nfs_node_lock(np);
	nfsmout_if(error);
	/*
	 * Symlinks should never need invalidations and are holding
	 * the one and only nfsbuf in an uncached acquired state
	 * trying to do a readlink. So we will hang if we invalidate
	 * in that case. Only in in the VREG case do we need to
	 * invalidate.
	 */
	if (vnode_vtype(vp) == VREG) {
		np->n_flag &= ~NNEEDINVALIDATE;
		nfs_node_unlock(np);
		error = nfs_vinvalbuf(vp, V_IGNORE_WRITEERR, ctx, 1);
		if (error)
			NFS_VNOP_DBG("nfs_vinvalbuf returned %d\n", error);
		nfsmout_if(error);
	} else {
		nfs_node_unlock(np);
	}

	NFS_VNOP_DBG("Looking up %s\n", name);
	error = nfs_lookitup(dnp, name, namelen, ctx, &np);
	if (error)
		NFS_VNOP_DBG("nfs_lookitup returned %d\n", error);

nfsmout:
	nfs_node_lock_force(np);
	wanted = ISSET(np->n_flag, NREFRESHWANT);
	CLR(np->n_flag, NREFRESH|NREFRESHWANT);
	nfs_node_unlock(np);
	if (wanted)
		wakeup(np);
	
	if (error == 0)
		NFS_VNOP_DBG("%s refreshed file handle\n", name);

	FREE(name, M_TEMP);
	FREE(fhp, M_TEMP);
	
	return (error ? ESTALE : 0);
}

int
nfs_getattr(nfsnode_t np, struct nfs_vattr *nvap, vfs_context_t ctx, int flags)
{
	int error;
	
retry:
	error = nfs_getattr_internal(np, nvap, ctx, flags);
	if (error == ESTALE) {
		error = nfs_refresh_fh(np, ctx);
		if (!error)
			goto retry;
	}
	return (error);
}

int
nfs_getattr_internal(nfsnode_t np, struct nfs_vattr *nvap, vfs_context_t ctx, int flags)
{
	struct nfsmount *nmp;
	int error = 0, nfsvers, inprogset = 0, wanted = 0, avoidfloods;
	struct nfs_vattr nvattr;
	struct timespec ts = { 2, 0 };
	u_int64_t xid;

	FSDBG_TOP(513, np->n_size, np, np->n_vattr.nva_size, np->n_flag);

	nmp = NFSTONMP(np);
	
	if (nfs_mount_gone(nmp))
		return (ENXIO);
	nfsvers = nmp->nm_vers;

	if (!nvap)
		nvap = &nvattr;
	NVATTR_INIT(nvap);

	/* Update local times for special files. */
	if (np->n_flag & (NACC | NUPD)) {
		nfs_node_lock_force(np);
		np->n_flag |= NCHG;
		nfs_node_unlock(np);
	}
	/* Update size, if necessary */
	if (ISSET(np->n_flag, NUPDATESIZE))
		nfs_data_update_size(np, 0);

	error = nfs_node_lock(np);
	nfsmout_if(error);
	if (!(flags & (NGA_UNCACHED|NGA_MONITOR)) || ((nfsvers >= NFS_VER4) && (np->n_openflags & N_DELEG_MASK))) {
		/*
		 * Use the cache or wait for any getattr in progress if:
		 * - it's a cached request, or
		 * - we have a delegation, or
		 * - the server isn't responding
		 */
		while (1) {
			error = nfs_getattrcache(np, nvap, flags);
			if (!error || (error != ENOENT)) {
				nfs_node_unlock(np);
				goto nfsmout;
			}
			error = 0;
			if (!ISSET(np->n_flag, NGETATTRINPROG))
				break;
			if (flags & NGA_MONITOR) {
				/* no need to wait if a request is pending */
				error = EINPROGRESS;
				nfs_node_unlock(np);
				goto nfsmout;
			}
			SET(np->n_flag, NGETATTRWANT);
			msleep(np, &np->n_lock, PZERO-1, "nfsgetattrwant", &ts);
			if ((error = nfs_sigintr(NFSTONMP(np), NULL, vfs_context_thread(ctx), 0))) {
				nfs_node_unlock(np);
				goto nfsmout;
			}
		}
		SET(np->n_flag, NGETATTRINPROG);
		inprogset = 1;
	} else if (!ISSET(np->n_flag, NGETATTRINPROG)) {
		SET(np->n_flag, NGETATTRINPROG);
		inprogset = 1;
	} else if (flags & NGA_MONITOR) {
		/* no need to make a request if one is pending */
		error = EINPROGRESS;
	}
	nfs_node_unlock(np);

	nmp = NFSTONMP(np);
	if (nfs_mount_gone(nmp))
		error = ENXIO;
	if (error)
		goto nfsmout;

	/*
	 * Return cached attributes if they are valid,
	 * if the server doesn't respond, and this is
	 * some softened up style of mount.
	 */
	if (NATTRVALID(np) && nfs_use_cache(nmp))
		flags |= NGA_SOFT;

	/*
	 * We might want to try to get both the attributes and access info by
	 * making an ACCESS call and seeing if it returns updated attributes.
	 * But don't bother if we aren't caching access info or if the
	 * attributes returned wouldn't be cached.
	 */
	if (!(flags & NGA_ACL) && (nfsvers != NFS_VER2) && nfs_access_for_getattr && (nfs_access_cache_timeout > 0)) {
		if (nfs_attrcachetimeout(np) > 0) {
			/*  OSAddAtomic(1, &nfsstats.accesscache_misses); */
			u_int32_t access = NFS_ACCESS_ALL;
			int rpcflags = 0;

			/* Return cached attrs if server doesn't respond */
			if (flags & NGA_SOFT)
				rpcflags |= R_SOFT;

			error = nmp->nm_funcs->nf_access_rpc(np, &access, rpcflags, ctx);

			if (error == ETIMEDOUT)
				goto returncached;

			if (error)
				goto nfsmout;
			nfs_node_lock_force(np);
			error = nfs_getattrcache(np, nvap, flags);
			nfs_node_unlock(np);
			if (!error || (error != ENOENT))
				goto nfsmout;
			/* Well, that didn't work... just do a getattr... */
			error = 0;
		}
	}

	avoidfloods = 0;

tryagain:
	error = nmp->nm_funcs->nf_getattr_rpc(np, NULL, np->n_fhp, np->n_fhsize, flags, ctx, nvap, &xid);
	if (!error) {
		nfs_node_lock_force(np);
		error = nfs_loadattrcache(np, nvap, &xid, 0);
		nfs_node_unlock(np);
	}

	/*
	 * If the server didn't respond, return cached attributes.
	 */
returncached:
	if ((flags & NGA_SOFT) && (error == ETIMEDOUT)) {
		nfs_node_lock_force(np);
		error = nfs_getattrcache(np, nvap, flags);
		if (!error || (error != ENOENT)) {
			nfs_node_unlock(np);
			goto nfsmout;
		}
		nfs_node_unlock(np);
	}
	nfsmout_if(error);

	if (!xid) { /* out-of-order rpc - attributes were dropped */
		FSDBG(513, -1, np, np->n_xid >> 32, np->n_xid);
		if (avoidfloods++ < 20)
			goto tryagain;
		/* avoidfloods>1 is bizarre.  at 20 pull the plug */
		/* just return the last attributes we got */
	}
nfsmout:
	nfs_node_lock_force(np);
	if (inprogset) {
		wanted = ISSET(np->n_flag, NGETATTRWANT);
		CLR(np->n_flag, (NGETATTRINPROG | NGETATTRWANT));
	}
	if (!error) {
		/* check if the node changed on us */
		vnode_t vp = NFSTOV(np);
		enum vtype vtype = vnode_vtype(vp);
		if ((vtype == VDIR) && NFS_CHANGED_NC(nfsvers, np, nvap)) {
			FSDBG(513, -1, np, 0, np);
			np->n_flag &= ~NNEGNCENTRIES;
			cache_purge(vp);
			np->n_ncgen++;
			NFS_CHANGED_UPDATE_NC(nfsvers, np, nvap);
			NFS_VNOP_DBG("Purge directory 0x%llx\n", 
			      (uint64_t)VM_KERNEL_ADDRPERM(vp));
		}
		if (NFS_CHANGED(nfsvers, np, nvap)) {
			FSDBG(513, -1, np, -1, np);
			if (vtype == VDIR) {
				NFS_VNOP_DBG("Invalidate directory 0x%llx\n", 
			               (uint64_t)VM_KERNEL_ADDRPERM(vp));
				nfs_invaldir(np);
			}
			nfs_node_unlock(np);
			if (wanted)
				wakeup(np);
			error = nfs_vinvalbuf(vp, V_SAVE, ctx, 1);
			FSDBG(513, -1, np, -2, error);
			if (!error) {
				nfs_node_lock_force(np);
				NFS_CHANGED_UPDATE(nfsvers, np, nvap);
				nfs_node_unlock(np);
			}
		} else {
			nfs_node_unlock(np);
			if (wanted)
				wakeup(np);
		}
	} else {
		nfs_node_unlock(np);
		if (wanted)
			wakeup(np);
	}

	if (nvap == &nvattr) {
		NVATTR_CLEANUP(nvap);
	} else if (!(flags & NGA_ACL)) {
		/* make sure we don't return an ACL if it wasn't asked for */
		NFS_BITMAP_CLR(nvap->nva_bitmap, NFS_FATTR_ACL);
		if (nvap->nva_acl) {
			kauth_acl_free(nvap->nva_acl);
			nvap->nva_acl = NULL;
		}
	}
	FSDBG_BOT(513, np->n_size, error, np->n_vattr.nva_size, np->n_flag);
	return (error);
}

/*
 * NFS getattr call from vfs.
 */

/*
 * The attributes we support over the wire.
 * We also get fsid but the vfs layer gets it out of the mount 
 * structure after this calling us so there's no need to return it,
 * and Finder expects to call getattrlist just looking for the FSID
 * with out hanging on a non responsive server.
 */
#define NFS3_SUPPORTED_VATTRS \
	(VNODE_ATTR_va_rdev |		\
	 VNODE_ATTR_va_nlink |		\
	 VNODE_ATTR_va_data_size |	\
	 VNODE_ATTR_va_data_alloc |	\
	 VNODE_ATTR_va_uid |		\
	 VNODE_ATTR_va_gid |		\
	 VNODE_ATTR_va_mode |		\
	 VNODE_ATTR_va_modify_time |	\
	 VNODE_ATTR_va_change_time |	\
	 VNODE_ATTR_va_access_time |	\
	 VNODE_ATTR_va_fileid |		\
	 VNODE_ATTR_va_type)

int
nfs3_vnop_getattr(
	struct vnop_getattr_args /* {
		struct vnodeop_desc *a_desc;
		vnode_t a_vp;
		struct vnode_attr *a_vap;
		vfs_context_t a_context;
	} */ *ap)
{
	int error;
	struct nfs_vattr nva;
	struct vnode_attr *vap = ap->a_vap;
	dev_t rdev;

	/*
	 * Lets don't go over the wire if we don't support any of the attributes.
	 * Just fall through at the VFS layer and let it cons up what it needs.
	 */
	/* Return the io size no matter what, since we don't go over the wire for this */
	VATTR_RETURN(vap, va_iosize, nfs_iosize);
	if ((vap->va_active & NFS3_SUPPORTED_VATTRS) == 0)
		return (0);

	if (VATTR_IS_ACTIVE(ap->a_vap, va_name))
	    NFS_VNOP_DBG("Getting attrs for 0x%llx, vname is %s\n", 
	          (uint64_t)VM_KERNEL_ADDRPERM(ap->a_vp),
	          ap->a_vp->v_name ? ap->a_vp->v_name : "empty");
	error = nfs_getattr(VTONFS(ap->a_vp), &nva, ap->a_context, NGA_CACHED);
	if (error)
		return (error);

	/* copy nva to *a_vap */
	VATTR_RETURN(vap, va_type, nva.nva_type);
	VATTR_RETURN(vap, va_mode, nva.nva_mode);
	rdev = makedev(nva.nva_rawdev.specdata1, nva.nva_rawdev.specdata2);
	VATTR_RETURN(vap, va_rdev, rdev);
	VATTR_RETURN(vap, va_uid, nva.nva_uid);
	VATTR_RETURN(vap, va_gid, nva.nva_gid);
	VATTR_RETURN(vap, va_nlink, nva.nva_nlink);
	VATTR_RETURN(vap, va_fileid, nva.nva_fileid);
	VATTR_RETURN(vap, va_data_size, nva.nva_size);
	VATTR_RETURN(vap, va_data_alloc, nva.nva_bytes);
	vap->va_access_time.tv_sec = nva.nva_timesec[NFSTIME_ACCESS];
	vap->va_access_time.tv_nsec = nva.nva_timensec[NFSTIME_ACCESS];
	VATTR_SET_SUPPORTED(vap, va_access_time);
	vap->va_modify_time.tv_sec = nva.nva_timesec[NFSTIME_MODIFY];
	vap->va_modify_time.tv_nsec = nva.nva_timensec[NFSTIME_MODIFY];
	VATTR_SET_SUPPORTED(vap, va_modify_time);
	vap->va_change_time.tv_sec = nva.nva_timesec[NFSTIME_CHANGE];
	vap->va_change_time.tv_nsec = nva.nva_timensec[NFSTIME_CHANGE];
	VATTR_SET_SUPPORTED(vap, va_change_time);

	// VATTR_RETURN(vap, va_encoding, 0xffff /* kTextEncodingUnknown */);
	return (error);
}

/*
 * NFS setattr call.
 */
int
nfs_vnop_setattr(
	struct vnop_setattr_args /* {
		struct vnodeop_desc *a_desc;
		vnode_t a_vp;
		struct vnode_attr *a_vap;
		vfs_context_t a_context;
	} */ *ap)
{
	vfs_context_t ctx = ap->a_context;
	vnode_t vp = ap->a_vp;
	nfsnode_t np = VTONFS(vp);
	struct nfsmount *nmp;
	struct vnode_attr *vap = ap->a_vap;
	int error = 0;
	int biosize, nfsvers, namedattrs;
	u_quad_t origsize, vapsize;
	struct nfs_dulookup dul;
	nfsnode_t dnp = NULL;
	vnode_t dvp = NULL;
	const char *vname = NULL;
	struct nfs_open_owner *noop = NULL;
	struct nfs_open_file *nofp = NULL;

	nmp = VTONMP(vp);
	if (nfs_mount_gone(nmp))
		return (ENXIO);
	nfsvers = nmp->nm_vers;
	namedattrs = (nmp->nm_fsattr.nfsa_flags & NFS_FSFLAG_NAMED_ATTR);
	biosize = nmp->nm_biosize;

	/* Disallow write attempts if the filesystem is mounted read-only. */
	if (vnode_vfsisrdonly(vp))
		return (EROFS);

	origsize = np->n_size;
	if (VATTR_IS_ACTIVE(vap, va_data_size)) {
		switch (vnode_vtype(vp)) {
		case VDIR:
			return (EISDIR);
		case VCHR:
		case VBLK:
		case VSOCK:
		case VFIFO:
			if (!VATTR_IS_ACTIVE(vap, va_modify_time) &&
			    !VATTR_IS_ACTIVE(vap, va_access_time) &&
			    !VATTR_IS_ACTIVE(vap, va_mode) &&
			    !VATTR_IS_ACTIVE(vap, va_uid) &&
			    !VATTR_IS_ACTIVE(vap, va_gid)) {
				return (0);
			}
			VATTR_CLEAR_ACTIVE(vap, va_data_size);
			break;
		default:
			/*
			 * Disallow write attempts if the filesystem is
			 * mounted read-only.
			 */
			if (vnode_vfsisrdonly(vp))
				return (EROFS);
			FSDBG_TOP(512, np->n_size, vap->va_data_size,
				  np->n_vattr.nva_size, np->n_flag);
			/* clear NNEEDINVALIDATE, if set */
			if ((error = nfs_node_lock(np)))
				return (error);
			if (np->n_flag & NNEEDINVALIDATE)
				np->n_flag &= ~NNEEDINVALIDATE;
			nfs_node_unlock(np);
			/* flush everything */
			error = nfs_vinvalbuf(vp, (vap->va_data_size ? V_SAVE : 0) , ctx, 1);
			if (error) {
				NP(np, "nfs_setattr: nfs_vinvalbuf %d", error);
				FSDBG_BOT(512, np->n_size, vap->va_data_size, np->n_vattr.nva_size, -1);
				return (error);
			}
			if (nfsvers >= NFS_VER4) {
				/* setting file size requires having the file open for write access */
				if (np->n_flag & NREVOKE)
					return (EIO);
				noop = nfs_open_owner_find(nmp, vfs_context_ucred(ctx), 1);
				if (!noop)
					return (ENOMEM);
restart:
				error = nfs_mount_state_in_use_start(nmp, vfs_context_thread(ctx));
				if (error)
					return (error);
				if (np->n_flag & NREVOKE) {
					nfs_mount_state_in_use_end(nmp, 0);
					return (EIO);
				}
				error = nfs_open_file_find(np, noop, &nofp, 0, 0, 1);
				if (!error && (nofp->nof_flags & NFS_OPEN_FILE_LOST))
					error = EIO;
				if (!error && (nofp->nof_flags & NFS_OPEN_FILE_REOPEN)) {
					nfs_mount_state_in_use_end(nmp, 0);
					error = nfs4_reopen(nofp, vfs_context_thread(ctx));
					nofp = NULL;
					if (!error)
						goto restart;
				}
				if (!error)
					error = nfs_open_file_set_busy(nofp, vfs_context_thread(ctx));
				if (error) {
					nfs_open_owner_rele(noop);
					return (error);
				}
				if (!(nofp->nof_access & NFS_OPEN_SHARE_ACCESS_WRITE)) {
					/* we don't have the file open for write access, so open it */
					error = nfs4_open(np, nofp, NFS_OPEN_SHARE_ACCESS_WRITE, NFS_OPEN_SHARE_DENY_NONE, ctx);
					if (!error)
						nofp->nof_flags |= NFS_OPEN_FILE_SETATTR;
					if (nfs_mount_state_error_should_restart(error)) {
						nfs_open_file_clear_busy(nofp);
						nofp = NULL;
						if (nfs_mount_state_in_use_end(nmp, error))
							goto restart;
					}
				}
			}
			nfs_data_lock(np, NFS_DATA_LOCK_EXCLUSIVE);
			if (np->n_size > vap->va_data_size) { /* shrinking? */
				daddr64_t obn, bn;
				int neweofoff, mustwrite;
				struct nfsbuf *bp;

				obn = (np->n_size - 1) / biosize;
				bn = vap->va_data_size / biosize;
				for ( ; obn >= bn; obn--) {
					if (!nfs_buf_is_incore(np, obn))
						continue;
					error = nfs_buf_get(np, obn, biosize, NULL, NBLK_READ, &bp);
					if (error)
						continue;
					if (obn != bn) {
						FSDBG(512, bp, bp->nb_flags, 0, obn);
						SET(bp->nb_flags, NB_INVAL);
						nfs_buf_release(bp, 1);
						continue;
					}
					mustwrite = 0;
					neweofoff = vap->va_data_size - NBOFF(bp);
					/* check for any dirty data before the new EOF */
					if ((bp->nb_dirtyend > 0) && (bp->nb_dirtyoff < neweofoff)) {
						/* clip dirty range to EOF */
						if (bp->nb_dirtyend > neweofoff) {
							bp->nb_dirtyend = neweofoff;
							if (bp->nb_dirtyoff >= bp->nb_dirtyend)
								bp->nb_dirtyoff = bp->nb_dirtyend = 0;
						}
						if ((bp->nb_dirtyend > 0) && (bp->nb_dirtyoff < neweofoff))
							mustwrite++;
					}
					bp->nb_dirty &= (1 << round_page_32(neweofoff)/PAGE_SIZE) - 1;
					if (bp->nb_dirty)
						mustwrite++;
					if (!mustwrite) {
						FSDBG(512, bp, bp->nb_flags, 0, obn);
						SET(bp->nb_flags, NB_INVAL);
						nfs_buf_release(bp, 1);
						continue;
					}
					/* gotta write out dirty data before invalidating */
					/* (NB_STABLE indicates that data writes should be FILESYNC) */
					/* (NB_NOCACHE indicates buffer should be discarded) */
					CLR(bp->nb_flags, (NB_DONE | NB_ERROR | NB_INVAL | NB_ASYNC | NB_READ));
					SET(bp->nb_flags, NB_STABLE | NB_NOCACHE);
					if (!IS_VALID_CRED(bp->nb_wcred)) {
						kauth_cred_t cred = vfs_context_ucred(ctx);
						kauth_cred_ref(cred);
						bp->nb_wcred = cred;
					}
					error = nfs_buf_write(bp);
					// Note: bp has been released
					if (error) {
						FSDBG(512, bp, 0xd00dee, 0xbad, error);
						nfs_node_lock_force(np);
						np->n_error = error;
						np->n_flag |= NWRITEERR;
						/*
						 * There was a write error and we need to
						 * invalidate attrs and flush buffers in
						 * order to sync up with the server.
						 * (if this write was extending the file,
						 * we may no longer know the correct size)
						 */
						NATTRINVALIDATE(np);
						nfs_node_unlock(np);
						nfs_data_unlock(np);
						nfs_vinvalbuf(vp, V_SAVE|V_IGNORE_WRITEERR, ctx, 1);
						nfs_data_lock(np, NFS_DATA_LOCK_EXCLUSIVE);
						error = 0;
					}
				}
			}
			if (vap->va_data_size != np->n_size)
				ubc_setsize(vp, (off_t)vap->va_data_size); /* XXX error? */
			origsize = np->n_size;
			np->n_size = np->n_vattr.nva_size = vap->va_data_size;
			nfs_node_lock_force(np);
			CLR(np->n_flag, NUPDATESIZE);
			nfs_node_unlock(np);
			FSDBG(512, np, np->n_size, np->n_vattr.nva_size, 0xf00d0001);
		}
	} else if (VATTR_IS_ACTIVE(vap, va_modify_time) ||
		    VATTR_IS_ACTIVE(vap, va_access_time) ||
		    (vap->va_vaflags & VA_UTIMES_NULL)) {
		if ((error = nfs_node_lock(np)))
			return (error);
		if ((np->n_flag & NMODIFIED) && (vnode_vtype(vp) == VREG)) {
			nfs_node_unlock(np);
			error = nfs_vinvalbuf(vp, V_SAVE, ctx, 1);
			if (error == EINTR)
				return (error);
		} else {
			nfs_node_unlock(np);
		}
	}
	if ((VATTR_IS_ACTIVE(vap, va_mode) || VATTR_IS_ACTIVE(vap, va_uid) || VATTR_IS_ACTIVE(vap, va_gid) ||
	     VATTR_IS_ACTIVE(vap, va_acl) || VATTR_IS_ACTIVE(vap, va_uuuid) || VATTR_IS_ACTIVE(vap, va_guuid)) &&
	    !(error = nfs_node_lock(np))) {
		NACCESSINVALIDATE(np);
		nfs_node_unlock(np);
		if (!namedattrs) {
			dvp = vnode_getparent(vp);
			vname = vnode_getname(vp);
			dnp = (dvp && vname) ? VTONFS(dvp) : NULL;
			if (dnp) {
				error = nfs_node_set_busy(dnp, vfs_context_thread(ctx));
				if (error) {
					dnp = NULL;
					error = 0;
				}
			}
			if (dnp) {
				nfs_dulookup_init(&dul, dnp, vname, strlen(vname), ctx);
				nfs_dulookup_start(&dul, dnp, ctx);
			}
		}
	}

	if (!error)
		error = nmp->nm_funcs->nf_setattr_rpc(np, vap, ctx);

	if (VATTR_IS_ACTIVE(vap, va_mode) || VATTR_IS_ACTIVE(vap, va_uid) || VATTR_IS_ACTIVE(vap, va_gid) ||
	    VATTR_IS_ACTIVE(vap, va_acl) || VATTR_IS_ACTIVE(vap, va_uuuid) || VATTR_IS_ACTIVE(vap, va_guuid)) {
		if (!namedattrs) {
			if (dnp) {
				nfs_dulookup_finish(&dul, dnp, ctx);
				nfs_node_clear_busy(dnp);
			}
			if (dvp != NULLVP)
				vnode_put(dvp);
			if (vname != NULL)
				vnode_putname(vname);
		}
	}

	FSDBG_BOT(512, np->n_size, vap->va_data_size, np->n_vattr.nva_size, error);
	if (VATTR_IS_ACTIVE(vap, va_data_size)) {
		if (error && (origsize != np->n_size) &&
		    ((nfsvers < NFS_VER4) || !nfs_mount_state_error_should_restart(error))) {
			/* make every effort to resync file size w/ server... */
			/* (don't bother if we'll be restarting the operation) */
			int err; /* preserve "error" for return */
			np->n_size = np->n_vattr.nva_size = origsize;
			nfs_node_lock_force(np);
			CLR(np->n_flag, NUPDATESIZE);
			nfs_node_unlock(np);
			FSDBG(512, np, np->n_size, np->n_vattr.nva_size, 0xf00d0002);
			ubc_setsize(vp, (off_t)np->n_size); /* XXX check error */
			vapsize = vap->va_data_size;
			vap->va_data_size = origsize;
			err = nmp->nm_funcs->nf_setattr_rpc(np, vap, ctx);
			if (err)
				NP(np, "nfs_vnop_setattr: nfs%d_setattr_rpc %d %d", nfsvers, error, err);
			vap->va_data_size = vapsize;
		}
		nfs_node_lock_force(np);
		/*
		 * The size was just set.  If the size is already marked for update, don't
		 * trust the newsize (it may have been set while the setattr was in progress).
		 * Clear the update flag and make sure we fetch new attributes so we are sure
		 * we have the latest size.
		 */
		if (ISSET(np->n_flag, NUPDATESIZE)) {
			CLR(np->n_flag, NUPDATESIZE);
			NATTRINVALIDATE(np);
			nfs_node_unlock(np);
			nfs_getattr(np, NULL, ctx, NGA_UNCACHED);
		} else {
			nfs_node_unlock(np);
		}
		nfs_data_unlock(np);
		if (nfsvers >= NFS_VER4) {
			if (nofp) {
				/* don't close our setattr open if we'll be restarting... */
				if (!nfs_mount_state_error_should_restart(error) &&
				    (nofp->nof_flags & NFS_OPEN_FILE_SETATTR)) {
					int err = nfs_close(np, nofp, NFS_OPEN_SHARE_ACCESS_WRITE, NFS_OPEN_SHARE_DENY_NONE, ctx);
					if (err)
						NP(np, "nfs_vnop_setattr: close error: %d", err);
					nofp->nof_flags &= ~NFS_OPEN_FILE_SETATTR;
				}
				nfs_open_file_clear_busy(nofp);
				nofp = NULL;
			}
			if (nfs_mount_state_in_use_end(nmp, error))
				goto restart;
			nfs_open_owner_rele(noop);
		}
	}
	return (error);
}

/*
 * Do an NFS setattr RPC.
 */
int
nfs3_setattr_rpc(
	nfsnode_t np,
	struct vnode_attr *vap,
	vfs_context_t ctx)
{
	struct nfsmount *nmp = NFSTONMP(np);
	int error = 0, lockerror = ENOENT, status, wccpostattr = 0, nfsvers;
	u_int64_t xid, nextxid;
	struct nfsm_chain nmreq, nmrep;

	if (nfs_mount_gone(nmp))
		return (ENXIO);
	nfsvers = nmp->nm_vers;

	VATTR_SET_SUPPORTED(vap, va_mode);
	VATTR_SET_SUPPORTED(vap, va_uid);
	VATTR_SET_SUPPORTED(vap, va_gid);
	VATTR_SET_SUPPORTED(vap, va_data_size);
	VATTR_SET_SUPPORTED(vap, va_access_time);
	VATTR_SET_SUPPORTED(vap, va_modify_time);

	if (VATTR_IS_ACTIVE(vap, va_flags)) {
		if (vap->va_flags) {	/* we don't support setting flags */
			if (vap->va_active & ~VNODE_ATTR_va_flags)
				return (EINVAL);	/* return EINVAL if other attributes also set */
			else
				return (ENOTSUP);	/* return ENOTSUP for chflags(2) */
		}
		/* no flags set, so we'll just ignore it */
		if (!(vap->va_active & ~VNODE_ATTR_va_flags))
			return (0); /* no (other) attributes to set, so nothing to do */
	}

	nfsm_chain_null(&nmreq);
	nfsm_chain_null(&nmrep);

	nfsm_chain_build_alloc_init(error, &nmreq,
		NFSX_FH(nfsvers) + NFSX_SATTR(nfsvers));
	nfsm_chain_add_fh(error, &nmreq, nfsvers, np->n_fhp, np->n_fhsize);
	if (nfsvers == NFS_VER3) {
		if (VATTR_IS_ACTIVE(vap, va_mode)) {
			nfsm_chain_add_32(error, &nmreq, TRUE);
			nfsm_chain_add_32(error, &nmreq, vap->va_mode);
		} else {
			nfsm_chain_add_32(error, &nmreq, FALSE);
		}
		if (VATTR_IS_ACTIVE(vap, va_uid)) {
			nfsm_chain_add_32(error, &nmreq, TRUE);
			nfsm_chain_add_32(error, &nmreq, vap->va_uid);
		} else {
			nfsm_chain_add_32(error, &nmreq, FALSE);
		}
		if (VATTR_IS_ACTIVE(vap, va_gid)) {
			nfsm_chain_add_32(error, &nmreq, TRUE);
			nfsm_chain_add_32(error, &nmreq, vap->va_gid);
		} else {
			nfsm_chain_add_32(error, &nmreq, FALSE);
		}
		if (VATTR_IS_ACTIVE(vap, va_data_size)) {
			nfsm_chain_add_32(error, &nmreq, TRUE);
			nfsm_chain_add_64(error, &nmreq, vap->va_data_size);
		} else {
			nfsm_chain_add_32(error, &nmreq, FALSE);
		}
		if (vap->va_vaflags & VA_UTIMES_NULL) {
			nfsm_chain_add_32(error, &nmreq, NFS_TIME_SET_TO_SERVER);
			nfsm_chain_add_32(error, &nmreq, NFS_TIME_SET_TO_SERVER);
		} else {
			if (VATTR_IS_ACTIVE(vap, va_access_time)) {
				nfsm_chain_add_32(error, &nmreq, NFS_TIME_SET_TO_CLIENT);
				nfsm_chain_add_32(error, &nmreq, vap->va_access_time.tv_sec);
				nfsm_chain_add_32(error, &nmreq, vap->va_access_time.tv_nsec);
			} else {
				nfsm_chain_add_32(error, &nmreq, NFS_TIME_DONT_CHANGE);
			}
			if (VATTR_IS_ACTIVE(vap, va_modify_time)) {
				nfsm_chain_add_32(error, &nmreq, NFS_TIME_SET_TO_CLIENT);
				nfsm_chain_add_32(error, &nmreq, vap->va_modify_time.tv_sec);
				nfsm_chain_add_32(error, &nmreq, vap->va_modify_time.tv_nsec);
			} else {
				nfsm_chain_add_32(error, &nmreq, NFS_TIME_DONT_CHANGE);
			}
		}
		nfsm_chain_add_32(error, &nmreq, FALSE);
	} else {
		nfsm_chain_add_32(error, &nmreq, VATTR_IS_ACTIVE(vap, va_mode) ?
			vtonfsv2_mode(vnode_vtype(NFSTOV(np)), vap->va_mode) : -1);
		nfsm_chain_add_32(error, &nmreq, VATTR_IS_ACTIVE(vap, va_uid) ?
			vap->va_uid : (uint32_t)-1);
		nfsm_chain_add_32(error, &nmreq, VATTR_IS_ACTIVE(vap, va_gid) ?
			vap->va_gid : (uint32_t)-1);
		nfsm_chain_add_32(error, &nmreq, VATTR_IS_ACTIVE(vap, va_data_size) ?
			vap->va_data_size : (uint32_t)-1);
		if (VATTR_IS_ACTIVE(vap, va_access_time)) {
			nfsm_chain_add_32(error, &nmreq, vap->va_access_time.tv_sec);
			nfsm_chain_add_32(error, &nmreq, (vap->va_access_time.tv_nsec != -1) ?
				((uint32_t)vap->va_access_time.tv_nsec / 1000) : 0xffffffff);
		} else {
			nfsm_chain_add_32(error, &nmreq, -1);
			nfsm_chain_add_32(error, &nmreq, -1);
		}
		if (VATTR_IS_ACTIVE(vap, va_modify_time)) {
			nfsm_chain_add_32(error, &nmreq, vap->va_modify_time.tv_sec);
			nfsm_chain_add_32(error, &nmreq, (vap->va_modify_time.tv_nsec != -1) ?
				((uint32_t)vap->va_modify_time.tv_nsec / 1000) : 0xffffffff);
		} else {
			nfsm_chain_add_32(error, &nmreq, -1);
			nfsm_chain_add_32(error, &nmreq, -1);
		}
	}
	nfsm_chain_build_done(error, &nmreq);
	nfsmout_if(error);
	error = nfs_request(np, NULL, &nmreq, NFSPROC_SETATTR, ctx, NULL, &nmrep, &xid, &status);
	if ((lockerror = nfs_node_lock(np)))
		error = lockerror;
	if (nfsvers == NFS_VER3) {
		struct timespec premtime = { 0, 0 };
		nfsm_chain_get_wcc_data(error, &nmrep, np, &premtime, &wccpostattr, &xid);
		nfsmout_if(error);
		/* if file hadn't changed, update cached mtime */
		if (nfstimespeccmp(&np->n_mtime, &premtime, ==))
			NFS_CHANGED_UPDATE(nfsvers, np, &np->n_vattr);
		/* if directory hadn't changed, update namecache mtime */
		if ((vnode_vtype(NFSTOV(np)) == VDIR) &&
		    nfstimespeccmp(&np->n_ncmtime, &premtime, ==))
			NFS_CHANGED_UPDATE_NC(nfsvers, np, &np->n_vattr);
		if (!wccpostattr)
			NATTRINVALIDATE(np);
		error = status;
	} else {
		if (!error)
			error = status;
		nfsm_chain_loadattr(error, &nmrep, np, nfsvers, &xid);
	}
	/*
	 * We just changed the attributes and we want to make sure that we
	 * see the latest attributes.  Get the next XID.  If it's not the
	 * next XID after the SETATTR XID, then it's possible that another
	 * RPC was in flight at the same time and it might put stale attributes
	 * in the cache.  In that case, we invalidate the attributes and set
	 * the attribute cache XID to guarantee that newer attributes will
	 * get loaded next.
	 */
	nextxid = 0;
	nfs_get_xid(&nextxid);
	if (nextxid != (xid + 1)) {
		np->n_xid = nextxid;
		NATTRINVALIDATE(np);
	}
nfsmout:
	if (!lockerror)
		nfs_node_unlock(np);
	nfsm_chain_cleanup(&nmreq);
	nfsm_chain_cleanup(&nmrep);
	return (error);
}

/*
 * NFS lookup call, one step at a time...
 * First look in cache
 * If not found, unlock the directory nfsnode and do the RPC
 */
int
nfs_vnop_lookup(
	struct vnop_lookup_args /* {
		struct vnodeop_desc *a_desc;
		vnode_t a_dvp;
		vnode_t *a_vpp;
		struct componentname *a_cnp;
		vfs_context_t a_context;
	} */ *ap)
{
	vfs_context_t ctx = ap->a_context;
	struct componentname *cnp = ap->a_cnp;
	vnode_t dvp = ap->a_dvp;
	vnode_t *vpp = ap->a_vpp;
	int flags = cnp->cn_flags;
	vnode_t newvp;
	nfsnode_t dnp, np;
	struct nfsmount *nmp;
	mount_t mp;
	int nfsvers, error, busyerror = ENOENT, isdot, isdotdot, negnamecache;
	u_int64_t xid;
	struct nfs_vattr nvattr;
	int ngflags;
	struct vnop_access_args naa;
	fhandle_t fh;
	struct nfsreq rq, *req = &rq;

	*vpp = NULLVP;

	dnp = VTONFS(dvp);
	NVATTR_INIT(&nvattr);

	mp = vnode_mount(dvp);
	nmp = VFSTONFS(mp);
	if (nfs_mount_gone(nmp)) {
		error = ENXIO;
		goto error_return;
	}
	nfsvers = nmp->nm_vers;
	negnamecache = !NMFLAG(nmp, NONEGNAMECACHE);

	if ((error = busyerror = nfs_node_set_busy(dnp, vfs_context_thread(ctx))))
		goto error_return;
	/* nfs_getattr() will check changed and purge caches */
	if ((error = nfs_getattr(dnp, NULL, ctx, NGA_CACHED)))
		goto error_return;

	error = cache_lookup(dvp, vpp, cnp);
	switch (error) {
	case ENOENT:
		/* negative cache entry */
		goto error_return;
	case 0:
		/* cache miss */
		if ((nfsvers > NFS_VER2) && NMFLAG(nmp, RDIRPLUS)) {
			/* if rdirplus, try dir buf cache lookup */
			error = nfs_dir_buf_cache_lookup(dnp, &np, cnp, ctx, 0);
			if (!error && np) {
				/* dir buf cache hit */
				*vpp = NFSTOV(np);
				error = -1;
			}
		}
		if (error != -1) /* cache miss */
			break;
		/* FALLTHROUGH */
	case -1:
		/* cache hit, not really an error */
		OSAddAtomic64(1, &nfsstats.lookupcache_hits);

		nfs_node_clear_busy(dnp);
		busyerror = ENOENT;

		/* check for directory access */
		naa.a_desc = &vnop_access_desc;
		naa.a_vp = dvp;
		naa.a_action = KAUTH_VNODE_SEARCH;
		naa.a_context = ctx;

		/* compute actual success/failure based on accessibility */
		error = nfs_vnop_access(&naa);
		/* FALLTHROUGH */
	default:
		/* unexpected error from cache_lookup */
		goto error_return;
	}

	/* skip lookup, if we know who we are: "." or ".." */
	isdot = isdotdot = 0;
	if (cnp->cn_nameptr[0] == '.') {
		if (cnp->cn_namelen == 1)
			isdot = 1;
		if ((cnp->cn_namelen == 2) && (cnp->cn_nameptr[1] == '.'))
			isdotdot = 1;
	}
	if (isdotdot || isdot) {
		fh.fh_len = 0;
		goto found;
	}
	if ((nfsvers >= NFS_VER4) && (dnp->n_vattr.nva_flags & NFS_FFLAG_TRIGGER)) {
		/* we should never be looking things up in a trigger directory, return nothing */
		error = ENOENT;
		goto error_return;
	}

	/* do we know this name is too long? */
	nmp = VTONMP(dvp);
	if (nfs_mount_gone(nmp)) {
		error = ENXIO;
		goto error_return;
	}
	if (NFS_BITMAP_ISSET(nmp->nm_fsattr.nfsa_bitmap, NFS_FATTR_MAXNAME) &&
	     (cnp->cn_namelen > (int)nmp->nm_fsattr.nfsa_maxname)) {
		error = ENAMETOOLONG;
		goto error_return;
	}

	error = 0;
	newvp = NULLVP;

	OSAddAtomic64(1, &nfsstats.lookupcache_misses);

	error = nmp->nm_funcs->nf_lookup_rpc_async(dnp, cnp->cn_nameptr, cnp->cn_namelen, ctx, &req);
	nfsmout_if(error);
	error = nmp->nm_funcs->nf_lookup_rpc_async_finish(dnp, cnp->cn_nameptr, cnp->cn_namelen, ctx, req, &xid, &fh, &nvattr);
	nfsmout_if(error);

	/* is the file handle the same as this directory's file handle? */
	isdot = NFS_CMPFH(dnp, fh.fh_data, fh.fh_len);

found:
	if (flags & ISLASTCN) {
		switch (cnp->cn_nameiop) {
		case DELETE:
			cnp->cn_flags &= ~MAKEENTRY;
			break;
		case RENAME:
			cnp->cn_flags &= ~MAKEENTRY;
			if (isdot) {
				error = EISDIR;
				goto error_return;
			}
			break;
		}
	}

	if (isdotdot) {
		newvp = vnode_getparent(dvp);
		if (!newvp) {
			error = ENOENT;
			goto error_return;
		}
	} else if (isdot) {
		error = vnode_get(dvp);
		if (error)
			goto error_return;
		newvp = dvp;
		nfs_node_lock_force(dnp);
		if (fh.fh_len && (dnp->n_xid <= xid))
			nfs_loadattrcache(dnp, &nvattr, &xid, 0);
		nfs_node_unlock(dnp);
	} else {
		ngflags = (cnp->cn_flags & MAKEENTRY) ? NG_MAKEENTRY : 0;
		error = nfs_nget(mp, dnp, cnp, fh.fh_data, fh.fh_len, &nvattr, &xid, rq.r_auth, ngflags, &np);
		if (error)
			goto error_return;
		newvp = NFSTOV(np);
		nfs_node_unlock(np);
	}
	*vpp = newvp;

nfsmout:
	if (error) {
		if (((cnp->cn_nameiop == CREATE) || (cnp->cn_nameiop == RENAME)) &&
		    (flags & ISLASTCN) && (error == ENOENT)) {
			if (vnode_mount(dvp) && vnode_vfsisrdonly(dvp))
				error = EROFS;
			else
				error = EJUSTRETURN;
		}
	}
	if ((error == ENOENT) && (cnp->cn_flags & MAKEENTRY) &&
	    (cnp->cn_nameiop != CREATE) && negnamecache) {
		/* add a negative entry in the name cache */
		nfs_node_lock_force(dnp);
		cache_enter(dvp, NULL, cnp);
		dnp->n_flag |= NNEGNCENTRIES;
		nfs_node_unlock(dnp);
	}
error_return:
	NVATTR_CLEANUP(&nvattr);
	if (!busyerror)
		nfs_node_clear_busy(dnp);
	if (error && *vpp) {
	        vnode_put(*vpp);
		*vpp = NULLVP;
	}
	return (error);
}

int nfs_readlink_nocache = DEFAULT_READLINK_NOCACHE;

/*
 * NFS readlink call
 */
int
nfs_vnop_readlink(
	struct vnop_readlink_args /* {
		struct vnodeop_desc *a_desc;
		vnode_t a_vp;
		struct uio *a_uio;
		vfs_context_t a_context;
	} */ *ap)
{
	vfs_context_t ctx = ap->a_context;
	nfsnode_t np = VTONFS(ap->a_vp);
	struct nfsmount *nmp;
	int error = 0, nfsvers;
	uint32_t buflen;
	uio_t uio = ap->a_uio;
	struct nfsbuf *bp = NULL;
	struct timespec ts;
	int timeo;

	if (vnode_vtype(ap->a_vp) != VLNK)
		return (EPERM);

	if (uio_resid(uio) == 0)
		return (0);
	if (uio_offset(uio) < 0)
		return (EINVAL);

	nmp = VTONMP(ap->a_vp);
	if (nfs_mount_gone(nmp))
		return (ENXIO);
	nfsvers = nmp->nm_vers;

	
	/* nfs_getattr() will check changed and purge caches */
	if ((error = nfs_getattr(np, NULL, ctx, nfs_readlink_nocache ? NGA_UNCACHED : NGA_CACHED))) {
		FSDBG(531, np, 0xd1e0001, 0, error);
		return (error);
	}

	if (nfs_readlink_nocache) {
		timeo = nfs_attrcachetimeout(np);
		nanouptime(&ts);
	}
	
retry:
	OSAddAtomic64(1, &nfsstats.biocache_readlinks);
	error = nfs_buf_get(np, 0, NFS_MAXPATHLEN, vfs_context_thread(ctx), NBLK_META, &bp);
	if (error) {
		FSDBG(531, np, 0xd1e0002, 0, error);
		return (error);
	}

	if (nfs_readlink_nocache) {
		NFS_VNOP_DBG("timeo = %d ts.tv_sec = %ld need refresh = %d cached = %d\n", timeo, ts.tv_sec,
			     (np->n_rltim.tv_sec + timeo) < ts.tv_sec || nfs_readlink_nocache > 1, 
			     ISSET(bp->nb_flags, NB_CACHE) == NB_CACHE);
		/* n_rltim is synchronized by the associated nfs buf */
		if (ISSET(bp->nb_flags, NB_CACHE) && ((nfs_readlink_nocache > 1)  || ((np->n_rltim.tv_sec + timeo) < ts.tv_sec))) {
			SET(bp->nb_flags, NB_INVAL);
			nfs_buf_release(bp, 0);
			goto retry;
		}
	}
	if (!ISSET(bp->nb_flags, NB_CACHE)) {
readagain:
		OSAddAtomic64(1, &nfsstats.readlink_bios);
		buflen = bp->nb_bufsize;
		error = nmp->nm_funcs->nf_readlink_rpc(np, bp->nb_data, &buflen, ctx);
		if (error) {
			if (error == ESTALE) {
				NFS_VNOP_DBG("Stale FH from readlink rpc\n");
				error = nfs_refresh_fh(np, ctx);
				if (error == 0)
					goto readagain;
			}
			SET(bp->nb_flags, NB_ERROR);
			bp->nb_error = error;
			NFS_VNOP_DBG("readlink failed %d\n", error);
		} else {
			bp->nb_validoff = 0;
			bp->nb_validend = buflen;
			np->n_rltim = ts;
			NFS_VNOP_DBG("readlink of %.*s\n", bp->nb_validend, (char *)bp->nb_data);
		}
	} else {
		NFS_VNOP_DBG("got cached link of %.*s\n", bp->nb_validend, (char *)bp->nb_data);
	}
	
	if (!error && (bp->nb_validend > 0))
		error = uiomove(bp->nb_data, bp->nb_validend, uio);
	FSDBG(531, np, bp->nb_validend, 0, error);
	nfs_buf_release(bp, 1);
	return (error);
}

/*
 * Do a readlink RPC.
 */
int
nfs3_readlink_rpc(nfsnode_t np, char *buf, uint32_t *buflenp, vfs_context_t ctx)
{
	struct nfsmount *nmp;
	int error = 0, lockerror = ENOENT, nfsvers, status;
	uint32_t len;
	u_int64_t xid;
	struct nfsm_chain nmreq, nmrep;

	nmp = NFSTONMP(np);
	if (nfs_mount_gone(nmp))
		return (ENXIO);
	nfsvers = nmp->nm_vers;
	nfsm_chain_null(&nmreq);
	nfsm_chain_null(&nmrep);

	nfsm_chain_build_alloc_init(error, &nmreq, NFSX_FH(nfsvers));
	nfsm_chain_add_fh(error, &nmreq, nfsvers, np->n_fhp, np->n_fhsize);
	nfsm_chain_build_done(error, &nmreq);
	nfsmout_if(error);
	error = nfs_request(np, NULL, &nmreq, NFSPROC_READLINK, ctx, NULL, &nmrep, &xid, &status);
	if ((lockerror = nfs_node_lock(np)))
		error = lockerror;
	if (nfsvers == NFS_VER3)
		nfsm_chain_postop_attr_update(error, &nmrep, np, &xid);
	if (!error)
		error = status;
	nfsm_chain_get_32(error, &nmrep, len);
	nfsmout_if(error);
	if ((nfsvers == NFS_VER2) && (len > *buflenp)) {
		error = EBADRPC;
		goto nfsmout;
	}
	if (len >= *buflenp) {
		if (np->n_size && (np->n_size < *buflenp))
			len = np->n_size;
		else
			len = *buflenp - 1;
	}
	nfsm_chain_get_opaque(error, &nmrep, len, buf);
	if (!error)
		*buflenp = len;
nfsmout:
	if (!lockerror)
		nfs_node_unlock(np);
	nfsm_chain_cleanup(&nmreq);
	nfsm_chain_cleanup(&nmrep);
	return (error);
}

/*
 * NFS read RPC call
 * Ditto above
 */
int
nfs_read_rpc(nfsnode_t np, uio_t uio, vfs_context_t ctx)
{
	struct nfsmount *nmp;
	int error = 0, nfsvers, eof = 0;
	size_t nmrsize, len, retlen;
	user_ssize_t tsiz;
	off_t txoffset;
	struct nfsreq rq, *req = &rq;
	uint32_t stategenid = 0, restart = 0;

	FSDBG_TOP(536, np, uio_offset(uio), uio_resid(uio), 0);
	nmp = NFSTONMP(np);
	if (nfs_mount_gone(nmp))
		return (ENXIO);
	nfsvers = nmp->nm_vers;
	nmrsize = nmp->nm_rsize;

	txoffset = uio_offset(uio);
	tsiz = uio_resid(uio);
	if ((nfsvers == NFS_VER2) && ((uint64_t)(txoffset + tsiz) > 0xffffffffULL)) {
		FSDBG_BOT(536, np, uio_offset(uio), uio_resid(uio), EFBIG);
		return (EFBIG);
	}

	while (tsiz > 0) {
		len = retlen = (tsiz > (user_ssize_t)nmrsize) ? nmrsize : (size_t)tsiz;
		FSDBG(536, np, txoffset, len, 0);
		if (np->n_flag & NREVOKE) {
			error = EIO;
			break;
		}
		if (nmp->nm_vers >= NFS_VER4)
			stategenid = nmp->nm_stategenid;
		error = nmp->nm_funcs->nf_read_rpc_async(np, txoffset, len,
				vfs_context_thread(ctx), vfs_context_ucred(ctx), NULL, &req);
		if (!error)
			error = nmp->nm_funcs->nf_read_rpc_async_finish(np, req, uio, &retlen, &eof);
		if ((nmp->nm_vers >= NFS_VER4) && nfs_mount_state_error_should_restart(error) &&
		    (++restart <= nfs_mount_state_max_restarts(nmp))) { /* guard against no progress */
			lck_mtx_lock(&nmp->nm_lock);
			if ((error != NFSERR_GRACE) && (stategenid == nmp->nm_stategenid)) {
				NP(np, "nfs_read_rpc: error %d, initiating recovery", error);
				nfs_need_recover(nmp, error);
			}
			lck_mtx_unlock(&nmp->nm_lock);
			if (np->n_flag & NREVOKE) {
				error = EIO;
			} else {
				if (error == NFSERR_GRACE)
					tsleep(&nmp->nm_state, (PZERO-1), "nfsgrace", 2*hz);
				if (!(error = nfs_mount_state_wait_for_recovery(nmp)))
					continue;
			}
		}
		if (error)
			break;
		txoffset += retlen;
		tsiz -= retlen;
		if (nfsvers != NFS_VER2) {
			if (eof || (retlen == 0))
				tsiz = 0;
		} else if (retlen < len)
			tsiz = 0;
	}

	FSDBG_BOT(536, np, eof, uio_resid(uio), error);
	return (error);
}

int
nfs3_read_rpc_async(
	nfsnode_t np,
	off_t offset,
	size_t len,
	thread_t thd,
	kauth_cred_t cred,
	struct nfsreq_cbinfo *cb,
	struct nfsreq **reqp)
{
	struct nfsmount *nmp;
	int error = 0, nfsvers;
	struct nfsm_chain nmreq;

	nmp = NFSTONMP(np);
	if (nfs_mount_gone(nmp))
		return (ENXIO);
	nfsvers = nmp->nm_vers;

	nfsm_chain_null(&nmreq);
	nfsm_chain_build_alloc_init(error, &nmreq, NFSX_FH(nfsvers) + 3 * NFSX_UNSIGNED);
	nfsm_chain_add_fh(error, &nmreq, nfsvers, np->n_fhp, np->n_fhsize);
	if (nfsvers == NFS_VER3) {
		nfsm_chain_add_64(error, &nmreq, offset);
		nfsm_chain_add_32(error, &nmreq, len);
	} else {
		nfsm_chain_add_32(error, &nmreq, offset);
		nfsm_chain_add_32(error, &nmreq, len);
		nfsm_chain_add_32(error, &nmreq, 0);
	}
	nfsm_chain_build_done(error, &nmreq);
	nfsmout_if(error);
	error = nfs_request_async(np, NULL, &nmreq, NFSPROC_READ, thd, cred, NULL, 0, cb, reqp);
nfsmout:
	nfsm_chain_cleanup(&nmreq);
	return (error);
}

int
nfs3_read_rpc_async_finish(
	nfsnode_t np,
	struct nfsreq *req,
	uio_t uio,
	size_t *lenp,
	int *eofp)
{
	int error = 0, lockerror, nfsvers, status, eof = 0;
	size_t retlen = 0;
	uint64_t xid;
	struct nfsmount *nmp;
	struct nfsm_chain nmrep;

	nmp = NFSTONMP(np);
	if (nfs_mount_gone(nmp)) {
		nfs_request_async_cancel(req);
		return (ENXIO);
	}
	nfsvers = nmp->nm_vers;

	nfsm_chain_null(&nmrep);

	error = nfs_request_async_finish(req, &nmrep, &xid, &status);
	if (error == EINPROGRESS) /* async request restarted */
		return (error);

	if ((lockerror = nfs_node_lock(np)))
		error = lockerror;
	if (nfsvers == NFS_VER3)
		nfsm_chain_postop_attr_update(error, &nmrep, np, &xid);
	if (!error)
		error = status;
	if (nfsvers == NFS_VER3) {
		nfsm_chain_adv(error, &nmrep, NFSX_UNSIGNED);
		nfsm_chain_get_32(error, &nmrep, eof);
	} else {
		nfsm_chain_loadattr(error, &nmrep, np, nfsvers, &xid);
	}
	if (!lockerror)
		nfs_node_unlock(np);
	nfsm_chain_get_32(error, &nmrep, retlen);
	if ((nfsvers == NFS_VER2) && (retlen > *lenp))
		error = EBADRPC;
	nfsmout_if(error);
	error = nfsm_chain_get_uio(&nmrep, MIN(retlen, *lenp), uio);
	if (eofp) {
		if (nfsvers == NFS_VER3) {
			if (!eof && !retlen)
				eof = 1;
		} else if (retlen < *lenp) {
			eof = 1;
		}
		*eofp = eof;
	}
	*lenp = MIN(retlen, *lenp);
nfsmout:
	nfsm_chain_cleanup(&nmrep);
	return (error);
}

/*
 * NFS write call
 */
int
nfs_vnop_write(
	struct vnop_write_args /* {
		struct vnodeop_desc *a_desc;
		vnode_t a_vp;
		struct uio *a_uio;
		int a_ioflag;
		vfs_context_t a_context;
	} */ *ap)
{
	vfs_context_t ctx = ap->a_context;
	uio_t uio = ap->a_uio;
	vnode_t vp = ap->a_vp;
	nfsnode_t np = VTONFS(vp);
	int ioflag = ap->a_ioflag;
	struct nfsbuf *bp;
	struct nfsmount *nmp = VTONMP(vp);
	daddr64_t lbn;
	int biosize;
	int n, on, error = 0;
	off_t boff, start, end;
	uio_t auio;
	char auio_buf [ UIO_SIZEOF(1) ];
	thread_t thd;
	kauth_cred_t cred;

	FSDBG_TOP(515, np, uio_offset(uio), uio_resid(uio), ioflag);

	if (vnode_vtype(vp) != VREG) {
		FSDBG_BOT(515, np, uio_offset(uio), uio_resid(uio), EIO);
		return (EIO);
	}

	thd = vfs_context_thread(ctx);
	cred = vfs_context_ucred(ctx);

	nfs_data_lock(np, NFS_DATA_LOCK_SHARED);

	if ((error = nfs_node_lock(np))) {
		nfs_data_unlock(np);
		FSDBG_BOT(515, np, uio_offset(uio), uio_resid(uio), error);
		return (error);
	}
	np->n_wrbusy++;

	if (np->n_flag & NWRITEERR) {
		error = np->n_error;
		np->n_flag &= ~NWRITEERR;
=======
			if (vp->v_mount->mnt_flag & MNT_RDONLY)
				return (EROFS);
			FSDBG_TOP(512, np->n_size, vap->va_size,
				  np->n_vattr.va_size, np->n_flag);
			if (np->n_flag & NMODIFIED) {
 				if (vap->va_size == 0)
 					error = nfs_vinvalbuf(vp, 0,
 						ap->a_cred, ap->a_p, 1);
 				else
 					error = nfs_vinvalbuf(vp, V_SAVE,
 						ap->a_cred, ap->a_p, 1);
	 			if (error) {
					printf("nfs_setattr: nfs_vinvalbuf %d\n", error);
					FSDBG_BOT(512, np->n_size, vap->va_size,
						  np->n_vattr.va_size, -1);
 					return (error);
				}
			} else if (np->n_size > vap->va_size) { /* shrinking? */
				daddr_t obn, bn;
				int biosize;
				struct buf *bp;

				biosize = min(vp->v_mount->mnt_stat.f_iosize,
					      PAGE_SIZE);
				obn = (np->n_size - 1) / biosize;
				bn = vap->va_size / biosize; 
				for ( ; obn >= bn; obn--)
					if (incore(vp, obn)) {
						bp = getblk(vp, obn, biosize, 0,
							    0, BLK_READ);
						FSDBG(512, bp, bp->b_flags,
						      0, obn);
						SET(bp->b_flags, B_INVAL);
						brelse(bp);
					}
			}
 			tsize = np->n_size;
			np->n_size = np->n_vattr.va_size = vap->va_size;
			ubc_setsize(vp, (off_t)vap->va_size); /* XXX */
  		};
  	} else if ((vap->va_mtime.tv_sec != VNOVAL ||
		    vap->va_atime.tv_sec != VNOVAL) &&
		   (np->n_flag & NMODIFIED) && vp->v_type == VREG &&
		   (error = nfs_vinvalbuf(vp, V_SAVE, ap->a_cred,
					  ap->a_p, 1)) == EINTR)
		return (error);
	error = nfs_setattrrpc(vp, vap, ap->a_cred, ap->a_p);
	FSDBG_BOT(512, np->n_size, vap->va_size, np->n_vattr.va_size, error);
	if (error && vap->va_size != VNOVAL) {
		/* make every effort to resync file size w/ server... */
		int err = 0; /* preserve "error" for return */

		printf("nfs_setattr: nfs_setattrrpc %d\n", error);
		np->n_size = np->n_vattr.va_size = tsize;
		ubc_setsize(vp, (off_t)np->n_size); /* XXX check error */
		vap->va_size = tsize;
		err = nfs_setattrrpc(vp, vap, ap->a_cred, ap->a_p);
		if (err)
			printf("nfs_setattr1: nfs_setattrrpc %d\n", err);
>>>>>>> origin/10.1
	}
	if (np->n_flag & NNEEDINVALIDATE) {
		np->n_flag &= ~NNEEDINVALIDATE;
		nfs_node_unlock(np);
		nfs_data_unlock(np);
		nfs_vinvalbuf(vp, V_SAVE|V_IGNORE_WRITEERR, ctx, 1);
		nfs_data_lock(np, NFS_DATA_LOCK_SHARED);
	} else {
		nfs_node_unlock(np);
	}
	if (error)
		goto out;

<<<<<<< HEAD
	biosize = nmp->nm_biosize;

	if (ioflag & (IO_APPEND | IO_SYNC)) {
		nfs_node_lock_force(np);
		if (np->n_flag & NMODIFIED) {
			NATTRINVALIDATE(np);
			nfs_node_unlock(np);
			nfs_data_unlock(np);
			error = nfs_vinvalbuf(vp, V_SAVE, ctx, 1);
			nfs_data_lock(np, NFS_DATA_LOCK_SHARED);
			if (error) {
				FSDBG(515, np, uio_offset(uio), 0x10bad01, error);
				goto out;
=======
/*
 * Do an nfs setattr rpc.
 */
static int
nfs_setattrrpc(vp, vap, cred, procp)
	register struct vnode *vp;
	register struct vattr *vap;
	struct ucred *cred;
	struct proc *procp;
{
	register struct nfsv2_sattr *sp;
	register caddr_t cp;
	register long t1, t2;
	caddr_t bpos, dpos, cp2;
	u_long *tl;
	int error = 0, wccpostattr = 0;
	struct mbuf *mreq, *mrep, *md, *mb, *mb2;
	int v3 = NFS_ISV3(vp);
	u_int64_t xid;

	nfsstats.rpccnt[NFSPROC_SETATTR]++;
	nfsm_reqhead(vp, NFSPROC_SETATTR, NFSX_FH(v3) + NFSX_SATTR(v3));
	nfsm_fhtom(vp, v3);
	if (v3) {
		if (vap->va_mode != (u_short)VNOVAL) {
			nfsm_build(tl, u_long *, 2 * NFSX_UNSIGNED);
			*tl++ = nfs_true;
			*tl = txdr_unsigned(vap->va_mode);
		} else {
			nfsm_build(tl, u_long *, NFSX_UNSIGNED);
			*tl = nfs_false;
		}
		if (vap->va_uid != (uid_t)VNOVAL) {
			nfsm_build(tl, u_long *, 2 * NFSX_UNSIGNED);
			*tl++ = nfs_true;
			*tl = txdr_unsigned(vap->va_uid);
		} else {
			nfsm_build(tl, u_long *, NFSX_UNSIGNED);
			*tl = nfs_false;
		}
		if (vap->va_gid != (gid_t)VNOVAL) {
			nfsm_build(tl, u_long *, 2 * NFSX_UNSIGNED);
			*tl++ = nfs_true;
			*tl = txdr_unsigned(vap->va_gid);
		} else {
			nfsm_build(tl, u_long *, NFSX_UNSIGNED);
			*tl = nfs_false;
		}
		if (vap->va_size != VNOVAL) {
			nfsm_build(tl, u_long *, 3 * NFSX_UNSIGNED);
			*tl++ = nfs_true;
			txdr_hyper(&vap->va_size, tl);
		} else {
			nfsm_build(tl, u_long *, NFSX_UNSIGNED);
			*tl = nfs_false;
		}
		if (vap->va_atime.tv_sec != VNOVAL) {
			if (vap->va_atime.tv_sec != time.tv_sec) {
				nfsm_build(tl, u_long *, 3 * NFSX_UNSIGNED);
				*tl++ = txdr_unsigned(NFSV3SATTRTIME_TOCLIENT);
				txdr_nfsv3time(&vap->va_atime, tl);
			} else {
				nfsm_build(tl, u_long *, NFSX_UNSIGNED);
				*tl = txdr_unsigned(NFSV3SATTRTIME_TOSERVER);
>>>>>>> origin/10.1
			}
		} else {
			nfs_node_unlock(np);
		}
		if (ioflag & IO_APPEND) {
			nfs_data_unlock(np);
			/* nfs_getattr() will check changed and purge caches */
			error = nfs_getattr(np, NULL, ctx, NGA_UNCACHED);
			/* we'll be extending the file, so take the data lock exclusive */
			nfs_data_lock(np, NFS_DATA_LOCK_EXCLUSIVE);
			if (error) {
				FSDBG(515, np, uio_offset(uio), 0x10bad02, error);
				goto out;
			}
			uio_setoffset(uio, np->n_size);
		}
<<<<<<< HEAD
	}
	if (uio_offset(uio) < 0) {
		error = EINVAL;
		FSDBG_BOT(515, np, uio_offset(uio), 0xbad0ff, error);
		goto out;
	}
	if (uio_resid(uio) == 0)
		goto out;

	if (((uio_offset(uio) + uio_resid(uio)) > (off_t)np->n_size) && !(ioflag & IO_APPEND)) {
		/*
		 * It looks like we'll be extending the file, so take the data lock exclusive.
		 */
		nfs_data_unlock(np);
		nfs_data_lock(np, NFS_DATA_LOCK_EXCLUSIVE);

		/*
		 * Also, if the write begins after the previous EOF buffer, make sure to zero
		 * and validate the new bytes in that buffer.
=======
		nfsm_build(tl, u_long *, NFSX_UNSIGNED);
		*tl = nfs_false;
	} else {
		nfsm_build(sp, struct nfsv2_sattr *, NFSX_V2SATTR);
		if (vap->va_mode == (u_short)VNOVAL)
			sp->sa_mode = VNOVAL;
		else
			sp->sa_mode = vtonfsv2_mode(vp->v_type, vap->va_mode);
		if (vap->va_uid == (uid_t)VNOVAL)
			sp->sa_uid = VNOVAL;
		else
			sp->sa_uid = txdr_unsigned(vap->va_uid);
		if (vap->va_gid == (gid_t)VNOVAL)
			sp->sa_gid = VNOVAL;
		else
			sp->sa_gid = txdr_unsigned(vap->va_gid);
		sp->sa_size = txdr_unsigned(vap->va_size);
		txdr_nfsv2time(&vap->va_atime, &sp->sa_atime);
		txdr_nfsv2time(&vap->va_mtime, &sp->sa_mtime);
	}
	nfsm_request(vp, NFSPROC_SETATTR, procp, cred, &xid);
	if (v3) {
<<<<<<< HEAD
		nfsm_wcc_data(vp, wccflag, &xid);
		if (!wccflag && vp->v_type != VBAD) /* EINVAL on VBAD node */
    			VTONFS(vp)->n_attrstamp = 0;
	} else
		nfsm_loadattr(vp, (struct vattr *)0, &xid);
=======
		time_t premtime = 0;
		if (mrep) {
			nfsm_wcc_data(vp, premtime, wccpostattr, &xid);
		}
		/* if file hadn't changed, update cached mtime */
		if (VTONFS(vp)->n_mtime == premtime) {
			VTONFS(vp)->n_mtime = VTONFS(vp)->n_vattr.va_mtime.tv_sec;
		}
		/* if directory hadn't changed, update namecache mtime */
		if ((vp->v_type == VDIR) && (VTONFS(vp)->n_ncmtime == premtime)) {
			VTONFS(vp)->n_ncmtime = VTONFS(vp)->n_vattr.va_mtime.tv_sec;
		}
		if (!wccpostattr)
    			VTONFS(vp)->n_xid = 0;
	} else {
		if (mrep) {
			nfsm_loadattr(vp, (struct vattr *)0, &xid);
		}
	}
>>>>>>> origin/10.3
	nfsm_reqdone;
	return (error);
}

/*
 * nfs lookup call, one step at a time...
 * First look in cache
 * If not found, unlock the directory nfsnode and do the rpc
 */
static int
nfs_lookup(ap)
	struct vop_lookup_args /* {
		struct vnodeop_desc *a_desc;
		struct vnode *a_dvp;
		struct vnode **a_vpp;
		struct componentname *a_cnp;
	} */ *ap;
{
	register struct componentname *cnp = ap->a_cnp;
	register struct vnode *dvp = ap->a_dvp;
	register struct vnode **vpp = ap->a_vpp;
	register int flags = cnp->cn_flags;
	register struct vnode *newvp;
	register u_long *tl;
	register caddr_t cp;
	register long t1, t2;
	struct nfsmount *nmp;
	caddr_t bpos, dpos, cp2;
	struct mbuf *mreq, *mrep, *md, *mb, *mb2;
	long len;
	nfsfh_t *fhp;
	struct nfsnode *np;
	int lockparent, wantparent, error = 0, attrflag, fhsize;
	int v3 = NFS_ISV3(dvp);
	struct proc *p = cnp->cn_proc;
	int worldbuildworkaround = 1;
	u_int64_t xid;

	if ((flags & ISLASTCN) && (dvp->v_mount->mnt_flag & MNT_RDONLY) &&
	    (cnp->cn_nameiop == DELETE || cnp->cn_nameiop == RENAME))
		return (EROFS);
	*vpp = NULLVP;
	if (dvp->v_type != VDIR)
		return (ENOTDIR);
	lockparent = flags & LOCKPARENT;
	wantparent = flags & (LOCKPARENT|WANTPARENT);
	nmp = VFSTONFS(dvp->v_mount);
	np = VTONFS(dvp);

<<<<<<< HEAD
	if (worldbuildworkaround) {
		/* 
		 * Temporary workaround for world builds to not have dvp go
		 * VBAD on during server calls in this routine. When
		 * the real ref counting problem is found take this out.
		 * Note if this was later and before the nfsm_request
		 * set up, the workaround did not work (NOTE other difference
		 * was I only put one VREF in that time. Thus it needs
		 * to be above the cache_lookup branch or with 2 VREFS. Not
		 * sure which. Can't play with world builds right now to see
		 * which.  VOP_ACCESS could also make it go to server.  - EKN
>>>>>>> origin/10.1
=======
	if (!VOP_GETATTR(dvp, &vattr, cnp->cn_cred, p) &&
	    (np->n_ncmtime != vattr.va_mtime.tv_sec)) {
		/*
		 * This directory has changed on us.
		 * Purge any name cache entries.
		 */
		cache_purge(dvp);
		np->n_ncmtime = vattr.va_mtime.tv_sec;
	}

	if ((error = cache_lookup(dvp, vpp, cnp)) && error != ENOENT) {
		int vpid;

		newvp = *vpp;
		vpid = newvp->v_id;

		/*
		 * See the comment starting `Step through' in ufs/ufs_lookup.c
		 * for an explanation of the locking protocol
>>>>>>> origin/10.3
		 */
		struct nfsbuf *eofbp = NULL;
		daddr64_t eofbn = np->n_size / biosize;
		int eofoff = np->n_size % biosize;
		lbn = uio_offset(uio) / biosize;

		if (eofoff && (eofbn < lbn)) {
			if ((error = nfs_buf_get(np, eofbn, biosize, thd, NBLK_WRITE|NBLK_ONLYVALID, &eofbp)))
				goto out;
			np->n_size += (biosize - eofoff);
			nfs_node_lock_force(np);
			CLR(np->n_flag, NUPDATESIZE);
			np->n_flag |= NMODIFIED;
			nfs_node_unlock(np);
			FSDBG(516, np, np->n_size, np->n_vattr.nva_size, 0xf00d0001);
			ubc_setsize(vp, (off_t)np->n_size); /* XXX errors */
			if (eofbp) {
				/*
				 * For the old last page, don't zero bytes if there
				 * are invalid bytes in that page (i.e. the page isn't
				 * currently valid).
				 * For pages after the old last page, zero them and
				 * mark them as valid.
				 */
				char *d;
				int i;
				if (ioflag & IO_NOCACHE)
					SET(eofbp->nb_flags, NB_NOCACHE);
				NFS_BUF_MAP(eofbp);
				FSDBG(516, eofbp, eofoff, biosize - eofoff, 0xe0fff01e);
				d = eofbp->nb_data;
				i = eofoff/PAGE_SIZE;
				while (eofoff < biosize) {
					int poff = eofoff & PAGE_MASK;
					if (!poff || NBPGVALID(eofbp,i)) {
						bzero(d + eofoff, PAGE_SIZE - poff);
						NBPGVALID_SET(eofbp, i);
					}
					eofoff += PAGE_SIZE - poff;
					i++;
				}
				nfs_buf_release(eofbp, 1);
			}
		}
	}

	do {
		OSAddAtomic64(1, &nfsstats.biocache_writes);
		lbn = uio_offset(uio) / biosize;
		on = uio_offset(uio) % biosize;
		n = biosize - on;
		if (uio_resid(uio) < n)
			n = uio_resid(uio);
again:
		/*
		 * Get a cache block for writing.  The range to be written is
		 * (off..off+n) within the block.  We ensure that the block
		 * either has no dirty region or that the given range is
		 * contiguous with the existing dirty region.
		 */
		error = nfs_buf_get(np, lbn, biosize, thd, NBLK_WRITE, &bp);
		if (error)
			goto out;
		/* map the block because we know we're going to write to it */
		NFS_BUF_MAP(bp);

<<<<<<< HEAD
		if (ioflag & IO_NOCACHE)
			SET(bp->nb_flags, NB_NOCACHE);

		if (!IS_VALID_CRED(bp->nb_wcred)) {
			kauth_cred_ref(cred);
			bp->nb_wcred = cred;
		}
=======
		if ((error = VOP_ACCESS(dvp, VEXEC, cnp->cn_cred, p))) {
			*vpp = NULLVP;
			goto error_return;
			}

		/* got to check to make sure the vnode didn't go away if access went to server */
		if ((*vpp)->v_type == VBAD) {
			error = EINVAL;
			goto error_return;
			}
>>>>>>> origin/10.1

		/*
		 * If there's already a dirty range AND dirty pages in this block we
		 * need to send a commit AND write the dirty pages before continuing.
		 *
		 * If there's already a dirty range OR dirty pages in this block
		 * and the new write range is not contiguous with the existing range,
		 * then force the buffer to be written out now.
		 * (We used to just extend the dirty range to cover the valid,
		 * but unwritten, data in between also.  But writing ranges
		 * of data that weren't actually written by an application
		 * risks overwriting some other client's data with stale data
		 * that's just masquerading as new written data.)
		 */
		if (bp->nb_dirtyend > 0) {
		    if (on > bp->nb_dirtyend || (on + n) < bp->nb_dirtyoff || bp->nb_dirty) {
			FSDBG(515, np, uio_offset(uio), bp, 0xd15c001);
			/* write/commit buffer "synchronously" */
			/* (NB_STABLE indicates that data writes should be FILESYNC) */
			CLR(bp->nb_flags, (NB_DONE | NB_ERROR | NB_INVAL));
			SET(bp->nb_flags, (NB_ASYNC | NB_STABLE));
			error = nfs_buf_write(bp);
			if (error)
			    goto out;
			goto again;
		    }
		} else if (bp->nb_dirty) {
		    int firstpg, lastpg;
		    u_int32_t pagemask;
		    /* calculate write range pagemask */
		    firstpg = on/PAGE_SIZE;
		    lastpg = (on+n-1)/PAGE_SIZE;
		    pagemask = ((1 << (lastpg+1)) - 1) & ~((1 << firstpg) - 1);
		    /* check if there are dirty pages outside the write range */
		    if (bp->nb_dirty & ~pagemask) {
			FSDBG(515, np, uio_offset(uio), bp, 0xd15c002);
			/* write/commit buffer "synchronously" */
			/* (NB_STABLE indicates that data writes should be FILESYNC) */
			CLR(bp->nb_flags, (NB_DONE | NB_ERROR | NB_INVAL));
			SET(bp->nb_flags, (NB_ASYNC | NB_STABLE));
			error = nfs_buf_write(bp);
			if (error)
			    goto out;
			goto again;
		    }
		    /* if the first or last pages are already dirty */
		    /* make sure that the dirty range encompasses those pages */
		    if (NBPGDIRTY(bp,firstpg) || NBPGDIRTY(bp,lastpg)) {
			FSDBG(515, np, uio_offset(uio), bp, 0xd15c003);
		    	bp->nb_dirtyoff = min(on, firstpg * PAGE_SIZE);
			if (NBPGDIRTY(bp,lastpg)) {
			    bp->nb_dirtyend = (lastpg+1) * PAGE_SIZE;
			    /* clip to EOF */
			    if (NBOFF(bp) + bp->nb_dirtyend > (off_t)np->n_size) {
				    bp->nb_dirtyend = np->n_size - NBOFF(bp);
				    if (bp->nb_dirtyoff >= bp->nb_dirtyend)
					    bp->nb_dirtyoff = bp->nb_dirtyend = 0;
			    }
			} else
			    bp->nb_dirtyend = on+n;
		    }
		}

		/*
		 * Are we extending the size of the file with this write?
		 * If so, update file size now that we have the block.
		 * If there was a partial buf at the old eof, validate
		 * and zero the new bytes.
		 */
		if ((uio_offset(uio) + n) > (off_t)np->n_size) {
			daddr64_t eofbn = np->n_size / biosize;
			int neweofoff = (uio_offset(uio) + n) % biosize;

			FSDBG(515, 0xb1ffa000, uio_offset(uio) + n, eofoff, neweofoff);

			/* if we're extending within the same last block */
			/* and the block is flagged as being cached... */
			if ((lbn == eofbn) && ISSET(bp->nb_flags, NB_CACHE)) {
				/* ...check that all pages in buffer are valid */
				int endpg = ((neweofoff ? neweofoff : biosize) - 1)/PAGE_SIZE;
				u_int32_t pagemask;
				/* pagemask only has to extend to last page being written to */
				pagemask = (1 << (endpg+1)) - 1;
				FSDBG(515, 0xb1ffa001, bp->nb_valid, pagemask, 0);
				if ((bp->nb_valid & pagemask) != pagemask) {
					/* zerofill any hole */
					if (on > bp->nb_validend) {
						int i;
						for (i=bp->nb_validend/PAGE_SIZE; i <= (on - 1)/PAGE_SIZE; i++)
							NBPGVALID_SET(bp, i);
						NFS_BUF_MAP(bp);
						FSDBG(516, bp, bp->nb_validend, on - bp->nb_validend, 0xf01e);
						bzero((char *)bp->nb_data + bp->nb_validend,
							on - bp->nb_validend);
					}
					/* zerofill any trailing data in the last page */
					if (neweofoff) {
						NFS_BUF_MAP(bp);
						FSDBG(516, bp, neweofoff, PAGE_SIZE - (neweofoff & PAGE_MASK), 0xe0f);
						bzero((char *)bp->nb_data + neweofoff,
							PAGE_SIZE - (neweofoff & PAGE_MASK));
					}
				}
			}
			np->n_size = uio_offset(uio) + n;
			nfs_node_lock_force(np);
			CLR(np->n_flag, NUPDATESIZE);
			np->n_flag |= NMODIFIED;
			nfs_node_unlock(np);
			FSDBG(516, np, np->n_size, np->n_vattr.nva_size, 0xf00d0001);
			ubc_setsize(vp, (off_t)np->n_size); /* XXX errors */
		}
<<<<<<< HEAD
		/*
		 * If dirtyend exceeds file size, chop it down.  This should
		 * not occur unless there is a race.
		 */
		if (NBOFF(bp) + bp->nb_dirtyend > (off_t)np->n_size) {
			bp->nb_dirtyend = np->n_size - NBOFF(bp);
			if (bp->nb_dirtyoff >= bp->nb_dirtyend)
				bp->nb_dirtyoff = bp->nb_dirtyend = 0;
		}
		/*
		 * UBC doesn't handle partial pages, so we need to make sure
		 * that any pages left in the page cache are completely valid.
		 *
		 * Writes that are smaller than a block are delayed if they
		 * don't extend to the end of the block.
		 *
		 * If the block isn't (completely) cached, we may need to read
		 * in some parts of pages that aren't covered by the write.
		 * If the write offset (on) isn't page aligned, we'll need to
		 * read the start of the first page being written to.  Likewise,
		 * if the offset of the end of the write (on+n) isn't page aligned,
		 * we'll need to read the end of the last page being written to.
		 *
		 * Notes:
		 * We don't want to read anything we're just going to write over.
		 * We don't want to read anything we're just going drop when the
		 *   I/O is complete (i.e. don't do reads for NOCACHE requests).
		 * We don't want to issue multiple I/Os if we don't have to
		 *   (because they're synchronous rpcs).
		 * We don't want to read anything we already have modified in the
		 *   page cache.
		 */
		if (!ISSET(bp->nb_flags, NB_CACHE) && (n < biosize)) {
			int firstpg, lastpg, dirtypg;
			int firstpgoff, lastpgoff;
			start = end = -1;
			firstpg = on/PAGE_SIZE;
			firstpgoff = on & PAGE_MASK;
			lastpg = (on+n-1)/PAGE_SIZE;
			lastpgoff = (on+n) & PAGE_MASK;
			if (firstpgoff && !NBPGVALID(bp,firstpg)) {
				/* need to read start of first page */
				start = firstpg * PAGE_SIZE;
				end = start + firstpgoff;
			}
			if (lastpgoff && !NBPGVALID(bp,lastpg)) {
				/* need to read end of last page */
				if (start < 0)
					start = (lastpg * PAGE_SIZE) + lastpgoff;
				end = (lastpg + 1) * PAGE_SIZE;
			}
			if (ISSET(bp->nb_flags, NB_NOCACHE)) {
				/*
				 * For nocache writes, if there is any partial page at the
				 * start or end of the write range, then we do the write
				 * synchronously to make sure that we can drop the data
				 * from the cache as soon as the WRITE finishes.  Normally,
				 * we would do an unstable write and not drop the data until
				 * it was committed.  But doing that here would risk allowing
				 * invalid data to be read from the cache between the WRITE
				 * and the COMMIT.
				 * (NB_STABLE indicates that data writes should be FILESYNC)
				 */
				if (end > start)
					SET(bp->nb_flags, NB_STABLE);
				goto skipread;
			}
			if (end > start) {
				/* need to read the data in range: start...end-1 */

				/* first, check for dirty pages in between */
				/* if there are, we'll have to do two reads because */
				/* we don't want to overwrite the dirty pages. */
				for (dirtypg=start/PAGE_SIZE; dirtypg <= (end-1)/PAGE_SIZE; dirtypg++)
					if (NBPGDIRTY(bp,dirtypg))
						break;

				/* if start is at beginning of page, try */
				/* to get any preceeding pages as well. */
				if (!(start & PAGE_MASK)) {
					/* stop at next dirty/valid page or start of block */
					for (; start > 0; start-=PAGE_SIZE)
						if (NBPGVALID(bp,((start-1)/PAGE_SIZE)))
							break;
				}

				NFS_BUF_MAP(bp);
				/* setup uio for read(s) */
				boff = NBOFF(bp);
				auio = uio_createwithbuffer(1, 0, UIO_SYSSPACE, UIO_READ,
					&auio_buf, sizeof(auio_buf));

				if (dirtypg <= (end-1)/PAGE_SIZE) {
					/* there's a dirty page in the way, so just do two reads */
					/* we'll read the preceding data here */
					uio_reset(auio, boff + start, UIO_SYSSPACE, UIO_READ);
					uio_addiov(auio, CAST_USER_ADDR_T(bp->nb_data + start), on - start);
					error = nfs_read_rpc(np, auio, ctx);
					if (error) {
						/* couldn't read the data, so treat buffer as synchronous NOCACHE */
						SET(bp->nb_flags, (NB_NOCACHE|NB_STABLE));
						goto skipread;
					}
					if (uio_resid(auio) > 0) {
						FSDBG(516, bp, (caddr_t)uio_curriovbase(auio) - bp->nb_data, uio_resid(auio), 0xd00dee01);
						bzero(CAST_DOWN(caddr_t, uio_curriovbase(auio)), uio_resid(auio));
					}
					if (!error) {
						/* update validoff/validend if necessary */
						if ((bp->nb_validoff < 0) || (bp->nb_validoff > start))
							bp->nb_validoff = start;
						if ((bp->nb_validend < 0) || (bp->nb_validend < on))
							bp->nb_validend = on;
						if ((off_t)np->n_size > boff + bp->nb_validend)
							bp->nb_validend = min(np->n_size - (boff + start), biosize);
						/* validate any pages before the write offset */
						for (; start < on/PAGE_SIZE; start+=PAGE_SIZE)
							NBPGVALID_SET(bp, start/PAGE_SIZE);
					}
					/* adjust start to read any trailing data */
					start = on+n;
				}

<<<<<<< HEAD
				/* if end is at end of page, try to */
				/* get any following pages as well. */
				if (!(end & PAGE_MASK)) {
					/* stop at next valid page or end of block */
					for (; end < biosize; end+=PAGE_SIZE)
						if (NBPGVALID(bp,end/PAGE_SIZE))
							break;
				}
=======
=======
		if (vpid == newvp->v_id) {
			nfsstats.lookupcache_hits++;
			if (cnp->cn_nameiop != LOOKUP && (flags & ISLASTCN))
				cnp->cn_flags |= SAVENAME;
			error = 0; /* ignore any from VOP_GETATTR  */
			goto error_return;
		}
		vput(newvp);
		if ((dvp != newvp) && lockparent && (flags & ISLASTCN))
			VOP_UNLOCK(dvp, 0, p);
cache_lookup_out:
>>>>>>> origin/10.3
		error = vn_lock(dvp, LK_EXCLUSIVE, p);
		*vpp = NULLVP;
		if (error) 
			goto error_return;
	}

	/* 
	 * Got to check to make sure the vnode didn't go away if VOP_GETATTR went to server
	 * or callers prior to this blocked and had it go VBAD.
	 */
	if (dvp->v_type == VBAD) { 
		error = EINVAL;
		goto error_return;
	}

	error = 0;
	newvp = NULLVP;
	nfsstats.lookupcache_misses++;
	nfsstats.rpccnt[NFSPROC_LOOKUP]++;
	len = cnp->cn_namelen;
	nfsm_reqhead(dvp, NFSPROC_LOOKUP,
		NFSX_FH(v3) + NFSX_UNSIGNED + nfsm_rndup(len));
	nfsm_fhtom(dvp, v3);
	nfsm_strtom(cnp->cn_nameptr, len, NFS_MAXNAMLEN);
	/* nfsm_request for NFSv2 causes you to goto to nfsmout upon errors */
	nfsm_request(dvp, NFSPROC_LOOKUP, cnp->cn_proc, cnp->cn_cred, &xid); 

	if (error) {
		if (mrep) {
			nfsm_postop_attr(dvp, attrflag, &xid);
			m_freem(mrep);
		}
		goto nfsmout;
	}
	nfsm_getfh(fhp, fhsize, v3);
>>>>>>> origin/10.1

				if (((boff+start) >= (off_t)np->n_size) ||
				    ((start >= on) && ((boff + on + n) >= (off_t)np->n_size))) {
					/*
					 * Either this entire read is beyond the current EOF
					 * or the range that we won't be modifying (on+n...end)
					 * is all beyond the current EOF.
					 * No need to make a trip across the network to
					 * read nothing.  So, just zero the buffer instead.
					 */
					FSDBG(516, bp, start, end - start, 0xd00dee00);
					bzero(bp->nb_data + start, end - start);
					error = 0;
				} else {
					/* now we'll read the (rest of the) data */
					uio_reset(auio, boff + start, UIO_SYSSPACE, UIO_READ);
					uio_addiov(auio, CAST_USER_ADDR_T(bp->nb_data + start), end - start);
					error = nfs_read_rpc(np, auio, ctx);
					if (error) {
						/* couldn't read the data, so treat buffer as synchronous NOCACHE */
						SET(bp->nb_flags, (NB_NOCACHE|NB_STABLE));
						goto skipread;
					}
					if (uio_resid(auio) > 0) {
						FSDBG(516, bp, (caddr_t)uio_curriovbase(auio) - bp->nb_data, uio_resid(auio), 0xd00dee02);
						bzero(CAST_DOWN(caddr_t, uio_curriovbase(auio)), uio_resid(auio));
					}
				}
				if (!error) {
					/* update validoff/validend if necessary */
					if ((bp->nb_validoff < 0) || (bp->nb_validoff > start))
						bp->nb_validoff = start;
					if ((bp->nb_validend < 0) || (bp->nb_validend < end))
						bp->nb_validend = end;
					if ((off_t)np->n_size > boff + bp->nb_validend)
						bp->nb_validend = min(np->n_size - (boff + start), biosize);
					/* validate any pages before the write offset's page */
					for (; start < (off_t)trunc_page_32(on); start+=PAGE_SIZE)
						NBPGVALID_SET(bp, start/PAGE_SIZE);
					/* validate any pages after the range of pages being written to */
					for (; (end - 1) > (off_t)round_page_32(on+n-1); end-=PAGE_SIZE)
						NBPGVALID_SET(bp, (end-1)/PAGE_SIZE);
				}
				/* Note: pages being written to will be validated when written */
			}
		}
skipread:

		if (ISSET(bp->nb_flags, NB_ERROR)) {
			error = bp->nb_error;
			nfs_buf_release(bp, 1);
			goto out;
		}
<<<<<<< HEAD
=======
		newvp = NFSTOV(np);
		if (v3) {
			u_int64_t dxid = xid;

			nfsm_postop_attr(newvp, attrflag, &xid);
			nfsm_postop_attr(dvp, attrflag, &dxid);
			if (np->n_xid == 0) {
				/*
				 * VFS currently requires that we have valid
				 * attributes when returning success.
				 */
				error = VOP_GETATTR(newvp, &vattr, cnp->cn_cred, p);
				if (error) {
					m_freem(mrep);
					vput(newvp);
					goto error_return;
				}
			}
		} else
			nfsm_loadattr(newvp, (struct vattr *)0, &xid);
		*vpp = newvp;
		m_freem(mrep);
		cnp->cn_flags |= SAVENAME;
		if (!lockparent)
			VOP_UNLOCK(dvp, 0, p);
		error = 0;
		goto error_return;
	}
>>>>>>> origin/10.1

		nfs_node_lock_force(np);
		np->n_flag |= NMODIFIED;
		nfs_node_unlock(np);

		NFS_BUF_MAP(bp);
		error = uiomove((char *)bp->nb_data + on, n, uio);
		if (error) {
			SET(bp->nb_flags, NB_ERROR);
			nfs_buf_release(bp, 1);
			goto out;
		}

		/* validate any pages written to */
		start = on & ~PAGE_MASK;
		for (; start < on+n; start += PAGE_SIZE) {
			NBPGVALID_SET(bp, start/PAGE_SIZE);
			/*
			 * This may seem a little weird, but we don't actually set the
			 * dirty bits for writes.  This is because we keep the dirty range
			 * in the nb_dirtyoff/nb_dirtyend fields.  Also, particularly for
			 * delayed writes, when we give the pages back to the VM we don't
			 * want to keep them marked dirty, because when we later write the
			 * buffer we won't be able to tell which pages were written dirty
			 * and which pages were mmapped and dirtied.
			 */
		}
		if (bp->nb_dirtyend > 0) {
			bp->nb_dirtyoff = min(on, bp->nb_dirtyoff);
			bp->nb_dirtyend = max((on + n), bp->nb_dirtyend);
		} else {
			bp->nb_dirtyoff = on;
			bp->nb_dirtyend = on + n;
		}
<<<<<<< HEAD
		if (bp->nb_validend <= 0 || bp->nb_validend < bp->nb_dirtyoff ||
		    bp->nb_validoff > bp->nb_dirtyend) {
			bp->nb_validoff = bp->nb_dirtyoff;
			bp->nb_validend = bp->nb_dirtyend;
		} else {
			bp->nb_validoff = min(bp->nb_validoff, bp->nb_dirtyoff);
			bp->nb_validend = max(bp->nb_validend, bp->nb_dirtyend);
=======
		if (!lockparent || !(flags & ISLASTCN))
			VOP_UNLOCK(dvp, 0, p);
		newvp = NFSTOV(np);
	}
	if (v3) {
		u_int64_t dxid = xid;

		nfsm_postop_attr(newvp, attrflag, &xid);
		nfsm_postop_attr(dvp, attrflag, &dxid);
		if (np->n_xid == 0) {
			/*
			 * VFS currently requires that we have valid
			 * attributes when returning success.
			 */
			error = VOP_GETATTR(newvp, &vattr, cnp->cn_cred, p);
			if (error) {
				if (unlockdvp)
					VOP_UNLOCK(dvp, 0, p);
				m_freem(mrep);
				vput(newvp);
				goto error_return;
			}
		}
	} else
		nfsm_loadattr(newvp, (struct vattr *)0, &xid);
	if (cnp->cn_nameiop != LOOKUP && (flags & ISLASTCN))
		cnp->cn_flags |= SAVENAME;
	if ((cnp->cn_flags & MAKEENTRY) &&
	    (cnp->cn_nameiop != DELETE || !(flags & ISLASTCN))) {
		cache_enter(dvp, newvp, cnp);
	}
	*vpp = newvp;
	nfsm_reqdone;
	if (error) {
		if (newvp != NULLVP) {
			vrele(newvp);
			*vpp = NULLVP;
		}
		if ((cnp->cn_nameiop == CREATE || cnp->cn_nameiop == RENAME) &&
		    (flags & ISLASTCN) && error == ENOENT) {
			if (!lockparent)
				VOP_UNLOCK(dvp, 0, p);
			if (dvp->v_mount->mnt_flag & MNT_RDONLY)
				error = EROFS;
			else
				error = EJUSTRETURN;
>>>>>>> origin/10.1
		}
		if (!ISSET(bp->nb_flags, NB_CACHE))
			nfs_buf_normalize_valid_range(np, bp);

		/*
		 * Since this block is being modified, it must be written
		 * again and not just committed.
		 */
		if (ISSET(bp->nb_flags, NB_NEEDCOMMIT)) {
			nfs_node_lock_force(np);
			if (ISSET(bp->nb_flags, NB_NEEDCOMMIT)) {
				np->n_needcommitcnt--;
				CHECK_NEEDCOMMITCNT(np);
			}
			CLR(bp->nb_flags, NB_NEEDCOMMIT);
			nfs_node_unlock(np);
		}

		if (ioflag & IO_SYNC) {
			error = nfs_buf_write(bp);
			if (error)
				goto out;
		} else if (((n + on) == biosize) || (ioflag & IO_APPEND) ||
			   (ioflag & IO_NOCACHE) || ISSET(bp->nb_flags, NB_NOCACHE)) {
			SET(bp->nb_flags, NB_ASYNC);
			error = nfs_buf_write(bp);
			if (error)
				goto out;
		} else {
			/* If the block wasn't already delayed: charge for the write */
			if (!ISSET(bp->nb_flags, NB_DELWRI)) {
				proc_t p = vfs_context_proc(ctx);
				if (p && p->p_stats)
					OSIncrementAtomicLong(&p->p_stats->p_ru.ru_oublock);
			}
			nfs_buf_write_delayed(bp);
		}
		if (np->n_needcommitcnt >= NFS_A_LOT_OF_NEEDCOMMITS)
		        nfs_flushcommits(np, 1);

<<<<<<< HEAD
	} while (uio_resid(uio) > 0 && n > 0);
=======

/*
 * nfs readlink call
 */
static int
nfs_readlink(ap)
	struct vop_readlink_args /* {
		struct vnode *a_vp;
		struct uio *a_uio;
		struct ucred *a_cred;
	} */ *ap;
{
	register struct vnode *vp = ap->a_vp;
>>>>>>> origin/10.1

out:
	nfs_node_lock_force(np);
	np->n_wrbusy--;
	nfs_node_unlock(np);
	nfs_data_unlock(np);
	FSDBG_BOT(515, np, uio_offset(uio), uio_resid(uio), error);
	return (error);
}


/*
 * NFS write call
 */
int
nfs_write_rpc(
	nfsnode_t np,
	uio_t uio,
	vfs_context_t ctx,
	int *iomodep,
	uint64_t *wverfp)
{
<<<<<<< HEAD
	return nfs_write_rpc2(np, uio, vfs_context_thread(ctx), vfs_context_ucred(ctx), iomodep, wverfp);
=======
	register u_long *tl;
	register caddr_t cp;
	register long t1, t2;
	caddr_t bpos, dpos, cp2;
	int error = 0, len, attrflag;
	struct mbuf *mreq, *mrep, *md, *mb, *mb2;
	int v3 = NFS_ISV3(vp);
	u_int64_t xid;

	nfsstats.rpccnt[NFSPROC_READLINK]++;
	nfsm_reqhead(vp, NFSPROC_READLINK, NFSX_FH(v3));
	nfsm_fhtom(vp, v3);
	nfsm_request(vp, NFSPROC_READLINK, uiop->uio_procp, cred, &xid);
	if (v3 && mrep)
		nfsm_postop_attr(vp, attrflag, &xid);
	if (!error) {
		nfsm_strsiz(len, NFS_MAXPATHLEN);
		if (len == NFS_MAXPATHLEN) {
			struct nfsnode *np = VTONFS(vp);
#if DIAGNOSTIC
                        if (!np)
                                panic("nfs_readlinkrpc: null np");
#endif  
                        if (np->n_size && np->n_size < NFS_MAXPATHLEN)
                                len = np->n_size;
                }
		nfsm_mtouio(uiop, len);
	}
	nfsm_reqdone;
	return (error);
>>>>>>> origin/10.1
}

int
nfs_write_rpc2(
	nfsnode_t np,
	uio_t uio,
	thread_t thd,
	kauth_cred_t cred,
	int *iomodep,
	uint64_t *wverfp)
{
	struct nfsmount *nmp;
<<<<<<< HEAD
	int error = 0, nfsvers;
	int wverfset, commit, committed;
	uint64_t wverf = 0, wverf2;
	size_t nmwsize, totalsize, tsiz, len, rlen;
	struct nfsreq rq, *req = &rq;
	uint32_t stategenid = 0, vrestart = 0, restart = 0;
	uio_t uio_save = NULL;
=======
	int error = 0, len, retlen, tsiz, eof, attrflag;
	int v3 = NFS_ISV3(vp);
	u_int64_t xid;
>>>>>>> origin/10.1

#if DIAGNOSTIC
	/* XXX limitation based on need to back up uio on short write */
	if (uio_iovcnt(uio) != 1)
		panic("nfs3_write_rpc: iovcnt > 1");
#endif
<<<<<<< HEAD
	FSDBG_TOP(537, np, uio_offset(uio), uio_resid(uio), *iomodep);
	nmp = NFSTONMP(np);
	if (nfs_mount_gone(nmp))
		return (ENXIO);
	nfsvers = nmp->nm_vers;
	nmwsize = nmp->nm_wsize;

	wverfset = 0;
	committed = NFS_WRITE_FILESYNC;

	totalsize = tsiz = uio_resid(uio);
	if ((nfsvers == NFS_VER2) && ((uint64_t)(uio_offset(uio) + tsiz) > 0xffffffffULL)) {
		FSDBG_BOT(537, np, uio_offset(uio), uio_resid(uio), EFBIG);
=======
	nmp = VFSTONFS(vp->v_mount);
	tsiz = uiop->uio_resid;
        if (((u_int64_t)uiop->uio_offset + (unsigned int)tsiz > 0xffffffff) &&
            !v3)
>>>>>>> origin/10.1
		return (EFBIG);
	}

	uio_save = uio_duplicate(uio);
	if (uio_save == NULL) {
		return (EIO);
	}

	while (tsiz > 0) {
		len = (tsiz > nmwsize) ? nmwsize : tsiz;
		FSDBG(537, np, uio_offset(uio), len, 0);
		if (np->n_flag & NREVOKE) {
			error = EIO;
			break;
		}
<<<<<<< HEAD
		if (nmp->nm_vers >= NFS_VER4)
			stategenid = nmp->nm_stategenid;
		error = nmp->nm_funcs->nf_write_rpc_async(np, uio, len, thd, cred, *iomodep, NULL, &req);
		if (!error)
			error = nmp->nm_funcs->nf_write_rpc_async_finish(np, req, &commit, &rlen, &wverf2);
		nmp = NFSTONMP(np);
		if (nfs_mount_gone(nmp))
			error = ENXIO;
		if ((nmp->nm_vers >= NFS_VER4) && nfs_mount_state_error_should_restart(error) &&
		    (++restart <= nfs_mount_state_max_restarts(nmp))) { /* guard against no progress */
			lck_mtx_lock(&nmp->nm_lock);
			if ((error != NFSERR_GRACE) && (stategenid == nmp->nm_stategenid)) {
				NP(np, "nfs_write_rpc: error %d, initiating recovery", error);
				nfs_need_recover(nmp, error);
			}
			lck_mtx_unlock(&nmp->nm_lock);
			if (np->n_flag & NREVOKE) {
				error = EIO;
			} else {
				if (error == NFSERR_GRACE)
					tsleep(&nmp->nm_state, (PZERO-1), "nfsgrace", 2*hz);
				if (!(error = nfs_mount_state_wait_for_recovery(nmp)))
					continue;
			}
		}
		if (error)
			break;
		if (nfsvers == NFS_VER2) {
			tsiz -= len;
			continue;
		}

		/* check for a short write */
		if (rlen < len) {
			/* Reset the uio to reflect the actual transfer */
			*uio = *uio_save;
			uio_update(uio, totalsize - (tsiz - rlen));
			len = rlen;
		}

		/* return lowest commit level returned */
		if (commit < committed)
			committed = commit;

		tsiz -= len;

		/* check write verifier */
		if (!wverfset) {
			wverf = wverf2;
			wverfset = 1;
		} else if (wverf != wverf2) {
			/* verifier changed, so we need to restart all the writes */
			if (++vrestart > 100) {
				/* give up after too many restarts */
				error = EIO;
				break;
			}
			*uio = *uio_save;	// Reset the uio back to the start
			committed = NFS_WRITE_FILESYNC;
			wverfset = 0;
			tsiz = totalsize;
		}
	}
	if (uio_save)
		uio_free(uio_save);
	if (wverfset && wverfp)
		*wverfp = wverf;
	*iomodep = committed;
	if (error)
		uio_setresid(uio, tsiz);
	FSDBG_BOT(537, np, committed, uio_resid(uio), error);
	return (error);
}

int
nfs3_write_rpc_async(
	nfsnode_t np,
	uio_t uio,
	size_t len,
	thread_t thd,
	kauth_cred_t cred,
	int iomode,
	struct nfsreq_cbinfo *cb,
	struct nfsreq **reqp)
{
	struct nfsmount *nmp;
	mount_t mp;
	int error = 0, nfsvers;
	struct nfsm_chain nmreq;

	nmp = NFSTONMP(np);
	if (nfs_mount_gone(nmp))
		return (ENXIO);
	nfsvers = nmp->nm_vers;

	/* for async mounts, don't bother sending sync write requests */
	if ((iomode != NFS_WRITE_UNSTABLE) && nfs_allow_async &&
	    ((mp = NFSTOMP(np))) && (vfs_flags(mp) & MNT_ASYNC))
		iomode = NFS_WRITE_UNSTABLE;

	nfsm_chain_null(&nmreq);
	nfsm_chain_build_alloc_init(error, &nmreq,
		NFSX_FH(nfsvers) + 5 * NFSX_UNSIGNED + nfsm_rndup(len));
	nfsm_chain_add_fh(error, &nmreq, nfsvers, np->n_fhp, np->n_fhsize);
	if (nfsvers == NFS_VER3) {
		nfsm_chain_add_64(error, &nmreq, uio_offset(uio));
		nfsm_chain_add_32(error, &nmreq, len);
		nfsm_chain_add_32(error, &nmreq, iomode);
	} else {
		nfsm_chain_add_32(error, &nmreq, 0);
		nfsm_chain_add_32(error, &nmreq, uio_offset(uio));
		nfsm_chain_add_32(error, &nmreq, 0);
=======
		nfsm_request(vp, NFSPROC_READ, uiop->uio_procp, cred, &xid);
		if (v3) {
			if (mrep) {
				nfsm_postop_attr(vp, attrflag, &xid);
			}
			if (error) {
				m_freem(mrep);
				goto nfsmout;
			}
			nfsm_dissect(tl, u_long *, 2 * NFSX_UNSIGNED);
			eof = fxdr_unsigned(int, *(tl + 1));
<<<<<<< HEAD
		} else
			nfsm_loadattr(vp, (struct vattr *)0, &xid);
		nfsm_strsiz(retlen, nmp->nm_rsize);
		nfsm_mtouio(uiop, retlen);
		m_freem(mrep);
=======
		} else {
			if (mrep) {
				nfsm_loadattr(vp, (struct vattr *)0, &xid);
			}
		}
		if (mrep) {
			nfsm_strsiz(retlen, nmrsize);
			nfsm_mtouio(uiop, retlen);
			m_freem(mrep);
		} else {
			retlen = 0;
		}
>>>>>>> origin/10.3
		tsiz -= retlen;
		if (v3) {
			if (eof || retlen == 0)
				tsiz = 0;
		} else if (retlen < len)
			tsiz = 0;
>>>>>>> origin/10.1
	}
	nfsm_chain_add_32(error, &nmreq, len);
	nfsmout_if(error);
	error = nfsm_chain_add_uio(&nmreq, uio, len);
	nfsm_chain_build_done(error, &nmreq);
	nfsmout_if(error);
	error = nfs_request_async(np, NULL, &nmreq, NFSPROC_WRITE, thd, cred, NULL, 0, cb, reqp);
nfsmout:
	nfsm_chain_cleanup(&nmreq);
	return (error);
}

int
nfs3_write_rpc_async_finish(
	nfsnode_t np,
	struct nfsreq *req,
	int *iomodep,
	size_t *rlenp,
	uint64_t *wverfp)
{
<<<<<<< HEAD
	struct nfsmount *nmp;
	int error = 0, lockerror = ENOENT, nfsvers, status;
	int updatemtime = 0, wccpostattr = 0, rlen, committed = NFS_WRITE_FILESYNC;
	u_int64_t xid, wverf;
	mount_t mp;
	struct nfsm_chain nmrep;

	nmp = NFSTONMP(np);
	if (nfs_mount_gone(nmp)) {
		nfs_request_async_cancel(req);
		return (ENXIO);
	}
	nfsvers = nmp->nm_vers;

	nfsm_chain_null(&nmrep);

	error = nfs_request_async_finish(req, &nmrep, &xid, &status);
	if (error == EINPROGRESS) /* async request restarted */
		return (error);
	nmp = NFSTONMP(np);
	if (nfs_mount_gone(nmp))
		error = ENXIO;
	if (!error && (lockerror = nfs_node_lock(np)))
		error = lockerror;
	if (nfsvers == NFS_VER3) {
		struct timespec premtime = { 0, 0 };
		nfsm_chain_get_wcc_data(error, &nmrep, np, &premtime, &wccpostattr, &xid);
		if (nfstimespeccmp(&np->n_mtime, &premtime, ==))
			updatemtime = 1;
		if (!error)
			error = status;
		nfsm_chain_get_32(error, &nmrep, rlen);
		nfsmout_if(error);
		*rlenp = rlen;
		if (rlen <= 0)
			error = NFSERR_IO;
		nfsm_chain_get_32(error, &nmrep, committed);
		nfsm_chain_get_64(error, &nmrep, wverf);
		nfsmout_if(error);
		if (wverfp)
			*wverfp = wverf;
		lck_mtx_lock(&nmp->nm_lock);
		if (!(nmp->nm_state & NFSSTA_HASWRITEVERF)) {
			nmp->nm_verf = wverf;
			nmp->nm_state |= NFSSTA_HASWRITEVERF;
		} else if (nmp->nm_verf != wverf) {
			nmp->nm_verf = wverf;
		}
		lck_mtx_unlock(&nmp->nm_lock);
	} else {
		if (!error)
			error = status;
		nfsm_chain_loadattr(error, &nmrep, np, nfsvers, &xid);
		nfsmout_if(error);
=======
	register u_long *tl;
	register caddr_t cp;
	register int t1, t2, backup;
	caddr_t bpos, dpos, cp2;
	struct mbuf *mreq, *mrep, *md, *mb, *mb2;
<<<<<<< HEAD
	struct nfsmount *nmp = VFSTONFS(vp->v_mount);
	int error = 0, len, tsiz, wccflag = NFSV3_WCCRATTR, rlen, commit;
	int v3 = NFS_ISV3(vp), committed = NFSV3WRITE_FILESYNC;
=======
	struct nfsmount *nmp;
	int error = 0, len, tsiz, updatemtime = 0, wccpostattr = 0, rlen, commit;
	int v3, committed = NFSV3WRITE_FILESYNC;
>>>>>>> origin/10.3
	u_int64_t xid;

#if DIAGNOSTIC
	if (uiop->uio_iovcnt != 1)
		panic("nfs_writerpc: iovcnt > 1");
#endif
	*must_commit = 0;
	tsiz = uiop->uio_resid;
        if (((u_int64_t)uiop->uio_offset + (unsigned int)tsiz > 0xffffffff) && !v3)
		return (EFBIG);
	while (tsiz > 0) {
		nfsstats.rpccnt[NFSPROC_WRITE]++;
		len = (tsiz > nmp->nm_wsize) ? nmp->nm_wsize : tsiz;
		nfsm_reqhead(vp, NFSPROC_WRITE,
			NFSX_FH(v3) + 5 * NFSX_UNSIGNED + nfsm_rndup(len));
		nfsm_fhtom(vp, v3);
		if (v3) {
			nfsm_build(tl, u_long *, 5 * NFSX_UNSIGNED);
			txdr_hyper(&uiop->uio_offset, tl);
			tl += 2;
			*tl++ = txdr_unsigned(len);
			*tl++ = txdr_unsigned(*iomode);
		} else {
			nfsm_build(tl, u_long *, 4 * NFSX_UNSIGNED);
			*++tl = txdr_unsigned(uiop->uio_offset);
			tl += 2;
		}
		*tl = txdr_unsigned(len);
		nfsm_uiotom(uiop, len);
		nfsm_request(vp, NFSPROC_WRITE, uiop->uio_procp, cred, &xid);
		if (v3) {
			if (mrep) {
				time_t premtime;
				nfsm_wcc_data(vp, premtime, wccpostattr, &xid);
				if (VTONFS(vp)->n_mtime == premtime)
					updatemtime = 1;
			}
			if (!error) {
				nfsm_dissect(tl, u_long *, 2 * NFSX_UNSIGNED +
					NFSX_V3WRITEVERF);
				rlen = fxdr_unsigned(int, *tl++);
				if (rlen <= 0) {
					error = NFSERR_IO;
					break;
				} else if (rlen < len) {
					backup = len - rlen;
					uiop->uio_iov->iov_base -= backup;
					uiop->uio_iov->iov_len += backup;
					uiop->uio_offset -= backup;
					uiop->uio_resid += backup;
					len = rlen;
				}
				commit = fxdr_unsigned(int, *tl++);

				/*
				 * Return the lowest committment level
				 * obtained by any of the RPCs.
				 */
				if (committed == NFSV3WRITE_FILESYNC)
					committed = commit;
				else if (committed == NFSV3WRITE_DATASYNC &&
					commit == NFSV3WRITE_UNSTABLE)
					committed = commit;
				if ((nmp->nm_flag & NFSMNT_HASWRITEVERF) == 0) {
				    bcopy((caddr_t)tl, (caddr_t)nmp->nm_verf,
					NFSX_V3WRITEVERF);
				    nmp->nm_flag |= NFSMNT_HASWRITEVERF;
				} else if (bcmp((caddr_t)tl,
				    (caddr_t)nmp->nm_verf, NFSX_V3WRITEVERF)) {
				    *must_commit = 1;
				    bcopy((caddr_t)tl, (caddr_t)nmp->nm_verf,
					NFSX_V3WRITEVERF);
				}
			}
		} else {
			if (mrep) {
				nfsm_loadattr(vp, (struct vattr *)0, &xid);
			}
		}

<<<<<<< HEAD
		if (wccflag && vp->v_type != VBAD) /* EINVAL set on VBAD node */
		    VTONFS(vp)->n_mtime = VTONFS(vp)->n_vattr.va_mtime.tv_sec;
=======
		if (updatemtime)
			VTONFS(vp)->n_mtime = VTONFS(vp)->n_vattr.va_mtime.tv_sec;
>>>>>>> origin/10.3
		m_freem(mrep);
		/*
		 * we seem to have a case where we end up looping on shutdown
		 * and taking down nfs servers.  For V3, error cases, there is
		 * no way to terminate loop, if the len was 0, meaning,
		 * nmp->nm_wsize was trashed. FreeBSD has this fix in it.
		 * Let's try it.
		 */
		if (error)
			break;
		tsiz -= len;
>>>>>>> origin/10.1
	}
	if (updatemtime)
		NFS_CHANGED_UPDATE(nfsvers, np, &np->n_vattr);
nfsmout:
<<<<<<< HEAD
	if (!lockerror)
		nfs_node_unlock(np);
	nfsm_chain_cleanup(&nmrep);
	if ((committed != NFS_WRITE_FILESYNC) && nfs_allow_async &&
	    ((mp = NFSTOMP(np))) && (vfs_flags(mp) & MNT_ASYNC))
		committed = NFS_WRITE_FILESYNC;
	*iomodep = committed;
=======
        /* EKN
         * does it make sense to even say it was committed if we had an error?
         * okay well just don't on bad vnodes then.  EINVAL will be
         * returned on bad vnodes
         */
        if (vp->v_type != VBAD && (vp->v_mount->mnt_flag & MNT_ASYNC))
		committed = NFSV3WRITE_FILESYNC;
        *iomode = committed;
	if (error)
		uiop->uio_resid = tsiz;
>>>>>>> origin/10.1
	return (error);
}

/*
 * NFS mknod vnode op
 *
 * For NFS v2 this is a kludge. Use a create RPC but with the IFMT bits of the
 * mode set to specify the file type and the size field for rdev.
 */
int
nfs3_vnop_mknod(
	struct vnop_mknod_args /* {
		struct vnodeop_desc *a_desc;
		vnode_t a_dvp;
		vnode_t *a_vpp;
		struct componentname *a_cnp;
		struct vnode_attr *a_vap;
		vfs_context_t a_context;
	} */ *ap)
{
<<<<<<< HEAD
	vnode_t dvp = ap->a_dvp;
	vnode_t *vpp = ap->a_vpp;
	struct componentname *cnp = ap->a_cnp;
	struct vnode_attr *vap = ap->a_vap;
	vfs_context_t ctx = ap->a_context;
	vnode_t newvp = NULL;
	nfsnode_t np = NULL;
	struct nfsmount *nmp;
	nfsnode_t dnp = VTONFS(dvp);
	struct nfs_vattr nvattr;
	fhandle_t fh;
	int error = 0, lockerror = ENOENT, busyerror = ENOENT, status, wccpostattr = 0;
	struct timespec premtime = { 0, 0 };
	u_int32_t rdev;
	u_int64_t xid = 0, dxid;
	int nfsvers, gotuid, gotgid;
	struct nfsm_chain nmreq, nmrep;
	struct nfsreq rq, *req = &rq;

	nmp = VTONMP(dvp);
	if (nfs_mount_gone(nmp))
		return (ENXIO);
	nfsvers = nmp->nm_vers;

	if (!VATTR_IS_ACTIVE(vap, va_type))
		return (EINVAL);
	if (vap->va_type == VCHR || vap->va_type == VBLK) {
		if (!VATTR_IS_ACTIVE(vap, va_rdev))
			return (EINVAL);
		rdev = vap->va_rdev;
	} else if (vap->va_type == VFIFO || vap->va_type == VSOCK)
=======
	register struct nfsv2_sattr *sp;
	register struct nfsv3_sattr *sp3;
	register u_long *tl;
	register caddr_t cp;
	register long t1, t2;
	struct vnode *newvp = (struct vnode *)0;
	struct nfsnode *np = (struct nfsnode *)0;
	struct vattr vattr;
	char *cp2;
	caddr_t bpos, dpos;
	int error = 0, wccpostattr = 0, gotvp = 0;
	time_t premtime = 0;
	struct mbuf *mreq, *mrep, *md, *mb, *mb2;
	u_long rdev;
	u_int64_t xid;
	int v3 = NFS_ISV3(dvp);

	if (vap->va_type == VCHR || vap->va_type == VBLK)
		rdev = txdr_unsigned(vap->va_rdev);
	else if (vap->va_type == VFIFO || vap->va_type == VSOCK)
>>>>>>> origin/10.1
		rdev = 0xffffffff;
	else {
		return (ENOTSUP);
	}
	if ((nfsvers == NFS_VER2) && (cnp->cn_namelen > NFS_MAXNAMLEN))
		return (ENAMETOOLONG);

	nfs_avoid_needless_id_setting_on_create(dnp, vap, ctx);

	VATTR_SET_SUPPORTED(vap, va_mode);
	VATTR_SET_SUPPORTED(vap, va_uid);
	VATTR_SET_SUPPORTED(vap, va_gid);
	VATTR_SET_SUPPORTED(vap, va_data_size);
	VATTR_SET_SUPPORTED(vap, va_access_time);
	VATTR_SET_SUPPORTED(vap, va_modify_time);
	gotuid = VATTR_IS_ACTIVE(vap, va_uid);
	gotgid = VATTR_IS_ACTIVE(vap, va_gid);

	nfsm_chain_null(&nmreq);
	nfsm_chain_null(&nmrep);

	nfsm_chain_build_alloc_init(error, &nmreq,
		NFSX_FH(nfsvers) + 4 * NFSX_UNSIGNED +
		nfsm_rndup(cnp->cn_namelen) + NFSX_SATTR(nfsvers));
	nfsm_chain_add_fh(error, &nmreq, nfsvers, dnp->n_fhp, dnp->n_fhsize);
	nfsm_chain_add_name(error, &nmreq, cnp->cn_nameptr, cnp->cn_namelen, nmp);
	if (nfsvers == NFS_VER3) {
		nfsm_chain_add_32(error, &nmreq, vtonfs_type(vap->va_type, nfsvers));
		nfsm_chain_add_v3sattr(error, &nmreq, vap);
		if (vap->va_type == VCHR || vap->va_type == VBLK) {
			nfsm_chain_add_32(error, &nmreq, major(vap->va_rdev));
			nfsm_chain_add_32(error, &nmreq, minor(vap->va_rdev));
		}
	} else {
<<<<<<< HEAD
		nfsm_chain_add_v2sattr(error, &nmreq, vap, rdev);
	}
	nfsm_chain_build_done(error, &nmreq);
	if (!error)
		error = busyerror = nfs_node_set_busy(dnp, vfs_context_thread(ctx));
	nfsmout_if(error);

	error = nfs_request_async(dnp, NULL, &nmreq, NFSPROC_MKNOD,
			vfs_context_thread(ctx), vfs_context_ucred(ctx), NULL, 0, NULL, &req);
	if (!error)
		error = nfs_request_async_finish(req, &nmrep, &xid, &status);

	if ((lockerror = nfs_node_lock(dnp)))
		error = lockerror;
	/* XXX no EEXIST kludge here? */
	dxid = xid;
	if (!error && !status) {
		if (dnp->n_flag & NNEGNCENTRIES) {
			dnp->n_flag &= ~NNEGNCENTRIES;
			cache_purge_negatives(dvp);
=======
		nfsm_build(sp, struct nfsv2_sattr *, NFSX_V2SATTR);
		sp->sa_mode = vtonfsv2_mode(vap->va_type, vap->va_mode);
		sp->sa_uid = txdr_unsigned(cnp->cn_cred->cr_uid);
		sp->sa_gid = txdr_unsigned(vattr.va_gid);
		sp->sa_size = rdev;
		txdr_nfsv2time(&vap->va_atime, &sp->sa_atime);
		txdr_nfsv2time(&vap->va_mtime, &sp->sa_mtime);
	}
	nfsm_request(dvp, NFSPROC_MKNOD, cnp->cn_proc, cnp->cn_cred, &xid);
	if (!error) {
		nfsm_mtofh(dvp, newvp, v3, gotvp, &xid);
		if (!gotvp) {
			if (newvp) {
				vput(newvp);
				newvp = (struct vnode *)0;
			}
			error = nfs_lookitup(dvp, cnp->cn_nameptr,
			    cnp->cn_namelen, cnp->cn_cred, cnp->cn_proc, &np);
			if (!error)
				newvp = NFSTOV(np);
>>>>>>> origin/10.1
		}
		error = nfsm_chain_get_fh_attr(&nmrep, dnp, ctx, nfsvers, &xid, &fh, &nvattr);
	}
	if (nfsvers == NFS_VER3)
		nfsm_chain_get_wcc_data(error, &nmrep, dnp, &premtime, &wccpostattr, &dxid);
	if (!error)
		error = status;
nfsmout:
	nfsm_chain_cleanup(&nmreq);
	nfsm_chain_cleanup(&nmrep);

	if (!lockerror) {
		dnp->n_flag |= NMODIFIED;
		/* if directory hadn't changed, update namecache mtime */
		if (nfstimespeccmp(&dnp->n_ncmtime, &premtime, ==))
			NFS_CHANGED_UPDATE_NC(nfsvers, dnp, &dnp->n_vattr);
		nfs_node_unlock(dnp);
		/* nfs_getattr() will check changed and purge caches */
		nfs_getattr(dnp, NULL, ctx, wccpostattr ? NGA_CACHED : NGA_UNCACHED);
	}

	if (!error && fh.fh_len)
		error = nfs_nget(NFSTOMP(dnp), dnp, cnp, fh.fh_data, fh.fh_len, &nvattr, &xid, rq.r_auth, NG_MAKEENTRY, &np);
	if (!error && !np)
		error = nfs_lookitup(dnp, cnp->cn_nameptr, cnp->cn_namelen, ctx, &np);
	if (!error && np)
		newvp = NFSTOV(np);
	if (!busyerror)
		nfs_node_clear_busy(dnp);

	if (!error && (gotuid || gotgid) &&
	    (!newvp || nfs_getattrcache(np, &nvattr, 0) ||
	     (gotuid && (nvattr.nva_uid != vap->va_uid)) ||
	     (gotgid && (nvattr.nva_gid != vap->va_gid)))) {
		/* clear ID bits if server didn't use them (or we can't tell) */
		VATTR_CLEAR_SUPPORTED(vap, va_uid);
		VATTR_CLEAR_SUPPORTED(vap, va_gid);
	}
<<<<<<< HEAD
<<<<<<< HEAD
=======
	if (v3)
		nfsm_wcc_data(dvp, wccflag, &xid);
=======
	if (v3 && mrep)
		nfsm_wcc_data(dvp, premtime, wccpostattr, &xid);
>>>>>>> origin/10.3
	nfsm_reqdone;
>>>>>>> origin/10.1
	if (error) {
		if (newvp) {
			nfs_node_unlock(np);
			vnode_put(newvp);
		}
	} else {
		*vpp = newvp;
		nfs_node_unlock(np);
	}
<<<<<<< HEAD
=======
	FREE_ZONE(cnp->cn_pnbuf, cnp->cn_pnlen, M_NAMEI);
	if (dvp->v_type != VBAD) { /* EINVAL set on VBAD vnode */
		VTONFS(dvp)->n_flag |= NMODIFIED;
		if (!wccflag)
			VTONFS(dvp)->n_attrstamp = 0;
	}
<<<<<<< HEAD
=======
	VTONFS(dvp)->n_flag |= NMODIFIED;
	/* if directory hadn't changed, update namecache mtime */
	if (VTONFS(dvp)->n_ncmtime == premtime)
		VTONFS(dvp)->n_ncmtime = VTONFS(dvp)->n_vattr.va_mtime.tv_sec;
	if (!wccpostattr)
		VTONFS(dvp)->n_xid = 0;
>>>>>>> origin/10.3
	vput(dvp);
>>>>>>> origin/10.1
	return (error);
}

static uint32_t create_verf;
/*
 * NFS file create call
 */
<<<<<<< HEAD
int
nfs3_vnop_create(
	struct vnop_create_args /* {
		struct vnodeop_desc *a_desc;
		vnode_t a_dvp;
		vnode_t *a_vpp;
=======
/* ARGSUSED */
static int
nfs_mknod(ap)
	struct vop_mknod_args /* {
		struct vnode *a_dvp;
		struct vnode **a_vpp;
		struct componentname *a_cnp;
		struct vattr *a_vap;
	} */ *ap;
{
	struct vnode *newvp;
	int error;

	error = nfs_mknodrpc(ap->a_dvp, &newvp, ap->a_cnp, ap->a_vap);
	if (!error && newvp)
		vput(newvp);
	*ap->a_vpp = 0;
	return (error);
}

static u_long create_verf;
/*
 * nfs file create call
 */
static int
nfs_create(ap)
	struct vop_create_args /* {
		struct vnode *a_dvp;
		struct vnode **a_vpp;
>>>>>>> origin/10.1
		struct componentname *a_cnp;
		struct vnode_attr *a_vap;
		vfs_context_t a_context;
	} */ *ap)
{
<<<<<<< HEAD
	vfs_context_t ctx = ap->a_context;
	vnode_t dvp = ap->a_dvp;
	struct vnode_attr *vap = ap->a_vap;
	struct componentname *cnp = ap->a_cnp;
	struct nfs_vattr nvattr;
	fhandle_t fh;
	nfsnode_t np = NULL;
	struct nfsmount *nmp;
	nfsnode_t dnp = VTONFS(dvp);
	vnode_t newvp = NULL;
	int error = 0, lockerror = ENOENT, busyerror = ENOENT, status, wccpostattr = 0, fmode = 0;
	struct timespec premtime = { 0, 0 };
	int nfsvers, gotuid, gotgid;
	u_int64_t xid, dxid;
	uint32_t val;
	struct nfsm_chain nmreq, nmrep;
	struct nfsreq rq, *req = &rq;
	struct nfs_dulookup dul;

	nmp = VTONMP(dvp);
	if (nfs_mount_gone(nmp))
		return (ENXIO);
	nfsvers = nmp->nm_vers;

	if ((nfsvers == NFS_VER2) && (cnp->cn_namelen > NFS_MAXNAMLEN))
		return (ENAMETOOLONG);

	nfs_avoid_needless_id_setting_on_create(dnp, vap, ctx);

	VATTR_SET_SUPPORTED(vap, va_mode);
	VATTR_SET_SUPPORTED(vap, va_uid);
	VATTR_SET_SUPPORTED(vap, va_gid);
	VATTR_SET_SUPPORTED(vap, va_data_size);
	VATTR_SET_SUPPORTED(vap, va_access_time);
	VATTR_SET_SUPPORTED(vap, va_modify_time);
	gotuid = VATTR_IS_ACTIVE(vap, va_uid);
	gotgid = VATTR_IS_ACTIVE(vap, va_gid);

	if (vap->va_vaflags & VA_EXCLUSIVE) {
=======
	register struct vnode *dvp = ap->a_dvp;
	register struct vattr *vap = ap->a_vap;
	register struct componentname *cnp = ap->a_cnp;
	register struct nfsv2_sattr *sp;
	register struct nfsv3_sattr *sp3;
	register u_long *tl;
	register caddr_t cp;
	register long t1, t2;
	struct nfsnode *np = (struct nfsnode *)0;
	struct vnode *newvp = (struct vnode *)0;
	caddr_t bpos, dpos, cp2;
	int error = 0, wccpostattr = 0, gotvp = 0, fmode = 0;
	time_t premtime = 0;
	struct mbuf *mreq, *mrep, *md, *mb, *mb2;
	struct vattr vattr;
	int v3 = NFS_ISV3(dvp);
	u_int64_t xid;

	/*
	 * Oops, not for me..
	 */
	if (vap->va_type == VSOCK)
		return (nfs_mknodrpc(dvp, ap->a_vpp, cnp, vap));

	if ((error = VOP_GETATTR(dvp, &vattr, cnp->cn_cred, cnp->cn_proc))) {
		VOP_ABORTOP(dvp, cnp);
		vput(dvp);
		return (error);
	}
	if (vap->va_vaflags & VA_EXCLUSIVE)
>>>>>>> origin/10.1
		fmode |= O_EXCL;
		if (!VATTR_IS_ACTIVE(vap, va_access_time) || !VATTR_IS_ACTIVE(vap, va_modify_time))
			vap->va_vaflags |= VA_UTIMES_NULL;
	}

again:
	error = busyerror = nfs_node_set_busy(dnp, vfs_context_thread(ctx));
	nfs_dulookup_init(&dul, dnp, cnp->cn_nameptr, cnp->cn_namelen, ctx);

	nfsm_chain_null(&nmreq);
	nfsm_chain_null(&nmrep);

	nfsm_chain_build_alloc_init(error, &nmreq,
		NFSX_FH(nfsvers) + 2 * NFSX_UNSIGNED +
		nfsm_rndup(cnp->cn_namelen) + NFSX_SATTR(nfsvers));
	nfsm_chain_add_fh(error, &nmreq, nfsvers, dnp->n_fhp, dnp->n_fhsize);
	nfsm_chain_add_name(error, &nmreq, cnp->cn_nameptr, cnp->cn_namelen, nmp);
	if (nfsvers == NFS_VER3) {
		if (fmode & O_EXCL) {
			nfsm_chain_add_32(error, &nmreq, NFS_CREATE_EXCLUSIVE);
			lck_rw_lock_shared(in_ifaddr_rwlock);
			if (!TAILQ_EMPTY(&in_ifaddrhead))
				val = IA_SIN(in_ifaddrhead.tqh_first)->sin_addr.s_addr;
			else
				val = create_verf;
			lck_rw_done(in_ifaddr_rwlock);
			nfsm_chain_add_32(error, &nmreq, val);
			++create_verf;
			nfsm_chain_add_32(error, &nmreq, create_verf);
		} else {
			nfsm_chain_add_32(error, &nmreq, NFS_CREATE_UNCHECKED);
			nfsm_chain_add_v3sattr(error, &nmreq, vap);
		}
	} else {
<<<<<<< HEAD
		nfsm_chain_add_v2sattr(error, &nmreq, vap, 0);
	}
	nfsm_chain_build_done(error, &nmreq);
	nfsmout_if(error);

	error = nfs_request_async(dnp, NULL, &nmreq, NFSPROC_CREATE,
			vfs_context_thread(ctx), vfs_context_ucred(ctx), NULL, 0, NULL, &req);
	if (!error) {
		nfs_dulookup_start(&dul, dnp, ctx);
		error = nfs_request_async_finish(req, &nmrep, &xid, &status);
	}

	if ((lockerror = nfs_node_lock(dnp)))
		error = lockerror;
	dxid = xid;
	if (!error && !status) {
		if (dnp->n_flag & NNEGNCENTRIES) {
			dnp->n_flag &= ~NNEGNCENTRIES;
			cache_purge_negatives(dvp);
=======
		nfsm_build(sp, struct nfsv2_sattr *, NFSX_V2SATTR);
		sp->sa_mode = vtonfsv2_mode(vap->va_type, vap->va_mode);
		sp->sa_uid = txdr_unsigned(cnp->cn_cred->cr_uid);
		sp->sa_gid = txdr_unsigned(vattr.va_gid);
		sp->sa_size = 0;
		txdr_nfsv2time(&vap->va_atime, &sp->sa_atime);
		txdr_nfsv2time(&vap->va_mtime, &sp->sa_mtime);
	}
	nfsm_request(dvp, NFSPROC_CREATE, cnp->cn_proc, cnp->cn_cred, &xid);
	if (!error) {
		nfsm_mtofh(dvp, newvp, v3, gotvp, &xid);
		if (!gotvp) {
			if (newvp) {
				vput(newvp);
				newvp = (struct vnode *)0;
			}
			error = nfs_lookitup(dvp, cnp->cn_nameptr,
			    cnp->cn_namelen, cnp->cn_cred, cnp->cn_proc, &np);
			if (!error)
				newvp = NFSTOV(np);
>>>>>>> origin/10.1
		}
		error = nfsm_chain_get_fh_attr(&nmrep, dnp, ctx, nfsvers, &xid, &fh, &nvattr);
	}
<<<<<<< HEAD
	if (nfsvers == NFS_VER3)
		nfsm_chain_get_wcc_data(error, &nmrep, dnp, &premtime, &wccpostattr, &dxid);
	if (!error)
		error = status;
nfsmout:
	nfsm_chain_cleanup(&nmreq);
	nfsm_chain_cleanup(&nmrep);

	if (!lockerror) {
		dnp->n_flag |= NMODIFIED;
		/* if directory hadn't changed, update namecache mtime */
		if (nfstimespeccmp(&dnp->n_ncmtime, &premtime, ==))
			NFS_CHANGED_UPDATE_NC(nfsvers, dnp, &dnp->n_vattr);
		nfs_node_unlock(dnp);
		/* nfs_getattr() will check changed and purge caches */
		nfs_getattr(dnp, NULL, ctx, wccpostattr ? NGA_CACHED : NGA_UNCACHED);
	}
<<<<<<< HEAD

	if (!error && fh.fh_len)
		error = nfs_nget(NFSTOMP(dnp), dnp, cnp, fh.fh_data, fh.fh_len, &nvattr, &xid, rq.r_auth, NG_MAKEENTRY, &np);
	if (!error && !np)
		error = nfs_lookitup(dnp, cnp->cn_nameptr, cnp->cn_namelen, ctx, &np);
	if (!error && np)
		newvp = NFSTOV(np);

	nfs_dulookup_finish(&dul, dnp, ctx);
	if (!busyerror)
		nfs_node_clear_busy(dnp);

=======
	if (v3)
		nfsm_wcc_data(dvp, wccflag, &xid);
=======
	if (v3 && mrep)
		nfsm_wcc_data(dvp, premtime, wccpostattr, &xid);
>>>>>>> origin/10.3
	nfsm_reqdone;
>>>>>>> origin/10.1
	if (error) {
		if ((nfsvers == NFS_VER3) && (fmode & O_EXCL) && (error == NFSERR_NOTSUPP)) {
			fmode &= ~O_EXCL;
			goto again;
		}
<<<<<<< HEAD
		if (newvp) {
			nfs_node_unlock(np);
			vnode_put(newvp);
		}
	} else if ((nfsvers == NFS_VER3) && (fmode & O_EXCL)) {
		nfs_node_unlock(np);
		error = nfs3_setattr_rpc(np, vap, ctx);
		if (error && (gotuid || gotgid)) {
			/* it's possible the server didn't like our attempt to set IDs. */
			/* so, let's try it again without those */
			VATTR_CLEAR_ACTIVE(vap, va_uid);
			VATTR_CLEAR_ACTIVE(vap, va_gid);
			error = nfs3_setattr_rpc(np, vap, ctx);
		}
		if (error)
			vnode_put(newvp);
		else
			nfs_node_lock_force(np);
	}
	if (!error)
=======
		if (newvp)
			vput(newvp);
	} else if (v3 && (fmode & O_EXCL)) {
		error = nfs_setattrrpc(newvp, vap, cnp->cn_cred, cnp->cn_proc);
		if (error)
			vput(newvp);
	}
	if (!error) {
		if (cnp->cn_flags & MAKEENTRY)
			cache_enter(dvp, newvp, cnp);
>>>>>>> origin/10.3
		*ap->a_vpp = newvp;
	if (!error && (gotuid || gotgid) &&
	    (!newvp || nfs_getattrcache(np, &nvattr, 0) ||
	     (gotuid && (nvattr.nva_uid != vap->va_uid)) ||
	     (gotgid && (nvattr.nva_gid != vap->va_gid)))) {
		/* clear ID bits if server didn't use them (or we can't tell) */
		VATTR_CLEAR_SUPPORTED(vap, va_uid);
		VATTR_CLEAR_SUPPORTED(vap, va_gid);
	}
<<<<<<< HEAD
	if (!error)
		nfs_node_unlock(np);
=======
	VTONFS(dvp)->n_flag |= NMODIFIED;
	/* if directory hadn't changed, update namecache mtime */
	if (VTONFS(dvp)->n_ncmtime == premtime)
		VTONFS(dvp)->n_ncmtime = VTONFS(dvp)->n_vattr.va_mtime.tv_sec;
	if (!wccpostattr)
		VTONFS(dvp)->n_xid = 0;
	vput(dvp);
	NFS_FREE_PNBUF(cnp);
>>>>>>> origin/10.3
	return (error);
}

/*
 * NFS file remove call
 * To try and make NFS semantics closer to UFS semantics, a file that has
 * other processes using the vnode is renamed instead of removed and then
 * removed later on the last close.
 * - If vnode_isinuse()
 *	  If a rename is not already in the works
 *	     call nfs_sillyrename() to set it up
 *     else
 *	  do the remove RPC
 */
int
nfs_vnop_remove(
	struct vnop_remove_args /* {
		struct vnodeop_desc *a_desc;
		vnode_t a_dvp;
		vnode_t a_vp;
		struct componentname *a_cnp;
		int a_flags;
		vfs_context_t a_context;
	} */ *ap)
{
	vfs_context_t ctx = ap->a_context;
	vnode_t vp = ap->a_vp;
	vnode_t dvp = ap->a_dvp;
	struct componentname *cnp = ap->a_cnp;
	nfsnode_t dnp = VTONFS(dvp);
	nfsnode_t np = VTONFS(vp);
	int error = 0, nfsvers, namedattrs, inuse, gotattr = 0, flushed = 0, setsize = 0;
	struct nfs_vattr nvattr;
	struct nfsmount *nmp;
	struct nfs_dulookup dul;

	/* XXX prevent removing a sillyrenamed file? */

	nmp = NFSTONMP(dnp);
	if (nfs_mount_gone(nmp))
		return (ENXIO);
	nfsvers = nmp->nm_vers;
	namedattrs = (nmp->nm_fsattr.nfsa_flags & NFS_FSFLAG_NAMED_ATTR);

again_relock:
	error = nfs_node_set_busy2(dnp, np, vfs_context_thread(ctx));
	if (error)
		return (error);

	/* lock the node while we remove the file */
	lck_mtx_lock(nfs_node_hash_mutex);
	while (np->n_hflag & NHLOCKED) {
		np->n_hflag |= NHLOCKWANT;
		msleep(np, nfs_node_hash_mutex, PINOD, "nfs_remove", NULL);
	}
	np->n_hflag |= NHLOCKED;
	lck_mtx_unlock(nfs_node_hash_mutex);

	if (!namedattrs)
		nfs_dulookup_init(&dul, dnp, cnp->cn_nameptr, cnp->cn_namelen, ctx);
again:
	inuse = vnode_isinuse(vp, 0);
	if ((ap->a_flags & VNODE_REMOVE_NODELETEBUSY) && inuse) {
		/* Caller requested Carbon delete semantics, but file is busy */
		error = EBUSY;
		goto out;
	}
	if (inuse && !gotattr) {
		if (nfs_getattr(np, &nvattr, ctx, NGA_CACHED))
			nvattr.nva_nlink = 1;
		gotattr = 1;
		goto again;
	}
	if (!inuse || (np->n_sillyrename && (nvattr.nva_nlink > 1))) {

		if (!inuse && !flushed) { /* flush all the buffers first */
			/* unlock the node */
			lck_mtx_lock(nfs_node_hash_mutex);
			np->n_hflag &= ~NHLOCKED;
			if (np->n_hflag & NHLOCKWANT) {
				np->n_hflag &= ~NHLOCKWANT;
				wakeup(np);
			}
			lck_mtx_unlock(nfs_node_hash_mutex);
			nfs_node_clear_busy2(dnp, np);
			error = nfs_vinvalbuf(vp, V_SAVE, ctx, 1);
			FSDBG(260, np, np->n_size, np->n_vattr.nva_size, 0xf00d0011);
			flushed = 1;
			if (error == EINTR) {
				nfs_node_lock_force(np);
				NATTRINVALIDATE(np);
				nfs_node_unlock(np);
				return (error);
			}
			if (!namedattrs)
				nfs_dulookup_finish(&dul, dnp, ctx);
			goto again_relock;
		}

		if ((nmp->nm_vers >= NFS_VER4) && (np->n_openflags & N_DELEG_MASK))
			nfs4_delegation_return(np, 0, vfs_context_thread(ctx), vfs_context_ucred(ctx));

		/*
		 * Purge the name cache so that the chance of a lookup for
		 * the name succeeding while the remove is in progress is
		 * minimized.
<<<<<<< HEAD
=======
		 */
		cache_purge(vp);
		/*
		 * throw away biocache buffers, mainly to avoid
		 * unnecessary delayed writes later.
>>>>>>> origin/10.3
		 */
<<<<<<< HEAD
		nfs_name_cache_purge(dnp, np, cnp, ctx);

		if (!namedattrs)
			nfs_dulookup_start(&dul, dnp, ctx);

=======
		error = nfs_vinvalbuf(vp, 0, cnp->cn_cred, cnp->cn_proc, 1);
		np->n_size = 0;
		ubc_setsize(vp, (off_t)0); /* XXX check error */
>>>>>>> origin/10.1
		/* Do the rpc */
		error = nmp->nm_funcs->nf_remove_rpc(dnp, cnp->cn_nameptr, cnp->cn_namelen,
				vfs_context_thread(ctx), vfs_context_ucred(ctx));

		/*
		 * Kludge City: If the first reply to the remove rpc is lost..
		 *   the reply to the retransmitted request will be ENOENT
		 *   since the file was in fact removed
		 *   Therefore, we cheat and return success.
		 */
		if (error == ENOENT)
			error = 0;

		if (!error && !inuse && !np->n_sillyrename) {
			/*
			 * removal succeeded, it's not in use, and not silly renamed so
			 * remove nfsnode from hash now so we can't accidentally find it
			 * again if another object gets created with the same filehandle
			 * before this vnode gets reclaimed
			 */
			lck_mtx_lock(nfs_node_hash_mutex);
			if (np->n_hflag & NHHASHED) {
				LIST_REMOVE(np, n_hash);
				np->n_hflag &= ~NHHASHED;
				FSDBG(266, 0, np, np->n_flag, 0xb1eb1e);
			}
			lck_mtx_unlock(nfs_node_hash_mutex);
			/* clear flags now: won't get nfs_vnop_inactive for recycled vnode */
			/* clear all flags other than these */
			nfs_node_lock_force(np);
			np->n_flag &= (NMODIFIED);
			NATTRINVALIDATE(np);
			nfs_node_unlock(np);
			vnode_recycle(vp);
			setsize = 1;
		} else {
			nfs_node_lock_force(np);
			NATTRINVALIDATE(np);
			nfs_node_unlock(np);
		}
	} else if (!np->n_sillyrename) {
		if (!namedattrs)
			nfs_dulookup_start(&dul, dnp, ctx);
		error = nfs_sillyrename(dnp, np, cnp, ctx);
		nfs_node_lock_force(np);
		NATTRINVALIDATE(np);
		nfs_node_unlock(np);
	} else {
		nfs_node_lock_force(np);
		NATTRINVALIDATE(np);
		nfs_node_unlock(np);
		if (!namedattrs)
			nfs_dulookup_start(&dul, dnp, ctx);
	}
<<<<<<< HEAD
=======
	np->n_xid = 0;
	vput(dvp);

	VOP_UNLOCK(vp, 0, cnp->cn_proc);
	NFS_FREE_PNBUF(cnp);
	ubc_uncache(vp);
	vrele(vp);
>>>>>>> origin/10.3

	/* nfs_getattr() will check changed and purge caches */
	nfs_getattr(dnp, NULL, ctx, NGA_CACHED);
	if (!namedattrs)
		nfs_dulookup_finish(&dul, dnp, ctx);
out:
	/* unlock the node */
	lck_mtx_lock(nfs_node_hash_mutex);
	np->n_hflag &= ~NHLOCKED;
	if (np->n_hflag & NHLOCKWANT) {
		np->n_hflag &= ~NHLOCKWANT;
		wakeup(np);
	}
	lck_mtx_unlock(nfs_node_hash_mutex);
	nfs_node_clear_busy2(dnp, np);
	if (setsize)
		ubc_setsize(vp, 0);
	return (error);
}

/*
 * NFS silly-renamed file removal function called from nfs_vnop_inactive
 */
int
nfs_removeit(struct nfs_sillyrename *nsp)
{
	struct nfsmount *nmp = NFSTONMP(nsp->nsr_dnp);
	if (nfs_mount_gone(nmp))
		return (ENXIO);
	return nmp->nm_funcs->nf_remove_rpc(nsp->nsr_dnp, nsp->nsr_name, nsp->nsr_namlen, NULL, nsp->nsr_cred);
}

/*
 * NFS remove rpc, called from nfs_remove() and nfs_removeit().
 */
int
nfs3_remove_rpc(
	nfsnode_t dnp,
	char *name,
	int namelen,
	thread_t thd,
	kauth_cred_t cred)
{
<<<<<<< HEAD
	int error = 0, lockerror = ENOENT, status, wccpostattr = 0;
	struct timespec premtime = { 0, 0 };
	struct nfsmount *nmp;
	int nfsvers;
	u_int64_t xid;
	struct nfsm_chain nmreq, nmrep;

	nmp = NFSTONMP(dnp);
	if (nfs_mount_gone(nmp))
		return (ENXIO);
	nfsvers = nmp->nm_vers;
	if ((nfsvers == NFS_VER2) && (namelen > NFS_MAXNAMLEN))
		return (ENAMETOOLONG);

	nfsm_chain_null(&nmreq);
	nfsm_chain_null(&nmrep);

	nfsm_chain_build_alloc_init(error, &nmreq,
		NFSX_FH(nfsvers) + NFSX_UNSIGNED + nfsm_rndup(namelen));
	nfsm_chain_add_fh(error, &nmreq, nfsvers, dnp->n_fhp, dnp->n_fhsize);
	nfsm_chain_add_name(error, &nmreq, name, namelen, nmp);
	nfsm_chain_build_done(error, &nmreq);
	nfsmout_if(error);

	error = nfs_request2(dnp, NULL, &nmreq, NFSPROC_REMOVE, thd, cred, NULL, 0, &nmrep, &xid, &status);

	if ((lockerror = nfs_node_lock(dnp)))
		error = lockerror;
	if (nfsvers == NFS_VER3)
		nfsm_chain_get_wcc_data(error, &nmrep, dnp, &premtime, &wccpostattr, &xid);
	nfsmout_if(error);
	dnp->n_flag |= NMODIFIED;
	/* if directory hadn't changed, update namecache mtime */
	if (nfstimespeccmp(&dnp->n_ncmtime, &premtime, ==))
		NFS_CHANGED_UPDATE_NC(nfsvers, dnp, &dnp->n_vattr);
	if (!wccpostattr)
		NATTRINVALIDATE(dnp);
	if (!error)
		error = status;
nfsmout:
	if (!lockerror)
		nfs_node_unlock(dnp);
	nfsm_chain_cleanup(&nmreq);
	nfsm_chain_cleanup(&nmrep);
=======
	register u_long *tl;
	register caddr_t cp;
	register long t1, t2;
	caddr_t bpos, dpos, cp2;
	int error = 0, wccpostattr = 0;
	time_t premtime = 0;
	struct mbuf *mreq, *mrep, *md, *mb, *mb2;
	int v3 = NFS_ISV3(dvp);
	u_int64_t xid;

	nfsstats.rpccnt[NFSPROC_REMOVE]++;
	nfsm_reqhead(dvp, NFSPROC_REMOVE,
		NFSX_FH(v3) + NFSX_UNSIGNED + nfsm_rndup(namelen));
	nfsm_fhtom(dvp, v3);
	nfsm_strtom(name, namelen, NFS_MAXNAMLEN);
	nfsm_request(dvp, NFSPROC_REMOVE, proc, cred, &xid);
	if (v3 && mrep)
		nfsm_wcc_data(dvp, premtime, wccpostattr, &xid);
	nfsm_reqdone;
<<<<<<< HEAD
	if (dvp->v_type != VBAD) { /* EINVAL set on VBAD vnode */
		VTONFS(dvp)->n_flag |= NMODIFIED;
		if (!wccflag)
			VTONFS(dvp)->n_attrstamp = 0;
	}
>>>>>>> origin/10.1
=======
	VTONFS(dvp)->n_flag |= NMODIFIED;
	/* if directory hadn't changed, update namecache mtime */
	if (VTONFS(dvp)->n_ncmtime == premtime)
		VTONFS(dvp)->n_ncmtime = VTONFS(dvp)->n_vattr.va_mtime.tv_sec;
	if (!wccpostattr)
		VTONFS(dvp)->n_xid = 0;
>>>>>>> origin/10.3
	return (error);
}

/*
 * NFS file rename call
 */
int
nfs_vnop_rename(
	struct vnop_rename_args  /* {
		struct vnodeop_desc *a_desc;
		vnode_t a_fdvp;
		vnode_t a_fvp;
		struct componentname *a_fcnp;
		vnode_t a_tdvp;
		vnode_t a_tvp;
		struct componentname *a_tcnp;
		vfs_context_t a_context;
	} */ *ap)
{
<<<<<<< HEAD
	vfs_context_t ctx = ap->a_context;
	vnode_t fdvp = ap->a_fdvp;
	vnode_t fvp = ap->a_fvp;
	vnode_t tdvp = ap->a_tdvp;
	vnode_t tvp = ap->a_tvp;
	nfsnode_t fdnp, fnp, tdnp, tnp;
	struct componentname *tcnp = ap->a_tcnp;
	struct componentname *fcnp = ap->a_fcnp;
	int error, nfsvers, inuse=0, tvprecycle=0, locked=0;
	mount_t fmp, tdmp, tmp;
	struct nfs_vattr nvattr;
	struct nfsmount *nmp;

	fdnp = VTONFS(fdvp);
	fnp = VTONFS(fvp);
	tdnp = VTONFS(tdvp);
	tnp = tvp ? VTONFS(tvp) : NULL;

	nmp = NFSTONMP(fdnp);
	if (nfs_mount_gone(nmp))
		return (ENXIO);
	nfsvers = nmp->nm_vers;

	error = nfs_node_set_busy4(fdnp, fnp, tdnp, tnp, vfs_context_thread(ctx));
	if (error)
		return (error);

	if (tvp && (tvp != fvp)) {
		/* lock the node while we rename over the existing file */
		lck_mtx_lock(nfs_node_hash_mutex);
		while (tnp->n_hflag & NHLOCKED) {
			tnp->n_hflag |= NHLOCKWANT;
			msleep(tnp, nfs_node_hash_mutex, PINOD, "nfs_rename", NULL);
		}
		tnp->n_hflag |= NHLOCKED;
		lck_mtx_unlock(nfs_node_hash_mutex);
		locked = 1;
	}
=======
	register struct vnode *fvp = ap->a_fvp;
	register struct vnode *tvp = ap->a_tvp;
	register struct vnode *fdvp = ap->a_fdvp;
	register struct vnode *tdvp = ap->a_tdvp;
	register struct componentname *tcnp = ap->a_tcnp;
	register struct componentname *fcnp = ap->a_fcnp;
	int error, inuse=0;
>>>>>>> origin/10.3

	/* Check for cross-device rename */
	fmp = vnode_mount(fvp);
	tmp = tvp ? vnode_mount(tvp) : NULL;
	tdmp = vnode_mount(tdvp);
	if ((fmp != tdmp) || (tvp && (fmp != tmp))) {
		error = EXDEV;
		goto out;
	}

	/* XXX prevent renaming from/over a sillyrenamed file? */

	/*
	 * If the tvp exists and is in use, sillyrename it before doing the
	 * rename of the new file over it.
	 * XXX Can't sillyrename a directory.
	 * Don't sillyrename if source and target are same vnode (hard
	 * links or case-variants)
	 */
	if (tvp && (tvp != fvp))
		inuse = vnode_isinuse(tvp, 0);
	if (inuse && !tnp->n_sillyrename && (vnode_vtype(tvp) != VDIR)) {
		error = nfs_sillyrename(tdnp, tnp, tcnp, ctx);
		if (error) {
			/* sillyrename failed. Instead of pressing on, return error */
			goto out; /* should not be ENOENT. */
		} else {
			/* sillyrename succeeded.*/
			tvp = NULL;
		}
	} else if (tvp && (nmp->nm_vers >= NFS_VER4) && (tnp->n_openflags & N_DELEG_MASK)) {
		nfs4_delegation_return(tnp, 0, vfs_context_thread(ctx), vfs_context_ucred(ctx));
	}

	error = nmp->nm_funcs->nf_rename_rpc(fdnp, fcnp->cn_nameptr, fcnp->cn_namelen,
			tdnp, tcnp->cn_nameptr, tcnp->cn_namelen, ctx);

<<<<<<< HEAD
	/*
	 * Kludge: Map ENOENT => 0 assuming that it is a reply to a retry.
	 */
	if (error == ENOENT)
		error = 0;

	if (tvp && (tvp != fvp) && !tnp->n_sillyrename) {
		nfs_node_lock_force(tnp);
		tvprecycle = (!error && !vnode_isinuse(tvp, 0) &&
		    (nfs_getattrcache(tnp, &nvattr, 0) || (nvattr.nva_nlink == 1)));
		nfs_node_unlock(tnp);
		lck_mtx_lock(nfs_node_hash_mutex);
		if (tvprecycle && (tnp->n_hflag & NHHASHED)) {
			/*
			 * remove nfsnode from hash now so we can't accidentally find it
			 * again if another object gets created with the same filehandle
			 * before this vnode gets reclaimed
			 */
			LIST_REMOVE(tnp, n_hash);
			tnp->n_hflag &= ~NHHASHED;
			FSDBG(266, 0, tnp, tnp->n_flag, 0xb1eb1e);
		}
		lck_mtx_unlock(nfs_node_hash_mutex);
	}

	/* purge the old name cache entries and enter the new one */
	nfs_name_cache_purge(fdnp, fnp, fcnp, ctx);
	if (tvp) {
		nfs_name_cache_purge(tdnp, tnp, tcnp, ctx);
		if (tvprecycle) {
			/* clear flags now: won't get nfs_vnop_inactive for recycled vnode */
			/* clear all flags other than these */
			nfs_node_lock_force(tnp);
			tnp->n_flag &= (NMODIFIED);
			nfs_node_unlock(tnp);
			vnode_recycle(tvp);
		}
	}
	if (!error) {
		nfs_node_lock_force(tdnp);
		if (tdnp->n_flag & NNEGNCENTRIES) {
			tdnp->n_flag &= ~NNEGNCENTRIES;
			cache_purge_negatives(tdvp);
		}
		nfs_node_unlock(tdnp);
		nfs_node_lock_force(fnp);
		cache_enter(tdvp, fvp, tcnp);
		if (tdvp != fdvp) {	/* update parent pointer */
			if (fnp->n_parent && !vnode_get(fnp->n_parent)) {
				/* remove ref from old parent */
				vnode_rele(fnp->n_parent);
				vnode_put(fnp->n_parent);
			}
			fnp->n_parent = tdvp;
			if (tdvp && !vnode_get(tdvp)) {
				/* add ref to new parent */
				vnode_ref(tdvp);
				vnode_put(tdvp);
			} else {
				fnp->n_parent = NULL;
			}
		}
		nfs_node_unlock(fnp);
=======
	if (!error && tvp && tvp != fvp && !VTONFS(tvp)->n_sillyrename) {
		/*
		 * remove nfsnode from hash now so we can't accidentally find it
		 * again if another object gets created with the same filehandle
		 * before this vnode gets reclaimed
		 */
		LIST_REMOVE(VTONFS(tvp), n_hash);
		VTONFS(tvp)->n_flag &= ~NHASHED;
	}
	
	cache_purge(fvp);
	if (tvp) {
		cache_purge(tvp);
		VOP_UNLOCK(tvp, 0, tcnp->cn_proc);
		ubc_uncache(tvp); /* get the nfs turd file to disappear */
>>>>>>> origin/10.3
	}
out:
	/* nfs_getattr() will check changed and purge caches */
	nfs_getattr(fdnp, NULL, ctx, NGA_CACHED);
	nfs_getattr(tdnp, NULL, ctx, NGA_CACHED);
	if (locked) {
		/* unlock node */
		lck_mtx_lock(nfs_node_hash_mutex);
		tnp->n_hflag &= ~NHLOCKED;
		if (tnp->n_hflag & NHLOCKWANT) {
			tnp->n_hflag &= ~NHLOCKWANT;
			wakeup(tnp);
		}
		lck_mtx_unlock(nfs_node_hash_mutex);
	}
	nfs_node_clear_busy4(fdnp, fnp, tdnp, tnp);
	return (error);
}

/*
 * Do an NFS rename rpc. Called from nfs_vnop_rename() and nfs_sillyrename().
 */
int
nfs3_rename_rpc(
	nfsnode_t fdnp,
	char *fnameptr,
	int fnamelen,
	nfsnode_t tdnp,
	char *tnameptr,
	int tnamelen,
	vfs_context_t ctx)
{
<<<<<<< HEAD
	int error = 0, lockerror = ENOENT, status, fwccpostattr = 0, twccpostattr = 0;
	struct timespec fpremtime = { 0, 0 }, tpremtime = { 0, 0 };
	struct nfsmount *nmp;
	int nfsvers;
	u_int64_t xid, txid;
	struct nfsm_chain nmreq, nmrep;

	nmp = NFSTONMP(fdnp);
	if (nfs_mount_gone(nmp))
		return (ENXIO);
	nfsvers = nmp->nm_vers;
	if ((nfsvers == NFS_VER2) &&
	    ((fnamelen > NFS_MAXNAMLEN) || (tnamelen > NFS_MAXNAMLEN)))
		return (ENAMETOOLONG);

	nfsm_chain_null(&nmreq);
	nfsm_chain_null(&nmrep);

	nfsm_chain_build_alloc_init(error, &nmreq,
		(NFSX_FH(nfsvers) + NFSX_UNSIGNED) * 2 +
		nfsm_rndup(fnamelen) + nfsm_rndup(tnamelen));
	nfsm_chain_add_fh(error, &nmreq, nfsvers, fdnp->n_fhp, fdnp->n_fhsize);
	nfsm_chain_add_name(error, &nmreq, fnameptr, fnamelen, nmp);
	nfsm_chain_add_fh(error, &nmreq, nfsvers, tdnp->n_fhp, tdnp->n_fhsize);
	nfsm_chain_add_name(error, &nmreq, tnameptr, tnamelen, nmp);
	nfsm_chain_build_done(error, &nmreq);
	nfsmout_if(error);

	error = nfs_request(fdnp, NULL, &nmreq, NFSPROC_RENAME, ctx, NULL, &nmrep, &xid, &status);

	if ((lockerror = nfs_node_lock2(fdnp, tdnp)))
		error = lockerror;
	if (nfsvers == NFS_VER3) {
		txid = xid;
		nfsm_chain_get_wcc_data(error, &nmrep, fdnp, &fpremtime, &fwccpostattr, &xid);
		nfsm_chain_get_wcc_data(error, &nmrep, tdnp, &tpremtime, &twccpostattr, &txid);
	}
	if (!error)
		error = status;
nfsmout:
	nfsm_chain_cleanup(&nmreq);
	nfsm_chain_cleanup(&nmrep);
	if (!lockerror) {
		fdnp->n_flag |= NMODIFIED;
		/* if directory hadn't changed, update namecache mtime */
		if (nfstimespeccmp(&fdnp->n_ncmtime, &fpremtime, ==))
			NFS_CHANGED_UPDATE_NC(nfsvers, fdnp, &fdnp->n_vattr);
		if (!fwccpostattr)
			NATTRINVALIDATE(fdnp);
		tdnp->n_flag |= NMODIFIED;
		/* if directory hadn't changed, update namecache mtime */
		if (nfstimespeccmp(&tdnp->n_ncmtime, &tpremtime, ==))
			NFS_CHANGED_UPDATE_NC(nfsvers, tdnp, &tdnp->n_vattr);
		if (!twccpostattr)
			NATTRINVALIDATE(tdnp);
		nfs_node_unlock2(fdnp, tdnp);
	}
=======
	register u_long *tl;
	register caddr_t cp;
	register long t1, t2;
	caddr_t bpos, dpos, cp2;
	int error = 0, fwccpostattr = 0, twccpostattr = 0;
	time_t fpremtime = 0, tpremtime = 0;
	struct mbuf *mreq, *mrep, *md, *mb, *mb2;
	int v3 = NFS_ISV3(fdvp);
	u_int64_t xid;

	nfsstats.rpccnt[NFSPROC_RENAME]++;
	nfsm_reqhead(fdvp, NFSPROC_RENAME,
		     (NFSX_FH(v3) + NFSX_UNSIGNED)*2 + nfsm_rndup(fnamelen) +
		      nfsm_rndup(tnamelen));
	nfsm_fhtom(fdvp, v3);
	nfsm_strtom(fnameptr, fnamelen, NFS_MAXNAMLEN);
	nfsm_fhtom(tdvp, v3);
	nfsm_strtom(tnameptr, tnamelen, NFS_MAXNAMLEN);
	nfsm_request(fdvp, NFSPROC_RENAME, proc, cred, &xid);
	if (v3 && mrep) {
		u_int64_t txid = xid;

		nfsm_wcc_data(fdvp, fpremtime, fwccpostattr, &xid);
		nfsm_wcc_data(tdvp, tpremtime, twccpostattr, &txid);
	}
	nfsm_reqdone;
<<<<<<< HEAD
	if (fdvp->v_type != VBAD) { /* EINVAL set on VBAD vnode */
		VTONFS(fdvp)->n_flag |= NMODIFIED;
		if (!fwccflag)
			VTONFS(fdvp)->n_attrstamp = 0;
	}
	if (tdvp->v_type != VBAD) { /* EINVAL set on VBAD vnode */
		VTONFS(tdvp)->n_flag |= NMODIFIED;
		if (!twccflag)
			VTONFS(tdvp)->n_attrstamp = 0;
        }
>>>>>>> origin/10.1
=======
	VTONFS(fdvp)->n_flag |= NMODIFIED;
	/* if directory hadn't changed, update namecache mtime */
	if (VTONFS(fdvp)->n_ncmtime == fpremtime)
		VTONFS(fdvp)->n_ncmtime = VTONFS(fdvp)->n_vattr.va_mtime.tv_sec;
	if (!fwccpostattr)
		VTONFS(fdvp)->n_xid = 0;
	VTONFS(tdvp)->n_flag |= NMODIFIED;
	/* if directory hadn't changed, update namecache mtime */
	if (VTONFS(tdvp)->n_ncmtime == tpremtime)
		VTONFS(tdvp)->n_ncmtime = VTONFS(tdvp)->n_vattr.va_mtime.tv_sec;
	if (!twccpostattr)
		VTONFS(tdvp)->n_xid = 0;
>>>>>>> origin/10.3
	return (error);
}

/*
 * NFS hard link create call
 */
int
nfs3_vnop_link(
	struct vnop_link_args /* {
		struct vnodeop_desc *a_desc;
		vnode_t a_vp;
		vnode_t a_tdvp;
		struct componentname *a_cnp;
		vfs_context_t a_context;
	} */ *ap)
{
<<<<<<< HEAD
	vfs_context_t ctx = ap->a_context;
	vnode_t vp = ap->a_vp;
	vnode_t tdvp = ap->a_tdvp;
	struct componentname *cnp = ap->a_cnp;
	int error = 0, lockerror = ENOENT, status, wccpostattr = 0, attrflag = 0;
	struct timespec premtime = { 0, 0 };
	struct nfsmount *nmp;
	nfsnode_t np = VTONFS(vp);
	nfsnode_t tdnp = VTONFS(tdvp);
	int nfsvers;
	u_int64_t xid, txid;
	struct nfsm_chain nmreq, nmrep;

	if (vnode_mount(vp) != vnode_mount(tdvp))
=======
	register struct vnode *vp = ap->a_vp;
	register struct vnode *tdvp = ap->a_tdvp;
	register struct componentname *cnp = ap->a_cnp;
	register u_long *tl;
	register caddr_t cp;
	register long t1, t2;
	caddr_t bpos, dpos, cp2;
	int error = 0, wccpostattr = 0, attrflag = 0;
	time_t premtime = 0;
	struct mbuf *mreq, *mrep, *md, *mb, *mb2;
	int v3 = NFS_ISV3(vp);
	u_int64_t xid;

	if (vp->v_mount != tdvp->v_mount) {
		VOP_ABORTOP(vp, cnp);
		if (tdvp == vp)
			vrele(tdvp);
		else
			vput(tdvp);
>>>>>>> origin/10.1
		return (EXDEV);

	nmp = VTONMP(vp);
	if (nfs_mount_gone(nmp))
		return (ENXIO);
	nfsvers = nmp->nm_vers;
	if ((nfsvers == NFS_VER2) && (cnp->cn_namelen > NFS_MAXNAMLEN))
		return (ENAMETOOLONG);

	/*
	 * Push all writes to the server, so that the attribute cache
	 * doesn't get "out of sync" with the server.
	 * XXX There should be a better way!
	 */
<<<<<<< HEAD
	nfs_flush(np, MNT_WAIT, vfs_context_thread(ctx), V_IGNORE_WRITEERR);

	error = nfs_node_set_busy2(tdnp, np, vfs_context_thread(ctx));
	if (error)
		return (error);

	nfsm_chain_null(&nmreq);
	nfsm_chain_null(&nmrep);

	nfsm_chain_build_alloc_init(error, &nmreq,
		NFSX_FH(nfsvers)*2 + NFSX_UNSIGNED + nfsm_rndup(cnp->cn_namelen));
	nfsm_chain_add_fh(error, &nmreq, nfsvers, np->n_fhp, np->n_fhsize);
	nfsm_chain_add_fh(error, &nmreq, nfsvers, tdnp->n_fhp, tdnp->n_fhsize);
	nfsm_chain_add_name(error, &nmreq, cnp->cn_nameptr, cnp->cn_namelen, nmp);
	nfsm_chain_build_done(error, &nmreq);
	nfsmout_if(error);
	error = nfs_request(np, NULL, &nmreq, NFSPROC_LINK, ctx, NULL, &nmrep, &xid, &status);

	if ((lockerror = nfs_node_lock2(tdnp, np))) {
		error = lockerror;
		goto nfsmout;
	}
	if (nfsvers == NFS_VER3) {
		txid = xid;
		nfsm_chain_postop_attr_update_flag(error, &nmrep, np, attrflag, &xid);
		nfsm_chain_get_wcc_data(error, &nmrep, tdnp, &premtime, &wccpostattr, &txid);
	}
	if (!error)
		error = status;
nfsmout:
	nfsm_chain_cleanup(&nmreq);
	nfsm_chain_cleanup(&nmrep);
	if (!lockerror) {
		if (!attrflag)
			NATTRINVALIDATE(np);
		tdnp->n_flag |= NMODIFIED;
		/* if directory hadn't changed, update namecache mtime */
		if (nfstimespeccmp(&tdnp->n_ncmtime, &premtime, ==))
			NFS_CHANGED_UPDATE_NC(nfsvers, tdnp, &tdnp->n_vattr);
		if (!wccpostattr)
			NATTRINVALIDATE(tdnp);
		if (!error && (tdnp->n_flag & NNEGNCENTRIES)) {
			tdnp->n_flag &= ~NNEGNCENTRIES;
			cache_purge_negatives(tdvp);
		}
		nfs_node_unlock2(tdnp, np);
	}
	nfs_node_clear_busy2(tdnp, np);
=======
	VOP_FSYNC(vp, cnp->cn_cred, MNT_WAIT, cnp->cn_proc);

	nfsstats.rpccnt[NFSPROC_LINK]++;
	nfsm_reqhead(vp, NFSPROC_LINK,
		NFSX_FH(v3)*2 + NFSX_UNSIGNED + nfsm_rndup(cnp->cn_namelen));
	nfsm_fhtom(vp, v3);
	nfsm_fhtom(tdvp, v3);
	nfsm_strtom(cnp->cn_nameptr, cnp->cn_namelen, NFS_MAXNAMLEN);
	nfsm_request(vp, NFSPROC_LINK, cnp->cn_proc, cnp->cn_cred, &xid);
	if (v3 && mrep) {
		u_int64_t txid = xid;

		nfsm_postop_attr(vp, attrflag, &xid);
		nfsm_wcc_data(tdvp, premtime, wccpostattr, &txid);
	}
	nfsm_reqdone;
	FREE_ZONE(cnp->cn_pnbuf, cnp->cn_pnlen, M_NAMEI);

	VTONFS(tdvp)->n_flag |= NMODIFIED;
<<<<<<< HEAD
	if (!attrflag && vp->v_type != VBAD)  /* EINVAL set on VBAD vnode */
		VTONFS(vp)->n_attrstamp = 0;
	if (!wccflag && tdvp->v_type != VBAD)  /* EINVAL set on VBAD vnode */
		VTONFS(tdvp)->n_attrstamp = 0;
=======
	if (!attrflag)
		VTONFS(vp)->n_xid = 0;
	/* if directory hadn't changed, update namecache mtime */
	if (VTONFS(tdvp)->n_ncmtime == premtime)
		VTONFS(tdvp)->n_ncmtime = VTONFS(tdvp)->n_vattr.va_mtime.tv_sec;
	if (!wccpostattr)
		VTONFS(tdvp)->n_xid = 0;
	if (didhold)
		ubc_rele(vp);
>>>>>>> origin/10.3
	vput(tdvp);
>>>>>>> origin/10.1
	/*
	 * Kludge: Map EEXIST => 0 assuming that it is a reply to a retry.
	 */
	if (error == EEXIST)
		error = 0;
	return (error);
}

/*
 * NFS symbolic link create call
 */
int
nfs3_vnop_symlink(
	struct vnop_symlink_args /* {
		struct vnodeop_desc *a_desc;
		vnode_t a_dvp;
		vnode_t *a_vpp;
		struct componentname *a_cnp;
		struct vnode_attr *a_vap;
		char *a_target;
		vfs_context_t a_context;
	} */ *ap)
{
	vfs_context_t ctx = ap->a_context;
	vnode_t dvp = ap->a_dvp;
	struct vnode_attr *vap = ap->a_vap;
	struct componentname *cnp = ap->a_cnp;
	struct nfs_vattr nvattr;
	fhandle_t fh;
	int slen, error = 0, lockerror = ENOENT, busyerror = ENOENT, status, wccpostattr = 0;
	struct timespec premtime = { 0, 0 };
	vnode_t newvp = NULL;
	int nfsvers, gotuid, gotgid;
	u_int64_t xid = 0, dxid;
	nfsnode_t np = NULL;
	nfsnode_t dnp = VTONFS(dvp);
	struct nfsmount *nmp;
	struct nfsm_chain nmreq, nmrep;
	struct nfsreq rq, *req = &rq;
	struct nfs_dulookup dul;

	nmp = VTONMP(dvp);
	if (nfs_mount_gone(nmp))
		return (ENXIO);
	nfsvers = nmp->nm_vers;

	slen = strlen(ap->a_target);
	if ((nfsvers == NFS_VER2) &&
	    ((cnp->cn_namelen > NFS_MAXNAMLEN) || (slen > NFS_MAXPATHLEN)))
		return (ENAMETOOLONG);

	nfs_avoid_needless_id_setting_on_create(dnp, vap, ctx);

	VATTR_SET_SUPPORTED(vap, va_mode);
	VATTR_SET_SUPPORTED(vap, va_uid);
	VATTR_SET_SUPPORTED(vap, va_gid);
	VATTR_SET_SUPPORTED(vap, va_data_size);
	VATTR_SET_SUPPORTED(vap, va_access_time);
	VATTR_SET_SUPPORTED(vap, va_modify_time);
	gotuid = VATTR_IS_ACTIVE(vap, va_uid);
	gotgid = VATTR_IS_ACTIVE(vap, va_gid);

	error = busyerror = nfs_node_set_busy(dnp, vfs_context_thread(ctx));
	nfs_dulookup_init(&dul, dnp, cnp->cn_nameptr, cnp->cn_namelen, ctx);

	nfsm_chain_null(&nmreq);
	nfsm_chain_null(&nmrep);

	nfsm_chain_build_alloc_init(error, &nmreq,
		NFSX_FH(nfsvers) + 2 * NFSX_UNSIGNED +
		nfsm_rndup(cnp->cn_namelen) + nfsm_rndup(slen) + NFSX_SATTR(nfsvers));
	nfsm_chain_add_fh(error, &nmreq, nfsvers, dnp->n_fhp, dnp->n_fhsize);
	nfsm_chain_add_name(error, &nmreq, cnp->cn_nameptr, cnp->cn_namelen, nmp);
	if (nfsvers == NFS_VER3)
		nfsm_chain_add_v3sattr(error, &nmreq, vap);
	nfsm_chain_add_name(error, &nmreq, ap->a_target, slen, nmp);
	if (nfsvers == NFS_VER2)
		nfsm_chain_add_v2sattr(error, &nmreq, vap, -1);
	nfsm_chain_build_done(error, &nmreq);
	nfsmout_if(error);

	error = nfs_request_async(dnp, NULL, &nmreq, NFSPROC_SYMLINK,
			vfs_context_thread(ctx), vfs_context_ucred(ctx), NULL, 0, NULL, &req);
	if (!error) {
		nfs_dulookup_start(&dul, dnp, ctx);
		error = nfs_request_async_finish(req, &nmrep, &xid, &status);
	}

	if ((lockerror = nfs_node_lock(dnp)))
		error = lockerror;
	dxid = xid;
	if (!error && !status) {
		if (dnp->n_flag & NNEGNCENTRIES) {
			dnp->n_flag &= ~NNEGNCENTRIES;
			cache_purge_negatives(dvp);
		}
		if (nfsvers == NFS_VER3)
			error = nfsm_chain_get_fh_attr(&nmrep, dnp, ctx, nfsvers, &xid, &fh, &nvattr);
		else
			fh.fh_len = 0;
	}
	if (nfsvers == NFS_VER3)
		nfsm_chain_get_wcc_data(error, &nmrep, dnp, &premtime, &wccpostattr, &dxid);
	if (!error)
		error = status;
nfsmout:
	nfsm_chain_cleanup(&nmreq);
	nfsm_chain_cleanup(&nmrep);

	if (!lockerror) {
		dnp->n_flag |= NMODIFIED;
		/* if directory hadn't changed, update namecache mtime */
		if (nfstimespeccmp(&dnp->n_ncmtime, &premtime, ==))
			NFS_CHANGED_UPDATE_NC(nfsvers, dnp, &dnp->n_vattr);
		nfs_node_unlock(dnp);
		/* nfs_getattr() will check changed and purge caches */
		nfs_getattr(dnp, NULL, ctx, wccpostattr ? NGA_CACHED : NGA_UNCACHED);
	}

	if (!error && fh.fh_len)
		error = nfs_nget(NFSTOMP(dnp), dnp, cnp, fh.fh_data, fh.fh_len, &nvattr, &xid, rq.r_auth, NG_MAKEENTRY, &np);
	if (!error && np)
		newvp = NFSTOV(np);

	nfs_dulookup_finish(&dul, dnp, ctx);

	/*
	 * Kludge: Map EEXIST => 0 assuming that you have a reply to a retry
	 * if we can succeed in looking up the symlink.
	 */
	if ((error == EEXIST) || (!error && !newvp)) {
		if (newvp) {
			nfs_node_unlock(np);
			vnode_put(newvp);
			newvp = NULL;
		}
		error = nfs_lookitup(dnp, cnp->cn_nameptr, cnp->cn_namelen, ctx, &np);
		if (!error) {
			newvp = NFSTOV(np);
			if (vnode_vtype(newvp) != VLNK)
				error = EEXIST;
		}
	}
	if (!busyerror)
		nfs_node_clear_busy(dnp);
	if (!error && (gotuid || gotgid) &&
	    (!newvp || nfs_getattrcache(np, &nvattr, 0) ||
	     (gotuid && (nvattr.nva_uid != vap->va_uid)) ||
	     (gotgid && (nvattr.nva_gid != vap->va_gid)))) {
		/* clear ID bits if server didn't use them (or we can't tell) */
		VATTR_CLEAR_SUPPORTED(vap, va_uid);
		VATTR_CLEAR_SUPPORTED(vap, va_gid);
	}
	if (error) {
		if (newvp) {
			nfs_node_unlock(np);
			vnode_put(newvp);
		}
	} else {
		nfs_node_unlock(np);
		*ap->a_vpp = newvp;
	}
	return (error);
}

/*
 * NFS make dir call
 */
int
nfs3_vnop_mkdir(
	struct vnop_mkdir_args /* {
		struct vnodeop_desc *a_desc;
		vnode_t a_dvp;
		vnode_t *a_vpp;
		struct componentname *a_cnp;
		struct vnode_attr *a_vap;
		vfs_context_t a_context;
	} */ *ap)
{
<<<<<<< HEAD
	vfs_context_t ctx = ap->a_context;
	vnode_t dvp = ap->a_dvp;
	struct vnode_attr *vap = ap->a_vap;
	struct componentname *cnp = ap->a_cnp;
	struct nfs_vattr nvattr;
	nfsnode_t np = NULL;
	struct nfsmount *nmp;
	nfsnode_t dnp = VTONFS(dvp);
	vnode_t newvp = NULL;
	int error = 0, lockerror = ENOENT, busyerror = ENOENT, status, wccpostattr = 0;
	struct timespec premtime = { 0, 0 };
	int nfsvers, gotuid, gotgid;
	u_int64_t xid= 0, dxid;
	fhandle_t fh;
	struct nfsm_chain nmreq, nmrep;
	struct nfsreq rq, *req = &rq;
	struct nfs_dulookup dul;

	nmp = VTONMP(dvp);
	if (nfs_mount_gone(nmp))
		return (ENXIO);
	nfsvers = nmp->nm_vers;
	if ((nfsvers == NFS_VER2) && (cnp->cn_namelen > NFS_MAXNAMLEN))
		return (ENAMETOOLONG);

	nfs_avoid_needless_id_setting_on_create(dnp, vap, ctx);

	VATTR_SET_SUPPORTED(vap, va_mode);
	VATTR_SET_SUPPORTED(vap, va_uid);
	VATTR_SET_SUPPORTED(vap, va_gid);
	VATTR_SET_SUPPORTED(vap, va_data_size);
	VATTR_SET_SUPPORTED(vap, va_access_time);
	VATTR_SET_SUPPORTED(vap, va_modify_time);
	gotuid = VATTR_IS_ACTIVE(vap, va_uid);
	gotgid = VATTR_IS_ACTIVE(vap, va_gid);

	error = busyerror = nfs_node_set_busy(dnp, vfs_context_thread(ctx));
	nfs_dulookup_init(&dul, dnp, cnp->cn_nameptr, cnp->cn_namelen, ctx);

	nfsm_chain_null(&nmreq);
	nfsm_chain_null(&nmrep);

	nfsm_chain_build_alloc_init(error, &nmreq,
		NFSX_FH(nfsvers) + NFSX_UNSIGNED +
		nfsm_rndup(cnp->cn_namelen) + NFSX_SATTR(nfsvers));
	nfsm_chain_add_fh(error, &nmreq, nfsvers, dnp->n_fhp, dnp->n_fhsize);
	nfsm_chain_add_name(error, &nmreq, cnp->cn_nameptr, cnp->cn_namelen, nmp);
	if (nfsvers == NFS_VER3)
		nfsm_chain_add_v3sattr(error, &nmreq, vap);
	else
		nfsm_chain_add_v2sattr(error, &nmreq, vap, -1);
	nfsm_chain_build_done(error, &nmreq);
	nfsmout_if(error);

	error = nfs_request_async(dnp, NULL, &nmreq, NFSPROC_MKDIR,
			vfs_context_thread(ctx), vfs_context_ucred(ctx), NULL, 0, NULL, &req);
	if (!error) {
		nfs_dulookup_start(&dul, dnp, ctx);
		error = nfs_request_async_finish(req, &nmrep, &xid, &status);
	}

	if ((lockerror = nfs_node_lock(dnp)))
		error = lockerror;
	dxid = xid;
	if (!error && !status) {
		if (dnp->n_flag & NNEGNCENTRIES) {
			dnp->n_flag &= ~NNEGNCENTRIES;
			cache_purge_negatives(dvp);
		}
		error = nfsm_chain_get_fh_attr(&nmrep, dnp, ctx, nfsvers, &xid, &fh, &nvattr);
	}
	if (nfsvers == NFS_VER3)
		nfsm_chain_get_wcc_data(error, &nmrep, dnp, &premtime, &wccpostattr, &dxid);
	if (!error)
		error = status;
nfsmout:
	nfsm_chain_cleanup(&nmreq);
	nfsm_chain_cleanup(&nmrep);

	if (!lockerror) {
		dnp->n_flag |= NMODIFIED;
		/* if directory hadn't changed, update namecache mtime */
		if (nfstimespeccmp(&dnp->n_ncmtime, &premtime, ==))
			NFS_CHANGED_UPDATE_NC(nfsvers, dnp, &dnp->n_vattr);
		nfs_node_unlock(dnp);
		/* nfs_getattr() will check changed and purge caches */
		nfs_getattr(dnp, NULL, ctx, wccpostattr ? NGA_CACHED : NGA_UNCACHED);
	}

	if (!error && fh.fh_len)
		error = nfs_nget(NFSTOMP(dnp), dnp, cnp, fh.fh_data, fh.fh_len, &nvattr, &xid, rq.r_auth, NG_MAKEENTRY, &np);
	if (!error && np)
		newvp = NFSTOV(np);

	nfs_dulookup_finish(&dul, dnp, ctx);

=======
	register struct vnode *dvp = ap->a_dvp;
	register struct vattr *vap = ap->a_vap;
	register struct componentname *cnp = ap->a_cnp;
	register struct nfsv2_sattr *sp;
	register struct nfsv3_sattr *sp3;
	register u_long *tl;
	register caddr_t cp;
	register long t1, t2;
	caddr_t bpos, dpos, cp2;
	int slen, error = 0, wccpostattr = 0, gotvp;
	time_t premtime = 0;
	struct mbuf *mreq, *mrep, *md, *mb, *mb2;
	struct vnode *newvp = (struct vnode *)0;
	int v3 = NFS_ISV3(dvp);
	u_int64_t xid;

	nfsstats.rpccnt[NFSPROC_SYMLINK]++;
	slen = strlen(ap->a_target);
	nfsm_reqhead(dvp, NFSPROC_SYMLINK, NFSX_FH(v3) + 2*NFSX_UNSIGNED +
	    nfsm_rndup(cnp->cn_namelen) + nfsm_rndup(slen) + NFSX_SATTR(v3));
	nfsm_fhtom(dvp, v3);
	nfsm_strtom(cnp->cn_nameptr, cnp->cn_namelen, NFS_MAXNAMLEN);
	if (v3) {
		nfsm_build(sp3, struct nfsv3_sattr *, NFSX_V3SRVSATTR);
		nfsm_v3sattr(sp3, vap, cnp->cn_cred->cr_uid,
			cnp->cn_cred->cr_gid);
	}
	nfsm_strtom(ap->a_target, slen, NFS_MAXPATHLEN);
	if (!v3) {
		nfsm_build(sp, struct nfsv2_sattr *, NFSX_V2SATTR);
		sp->sa_mode = vtonfsv2_mode(VLNK, vap->va_mode);
		sp->sa_uid = txdr_unsigned(cnp->cn_cred->cr_uid);
		sp->sa_gid = txdr_unsigned(cnp->cn_cred->cr_gid);
		sp->sa_size = -1;
		txdr_nfsv2time(&vap->va_atime, &sp->sa_atime);
		txdr_nfsv2time(&vap->va_mtime, &sp->sa_mtime);
	}
	nfsm_request(dvp, NFSPROC_SYMLINK, cnp->cn_proc, cnp->cn_cred, &xid);
	if (v3 && mrep) {
		u_int64_t dxid = xid;

		if (!error)
			nfsm_mtofh(dvp, newvp, v3, gotvp, &xid);
		nfsm_wcc_data(dvp, premtime, wccpostattr, &dxid);
	}
	nfsm_reqdone;
	if (newvp)
		vput(newvp);
<<<<<<< HEAD
	FREE_ZONE(cnp->cn_pnbuf, cnp->cn_pnlen, M_NAMEI);
	if (dvp->v_type != VBAD) { /* EINVAL set on VBAD vnode */
		VTONFS(dvp)->n_flag |= NMODIFIED;
		if (!wccflag)
			VTONFS(dvp)->n_attrstamp = 0;
	}
=======

	VTONFS(dvp)->n_flag |= NMODIFIED;
	/* if directory hadn't changed, update namecache mtime */
	if (VTONFS(dvp)->n_ncmtime == premtime)
		VTONFS(dvp)->n_ncmtime = VTONFS(dvp)->n_vattr.va_mtime.tv_sec;
	if (!wccpostattr)
		VTONFS(dvp)->n_xid = 0;
>>>>>>> origin/10.3
	vput(dvp);
	/*
	 * Kludge: Map EEXIST => 0 assuming that it is a reply to a retry.
	 */
	if (error == EEXIST)
		error = 0;
	return (error);
}

/*
 * nfs make dir call
 */
static int
nfs_mkdir(ap)
	struct vop_mkdir_args /* {
		struct vnode *a_dvp;
		struct vnode **a_vpp;
		struct componentname *a_cnp;
		struct vattr *a_vap;
	} */ *ap;
{
	register struct vnode *dvp = ap->a_dvp;
	register struct vattr *vap = ap->a_vap;
	register struct componentname *cnp = ap->a_cnp;
	register struct nfsv2_sattr *sp;
	register struct nfsv3_sattr *sp3;
	register u_long *tl;
	register caddr_t cp;
	register long t1, t2;
	register int len;
	struct nfsnode *np = (struct nfsnode *)0;
	struct vnode *newvp = (struct vnode *)0;
	caddr_t bpos, dpos, cp2;
	int error = 0, wccpostattr = 0;
	time_t premtime = 0;
	int gotvp = 0;
	struct mbuf *mreq, *mrep, *md, *mb, *mb2;
	struct vattr vattr;
	int v3 = NFS_ISV3(dvp);
	u_int64_t xid, dxid;

	if ((error = VOP_GETATTR(dvp, &vattr, cnp->cn_cred, cnp->cn_proc))) {
		VOP_ABORTOP(dvp, cnp);
		vput(dvp);
		return (error);
	}
	len = cnp->cn_namelen;
	nfsstats.rpccnt[NFSPROC_MKDIR]++;
	nfsm_reqhead(dvp, NFSPROC_MKDIR,
	  NFSX_FH(v3) + NFSX_UNSIGNED + nfsm_rndup(len) + NFSX_SATTR(v3));
	nfsm_fhtom(dvp, v3);
	nfsm_strtom(cnp->cn_nameptr, len, NFS_MAXNAMLEN);
	if (v3) {
		nfsm_build(sp3, struct nfsv3_sattr *, NFSX_V3SRVSATTR);
		nfsm_v3sattr(sp3, vap, cnp->cn_cred->cr_uid, vattr.va_gid);
	} else {
		nfsm_build(sp, struct nfsv2_sattr *, NFSX_V2SATTR);
		sp->sa_mode = vtonfsv2_mode(VDIR, vap->va_mode);
		sp->sa_uid = txdr_unsigned(cnp->cn_cred->cr_uid);
		sp->sa_gid = txdr_unsigned(vattr.va_gid);
		sp->sa_size = -1;
		txdr_nfsv2time(&vap->va_atime, &sp->sa_atime);
		txdr_nfsv2time(&vap->va_mtime, &sp->sa_mtime);
	}
	nfsm_request(dvp, NFSPROC_MKDIR, cnp->cn_proc, cnp->cn_cred, &xid);
	dxid = xid;
	if (!error)
		nfsm_mtofh(dvp, newvp, v3, gotvp, &xid);
	if (v3 && mrep)
		nfsm_wcc_data(dvp, premtime, wccpostattr, &dxid);
	nfsm_reqdone;
<<<<<<< HEAD
	if (dvp->v_type != VBAD) { /* EINVAL set on this case */
		VTONFS(dvp)->n_flag |= NMODIFIED;
		if (!wccflag)
			VTONFS(dvp)->n_attrstamp = 0;
	}
>>>>>>> origin/10.1
=======
	VTONFS(dvp)->n_flag |= NMODIFIED;
	/* if directory hadn't changed, update namecache mtime */
	if (VTONFS(dvp)->n_ncmtime == premtime)
		VTONFS(dvp)->n_ncmtime = VTONFS(dvp)->n_vattr.va_mtime.tv_sec;
	if (!wccpostattr)
		VTONFS(dvp)->n_xid = 0;
>>>>>>> origin/10.3
	/*
	 * Kludge: Map EEXIST => 0 assuming that you have a reply to a retry
	 * if we can succeed in looking up the directory.
	 */
	if ((error == EEXIST) || (!error && !newvp)) {
		if (newvp) {
			nfs_node_unlock(np);
			vnode_put(newvp);
			newvp = NULL;
		}
<<<<<<< HEAD
		error = nfs_lookitup(dnp, cnp->cn_nameptr, cnp->cn_namelen, ctx, &np);
=======
		error = nfs_lookitup(dvp, cnp->cn_nameptr, len, cnp->cn_cred,
				     cnp->cn_proc, &np);
>>>>>>> origin/10.1
		if (!error) {
			newvp = NFSTOV(np);
			if (vnode_vtype(newvp) != VDIR)
				error = EEXIST;
		}
	}
	if (!busyerror)
		nfs_node_clear_busy(dnp);
	if (!error && (gotuid || gotgid) &&
	    (!newvp || nfs_getattrcache(np, &nvattr, 0) ||
	     (gotuid && (nvattr.nva_uid != vap->va_uid)) ||
	     (gotgid && (nvattr.nva_gid != vap->va_gid)))) {
		/* clear ID bits if server didn't use them (or we can't tell) */
		VATTR_CLEAR_SUPPORTED(vap, va_uid);
		VATTR_CLEAR_SUPPORTED(vap, va_gid);
	}
	if (error) {
<<<<<<< HEAD
		if (newvp) {
			nfs_node_unlock(np);
			vnode_put(newvp);
		}
	} else {
		nfs_node_unlock(np);
		*ap->a_vpp = newvp;
	}
=======
		if (newvp)
			vput(newvp);
	} else {
		if (cnp->cn_flags & MAKEENTRY)
			cache_enter(dvp, newvp, cnp);
		*ap->a_vpp = newvp;
	}
	vput(dvp);
	NFS_FREE_PNBUF(cnp);
>>>>>>> origin/10.3
	return (error);
}

/*
 * NFS remove directory call
 */
int
nfs3_vnop_rmdir(
	struct vnop_rmdir_args /* {
		struct vnodeop_desc *a_desc;
		vnode_t a_dvp;
		vnode_t a_vp;
		struct componentname *a_cnp;
		vfs_context_t a_context;
	} */ *ap)
{
<<<<<<< HEAD
	vfs_context_t ctx = ap->a_context;
	vnode_t vp = ap->a_vp;
	vnode_t dvp = ap->a_dvp;
	struct componentname *cnp = ap->a_cnp;
	int error = 0, lockerror = ENOENT, status, wccpostattr = 0;
	struct timespec premtime = { 0, 0 };
	struct nfsmount *nmp;
	nfsnode_t np = VTONFS(vp);
	nfsnode_t dnp = VTONFS(dvp);
	int nfsvers;
	u_int64_t xid;
	struct nfsm_chain nmreq, nmrep;
	struct nfsreq rq, *req = &rq;
	struct nfs_dulookup dul;

	nmp = VTONMP(vp);
	if (nfs_mount_gone(nmp))
		return (ENXIO);
	nfsvers = nmp->nm_vers;
	if ((nfsvers == NFS_VER2) && (cnp->cn_namelen > NFS_MAXNAMLEN))
		return (ENAMETOOLONG);

	if ((error = nfs_node_set_busy2(dnp, np, vfs_context_thread(ctx))))
		return (error);

	nfs_dulookup_init(&dul, dnp, cnp->cn_nameptr, cnp->cn_namelen, ctx);

	nfsm_chain_null(&nmreq);
	nfsm_chain_null(&nmrep);

	nfsm_chain_build_alloc_init(error, &nmreq,
		NFSX_FH(nfsvers) + NFSX_UNSIGNED + nfsm_rndup(cnp->cn_namelen));
	nfsm_chain_add_fh(error, &nmreq, nfsvers, dnp->n_fhp, dnp->n_fhsize);
	nfsm_chain_add_name(error, &nmreq, cnp->cn_nameptr, cnp->cn_namelen, nmp);
	nfsm_chain_build_done(error, &nmreq);
	nfsmout_if(error);

	error = nfs_request_async(dnp, NULL, &nmreq, NFSPROC_RMDIR,
			vfs_context_thread(ctx), vfs_context_ucred(ctx), NULL, 0, NULL, &req);
	if (!error) {
		nfs_dulookup_start(&dul, dnp, ctx);
		error = nfs_request_async_finish(req, &nmrep, &xid, &status);
	}

	if ((lockerror = nfs_node_lock(dnp)))
		error = lockerror;
	if (nfsvers == NFS_VER3)
		nfsm_chain_get_wcc_data(error, &nmrep, dnp, &premtime, &wccpostattr, &xid);
	if (!error)
		error = status;
nfsmout:
	nfsm_chain_cleanup(&nmreq);
	nfsm_chain_cleanup(&nmrep);

	if (!lockerror) {
		dnp->n_flag |= NMODIFIED;
		/* if directory hadn't changed, update namecache mtime */
		if (nfstimespeccmp(&dnp->n_ncmtime, &premtime, ==))
			NFS_CHANGED_UPDATE_NC(nfsvers, dnp, &dnp->n_vattr);
		nfs_node_unlock(dnp);
		nfs_name_cache_purge(dnp, np, cnp, ctx);
		/* nfs_getattr() will check changed and purge caches */
		nfs_getattr(dnp, NULL, ctx, wccpostattr ? NGA_CACHED : NGA_UNCACHED);
	}
	nfs_dulookup_finish(&dul, dnp, ctx);
	nfs_node_clear_busy2(dnp, np);

=======
	register struct vnode *vp = ap->a_vp;
	register struct vnode *dvp = ap->a_dvp;
	register struct componentname *cnp = ap->a_cnp;
	register u_long *tl;
	register caddr_t cp;
	register long t1, t2;
	caddr_t bpos, dpos, cp2;
	int error = 0, wccpostattr = 0;
	time_t premtime = 0;
	struct mbuf *mreq, *mrep, *md, *mb, *mb2;
	int v3 = NFS_ISV3(dvp);
	u_int64_t xid;

	nfsstats.rpccnt[NFSPROC_RMDIR]++;
	nfsm_reqhead(dvp, NFSPROC_RMDIR,
		NFSX_FH(v3) + NFSX_UNSIGNED + nfsm_rndup(cnp->cn_namelen));
	nfsm_fhtom(dvp, v3);
	nfsm_strtom(cnp->cn_nameptr, cnp->cn_namelen, NFS_MAXNAMLEN);
	nfsm_request(dvp, NFSPROC_RMDIR, cnp->cn_proc, cnp->cn_cred, &xid);
	if (v3 && mrep)
		nfsm_wcc_data(dvp, premtime, wccpostattr, &xid);
	nfsm_reqdone;
<<<<<<< HEAD
	FREE_ZONE(cnp->cn_pnbuf, cnp->cn_pnlen, M_NAMEI);
	if (dvp->v_type != VBAD) { /* EINVAL set on this case */
		VTONFS(dvp)->n_flag |= NMODIFIED;
		if (!wccflag)
			VTONFS(dvp)->n_attrstamp = 0;
	}
	cache_purge(dvp);
	cache_purge(vp);
	vput(vp);
	vput(dvp);
>>>>>>> origin/10.1
=======
	VTONFS(dvp)->n_flag |= NMODIFIED;
	/* if directory hadn't changed, update namecache mtime */
	if (VTONFS(dvp)->n_ncmtime == premtime)
		VTONFS(dvp)->n_ncmtime = VTONFS(dvp)->n_vattr.va_mtime.tv_sec;
	if (!wccpostattr)
		VTONFS(dvp)->n_xid = 0;
	cache_purge(vp);
>>>>>>> origin/10.3
	/*
	 * Kludge: Map ENOENT => 0 assuming that you have a reply to a retry.
	 */
	if (error == ENOENT)
		error = 0;
	if (!error) {
		/*
		 * remove nfsnode from hash now so we can't accidentally find it
		 * again if another object gets created with the same filehandle
		 * before this vnode gets reclaimed
		 */
<<<<<<< HEAD
		lck_mtx_lock(nfs_node_hash_mutex);
		if (np->n_hflag & NHHASHED) {
			LIST_REMOVE(np, n_hash);
			np->n_hflag &= ~NHHASHED;
			FSDBG(266, 0, np, np->n_flag, 0xb1eb1e);
		}
		lck_mtx_unlock(nfs_node_hash_mutex);
	}
=======
		LIST_REMOVE(VTONFS(vp), n_hash);
		VTONFS(vp)->n_flag &= ~NHASHED;
	}
	vput(vp);
	vput(dvp);
	NFS_FREE_PNBUF(cnp);
>>>>>>> origin/10.3
	return (error);
}

/*
 * NFS readdir call
 *
 * The incoming "offset" is a directory cookie indicating where in the
 * directory entries should be read from.  A zero cookie means start at
 * the beginning of the directory.  Any other cookie will be a cookie
 * returned from the server.  
 *
 * Using that cookie, determine which buffer (and where in that buffer)
 * to start returning entries from.  Buffer logical block numbers are
 * the cookies they start at.  If a buffer is found that is not full,
 * call into the bio/RPC code to fill it.  The RPC code will probably
 * fill several buffers (dropping the first, requiring a re-get).
 *
 * When done copying entries to the buffer, set the offset to the current
 * entry's cookie and enter that cookie in the cookie cache.
 *
 * Note: because the getdirentries(2) API returns a long-typed offset,
 * the incoming offset is a potentially truncated cookie (ptc).
 * The cookie matching code is aware of this and will fall back to
 * matching only 32 bits of the cookie.
 */
int
nfs_vnop_readdir(
	struct vnop_readdir_args /* {
		struct vnodeop_desc *a_desc;
		vnode_t a_vp;
		struct uio *a_uio;
		int a_flags;
		int *a_eofflag;
		int *a_numdirent;
		vfs_context_t a_context;
	} */ *ap)
{
	vfs_context_t ctx = ap->a_context;
	vnode_t dvp = ap->a_vp;
	nfsnode_t dnp = VTONFS(dvp);
	struct nfsmount *nmp;
	uio_t uio = ap->a_uio;
	int error, nfsvers, extended, numdirent, bigcookies, ptc, done;
	uint16_t i, iptc, rlen, nlen;
	uint64_t cookie, nextcookie, lbn = 0;
	struct nfsbuf *bp = NULL;
	struct nfs_dir_buf_header *ndbhp;
	struct direntry *dp, *dpptc;
	struct dirent dent;
	char *cp = NULL;
	thread_t thd;

	nmp = VTONMP(dvp);
	if (nfs_mount_gone(nmp))
		return (ENXIO);
	nfsvers = nmp->nm_vers;
	bigcookies = (nmp->nm_state & NFSSTA_BIGCOOKIES);
	extended = (ap->a_flags & VNODE_READDIR_EXTENDED);

	if (vnode_vtype(dvp) != VDIR)
		return (EPERM);
<<<<<<< HEAD
=======
	/*
	 * First, check for hit on the EOF offset cache
	 */
	if (np->n_direofoffset > 0 && uio->uio_offset >= np->n_direofoffset &&
	    (np->n_flag & NMODIFIED) == 0) {
		if (VFSTONFS(vp->v_mount)->nm_flag & NFSMNT_NQNFS) {
			if (NQNFS_CKCACHABLE(vp, ND_READ)) {
				nfsstats.direofcache_hits++;
				return (0);
			}
		} else if (!VOP_GETATTR(vp, &vattr, ap->a_cred, uio->uio_procp)) {
			if (np->n_mtime == vattr.va_mtime.tv_sec) {
				nfsstats.direofcache_hits++;
				return (0);
			}
			if (np->n_ncmtime != vattr.va_mtime.tv_sec) {
				/* directory changed, purge any name cache entries */
				cache_purge(vp);
			}
		}
	}
>>>>>>> origin/10.3

	if (ap->a_eofflag)
		*ap->a_eofflag = 0;

	if (uio_resid(uio) == 0)
		return (0);

	if ((nfsvers >= NFS_VER4) && (dnp->n_vattr.nva_flags & NFS_FFLAG_TRIGGER)) {
		/* trigger directories should never be read, return nothing */
		return (0);
	}

<<<<<<< HEAD
	thd = vfs_context_thread(ctx);
	numdirent = done = 0;
	nextcookie = uio_offset(uio);
	ptc = bigcookies && NFS_DIR_COOKIE_POTENTIALLY_TRUNCATED(nextcookie);

	if ((error = nfs_node_lock(dnp)))
		goto out;

	if (dnp->n_flag & NNEEDINVALIDATE) {
		dnp->n_flag &= ~NNEEDINVALIDATE;
		nfs_invaldir(dnp);
		nfs_node_unlock(dnp);
		error = nfs_vinvalbuf(dvp, 0, ctx, 1);
		if (!error)
			error = nfs_node_lock(dnp);
		if (error)
			goto out;
	}
=======
{
	register int len, left;
	register struct dirent *dp;
	register u_long *tl;
	register caddr_t cp;
	register long t1, t2;
	register nfsuint64 *cookiep;
	caddr_t bpos, dpos, cp2;
	struct mbuf *mreq, *mrep, *md, *mb, *mb2;
	nfsuint64 cookie;
	struct nfsmount *nmp = VFSTONFS(vp->v_mount);
	struct nfsnode *dnp = VTONFS(vp);
	u_quad_t fileno;
	int error = 0, tlen, more_dirs = 1, blksiz = 0, bigenough = 1;
	int attrflag;
	int v3 = NFS_ISV3(vp);
	u_int64_t xid;

#ifndef nolint
	dp = (struct dirent *)0;
#endif
#if DIAGNOSTIC
	if (uiop->uio_iovcnt != 1 || (uiop->uio_offset & (NFS_DIRBLKSIZ - 1)) ||
		(uiop->uio_resid & (NFS_DIRBLKSIZ - 1)))
		panic("nfs_readdirrpc: bad uio");
#endif
>>>>>>> origin/10.1

	/*
	 * check for need to invalidate when (re)starting at beginning
	 */
	if (!nextcookie) {
		if (dnp->n_flag & NMODIFIED) {
			nfs_invaldir(dnp);
			nfs_node_unlock(dnp);
			if ((error = nfs_vinvalbuf(dvp, 0, ctx, 1)))
				goto out;
		} else {
<<<<<<< HEAD
			nfs_node_unlock(dnp);
=======
			nfsm_build(tl, u_long *, 2 * NFSX_UNSIGNED);
			*tl++ = cookie.nfsuquad[0];
		}
		*tl = txdr_unsigned(nmp->nm_readdirsize);
		nfsm_request(vp, NFSPROC_READDIR, uiop->uio_procp, cred, &xid);
		if (v3) {
			if (mrep) {
				nfsm_postop_attr(vp, attrflag, &xid);
			}
			if (!error) {
				nfsm_dissect(tl, u_long *, 2 * NFSX_UNSIGNED);
				dnp->n_cookieverf.nfsuquad[0] = *tl++;
				dnp->n_cookieverf.nfsuquad[1] = *tl;
			} else {
				m_freem(mrep);
				goto nfsmout;
			}
		} else if (!mrep) {
			// XXX assert error?
			goto nfsmout;
		}
		nfsm_dissect(tl, u_long *, NFSX_UNSIGNED);
		more_dirs = fxdr_unsigned(int, *tl);
	
		/* loop thru the dir entries, doctoring them to 4bsd form */
		while (more_dirs && bigenough) {
			if (v3) {
				nfsm_dissect(tl, u_long *, 3 * NFSX_UNSIGNED);
				fxdr_hyper(tl, &fileno);
				len = fxdr_unsigned(int, *(tl + 2));
			} else {
				nfsm_dissect(tl, u_long *, 2 * NFSX_UNSIGNED);
				fileno = fxdr_unsigned(u_quad_t, *tl++);
				len = fxdr_unsigned(int, *tl);
			}
			if (len <= 0 || len > NFS_MAXNAMLEN) {
				error = EBADRPC;
				m_freem(mrep);
				goto nfsmout;
			}
			tlen = nfsm_rndup(len);
			if (tlen == len)
				tlen += 4;	/* To ensure null termination */
			left = DIRBLKSIZ - blksiz;
			if ((tlen + DIRHDSIZ) > left) {
				dp->d_reclen += left;
				uiop->uio_iov->iov_base += left;
				uiop->uio_iov->iov_len -= left;
				uiop->uio_offset += left;
				uiop->uio_resid -= left;
				blksiz = 0;
			}
			if ((tlen + DIRHDSIZ) > uiop->uio_resid)
				bigenough = 0;
			if (bigenough) {
				dp = (struct dirent *)uiop->uio_iov->iov_base;
				dp->d_fileno = (int)fileno;
				dp->d_namlen = len;
				dp->d_reclen = tlen + DIRHDSIZ;
				dp->d_type = DT_UNKNOWN;
				blksiz += dp->d_reclen;
				if (blksiz == DIRBLKSIZ)
					blksiz = 0;
				uiop->uio_offset += DIRHDSIZ;
				uiop->uio_resid -= DIRHDSIZ;
				uiop->uio_iov->iov_base += DIRHDSIZ;
				uiop->uio_iov->iov_len -= DIRHDSIZ;
				nfsm_mtouio(uiop, len);
				cp = uiop->uio_iov->iov_base;
				tlen -= len;
				*cp = '\0';	/* null terminate */
				uiop->uio_iov->iov_base += tlen;
				uiop->uio_iov->iov_len -= tlen;
				uiop->uio_offset += tlen;
				uiop->uio_resid -= tlen;
			} else
				nfsm_adv(nfsm_rndup(len));
			if (v3) {
				nfsm_dissect(tl, u_long *, 3 * NFSX_UNSIGNED);
			} else {
				nfsm_dissect(tl, u_long *, 2 * NFSX_UNSIGNED);
			}
			if (bigenough) {
				cookie.nfsuquad[0] = *tl++;
				if (v3)
					cookie.nfsuquad[1] = *tl++;
			} else if (v3)
				tl += 2;
			else
				tl++;
			more_dirs = fxdr_unsigned(int, *tl);
>>>>>>> origin/10.1
		}
		/* nfs_getattr() will check changed and purge caches */
		if ((error = nfs_getattr(dnp, NULL, ctx, NGA_UNCACHED)))
			goto out;
	} else {
		nfs_node_unlock(dnp);
	}

	error = nfs_dir_cookie_to_lbn(dnp, nextcookie, &ptc, &lbn);
	if (error) {
		if (error < 0) { /* just hit EOF cookie */
			done = 1;
			error = 0;
		}
		if (ap->a_eofflag)
			*ap->a_eofflag = 1;
	}

<<<<<<< HEAD
	while (!error && !done) {
		OSAddAtomic64(1, &nfsstats.biocache_readdirs);
		cookie = nextcookie;
getbuffer:
		error = nfs_buf_get(dnp, lbn, NFS_DIRBLKSIZ, thd, NBLK_READ, &bp);
		if (error)
			goto out;
		ndbhp = (struct nfs_dir_buf_header*)bp->nb_data;
		if (!ISSET(bp->nb_flags, NB_CACHE) || !ISSET(ndbhp->ndbh_flags, NDB_FULL)) {
			if (!ISSET(bp->nb_flags, NB_CACHE)) { /* initialize the buffer */
				ndbhp->ndbh_flags = 0;
				ndbhp->ndbh_count = 0;
				ndbhp->ndbh_entry_end = sizeof(*ndbhp);
				ndbhp->ndbh_ncgen = dnp->n_ncgen;
=======
/*
 * NFS V3 readdir plus RPC. Used in place of nfs_readdirrpc().
 */
int
nfs_readdirplusrpc(vp, uiop, cred)
	struct vnode *vp;
	register struct uio *uiop;
	struct ucred *cred;
{
	register int len, left;
	register struct dirent *dp;
	register u_long *tl;
	register caddr_t cp;
	register long t1, t2;
	register struct vnode *newvp;
	register nfsuint64 *cookiep;
	caddr_t bpos, dpos, cp2, dpossav1, dpossav2;
	struct mbuf *mreq, *mrep, *md, *mb, *mb2, *mdsav1, *mdsav2;
	struct nameidata nami, *ndp = &nami;
	struct componentname *cnp = &ndp->ni_cnd;
	nfsuint64 cookie;
	struct nfsmount *nmp = VFSTONFS(vp->v_mount);
	struct nfsnode *dnp = VTONFS(vp), *np;
	nfsfh_t *fhp;
	u_quad_t fileno;
	int error = 0, tlen, more_dirs = 1, blksiz = 0, doit, bigenough = 1, i;
	int attrflag, fhsize;
	u_int64_t xid, savexid;

#ifndef nolint
	dp = (struct dirent *)0;
#endif
#if DIAGNOSTIC
	if (uiop->uio_iovcnt != 1 || (uiop->uio_offset & (DIRBLKSIZ - 1)) ||
		(uiop->uio_resid & (DIRBLKSIZ - 1)))
		panic("nfs_readdirplusrpc: bad uio");
#endif
	ndp->ni_dvp = vp;
	newvp = NULLVP;

	/*
	 * If there is no cookie, assume directory was stale.
	 */
	cookiep = nfs_getcookie(dnp, uiop->uio_offset, 0);
	if (cookiep)
		cookie = *cookiep;
	else
		return (NFSERR_BAD_COOKIE);
	/*
	 * Loop around doing readdir rpc's of size nm_readdirsize
	 * truncated to a multiple of DIRBLKSIZ.
	 * The stopping criteria is EOF or buffer full.
	 */
	while (more_dirs && bigenough) {
		nfsstats.rpccnt[NFSPROC_READDIRPLUS]++;
		nfsm_reqhead(vp, NFSPROC_READDIRPLUS,
			NFSX_FH(1) + 6 * NFSX_UNSIGNED);
		nfsm_fhtom(vp, 1);
 		nfsm_build(tl, u_long *, 6 * NFSX_UNSIGNED);
		*tl++ = cookie.nfsuquad[0];
		*tl++ = cookie.nfsuquad[1];
		*tl++ = dnp->n_cookieverf.nfsuquad[0];
		*tl++ = dnp->n_cookieverf.nfsuquad[1];
		*tl++ = txdr_unsigned(nmp->nm_readdirsize);
		*tl = txdr_unsigned(nmp->nm_rsize);
		nfsm_request(vp, NFSPROC_READDIRPLUS, uiop->uio_procp, cred,
			     &xid);
		savexid = xid;
		if (mrep) {
			nfsm_postop_attr(vp, attrflag, &xid);
		}
		if (error) {
			m_freem(mrep);
			goto nfsmout;
		}
		nfsm_dissect(tl, u_long *, 3 * NFSX_UNSIGNED);
		dnp->n_cookieverf.nfsuquad[0] = *tl++;
		dnp->n_cookieverf.nfsuquad[1] = *tl++;
		more_dirs = fxdr_unsigned(int, *tl);

		/* loop thru the dir entries, doctoring them to 4bsd form */
		while (more_dirs && bigenough) {
			nfsm_dissect(tl, u_long *, 3 * NFSX_UNSIGNED);
			fxdr_hyper(tl, &fileno);
			len = fxdr_unsigned(int, *(tl + 2));
			if (len <= 0 || len > NFS_MAXNAMLEN) {
				error = EBADRPC;
				m_freem(mrep);
				goto nfsmout;
<<<<<<< HEAD
>>>>>>> origin/10.1
			}
			error = nfs_buf_readdir(bp, ctx);
			if (error == NFSERR_DIRBUFDROPPED)
				goto getbuffer;
			if (error)
				nfs_buf_release(bp, 1);
			if (error && (error != ENXIO) && (error != ETIMEDOUT) && (error != EINTR) && (error != ERESTART)) {
				if (!nfs_node_lock(dnp)) {
					nfs_invaldir(dnp);
					nfs_node_unlock(dnp);
=======
			}
			tlen = nfsm_rndup(len);
			if (tlen == len)
				tlen += 4;	/* To ensure null termination*/
			left = DIRBLKSIZ - blksiz;
			if ((tlen + DIRHDSIZ) > left) {
				dp->d_reclen += left;
				uiop->uio_iov->iov_base += left;
				uiop->uio_iov->iov_len -= left;
				uiop->uio_offset += left;
				uiop->uio_resid -= left;
				blksiz = 0;
			}
			if ((tlen + DIRHDSIZ) > uiop->uio_resid)
				bigenough = 0;
			if (bigenough) {
				dp = (struct dirent *)uiop->uio_iov->iov_base;
				dp->d_fileno = (int)fileno;
				dp->d_namlen = len;
				dp->d_reclen = tlen + DIRHDSIZ;
				dp->d_type = DT_UNKNOWN;
				blksiz += dp->d_reclen;
				if (blksiz == DIRBLKSIZ)
					blksiz = 0;
				uiop->uio_offset += DIRHDSIZ;
				uiop->uio_resid -= DIRHDSIZ;
				uiop->uio_iov->iov_base += DIRHDSIZ;
				uiop->uio_iov->iov_len -= DIRHDSIZ;
				cnp->cn_nameptr = uiop->uio_iov->iov_base;
				cnp->cn_namelen = len;
				nfsm_mtouio(uiop, len);
				cp = uiop->uio_iov->iov_base;
				tlen -= len;
				*cp = '\0';
				uiop->uio_iov->iov_base += tlen;
				uiop->uio_iov->iov_len -= tlen;
				uiop->uio_offset += tlen;
				uiop->uio_resid -= tlen;
			} else
				nfsm_adv(nfsm_rndup(len));
			nfsm_dissect(tl, u_long *, 3 * NFSX_UNSIGNED);
			if (bigenough) {
				cookie.nfsuquad[0] = *tl++;
				cookie.nfsuquad[1] = *tl++;
			} else
				tl += 2;

			/*
			 * Since the attributes are before the file handle
			 * (sigh), we must skip over the attributes and then
			 * come back and get them.
			 */
			attrflag = fxdr_unsigned(int, *tl);
			if (attrflag) {
			    dpossav1 = dpos;
			    mdsav1 = md;
			    nfsm_adv(NFSX_V3FATTR);
			    nfsm_dissect(tl, u_long *, NFSX_UNSIGNED);
			    doit = fxdr_unsigned(int, *tl);
			    if (doit) {
				nfsm_getfh(fhp, fhsize, 1);
				if (NFS_CMPFH(dnp, fhp, fhsize)) {
				    VREF(vp);
				    newvp = vp;
				    np = dnp;
				} else if (!bigenough ||
				        (cnp->cn_namelen == 2 &&
					 cnp->cn_nameptr[1] == '.' &&
					 cnp->cn_nameptr[0] == '.')) {
				    /*
				     * don't doit if we can't guarantee
				     * that this entry is NOT ".." because
				     * we would have to drop the lock on
				     * the directory before getting the
				     * (lock on) the ".." vnode... and we
				     * don't want to drop the dvp lock in
				     * the middle of a readdirplus.
				     */
				    doit = 0;
				} else {
				    if ((error = nfs_nget(vp->v_mount, fhp,
					fhsize, &np)))
					doit = 0;
				    else
					newvp = NFSTOV(np);
				}
			    }
			    if (doit && bigenough) {
				dpossav2 = dpos;
				dpos = dpossav1;
				mdsav2 = md;
				md = mdsav1;
				xid = savexid;
				nfsm_loadattr(newvp, (struct vattr *)0, &xid);
				dpos = dpossav2;
				md = mdsav2;
				dp->d_type =
				    IFTODT(VTTOIF(np->n_vattr.va_type));
				ndp->ni_vp = newvp;
				cnp->cn_hash = 0;
				for (cp = cnp->cn_nameptr, i = 1; i <= len;
				    i++, cp++)
				    cnp->cn_hash += (unsigned char)*cp * i;
				cache_enter(ndp->ni_dvp, ndp->ni_vp, cnp);
			    }
			} else {
			    /* Just skip over the file handle */
			    nfsm_dissect(tl, u_long *, NFSX_UNSIGNED);
			    i = fxdr_unsigned(int, *tl);
			    nfsm_adv(nfsm_rndup(i));
			}
			if (newvp != NULLVP) {
			    if (newvp == vp)
				vrele(newvp);
			    else
				vput(newvp);
			    newvp = NULLVP;
			}
			nfsm_dissect(tl, u_long *, NFSX_UNSIGNED);
			more_dirs = fxdr_unsigned(int, *tl);
		}
		/*
		 * If at end of rpc data, get the eof boolean
		 */
		if (!more_dirs) {
			nfsm_dissect(tl, u_long *, NFSX_UNSIGNED);
			more_dirs = (fxdr_unsigned(int, *tl) == 0);
		}
		m_freem(mrep);
	}
	/*
	 * Fill last record, iff any, out to a multiple of NFS_DIRBLKSIZ
	 * by increasing d_reclen for the last record.
	 */
	if (blksiz > 0) {
		left = DIRBLKSIZ - blksiz;
		dp->d_reclen += left;
		uiop->uio_iov->iov_base += left;
		uiop->uio_iov->iov_len -= left;
		uiop->uio_offset += left;
		uiop->uio_resid -= left;
	}

	/*
	 * We are now either at the end of the directory or have filled the
	 * block.
	 */
	if (bigenough)
		dnp->n_direofoffset = uiop->uio_offset;
	else {
		if (uiop->uio_resid > 0)
			printf("EEK! readdirplusrpc resid > 0\n");
		cookiep = nfs_getcookie(dnp, uiop->uio_offset, 1);
		*cookiep = cookie;
	}
nfsmout:
	if (newvp != NULLVP) {
	        if (newvp == vp)
			vrele(newvp);
		else
			vput(newvp);
		newvp = NULLVP;
	}
	return (error);
}

/*
 * Silly rename. To make the NFS filesystem that is stateless look a little
 * more like the "ufs" a remove of an active vnode is translated to a rename
 * to a funny looking filename that is removed by nfs_inactive on the
 * nfsnode. There is the potential for another process on a different client
 * to create the same funny name between the nfs_lookitup() fails and the
 * nfs_rename() completes, but...
 */

/* format of "random" names and next name to try */
/* (note: shouldn't exceed size of sillyrename.s_name) */
static char sillyrename_name[] = ".nfsAAA%04x4.4";

static int
nfs_sillyrename(dvp, vp, cnp)
	struct vnode *dvp, *vp;
	struct componentname *cnp;
{
	register struct sillyrename *sp;
	struct nfsnode *np;
	int error;
	short pid;
	struct ucred *cred;
	int i, j, k;

	cache_purge(vp);
	np = VTONFS(vp);
#if DIAGNOSTIC
	if (vp->v_type == VDIR)
		panic("nfs_sillyrename: dir");
#endif
	MALLOC_ZONE(sp, struct sillyrename *,
			sizeof (struct sillyrename), M_NFSREQ, M_WAITOK);
	sp->s_cred = crdup(cnp->cn_cred);
	sp->s_dvp = dvp;
	VREF(dvp);

	/* Fudge together a funny name */
	pid = cnp->cn_proc->p_pid;
	sp->s_namlen = sprintf(sp->s_name, sillyrename_name, pid);

	/* Try lookitups until we get one that isn't there */
	i = j = k = 0;
	while (nfs_lookitup(dvp, sp->s_name, sp->s_namlen, sp->s_cred,
		cnp->cn_proc, (struct nfsnode **)0) == 0) {
		if (sp->s_name[4]++ >= 'z')
			sp->s_name[4] = 'A';
		if (++i > ('z' - 'A' + 1)) {
			i = 0;
			if (sp->s_name[5]++ >= 'z')
				sp->s_name[5] = 'A';
			if (++j > ('z' - 'A' + 1)) {
				j = 0;
				if (sp->s_name[6]++ >= 'z')
					sp->s_name[6] = 'A';
				if (++k > ('z' - 'A' + 1)) {
					error = EINVAL;
					goto bad;
>>>>>>> origin/10.3
				}
				nfs_vinvalbuf(dvp, 0, ctx, 1);
				if (error == NFSERR_BAD_COOKIE)
					error = ENOENT;
			}
			if (error)
				goto out;
		}

		/* find next entry to return */
		dp = NFS_DIR_BUF_FIRST_DIRENTRY(bp);
		i = 0;
		if ((lbn != cookie) && !(ptc && NFS_DIR_COOKIE_SAME32(lbn, cookie))) {
			dpptc = NULL;
			iptc = 0;
			for (; (i < ndbhp->ndbh_count) && (cookie != dp->d_seekoff); i++) {
				if (ptc && !dpptc && NFS_DIR_COOKIE_SAME32(cookie, dp->d_seekoff)) {
					iptc = i;
					dpptc = dp;
				}
<<<<<<< HEAD
				nextcookie = dp->d_seekoff;
				dp = NFS_DIRENTRY_NEXT(dp);
=======
			    }
			    if (doit) {
				dpossav2 = dpos;
				dpos = dpossav1;
				mdsav2 = md;
				md = mdsav1;
				xid = savexid;
				nfsm_loadattr(newvp, (struct vattr *)0, &xid);
				dpos = dpossav2;
				md = mdsav2;
				dp->d_type =
				    IFTODT(VTTOIF(np->n_vattr.va_type));
				ndp->ni_vp = newvp;
				cnp->cn_hash = 0;
				for (cp = cnp->cn_nameptr, i = 1; i <= len;
				    i++, cp++)
				    cnp->cn_hash += (unsigned char)*cp * i;
				if (cnp->cn_namelen <= NCHNAMLEN)
				    cache_enter(ndp->ni_dvp, ndp->ni_vp, cnp);
			    }
			} else {
			    /* Just skip over the file handle */
			    nfsm_dissect(tl, u_long *, NFSX_UNSIGNED);
			    i = fxdr_unsigned(int, *tl);
			    nfsm_adv(nfsm_rndup(i));
>>>>>>> origin/10.1
			}
			if ((i == ndbhp->ndbh_count) && dpptc) {
				i = iptc;
				dp = dpptc;
			}
			if (i < ndbhp->ndbh_count) {
				nextcookie = dp->d_seekoff;
				dp = NFS_DIRENTRY_NEXT(dp);
				i++;
			}
		}
		ptc = 0;  /* only have to deal with ptc on first cookie */

<<<<<<< HEAD
		/* return as many entries as we can */
		for (; i < ndbhp->ndbh_count; i++) {
			if (extended) {
				rlen = dp->d_reclen;
				cp = (char*)dp;
			} else {
				if (!cp) {
					cp = (char*)&dent;
					bzero(cp, sizeof(dent));
				}
				if (dp->d_namlen > (sizeof(dent.d_name) - 1))
					nlen = sizeof(dent.d_name) - 1;
=======
/*
 * Look up a file name and optionally either update the file handle or
 * allocate an nfsnode, depending on the value of npp.
 * npp == NULL	--> just do the lookup
 * *npp == NULL --> allocate a new nfsnode and make sure attributes are
 *			handled too
 * *npp != NULL --> update the file handle in the vnode
 */
static int
nfs_lookitup(dvp, name, len, cred, procp, npp)
	register struct vnode *dvp;
	char *name;
	int len;
	struct ucred *cred;
	struct proc *procp;
	struct nfsnode **npp;
{
	register u_long *tl;
	register caddr_t cp;
	register long t1, t2;
	struct vnode *newvp = (struct vnode *)0;
	struct nfsnode *np, *dnp = VTONFS(dvp);
	caddr_t bpos, dpos, cp2;
	int error = 0, fhlen, attrflag;
	struct mbuf *mreq, *mrep, *md, *mb, *mb2;
	nfsfh_t *nfhp;
	int v3 = NFS_ISV3(dvp);
	u_int64_t xid;

	nfsstats.rpccnt[NFSPROC_LOOKUP]++;
	nfsm_reqhead(dvp, NFSPROC_LOOKUP,
		NFSX_FH(v3) + NFSX_UNSIGNED + nfsm_rndup(len));
	nfsm_fhtom(dvp, v3);
	nfsm_strtom(name, len, NFS_MAXNAMLEN);
	nfsm_request(dvp, NFSPROC_LOOKUP, procp, cred, &xid);
	if (npp && !error) {
		nfsm_getfh(nfhp, fhlen, v3);
		if (*npp) {
		    np = *npp;
		    if (np->n_fhsize > NFS_SMALLFH && fhlen <= NFS_SMALLFH) {
			_FREE_ZONE((caddr_t)np->n_fhp,
					np->n_fhsize, M_NFSBIGFH);
			np->n_fhp = &np->n_fh;
		    } else if (np->n_fhsize <= NFS_SMALLFH && fhlen>NFS_SMALLFH)
			MALLOC_ZONE(np->n_fhp, nfsfh_t *,
						fhlen, M_NFSBIGFH, M_WAITOK);
		    bcopy((caddr_t)nfhp, (caddr_t)np->n_fhp, fhlen);
		    np->n_fhsize = fhlen;
		    newvp = NFSTOV(np);
		} else if (NFS_CMPFH(dnp, nfhp, fhlen)) {
		    VREF(dvp);
		    newvp = dvp;
		} else {
		    error = nfs_nget(dvp->v_mount, nfhp, fhlen, &np);
		    if (error) {
			m_freem(mrep);
			return (error);
		    }
		    newvp = NFSTOV(np);
		}
		if (v3) {
			nfsm_postop_attr(newvp, attrflag, &xid);
			if (!attrflag && *npp == NULL) {
				m_freem(mrep);
				if (newvp == dvp)
					vrele(newvp);
>>>>>>> origin/10.1
				else
					nlen = dp->d_namlen;
				rlen = NFS_DIRENT_LEN(nlen);
				dent.d_reclen = rlen;
				dent.d_ino = dp->d_ino;
				dent.d_type = dp->d_type;
				dent.d_namlen = nlen;
				strlcpy(dent.d_name, dp->d_name, nlen + 1);
			}
<<<<<<< HEAD
			/* check that the record fits */
			if (rlen > uio_resid(uio)) {
				done = 1;
				break;
			}
			if ((error = uiomove(cp, rlen, uio)))
				break;
			numdirent++;
			nextcookie = dp->d_seekoff;
			dp = NFS_DIRENTRY_NEXT(dp);
		}

		if (i == ndbhp->ndbh_count) {
			/* hit end of buffer, move to next buffer */
			lbn = nextcookie;
			/* if we also hit EOF, we're done */
			if (ISSET(ndbhp->ndbh_flags, NDB_EOF)) {
				done = 1;
				if (ap->a_eofflag)
					*ap->a_eofflag = 1;
			}
=======
		} else
			nfsm_loadattr(newvp, (struct vattr *)0, &xid);
	}
	nfsm_reqdone;
	if (npp && *npp == NULL) {
		if (error) {
			if (newvp)
				if (newvp == dvp)
					vrele(newvp);
				else
					vput(newvp);
		} else
			*npp = np;
	}
	return (error);
}

/*
 * Nfs Version 3 commit rpc
 */
static int
nfs_commit(vp, offset, cnt, cred, procp)
	register struct vnode *vp;
	u_quad_t offset;
	int cnt;
	struct ucred *cred;
	struct proc *procp;
{
	register caddr_t cp;
	register u_long *tl;
	register int t1, t2;
	register struct nfsmount *nmp = VFSTONFS(vp->v_mount);
	caddr_t bpos, dpos, cp2;
	int error = 0, wccflag = NFSV3_WCCRATTR;
	struct mbuf *mreq, *mrep, *md, *mb, *mb2;
	u_int64_t xid;
	
	FSDBG(521, vp, offset, cnt, nmp->nm_flag);
	if ((nmp->nm_flag & NFSMNT_HASWRITEVERF) == 0)
		return (0);
	nfsstats.rpccnt[NFSPROC_COMMIT]++;
	nfsm_reqhead(vp, NFSPROC_COMMIT, NFSX_FH(1));
	nfsm_fhtom(vp, 1);
	nfsm_build(tl, u_long *, 3 * NFSX_UNSIGNED);
	txdr_hyper(&offset, tl);
	tl += 2;
	*tl = txdr_unsigned(cnt);
	nfsm_request(vp, NFSPROC_COMMIT, procp, cred, &xid);
	nfsm_wcc_data(vp, wccflag, &xid);
	if (!error) {
		nfsm_dissect(tl, u_long *, NFSX_V3WRITEVERF);
		if (bcmp((caddr_t)nmp->nm_verf, (caddr_t)tl,
			 NFSX_V3WRITEVERF)) {
			bcopy((caddr_t)tl, (caddr_t)nmp->nm_verf,
				NFSX_V3WRITEVERF);
			error = NFSERR_STALEWRITEVERF;
>>>>>>> origin/10.1
		}
		if (!error)
			uio_setoffset(uio, nextcookie);
		if (!error && !done && (nextcookie == cookie)) {
			printf("nfs readdir cookie didn't change 0x%llx, %d/%d\n", cookie, i, ndbhp->ndbh_count);
			error = EIO;
		}
		nfs_buf_release(bp, 1);
	}

	if (!error)
		nfs_dir_cookie_cache(dnp, nextcookie, lbn);

	if (ap->a_numdirent)
		*ap->a_numdirent = numdirent;
out:
	return (error);
}


/*
 * Invalidate cached directory information, except for the actual directory
 * blocks (which are invalidated separately).
 */
void
nfs_invaldir(nfsnode_t dnp)
{
	if (vnode_vtype(NFSTOV(dnp)) != VDIR)
		return;
	dnp->n_eofcookie = 0;
	dnp->n_cookieverf = 0;
	if (!dnp->n_cookiecache)
		return;
	dnp->n_cookiecache->free = 0;
	dnp->n_cookiecache->mru = -1;
	memset(dnp->n_cookiecache->next, -1, NFSNUMCOOKIES);
}

/*
 * calculate how much space is available for additional directory entries.
 */
uint32_t
nfs_dir_buf_freespace(struct nfsbuf *bp, int rdirplus)
{
<<<<<<< HEAD
	struct nfs_dir_buf_header *ndbhp = (struct nfs_dir_buf_header*)bp->nb_data;
	uint32_t space;

	if (!ndbhp)
		return (0);
	space = bp->nb_bufsize - ndbhp->ndbh_entry_end;
	if (rdirplus)
		space -= ndbhp->ndbh_count * sizeof(struct nfs_vattr);
	return (space);
=======
	return (nfs_flush(ap->a_vp, ap->a_cred, ap->a_waitfor, ap->a_p, 1));
>>>>>>> origin/10.1
}

/*
 * add/update a cookie->lbn entry in the directory cookie cache
 */
void
nfs_dir_cookie_cache(nfsnode_t dnp, uint64_t cookie, uint64_t lbn)
{
<<<<<<< HEAD
	struct nfsdmap *ndcc;
	int8_t i, prev;

	if (!cookie)
		return;

	if (nfs_node_lock(dnp))
		return;
=======
	register struct nfsnode *np = VTONFS(vp);
	register struct buf *bp;
	register int i;
	struct buf *nbp;
	struct nfsmount *nmp = VFSTONFS(vp->v_mount);
	int s, error = 0, slptimeo = 0, slpflag = 0, retv, bvecpos, err;
	int passone = 1;
	u_quad_t off, endoff, toff;
	struct ucred* wcred = NULL;
	struct buf **bvec = NULL;
#ifndef NFS_COMMITBVECSIZ
#define NFS_COMMITBVECSIZ	20
#endif
	struct buf *bvec_on_stack[NFS_COMMITBVECSIZ];
	int bvecsize = 0, bveccount;
	kern_return_t kret;
	upl_t	      upl;

	FSDBG_TOP(517, vp, np, waitfor, commit);
>>>>>>> origin/10.1

	if (cookie == dnp->n_eofcookie) { /* EOF cookie */
		nfs_node_unlock(dnp);
		return;
	}

	ndcc = dnp->n_cookiecache;
	if (!ndcc) {
		/* allocate the cookie cache structure */
		MALLOC_ZONE(dnp->n_cookiecache, struct nfsdmap *,
			sizeof(struct nfsdmap), M_NFSDIROFF, M_WAITOK); 
		if (!dnp->n_cookiecache) {
			nfs_node_unlock(dnp);
			return;
		}
		ndcc = dnp->n_cookiecache;
		ndcc->free = 0;
		ndcc->mru = -1;
		memset(ndcc->next, -1, NFSNUMCOOKIES);
	}

	/*
	 * Search the list for this cookie.
	 * Keep track of previous and last entries.
	 */
<<<<<<< HEAD
	prev = -1;
	i = ndcc->mru;
	while ((i != -1) && (cookie != ndcc->cookies[i].key)) {
		if (ndcc->next[i] == -1) /* stop on last entry so we can reuse */
			break;
		prev = i;
		i = ndcc->next[i];
	}
	if ((i != -1) && (cookie == ndcc->cookies[i].key)) {
		/* found it, remove from list */
		if (prev != -1)
			ndcc->next[prev] = ndcc->next[i];
		else
			ndcc->mru = ndcc->next[i];
	} else {
		/* not found, use next free entry or reuse last entry */
		if (ndcc->free != NFSNUMCOOKIES)
			i = ndcc->free++;
		else
			ndcc->next[prev] = -1;
		ndcc->cookies[i].key = cookie;
		ndcc->cookies[i].lbn = lbn;
	}
	/* insert cookie at head of MRU list */
	ndcc->next[i] = ndcc->mru;
	ndcc->mru = i;
	nfs_node_unlock(dnp);
}

/*
 * Try to map the given directory cookie to a directory buffer (return lbn).
 * If we have a possibly truncated cookie (ptc), check for 32-bit matches too.
 */
int
nfs_dir_cookie_to_lbn(nfsnode_t dnp, uint64_t cookie, int *ptc, uint64_t *lbnp)
{
	struct nfsdmap *ndcc = dnp->n_cookiecache;
	int8_t eofptc, found;
	int i, iptc;
	struct nfsmount *nmp;
	struct nfsbuf *bp, *lastbp;
	struct nfsbuflists blist;
	struct direntry *dp, *dpptc;
	struct nfs_dir_buf_header *ndbhp;

	if (!cookie) {  /* initial cookie */
		*lbnp = 0;
		*ptc = 0;
		return (0);
	}

	if (nfs_node_lock(dnp))
		return (ENOENT);

	if (cookie == dnp->n_eofcookie) { /* EOF cookie */
		nfs_node_unlock(dnp);
		OSAddAtomic64(1, &nfsstats.direofcache_hits);
		*ptc = 0;
		return (-1);
	}
	/* note if cookie is a 32-bit match with the EOF cookie */
	eofptc = *ptc ? NFS_DIR_COOKIE_SAME32(cookie, dnp->n_eofcookie) : 0;
	iptc = -1;

	/* search the list for the cookie */
	for (i = ndcc ? ndcc->mru : -1; i >= 0; i = ndcc->next[i]) {
		if (ndcc->cookies[i].key == cookie) {
			/* found a match for this cookie */
			*lbnp = ndcc->cookies[i].lbn;
			nfs_node_unlock(dnp);
			OSAddAtomic64(1, &nfsstats.direofcache_hits);
			*ptc = 0;
			return (0);
		}
		/* check for 32-bit match */
		if (*ptc && (iptc == -1) && NFS_DIR_COOKIE_SAME32(ndcc->cookies[i].key, cookie))
			iptc = i;
	}
	/* exact match not found */
	if (eofptc) {
		/* but 32-bit match hit the EOF cookie */
		nfs_node_unlock(dnp);
		OSAddAtomic64(1, &nfsstats.direofcache_hits);
		return (-1);
	}
	if (iptc >= 0) {
		/* but 32-bit match got a hit */
		*lbnp = ndcc->cookies[iptc].lbn;
		nfs_node_unlock(dnp);
		OSAddAtomic64(1, &nfsstats.direofcache_hits);
		return (0);
	}
	nfs_node_unlock(dnp);

	/*
	 * No match found in the cookie cache... hmm...
	 * Let's search the directory's buffers for the cookie.
	 */
	nmp = NFSTONMP(dnp);
	if (nfs_mount_gone(nmp))
		return (ENXIO);
	dpptc = NULL;
	found = 0;

	lck_mtx_lock(nfs_buf_mutex);
	/*
	 * Scan the list of buffers, keeping them in order.
	 * Note that itercomplete inserts each of the remaining buffers
	 * into the head of list (thus reversing the elements).  So, we
	 * make sure to iterate through all buffers, inserting them after
	 * each other, to keep them in order.
	 * Also note: the LIST_INSERT_AFTER(lastbp) is only safe because
	 * we don't drop nfs_buf_mutex.
	 */
	if (!nfs_buf_iterprepare(dnp, &blist, NBI_CLEAN)) {
		lastbp = NULL;
		while ((bp = LIST_FIRST(&blist))) {
			LIST_REMOVE(bp, nb_vnbufs);
			if (!lastbp)
				LIST_INSERT_HEAD(&dnp->n_cleanblkhd, bp, nb_vnbufs);
			else
				LIST_INSERT_AFTER(lastbp, bp, nb_vnbufs);
			lastbp = bp;
			if (found)
				continue;
			nfs_buf_refget(bp);
			if (nfs_buf_acquire(bp, NBAC_NOWAIT, 0, 0)) {
				/* just skip this buffer */
				nfs_buf_refrele(bp);
				continue;
			}
			nfs_buf_refrele(bp);

			/* scan the buffer for the cookie */
			ndbhp = (struct nfs_dir_buf_header*)bp->nb_data;
			dp = NFS_DIR_BUF_FIRST_DIRENTRY(bp);
			dpptc = NULL;
			for (i=0; (i < ndbhp->ndbh_count) && (cookie != dp->d_seekoff); i++) {
				if (*ptc && !dpptc && NFS_DIR_COOKIE_SAME32(cookie, dp->d_seekoff)) {
					dpptc = dp;
					iptc = i;
				}
				dp = NFS_DIRENTRY_NEXT(dp);
			}
			if ((i == ndbhp->ndbh_count) && dpptc) {
				/* found only a PTC match */
				dp = dpptc;
				i = iptc;
			} else if (i < ndbhp->ndbh_count) {
				*ptc = 0;
			}
			if (i < (ndbhp->ndbh_count-1)) {
				/* next entry is *in* this buffer: return this block */
				*lbnp = bp->nb_lblkno;
				found = 1;
			} else if (i == (ndbhp->ndbh_count-1)) {
				/* next entry refers to *next* buffer: return next block */
				*lbnp = dp->d_seekoff;
				found = 1;
=======
again:
	FSDBG(518, vp->v_dirtyblkhd.lh_first, np->n_flag, 0, 0);
	if (vp->v_dirtyblkhd.lh_first)
		np->n_flag |= NMODIFIED;
	off = (u_quad_t)-1;
	endoff = 0;
	bvecpos = 0;
	if (NFS_ISV3(vp) && commit) {
		s = splbio();
		/*
		 * Count up how many buffers waiting for a commit.
		 * This is an upper bound - any with dirty pages must be
		 * written not commited.
		 */
		bveccount = 0;
		for (bp = vp->v_dirtyblkhd.lh_first; bp; bp = nbp) {
			nbp = bp->b_vnbufs.le_next;
			if ((bp->b_flags & (B_BUSY | B_DELWRI | B_NEEDCOMMIT))
			    == (B_DELWRI | B_NEEDCOMMIT))
				bveccount++;
			FSDBG(519, bp, bp->b_flags, bveccount, 0);
		}
		/*
		 * Allocate space to remember the list of bufs to commit.  It is
		 * important to use M_NOWAIT here to avoid a race with nfs_write
		 * If we can't get memory (for whatever reason), we will end up
		 * committing the buffers one-by-one in the loop below.
		 */
		if (bvec != NULL && bvec != bvec_on_stack)
			_FREE(bvec, M_TEMP);
		if (bveccount > NFS_COMMITBVECSIZ) {
			MALLOC(bvec, struct buf **,
			       bveccount * sizeof(struct buf *), M_TEMP,
			       M_NOWAIT);
			if (bvec == NULL) {
				bvec = bvec_on_stack;
				bvecsize = NFS_COMMITBVECSIZ;
			} else
				bvecsize = bveccount;
		} else {
			bvec = bvec_on_stack;
			bvecsize = NFS_COMMITBVECSIZ;
		}
		FSDBG(519, 0, bvecsize, bveccount, 0);

		for (bp = vp->v_dirtyblkhd.lh_first; bp; bp = nbp) {
			nbp = bp->b_vnbufs.le_next;
			/* XXX nbp aok if we sleep in this loop? */
			FSDBG(520, bp, bp->b_flags, bvecpos, bp->b_bufsize);
			FSDBG(520, bp->b_validoff, bp->b_validend,
			      bp->b_dirtyoff, bp->b_dirtyend);
			if (bvecpos >= bvecsize)
				break;
			if ((bp->b_flags & (B_BUSY | B_DELWRI | B_NEEDCOMMIT))
				!= (B_DELWRI | B_NEEDCOMMIT))
				continue;
			SET(bp->b_flags, B_BUSY);
			/*
			 * we need a upl to see if the page has been
			 * dirtied (think mmap) since the unstable write, and
			 * so to prevent vm from paging during our commit rpc
			 */
			if (ISSET(bp->b_flags, B_PAGELIST)) {
				upl = bp->b_pagelist;
			} else {
				kret = ubc_create_upl(vp, ubc_blktooff(vp, bp->b_lblkno),
						      bp->b_bufsize, &upl,
						      NULL, UPL_PRECIOUS);
				if (kret != KERN_SUCCESS) 
					panic("nfs_flush: create upl %d", kret);
#ifdef UBC_DEBUG
				upl_ubc_alias_set(upl, current_act(), 1);
#endif /* UBC_DEBUG */
			}
			if (upl_dirty_page(ubc_upl_pageinfo(upl), 0)) {
				if (!ISSET(bp->b_flags, B_PAGELIST)) {
					err = ubc_upl_abort(upl, NULL); 
					if (err)
						printf("nfs_flush: upl abort %d\n", err);
				}
				/*
				 * Any/all of it may be modified...
				 */
				bp->b_dirtyoff = bp->b_validoff;
				bp->b_dirtyend = bp->b_validend;
				CLR(bp->b_flags, B_BUSY | B_NEEDCOMMIT);
				continue;
			}
			if (!ISSET(bp->b_flags, B_PAGELIST)) {
				bp->b_pagelist = upl;
				SET(bp->b_flags, B_PAGELIST);
				ubc_upl_map(upl, (vm_address_t *)&bp->b_data);
			}
			bremfree(bp);
			/*
			 * Work out if all buffers are using the same cred
			 * so we can deal with them all with one commit.
			 */
			if (wcred == NULL)
				wcred = bp->b_wcred;
			else if (wcred != bp->b_wcred)
				wcred = NOCRED;
			SET(bp->b_flags, B_WRITEINPROG);

			/*
			 * A list of these buffers is kept so that the
			 * second loop knows which buffers have actually
			 * been committed. This is necessary, since there
			 * may be a race between the commit rpc and new
			 * uncommitted writes on the file.
			 */
			bvec[bvecpos++] = bp;
			toff = ((u_quad_t)bp->b_blkno) * DEV_BSIZE +
				bp->b_dirtyoff;
			if (toff < off)
				off = toff;
			toff += (u_quad_t)(bp->b_dirtyend - bp->b_dirtyoff);
			if (toff > endoff)
				endoff = toff;
		}
		splx(s);
	}
	if (bvecpos > 0) {
		/*
		 * Commit data on the server, as required.
		 * If all bufs are using the same wcred, then use that with
		 * one call for all of them, otherwise commit each one
		 * separately.
		 */
		if (wcred != NOCRED)
			retv = nfs_commit(vp, off, (int)(endoff - off),
					  wcred, p);
		else {
			retv = 0;
			for (i = 0; i < bvecpos; i++) {
				off_t off, size;
				bp = bvec[i];
				FSDBG(522, bp, bp->b_blkno * DEV_BSIZE,
				      bp->b_dirtyoff, bp->b_dirtyend);
				off = ((u_quad_t)bp->b_blkno) * DEV_BSIZE +
					bp->b_dirtyoff;
				size = (u_quad_t)(bp->b_dirtyend
						  - bp->b_dirtyoff);
				retv = nfs_commit(vp, off, (int)size,
						  bp->b_wcred, p);
				if (retv) break;
>>>>>>> origin/10.1
			}
			nfs_buf_drop(bp);
		}
		nfs_buf_itercomplete(dnp, &blist, NBI_CLEAN);
	}
	lck_mtx_unlock(nfs_buf_mutex);
	if (found) {
		OSAddAtomic64(1, &nfsstats.direofcache_hits);
		return (0);
	}

<<<<<<< HEAD
	/* still not found... oh well, just start a new block */
	*lbnp = cookie;
	OSAddAtomic64(1, &nfsstats.direofcache_misses);
	return (0);
}

/*
 * scan a directory buffer for the given name
 * Returns: ESRCH if not found, ENOENT if found invalid, 0 if found
 * Note: should only be called with RDIRPLUS directory buffers
 */

#define NDBS_PURGE	1
#define NDBS_UPDATE	2

int
nfs_dir_buf_search(
	struct nfsbuf *bp,
	struct componentname *cnp,
	fhandle_t *fhp,
	struct nfs_vattr *nvap,
	uint64_t *xidp,
	time_t *attrstampp,
	daddr64_t *nextlbnp,
	int flags)
{
	struct direntry *dp;
	struct nfs_dir_buf_header *ndbhp;
	struct nfs_vattr *nvattrp;
	daddr64_t nextlbn = 0;
	int i, error = ESRCH, fhlen;

	/* scan the buffer for the name */
	ndbhp = (struct nfs_dir_buf_header*)bp->nb_data;
	dp = NFS_DIR_BUF_FIRST_DIRENTRY(bp);
	for (i=0; i < ndbhp->ndbh_count; i++) {
		nextlbn = dp->d_seekoff;
		if ((cnp->cn_namelen == dp->d_namlen) && !strcmp(cnp->cn_nameptr, dp->d_name)) {
			fhlen = dp->d_name[dp->d_namlen+1];
			nvattrp = NFS_DIR_BUF_NVATTR(bp, i);
			if ((ndbhp->ndbh_ncgen != bp->nb_np->n_ncgen) || (fhp->fh_len == 0) ||
			    (nvattrp->nva_type == VNON) || (nvattrp->nva_fileid == 0)) {
				/* entry is not valid */
				error = ENOENT;
				break;
			}
			if (flags == NDBS_PURGE) {
				dp->d_fileno = 0;
				bzero(nvattrp, sizeof(*nvattrp));
				error = ENOENT;
				break;
=======
		if (retv == NFSERR_STALEWRITEVERF)
			nfs_clearcommit(vp->v_mount);

		/*
		 * Now, either mark the blocks I/O done or mark the
		 * blocks dirty, depending on whether the commit
		 * succeeded.
		 */
		for (i = 0; i < bvecpos; i++) {
			bp = bvec[i];
			FSDBG(523, bp, retv, bp->b_flags, 0);
			CLR(bp->b_flags, (B_NEEDCOMMIT | B_WRITEINPROG));
			if (retv) {
				brelse(bp);
			} else {
				s = splbio();
				vp->v_numoutput++;
				SET(bp->b_flags, B_ASYNC);
				CLR(bp->b_flags,
				    (B_READ|B_DONE|B_ERROR|B_DELWRI));
				bp->b_dirtyoff = bp->b_dirtyend = 0;
				reassignbuf(bp, vp);
				splx(s);
				biodone(bp);
>>>>>>> origin/10.1
			}
			if (flags == NDBS_UPDATE) {
				/* update direntry's attrs if fh matches */
				if ((fhp->fh_len == fhlen) && !bcmp(&dp->d_name[dp->d_namlen+2], fhp->fh_data, fhlen)) {
					bcopy(nvap, nvattrp, sizeof(*nvap));
					dp->d_fileno = nvattrp->nva_fileid;
					nvattrp->nva_fileid = *xidp;
					*(time_t*)(&dp->d_name[dp->d_namlen+2+fhp->fh_len]) = *attrstampp;
				}
				error = 0;
				break;
			}
			/* copy out fh, attrs, attrstamp, and xid */
			fhp->fh_len = fhlen;
			bcopy(&dp->d_name[dp->d_namlen+2], fhp->fh_data, MAX(fhp->fh_len, (int)sizeof(fhp->fh_data)));
			*attrstampp = *(time_t*)(&dp->d_name[dp->d_namlen+2+fhp->fh_len]);
			bcopy(nvattrp, nvap, sizeof(*nvap));
			*xidp = nvap->nva_fileid;
			nvap->nva_fileid = dp->d_fileno;
			error = 0;
			break;
		}
		dp = NFS_DIRENTRY_NEXT(dp);
	}
	if (nextlbnp)
		*nextlbnp = nextlbn;
	return (error);
}

/*
 * Look up a name in a directory's buffers.
 * Note: should only be called with RDIRPLUS directory buffers
 */
int
nfs_dir_buf_cache_lookup(nfsnode_t dnp, nfsnode_t *npp, struct componentname *cnp, vfs_context_t ctx, int purge)
{
<<<<<<< HEAD
	nfsnode_t newnp;
	struct nfsmount *nmp;
	int error = 0, i, found = 0, count = 0;
=======
	register caddr_t cp;
	register u_long *tl;
	register int t1, t2;
	register struct nfsmount *nmp = VFSTONFS(vp->v_mount);
	caddr_t bpos, dpos, cp2;
	int error = 0, wccpostattr = 0;
	time_t premtime = 0;
	struct mbuf *mreq, *mrep, *md, *mb, *mb2;
>>>>>>> origin/10.3
	u_int64_t xid;
	struct nfs_vattr nvattr;
	fhandle_t fh;
	time_t attrstamp = 0;
	thread_t thd = vfs_context_thread(ctx);
	struct nfsbuf *bp, *lastbp, *foundbp;
	struct nfsbuflists blist;
	daddr64_t lbn, nextlbn;
	int dotunder = (cnp->cn_namelen > 2) && (cnp->cn_nameptr[0] == '.') && (cnp->cn_nameptr[1] == '_');

	nmp = NFSTONMP(dnp);
	if (nfs_mount_gone(nmp))
		return (ENXIO);
<<<<<<< HEAD
	if (!purge)
		*npp = NULL;

	/* first check most recent buffer (and next one too) */
	lbn = dnp->n_lastdbl;
	for (i=0; i < 2; i++) {
		if ((error = nfs_buf_get(dnp, lbn, NFS_DIRBLKSIZ, thd, NBLK_READ|NBLK_ONLYVALID, &bp)))
			return (error);
		if (!bp)
			break;
		count++;
		error = nfs_dir_buf_search(bp, cnp, &fh, &nvattr, &xid, &attrstamp, &nextlbn, purge ? NDBS_PURGE : 0);
		nfs_buf_release(bp, 0);
		if (error == ESRCH) {
			error = 0;
		} else {
			found = 1;
			break;
=======
	if ((nmp->nm_state & NFSSTA_HASWRITEVERF) == 0)
		return (0);
	nfsstats.rpccnt[NFSPROC_COMMIT]++;
	nfsm_reqhead(vp, NFSPROC_COMMIT, NFSX_FH(1));
	nfsm_fhtom(vp, 1);
	nfsm_build(tl, u_long *, 3 * NFSX_UNSIGNED);
	txdr_hyper(&offset, tl);
	tl += 2;
	*tl = txdr_unsigned(cnt);
	nfsm_request(vp, NFSPROC_COMMIT, procp, cred, &xid);
	if (mrep) {
		nfsm_wcc_data(vp, premtime, wccpostattr, &xid);
		/* XXX can we do anything useful with the wcc info? */
	}
	if (!error) {
		nfsm_dissect(tl, u_long *, NFSX_V3WRITEVERF);
		if (bcmp((caddr_t)nmp->nm_verf, (caddr_t)tl,
			 NFSX_V3WRITEVERF)) {
			bcopy((caddr_t)tl, (caddr_t)nmp->nm_verf,
				NFSX_V3WRITEVERF);
			error = NFSERR_STALEWRITEVERF;
>>>>>>> origin/10.3
		}
		lbn = nextlbn;
	}

	lck_mtx_lock(nfs_buf_mutex);
	if (found) {
		dnp->n_lastdbl = lbn;
		goto done;
	}
	/*
<<<<<<< HEAD
	 * Scan the list of buffers, keeping them in order.
	 * Note that itercomplete inserts each of the remaining buffers
	 * into the head of list (thus reversing the elements).  So, we
	 * make sure to iterate through all buffers, inserting them after
	 * each other, to keep them in order.
	 * Also note: the LIST_INSERT_AFTER(lastbp) is only safe because
	 * we don't drop nfs_buf_mutex.
	 */
	if (!nfs_buf_iterprepare(dnp, &blist, NBI_CLEAN)) {
		lastbp = foundbp = NULL;
		while ((bp = LIST_FIRST(&blist))) {
			LIST_REMOVE(bp, nb_vnbufs);
			if (!lastbp)
				LIST_INSERT_HEAD(&dnp->n_cleanblkhd, bp, nb_vnbufs);
			else
				LIST_INSERT_AFTER(lastbp, bp, nb_vnbufs);
			lastbp = bp;
			if (error || found)
				continue;
			if (!purge && dotunder && (count > 100)) /* don't waste too much time looking for ._ files */
				continue;
			nfs_buf_refget(bp);
			lbn = bp->nb_lblkno;
			if (nfs_buf_acquire(bp, NBAC_NOWAIT, 0, 0)) {
				/* just skip this buffer */
				nfs_buf_refrele(bp);
				continue;
			}
			nfs_buf_refrele(bp);
			count++;
			error = nfs_dir_buf_search(bp, cnp, &fh, &nvattr, &xid, &attrstamp, NULL, purge ? NDBS_PURGE : 0);
			if (error == ESRCH) {
				error = 0;
			} else {
				found = 1;
				foundbp = bp;
=======
	 * Start/do any write(s) that are required.  There is a window here
	 * where B_BUSY protects the buffer. The vm pages have been freed up,
	 * yet B_BUSY is set. Don't think you will hit any busy/incore problems
	 * while we sleep, but not absolutely sure. Keep an eye on it. Otherwise
	 * we will have to hold vm page across this locked. - EKN
	 */
loop:
	if (current_thread_aborted()) {
		error = EINTR;
		goto done;
	}
	s = splbio();
	for (bp = vp->v_dirtyblkhd.lh_first; bp; bp = nbp) {
		nbp = bp->b_vnbufs.le_next;
		if (ISSET(bp->b_flags, B_BUSY)) {
			FSDBG(524, bp, waitfor, passone, bp->b_flags);
			if (waitfor != MNT_WAIT || passone)
				continue;
			SET(bp->b_flags, B_WANTED);
			error = tsleep((caddr_t)bp, slpflag | (PRIBIO + 1),
				       "nfsfsync", slptimeo);
			splx(s);
			if (error) {
				if (nfs_sigintr(nmp, (struct nfsreq *)0, p)) {
					error = EINTR;
					goto done;
				}
				if (slpflag == PCATCH) {
					slpflag = 0;
					slptimeo = 2 * hz;
				}
			}
			goto loop;
		}
		if (!ISSET(bp->b_flags, B_DELWRI))
			panic("nfs_fsync: not dirty");
		FSDBG(525, bp, passone, commit, bp->b_flags);
		if ((passone || !commit) && ISSET(bp->b_flags, B_NEEDCOMMIT))
			continue;
		bremfree(bp);
		if (passone || !commit)
			SET(bp->b_flags, B_BUSY|B_ASYNC);
		else
			SET(bp->b_flags,
			    B_BUSY|B_ASYNC|B_WRITEINPROG|B_NEEDCOMMIT);
		splx(s);
		VOP_BWRITE(bp);
		goto loop;
	}
	splx(s);
	if (passone) {
		passone = 0;
		goto again;
	}
	if (waitfor == MNT_WAIT) {
		while (vp->v_numoutput) {
			vp->v_flag |= VBWAIT;
			error = tsleep((caddr_t)&vp->v_numoutput,
				slpflag | (PRIBIO + 1), "nfsfsync", slptimeo);
			if (error) {
			    if (nfs_sigintr(nmp, (struct nfsreq *)0, p)) {
				error = EINTR;
				goto done;
			    }
			    if (slpflag == PCATCH) {
				slpflag = 0;
				slptimeo = 2 * hz;
			    }
>>>>>>> origin/10.1
			}
			nfs_buf_drop(bp);
		}
		if (found) {
			LIST_REMOVE(foundbp, nb_vnbufs);
			LIST_INSERT_HEAD(&dnp->n_cleanblkhd, foundbp, nb_vnbufs);
			dnp->n_lastdbl = foundbp->nb_lblkno;
		}
<<<<<<< HEAD
		nfs_buf_itercomplete(dnp, &blist, NBI_CLEAN);
	}
done:
	lck_mtx_unlock(nfs_buf_mutex);

	if (!error && found && !purge) {
		error = nfs_nget(NFSTOMP(dnp), dnp, cnp, fh.fh_data, fh.fh_len,
				&nvattr, &xid, dnp->n_auth, NG_MAKEENTRY, &newnp);
		if (error)
			return (error);
		newnp->n_attrstamp = attrstamp;
		*npp = newnp;
		nfs_node_unlock(newnp);
		/* check if the dir buffer's attrs are out of date */
		if (!nfs_getattr(newnp, &nvattr, ctx, NGA_CACHED) &&
		    (newnp->n_attrstamp != attrstamp)) {
			/* they are, so update them */
			error = nfs_buf_get(dnp, lbn, NFS_DIRBLKSIZ, thd, NBLK_READ|NBLK_ONLYVALID, &bp);
			if (!error && bp) {
				attrstamp = newnp->n_attrstamp;
				xid = newnp->n_xid;
				nfs_dir_buf_search(bp, cnp, &fh, &nvattr, &xid, &attrstamp, NULL, NDBS_UPDATE);
				nfs_buf_release(bp, 0);
			}
			error = 0;
		}
	}

=======
	}
	FSDBG(526, np->n_flag, np->n_error, 0, 0);
	if (np->n_flag & NWRITEERR) {
		error = np->n_error;
		np->n_flag &= ~NWRITEERR;
	}
done:
	FSDBG_BOT(517, vp, np, error, 0);
	if (bvec != NULL && bvec != bvec_on_stack)
		_FREE(bvec, M_TEMP);
>>>>>>> origin/10.1
	return (error);
}

/*
 * Purge name cache entries for the given node.
 * For RDIRPLUS, also invalidate the entry in the directory's buffers.
 */
void
nfs_name_cache_purge(nfsnode_t dnp, nfsnode_t np, struct componentname *cnp, vfs_context_t ctx)
{
	struct nfsmount *nmp = NFSTONMP(dnp);

	cache_purge(NFSTOV(np));
	if (nmp && (nmp->nm_vers > NFS_VER2) && NMFLAG(nmp, RDIRPLUS))
		nfs_dir_buf_cache_lookup(dnp, NULL, cnp, ctx, 1);
}

/*
<<<<<<< HEAD
 * NFS V3 readdir (plus) RPC.
 */
=======
 * fsync vnode op. Just call nfs_flush().
 */
/* ARGSUSED */
static int
nfs_fsync(ap)
	struct vop_fsync_args /* {
		struct vnodeop_desc *a_desc;
		struct vnode * a_vp;
		struct ucred * a_cred;
		int  a_waitfor;
		struct proc * a_p;
	} */ *ap;
{
	return (nfs_flush(ap->a_vp, ap->a_cred, ap->a_waitfor, ap->a_p));
}
 
>>>>>>> origin/10.3
int
nfs3_readdir_rpc(nfsnode_t dnp, struct nfsbuf *bp, vfs_context_t ctx)
{
	struct nfsmount *nmp;
	int error = 0, lockerror, nfsvers, rdirplus, bigcookies;
	int i, status, attrflag, fhflag, more_entries = 1, eof, bp_dropped = 0;
	uint32_t nmreaddirsize, nmrsize;
	uint32_t namlen, skiplen, fhlen, xlen, attrlen, reclen, space_free, space_needed;
	uint64_t cookie, lastcookie, xid, savedxid, fileno;
	struct nfsm_chain nmreq, nmrep, nmrepsave;
	fhandle_t fh;
	struct nfs_vattr *nvattrp;
	struct nfs_dir_buf_header *ndbhp;
	struct direntry *dp;
	char *padstart, padlen;
	struct timeval now;

	nmp = NFSTONMP(dnp);
	if (nfs_mount_gone(nmp))
		return (ENXIO);
	nfsvers = nmp->nm_vers;
	nmreaddirsize = nmp->nm_readdirsize;
	nmrsize = nmp->nm_rsize;
	bigcookies = nmp->nm_state & NFSSTA_BIGCOOKIES;
noplus:
	rdirplus = ((nfsvers > NFS_VER2) && NMFLAG(nmp, RDIRPLUS)) ? 1 : 0;

	if ((lockerror = nfs_node_lock(dnp)))
		return (lockerror);

	/* determine cookie to use, and move dp to the right offset */
	ndbhp = (struct nfs_dir_buf_header*)bp->nb_data;
	dp = NFS_DIR_BUF_FIRST_DIRENTRY(bp);
	if (ndbhp->ndbh_count) {
		for (i=0; i < ndbhp->ndbh_count-1; i++)
			dp = NFS_DIRENTRY_NEXT(dp);
		cookie = dp->d_seekoff;
		dp = NFS_DIRENTRY_NEXT(dp);
	} else {
		cookie = bp->nb_lblkno;
		/* increment with every buffer read */
		OSAddAtomic64(1, &nfsstats.readdir_bios);
	}
	lastcookie = cookie;

	/*
	 * Loop around doing readdir(plus) RPCs of size nm_readdirsize until
	 * the buffer is full (or we hit EOF).  Then put the remainder of the
	 * results in the next buffer(s).
	 */
	nfsm_chain_null(&nmreq);
	nfsm_chain_null(&nmrep);
	while (nfs_dir_buf_freespace(bp, rdirplus) && !(ndbhp->ndbh_flags & NDB_FULL)) {
		nfsm_chain_build_alloc_init(error, &nmreq,
			NFSX_FH(nfsvers) + NFSX_READDIR(nfsvers) + NFSX_UNSIGNED);
		nfsm_chain_add_fh(error, &nmreq, nfsvers, dnp->n_fhp, dnp->n_fhsize);
		if (nfsvers == NFS_VER3) {
			/* opaque values don't need swapping, but as long */
			/* as we are consistent about it, it should be ok */
			nfsm_chain_add_64(error, &nmreq, cookie);
			nfsm_chain_add_64(error, &nmreq, dnp->n_cookieverf);
		} else {
			nfsm_chain_add_32(error, &nmreq, cookie);
		}
		nfsm_chain_add_32(error, &nmreq, nmreaddirsize);
		if (rdirplus)
			nfsm_chain_add_32(error, &nmreq, nmrsize);
		nfsm_chain_build_done(error, &nmreq);
		nfs_node_unlock(dnp);
		lockerror = ENOENT;
		nfsmout_if(error);

		error = nfs_request(dnp, NULL, &nmreq,
				rdirplus ? NFSPROC_READDIRPLUS : NFSPROC_READDIR,
				ctx, NULL, &nmrep, &xid, &status);

		if ((lockerror = nfs_node_lock(dnp)))
			error = lockerror;

		savedxid = xid;
		if (nfsvers == NFS_VER3)
			nfsm_chain_postop_attr_update(error, &nmrep, dnp, &xid);
		if (!error)
			error = status;
		if (nfsvers == NFS_VER3)
			nfsm_chain_get_64(error, &nmrep, dnp->n_cookieverf);
		nfsm_chain_get_32(error, &nmrep, more_entries);

		if (!lockerror) {
			nfs_node_unlock(dnp);
			lockerror = ENOENT;
		}
		if (error == NFSERR_NOTSUPP) {
			/* oops... it doesn't look like readdirplus is supported */
			lck_mtx_lock(&nmp->nm_lock);
			NFS_BITMAP_CLR(nmp->nm_flags, NFS_MFLAG_RDIRPLUS);
			lck_mtx_unlock(&nmp->nm_lock);
			goto noplus;
		}
		nfsmout_if(error);

		if (rdirplus)
			microuptime(&now);

		/* loop through the entries packing them into the buffer */
		while (more_entries) {
			if (nfsvers == NFS_VER3)
				nfsm_chain_get_64(error, &nmrep, fileno);
			else
				nfsm_chain_get_32(error, &nmrep, fileno);
			nfsm_chain_get_32(error, &nmrep, namlen);
			nfsmout_if(error);
			/* just truncate names that don't fit in direntry.d_name */
			if (namlen <= 0) {
				error = EBADRPC;
				goto nfsmout;
			}
			if (namlen > (sizeof(dp->d_name)-1)) {
				skiplen = namlen - sizeof(dp->d_name) + 1;
				namlen = sizeof(dp->d_name) - 1;
			} else {
				skiplen = 0;
			}
			/* guess that fh size will be same as parent */
			fhlen = rdirplus ? (1 + dnp->n_fhsize) : 0;
			xlen = rdirplus ? (fhlen + sizeof(time_t)) : 0;
			attrlen = rdirplus ? sizeof(struct nfs_vattr) : 0;
			reclen = NFS_DIRENTRY_LEN(namlen + xlen);
			space_needed = reclen + attrlen;
			space_free = nfs_dir_buf_freespace(bp, rdirplus);
			if (space_needed > space_free) {
				/*
				 * We still have entries to pack, but we've
				 * run out of room in the current buffer.
				 * So we need to move to the next buffer.
				 * The block# for the next buffer is the
				 * last cookie in the current buffer.
				 */
nextbuffer:
				ndbhp->ndbh_flags |= NDB_FULL;
				nfs_buf_release(bp, 0);
				bp_dropped = 1;
				bp = NULL;
				error = nfs_buf_get(dnp, lastcookie, NFS_DIRBLKSIZ, vfs_context_thread(ctx), NBLK_READ, &bp);
				nfsmout_if(error);
				/* initialize buffer */
				ndbhp = (struct nfs_dir_buf_header*)bp->nb_data;
				ndbhp->ndbh_flags = 0;
				ndbhp->ndbh_count = 0;
				ndbhp->ndbh_entry_end = sizeof(*ndbhp);
				ndbhp->ndbh_ncgen = dnp->n_ncgen;
				space_free = nfs_dir_buf_freespace(bp, rdirplus);
				dp = NFS_DIR_BUF_FIRST_DIRENTRY(bp);
				/* increment with every buffer read */
				OSAddAtomic64(1, &nfsstats.readdir_bios);
			}
			nmrepsave = nmrep;
			dp->d_fileno = fileno;
			dp->d_namlen = namlen;
			dp->d_reclen = reclen;
			dp->d_type = DT_UNKNOWN;
			nfsm_chain_get_opaque(error, &nmrep, namlen, dp->d_name);
			nfsmout_if(error);
			dp->d_name[namlen] = '\0';
			if (skiplen)
				nfsm_chain_adv(error, &nmrep,
					nfsm_rndup(namlen + skiplen) - nfsm_rndup(namlen));
			if (nfsvers == NFS_VER3)
				nfsm_chain_get_64(error, &nmrep, cookie);
			else
				nfsm_chain_get_32(error, &nmrep, cookie);
			nfsmout_if(error);
			dp->d_seekoff = cookie;
			if (!bigcookies && (cookie >> 32) && (nmp == NFSTONMP(dnp))) {
				/* we've got a big cookie, make sure flag is set */
				lck_mtx_lock(&nmp->nm_lock);
				nmp->nm_state |= NFSSTA_BIGCOOKIES;
				lck_mtx_unlock(&nmp->nm_lock);
				bigcookies = 1;
			}
			if (rdirplus) {
				nvattrp = NFS_DIR_BUF_NVATTR(bp, ndbhp->ndbh_count);
				/* check for attributes */
				nfsm_chain_get_32(error, &nmrep, attrflag);
				nfsmout_if(error);
				if (attrflag) {
					/* grab attributes */
					error = nfs_parsefattr(&nmrep, NFS_VER3, nvattrp);
					nfsmout_if(error);
					dp->d_type = IFTODT(VTTOIF(nvattrp->nva_type));
					/* fileid is already in d_fileno, so stash xid in attrs */
					nvattrp->nva_fileid = savedxid;
				} else {
					/* mark the attributes invalid */
					bzero(nvattrp, sizeof(struct nfs_vattr));
				}
				/* check for file handle */
				nfsm_chain_get_32(error, &nmrep, fhflag);
				nfsmout_if(error);
				if (fhflag) {
					nfsm_chain_get_fh(error, &nmrep, NFS_VER3, &fh);
					nfsmout_if(error);
					fhlen = fh.fh_len + 1;
					xlen = fhlen + sizeof(time_t);
					reclen = NFS_DIRENTRY_LEN(namlen + xlen);
					space_needed = reclen + attrlen;
					if (space_needed > space_free) {
						/* didn't actually have the room... move on to next buffer */
						nmrep = nmrepsave;
						goto nextbuffer;
					}
					/* pack the file handle into the record */
					dp->d_name[dp->d_namlen+1] = fh.fh_len;
					bcopy(fh.fh_data, &dp->d_name[dp->d_namlen+2], fh.fh_len);
				} else {
					/* mark the file handle invalid */
					fh.fh_len = 0;
					fhlen = fh.fh_len + 1;
					xlen = fhlen + sizeof(time_t);
					reclen = NFS_DIRENTRY_LEN(namlen + xlen);
					bzero(&dp->d_name[dp->d_namlen+1], fhlen);
				}
				*(time_t*)(&dp->d_name[dp->d_namlen+1+fhlen]) = now.tv_sec;
				dp->d_reclen = reclen;
			}
			padstart = dp->d_name + dp->d_namlen + 1 + xlen;
			ndbhp->ndbh_count++;
			lastcookie = cookie;
			/* advance to next direntry in buffer */
			dp = NFS_DIRENTRY_NEXT(dp);
			ndbhp->ndbh_entry_end = (char*)dp - bp->nb_data;
			/* zero out the pad bytes */
			padlen = (char*)dp - padstart;
			if (padlen > 0)
				bzero(padstart, padlen);
			/* check for more entries */
			nfsm_chain_get_32(error, &nmrep, more_entries);
			nfsmout_if(error);
		}
		/* Finally, get the eof boolean */
		nfsm_chain_get_32(error, &nmrep, eof);
		nfsmout_if(error);
		if (eof) {
			ndbhp->ndbh_flags |= (NDB_FULL|NDB_EOF);
			nfs_node_lock_force(dnp);
			dnp->n_eofcookie = lastcookie;
			nfs_node_unlock(dnp);
		} else {
			more_entries = 1;
		}
		if (bp_dropped) {
			nfs_buf_release(bp, 0);
			bp = NULL;
			break;
		}
		if ((lockerror = nfs_node_lock(dnp)))
			error = lockerror;
		nfsmout_if(error);
		nfsm_chain_cleanup(&nmrep);
		nfsm_chain_null(&nmreq);
	}
nfsmout:
	if (bp_dropped && bp)
		nfs_buf_release(bp, 0);
	if (!lockerror)
		nfs_node_unlock(dnp);
	nfsm_chain_cleanup(&nmreq);
	nfsm_chain_cleanup(&nmrep);
	return (bp_dropped ? NFSERR_DIRBUFDROPPED : error);
}

/*
 * Silly rename. To make the NFS filesystem that is stateless look a little
 * more like the "ufs" a remove of an active vnode is translated to a rename
 * to a funny looking filename that is removed by nfs_vnop_inactive on the
 * nfsnode. There is the potential for another process on a different client
 * to create the same funny name between when the lookitup() fails and the
 * rename() completes, but...
 */

/* format of "random" silly names - includes a number and pid */
/* (note: shouldn't exceed size of nfs_sillyrename.nsr_name) */
#define NFS_SILLYNAME_FORMAT ".nfs.%08x.%04x"
/* starting from zero isn't silly enough */
static uint32_t nfs_sillyrename_number = 0x20051025;

int
nfs_sillyrename(
	nfsnode_t dnp,
	nfsnode_t np,
	struct componentname *cnp,
	vfs_context_t ctx)
{
	struct nfs_sillyrename *nsp;
	int error;
	short pid;
	kauth_cred_t cred;
	uint32_t num;
	struct nfsmount *nmp;

	nmp = NFSTONMP(dnp);
	if (nfs_mount_gone(nmp))
		return (ENXIO);

	nfs_name_cache_purge(dnp, np, cnp, ctx);

	MALLOC_ZONE(nsp, struct nfs_sillyrename *,
			sizeof (struct nfs_sillyrename), M_NFSREQ, M_WAITOK);
	if (!nsp)
		return (ENOMEM);
	cred = vfs_context_ucred(ctx);
	kauth_cred_ref(cred);
	nsp->nsr_cred = cred;
	nsp->nsr_dnp = dnp;
	error = vnode_ref(NFSTOV(dnp));
	if (error)
		goto bad_norele;

	/* Fudge together a funny name */
	pid = vfs_context_pid(ctx);
	num = OSAddAtomic(1, &nfs_sillyrename_number);
	nsp->nsr_namlen = snprintf(nsp->nsr_name, sizeof(nsp->nsr_name),
				NFS_SILLYNAME_FORMAT, num, (pid & 0xffff));
	if (nsp->nsr_namlen >= (int)sizeof(nsp->nsr_name))
		nsp->nsr_namlen = sizeof(nsp->nsr_name) - 1;

	/* Try lookitups until we get one that isn't there */
	while (nfs_lookitup(dnp, nsp->nsr_name, nsp->nsr_namlen, ctx, NULL) == 0) {
		num = OSAddAtomic(1, &nfs_sillyrename_number);
		nsp->nsr_namlen = snprintf(nsp->nsr_name, sizeof(nsp->nsr_name),
					NFS_SILLYNAME_FORMAT, num, (pid & 0xffff));
		if (nsp->nsr_namlen >= (int)sizeof(nsp->nsr_name))
			nsp->nsr_namlen = sizeof(nsp->nsr_name) - 1;
	}

	/* now, do the rename */
	error = nmp->nm_funcs->nf_rename_rpc(dnp, cnp->cn_nameptr, cnp->cn_namelen,
					dnp, nsp->nsr_name, nsp->nsr_namlen, ctx);

	/* Kludge: Map ENOENT => 0 assuming that it is a reply to a retry. */
	if (error == ENOENT)
		error = 0;
	if (!error) {
		nfs_node_lock_force(dnp);
		if (dnp->n_flag & NNEGNCENTRIES) {
			dnp->n_flag &= ~NNEGNCENTRIES;
			cache_purge_negatives(NFSTOV(dnp));
		}
		nfs_node_unlock(dnp);
	}
	FSDBG(267, dnp, np, num, error);
	if (error)
		goto bad;
	error = nfs_lookitup(dnp, nsp->nsr_name, nsp->nsr_namlen, ctx, &np);
	nfs_node_lock_force(np);
	np->n_sillyrename = nsp;
	nfs_node_unlock(np);
	return (0);
bad:
	vnode_rele(NFSTOV(dnp));
bad_norele:
	nsp->nsr_cred = NOCRED;
	kauth_cred_unref(&cred);
	FREE_ZONE(nsp, sizeof(*nsp), M_NFSREQ);
	return (error);
}

int
nfs3_lookup_rpc_async(
	nfsnode_t dnp,
	char *name,
	int namelen,
	vfs_context_t ctx,
	struct nfsreq **reqp)
{
	struct nfsmount *nmp;
	struct nfsm_chain nmreq;
	int error = 0, nfsvers;

	nmp = NFSTONMP(dnp);
	if (nfs_mount_gone(nmp))
		return (ENXIO);
	nfsvers = nmp->nm_vers;

	nfsm_chain_null(&nmreq);

	nfsm_chain_build_alloc_init(error, &nmreq,
		NFSX_FH(nfsvers) + NFSX_UNSIGNED + nfsm_rndup(namelen));
	nfsm_chain_add_fh(error, &nmreq, nfsvers, dnp->n_fhp, dnp->n_fhsize);
	nfsm_chain_add_name(error, &nmreq, name, namelen, nmp);
	nfsm_chain_build_done(error, &nmreq);
	nfsmout_if(error);
	error = nfs_request_async(dnp, NULL, &nmreq, NFSPROC_LOOKUP,
			vfs_context_thread(ctx), vfs_context_ucred(ctx), NULL, 0, NULL, reqp);
nfsmout:
	nfsm_chain_cleanup(&nmreq);
	return (error);
}

int
nfs3_lookup_rpc_async_finish(
	nfsnode_t dnp,
	__unused char *name,
	__unused int namelen,
	vfs_context_t ctx,
	struct nfsreq *req,
	u_int64_t *xidp,
	fhandle_t *fhp,
	struct nfs_vattr *nvap)
{
	int error = 0, lockerror = ENOENT, status, nfsvers, attrflag;
	u_int64_t xid;
	struct nfsmount *nmp;
	struct nfsm_chain nmrep;

	nmp = NFSTONMP(dnp);
	if (nmp == NULL)
		return (ENXIO);
	nfsvers = nmp->nm_vers;

<<<<<<< HEAD
	nfsm_chain_null(&nmrep);
=======
		if (retv) {
			nfs_buf_release(bp, 1);
		} else {
			s = splbio();
			vp->v_numoutput++;
>>>>>>> origin/10.3

	error = nfs_request_async_finish(req, &nmrep, xidp, &status);

	if ((lockerror = nfs_node_lock(dnp)))
		error = lockerror;
	xid = *xidp;
	if (error || status) {
		if (nfsvers == NFS_VER3)
			nfsm_chain_postop_attr_update(error, &nmrep, dnp, &xid);
		if (!error)
			error = status;
		goto nfsmout;
	}

	nfsmout_if(error || !fhp || !nvap);

	/* get the file handle */
	nfsm_chain_get_fh(error, &nmrep, nfsvers, fhp);

	/* get the attributes */
	if (nfsvers == NFS_VER3) {
		nfsm_chain_postop_attr_get(error, &nmrep, attrflag, nvap);
		nfsm_chain_postop_attr_update(error, &nmrep, dnp, &xid);
		if (!error && !attrflag)
			error = nfs3_getattr_rpc(NULL, NFSTOMP(dnp), fhp->fh_data, fhp->fh_len, 0, ctx, nvap, xidp);
	} else {
		error = nfs_parsefattr(&nmrep, nfsvers, nvap);
	}
nfsmout:
	if (!lockerror)
		nfs_node_unlock(dnp);
	nfsm_chain_cleanup(&nmrep);
	return (error);
}

/*
 * Look up a file name and optionally either update the file handle or
 * allocate an nfsnode, depending on the value of npp.
 * npp == NULL	--> just do the lookup
 * *npp == NULL --> allocate a new nfsnode and make sure attributes are
 *			handled too
 * *npp != NULL --> update the file handle in the vnode
 */
<<<<<<< HEAD
int
nfs_lookitup(
	nfsnode_t dnp,
	char *name,
	int namelen,
	vfs_context_t ctx,
	nfsnode_t *npp)
=======
static int
nfs_flush(vp, cred, waitfor, p)
	register struct vnode *vp;
	struct ucred *cred;
	int waitfor;
	struct proc *p;
>>>>>>> origin/10.3
{
	int error = 0;
	nfsnode_t np, newnp = NULL;
	u_int64_t xid;
	fhandle_t fh;
	struct nfsmount *nmp;
	struct nfs_vattr nvattr;
	struct nfsreq rq, *req = &rq;

<<<<<<< HEAD
	nmp = NFSTONMP(dnp);
	if (nfs_mount_gone(nmp))
		return (ENXIO);

	if (NFS_BITMAP_ISSET(nmp->nm_fsattr.nfsa_bitmap, NFS_FATTR_MAXNAME) &&
	    (namelen > (int)nmp->nm_fsattr.nfsa_maxname))
		return (ENAMETOOLONG);

	NVATTR_INIT(&nvattr);

	/* check for lookup of "." */
	if ((name[0] == '.') && (namelen == 1)) {
		/* skip lookup, we know who we are */
		fh.fh_len = 0;
		newnp = dnp;
		goto nfsmout;
=======
	FSDBG_TOP(517, vp, np, waitfor, 0);

	if (!nmp) {
		error = ENXIO;
		goto done;
	}
	if (nmp->nm_flag & NFSMNT_INT)
		slpflag = PCATCH;

	/*
	 * On the first pass, start async/unstable writes on all
	 * delayed write buffers.  Then wait for all writes to complete
	 * and call nfs_flushcommits() to commit any uncommitted buffers.
	 * On all subsequent passes, start STABLE writes on any remaining
	 * dirty buffers.  Then wait for all writes to complete.
	 */
again:
	FSDBG(518, np->n_dirtyblkhd.lh_first, np->n_flag, 0, 0);
	if (np->n_dirtyblkhd.lh_first)
		np->n_flag |= NMODIFIED;
	if (!VFSTONFS(vp->v_mount)) {
		error = ENXIO;
		goto done;
>>>>>>> origin/10.3
	}

	error = nmp->nm_funcs->nf_lookup_rpc_async(dnp, name, namelen, ctx, &req);
	nfsmout_if(error);
	error = nmp->nm_funcs->nf_lookup_rpc_async_finish(dnp, name, namelen, ctx, req, &xid, &fh, &nvattr);
	nfsmout_if(!npp || error);

	if (*npp) {
		np = *npp;
		if (fh.fh_len != np->n_fhsize) {
			u_char *oldbuf = (np->n_fhsize > NFS_SMALLFH) ? np->n_fhp : NULL;
			if (fh.fh_len > NFS_SMALLFH) {
				MALLOC_ZONE(np->n_fhp, u_char *, fh.fh_len, M_NFSBIGFH, M_WAITOK);
				if (!np->n_fhp) {
				    np->n_fhp = oldbuf;
				    error = ENOMEM;
				    goto nfsmout;
				}
			} else {
				np->n_fhp = &np->n_fh[0];
			}
<<<<<<< HEAD
			if (oldbuf)
				FREE_ZONE(oldbuf, np->n_fhsize, M_NFSBIGFH);
		}
		bcopy(fh.fh_data, np->n_fhp, fh.fh_len);
		np->n_fhsize = fh.fh_len;
		nfs_node_lock_force(np);
		error = nfs_loadattrcache(np, &nvattr, &xid, 0);
		nfs_node_unlock(np);
		nfsmout_if(error);
		newnp = np;
	} else if (NFS_CMPFH(dnp, fh.fh_data, fh.fh_len)) {
		nfs_node_lock_force(dnp);
		if (dnp->n_xid <= xid)
			error = nfs_loadattrcache(dnp, &nvattr, &xid, 0);
		nfs_node_unlock(dnp);
		nfsmout_if(error);
		newnp = dnp;
	} else {
		struct componentname cn, *cnp = &cn;
		bzero(cnp, sizeof(*cnp));
		cnp->cn_nameptr = name;
		cnp->cn_namelen = namelen;
		error = nfs_nget(NFSTOMP(dnp), dnp, cnp, fh.fh_data, fh.fh_len,
			    &nvattr, &xid, rq.r_auth, NG_MAKEENTRY, &np);
		nfsmout_if(error);
		newnp = np;
	}

nfsmout:
	if (npp && !*npp && !error)
		*npp = newnp;
	NVATTR_CLEANUP(&nvattr);
=======
			goto loop;
		}
		if (!ISSET(bp->nb_flags, NB_DELWRI))
			panic("nfs_fsync: not dirty");
		FSDBG(525, bp, passone, 0, bp->nb_flags);
		if ((passone || (waitfor != MNT_WAIT)) && ISSET(bp->nb_flags, NB_NEEDCOMMIT))
			continue;
		nfs_buf_remfree(bp);
		if (ISSET(bp->nb_flags, NB_ERROR)) {
			np->n_error = bp->nb_error ? bp->nb_error : EIO;
			np->n_flag |= NWRITEERR;
			nfs_buf_release(bp, 1);
			continue;
		}
		if (passone)
			SET(bp->nb_flags, NB_BUSY|NB_ASYNC);
		else {
			/* the NB_STABLE forces this to be written FILESYNC */
			SET(bp->nb_flags, NB_BUSY|NB_ASYNC|NB_STABLE);
		}
		splx(s);
		nfs_buf_write(bp);
		goto loop;
	}
	splx(s);

	if (waitfor == MNT_WAIT) {
		while (vp->v_numoutput) {
			vp->v_flag |= VBWAIT;
			error = tsleep((caddr_t)&vp->v_numoutput,
				slpflag | (PRIBIO + 1), "nfsfsync", slptimeo);
			if (error) {
				error2 = nfs_sigintr(VFSTONFS(vp->v_mount),
				    (struct nfsreq *)0, p);
			    if (error2) {
				error = error2;
				goto done;
			    }
			    if (slpflag == PCATCH) {
				slpflag = 0;
				slptimeo = 2 * hz;
			    }
			}
		}
	}

	if (NFS_ISV3(vp)) {
		/* loop while it looks like there are still buffers to be */
		/* commited and nfs_flushcommits() seems to be handling them. */
		while (np->n_needcommitcnt)
			if (nfs_flushcommits(vp, p))
				break;
	}

	if (passone) {
		passone = 0;
		goto again;
	}

	if (waitfor == MNT_WAIT) {
		if (np->n_dirtyblkhd.lh_first) {
			goto again;
		}
	}

	FSDBG(526, np->n_flag, np->n_error, 0, 0);
	if (np->n_flag & NWRITEERR) {
		error = np->n_error;
		np->n_flag &= ~NWRITEERR;
	}
done:
	FSDBG_BOT(517, vp, np, error, 0);
>>>>>>> origin/10.3
	return (error);
}

/*
 * set up and initialize a "._" file lookup structure used for
 * performing async lookups.
 */
void
nfs_dulookup_init(struct nfs_dulookup *dulp, nfsnode_t dnp, const char *name, int namelen, vfs_context_t ctx)
{
	int error, du_namelen;
	vnode_t du_vp;
	struct nfsmount *nmp = NFSTONMP(dnp);

	/* check for ._ file in name cache */
	dulp->du_flags = 0;
	bzero(&dulp->du_cn, sizeof(dulp->du_cn));
	du_namelen = namelen + 2;
	if (!nmp || NMFLAG(nmp, NONEGNAMECACHE))
		return;
	if ((namelen >= 2) && (name[0] == '.') && (name[1] == '_'))
		return;
	if (du_namelen >= (int)sizeof(dulp->du_smallname))
		MALLOC(dulp->du_cn.cn_nameptr, char *, du_namelen + 1, M_TEMP, M_WAITOK);
	else
		dulp->du_cn.cn_nameptr = dulp->du_smallname;
	if (!dulp->du_cn.cn_nameptr)
		return;
	dulp->du_cn.cn_namelen = du_namelen;
	snprintf(dulp->du_cn.cn_nameptr, du_namelen + 1, "._%s", name);
	dulp->du_cn.cn_nameptr[du_namelen] = '\0';
	dulp->du_cn.cn_nameiop = LOOKUP;
	dulp->du_cn.cn_flags = MAKEENTRY;

	error = cache_lookup(NFSTOV(dnp), &du_vp, &dulp->du_cn);
	if (error == -1) {
		vnode_put(du_vp);
	} else if (!error) {
		nmp = NFSTONMP(dnp);
		if (nmp && (nmp->nm_vers > NFS_VER2) && NMFLAG(nmp, RDIRPLUS)) {
			/* if rdirplus, try dir buf cache lookup */
			nfsnode_t du_np = NULL;
			if (!nfs_dir_buf_cache_lookup(dnp, &du_np, &dulp->du_cn, ctx, 0) && du_np) {
				/* dir buf cache hit */
				du_vp = NFSTOV(du_np);
				vnode_put(du_vp);
				error = -1;
			}
		}
		if (!error)
			dulp->du_flags |= NFS_DULOOKUP_DOIT;
	}
}

/*
 * start an async "._" file lookup request
 */
void
nfs_dulookup_start(struct nfs_dulookup *dulp, nfsnode_t dnp, vfs_context_t ctx)
{
	struct nfsmount *nmp = NFSTONMP(dnp);
	struct nfsreq *req = &dulp->du_req;

	if (!nmp || !(dulp->du_flags & NFS_DULOOKUP_DOIT) || (dulp->du_flags & NFS_DULOOKUP_INPROG))
		return;
	if (!nmp->nm_funcs->nf_lookup_rpc_async(dnp, dulp->du_cn.cn_nameptr,
			dulp->du_cn.cn_namelen, ctx, &req))
		dulp->du_flags |= NFS_DULOOKUP_INPROG;
}

/*
 * finish an async "._" file lookup request and clean up the structure
 */
void
nfs_dulookup_finish(struct nfs_dulookup *dulp, nfsnode_t dnp, vfs_context_t ctx)
{
	struct nfsmount *nmp = NFSTONMP(dnp);
	int error;
	nfsnode_t du_np;
	u_int64_t xid;
	fhandle_t fh;
	struct nfs_vattr nvattr;

	if (!nmp || !(dulp->du_flags & NFS_DULOOKUP_INPROG))
		goto out;

	NVATTR_INIT(&nvattr);
	error = nmp->nm_funcs->nf_lookup_rpc_async_finish(dnp, dulp->du_cn.cn_nameptr,
			dulp->du_cn.cn_namelen, ctx, &dulp->du_req, &xid, &fh, &nvattr);
	dulp->du_flags &= ~NFS_DULOOKUP_INPROG;
	if (error == ENOENT) {
		/* add a negative entry in the name cache */
		nfs_node_lock_force(dnp);
		cache_enter(NFSTOV(dnp), NULL, &dulp->du_cn);
		dnp->n_flag |= NNEGNCENTRIES;
		nfs_node_unlock(dnp);
	} else if (!error) {
		error = nfs_nget(NFSTOMP(dnp), dnp, &dulp->du_cn, fh.fh_data, fh.fh_len,
			    &nvattr, &xid, dulp->du_req.r_auth, NG_MAKEENTRY, &du_np);
		if (!error) {
			nfs_node_unlock(du_np);
			vnode_put(NFSTOV(du_np));
		}
	}
	NVATTR_CLEANUP(&nvattr);
out:
	if (dulp->du_flags & NFS_DULOOKUP_INPROG)
		nfs_request_async_cancel(&dulp->du_req);
	if (dulp->du_cn.cn_nameptr && (dulp->du_cn.cn_nameptr != dulp->du_smallname))
		FREE(dulp->du_cn.cn_nameptr, M_TEMP);
}


/*
 * NFS Version 3 commit RPC
 */
int
nfs3_commit_rpc(
	nfsnode_t np,
	uint64_t offset,
	uint64_t count,
	kauth_cred_t cred,
	uint64_t wverf)
{
	struct nfsmount *nmp;
	int error = 0, lockerror, status, wccpostattr = 0, nfsvers;
	struct timespec premtime = { 0, 0 };
	u_int64_t xid, newwverf;
	uint32_t count32;
	struct nfsm_chain nmreq, nmrep;

	nmp = NFSTONMP(np);
	FSDBG(521, np, offset, count, nmp ? nmp->nm_state : 0);
	if (nfs_mount_gone(nmp))
		return (ENXIO);
	if (!(nmp->nm_state & NFSSTA_HASWRITEVERF))
		return (0);
	nfsvers = nmp->nm_vers;

	if (count > UINT32_MAX)
		count32 = 0;
	else
		count32 = count;

	nfsm_chain_null(&nmreq);
	nfsm_chain_null(&nmrep);

	nfsm_chain_build_alloc_init(error, &nmreq, NFSX_FH(NFS_VER3));
	nfsm_chain_add_fh(error, &nmreq, nfsvers, np->n_fhp, np->n_fhsize);
	nfsm_chain_add_64(error, &nmreq, offset);
	nfsm_chain_add_32(error, &nmreq, count32);
	nfsm_chain_build_done(error, &nmreq);
	nfsmout_if(error);
	error = nfs_request2(np, NULL, &nmreq, NFSPROC_COMMIT,
			current_thread(), cred, NULL, 0, &nmrep, &xid, &status);
	if ((lockerror = nfs_node_lock(np)))
		error = lockerror;
	/* can we do anything useful with the wcc info? */
	nfsm_chain_get_wcc_data(error, &nmrep, np, &premtime, &wccpostattr, &xid);
	if (!lockerror)
		nfs_node_unlock(np);
	if (!error)
		error = status;
	nfsm_chain_get_64(error, &nmrep, newwverf);
	nfsmout_if(error);
	lck_mtx_lock(&nmp->nm_lock);
	if (nmp->nm_verf != newwverf)
		nmp->nm_verf = newwverf;
	if (wverf != newwverf)
		error = NFSERR_STALEWRITEVERF;
	lck_mtx_unlock(&nmp->nm_lock);
nfsmout:
	nfsm_chain_cleanup(&nmreq);
	nfsm_chain_cleanup(&nmrep);
	return (error);
}


int
nfs_vnop_blockmap(
	__unused struct vnop_blockmap_args /* {
		struct vnodeop_desc *a_desc;
		vnode_t a_vp;
		off_t a_foffset;
		size_t a_size;
		daddr64_t *a_bpn;
		size_t *a_run;
		void *a_poff;
		int a_flags;
	} */ *ap)
{
	return (ENOTSUP);
}


/*
 * fsync vnode op. Just call nfs_flush().
 */
/* ARGSUSED */
int
nfs_vnop_fsync(
	struct vnop_fsync_args /* {
		struct vnodeop_desc *a_desc;
		vnode_t a_vp;
		int a_waitfor;
		vfs_context_t a_context;
	} */ *ap)
{
	return (nfs_flush(VTONFS(ap->a_vp), ap->a_waitfor, vfs_context_thread(ap->a_context), 0));
}


/*
 * Do an NFS pathconf RPC.
 */
int
nfs3_pathconf_rpc(
	nfsnode_t np,
	struct nfs_fsattr *nfsap,
	vfs_context_t ctx)
{
	u_int64_t xid;
	int error = 0, lockerror, status, nfsvers;
	struct nfsm_chain nmreq, nmrep;
	struct nfsmount *nmp = NFSTONMP(np);
	uint32_t val = 0;

	if (nfs_mount_gone(nmp))
		return (ENXIO);
	nfsvers = nmp->nm_vers;

	nfsm_chain_null(&nmreq);
	nfsm_chain_null(&nmrep);

	/* fetch pathconf info from server */
	nfsm_chain_build_alloc_init(error, &nmreq, NFSX_FH(NFS_VER3));
	nfsm_chain_add_fh(error, &nmreq, nfsvers, np->n_fhp, np->n_fhsize);
	nfsm_chain_build_done(error, &nmreq);
	nfsmout_if(error);
	error = nfs_request(np, NULL, &nmreq, NFSPROC_PATHCONF, ctx, NULL, &nmrep, &xid, &status);
	if ((lockerror = nfs_node_lock(np)))
		error = lockerror;
	nfsm_chain_postop_attr_update(error, &nmrep, np, &xid);
	if (!lockerror)
		nfs_node_unlock(np);
	if (!error)
		error = status;
	nfsm_chain_get_32(error, &nmrep, nfsap->nfsa_maxlink);
	nfsm_chain_get_32(error, &nmrep, nfsap->nfsa_maxname);
	nfsap->nfsa_flags &= ~(NFS_FSFLAG_NO_TRUNC|NFS_FSFLAG_CHOWN_RESTRICTED|NFS_FSFLAG_CASE_INSENSITIVE|NFS_FSFLAG_CASE_PRESERVING);
	nfsm_chain_get_32(error, &nmrep, val);
	if (val)
		nfsap->nfsa_flags |= NFS_FSFLAG_NO_TRUNC;
	nfsm_chain_get_32(error, &nmrep, val);
	if (val)
		nfsap->nfsa_flags |= NFS_FSFLAG_CHOWN_RESTRICTED;
	nfsm_chain_get_32(error, &nmrep, val);
	if (val)
		nfsap->nfsa_flags |= NFS_FSFLAG_CASE_INSENSITIVE;
	nfsm_chain_get_32(error, &nmrep, val);
	if (val)
		nfsap->nfsa_flags |= NFS_FSFLAG_CASE_PRESERVING;
	NFS_BITMAP_SET(nfsap->nfsa_bitmap, NFS_FATTR_MAXLINK);
	NFS_BITMAP_SET(nfsap->nfsa_bitmap, NFS_FATTR_MAXNAME);
	NFS_BITMAP_SET(nfsap->nfsa_bitmap, NFS_FATTR_NO_TRUNC);
	NFS_BITMAP_SET(nfsap->nfsa_bitmap, NFS_FATTR_CHOWN_RESTRICTED);
	NFS_BITMAP_SET(nfsap->nfsa_bitmap, NFS_FATTR_CASE_INSENSITIVE);
	NFS_BITMAP_SET(nfsap->nfsa_bitmap, NFS_FATTR_CASE_PRESERVING);
nfsmout:
	nfsm_chain_cleanup(&nmreq);
	nfsm_chain_cleanup(&nmrep);
	return (error);
}

/* save pathconf info for NFSv3 mount */
void
nfs3_pathconf_cache(struct nfsmount *nmp, struct nfs_fsattr *nfsap)
{
	nmp->nm_fsattr.nfsa_maxlink = nfsap->nfsa_maxlink;
	nmp->nm_fsattr.nfsa_maxname = nfsap->nfsa_maxname;
	nmp->nm_fsattr.nfsa_flags &= ~(NFS_FSFLAG_NO_TRUNC|NFS_FSFLAG_CHOWN_RESTRICTED|NFS_FSFLAG_CASE_INSENSITIVE|NFS_FSFLAG_CASE_PRESERVING);
	nmp->nm_fsattr.nfsa_flags |= nfsap->nfsa_flags & NFS_FSFLAG_NO_TRUNC;
	nmp->nm_fsattr.nfsa_flags |= nfsap->nfsa_flags & NFS_FSFLAG_CHOWN_RESTRICTED;
	nmp->nm_fsattr.nfsa_flags |= nfsap->nfsa_flags & NFS_FSFLAG_CASE_INSENSITIVE;
	nmp->nm_fsattr.nfsa_flags |= nfsap->nfsa_flags & NFS_FSFLAG_CASE_PRESERVING;
	NFS_BITMAP_SET(nmp->nm_fsattr.nfsa_bitmap, NFS_FATTR_MAXLINK);
	NFS_BITMAP_SET(nmp->nm_fsattr.nfsa_bitmap, NFS_FATTR_MAXNAME);
	NFS_BITMAP_SET(nmp->nm_fsattr.nfsa_bitmap, NFS_FATTR_NO_TRUNC);
	NFS_BITMAP_SET(nmp->nm_fsattr.nfsa_bitmap, NFS_FATTR_CHOWN_RESTRICTED);
	NFS_BITMAP_SET(nmp->nm_fsattr.nfsa_bitmap, NFS_FATTR_CASE_INSENSITIVE);
	NFS_BITMAP_SET(nmp->nm_fsattr.nfsa_bitmap, NFS_FATTR_CASE_PRESERVING);
	nmp->nm_state |= NFSSTA_GOTPATHCONF;
}

/*
 * Return POSIX pathconf information applicable to nfs.
 *
 * The NFS V2 protocol doesn't support this, so just return EINVAL
 * for V2.
 */
/* ARGSUSED */
int
nfs_vnop_pathconf(
	struct vnop_pathconf_args /* {
		struct vnodeop_desc *a_desc;
		vnode_t a_vp;
		int a_name;
		int32_t *a_retval;
		vfs_context_t a_context;
	} */ *ap)
{
<<<<<<< HEAD
	vnode_t vp = ap->a_vp;
	nfsnode_t np = VTONFS(vp);
	struct nfsmount *nmp;
	struct nfs_fsattr nfsa, *nfsap;
	int error = 0;
	uint64_t maxFileSize;
	uint nbits;

	nmp = VTONMP(vp);
	if (nfs_mount_gone(nmp))
		return (ENXIO);

	switch (ap->a_name) {
	case _PC_LINK_MAX:
	case _PC_NAME_MAX:
	case _PC_CHOWN_RESTRICTED:
	case _PC_NO_TRUNC:
	case _PC_CASE_SENSITIVE:
	case _PC_CASE_PRESERVING:
		break;
	case _PC_FILESIZEBITS:
		if (nmp->nm_vers == NFS_VER2) {
			*ap->a_retval = 32;
			return (0);
		}
		break;
	case _PC_XATTR_SIZE_BITS:
		/* Do we support xattrs natively? */
		if (nmp->nm_fsattr.nfsa_flags & NFS_FSFLAG_NAMED_ATTR)
			break;  /* Yes */
		/* No... so just return an error */
		/* FALLTHROUGH */
	default:
		/* don't bother contacting the server if we know the answer */
		return (EINVAL);
=======
	int s;
	register int oldflags = bp->b_flags, retv = 1;
	off_t off;
	upl_t upl;
	kern_return_t kret;
	struct vnode *vp = bp->b_vp;
	upl_page_info_t *pl;

	if(!ISSET(bp->b_flags, B_BUSY))
		panic("nfs_writebp: buffer is not busy???");

	s = splbio();
	CLR(bp->b_flags, (B_READ|B_DONE|B_ERROR|B_DELWRI));
	if (ISSET(oldflags, B_DELWRI)) {
		extern int nbdwrite;
		nbdwrite--;
	}

	if (ISSET(oldflags, (B_ASYNC|B_DELWRI))) {
		reassignbuf(bp, vp);
	}

	vp->v_numoutput++;
	current_proc()->p_stats->p_ru.ru_oublock++;
	splx(s);
        
        /* 
         * Since the B_BUSY flag is set, we need to lock the page before doing
         * nfs_commit.  Otherwise we may block and get a busy incore pages
         * during a vm pageout.  Move the existing code up before the commit.
         */
        if (!ISSET(bp->b_flags, B_META) && UBCISVALID(vp) &&
            !ISSET(bp->b_flags, B_PAGELIST)) {
		kret = ubc_create_upl(vp, ubc_blktooff(vp, bp->b_lblkno),
				      bp->b_bufsize, &upl, &pl, UPL_PRECIOUS);
		if (kret != KERN_SUCCESS)
			panic("nfs_writebp: ubc_create_upl %d", kret);
#ifdef UBC_DEBUG
		upl_ubc_alias_set(upl, current_act(), 2);
#endif /* UBC_DEBUG */
		s = splbio();
		bp->b_pagelist = upl;
		SET(bp->b_flags, B_PAGELIST);
		splx(s);

		kret = ubc_upl_map(upl, (vm_address_t *)&(bp->b_data));
		if (kret != KERN_SUCCESS)
			panic("nfs_writebp: ubc_upl_map %d", kret);
		if(bp->b_data == 0) 
			panic("nfs_writebp: ubc_upl_map mapped 0");
		if (!upl_page_present(pl, 0)) /* even more paranoia */
			panic("nfs_writebp: nopage");
>>>>>>> origin/10.1
	}

	if (nmp->nm_vers == NFS_VER2)
		return (EINVAL);

	lck_mtx_lock(&nmp->nm_lock);
	if (nmp->nm_vers == NFS_VER3) {
		if (!(nmp->nm_state & NFSSTA_GOTPATHCONF)) {
			/* no pathconf info cached */
			lck_mtx_unlock(&nmp->nm_lock);
			NFS_CLEAR_ATTRIBUTES(nfsa.nfsa_bitmap);
			error = nfs3_pathconf_rpc(np, &nfsa, ap->a_context);
			if (error)
				return (error);
			nmp = VTONMP(vp);
			if (nfs_mount_gone(nmp))
				return (ENXIO);
			lck_mtx_lock(&nmp->nm_lock);
			if (nmp->nm_fsattr.nfsa_flags & NFS_FSFLAG_HOMOGENEOUS) {
				/* all files have the same pathconf info, */
				/* so cache a copy of the results */
				nfs3_pathconf_cache(nmp, &nfsa);
			}
			nfsap = &nfsa;
		} else {
			nfsap = &nmp->nm_fsattr;
		}
<<<<<<< HEAD
	} else if (!(nmp->nm_fsattr.nfsa_flags & NFS_FSFLAG_HOMOGENEOUS)) {
		/* no pathconf info cached */
		lck_mtx_unlock(&nmp->nm_lock);
		NFS_CLEAR_ATTRIBUTES(nfsa.nfsa_bitmap);
		error = nfs4_pathconf_rpc(np, &nfsa, ap->a_context);
		if (error)
			return (error);
		nmp = VTONMP(vp);
		if (nfs_mount_gone(nmp))
			return (ENXIO);
		lck_mtx_lock(&nmp->nm_lock);
		nfsap = &nfsa;
	} else {
		nfsap = &nmp->nm_fsattr;
	}
=======
		FSDBG_BOT(553, bp, NBOFF(bp), bp->nb_flags, rv);
		nfs_buf_release(bp, 1);
		return (rv);
	} 

	FSDBG_BOT(553, bp, NBOFF(bp), bp->nb_flags, rv);
	return (rv);
}

/*
 * nfs special file access vnode op.
 * Essentially just get vattr and then imitate iaccess() since the device is
 * local to the client.
 */
static int
nfsspec_access(ap)
	struct vop_access_args /* {
		struct vnode *a_vp;
		int  a_mode;
		struct ucred *a_cred;
		struct proc *a_p;
	} */ *ap;
{
	register struct vattr *vap;
	register gid_t *gp;
	register struct ucred *cred = ap->a_cred;
	struct vnode *vp = ap->a_vp;
	mode_t mode = ap->a_mode;
	struct vattr vattr;
	register int i;
	int error;
>>>>>>> origin/10.3

	switch (ap->a_name) {
	case _PC_LINK_MAX:
		if (NFS_BITMAP_ISSET(nfsap->nfsa_bitmap, NFS_FATTR_MAXLINK))
			*ap->a_retval = nfsap->nfsa_maxlink;
		else if ((nmp->nm_vers == NFS_VER4) && NFS_BITMAP_ISSET(np->n_vattr.nva_bitmap, NFS_FATTR_MAXLINK))
			*ap->a_retval = np->n_vattr.nva_maxlink;
		else
			error = EINVAL;
		break;
	case _PC_NAME_MAX:
		if (NFS_BITMAP_ISSET(nfsap->nfsa_bitmap, NFS_FATTR_MAXNAME))
			*ap->a_retval = nfsap->nfsa_maxname;
		else
			error = EINVAL;
		break;
	case _PC_CHOWN_RESTRICTED:
		if (NFS_BITMAP_ISSET(nfsap->nfsa_bitmap, NFS_FATTR_CHOWN_RESTRICTED))
			*ap->a_retval = (nfsap->nfsa_flags & NFS_FSFLAG_CHOWN_RESTRICTED) ? 200112 /* _POSIX_CHOWN_RESTRICTED */ : 0;
		else
			error = EINVAL;
		break;
	case _PC_NO_TRUNC:
		if (NFS_BITMAP_ISSET(nfsap->nfsa_bitmap, NFS_FATTR_NO_TRUNC))
			*ap->a_retval = (nfsap->nfsa_flags & NFS_FSFLAG_NO_TRUNC) ? 200112 /* _POSIX_NO_TRUNC */ : 0;
		else
			error = EINVAL;
		break;
	case _PC_CASE_SENSITIVE:
		if (NFS_BITMAP_ISSET(nfsap->nfsa_bitmap, NFS_FATTR_CASE_INSENSITIVE))
			*ap->a_retval = (nfsap->nfsa_flags & NFS_FSFLAG_CASE_INSENSITIVE) ? 0 : 1;
		else
			error = EINVAL;
		break;
	case _PC_CASE_PRESERVING:
		if (NFS_BITMAP_ISSET(nfsap->nfsa_bitmap, NFS_FATTR_CASE_PRESERVING))
			*ap->a_retval = (nfsap->nfsa_flags & NFS_FSFLAG_CASE_PRESERVING) ? 1 : 0;
		else
			error = EINVAL;
		break;
	case _PC_XATTR_SIZE_BITS: /* same as file size bits if named attrs supported */
	case _PC_FILESIZEBITS:
		if (!NFS_BITMAP_ISSET(nfsap->nfsa_bitmap, NFS_FATTR_MAXFILESIZE)) {
			*ap->a_retval = 64;
			error = 0;
			break;
		}
		maxFileSize = nfsap->nfsa_maxfilesize;
		nbits = 1;
		if (maxFileSize & 0xffffffff00000000ULL) {
			nbits += 32;
			maxFileSize >>= 32;
		}
		if (maxFileSize & 0xffff0000) {
			nbits += 16;
			maxFileSize >>= 16;
		}
		if (maxFileSize & 0xff00) {
			nbits += 8;
			maxFileSize >>= 8;
		}
		if (maxFileSize & 0xf0) {
			nbits += 4;
			maxFileSize >>= 4;
		}
		if (maxFileSize & 0xc) {
			nbits += 2;
			maxFileSize >>= 2;
		}
		if (maxFileSize & 0x2) {
			nbits += 1;
		}
		*ap->a_retval = nbits;
		break;
	default:
		error = EINVAL;
	}

	lck_mtx_unlock(&nmp->nm_lock);

	return (error);
}

/*
 * Read wrapper for special devices.
 */
int
nfsspec_vnop_read(
	struct vnop_read_args /* {
		struct vnodeop_desc *a_desc;
		vnode_t a_vp;
		struct uio *a_uio;
		int a_ioflag;
		vfs_context_t a_context;
	} */ *ap)
{
	nfsnode_t np = VTONFS(ap->a_vp);
	struct timeval now;
	int error;

	/*
	 * Set access flag.
	 */
	if ((error = nfs_node_lock(np)))
		return (error);
	np->n_flag |= NACC;
	microtime(&now);
	np->n_atim.tv_sec = now.tv_sec;
	np->n_atim.tv_nsec = now.tv_usec * 1000;
	nfs_node_unlock(np);
	return (VOCALL(spec_vnodeop_p, VOFFSET(vnop_read), ap));
}

/*
 * Write wrapper for special devices.
 */
int
nfsspec_vnop_write(
	struct vnop_write_args /* {
		struct vnodeop_desc *a_desc;
		vnode_t a_vp;
		struct uio *a_uio;
		int a_ioflag;
		vfs_context_t a_context;
	} */ *ap)
{
	nfsnode_t np = VTONFS(ap->a_vp);
	struct timeval now;
	int error;

	/*
	 * Set update flag.
	 */
	if ((error = nfs_node_lock(np)))
		return (error);
	np->n_flag |= NUPD;
	microtime(&now);
	np->n_mtim.tv_sec = now.tv_sec;
	np->n_mtim.tv_nsec = now.tv_usec * 1000;
	nfs_node_unlock(np);
	return (VOCALL(spec_vnodeop_p, VOFFSET(vnop_write), ap));
}

/*
 * Close wrapper for special devices.
 *
 * Update the times on the nfsnode then do device close.
 */
int
nfsspec_vnop_close(
	struct vnop_close_args /* {
		struct vnodeop_desc *a_desc;
		vnode_t a_vp;
		int a_fflag;
		vfs_context_t a_context;
	} */ *ap)
{
	vnode_t vp = ap->a_vp;
	nfsnode_t np = VTONFS(vp);
	struct vnode_attr vattr;
	mount_t mp;
	int error;

	if ((error = nfs_node_lock(np)))
		return (error);
	if (np->n_flag & (NACC | NUPD)) {
		np->n_flag |= NCHG;
		if (!vnode_isinuse(vp, 0) && (mp = vnode_mount(vp)) && !vfs_isrdonly(mp)) {
			VATTR_INIT(&vattr);
			if (np->n_flag & NACC) {
				vattr.va_access_time = np->n_atim;
				VATTR_SET_ACTIVE(&vattr, va_access_time);
			}
			if (np->n_flag & NUPD) {
				vattr.va_modify_time = np->n_mtim;
				VATTR_SET_ACTIVE(&vattr, va_modify_time);
			}
			nfs_node_unlock(np);
			vnode_setattr(vp, &vattr, ap->a_context);
		} else {
			nfs_node_unlock(np);
		}
	} else {
		nfs_node_unlock(np);
	}
	return (VOCALL(spec_vnodeop_p, VOFFSET(vnop_close), ap));
}

#if FIFO
extern vnop_t **fifo_vnodeop_p;

/*
 * Read wrapper for fifos.
 */
int
nfsfifo_vnop_read(
	struct vnop_read_args /* {
		struct vnodeop_desc *a_desc;
		vnode_t a_vp;
		struct uio *a_uio;
		int a_ioflag;
		vfs_context_t a_context;
	} */ *ap)
{
	nfsnode_t np = VTONFS(ap->a_vp);
	struct timeval now;
	int error;

	/*
	 * Set access flag.
	 */
	if ((error = nfs_node_lock(np)))
		return (error);
	np->n_flag |= NACC;
	microtime(&now);
	np->n_atim.tv_sec = now.tv_sec;
	np->n_atim.tv_nsec = now.tv_usec * 1000;
	nfs_node_unlock(np);
	return (VOCALL(fifo_vnodeop_p, VOFFSET(vnop_read), ap));
}

/*
 * Write wrapper for fifos.
 */
int
nfsfifo_vnop_write(
	struct vnop_write_args /* {
		struct vnodeop_desc *a_desc;
		vnode_t a_vp;
		struct uio *a_uio;
		int a_ioflag;
		vfs_context_t a_context;
	} */ *ap)
{
	nfsnode_t np = VTONFS(ap->a_vp);
	struct timeval now;
	int error;

	/*
	 * Set update flag.
	 */
	if ((error = nfs_node_lock(np)))
		return (error);
	np->n_flag |= NUPD;
	microtime(&now);
	np->n_mtim.tv_sec = now.tv_sec;
	np->n_mtim.tv_nsec = now.tv_usec * 1000;
	nfs_node_unlock(np);
	return (VOCALL(fifo_vnodeop_p, VOFFSET(vnop_write), ap));
}

/*
 * Close wrapper for fifos.
 *
 * Update the times on the nfsnode then do fifo close.
 */
int
nfsfifo_vnop_close(
	struct vnop_close_args /* {
		struct vnodeop_desc *a_desc;
		vnode_t a_vp;
		int a_fflag;
		vfs_context_t a_context;
	} */ *ap)
{
	vnode_t vp = ap->a_vp;
	nfsnode_t np = VTONFS(vp);
	struct vnode_attr vattr;
	struct timeval now;
	mount_t mp;
	int error;

	if ((error = nfs_node_lock(np)))
		return (error);
	if (np->n_flag & (NACC | NUPD)) {
		microtime(&now);
		if (np->n_flag & NACC) {
			np->n_atim.tv_sec = now.tv_sec;
			np->n_atim.tv_nsec = now.tv_usec * 1000;
		}
		if (np->n_flag & NUPD) {
			np->n_mtim.tv_sec = now.tv_sec;
			np->n_mtim.tv_nsec = now.tv_usec * 1000;
		}
		np->n_flag |= NCHG;
		if (!vnode_isinuse(vp, 1) && (mp = vnode_mount(vp)) && !vfs_isrdonly(mp)) {
			VATTR_INIT(&vattr);
			if (np->n_flag & NACC) {
				vattr.va_access_time = np->n_atim;
				VATTR_SET_ACTIVE(&vattr, va_access_time);
			}
			if (np->n_flag & NUPD) {
				vattr.va_modify_time = np->n_mtim;
				VATTR_SET_ACTIVE(&vattr, va_modify_time);
			}
			nfs_node_unlock(np);
			vnode_setattr(vp, &vattr, ap->a_context);
		} else {
			nfs_node_unlock(np);
		}
	} else {
		nfs_node_unlock(np);
	}
	return (VOCALL(fifo_vnodeop_p, VOFFSET(vnop_close), ap));
}
#endif /* FIFO */

/*ARGSUSED*/
int
nfs_vnop_ioctl(
	struct vnop_ioctl_args /* {
		struct vnodeop_desc *a_desc;
		vnode_t a_vp;
		u_int32_t a_command;
		caddr_t a_data;
		int a_fflag;
		vfs_context_t a_context;
	} */ *ap)
{
	vfs_context_t ctx = ap->a_context;
	vnode_t vp = ap->a_vp;
	struct nfsmount *mp = VTONMP(vp);
	struct user_nfs_gss_principal gprinc;
	uint32_t len;
	int error = ENOTTY;

	if (mp == NULL)
		return (ENXIO);
	
	switch (ap->a_command) {

	case F_FULLFSYNC:
		if (vnode_vfsisrdonly(vp))
			return (EROFS);
		error = nfs_flush(VTONFS(vp), MNT_WAIT, vfs_context_thread(ctx), 0);
		break;
	case NFS_FSCTL_DESTROY_CRED:
		if (!auth_is_kerberized(mp->nm_auth))
			return (ENOTSUP);
		error = nfs_gss_clnt_ctx_remove(mp, vfs_context_ucred(ctx));
		break;
	case NFS_FSCTL_SET_CRED:
		if (!auth_is_kerberized(mp->nm_auth))
			return (ENOTSUP);
		NFS_DBG(NFS_FAC_GSS, 7, "Enter NFS_FSCTL_SET_CRED (proc %d) data = %p\n", vfs_context_is64bit(ctx), (void *)ap->a_data);
		if (vfs_context_is64bit(ctx)) {
			gprinc = *(struct user_nfs_gss_principal *)ap->a_data;
		} else {
			struct nfs_gss_principal *tp;
			tp = (struct nfs_gss_principal *)ap->a_data;
			gprinc.princlen = tp->princlen;
			gprinc.nametype = tp->nametype;
			gprinc.principal = CAST_USER_ADDR_T(tp->principal);
		}
		if (gprinc.princlen > MAXPATHLEN)
			return (EINVAL);
		NFS_DBG(NFS_FAC_GSS, 7, "Received principal length %d name type = %d\n", gprinc.princlen, gprinc.nametype);
		uint8_t *p;
		MALLOC(p, uint8_t *, gprinc.princlen+1, M_TEMP, M_WAITOK|M_ZERO);
		if (p == NULL)
			return (ENOMEM);
		error = copyin(gprinc.principal, p, gprinc.princlen);
		if (error) {
			NFS_DBG(NFS_FAC_GSS, 7, "NFS_FSCTL_SET_CRED could not copy in princiapl data of len %d: %d\n",
				gprinc.princlen, error);
			FREE(p, M_TEMP);
			return (error);
		}
		NFS_DBG(NFS_FAC_GSS, 7, "Seting credential to principal %s\n", p);
		error = nfs_gss_clnt_ctx_set_principal(mp, ctx, p, gprinc.princlen, gprinc.nametype);
		NFS_DBG(NFS_FAC_GSS, 7, "Seting credential to principal %s returned %d\n", p, error);
		FREE(p, M_TEMP);
		break;
	case NFS_FSCTL_GET_CRED:
		if (!auth_is_kerberized(mp->nm_auth))
			return (ENOTSUP);
		error = nfs_gss_clnt_ctx_get_principal(mp, ctx, &gprinc);
		if (error)
			break;
		if (vfs_context_is64bit(ctx)) {
			struct user_nfs_gss_principal *upp = (struct user_nfs_gss_principal *)ap->a_data;
			len = upp->princlen;
			if (gprinc.princlen < len)
				len = gprinc.princlen;
			upp->princlen = gprinc.princlen;
			upp->nametype = gprinc.nametype;
			upp->flags = gprinc.flags;
			if (gprinc.principal)
				error = copyout((void *)gprinc.principal, upp->principal, len);
			else
				upp->principal = USER_ADDR_NULL;
		} else {
			struct nfs_gss_principal *u32pp = (struct nfs_gss_principal *)ap->a_data;
			len = u32pp->princlen;
			if (gprinc.princlen < len)
				len = gprinc.princlen;
			u32pp->princlen = gprinc.princlen;
			u32pp->nametype = gprinc.nametype;
			u32pp->flags = gprinc.flags;
			if (gprinc.principal)
				error = copyout((void *)gprinc.principal, u32pp->principal, len);
			else
				u32pp->principal = (user32_addr_t)0;
		}
		if (error) {
			NFS_DBG(NFS_FAC_GSS, 7, "NFS_FSCTL_GET_CRED could not copy out princiapl data of len %d: %d\n",
				gprinc.princlen, error);
		}
		FREE(gprinc.principal, M_TEMP);
	}

	return (error);
}

/*ARGSUSED*/
int
nfs_vnop_select(
	__unused struct vnop_select_args /* {
		struct vnodeop_desc *a_desc;
		vnode_t a_vp;
		int a_which;
		int a_fflags;
		void *a_wql;
		vfs_context_t a_context;
	} */ *ap)
{

	/*
	 * We were once bogusly seltrue() which returns 1.  Is this right?
	 */
	return (1);
}

/*
 * vnode OP for pagein using UPL
 *
 * No buffer I/O, just RPCs straight into the mapped pages.
 */
int
nfs_vnop_pagein(
	struct vnop_pagein_args /* {
		struct vnodeop_desc *a_desc;
		vnode_t a_vp;
		upl_t a_pl;
		vm_offset_t a_pl_offset;
		off_t a_f_offset;
		size_t a_size;
		int a_flags;
		vfs_context_t a_context;
	} */ *ap)
{
	vnode_t vp = ap->a_vp;
	upl_t pl = ap->a_pl;
	size_t size = ap->a_size;
	off_t f_offset = ap->a_f_offset;
	vm_offset_t pl_offset = ap->a_pl_offset;
	int flags = ap->a_flags;
	thread_t thd;
	kauth_cred_t cred;
	nfsnode_t np = VTONFS(vp);
	size_t nmrsize, iosize, txsize, rxsize, retsize;
	off_t txoffset;
	struct nfsmount *nmp;
	int error = 0;
<<<<<<< HEAD
	vm_offset_t ioaddr, rxaddr;
	uio_t uio;
	char uio_buf [ UIO_SIZEOF(1) ];
	int nofreeupl = flags & UPL_NOCOMMIT;
	upl_page_info_t *plinfo;
#define MAXPAGINGREQS	16	/* max outstanding RPCs for pagein/pageout */
	struct nfsreq *req[MAXPAGINGREQS];
	int nextsend, nextwait;
	uint32_t stategenid = 0, restart = 0;
	kern_return_t kret;

	FSDBG(322, np, f_offset, size, flags);
	if (pl == (upl_t)NULL)
		panic("nfs_pagein: no upl");

	if (size <= 0) {
		printf("nfs_pagein: invalid size %ld", size);
		if (!nofreeupl)
			(void) ubc_upl_abort_range(pl, pl_offset, size, 0);
		return (EINVAL);
	}
	if (f_offset < 0 || f_offset >= (off_t)np->n_size || (f_offset & PAGE_MASK_64)) {
		if (!nofreeupl)
			ubc_upl_abort_range(pl, pl_offset, size,
=======
	vm_offset_t ioaddr;
	struct uio	auio;
	struct iovec	aiov;
	struct uio * uio = &auio;
	int nofreeupl = flags & UPL_NOCOMMIT;

	FSDBG(322, f_offset, size, pl, pl_offset);
	if (pl == (upl_t)NULL)
		panic("nfs_pagein: no upl");

	if (UBCINVALID(vp)) {
		printf("nfs_pagein: invalid vnode 0x%x", (int)vp);
		if (!nofreeupl)
			(void) ubc_upl_abort(pl, NULL); 
		return (EPERM);
	}
	UBCINFOCHECK("nfs_pagein", vp);

	if (size <= 0) {
		printf("nfs_pagein: invalid size %d", size);
		if (!nofreeupl)
			(void) ubc_upl_abort(pl, NULL); 
		return (EINVAL);
	}
	if (f_offset < 0 || f_offset >= np->n_size ||
	    (f_offset & PAGE_MASK_64)) {
		if (!nofreeupl)
			ubc_upl_abort_range(pl, pl_offset, size, 
>>>>>>> origin/10.1
				UPL_ABORT_ERROR | UPL_ABORT_FREE_ON_EMPTY);
		return (EINVAL);
	}
	cred = ubc_getcred(vp);
	if (cred == NOCRED)
		cred = ap->a_cred;

<<<<<<< HEAD
	thd = vfs_context_thread(ap->a_context);
	cred = ubc_getcred(vp);
	if (!IS_VALID_CRED(cred))
		cred = vfs_context_ucred(ap->a_context);
=======
	auio.uio_iov = &aiov;
	auio.uio_iovcnt = 1;
	auio.uio_offset = f_offset;
	auio.uio_segflg = UIO_SYSSPACE;
	auio.uio_rw = UIO_READ;
	auio.uio_procp = NULL;

	if ((nmp->nm_flag & (NFSMNT_NFSV3 | NFSMNT_GOTFSINFO)) == NFSMNT_NFSV3)
		(void)nfs_fsinfo(nmp, vp, cred, p);
	biosize = min(vp->v_mount->mnt_stat.f_iosize, size);
>>>>>>> origin/10.1

	uio = uio_createwithbuffer(1, f_offset, UIO_SYSSPACE, UIO_READ,
		&uio_buf, sizeof(uio_buf));

<<<<<<< HEAD
	nmp = VTONMP(vp);
	if (nfs_mount_gone(nmp)) {
		if (!nofreeupl)
			ubc_upl_abort_range(pl, pl_offset, size,
				UPL_ABORT_ERROR | UPL_ABORT_FREE_ON_EMPTY);
		return (ENXIO);
	}
	nmrsize = nmp->nm_rsize;

	plinfo = ubc_upl_pageinfo(pl);
	kret = ubc_upl_map(pl, &ioaddr);
	if (kret != KERN_SUCCESS)
		panic("nfs_vnop_pagein: ubc_upl_map() failed with (%d)", kret);
=======
	ubc_upl_map(pl, &ioaddr);
>>>>>>> origin/10.1
	ioaddr += pl_offset;

<<<<<<< HEAD
tryagain:
	if (nmp->nm_vers >= NFS_VER4)
		stategenid = nmp->nm_stategenid;
	txsize = rxsize = size;
	txoffset = f_offset;
	rxaddr = ioaddr;

	bzero(req, sizeof(req));
	nextsend = nextwait = 0;
	do {
		if (np->n_flag & NREVOKE) {
			error = EIO;
			break;
		}
		/* send requests while we need to and have available slots */
		while ((txsize > 0) && (req[nextsend] == NULL)) {
			iosize = MIN(nmrsize, txsize);
			if ((error = nmp->nm_funcs->nf_read_rpc_async(np, txoffset, iosize, thd, cred, NULL, &req[nextsend]))) {
				req[nextsend] = NULL;
				break;
			}
			txoffset += iosize;
			txsize -= iosize;
			nextsend = (nextsend + 1) % MAXPAGINGREQS;
		}
		/* wait while we need to and break out if more requests to send */
		while ((rxsize > 0) && req[nextwait]) {
			iosize = retsize = MIN(nmrsize, rxsize);
			uio_reset(uio, uio_offset(uio), UIO_SYSSPACE, UIO_READ);
			uio_addiov(uio, CAST_USER_ADDR_T(rxaddr), iosize);
			FSDBG(322, uio_offset(uio), uio_resid(uio), rxaddr, rxsize);
#if UPL_DEBUG
			upl_ubc_alias_set(pl, (uintptr_t) current_thread(), (uintptr_t) 2);
#endif /* UPL_DEBUG */
			OSAddAtomic64(1, &nfsstats.pageins);
			error = nmp->nm_funcs->nf_read_rpc_async_finish(np, req[nextwait], uio, &retsize, NULL);
			req[nextwait] = NULL;
			nextwait = (nextwait + 1) % MAXPAGINGREQS;
			if ((nmp->nm_vers >= NFS_VER4) && nfs_mount_state_error_should_restart(error)) {
				lck_mtx_lock(&nmp->nm_lock);
				if ((error != NFSERR_GRACE) && (stategenid == nmp->nm_stategenid)) {
					NP(np, "nfs_vnop_pagein: error %d, initiating recovery", error);
					nfs_need_recover(nmp, error);
				}
				lck_mtx_unlock(&nmp->nm_lock);
				restart++;
				goto cancel;
			}
			if (error) {
				FSDBG(322, uio_offset(uio), uio_resid(uio), error, -1);
				break;
			}
			if (retsize < iosize) {
				/* Just zero fill the rest of the valid area. */
				int zcnt = iosize - retsize;
				bzero((char *)rxaddr + retsize, zcnt);
				FSDBG(324, uio_offset(uio), retsize, zcnt, rxaddr);
				uio_update(uio, zcnt);
			}
			rxaddr += iosize;	
			rxsize -= iosize;
			if (txsize)
				break;
		}
	} while (!error && (txsize || rxsize));

	restart = 0;

	if (error) {
cancel:
		/* cancel any outstanding requests */
		while (req[nextwait]) {
			nfs_request_async_cancel(req[nextwait]);
			req[nextwait] = NULL;
			nextwait = (nextwait + 1) % MAXPAGINGREQS;
		}
		if (np->n_flag & NREVOKE) {
			error = EIO;
		} else if (restart) {
			if (restart <= nfs_mount_state_max_restarts(nmp)) { /* guard against no progress */
				if (error == NFSERR_GRACE)
					tsleep(&nmp->nm_state, (PZERO-1), "nfsgrace", 2*hz);
				if (!(error = nfs_mount_state_wait_for_recovery(nmp)))
					goto tryagain;
			} else {
				NP(np, "nfs_pagein: too many restarts, aborting");
			}
=======
	do {
		uio->uio_resid = min(biosize, xsize);
		aiov.iov_len  = uio->uio_resid;
		aiov.iov_base = (caddr_t)ioaddr;

		FSDBG(322, uio->uio_offset, uio->uio_resid, ioaddr, xsize);
#warning our nfs_pagein does not support NQNFS
		/*
		 * With UBC we get here only when the file data is not in the VM
		 * page cache, so go ahead and read in.
		 */
#ifdef UBC_DEBUG
		upl_ubc_alias_set(pl, current_act(), 2);
#endif /* UBC_DEBUG */
		nfsstats.pageins++;
		error = nfs_readrpc(vp, uio, cred);

		if (!error) {
			if (uio->uio_resid) {
				/*
				 * If uio_resid > 0, there is a hole in the file
				 * and no writes after the hole have been pushed
				 * to the server yet... or we're at the EOF
				 * Just zero fill the rest of the valid area.
				 */
				int zcnt = uio->uio_resid;
				int zoff = biosize - zcnt;
				bzero((char *)ioaddr + zoff, zcnt);

				FSDBG(324, uio->uio_offset, zoff, zcnt, ioaddr);
				uio->uio_offset += zcnt;
			}
			ioaddr += biosize;	
			xsize  -= biosize;
		} else
			FSDBG(322, uio->uio_offset, uio->uio_resid, error, -1);
		if (p && (vp->v_flag & VTEXT) &&
		    ((nmp->nm_flag & NFSMNT_NQNFS &&
		      NQNFS_CKINVALID(vp, np, ND_READ) &&
		      np->n_lrev != np->n_brev) ||
		     (!(nmp->nm_flag & NFSMNT_NQNFS) &&
		      np->n_mtime != np->n_vattr.va_mtime.tv_sec))) {
			uprintf("Process killed due to text file modification\n");
			psignal(p, SIGKILL);
			p->p_flag |= P_NOSWAP;
>>>>>>> origin/10.1
		}
	}

	ubc_upl_unmap(pl);

	if (!nofreeupl) {
<<<<<<< HEAD
		if (error)
			ubc_upl_abort_range(pl, pl_offset, size,
=======
		if (error) 
			ubc_upl_abort_range(pl, pl_offset, size, 
>>>>>>> origin/10.1
					    UPL_ABORT_ERROR |
					    UPL_ABORT_FREE_ON_EMPTY);
		else
			ubc_upl_commit_range(pl, pl_offset, size,
					     UPL_COMMIT_CLEAR_DIRTY |
					     UPL_COMMIT_FREE_ON_EMPTY);
	}
	return (error);
}


<<<<<<< HEAD
/*
 * the following are needed only by nfs_pageout to know how to handle errors
 * see nfs_pageout comments on explanation of actions.
 * the errors here are copied from errno.h and errors returned by servers
 * are expected to match the same numbers here. If not, our actions maybe
 * erroneous.
 */
char nfs_pageouterrorhandler(int);
enum actiontype {NOACTION, DUMP, DUMPANDLOG, RETRY, SEVER};
#define NFS_ELAST 88
static u_char errorcount[NFS_ELAST+1]; /* better be zeros when initialized */
static const char errortooutcome[NFS_ELAST+1] = {
	NOACTION,
	DUMP,			/* EPERM	1	Operation not permitted */
	DUMP,			/* ENOENT	2	No such file or directory */
	DUMPANDLOG,		/* ESRCH	3	No such process */
	RETRY,			/* EINTR 	4	Interrupted system call */
	DUMP,			/* EIO		5	Input/output error */
	DUMP,			/* ENXIO	6	Device not configured */
	DUMPANDLOG,		/* E2BIG	7	Argument list too long */
	DUMPANDLOG,		/* ENOEXEC	8	Exec format error */
	DUMPANDLOG,		/* EBADF	9	Bad file descriptor */
	DUMPANDLOG,		/* ECHILD	10	No child processes */
	DUMPANDLOG,		/* EDEADLK	11	Resource deadlock avoided - was EAGAIN */
	RETRY,			/* ENOMEM	12	Cannot allocate memory */
	DUMP,			/* EACCES	13	Permission denied */
	DUMPANDLOG,		/* EFAULT	14	Bad address */
	DUMPANDLOG,		/* ENOTBLK	15	POSIX - Block device required */
	RETRY,			/* EBUSY	16	Device busy */
	DUMP,			/* EEXIST	17	File exists */
	DUMP,			/* EXDEV	18	Cross-device link */
	DUMP,			/* ENODEV	19	Operation not supported by device */
	DUMP,			/* ENOTDIR	20	Not a directory */
	DUMP,			/* EISDIR 	21	Is a directory */
	DUMP,			/* EINVAL	22	Invalid argument */
	DUMPANDLOG,		/* ENFILE	23	Too many open files in system */
	DUMPANDLOG,		/* EMFILE	24	Too many open files */
	DUMPANDLOG,		/* ENOTTY	25	Inappropriate ioctl for device */
	DUMPANDLOG,		/* ETXTBSY	26	Text file busy - POSIX */
	DUMP,			/* EFBIG	27	File too large */
	DUMP,			/* ENOSPC	28	No space left on device */
	DUMPANDLOG,		/* ESPIPE	29	Illegal seek */
	DUMP,			/* EROFS	30	Read-only file system */
	DUMP,			/* EMLINK	31	Too many links */
	RETRY,			/* EPIPE	32	Broken pipe */
	/* math software */
	DUMPANDLOG,		/* EDOM				33	Numerical argument out of domain */
	DUMPANDLOG,		/* ERANGE			34	Result too large */
	RETRY,			/* EAGAIN/EWOULDBLOCK	35	Resource temporarily unavailable */
	DUMPANDLOG,		/* EINPROGRESS		36	Operation now in progress */
	DUMPANDLOG,		/* EALREADY			37	Operation already in progress */
	/* ipc/network software -- argument errors */
	DUMPANDLOG,		/* ENOTSOC			38	Socket operation on non-socket */
	DUMPANDLOG,		/* EDESTADDRREQ		39	Destination address required */
	DUMPANDLOG,		/* EMSGSIZE			40	Message too long */
	DUMPANDLOG,		/* EPROTOTYPE		41	Protocol wrong type for socket */
	DUMPANDLOG,		/* ENOPROTOOPT		42	Protocol not available */
	DUMPANDLOG,		/* EPROTONOSUPPORT	43	Protocol not supported */
	DUMPANDLOG,		/* ESOCKTNOSUPPORT	44	Socket type not supported */
	DUMPANDLOG,		/* ENOTSUP			45	Operation not supported */
	DUMPANDLOG,		/* EPFNOSUPPORT		46	Protocol family not supported */
	DUMPANDLOG,		/* EAFNOSUPPORT		47	Address family not supported by protocol family */
	DUMPANDLOG,		/* EADDRINUSE		48	Address already in use */
	DUMPANDLOG,		/* EADDRNOTAVAIL	49	Can't assign requested address */
	/* ipc/network software -- operational errors */
	RETRY,			/* ENETDOWN			50	Network is down */
	RETRY,			/* ENETUNREACH		51	Network is unreachable */
	RETRY,			/* ENETRESET		52	Network dropped connection on reset */
	RETRY,			/* ECONNABORTED		53	Software caused connection abort */
	RETRY,			/* ECONNRESET		54	Connection reset by peer */
	RETRY,			/* ENOBUFS			55	No buffer space available */
	RETRY,			/* EISCONN			56	Socket is already connected */
	RETRY,			/* ENOTCONN			57	Socket is not connected */
	RETRY,			/* ESHUTDOWN		58	Can't send after socket shutdown */
	RETRY,			/* ETOOMANYREFS		59	Too many references: can't splice */
	RETRY,			/* ETIMEDOUT		60	Operation timed out */
	RETRY,			/* ECONNREFUSED		61	Connection refused */

	DUMPANDLOG,		/* ELOOP			62	Too many levels of symbolic links */
	DUMP,			/* ENAMETOOLONG		63	File name too long */
	RETRY,			/* EHOSTDOWN		64	Host is down */
	RETRY,			/* EHOSTUNREACH		65	No route to host */
	DUMP,			/* ENOTEMPTY		66	Directory not empty */
	/* quotas & mush */
	DUMPANDLOG,		/* PROCLIM			67	Too many processes */
	DUMPANDLOG,		/* EUSERS			68	Too many users */
	DUMPANDLOG,		/* EDQUOT			69	Disc quota exceeded */
	/* Network File System */
	DUMP,			/* ESTALE			70	Stale NFS file handle */
	DUMP,			/* EREMOTE			71	Too many levels of remote in path */
	DUMPANDLOG,		/* EBADRPC			72	RPC struct is bad */
	DUMPANDLOG,		/* ERPCMISMATCH		73	RPC version wrong */
	DUMPANDLOG,		/* EPROGUNAVAIL		74	RPC prog. not avail */
	DUMPANDLOG,		/* EPROGMISMATCH	75	Program version wrong */
	DUMPANDLOG,		/* EPROCUNAVAIL		76	Bad procedure for program */

	DUMPANDLOG,		/* ENOLCK			77	No locks available */
	DUMPANDLOG,		/* ENOSYS			78	Function not implemented */
	DUMPANDLOG,		/* EFTYPE			79	Inappropriate file type or format */
	DUMPANDLOG,		/* EAUTH			80	Authentication error */
	DUMPANDLOG,		/* ENEEDAUTH		81	Need authenticator */
	/* Intelligent device errors */
	DUMPANDLOG,		/* EPWROFF			82	Device power is off */
	DUMPANDLOG,		/* EDEVERR			83	Device error, e.g. paper out */
	DUMPANDLOG,		/* EOVERFLOW		84	Value too large to be stored in data type */
	/* Program loading errors */
	DUMPANDLOG,		/* EBADEXEC			85	Bad executable */
	DUMPANDLOG,		/* EBADARCH			86	Bad CPU type in executable */
	DUMPANDLOG,		/* ESHLIBVERS		87	Shared library version mismatch */
	DUMPANDLOG,		/* EBADMACHO		88	Malformed Macho file */
};

char
nfs_pageouterrorhandler(int error)
{
	if (error > NFS_ELAST)
		return(DUMP);
	else
		return(errortooutcome[error]);
}


=======
>>>>>>> origin/10.1
/*
 * vnode OP for pageout using UPL
 *
 * No buffer I/O, just RPCs straight from the mapped pages.
 * File size changes are not permitted in pageout.
 */
int
nfs_vnop_pageout(
	struct vnop_pageout_args /* {
		struct vnodeop_desc *a_desc;
		vnode_t a_vp;
		upl_t a_pl;
		vm_offset_t a_pl_offset;
		off_t a_f_offset;
		size_t a_size;
		int a_flags;
		vfs_context_t a_context;
	} */ *ap)
{
	vnode_t vp = ap->a_vp;
	upl_t pl = ap->a_pl;
	size_t size = ap->a_size;
	off_t f_offset = ap->a_f_offset;
	vm_offset_t pl_offset = ap->a_pl_offset;
<<<<<<< HEAD
	int flags = ap->a_flags;
	nfsnode_t np = VTONFS(vp);
	thread_t thd;
	kauth_cred_t cred;
	struct nfsbuf *bp;
	struct nfsmount *nmp = VTONMP(vp);
	daddr64_t lbn;
	int error = 0, iomode;
	off_t off, txoffset, rxoffset;
	vm_offset_t ioaddr, txaddr, rxaddr;
	uio_t auio;
	char uio_buf [ UIO_SIZEOF(1) ];
	int nofreeupl = flags & UPL_NOCOMMIT;
	size_t nmwsize, biosize, iosize, pgsize, txsize, rxsize, xsize, remsize;
	struct nfsreq *req[MAXPAGINGREQS];
	int nextsend, nextwait, wverfset, commit;
	uint64_t wverf, wverf2;
	uint32_t stategenid = 0, vrestart = 0, restart = 0, vrestarts = 0, restarts = 0;
	kern_return_t kret;

	FSDBG(323, f_offset, size, pl, pl_offset);

	if (pl == (upl_t)NULL)
		panic("nfs_pageout: no upl");
=======
	int flags  = ap->a_flags;
	int ioflag = ap->a_flags;
	register int biosize;
	struct proc *p = current_proc();
	struct nfsnode *np = VTONFS(vp);
	register struct ucred *cred;
	struct buf *bp;
	struct nfsmount *nmp = VFSTONFS(vp->v_mount);
	daddr_t lbn;
	int n = 0, on, error = 0, iomode, must_commit, s;
	off_t off;
	vm_offset_t ioaddr;
	struct uio	auio;
	struct iovec	aiov;
	struct uio * uio = &auio;
	int nofreeupl = flags & UPL_NOCOMMIT;
	int iosize;
	int pgsize;

	FSDBG(323, f_offset, size, pl, pl_offset);

	if (pl == (upl_t)NULL)
		panic("nfs_pageout: no upl");

	if (UBCINVALID(vp)) {
		printf("nfs_pageout: invalid vnode 0x%x", (int)vp);
		if (!nofreeupl)
			(void) ubc_upl_abort(pl, NULL); 
		return (EIO);
	}
	UBCINFOCHECK("nfs_pageout", vp);

	if (size <= 0) {
		printf("nfs_pageout: invalid size %d", size);
		if (!nofreeupl)
			(void) ubc_upl_abort(pl, NULL); 
		return (EINVAL);
	}
>>>>>>> origin/10.1

	if (size <= 0) {
		printf("nfs_pageout: invalid size %ld", size);
		if (!nofreeupl)
			ubc_upl_abort_range(pl, pl_offset, size, 0);
		return (EINVAL);
	}

	if (!nmp) {
		if (!nofreeupl)
			ubc_upl_abort(pl, UPL_ABORT_DUMP_PAGES|UPL_ABORT_FREE_ON_EMPTY);
		return (ENXIO);
	}
	biosize = nmp->nm_biosize;
	nmwsize = nmp->nm_wsize;

<<<<<<< HEAD
	nfs_data_lock_noupdate(np, NFS_DATA_LOCK_SHARED);

=======
>>>>>>> origin/10.1
	/*
	 * Check to see whether the buffer is incore.
	 * If incore and not busy, invalidate it from the cache.
	 */
<<<<<<< HEAD
	for (iosize = 0; iosize < size; iosize += xsize) {
		off = f_offset + iosize;
		/* need make sure we do things on block boundaries */
		xsize = biosize - (off % biosize);
		if (off + xsize > f_offset + size)
			xsize = f_offset + size - off;
		lbn = (daddr64_t)(off / biosize);
		lck_mtx_lock(nfs_buf_mutex);
		if ((bp = nfs_buf_incore(np, lbn))) {
			FSDBG(323, off, bp, bp->nb_lflags, bp->nb_flags);
			if (nfs_buf_acquire(bp, NBAC_NOWAIT, 0, 0)) {
				lck_mtx_unlock(nfs_buf_mutex);
				nfs_data_unlock_noupdate(np);
				/* no panic. just tell vm we are busy */
				if (!nofreeupl)
					ubc_upl_abort_range(pl, pl_offset, size, 0);
				return (EBUSY);
			}
			if (bp->nb_dirtyend > 0) {
				/*
				 * if there's a dirty range in the buffer, check
				 * to see if it extends beyond the pageout region
				 *
				 * if the dirty region lies completely within the
				 * pageout region, we just invalidate the buffer
				 * because it's all being written out now anyway.
				 *
				 * if any of the dirty region lies outside the
				 * pageout region, we'll try to clip the dirty
				 * region to eliminate the portion that's being
				 * paged out.  If that's not possible, because
				 * the dirty region extends before and after the
				 * pageout region, then we'll just return EBUSY.
				 */
				off_t boff, start, end;
				boff = NBOFF(bp);
				start = off;
				end = off + xsize;
				/* clip end to EOF */
				if (end > (off_t)np->n_size)
					end = np->n_size;
				start -= boff;
				end -= boff;
				if ((bp->nb_dirtyoff < start) &&
				    (bp->nb_dirtyend > end)) {
				    /*
				     * not gonna be able to clip the dirty region
				     *
				     * But before returning the bad news, move the
				     * buffer to the start of the delwri list and
				     * give the list a push to try to flush the
				     * buffer out.
				     */
				    FSDBG(323, np, bp, 0xd00deebc, EBUSY);
				    nfs_buf_remfree(bp);
				    TAILQ_INSERT_HEAD(&nfsbufdelwri, bp, nb_free);
				    nfsbufdelwricnt++;
				    nfs_buf_drop(bp);
				    nfs_buf_delwri_push(1);
				    lck_mtx_unlock(nfs_buf_mutex);
				    nfs_data_unlock_noupdate(np);
				    if (!nofreeupl)
					    ubc_upl_abort_range(pl, pl_offset, size, 0);
				    return (EBUSY);
				}
				if ((bp->nb_dirtyoff < start) ||
				    (bp->nb_dirtyend > end)) {
				    /* clip dirty region, if necessary */
				    if (bp->nb_dirtyoff < start)
					bp->nb_dirtyend = min(bp->nb_dirtyend, start);
				    if (bp->nb_dirtyend > end)
					bp->nb_dirtyoff = max(bp->nb_dirtyoff, end);
				    FSDBG(323, bp, bp->nb_dirtyoff, bp->nb_dirtyend, 0xd00dee00);
				    /* we're leaving this block dirty */
				    nfs_buf_drop(bp);
				    lck_mtx_unlock(nfs_buf_mutex);
				    continue;
				}
			}
			nfs_buf_remfree(bp);
			lck_mtx_unlock(nfs_buf_mutex);
			SET(bp->nb_flags, NB_INVAL);
			nfs_node_lock_force(np);
			if (ISSET(bp->nb_flags, NB_NEEDCOMMIT)) {
				CLR(bp->nb_flags, NB_NEEDCOMMIT);
				np->n_needcommitcnt--;
				CHECK_NEEDCOMMITCNT(np);
			}
<<<<<<< HEAD
			nfs_node_unlock(np);
			nfs_buf_release(bp, 1);
		} else {
			lck_mtx_unlock(nfs_buf_mutex);
=======
	lbn = f_offset / PAGE_SIZE; /* to match the size getblk uses */
        
	for (iosize = size; iosize > 0; iosize -= PAGE_SIZE, lbn++) {
		s = splbio();
		if (bp = incore(vp, lbn)) {
			FSDBG(323, lbn*PAGE_SIZE, 1, bp, bp->b_flags);
			if (ISSET(bp->b_flags, B_BUSY)) {
				/* no panic. just tell vm we are busy */
				if (!nofreeupl)
					(void) ubc_upl_abort(pl, NULL); 
				return(EBUSY);
			}
			bremfree(bp);
			SET(bp->b_flags, (B_BUSY | B_INVAL));
			brelse(bp);
>>>>>>> origin/10.1
=======
			nfs_buf_release(bp, 1);
>>>>>>> origin/10.3
		}
	}

	thd = vfs_context_thread(ap->a_context);
	cred = ubc_getcred(vp);
	if (!IS_VALID_CRED(cred))
		cred = vfs_context_ucred(ap->a_context);

	nfs_node_lock_force(np);
	if (np->n_flag & NWRITEERR) {
<<<<<<< HEAD
		error = np->n_error;
		nfs_node_unlock(np);
		nfs_data_unlock_noupdate(np);
		if (!nofreeupl)
			ubc_upl_abort_range(pl, pl_offset, size,
					    UPL_ABORT_FREE_ON_EMPTY);
		return (error);
	}
	nfs_node_unlock(np);

	if (f_offset < 0 || f_offset >= (off_t)np->n_size ||
	    f_offset & PAGE_MASK_64 || size & PAGE_MASK_64) {
		nfs_data_unlock_noupdate(np);
=======
		np->n_flag &= ~NWRITEERR;
		if (!nofreeupl)
			ubc_upl_abort_range(pl, pl_offset, size,
					    UPL_ABORT_FREE_ON_EMPTY);
		return (np->n_error);
	}
	if ((nmp->nm_flag & (NFSMNT_NFSV3 | NFSMNT_GOTFSINFO)) == NFSMNT_NFSV3)
		(void)nfs_fsinfo(nmp, vp, cred, p);

	if (f_offset < 0 || f_offset >= np->n_size ||
	   f_offset & PAGE_MASK_64 || size & PAGE_MASK) {
>>>>>>> origin/10.1
		if (!nofreeupl)
			ubc_upl_abort_range(pl, pl_offset, size,
					    UPL_ABORT_FREE_ON_EMPTY);
		return (EINVAL);
	}

	kret = ubc_upl_map(pl, &ioaddr);
	if (kret != KERN_SUCCESS)
		panic("nfs_vnop_pageout: ubc_upl_map() failed with (%d)", kret);
	ioaddr += pl_offset;

<<<<<<< HEAD
	if ((u_quad_t)f_offset + size > np->n_size)
		xsize = np->n_size - f_offset;
	else
		xsize = size;

	pgsize = round_page_64(xsize);
	if ((size > pgsize) && !nofreeupl)
		ubc_upl_abort_range(pl, pl_offset + pgsize, size - pgsize,
				    UPL_ABORT_FREE_ON_EMPTY);

	/*
=======
	if (f_offset + size > np->n_size)
		iosize = np->n_size - f_offset;
	else
		iosize = size;

	pgsize = (iosize + (PAGE_SIZE - 1)) & ~PAGE_MASK;

	if (size > pgsize) {
		if (!nofreeupl)
			ubc_upl_abort_range(pl, pl_offset + pgsize,
					    size - pgsize,
					    UPL_ABORT_FREE_ON_EMPTY);
	}
	auio.uio_iov = &aiov;
	auio.uio_iovcnt = 1;
	auio.uio_offset = f_offset;
	auio.uio_segflg = UIO_SYSSPACE;
	auio.uio_rw = UIO_READ;
	auio.uio_resid = iosize;
	auio.uio_procp = NULL;

	aiov.iov_len = iosize;
	aiov.iov_base = (caddr_t)ioaddr + pl_offset;
	/* 
>>>>>>> origin/10.1
	 * check for partial page and clear the
	 * contents past end of the file before
	 * releasing it in the VM page cache
	 */
<<<<<<< HEAD
	if ((u_quad_t)f_offset < np->n_size && (u_quad_t)f_offset + size > np->n_size) {
		size_t io = np->n_size - f_offset;
		bzero((caddr_t)(ioaddr + io), size - io);
=======
	if (f_offset < np->n_size && f_offset + size > np->n_size) {
		size_t io = np->n_size - f_offset;

		bzero((caddr_t)(ioaddr + pl_offset + io), size - io);

>>>>>>> origin/10.1
		FSDBG(321, np->n_size, f_offset, f_offset + io, size - io);
	}
	nfs_data_unlock_noupdate(np);

	auio = uio_createwithbuffer(1, 0, UIO_SYSSPACE, UIO_WRITE,
		&uio_buf, sizeof(uio_buf));

tryagain:
	if (nmp->nm_vers >= NFS_VER4)
		stategenid = nmp->nm_stategenid;
	wverf = wverf2 = wverfset = 0;
	txsize = rxsize = xsize;
	txoffset = rxoffset = f_offset;
	txaddr = rxaddr = ioaddr;
	commit = NFS_WRITE_FILESYNC;

	bzero(req, sizeof(req));
	nextsend = nextwait = 0;
	do {
<<<<<<< HEAD
		if (np->n_flag & NREVOKE) {
			error = EIO;
			break;
		}
		/* send requests while we need to and have available slots */
		while ((txsize > 0) && (req[nextsend] == NULL)) {
			iosize = MIN(nmwsize, txsize);
			uio_reset(auio, txoffset, UIO_SYSSPACE, UIO_WRITE);
			uio_addiov(auio, CAST_USER_ADDR_T(txaddr), iosize);
			FSDBG(323, uio_offset(auio), iosize, txaddr, txsize);
			OSAddAtomic64(1, &nfsstats.pageouts);
			nfs_node_lock_force(np);
			np->n_numoutput++;
			nfs_node_unlock(np);
			vnode_startwrite(vp);
			iomode = NFS_WRITE_UNSTABLE;
			if ((error = nmp->nm_funcs->nf_write_rpc_async(np, auio, iosize, thd, cred, iomode, NULL, &req[nextsend]))) {
				req[nextsend] = NULL;
				vnode_writedone(vp);
				nfs_node_lock_force(np);
				np->n_numoutput--;
				nfs_node_unlock(np);
				break;
			}
			txaddr += iosize;
			txoffset += iosize;
			txsize -= iosize;
			nextsend = (nextsend + 1) % MAXPAGINGREQS;
		}
		/* wait while we need to and break out if more requests to send */
		while ((rxsize > 0) && req[nextwait]) {
			iosize = remsize = MIN(nmwsize, rxsize);
			error = nmp->nm_funcs->nf_write_rpc_async_finish(np, req[nextwait], &iomode, &iosize, &wverf2);
			req[nextwait] = NULL;
			nextwait = (nextwait + 1) % MAXPAGINGREQS;
			vnode_writedone(vp);
			nfs_node_lock_force(np);
			np->n_numoutput--;
			nfs_node_unlock(np);
			if ((nmp->nm_vers >= NFS_VER4) && nfs_mount_state_error_should_restart(error)) {
				lck_mtx_lock(&nmp->nm_lock);
				if ((error != NFSERR_GRACE) && (stategenid == nmp->nm_stategenid)) {
					NP(np, "nfs_vnop_pageout: error %d, initiating recovery", error);
					nfs_need_recover(nmp, error);
				}
				lck_mtx_unlock(&nmp->nm_lock);
				restart = 1;
				goto cancel;
			}
			if (error) {
				FSDBG(323, rxoffset, rxsize, error, -1);
				break;
			}
			if (!wverfset) {
				wverf = wverf2;
				wverfset = 1;
			} else if (wverf != wverf2) {
				/* verifier changed, so we need to restart all the writes */
				vrestart = 1;
				goto cancel;
			}
			/* Retain the lowest commitment level returned. */
			if (iomode < commit)
				commit = iomode;
			rxaddr += iosize;	
			rxoffset += iosize;	
			rxsize -= iosize;
			remsize -= iosize;
			if (remsize > 0) {
				/* need to try sending the remainder */
				iosize = remsize;
				uio_reset(auio, rxoffset, UIO_SYSSPACE, UIO_WRITE);
				uio_addiov(auio, CAST_USER_ADDR_T(rxaddr), remsize);
				iomode = NFS_WRITE_UNSTABLE;
				error = nfs_write_rpc2(np, auio, thd, cred, &iomode, &wverf2);
				if ((nmp->nm_vers >= NFS_VER4) && nfs_mount_state_error_should_restart(error)) {
					NP(np, "nfs_vnop_pageout: restart: error %d", error);
					lck_mtx_lock(&nmp->nm_lock);
					if ((error != NFSERR_GRACE) && (stategenid == nmp->nm_stategenid)) {
						NP(np, "nfs_vnop_pageout: error %d, initiating recovery", error);
						nfs_need_recover(nmp, error);
					}
					lck_mtx_unlock(&nmp->nm_lock);
					restart = 1;
					goto cancel;
				}
				if (error) {
					FSDBG(323, rxoffset, rxsize, error, -1);
					break;
				}
				if (wverf != wverf2) {
					/* verifier changed, so we need to restart all the writes */
					vrestart = 1;
					goto cancel;
				}
				if (iomode < commit)
					commit = iomode;
				rxaddr += iosize;	
				rxoffset += iosize;	
				rxsize -= iosize;
			}
			if (txsize)
				break;
		}
	} while (!error && (txsize || rxsize));

	vrestart = 0;
=======
#warning our nfs_pageout does not support NQNFS
		nfsstats.pageouts++;
		lbn = uio->uio_offset / biosize;
		on = uio->uio_offset & (biosize-1);
		n = min((unsigned)(biosize - on), uio->uio_resid);
again:
#if 0
		/* (removed for UBC) */
		bufsize = biosize;
		if ((off_t)(lbn + 1) * biosize > np->n_size) {
			bufsize = np->n_size - (off_t)lbn * biosize;
			bufsize = (bufsize + DEV_BSIZE - 1) & ~(DEV_BSIZE - 1);
		}
#endif
		vp->v_numoutput++;
		/* NMODIFIED would be set here if doing unstable writes */
		iomode = NFSV3WRITE_FILESYNC;
		error = nfs_writerpc(vp, uio, cred, &iomode, &must_commit);
		if (must_commit)
			nfs_clearcommit(vp->v_mount);
		vpwakeup(vp);
>>>>>>> origin/10.1

	if (!error && (commit != NFS_WRITE_FILESYNC)) {
		error = nmp->nm_funcs->nf_commit_rpc(np, f_offset, xsize, cred, wverf);
		if (error == NFSERR_STALEWRITEVERF) {
			vrestart = 1;
			error = EIO;
		}
	}

	if (error) {
cancel:
		/* cancel any outstanding requests */
		while (req[nextwait]) {
			nfs_request_async_cancel(req[nextwait]);
			req[nextwait] = NULL;
			nextwait = (nextwait + 1) % MAXPAGINGREQS;
			vnode_writedone(vp);
			nfs_node_lock_force(np);
			np->n_numoutput--;
			nfs_node_unlock(np);
		}
		if (np->n_flag & NREVOKE) {
			error = EIO;
		} else {
			if (vrestart) {
				if (++vrestarts <= 100) /* guard against no progress */
					goto tryagain;
				NP(np, "nfs_pageout: too many restarts, aborting");
				FSDBG(323, f_offset, xsize, ERESTART, -1);
			}
			if (restart) {
				if (restarts <= nfs_mount_state_max_restarts(nmp)) { /* guard against no progress */
					if (error == NFSERR_GRACE)
						tsleep(&nmp->nm_state, (PZERO-1), "nfsgrace", 2*hz);
					if (!(error = nfs_mount_state_wait_for_recovery(nmp)))
						goto tryagain;
				} else {
					NP(np, "nfs_pageout: too many restarts, aborting");
					FSDBG(323, f_offset, xsize, ERESTART, -1);
				}
			}
		}
	}

	ubc_upl_unmap(pl);

	/*
	 * We've had several different solutions on what to do when the pageout
	 * gets an error. If we don't handle it, and return an error to the
	 * caller, vm, it will retry . This can end in endless looping
	 * between vm and here doing retries of the same page. Doing a dump
	 * back to vm, will get it out of vm's knowledge and we lose whatever
	 * data existed. This is risky, but in some cases necessary. For
	 * example, the initial fix here was to do that for ESTALE. In that case
	 * the server is telling us that the file is no longer the same. We
	 * would not want to keep paging out to that. We also saw some 151
	 * errors from Auspex server and NFSv3 can return errors higher than
	 * ELAST. Those along with NFS known server errors we will "dump" from
	 * vm.  Errors we don't expect to occur, we dump and log for further
	 * analysis. Errors that could be transient, networking ones,
	 * we let vm "retry". Lastly, errors that we retry, but may have potential
	 * to storm the network, we "retrywithsleep". "sever" will be used in
	 * in the future to dump all pages of object for cases like ESTALE.
	 * All this is the basis for the states returned and first guesses on
	 * error handling. Tweaking expected as more statistics are gathered.
	 * Note, in the long run we may need another more robust solution to
	 * have some kind of persistant store when the vm cannot dump nor keep
	 * retrying as a solution, but this would be a file architectural change
	 */
<<<<<<< HEAD
=======
	  
>>>>>>> origin/10.1
	if (!nofreeupl) { /* otherwise stacked file system has to handle this */
		if (error) {
			int abortflags = 0;
			char action = nfs_pageouterrorhandler(error);
			
			switch (action) {
				case DUMP:
					abortflags = UPL_ABORT_DUMP_PAGES|UPL_ABORT_FREE_ON_EMPTY;
					break;
				case DUMPANDLOG:
					abortflags = UPL_ABORT_DUMP_PAGES|UPL_ABORT_FREE_ON_EMPTY;
<<<<<<< HEAD
					if (error <= NFS_ELAST) {
						if ((errorcount[error] % 100) == 0)
							NP(np, "nfs_pageout: unexpected error %d. dumping vm page", error);
						errorcount[error]++;
					}
=======
					if (error <= ELAST &&
					    (errorcount[error] % 100 == 0)) 
						printf("nfs_pageout: unexpected error %d. dumping vm page\n", error);
					errorcount[error]++;
>>>>>>> origin/10.1
					break;
				case RETRY:
					abortflags = UPL_ABORT_FREE_ON_EMPTY;
					break;
<<<<<<< HEAD
=======
				case RETRYWITHSLEEP:
					abortflags = UPL_ABORT_FREE_ON_EMPTY;
					/* pri unused. PSOCK for placeholder. */
					(void) tsleep(&lbolt, PSOCK,
						      "nfspageout", 0);
					break;
>>>>>>> origin/10.1
				case SEVER: /* not implemented */
				default:
					NP(np, "nfs_pageout: action %d not expected", action);
					break;
			}

			ubc_upl_abort_range(pl, pl_offset, pgsize, abortflags);
			/* return error in all cases above */

		} else {
			ubc_upl_commit_range(pl, pl_offset, pgsize,
					     UPL_COMMIT_CLEAR_DIRTY |
					     UPL_COMMIT_FREE_ON_EMPTY);
<<<<<<< HEAD
		}
=======
>>>>>>> origin/10.1
	}
	return (error);
}

/* Blktooff derives file offset given a logical block number */
int
nfs_vnop_blktooff(
	struct vnop_blktooff_args /* {
		struct vnodeop_desc *a_desc;
		vnode_t a_vp;
		daddr64_t a_lblkno;
		off_t *a_offset;
	} */ *ap)
{
	int biosize;
	vnode_t vp = ap->a_vp;
	struct nfsmount *nmp = VTONMP(vp);

	if (nfs_mount_gone(nmp))
		return (ENXIO);
	biosize = nmp->nm_biosize;

<<<<<<< HEAD
	*ap->a_offset = (off_t)(ap->a_lblkno * biosize);
=======
	*ap->a_offset = (off_t)ap->a_lblkno *  biosize;
>>>>>>> origin/10.2

	return (0);
}

<<<<<<< HEAD
int
nfs_vnop_offtoblk(
	struct vnop_offtoblk_args /* {
		struct vnodeop_desc *a_desc;
		vnode_t a_vp;
		off_t a_offset;
		daddr64_t *a_lblkno;
	} */ *ap)
=======
static int
nfs_offtoblk(ap)
	struct vop_offtoblk_args /* {
		struct vnode *a_vp;
		off_t a_offset;    
		daddr_t *a_lblkno;
	} */ *ap;
>>>>>>> origin/10.1
{
	int biosize;
	vnode_t vp = ap->a_vp;
	struct nfsmount *nmp = VTONMP(vp);

	if (nfs_mount_gone(nmp))
		return (ENXIO);
	biosize = nmp->nm_biosize;

	*ap->a_lblkno = (daddr64_t)(ap->a_offset / biosize);

	return (0);
}

/*
 * vnode change monitoring
 */
int
nfs_vnop_monitor(
	struct vnop_monitor_args /* {
		struct vnodeop_desc *a_desc;
		vnode_t a_vp;
		uint32_t a_events;
		uint32_t a_flags;
		void *a_handle;
		vfs_context_t a_context;
	} */ *ap)
{
	nfsnode_t np = VTONFS(ap->a_vp);
	struct nfsmount *nmp = VTONMP(ap->a_vp);
	int error = 0;

	if (nfs_mount_gone(nmp))
		return (ENXIO);

	/* make sure that the vnode's monitoring status is up to date */
	lck_mtx_lock(&nmp->nm_lock);
	if (vnode_ismonitored(ap->a_vp)) {
		/* This vnode is currently being monitored, make sure we're tracking it. */
		if (np->n_monlink.le_next == NFSNOLIST) {
			LIST_INSERT_HEAD(&nmp->nm_monlist, np, n_monlink);
			nfs_mount_sock_thread_wake(nmp);
		}
	} else {
		/* This vnode is no longer being monitored, make sure we're not tracking it. */
		/* Wait for any in-progress getattr to complete first. */
		while (np->n_mflag & NMMONSCANINPROG) {
			struct timespec ts = { 1, 0 };
			np->n_mflag |= NMMONSCANWANT;
			msleep(&np->n_mflag, &nmp->nm_lock, PZERO-1, "nfswaitmonscan", &ts);
		}
		if (np->n_monlink.le_next != NFSNOLIST) {
			LIST_REMOVE(np, n_monlink);
			np->n_monlink.le_next = NFSNOLIST;
		}
	}
	lck_mtx_unlock(&nmp->nm_lock);

	return (error);
}

/*
 * Send a vnode notification for the given events.
 */
void
nfs_vnode_notify(nfsnode_t np, uint32_t events)
{
	struct nfsmount *nmp = NFSTONMP(np);
	struct nfs_vattr nvattr;
	struct vnode_attr vattr, *vap = NULL;
	struct timeval now;

	microuptime(&now);
	if ((np->n_evtstamp == now.tv_sec) || !nmp) {
		/* delay sending this notify */
		np->n_events |= events;
		return;
	}
	events |= np->n_events;
	np->n_events = 0;
	np->n_evtstamp = now.tv_sec;

	vfs_get_notify_attributes(&vattr);
	if (!nfs_getattrcache(np, &nvattr, 0)) {
		vap = &vattr;
		VATTR_INIT(vap);
		VATTR_RETURN(vap, va_fsid, vfs_statfs(nmp->nm_mountp)->f_fsid.val[0]);
		VATTR_RETURN(vap, va_fileid, nvattr.nva_fileid);
		VATTR_RETURN(vap, va_mode, nvattr.nva_mode);
		VATTR_RETURN(vap, va_uid, nvattr.nva_uid);
		VATTR_RETURN(vap, va_gid, nvattr.nva_gid);
		VATTR_RETURN(vap, va_nlink, nvattr.nva_nlink);
	}
	vnode_notify(NFSTOV(np), events, vap);
}
