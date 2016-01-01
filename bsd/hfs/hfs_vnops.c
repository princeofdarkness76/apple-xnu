/*
<<<<<<< HEAD
<<<<<<< HEAD
 * Copyright (c) 2000-2015 Apple Inc. All rights reserved.
=======
 * Copyright (c) 2000-2008 Apple Inc. All rights reserved.
>>>>>>> origin/10.5
=======
 * Copyright (c) 2000-2013 Apple Inc. All rights reserved.
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

#include <stdbool.h>
#include <sys/systm.h>
#include <sys/param.h>
#include <sys/kernel.h>
#include <sys/param.h>
#include <sys/file_internal.h>
#include <sys/dirent.h>
#include <sys/stat.h>
#include <sys/buf.h>
#include <sys/buf_internal.h>
#include <sys/mount.h>
#include <sys/vnode_if.h>
#include <sys/vnode_internal.h>
#include <sys/malloc.h>
#include <sys/ubc.h>
#include <sys/ubc_internal.h>
#include <sys/paths.h>
#include <sys/quota.h>
#include <sys/time.h>
#include <sys/disk.h>
#include <sys/kauth.h>
#include <sys/uio_internal.h>
#include <sys/fsctl.h>
#include <sys/xattr.h>
#include <string.h>
#include <sys/fsevents.h>
#include <kern/kalloc.h>

#include <miscfs/specfs/specdev.h>
#include <miscfs/fifofs/fifo.h>
#include <vfs/vfs_support.h>
#include <machine/spl.h>

#include <sys/kdebug.h>
#include <sys/sysctl.h>
#include <stdbool.h>

#include "hfs.h"
#include "hfs_catalog.h"
#include "hfs_cnode.h"
#include "hfs_dbg.h"
#include "hfs_mount.h"
#include "hfs_quota.h"
#include "hfs_endian.h"
#include "hfs_kdebug.h"
#include "hfs_cprotect.h"


#include "hfscommon/headers/BTreesInternal.h"
#include "hfscommon/headers/FileMgrInternal.h"

#define KNDETACH_VNLOCKED 0x00000001

/* Global vfs data structures for hfs */

/* Always F_FULLFSYNC? 1=yes,0=no (default due to "various" reasons is 'no') */
int always_do_fullfsync = 0;
SYSCTL_DECL(_vfs_generic);
SYSCTL_INT (_vfs_generic, OID_AUTO, always_do_fullfsync, CTLFLAG_RW | CTLFLAG_LOCKED, &always_do_fullfsync, 0, "always F_FULLFSYNC when fsync is called");

<<<<<<< HEAD
int hfs_makenode(struct vnode *dvp, struct vnode **vpp,
                        struct componentname *cnp, struct vnode_attr *vap,
                        vfs_context_t ctx);
int hfs_metasync(struct hfsmount *hfsmp, daddr64_t node, __unused struct proc *p);
int hfs_metasync_all(struct hfsmount *hfsmp);

int hfs_removedir(struct vnode *, struct vnode *, struct componentname *,
                         int, int);
int hfs_removefile(struct vnode *, struct vnode *, struct componentname *,
                          int, int, int, struct vnode *, int);

/* Used here and in cnode teardown -- for symlinks */
int hfs_removefile_callback(struct buf *bp, void *hfsmp);

enum {
	HFS_MOVE_DATA_INCLUDE_RSRC		= 1,
};
typedef uint32_t hfs_move_data_options_t;
=======
extern unsigned long strtoul(const char *, char **, int);

/* Global vfs data structures for hfs */
>>>>>>> origin/10.2

static int hfs_move_data(cnode_t *from_cp, cnode_t *to_cp, 
						 hfs_move_data_options_t options);
static int hfs_move_fork(filefork_t *srcfork, cnode_t *src, 
 						 filefork_t *dstfork, cnode_t *dst);

#if HFS_COMPRESSION
static int hfs_move_compressed(cnode_t *from_vp, cnode_t *to_vp);
#endif

decmpfs_cnode* hfs_lazy_init_decmpfs_cnode (struct cnode *cp);

#if FIFO
static int hfsfifo_read(struct vnop_read_args *);
static int hfsfifo_write(struct vnop_write_args *);
static int hfsfifo_close(struct vnop_close_args *);

extern int (**fifo_vnodeop_p)(void *);
#endif /* FIFO */

int hfs_vnop_close(struct vnop_close_args*);
int hfs_vnop_create(struct vnop_create_args*);
int hfs_vnop_exchange(struct vnop_exchange_args*);
int hfs_vnop_fsync(struct vnop_fsync_args*);
int hfs_vnop_mkdir(struct vnop_mkdir_args*);
int hfs_vnop_mknod(struct vnop_mknod_args*);
int hfs_vnop_getattr(struct vnop_getattr_args*);
int hfs_vnop_open(struct vnop_open_args*);
int hfs_vnop_readdir(struct vnop_readdir_args*);
int hfs_vnop_remove(struct vnop_remove_args*);
int hfs_vnop_rename(struct vnop_rename_args*);
int hfs_vnop_rmdir(struct vnop_rmdir_args*);
int hfs_vnop_symlink(struct vnop_symlink_args*);
int hfs_vnop_setattr(struct vnop_setattr_args*);
int hfs_vnop_readlink(struct vnop_readlink_args *);
int hfs_vnop_pathconf(struct vnop_pathconf_args *);
int hfs_vnop_mmap(struct vnop_mmap_args *ap);
int hfsspec_read(struct vnop_read_args *);
int hfsspec_write(struct vnop_write_args *);
int hfsspec_close(struct vnop_close_args *);

/* Options for hfs_removedir and hfs_removefile */
#define HFSRM_SKIP_RESERVE  0x01



/*****************************************************************************
*
* Common Operations on vnodes
*
*****************************************************************************/

/*
 * Is the given cnode either the .journal or .journal_info_block file on
 * a volume with an active journal?  Many VNOPs use this to deny access
 * to those files.
 *
 * Note: the .journal file on a volume with an external journal still
 * returns true here, even though it does not actually hold the contents
 * of the volume's journal.
 */
static _Bool
hfs_is_journal_file(struct hfsmount *hfsmp, struct cnode *cp)
{
	if (hfsmp->jnl != NULL &&
	    (cp->c_fileid == hfsmp->hfs_jnlinfoblkid ||
	     cp->c_fileid == hfsmp->hfs_jnlfileid)) {
		return true;
	} else {
		return false;
	}
}

/*
 * Create a regular file.
 */
int
hfs_vnop_create(struct vnop_create_args *ap)
{
	/*
	 * We leave handling of certain race conditions here to the caller
	 * which will have a better understanding of the semantics it
	 * requires.  For example, if it turns out that the file exists,
	 * it would be wrong of us to return a reference to the existing
	 * file because the caller might not want that and it would be
	 * misleading to suggest the file had been created when it hadn't
	 * been.  Note that our NFS server code does not set the
	 * VA_EXCLUSIVE flag so you cannot assume that callers don't want
	 * EEXIST errors if it's not set.  The common case, where users
	 * are calling open with the O_CREAT mode, is handled in VFS; when
	 * we return EEXIST, it will loop and do the look-up again.
	 */
	return hfs_makenode(ap->a_dvp, ap->a_vpp, ap->a_cnp, ap->a_vap, ap->a_context);
}

/*
 * Make device special file.
 */
int
hfs_vnop_mknod(struct vnop_mknod_args *ap)
{
	struct vnode_attr *vap = ap->a_vap;
	struct vnode *dvp = ap->a_dvp;
	struct vnode **vpp = ap->a_vpp;
	struct cnode *cp;
	int error;

	if (VTOVCB(dvp)->vcbSigWord != kHFSPlusSigWord) {
		return (ENOTSUP);
	}

	/* Create the vnode */
	error = hfs_makenode(dvp, vpp, ap->a_cnp, vap, ap->a_context);
	if (error)
		return (error);

	cp = VTOC(*vpp);
	cp->c_touch_acctime = TRUE;
	cp->c_touch_chgtime = TRUE;
	cp->c_touch_modtime = TRUE;

	if ((vap->va_rdev != VNOVAL) &&
	    (vap->va_type == VBLK || vap->va_type == VCHR))
		cp->c_rdev = vap->va_rdev;

	return (0);
}

#if HFS_COMPRESSION
/* 
 *	hfs_ref_data_vp(): returns the data fork vnode for a given cnode. 
 *	In the (hopefully rare) case where the data fork vnode is not 
 *	present, it will use hfs_vget() to create a new vnode for the
 *	data fork. 
 *	
 *	NOTE: If successful and a vnode is returned, the caller is responsible
 *	for releasing the returned vnode with vnode_rele().
 */
static int
hfs_ref_data_vp(struct cnode *cp, struct vnode **data_vp, int skiplock)
{
	int vref = 0;

	if (!data_vp || !cp) /* sanity check incoming parameters */
		return EINVAL;
	
	/* maybe we should take the hfs cnode lock here, and if so, use the skiplock parameter to tell us not to */

	if (!skiplock) hfs_lock(cp, HFS_SHARED_LOCK, HFS_LOCK_DEFAULT);
	struct vnode *c_vp = cp->c_vp;
	if (c_vp) {
		/* we already have a data vnode */
		*data_vp = c_vp;
		vref = vnode_ref(*data_vp);
		if (!skiplock) hfs_unlock(cp);
		if (vref == 0) {
			return 0;
		}
		return EINVAL;
	}
	/* no data fork vnode in the cnode, so ask hfs for one. */

	if (!cp->c_rsrc_vp) {
		/* if we don't have either a c_vp or c_rsrc_vp, we can't really do anything useful */
		*data_vp = NULL;
		if (!skiplock) hfs_unlock(cp);
		return EINVAL;
	}
	
	if (0 == hfs_vget(VTOHFS(cp->c_rsrc_vp), cp->c_cnid, data_vp, 1, 0) &&
		0 != data_vp) {
		vref = vnode_ref(*data_vp);
		vnode_put(*data_vp);
		if (!skiplock) hfs_unlock(cp);
		if (vref == 0) {
			return 0;
		}
		return EINVAL;
	}
	/* there was an error getting the vnode */
	*data_vp = NULL;
	if (!skiplock) hfs_unlock(cp);
	return EINVAL;
}

/*
 *	hfs_lazy_init_decmpfs_cnode(): returns the decmpfs_cnode for a cnode,
 *	allocating it if necessary; returns NULL if there was an allocation error.
 *  function is non-static so that it can be used from the FCNTL handler.
 */
decmpfs_cnode *
hfs_lazy_init_decmpfs_cnode(struct cnode *cp)
{
	if (!cp->c_decmp) {
		decmpfs_cnode *dp = NULL;
		MALLOC_ZONE(dp, decmpfs_cnode *, sizeof(decmpfs_cnode), M_DECMPFS_CNODE, M_WAITOK);
		if (!dp) {
			/* error allocating a decmpfs cnode */
			return NULL;
		}
		decmpfs_cnode_init(dp);
		if (!OSCompareAndSwapPtr(NULL, dp, (void * volatile *)&cp->c_decmp)) {
			/* another thread got here first, so free the decmpfs_cnode we allocated */
			decmpfs_cnode_destroy(dp);
			FREE_ZONE(dp, sizeof(*dp), M_DECMPFS_CNODE);
		}
	}
	
	return cp->c_decmp;
}

/*
 *	hfs_file_is_compressed(): returns 1 if the file is compressed, and 0 (zero) if not.
 *	if the file's compressed flag is set, makes sure that the decmpfs_cnode field
 *	is allocated by calling hfs_lazy_init_decmpfs_cnode(), then makes sure it is populated,
 *	or else fills it in via the decmpfs_file_is_compressed() function.
 */
int
hfs_file_is_compressed(struct cnode *cp, int skiplock)
{
	int ret = 0;
	
	/* fast check to see if file is compressed. If flag is clear, just answer no */
	if (!(cp->c_bsdflags & UF_COMPRESSED)) {
		return 0;
	}

	decmpfs_cnode *dp = hfs_lazy_init_decmpfs_cnode(cp);
	if (!dp) {
		/* error allocating a decmpfs cnode, treat the file as uncompressed */
		return 0;
	}
	
	/* flag was set, see if the decmpfs_cnode state is valid (zero == invalid) */
	uint32_t decmpfs_state = decmpfs_cnode_get_vnode_state(dp);
	switch(decmpfs_state) {
		case FILE_IS_COMPRESSED:
		case FILE_IS_CONVERTING: /* treat decompressing files as if they are compressed */
			return 1;
		case FILE_IS_NOT_COMPRESSED:
			return 0;
		/* otherwise the state is not cached yet */
	}
	
	/* decmpfs hasn't seen this file yet, so call decmpfs_file_is_compressed() to init the decmpfs_cnode struct */
	struct vnode *data_vp = NULL;
	if (0 == hfs_ref_data_vp(cp, &data_vp, skiplock)) {
		if (data_vp) {
			ret = decmpfs_file_is_compressed(data_vp, VTOCMP(data_vp)); // fill in decmpfs_cnode
			vnode_rele(data_vp);
		}
	}
	return ret;
}

/*	hfs_uncompressed_size_of_compressed_file() - get the uncompressed size of the file.
 *	if the caller has passed a valid vnode (has a ref count > 0), then hfsmp and fid are not required.
 *	if the caller doesn't have a vnode, pass NULL in vp, and pass valid hfsmp and fid.
 *	files size is returned in size (required)
 *	if the indicated file is a directory (or something that doesn't have a data fork), then this call
 *	will return an error and the caller should fall back to treating the item as an uncompressed file
 */
int
hfs_uncompressed_size_of_compressed_file(struct hfsmount *hfsmp, struct vnode *vp, cnid_t fid, off_t *size, int skiplock)
{
	int ret = 0;
	int putaway = 0;									/* flag to remember if we used hfs_vget() */

	if (!size) {
		return EINVAL;									/* no place to put the file size */
	}

	if (NULL == vp) {
		if (!hfsmp || !fid) {							/* make sure we have the required parameters */
			return EINVAL;
		}
		if (0 != hfs_vget(hfsmp, fid, &vp, skiplock, 0)) {		/* vnode is null, use hfs_vget() to get it */
			vp = NULL;
		} else {
			putaway = 1;								/* note that hfs_vget() was used to aquire the vnode */
		}
	}
	/* this double check for compression (hfs_file_is_compressed)
	 * ensures the cached size is present in case decmpfs hasn't 
	 * encountered this node yet.
	 */
	if (vp) {
		if (hfs_file_is_compressed(VTOC(vp), skiplock) ) {
			*size = decmpfs_cnode_get_vnode_cached_size(VTOCMP(vp));	/* file info will be cached now, so get size */
		} else {
			if (VTOCMP(vp) && VTOCMP(vp)->cmp_type >= CMP_MAX) {
				if (VTOCMP(vp)->cmp_type != DATALESS_CMPFS_TYPE) {
					// if we don't recognize this type, just use the real data fork size
					if (VTOC(vp)->c_datafork) {
						*size = VTOC(vp)->c_datafork->ff_size;
						ret = 0;
					} else {
						ret = EINVAL;
					}
				} else {
					*size = decmpfs_cnode_get_vnode_cached_size(VTOCMP(vp));	/* file info will be cached now, so get size */
					ret = 0;
				}
			} else {
				ret = EINVAL;
			}
		}
	}
	
	if (putaway) {		/* did we use hfs_vget() to get this vnode? */
		vnode_put(vp);	/* if so, release it and set it to null */
		vp = NULL;
	}
	return ret;
}

int
hfs_hides_rsrc(vfs_context_t ctx, struct cnode *cp, int skiplock)
{
	if (ctx == decmpfs_ctx)
		return 0;
	if (!hfs_file_is_compressed(cp, skiplock))
		return 0;
	return decmpfs_hides_rsrc(ctx, cp->c_decmp);
}

int
hfs_hides_xattr(vfs_context_t ctx, struct cnode *cp, const char *name, int skiplock)
{
	if (ctx == decmpfs_ctx)
		return 0;
	if (!hfs_file_is_compressed(cp, skiplock))
		return 0;
	return decmpfs_hides_xattr(ctx, cp->c_decmp, name);
}
#endif /* HFS_COMPRESSION */
		

//
// This function gets the doc_tombstone structure for the
// current thread.  If the thread doesn't have one, the
// structure is allocated.
//
static struct doc_tombstone *
get_uthread_doc_tombstone(void)
{
	struct  uthread *ut;
	ut = get_bsdthread_info(current_thread());

	if (ut->t_tombstone == NULL) {
		ut->t_tombstone = kalloc(sizeof(struct doc_tombstone));
		if (ut->t_tombstone) {
			memset(ut->t_tombstone, 0, sizeof(struct doc_tombstone));
		}
	}
	
	return ut->t_tombstone;
}

//
// This routine clears out the current tombstone for the
// current thread and if necessary passes the doc-id of
// the tombstone on to the dst_cnode.
//
// If the doc-id transfers to dst_cnode, we also generate
// a doc-id changed fsevent.  Unlike all the other fsevents,
// doc-id changed events can only be generated here in HFS
// where we have the necessary info.
// 
static void
<<<<<<< HEAD
clear_tombstone_docid(struct  doc_tombstone *ut, __unused struct hfsmount *hfsmp, struct cnode *dst_cnode)
=======
clear_tombstone_docid(struct  doc_tombstone *ut, struct hfsmount *hfsmp, struct cnode *dst_cnode)
>>>>>>> origin/10.9
{
	uint32_t old_id = ut->t_lastop_document_id;

	ut->t_lastop_document_id = 0;
	ut->t_lastop_parent = NULL;
	ut->t_lastop_parent_vid = 0;
	ut->t_lastop_filename[0] = '\0';

	//
	// If the lastop item is still the same and needs to be cleared,
	// clear it.
	//
	if (dst_cnode && old_id && ut->t_lastop_item && vnode_vid(ut->t_lastop_item) == ut->t_lastop_item_vid) {
		//
		// clear the document_id from the file that used to have it.
		// XXXdbg - we need to lock the other vnode and make sure to
		// update it on disk.
		//
		struct cnode *ocp = VTOC(ut->t_lastop_item);
		struct FndrExtendedFileInfo *ofip = (struct FndrExtendedFileInfo *)((char *)&ocp->c_attr.ca_finderinfo + 16);

		// printf("clearing doc-id from ino %d\n", ocp->c_desc.cd_cnid);
		ofip->document_id = 0;
		ocp->c_bsdflags &= ~UF_TRACKED;
<<<<<<< HEAD
		ocp->c_flag |= C_MODIFIED;
=======
		ocp->c_flag |= C_MODIFIED | C_FORCEUPDATE;   // mark it dirty
>>>>>>> origin/10.9
		/* cat_update(hfsmp, &ocp->c_desc, &ocp->c_attr, NULL, NULL); */

	}

#if CONFIG_FSE
	if (dst_cnode && old_id) {
		struct FndrExtendedFileInfo *fip = (struct FndrExtendedFileInfo *)((char *)&dst_cnode->c_attr.ca_finderinfo + 16);

		add_fsevent(FSE_DOCID_CHANGED, vfs_context_current(),
			    FSE_ARG_DEV, hfsmp->hfs_raw_dev,
			    FSE_ARG_INO, (ino64_t)ut->t_lastop_fileid,    // src inode #
			    FSE_ARG_INO, (ino64_t)dst_cnode->c_fileid,    // dst inode #
			    FSE_ARG_INT32, (uint32_t)fip->document_id,
			    FSE_ARG_DONE);
	}
#endif
	// last, clear these now that we're all done
	ut->t_lastop_item     = NULL;
	ut->t_lastop_fileid   = 0;
	ut->t_lastop_item_vid = 0;
}


//
// This function is used to filter out operations on temp
// filenames.  We have to filter out operations on certain
// temp filenames to work-around questionable application
// behavior from apps like Autocad that perform unusual
// sequences of file system operations for a "safe save".
static int
is_ignorable_temp_name(const char *nameptr, int len)
{
	if (len == 0) {
		len = strlen(nameptr);
	}
	
	if (   strncmp(nameptr, "atmp", 4) == 0
	   || (len > 4 && strncmp(nameptr+len-4, ".bak", 4) == 0)
	   || (len > 4 && strncmp(nameptr+len-4, ".tmp", 4) == 0)) {
		return 1;
	}

	return 0;
}

//
// Decide if we need to save a tombstone or not.  Normally we always
// save a tombstone - but if there already is one and the name we're
// given is an ignorable name, then we will not save a tombstone.
// 
static int
should_save_docid_tombstone(struct doc_tombstone *ut, struct vnode *vp, struct componentname *cnp)
{
	if (cnp->cn_nameptr == NULL) {
		return 0;
	}

	if (ut->t_lastop_document_id && ut->t_lastop_item == vp && is_ignorable_temp_name(cnp->cn_nameptr, cnp->cn_namelen)) {
		return 0;
	}

	return 1;
}


//
// This function saves a tombstone for the given vnode and name.  The
// tombstone represents the parent directory and name where the document
// used to live and the document-id of that file.  This info is recorded
// in the doc_tombstone structure hanging off the uthread (which assumes
// that all safe-save operations happen on the same thread).
//
// If later on the same parent/name combo comes back into existence then
// we'll preserve the doc-id from this vnode onto the new vnode.
//
static void
save_tombstone(struct hfsmount *hfsmp, struct vnode *dvp, struct vnode *vp, struct componentname *cnp, int for_unlink)
{
	struct cnode *cp = VTOC(vp);
	struct  doc_tombstone *ut;
	ut = get_uthread_doc_tombstone();
				
	if (for_unlink && vp->v_type == VREG && cp->c_linkcount > 1) {
		//
		// a regular file that is being unlinked and that is also
		// hardlinked should not clear the UF_TRACKED state or
		// mess with the tombstone because somewhere else in the
		// file system the file is still alive.
		// 
		return;
	}

<<<<<<< HEAD
<<<<<<< HEAD
=======
>>>>>>> origin/10.9
	ut->t_lastop_parent     = dvp;
	ut->t_lastop_parent_vid = vnode_vid(dvp);
	ut->t_lastop_fileid     = cp->c_fileid;
	if (for_unlink) {
		ut->t_lastop_item      = NULL;
		ut->t_lastop_item_vid  = 0;
	} else {
		ut->t_lastop_item      = vp;
		ut->t_lastop_item_vid  = vnode_vid(vp);
<<<<<<< HEAD
=======
	if (cp->c_flags & (IMMUTABLE | APPEND))
		return (EPERM);

	// XXXdbg - don't allow modification of the journal or journal_info_block
	if (VTOHFS(vp)->jnl && cp->c_datafork) {
		struct HFSPlusExtentDescriptor *extd;

		extd = &cp->c_datafork->ff_data.cf_extents[0];
		if (extd->startBlock == VTOVCB(vp)->vcbJinfoBlock || extd->startBlock == VTOHFS(vp)->jnl_start) {
			return EPERM;
		}
	}

	/*
	 * Go through the fields and update iff not VNOVAL.
	 */
	if (vap->va_uid != (uid_t)VNOVAL || vap->va_gid != (gid_t)VNOVAL) {
		if (VTOVFS(vp)->mnt_flag & MNT_RDONLY)
			return (EROFS);
		if ((error = hfs_chown(vp, vap->va_uid, vap->va_gid, cred, p)))
			return (error);
	}
	if (vap->va_size != VNOVAL) {
		/*
		 * Disallow write attempts on read-only file systems;
		 * unless the file is a socket, fifo, or a block or
		 * character device resident on the file system.
		 */
		switch (vp->v_type) {
		case VDIR:
			return (EISDIR);
 		case VLNK:
		case VREG:
			if (VTOVFS(vp)->mnt_flag & MNT_RDONLY)
				return (EROFS);
                	break;
		default:
                	break;
		}
		if ((error = VOP_TRUNCATE(vp, vap->va_size, 0, cred, p)))
			return (error);
>>>>>>> origin/10.2
=======
>>>>>>> origin/10.9
	}
		
	strlcpy((char *)&ut->t_lastop_filename[0], cnp->cn_nameptr, sizeof(ut->t_lastop_filename));
		
	struct FndrExtendedFileInfo *fip = (struct FndrExtendedFileInfo *)((char *)&cp->c_attr.ca_finderinfo + 16);
	ut->t_lastop_document_id = fip->document_id;

	if (for_unlink) {
		// clear this so it's never returned again
		fip->document_id = 0;
		cp->c_bsdflags &= ~UF_TRACKED;

		if (ut->t_lastop_document_id) {
			(void) cat_update(hfsmp, &cp->c_desc, &cp->c_attr, NULL, NULL);

#if CONFIG_FSE
			// this event is more of a "pending-delete" 
			add_fsevent(FSE_DOCID_CHANGED, vfs_context_current(),
				    FSE_ARG_DEV, hfsmp->hfs_raw_dev,
				    FSE_ARG_INO, (ino64_t)cp->c_fileid,       // src inode #
				    FSE_ARG_INO, (ino64_t)0,                  // dst inode #
				    FSE_ARG_INT32, ut->t_lastop_document_id,  // document id
				    FSE_ARG_DONE);
#endif
		}
	}
}


/*
 * Open a file/directory.
 */
int
hfs_vnop_open(struct vnop_open_args *ap)
{
	struct vnode *vp = ap->a_vp;
	struct filefork *fp;
	struct timeval tv;
	int error;
	static int past_bootup = 0;
	struct cnode *cp = VTOC(vp);
	struct hfsmount *hfsmp = VTOHFS(vp);
	
#if HFS_COMPRESSION
	if (ap->a_mode & FWRITE) {
		/* open for write */
		if ( hfs_file_is_compressed(cp, 1) ) { /* 1 == don't take the cnode lock */
			/* opening a compressed file for write, so convert it to decompressed */
			struct vnode *data_vp = NULL;
			error = hfs_ref_data_vp(cp, &data_vp, 1); /* 1 == don't take the cnode lock */
			if (0 == error) {
				if (data_vp) {
					error = decmpfs_decompress_file(data_vp, VTOCMP(data_vp), -1, 1, 0);
					vnode_rele(data_vp);
				} else {
					error = EINVAL;
				}
			}
			if (error != 0)
				return error;
		}
	} else {
		/* open for read */
		if (hfs_file_is_compressed(cp, 1) ) { /* 1 == don't take the cnode lock */
			if (VNODE_IS_RSRC(vp)) {
				/* opening the resource fork of a compressed file, so nothing to do */
			} else {
				/* opening a compressed file for read, make sure it validates */
				error = decmpfs_validate_compressed_file(vp, VTOCMP(vp));
				if (error != 0)
					return error;
			}
		}
	}
#endif

	/*
	 * Files marked append-only must be opened for appending.
	 */
	if ((cp->c_bsdflags & APPEND) && !vnode_isdir(vp) &&
	    (ap->a_mode & (FWRITE | O_APPEND)) == FWRITE)
		return (EPERM);

	if (vnode_isreg(vp) && !UBCINFOEXISTS(vp))
		return (EBUSY);  /* file is in use by the kernel */

	/* Don't allow journal to be opened externally. */
	if (hfs_is_journal_file(hfsmp, cp))
		return (EPERM);

<<<<<<< HEAD
	bool have_lock = false;

#if CONFIG_PROTECT
	if (ISSET(ap->a_mode, FENCRYPTED) && cp->c_cpentry && vnode_isreg(vp)) {
		bool have_trunc_lock = false;


		if ((error = hfs_lock(cp, HFS_EXCLUSIVE_LOCK, HFS_LOCK_DEFAULT))) {
			if (have_trunc_lock)
				hfs_unlock_truncate(cp, 0);
			return error;
		}

		have_lock = true;

		if (cp->c_cpentry->cp_raw_open_count + 1
			< cp->c_cpentry->cp_raw_open_count) {
			// Overflow; too many raw opens on this file
			hfs_unlock(cp);
			if (have_trunc_lock)
				hfs_unlock_truncate(cp, 0);
			return ENFILE;
		}


		if (have_trunc_lock)
			hfs_unlock_truncate(cp, 0);

		++cp->c_cpentry->cp_raw_open_count;
	}
#endif

=======
>>>>>>> origin/10.7
	if ((hfsmp->hfs_flags & HFS_READ_ONLY) ||
	    (hfsmp->jnl == NULL) ||
#if NAMEDSTREAMS
	    !vnode_isreg(vp) || vnode_isinuse(vp, 0) || vnode_isnamedstream(vp)) {
#else
	    !vnode_isreg(vp) || vnode_isinuse(vp, 0)) {
#endif

#if CONFIG_PROTECT
		if (have_lock)
			hfs_unlock(cp);
#endif

		return (0);
	}

	if (!have_lock && (error = hfs_lock(cp, HFS_EXCLUSIVE_LOCK, HFS_LOCK_DEFAULT)))
		return (error);

#if QUOTA
	/* If we're going to write to the file, initialize quotas. */
	if ((ap->a_mode & FWRITE) && (hfsmp->hfs_flags & HFS_QUOTAS))
		(void)hfs_getinoquota(cp);
#endif /* QUOTA */

	/*
	 * On the first (non-busy) open of a fragmented
	 * file attempt to de-frag it (if its less than 20MB).
	 */
	fp = VTOF(vp);
	if (fp->ff_blocks &&
	    fp->ff_extents[7].blockCount != 0 &&
	    fp->ff_size <= (20 * 1024 * 1024)) {
		int no_mods = 0;
		struct timeval now;
		/* 
		 * Wait until system bootup is done (3 min).
		 * And don't relocate a file that's been modified
		 * within the past minute -- this can lead to
		 * system thrashing.
		 */

		if (!past_bootup) {
			microuptime(&tv);
			if (tv.tv_sec > (60*3)) {
				past_bootup = 1;
			}
		}
		
		microtime(&now);
		if ((now.tv_sec - cp->c_mtime) > 60) {	
			no_mods = 1;
		} 
		
		if (past_bootup && no_mods) {
			(void) hfs_relocate(vp, hfsmp->nextAllocation + 4096,
					vfs_context_ucred(ap->a_context),
					vfs_context_proc(ap->a_context));
		}
	}

	hfs_unlock(cp);

	return (0);
}


/*
 * Close a file/directory.
 */
int
hfs_vnop_close(ap)
	struct vnop_close_args /* {
		struct vnode *a_vp;
		int a_fflag;
		vfs_context_t a_context;
	} */ *ap;
{
	register struct vnode *vp = ap->a_vp;
 	register struct cnode *cp;
	struct proc *p = vfs_context_proc(ap->a_context);
	struct hfsmount *hfsmp;
	int busy;
<<<<<<< HEAD
	int tooktrunclock = 0;
	int knownrefs = 0;
=======
	int knownrefs = 0;
	int tooktrunclock = 0;
>>>>>>> origin/10.5

	if ( hfs_lock(VTOC(vp), HFS_EXCLUSIVE_LOCK, HFS_LOCK_DEFAULT) != 0)
		return (0);
	cp = VTOC(vp);
	hfsmp = VTOHFS(vp);

<<<<<<< HEAD
#if CONFIG_PROTECT
	if (cp->c_cpentry && ISSET(ap->a_fflag, FENCRYPTED) && vnode_isreg(vp)) {
		assert(cp->c_cpentry->cp_raw_open_count > 0);
		--cp->c_cpentry->cp_raw_open_count;
=======
	/*
	 * If the rsrc fork is a named stream, it holds a usecount on 
	 * the data fork, which prevents the data fork from getting recycled, which
	 * then prevents the de-allocation of its extra blocks.  
	 * Do checks for truncation on close. Purge extra extents if they
	 * exist.  Make sure the vp is not a directory, that it has a resource
	 * fork, and that rsrc fork is a named stream.
	 */
	
	if ((vp->v_type == VREG) && (cp->c_rsrc_vp)
			&& (vnode_isnamedstream(cp->c_rsrc_vp))) {
		uint32_t blks;

		blks = howmany(VTOF(vp)->ff_size, VTOVCB(vp)->blockSize);
		/*
		 *  If there are any extra blocks and there are only 2 refs on 
		 *  this vp (ourselves + rsrc fork holding ref on us), go ahead
		 *  and try to truncate the extra blocks away.
		 */
		if ((blks < VTOF(vp)->ff_blocks) && (!vnode_isinuse(vp, 2))) {
			// release cnode lock ; must acquire truncate lock BEFORE cnode lock
			hfs_unlock (cp);

			hfs_lock_truncate(cp, TRUE);
			tooktrunclock = 1;
			
			if (hfs_lock(VTOC(vp), HFS_EXCLUSIVE_LOCK) != 0) {
				hfs_unlock_truncate(cp, TRUE);
				return (0);			
			}

			//now re-test to make sure it's still valid.
			if (cp->c_rsrc_vp) {
				knownrefs = 1 + vnode_isnamedstream(cp->c_rsrc_vp);
				if (!vnode_isinuse(vp, knownrefs)) {
					blks = howmany(VTOF(vp)->ff_size, VTOVCB(vp)->blockSize);
					if (blks < VTOF(vp)->ff_blocks) {
						(void) hfs_truncate(vp, VTOF(vp)->ff_size, IO_NDELAY, 0, ap->a_context);
					}
				}
			}
		}
	}

	// if we froze the fs and we're exiting, then "thaw" the fs 
	if (hfsmp->hfs_freezing_proc == p && proc_exiting(p)) {
	    hfsmp->hfs_freezing_proc = NULL;
	    hfs_global_exclusive_lock_release(hfsmp);
	    lck_rw_unlock_exclusive(&hfsmp->hfs_insync);
>>>>>>> origin/10.5
	}
#endif

	/* 
	 * If the rsrc fork is a named stream, it can cause the data fork to
	 * stay around, preventing de-allocation of these blocks. 
	 * Do checks for truncation on close. Purge extra extents if they exist.
	 * Make sure the vp is not a directory, and that it has a resource fork,
	 * and that resource fork is also a named stream.
	 */

	if ((vp->v_type == VREG) && (cp->c_rsrc_vp)
			&& (vnode_isnamedstream(cp->c_rsrc_vp))) {
		uint32_t blks;

		blks = howmany(VTOF(vp)->ff_size, VTOVCB(vp)->blockSize);
		/*
		 * If there are extra blocks and there are only 2 refs on
		 * this vp (ourselves + rsrc fork holding ref on us), go ahead
		 * and try to truncate.
		 */
		if ((blks < VTOF(vp)->ff_blocks) && (!vnode_isinuse(vp, 2))) {
			// release cnode lock; must acquire truncate lock BEFORE cnode lock
			hfs_unlock(cp);

			hfs_lock_truncate(cp, HFS_EXCLUSIVE_LOCK, HFS_LOCK_DEFAULT);
			tooktrunclock = 1;

			if (hfs_lock(VTOC(vp), HFS_EXCLUSIVE_LOCK, HFS_LOCK_DEFAULT) != 0) { 
				hfs_unlock_truncate(cp, HFS_LOCK_DEFAULT);
				// bail out if we can't re-acquire cnode lock
				return 0;
			}
			// now re-test to make sure it's still valid
			if (cp->c_rsrc_vp) {
				knownrefs = 1 + vnode_isnamedstream(cp->c_rsrc_vp);
				if (!vnode_isinuse(vp, knownrefs)){
					// now we can truncate the file, if necessary
					blks = howmany(VTOF(vp)->ff_size, VTOVCB(vp)->blockSize);
					if (blks < VTOF(vp)->ff_blocks){
						(void) hfs_truncate(vp, VTOF(vp)->ff_size, IO_NDELAY,
											0, ap->a_context);
					}
				}
			}
		}
	}


	// if we froze the fs and we're exiting, then "thaw" the fs 
	if (hfsmp->hfs_freeze_state == HFS_FROZEN
	    && hfsmp->hfs_freezing_proc == p && proc_exiting(p)) {
		hfs_thaw(hfsmp, p);
	}

	busy = vnode_isinuse(vp, 1);

	if (busy) {
		hfs_touchtimes(VTOHFS(vp), cp);	
	}
	if (vnode_isdir(vp)) {
		hfs_reldirhints(cp, busy);
	} else if (vnode_issystem(vp) && !busy) {
		vnode_recycle(vp);
	}
<<<<<<< HEAD

	if (tooktrunclock){
		hfs_unlock_truncate(cp, HFS_LOCK_DEFAULT);
	}
=======
	if (tooktrunclock) {
		hfs_unlock_truncate(cp, TRUE);
	}
	
>>>>>>> origin/10.5
	hfs_unlock(cp);

	if (ap->a_fflag & FWASWRITTEN) {
		hfs_sync_ejectable(hfsmp);
	}

	return (0);
}

static bool hfs_should_generate_document_id(hfsmount_t *hfsmp, cnode_t *cp)
{
	return (!ISSET(hfsmp->hfs_flags, HFS_READ_ONLY)
			&& ISSET(cp->c_bsdflags, UF_TRACKED)
			&& cp->c_desc.cd_cnid != kHFSRootFolderID
			&& (S_ISDIR(cp->c_mode) || S_ISREG(cp->c_mode) || S_ISLNK(cp->c_mode)));
}

/*
 * Get basic attributes.
 */
int
hfs_vnop_getattr(struct vnop_getattr_args *ap)
{
#define VNODE_ATTR_TIMES  \
	(VNODE_ATTR_va_access_time|VNODE_ATTR_va_change_time|VNODE_ATTR_va_modify_time)
#define VNODE_ATTR_AUTH  \
	(VNODE_ATTR_va_mode | VNODE_ATTR_va_uid | VNODE_ATTR_va_gid | \
         VNODE_ATTR_va_flags | VNODE_ATTR_va_acl)

	struct vnode *vp = ap->a_vp;
	struct vnode_attr *vap = ap->a_vap;
	struct vnode *rvp = NULLVP;
	struct hfsmount *hfsmp;
	struct cnode *cp;
	uint64_t data_size;
	enum vtype v_type;
	int error = 0;
	cp = VTOC(vp);

#if HFS_COMPRESSION
	/* we need to inspect the decmpfs state of the file before we take the hfs cnode lock */
	int compressed = 0;
	int hide_size = 0;
	off_t uncompressed_size = -1;
	if (VATTR_IS_ACTIVE(vap, va_data_size) || VATTR_IS_ACTIVE(vap, va_total_alloc) || VATTR_IS_ACTIVE(vap, va_data_alloc) || VATTR_IS_ACTIVE(vap, va_total_size)) {
		/* we only care about whether the file is compressed if asked for the uncompressed size */
		if (VNODE_IS_RSRC(vp)) {
			/* if it's a resource fork, decmpfs may want us to hide the size */
			hide_size = hfs_hides_rsrc(ap->a_context, cp, 0);
		} else {
			/* if it's a data fork, we need to know if it was compressed so we can report the uncompressed size */
			compressed = hfs_file_is_compressed(cp, 0);
		}
		if ((VATTR_IS_ACTIVE(vap, va_data_size) || VATTR_IS_ACTIVE(vap, va_total_size))) {
			// if it's compressed 
			if (compressed || (!VNODE_IS_RSRC(vp) && cp->c_decmp && cp->c_decmp->cmp_type >= CMP_MAX)) {
				if (0 != hfs_uncompressed_size_of_compressed_file(NULL, vp, 0, &uncompressed_size, 0)) {
					/* failed to get the uncompressed size, we'll check for this later */
					uncompressed_size = -1;
				} else {
					// fake that it's compressed
					compressed = 1;
				}
			}
		}
	}
#endif

	/*
	 * Shortcut for vnode_authorize path.  Each of the attributes
	 * in this set is updated atomically so we don't need to take
	 * the cnode lock to access them.
	 */
	if ((vap->va_active & ~VNODE_ATTR_AUTH) == 0) {
		/* Make sure file still exists. */
		if (cp->c_flag & C_NOEXISTS)
			return (ENOENT);

		vap->va_uid = cp->c_uid;
		vap->va_gid = cp->c_gid;
		vap->va_mode = cp->c_mode;
		vap->va_flags = cp->c_bsdflags;
		vap->va_supported |= VNODE_ATTR_AUTH & ~VNODE_ATTR_va_acl;

		if ((cp->c_attr.ca_recflags & kHFSHasSecurityMask) == 0) {
			vap->va_acl = (kauth_acl_t) KAUTH_FILESEC_NONE;
			VATTR_SET_SUPPORTED(vap, va_acl);
		}
	
		return (0);
	}

	hfsmp = VTOHFS(vp);
	v_type = vnode_vtype(vp);

	if (VATTR_IS_ACTIVE(vap, va_document_id)) {
		uint32_t document_id;

		if (cp->c_desc.cd_cnid == kHFSRootFolderID)
			document_id = kHFSRootFolderID;
		else {
			/*
			 * This is safe without a lock because we're just reading
			 * a 32 bit aligned integer which should be atomic on all
			 * platforms we support.
			 */
			document_id = hfs_get_document_id(cp);

			if (!document_id && hfs_should_generate_document_id(hfsmp, cp)) {
				uint32_t new_document_id;

				error = hfs_generate_document_id(hfsmp, &new_document_id);
				if (error)
					return error;

				error = hfs_lock(cp, HFS_EXCLUSIVE_LOCK, HFS_LOCK_DEFAULT);
				if (error)
					return error;

				bool want_docid_fsevent = false;

				// Need to check again now that we have the lock
				document_id = hfs_get_document_id(cp);
				if (!document_id && hfs_should_generate_document_id(hfsmp, cp)) {
					cp->c_attr.ca_finderextendeddirinfo.document_id = document_id = new_document_id;
					want_docid_fsevent = true;
					SET(cp->c_flag, C_MODIFIED);
				}

				hfs_unlock(cp);

				if (want_docid_fsevent) {
#if CONFIG_FSE
					add_fsevent(FSE_DOCID_CHANGED, ap->a_context,
								FSE_ARG_DEV,   hfsmp->hfs_raw_dev,
								FSE_ARG_INO,   (ino64_t)0,             // src inode #
								FSE_ARG_INO,   (ino64_t)cp->c_fileid,  // dst inode #
								FSE_ARG_INT32, document_id,
								FSE_ARG_DONE);

					if (need_fsevent(FSE_STAT_CHANGED, vp)) {
						add_fsevent(FSE_STAT_CHANGED, ap->a_context, 
									FSE_ARG_VNODE, vp, FSE_ARG_DONE);
					}
#endif
				}
			}
		}

		vap->va_document_id = document_id;
		VATTR_SET_SUPPORTED(vap, va_document_id);
	}

	/*
	 * If time attributes are requested and we have cnode times
	 * that require updating, then acquire an exclusive lock on
	 * the cnode before updating the times.  Otherwise we can
	 * just acquire a shared lock.
	 */
	if ((vap->va_active & VNODE_ATTR_TIMES) &&
	    (cp->c_touch_acctime || cp->c_touch_chgtime || cp->c_touch_modtime)) {
		if ((error = hfs_lock(cp, HFS_EXCLUSIVE_LOCK, HFS_LOCK_DEFAULT)))
			return (error);
		hfs_touchtimes(hfsmp, cp);

		// downgrade to a shared lock since that's all we need from here on out
		cp->c_lockowner = HFS_SHARED_OWNER;
		lck_rw_lock_exclusive_to_shared(&cp->c_rwlock);

	} else if ((error = hfs_lock(cp, HFS_SHARED_LOCK, HFS_LOCK_DEFAULT))) {
		return (error);
	}

	if (v_type == VDIR) {
		data_size = (cp->c_entries + 2) * AVERAGE_HFSDIRENTRY_SIZE;

		if (VATTR_IS_ACTIVE(vap, va_nlink)) {
			int nlink;
	
			/*
			 * For directories, the va_nlink is esentially a count
			 * of the ".." references to a directory plus the "."
			 * reference and the directory itself. So for HFS+ this
			 * becomes the sub-directory count plus two.
			 *
			 * In the absence of a sub-directory count we use the
			 * directory's item count.  This will be too high in
			 * most cases since it also includes files.
			 */
			if ((hfsmp->hfs_flags & HFS_FOLDERCOUNT) && 
			    (cp->c_attr.ca_recflags & kHFSHasFolderCountMask))
				nlink = cp->c_attr.ca_dircount;  /* implied ".." entries */
			else
				nlink = cp->c_entries;

			/* Account for ourself and our "." entry */
			nlink += 2;  
			 /* Hide our private directories. */
			if (cp->c_cnid == kHFSRootFolderID) {
				if (hfsmp->hfs_private_desc[FILE_HARDLINKS].cd_cnid != 0) {
					--nlink;    
				}
				if (hfsmp->hfs_private_desc[DIR_HARDLINKS].cd_cnid != 0) {
					--nlink;
				}
			}
			VATTR_RETURN(vap, va_nlink, (u_int64_t)nlink);
		}		
		if (VATTR_IS_ACTIVE(vap, va_nchildren)) {
			int entries;
	
			entries = cp->c_entries;
			/* Hide our private files and directories. */
			if (cp->c_cnid == kHFSRootFolderID) {
				if (hfsmp->hfs_private_desc[FILE_HARDLINKS].cd_cnid != 0)
					--entries;
				if (hfsmp->hfs_private_desc[DIR_HARDLINKS].cd_cnid != 0)
					--entries;
				if (hfsmp->jnl || ((hfsmp->vcbAtrb & kHFSVolumeJournaledMask) && (hfsmp->hfs_flags & HFS_READ_ONLY)))
					entries -= 2;   /* hide the journal files */
			}
			VATTR_RETURN(vap, va_nchildren, entries);
		}
		/*
		 * The va_dirlinkcount is the count of real directory hard links.
		 * (i.e. its not the sum of the implied "." and ".." references)
		 */
		if (VATTR_IS_ACTIVE(vap, va_dirlinkcount)) {
			VATTR_RETURN(vap, va_dirlinkcount, (uint32_t)cp->c_linkcount);
		}
	} else /* !VDIR */ {
		data_size = VCTOF(vp, cp)->ff_size;

		VATTR_RETURN(vap, va_nlink, (u_int64_t)cp->c_linkcount);
		if (VATTR_IS_ACTIVE(vap, va_data_alloc)) {
			u_int64_t blocks;
	
#if HFS_COMPRESSION
			if (hide_size) {
				VATTR_RETURN(vap, va_data_alloc, 0);
			} else if (compressed) {
				/* for compressed files, we report all allocated blocks as belonging to the data fork */
				blocks = cp->c_blocks;
				VATTR_RETURN(vap, va_data_alloc, blocks * (u_int64_t)hfsmp->blockSize);
			}
			else
#endif
			{
				blocks = VCTOF(vp, cp)->ff_blocks;
				VATTR_RETURN(vap, va_data_alloc, blocks * (u_int64_t)hfsmp->blockSize);
			}
		}
	}

	/* conditional because 64-bit arithmetic can be expensive */
	if (VATTR_IS_ACTIVE(vap, va_total_size)) {
		if (v_type == VDIR) {
			VATTR_RETURN(vap, va_total_size, (cp->c_entries + 2) * AVERAGE_HFSDIRENTRY_SIZE);
		} else {
			u_int64_t total_size = ~0ULL;
			struct cnode *rcp;
#if HFS_COMPRESSION
			if (hide_size) {
				/* we're hiding the size of this file, so just return 0 */
				total_size = 0;
			} else if (compressed) {
				if (uncompressed_size == -1) {
					/*
					 * We failed to get the uncompressed size above,
					 * so we'll fall back to the standard path below
					 * since total_size is still -1
					 */
				} else {
					/* use the uncompressed size we fetched above */
					total_size = uncompressed_size;
				}
			}
<<<<<<< HEAD
#endif
			if (total_size == ~0ULL) {
				if (cp->c_datafork) {
					total_size = cp->c_datafork->ff_size;
				}
				
				if (cp->c_blocks - VTOF(vp)->ff_blocks) {
					/* We deal with rsrc fork vnode iocount at the end of the function */
<<<<<<< HEAD
					error = hfs_vgetrsrc(hfsmp, vp, &rvp);
					if (error) {
						/*
						 * Note that we call hfs_vgetrsrc with error_on_unlinked
						 * set to FALSE.  This is because we may be invoked via
						 * fstat() on an open-unlinked file descriptor and we must 
						 * continue to support access to the rsrc fork until it disappears.
						 * The code at the end of this function will be
						 * responsible for releasing the iocount generated by 
						 * hfs_vgetrsrc.  This is because we can't drop the iocount
						 * without unlocking the cnode first.
=======
					error = hfs_vgetrsrc(hfsmp, vp, &rvp, TRUE, TRUE);
					if (error) {
						/* 
						 * hfs_vgetrsrc may have returned a vnode in rvp even though
						 * we got an error, because we specified error_on_unlinked.
						 * We need to drop the iocount after we release the cnode lock, so
						 * it will be taken care of at the end of the function if it's needed.
>>>>>>> origin/10.6
						 */
						goto out;
					}
					
					rcp = VTOC(rvp);
					if (rcp && rcp->c_rsrcfork) {
						total_size += rcp->c_rsrcfork->ff_size;
					}
=======

			if (cp->c_blocks - VTOF(vp)->ff_blocks) {
				/* We deal with resource fork vnode iocount at the end of the function */
				error = hfs_vgetrsrc(hfsmp, vp, &rvp, TRUE);
				if (error) {
					goto out;
				}
				rcp = VTOC(rvp);
				if (rcp && rcp->c_rsrcfork) {
					total_size += rcp->c_rsrcfork->ff_size;
>>>>>>> origin/10.5
				}
			}
			
			VATTR_RETURN(vap, va_total_size, total_size);
		}
	}
	if (VATTR_IS_ACTIVE(vap, va_total_alloc)) {
		if (v_type == VDIR) {
			VATTR_RETURN(vap, va_total_alloc, 0);
		} else {
			VATTR_RETURN(vap, va_total_alloc, (u_int64_t)cp->c_blocks * (u_int64_t)hfsmp->blockSize);
		}
	}

	/*
	 * If the VFS wants extended security data, and we know that we
	 * don't have any (because it never told us it was setting any)
	 * then we can return the supported bit and no data.  If we do
	 * have extended security, we can just leave the bit alone and
	 * the VFS will use the fallback path to fetch it.
	 */
	if (VATTR_IS_ACTIVE(vap, va_acl)) {
		if ((cp->c_attr.ca_recflags & kHFSHasSecurityMask) == 0) {
			vap->va_acl = (kauth_acl_t) KAUTH_FILESEC_NONE;
			VATTR_SET_SUPPORTED(vap, va_acl);
		}
	}

	vap->va_access_time.tv_sec = cp->c_atime;
	vap->va_access_time.tv_nsec = 0;
	vap->va_create_time.tv_sec = cp->c_itime;
	vap->va_create_time.tv_nsec = 0;
	vap->va_modify_time.tv_sec = cp->c_mtime;
	vap->va_modify_time.tv_nsec = 0;
	vap->va_change_time.tv_sec = cp->c_ctime;
	vap->va_change_time.tv_nsec = 0;
	vap->va_backup_time.tv_sec = cp->c_btime;
	vap->va_backup_time.tv_nsec = 0;	

	/* See if we need to emit the date added field to the user */
	if (VATTR_IS_ACTIVE(vap, va_addedtime)) {
		u_int32_t dateadded = hfs_get_dateadded (cp);
		if (dateadded) {
			vap->va_addedtime.tv_sec = dateadded;
			vap->va_addedtime.tv_nsec = 0;
			VATTR_SET_SUPPORTED (vap, va_addedtime);
		}
	}

	/* XXX is this really a good 'optimal I/O size'? */
	vap->va_iosize = hfsmp->hfs_logBlockSize;
	vap->va_uid = cp->c_uid;
	vap->va_gid = cp->c_gid;
	vap->va_mode = cp->c_mode;
	vap->va_flags = cp->c_bsdflags;

	/*
	 * Exporting file IDs from HFS Plus:
	 *
	 * For "normal" files the c_fileid is the same value as the
	 * c_cnid.  But for hard link files, they are different - the
	 * c_cnid belongs to the active directory entry (ie the link)
	 * and the c_fileid is for the actual inode (ie the data file).
	 *
	 * The stat call (getattr) uses va_fileid and the Carbon APIs,
	 * which are hardlink-ignorant, will ask for va_linkid.
	 */
	vap->va_fileid = (u_int64_t)cp->c_fileid;
	/* 
	 * We need to use the origin cache for both hardlinked files 
	 * and directories. Hardlinked directories have multiple cnids 
	 * and parents (one per link). Hardlinked files also have their 
	 * own parents and link IDs separate from the indirect inode number. 
	 * If we don't use the cache, we could end up vending the wrong ID 
	 * because the cnode will only reflect the link that was looked up most recently.
	 */
	if (cp->c_flag & C_HARDLINK) {
		vap->va_linkid = (u_int64_t)hfs_currentcnid(cp);
		vap->va_parentid = (u_int64_t)hfs_currentparent(cp, /* have_lock: */ true);
	} else {
		vap->va_linkid = (u_int64_t)cp->c_cnid;
		vap->va_parentid = (u_int64_t)cp->c_parentcnid;
	}
	vap->va_fsid = hfsmp->hfs_raw_dev;
	vap->va_filerev = 0;
	vap->va_encoding = cp->c_encoding;
	vap->va_rdev = (v_type == VBLK || v_type == VCHR) ? cp->c_rdev : 0;
#if HFS_COMPRESSION
	if (VATTR_IS_ACTIVE(vap, va_data_size)) {
		if (hide_size)
			vap->va_data_size = 0;
		else if (compressed) {
			if (uncompressed_size == -1) {
				/* failed to get the uncompressed size above, so just return data_size */
				vap->va_data_size = data_size;
			} else {
				/* use the uncompressed size we fetched above */
				vap->va_data_size = uncompressed_size;
			}
		} else
			vap->va_data_size = data_size;
		VATTR_SET_SUPPORTED(vap, va_data_size);
	}
#else
	vap->va_data_size = data_size;
	vap->va_supported |= VNODE_ATTR_va_data_size;
#endif

<<<<<<< HEAD
#if CONFIG_PROTECT
	if (VATTR_IS_ACTIVE(vap, va_dataprotect_class)) {
    		vap->va_dataprotect_class = cp->c_cpentry ? CP_CLASS(cp->c_cpentry->cp_pclass) : 0;
    		VATTR_SET_SUPPORTED(vap, va_dataprotect_class);
    	} 	
#endif
	if (VATTR_IS_ACTIVE(vap, va_write_gencount)) {
		if (ubc_is_mapped_writable(vp)) {
			/*
			 * Return 0 to the caller to indicate the file may be
			 * changing.  There is no need for us to increment the
			 * generation counter here because it gets done as part of
			 * page-out and also when the file is unmapped (to account
			 * for changes we might not have seen).
			 */
			vap->va_write_gencount = 0;
		} else {
			vap->va_write_gencount = hfs_get_gencount(cp);
		}

		VATTR_SET_SUPPORTED(vap, va_write_gencount);
=======
	if (VATTR_IS_ACTIVE(vap, va_gen)) {
		if (UBCINFOEXISTS(vp) && (vp->v_ubcinfo->ui_flags & UI_ISMAPPED)) {
			/* While file is mmapped the generation count is invalid. 
		 	 * However, bump the value so that the write-gen counter 
		 	 * will be different once the file is unmapped (since,
		 	 * when unmapped the pageouts may not yet have happened)
		 	 */
			if (vp->v_ubcinfo->ui_flags & UI_MAPPEDWRITE) {
			     	hfs_incr_gencount (cp);
			}
			vap->va_gen = 0;
		} else {
			vap->va_gen = hfs_get_gencount(cp);
		}
			
		VATTR_SET_SUPPORTED(vap, va_gen);
	}
	if (VATTR_IS_ACTIVE(vap, va_document_id)) {
		vap->va_document_id = hfs_get_document_id(cp);
		VATTR_SET_SUPPORTED(vap, va_document_id);
>>>>>>> origin/10.9
	}

	/* Mark them all at once instead of individual VATTR_SET_SUPPORTED calls. */
	vap->va_supported |= VNODE_ATTR_va_access_time |
						 VNODE_ATTR_va_create_time | VNODE_ATTR_va_modify_time |
	                     VNODE_ATTR_va_change_time| VNODE_ATTR_va_backup_time |
	                     VNODE_ATTR_va_iosize | VNODE_ATTR_va_uid |
	                     VNODE_ATTR_va_gid | VNODE_ATTR_va_mode |
	                     VNODE_ATTR_va_flags |VNODE_ATTR_va_fileid |
	                     VNODE_ATTR_va_linkid | VNODE_ATTR_va_parentid |
	                     VNODE_ATTR_va_fsid | VNODE_ATTR_va_filerev |
	                     VNODE_ATTR_va_encoding | VNODE_ATTR_va_rdev;

<<<<<<< HEAD
	/* If this is the root, let VFS to find out the mount name, which 
	 * may be different from the real name.  Otherwise, we need to take care
	 * for hardlinked files, which need to be looked up, if necessary 
=======
	/* If this is the root, let VFS to find out the mount name, which may be different from the real name.
	 * Otherwise, we need to just take care for hardlinked files, which need to be looked up, if necessary
>>>>>>> origin/10.5
	 */
	if (VATTR_IS_ACTIVE(vap, va_name) && (cp->c_cnid != kHFSRootFolderID)) {
		struct cat_desc linkdesc;
		int lockflags;
		int uselinkdesc = 0;
		cnid_t nextlinkid = 0;
<<<<<<< HEAD
		cnid_t prevlinkid = 0;
=======
		cnid_t prevlinkid = 0;	
>>>>>>> origin/10.5

		/* Get the name for ATTR_CMN_NAME.  We need to take special care for hardlinks      
		 * here because the info. for the link ID requested by getattrlist may be
		 * different than what's currently in the cnode.  This is because the cnode     
		 * will be filled in with the information for the most recent link ID that went
		 * through namei/lookup().  If there are competing lookups for hardlinks that point 
<<<<<<< HEAD
	 	 * to the same inode, one (or more) getattrlists could be vended incorrect name information.
		 * Also, we need to beware of open-unlinked files which could have a namelen of 0.     
		 */

		if ((cp->c_flag & C_HARDLINK) && 
				((cp->c_desc.cd_namelen == 0) || (vap->va_linkid != cp->c_cnid))) {
			/* 
			 * If we have no name and our link ID is the raw inode number, then we may
			 * have an open-unlinked file.  Go to the next link in this case.
			 */
			if ((cp->c_desc.cd_namelen == 0) && (vap->va_linkid == cp->c_fileid)) {
				if ((error = hfs_lookup_siblinglinks(hfsmp, vap->va_linkid, &prevlinkid, &nextlinkid))){
					goto out;
				}
			}	
			else {
				/* just use link obtained from vap above */
				nextlinkid = vap->va_linkid;
			}

			/* We need to probe the catalog for the descriptor corresponding to the link ID
			 * stored in nextlinkid.  Note that we don't know if we have the exclusive lock
			 * for the cnode here, so we can't just update the descriptor.  Instead,
			 * we should just store the descriptor's value locally and then use it to pass
			 * out the name value as needed below. 
			 */ 
			if (nextlinkid){
				lockflags = hfs_systemfile_lock(hfsmp, SFL_CATALOG, HFS_SHARED_LOCK);
				error = cat_findname(hfsmp, nextlinkid, &linkdesc);
				hfs_systemfile_unlock(hfsmp, lockflags);	
=======
		 * to the same inode, one (or more) getattrlists could be vended incorrect name information.
		 * Also, we need to beware of open-unlinked files which could have a namelen of 0.  Note
		 * that if another hardlink sibling of this file is being unlinked, that could also thrash
		 * the name fields but it should *not* be treated like an open-unlinked file here.
		 */
		if ((cp->c_flag & C_HARDLINK) &&
				((cp->c_desc.cd_namelen == 0) || (vap->va_linkid != cp->c_cnid))) {
			/* If we have no name and our linkID is the raw inode number, then we may
			 * have an open-unlinked file.  Go to the next link in this case. 
			 */
			if ((cp->c_desc.cd_namelen == 0) && (vap->va_linkid == cp->c_fileid)) {
				if ((error = hfs_lookuplink(hfsmp, vap->va_linkid, &prevlinkid, &nextlinkid))) {
					goto out;
				}
			}
			else {
				nextlinkid = vap->va_linkid;
			}
			/* Now probe the catalog for the linkID.  Note that we don't know if we have
			 * the exclusive lock here for the cnode, so we can't just update the descriptor.  
			 * Instead, we should just store the descriptor's value locally and then use it to pass
			 * out the name value as needed below.
			 */
			if (nextlinkid) {
				lockflags = hfs_systemfile_lock(hfsmp, SFL_CATALOG, HFS_SHARED_LOCK);
				error = cat_findname(hfsmp, nextlinkid, &linkdesc);	
				hfs_systemfile_unlock(hfsmp, lockflags);
>>>>>>> origin/10.5
				if (error == 0) {
					uselinkdesc = 1;
				}
			}
		}
<<<<<<< HEAD

		/* By this point, we've either patched up the name above and the c_desc
		 * points to the correct data, or it already did, in which case we just proceed
		 * by copying the name into the vap.  Note that we will never set va_name to
		 * supported if nextlinkid is never initialized.  This could happen in the degenerate
		 * case above involving the raw inode number, where it has no nextlinkid.  In this case
		 * we will simply not mark the name bit as supported.
		 */
		if (uselinkdesc) {
			strlcpy(vap->va_name, (const char*) linkdesc.cd_nameptr, MAXPATHLEN);
			VATTR_SET_SUPPORTED(vap, va_name);
			cat_releasedesc(&linkdesc);
		}	
		else if (cp->c_desc.cd_namelen) {
			strlcpy(vap->va_name, (const char*) cp->c_desc.cd_nameptr, MAXPATHLEN);
=======
		
		/* By this point, we either patched the name above, and the c_desc points 
		 * to correct data, or it already did, in which case we just proceed by copying
		 * the name into the VAP.  Note that we will never set va_name to supported if
		 * nextlinkid is never initialized.  This could happen in the degenerate case above
		 * involving the raw inode number, where it has no nextlinkid.  In this case, we will
		 * simply not export the name as supported.
		 */
		if (uselinkdesc) {
			strlcpy(vap->va_name, (const char *)linkdesc.cd_nameptr, MAXPATHLEN);
			VATTR_SET_SUPPORTED(vap, va_name);
			cat_releasedesc(&linkdesc);	
		}
		else if (cp->c_desc.cd_namelen) {
			strlcpy(vap->va_name, (const char *)cp->c_desc.cd_nameptr, MAXPATHLEN);
>>>>>>> origin/10.5
			VATTR_SET_SUPPORTED(vap, va_name);
		}
	}

out:
	hfs_unlock(cp);
<<<<<<< HEAD
	/*
	 * We need to vnode_put the rsrc fork vnode only *after* we've released
	 * the cnode lock, since vnode_put can trigger an inactive call, which 
	 * will go back into HFS and try to acquire a cnode lock.
=======
	/* 
	 * We need to drop the iocount on the rsrc fork vnode only *after* we've 
	 * released the cnode lock, since vnode_put can trigger an inactive call, which
	 * will go back into the HFS and try to acquire a cnode lock.  	 
>>>>>>> origin/10.5
	 */
	if (rvp) {
		vnode_put (rvp);
	}

	return (error);
}

int
hfs_vnop_setattr(ap)
	struct vnop_setattr_args /* {
		struct vnode *a_vp;
		struct vnode_attr *a_vap;
		vfs_context_t a_context;
	} */ *ap;
{
	struct vnode_attr *vap = ap->a_vap;
	struct vnode *vp = ap->a_vp;
	struct cnode *cp = NULL;
	struct hfsmount *hfsmp;
	kauth_cred_t cred = vfs_context_ucred(ap->a_context);
	struct proc *p = vfs_context_proc(ap->a_context);
	int error = 0;
	uid_t nuid;
	gid_t ngid;
	time_t orig_ctime;

	orig_ctime = VTOC(vp)->c_ctime;
	
#if HFS_COMPRESSION
	int decmpfs_reset_state = 0;
	/*
	 we call decmpfs_update_attributes even if the file is not compressed
	 because we want to update the incoming flags if the xattrs are invalid
	 */
	error = decmpfs_update_attributes(vp, vap);
	if (error)
		return error;
#endif
	//
	// if this is not a size-changing setattr and it is not just
	// an atime update, then check for a snapshot.
	//
	if (!VATTR_IS_ACTIVE(vap, va_data_size) && !(vap->va_active == VNODE_ATTR_va_access_time)) {
		check_for_tracked_file(vp, orig_ctime, NAMESPACE_HANDLER_METADATA_MOD, NSPACE_REARM_NO_ARG);
	}

#if CONFIG_PROTECT
	/*
	 * All metadata changes should be allowed except a size-changing setattr, which
	 * has effects on file content and requires calling into cp_handle_vnop
	 * to have content protection check.
	 */
	if (VATTR_IS_ACTIVE(vap, va_data_size)) {
		if ((error = cp_handle_vnop(vp, CP_WRITE_ACCESS, 0)) != 0) {
			return (error);
		}
	}
#endif /* CONFIG_PROTECT */

	hfsmp = VTOHFS(vp);

	/* Don't allow modification of the journal. */
	if (hfs_is_journal_file(hfsmp, VTOC(vp))) {
		return (EPERM);
	}

	//
	// Check if we'll need a document_id and if so, get it before we lock the
	// the cnode to avoid any possible deadlock with the root vnode which has
	// to get locked to get the document id
	//
	u_int32_t document_id=0;
	if (VATTR_IS_ACTIVE(vap, va_flags) && (vap->va_flags & UF_TRACKED) && !(VTOC(vp)->c_bsdflags & UF_TRACKED)) {
		struct FndrExtendedDirInfo *fip = (struct FndrExtendedDirInfo *)((char *)&(VTOC(vp)->c_attr.ca_finderinfo) + 16);
		//
		// If the document_id is not set, get a new one.  It will be set
		// on the file down below once we hold the cnode lock.
		//
		if (fip->document_id == 0) {
			if (hfs_generate_document_id(hfsmp, &document_id) != 0) {
				document_id = 0;
			}
		}
	}


	/*
	 * File size change request.
	 * We are guaranteed that this is not a directory, and that
	 * the filesystem object is writeable.
	 *
	 * NOTE: HFS COMPRESSION depends on the data_size being set *before* the bsd flags are updated
	 */
	VATTR_SET_SUPPORTED(vap, va_data_size);
	if (VATTR_IS_ACTIVE(vap, va_data_size) && !vnode_islnk(vp)) {
#if HFS_COMPRESSION
		/* keep the compressed state locked until we're done truncating the file */
		decmpfs_cnode *dp = VTOCMP(vp);
		if (!dp) {
			/*
			 * call hfs_lazy_init_decmpfs_cnode() to make sure that the decmpfs_cnode
			 * is filled in; we need a decmpfs_cnode to lock out decmpfs state changes
			 * on this file while it's truncating
			 */
			dp = hfs_lazy_init_decmpfs_cnode(VTOC(vp));
			if (!dp) {
				/* failed to allocate a decmpfs_cnode */
				return ENOMEM; /* what should this be? */
			}
		}
		
		check_for_tracked_file(vp, orig_ctime, vap->va_data_size == 0 ? NAMESPACE_HANDLER_TRUNCATE_OP|NAMESPACE_HANDLER_DELETE_OP : NAMESPACE_HANDLER_TRUNCATE_OP, NULL);

		decmpfs_lock_compressed_data(dp, 1);
		if (hfs_file_is_compressed(VTOC(vp), 1)) {
			error = decmpfs_decompress_file(vp, dp, -1/*vap->va_data_size*/, 0, 1);
			if (error != 0) {
				decmpfs_unlock_compressed_data(dp, 1);
				return error;
			}
		}
#endif

		// Take truncate lock
		hfs_lock_truncate(VTOC(vp), HFS_EXCLUSIVE_LOCK, HFS_LOCK_DEFAULT);

		// hfs_truncate will deal with the cnode lock
		error = hfs_truncate(vp, vap->va_data_size, vap->va_vaflags & 0xffff, 
							 0, ap->a_context);

		hfs_unlock_truncate(VTOC(vp), HFS_LOCK_DEFAULT);
#if HFS_COMPRESSION
		decmpfs_unlock_compressed_data(dp, 1);
#endif
		if (error)
			return error;
	}
	if (cp == NULL) {
		if ((error = hfs_lock(VTOC(vp), HFS_EXCLUSIVE_LOCK, HFS_LOCK_DEFAULT)))
			return (error);
		cp = VTOC(vp);
	}

	/*
	 * If it is just an access time update request by itself
	 * we know the request is from kernel level code, and we
	 * can delay it without being as worried about consistency.
	 * This change speeds up mmaps, in the rare case that they
	 * get caught behind a sync.
	 */

	if (vap->va_active == VNODE_ATTR_va_access_time) {
		cp->c_touch_acctime=TRUE;
		goto out;
	}



	/*
	 * Owner/group change request.
	 * We are guaranteed that the new owner/group is valid and legal.
	 */
	VATTR_SET_SUPPORTED(vap, va_uid);
	VATTR_SET_SUPPORTED(vap, va_gid);
	nuid = VATTR_IS_ACTIVE(vap, va_uid) ? vap->va_uid : (uid_t)VNOVAL;
	ngid = VATTR_IS_ACTIVE(vap, va_gid) ? vap->va_gid : (gid_t)VNOVAL;
	if (((nuid != (uid_t)VNOVAL) || (ngid != (gid_t)VNOVAL)) &&
	    ((error = hfs_chown(vp, nuid, ngid, cred, p)) != 0))
		goto out;

	/*
	 * Mode change request.
	 * We are guaranteed that the mode value is valid and that in
	 * conjunction with the owner and group, this change is legal.
   	*/
	VATTR_SET_SUPPORTED(vap, va_mode);
	if (VATTR_IS_ACTIVE(vap, va_mode) &&
	    ((error = hfs_chmod(vp, (int)vap->va_mode, cred, p)) != 0))
	    goto out;

	/*
	 * File flags change.
	 * We are guaranteed that only flags allowed to change given the
	 * current securelevel are being changed.
	 */
	VATTR_SET_SUPPORTED(vap, va_flags);
	if (VATTR_IS_ACTIVE(vap, va_flags)) {
		u_int16_t *fdFlags;

#if HFS_COMPRESSION
		if ((cp->c_bsdflags ^ vap->va_flags) & UF_COMPRESSED) {
			/*
			 * the UF_COMPRESSED was toggled, so reset our cached compressed state
			 * but we don't want to actually do the update until we've released the cnode lock down below
			 * NOTE: turning the flag off doesn't actually decompress the file, so that we can
			 * turn off the flag and look at the "raw" file for debugging purposes
			 */
			decmpfs_reset_state = 1;
		}
#endif
		if ((vap->va_flags & UF_TRACKED) && !(cp->c_bsdflags & UF_TRACKED)) {
			struct FndrExtendedDirInfo *fip = (struct FndrExtendedDirInfo *)((char *)&cp->c_attr.ca_finderinfo + 16);

			//
			// we're marking this item UF_TRACKED.  if the document_id is
			// not set, get a new one and put it on the file.
			//
			if (fip->document_id == 0) {
				if (document_id != 0) {
					// printf("SETATTR: assigning doc-id %d to %s (ino %d)\n", document_id, vp->v_name, cp->c_desc.cd_cnid);
					fip->document_id = (uint32_t)document_id;
#if CONFIG_FSE
					add_fsevent(FSE_DOCID_CHANGED, ap->a_context,
						    FSE_ARG_DEV, hfsmp->hfs_raw_dev,
						    FSE_ARG_INO, (ino64_t)0,                // src inode #
						    FSE_ARG_INO, (ino64_t)cp->c_fileid,     // dst inode #
						    FSE_ARG_INT32, document_id,
						    FSE_ARG_DONE);
#endif
				} else {
					// printf("hfs: could not acquire a new document_id for %s (ino %d)\n", vp->v_name, cp->c_desc.cd_cnid);
				}
			}

		} else if (!(vap->va_flags & UF_TRACKED) && (cp->c_bsdflags & UF_TRACKED)) {
			//
			// UF_TRACKED is being cleared so clear the document_id
			//
			struct FndrExtendedDirInfo *fip = (struct FndrExtendedDirInfo *)((char *)&cp->c_attr.ca_finderinfo + 16);
			if (fip->document_id) {
				// printf("SETATTR: clearing doc-id %d from %s (ino %d)\n", fip->document_id, vp->v_name, cp->c_desc.cd_cnid);
#if CONFIG_FSE
				add_fsevent(FSE_DOCID_CHANGED, ap->a_context,
					    FSE_ARG_DEV, hfsmp->hfs_raw_dev,
					    FSE_ARG_INO, (ino64_t)cp->c_fileid,          // src inode #
					    FSE_ARG_INO, (ino64_t)0,                     // dst inode #
					    FSE_ARG_INT32, fip->document_id,             // document id
					    FSE_ARG_DONE);
#endif
				fip->document_id = 0;
				cp->c_bsdflags &= ~UF_TRACKED;
			}
		}
<<<<<<< HEAD

		cp->c_bsdflags = vap->va_flags;
		cp->c_flag |= C_MODIFIED;
		cp->c_touch_chgtime = TRUE;

=======

		cp->c_bsdflags = vap->va_flags;
		cp->c_touch_chgtime = TRUE;

>>>>>>> origin/10.9
		
		/*
		 * Mirror the UF_HIDDEN flag to the invisible bit of the Finder Info.
		 *
		 * The fdFlags for files and frFlags for folders are both 8 bytes
		 * into the userInfo (the first 16 bytes of the Finder Info).  They
		 * are both 16-bit fields.
		 */
		fdFlags = (u_int16_t *) &cp->c_finderinfo[8];
		if (vap->va_flags & UF_HIDDEN)
			*fdFlags |= OSSwapHostToBigConstInt16(kFinderInvisibleMask);
		else
			*fdFlags &= ~OSSwapHostToBigConstInt16(kFinderInvisibleMask);
	}

	/*
	 * Timestamp updates.
	 */
	VATTR_SET_SUPPORTED(vap, va_create_time);
	VATTR_SET_SUPPORTED(vap, va_access_time);
	VATTR_SET_SUPPORTED(vap, va_modify_time);
	VATTR_SET_SUPPORTED(vap, va_backup_time);
	VATTR_SET_SUPPORTED(vap, va_change_time);
	if (VATTR_IS_ACTIVE(vap, va_create_time) ||
	    VATTR_IS_ACTIVE(vap, va_access_time) ||
	    VATTR_IS_ACTIVE(vap, va_modify_time) ||
	    VATTR_IS_ACTIVE(vap, va_backup_time)) {
		if (VATTR_IS_ACTIVE(vap, va_create_time))
			cp->c_itime = vap->va_create_time.tv_sec;
		if (VATTR_IS_ACTIVE(vap, va_access_time)) {
			cp->c_atime = vap->va_access_time.tv_sec;
			cp->c_touch_acctime = FALSE;
		}
		if (VATTR_IS_ACTIVE(vap, va_modify_time)) {
			cp->c_mtime = vap->va_modify_time.tv_sec;
			cp->c_touch_modtime = FALSE;
			cp->c_touch_chgtime = TRUE;

			hfs_clear_might_be_dirty_flag(cp);

			/*
			 * The utimes system call can reset the modification
			 * time but it doesn't know about HFS create times.
			 * So we need to ensure that the creation time is
			 * always at least as old as the modification time.
			 */
			if ((VTOVCB(vp)->vcbSigWord == kHFSPlusSigWord) &&
			    (cp->c_cnid != kHFSRootFolderID) &&
				!VATTR_IS_ACTIVE(vap, va_create_time) &&
			    (cp->c_mtime < cp->c_itime)) {
				cp->c_itime = cp->c_mtime;
			}
		}
		if (VATTR_IS_ACTIVE(vap, va_backup_time))
			cp->c_btime = vap->va_backup_time.tv_sec;
		cp->c_flag |= C_MINOR_MOD;
	}
	
	/*
	 * Set name encoding.
	 */
	VATTR_SET_SUPPORTED(vap, va_encoding);
	if (VATTR_IS_ACTIVE(vap, va_encoding)) {
		cp->c_encoding = vap->va_encoding;
		cp->c_flag |= C_MODIFIED;
		hfs_setencodingbits(hfsmp, cp->c_encoding);
	}

	if ((error = hfs_update(vp, 0)) != 0)
		goto out;
out:
	if (cp) {
		/* Purge origin cache for cnode, since caller now has correct link ID for it 
		 * We purge it here since it was acquired for us during lookup, and we no longer need it.
		 */
		if ((cp->c_flag & C_HARDLINK) && (vp->v_type != VDIR)){
			hfs_relorigin(cp, 0);
		}

		hfs_unlock(cp);
#if HFS_COMPRESSION
		if (decmpfs_reset_state) {
			/*
			 * we've changed the UF_COMPRESSED flag, so reset the decmpfs state for this cnode
			 * but don't do it while holding the hfs cnode lock
			 */
			decmpfs_cnode *dp = VTOCMP(vp);
			if (!dp) {
				/*
				 * call hfs_lazy_init_decmpfs_cnode() to make sure that the decmpfs_cnode
				 * is filled in; we need a decmpfs_cnode to prevent decmpfs state changes
				 * on this file if it's locked
				 */
				dp = hfs_lazy_init_decmpfs_cnode(VTOC(vp));
				if (!dp) {
					/* failed to allocate a decmpfs_cnode */
					return ENOMEM; /* what should this be? */
				}
			}
			decmpfs_cnode_set_vnode_state(dp, FILE_TYPE_UNKNOWN, 0);
		}
#endif
	}
	return (error);
}


/*
 * Change the mode on a file.
 * cnode must be locked before calling.
 */
int
hfs_chmod(struct vnode *vp, int mode, __unused kauth_cred_t cred, __unused struct proc *p)
{
	register struct cnode *cp = VTOC(vp);

	if (VTOVCB(vp)->vcbSigWord != kHFSPlusSigWord)
		return (0);

	// Don't allow modification of the journal or journal_info_block
	if (hfs_is_journal_file(VTOHFS(vp), cp)) {
		return EPERM;
	}

	// XXXdbg - don't allow modification of the journal or journal_info_block
	if (VTOHFS(vp)->jnl && cp && cp->c_datafork) {
		struct HFSPlusExtentDescriptor *extd;

		extd = &cp->c_datafork->ff_data.cf_extents[0];
		if (extd->startBlock == VTOVCB(vp)->vcbJinfoBlock || extd->startBlock == VTOHFS(vp)->jnl_start) {
			return EPERM;
		}
	}

#if OVERRIDE_UNKNOWN_PERMISSIONS
	if (((unsigned int)vfs_flags(VTOVFS(vp))) & MNT_UNKNOWNPERMISSIONS) {
		return (0);
	};
#endif

	mode_t new_mode = (cp->c_mode & ~ALLPERMS) | (mode & ALLPERMS);
	if (new_mode != cp->c_mode) {
		cp->c_mode = new_mode;
		cp->c_flag |= C_MINOR_MOD;
	}
	cp->c_touch_chgtime = TRUE;
	return (0);
}


int
hfs_write_access(struct vnode *vp, kauth_cred_t cred, struct proc *p, Boolean considerFlags)
{
	struct cnode *cp = VTOC(vp);
	int retval = 0;
	int is_member;

	/*
	 * Disallow write attempts on read-only file systems;
	 * unless the file is a socket, fifo, or a block or
	 * character device resident on the file system.
	 */
	switch (vnode_vtype(vp)) {
	case VDIR:
 	case VLNK:
	case VREG:
		if (VTOHFS(vp)->hfs_flags & HFS_READ_ONLY)
			return (EROFS);
		break;
	default:
		break;
 	}
 
	/* If immutable bit set, nobody gets to write it. */
	if (considerFlags && (cp->c_bsdflags & IMMUTABLE))
		return (EPERM);

	/* Otherwise, user id 0 always gets access. */
	if (!suser(cred, NULL))
		return (0);

	/* Otherwise, check the owner. */
	if ((retval = hfs_owner_rights(VTOHFS(vp), cp->c_uid, cred, p, false)) == 0)
		return ((cp->c_mode & S_IWUSR) == S_IWUSR ? 0 : EACCES);
 
	/* Otherwise, check the groups. */
	if (kauth_cred_ismember_gid(cred, cp->c_gid, &is_member) == 0 && is_member) {
		return ((cp->c_mode & S_IWGRP) == S_IWGRP ? 0 : EACCES);
 	}
 
	/* Otherwise, check everyone else. */
	return ((cp->c_mode & S_IWOTH) == S_IWOTH ? 0 : EACCES);
}


/*
 * Perform chown operation on cnode cp;
 * code must be locked prior to call.
 */
int
#if !QUOTA
hfs_chown(struct vnode *vp, uid_t uid, gid_t gid, __unused kauth_cred_t cred,
	__unused struct proc *p)
#else 
hfs_chown(struct vnode *vp, uid_t uid, gid_t gid, kauth_cred_t cred,
	__unused struct proc *p)
#endif
{
	register struct cnode *cp = VTOC(vp);
	uid_t ouid;
	gid_t ogid;
#if QUOTA
	int error = 0;
	register int i;
	int64_t change;
#endif /* QUOTA */

	if (VTOVCB(vp)->vcbSigWord != kHFSPlusSigWord)
		return (ENOTSUP);

	if (((unsigned int)vfs_flags(VTOVFS(vp))) & MNT_UNKNOWNPERMISSIONS)
		return (0);
	
	if (uid == (uid_t)VNOVAL)
		uid = cp->c_uid;
	if (gid == (gid_t)VNOVAL)
		gid = cp->c_gid;

#if 0	/* we are guaranteed that this is already the case */
	/*
	 * If we don't own the file, are trying to change the owner
	 * of the file, or are not a member of the target group,
	 * the caller must be superuser or the call fails.
	 */
	if ((kauth_cred_getuid(cred) != cp->c_uid || uid != cp->c_uid ||
	    (gid != cp->c_gid &&
	     (kauth_cred_ismember_gid(cred, gid, &is_member) || !is_member))) &&
	    (error = suser(cred, 0)))
		return (error);
#endif

	ogid = cp->c_gid;
	ouid = cp->c_uid;

	if (ouid == uid && ogid == gid) {
		// No change, just set change time
		cp->c_touch_chgtime = TRUE;
		return 0;
	}

#if QUOTA
	if ((error = hfs_getinoquota(cp)))
		return (error);
	if (ouid == uid) {
		dqrele(cp->c_dquot[USRQUOTA]);
		cp->c_dquot[USRQUOTA] = NODQUOT;
	}
	if (ogid == gid) {
		dqrele(cp->c_dquot[GRPQUOTA]);
		cp->c_dquot[GRPQUOTA] = NODQUOT;
	}

	/*
	 * Eventually need to account for (fake) a block per directory
	 * if (vnode_isdir(vp))
	 *     change = VTOHFS(vp)->blockSize;
	 * else
	 */

	change = (int64_t)(cp->c_blocks) * (int64_t)VTOVCB(vp)->blockSize;
	(void) hfs_chkdq(cp, -change, cred, CHOWN);
	(void) hfs_chkiq(cp, -1, cred, CHOWN);
	for (i = 0; i < MAXQUOTAS; i++) {
		dqrele(cp->c_dquot[i]);
		cp->c_dquot[i] = NODQUOT;
	}
#endif /* QUOTA */
	cp->c_gid = gid;
	cp->c_uid = uid;
#if QUOTA
	if ((error = hfs_getinoquota(cp)) == 0) {
		if (ouid == uid) {
			dqrele(cp->c_dquot[USRQUOTA]);
			cp->c_dquot[USRQUOTA] = NODQUOT;
		}
		if (ogid == gid) {
			dqrele(cp->c_dquot[GRPQUOTA]);
			cp->c_dquot[GRPQUOTA] = NODQUOT;
		}
		if ((error = hfs_chkdq(cp, change, cred, CHOWN)) == 0) {
			if ((error = hfs_chkiq(cp, 1, cred, CHOWN)) == 0)
				goto good;
			else
				(void) hfs_chkdq(cp, -change, cred, CHOWN|FORCE);
		}
		for (i = 0; i < MAXQUOTAS; i++) {
			dqrele(cp->c_dquot[i]);
			cp->c_dquot[i] = NODQUOT;
		}
	}
	cp->c_gid = ogid;
	cp->c_uid = ouid;
	if (hfs_getinoquota(cp) == 0) {
		if (ouid == uid) {
			dqrele(cp->c_dquot[USRQUOTA]);
			cp->c_dquot[USRQUOTA] = NODQUOT;
		}
		if (ogid == gid) {
			dqrele(cp->c_dquot[GRPQUOTA]);
			cp->c_dquot[GRPQUOTA] = NODQUOT;
		}
		(void) hfs_chkdq(cp, change, cred, FORCE|CHOWN);
		(void) hfs_chkiq(cp, 1, cred, FORCE|CHOWN);
		(void) hfs_getinoquota(cp);
	}
	return (error);
good:
	if (hfs_getinoquota(cp))
		panic("hfs_chown: lost quota");
#endif /* QUOTA */

	/*
	 * Without quotas, we could probably make this a minor
	 * modification.
	 */
	cp->c_flag |= C_MODIFIED;

	/*
	  According to the SUSv3 Standard, chown() shall mark
	  for update the st_ctime field of the file.
	  (No exceptions mentioned)
	*/
		cp->c_touch_chgtime = TRUE;
	return (0);
}

#if HFS_COMPRESSION
/*
 * Flush the resource fork if it exists.  vp is the data fork and has
 * an iocount.
 */
static int hfs_flush_rsrc(vnode_t vp, vfs_context_t ctx)
{
	cnode_t *cp = VTOC(vp);

	hfs_lock(cp, HFS_SHARED_LOCK, 0);

	vnode_t rvp = cp->c_rsrc_vp;

	if (!rvp) {
		hfs_unlock(cp);
		return 0;
	}

	int vid = vnode_vid(rvp);

	hfs_unlock(cp);

	int error = vnode_getwithvid(rvp, vid);

	if (error)
		return error == ENOENT ? 0 : error;

	hfs_lock_truncate(cp, HFS_EXCLUSIVE_LOCK, 0);
	hfs_lock_always(cp, HFS_EXCLUSIVE_LOCK);
	hfs_filedone(rvp, ctx, HFS_FILE_DONE_NO_SYNC);
	hfs_unlock(cp);
	hfs_unlock_truncate(cp, 0);

	error = ubc_msync(rvp, 0, ubc_getsize(rvp), NULL,
					  UBC_PUSHALL | UBC_SYNC);

	vnode_put(rvp);

	return error;
}
#endif // HFS_COMPRESSION

/*
 * hfs_vnop_exchange:
 * 
 * Inputs: 
 * 		'from' vnode/cnode
 * 		'to' vnode/cnode
 * 		options flag bits
 * 		vfs_context
 * 
 * Discussion:
 * hfs_vnop_exchange is used to service the exchangedata(2) system call.
 * Per the requirements of that system call, this function "swaps" some
 * of the information that lives in one catalog record for some that
 * lives in another.  Note that not everything is swapped; in particular, 
 * the extent information stored in each cnode is kept local to that 
 * cnode.  This allows existing file descriptor references to continue
 * to operate on the same content, regardless of the location in the
 * namespace that the file may have moved to.  See inline comments
 * in the function for more information.
 */
int
hfs_vnop_exchange(ap)
	struct vnop_exchange_args /* {
		struct vnode *a_fvp;
		struct vnode *a_tvp;
		int a_options;
		vfs_context_t a_context;
	} */ *ap;
{
	struct vnode *from_vp = ap->a_fvp;
	struct vnode *to_vp = ap->a_tvp;
	struct cnode *from_cp;
	struct cnode *to_cp;
	struct hfsmount *hfsmp;
	struct cat_desc tempdesc;
	struct cat_attr tempattr;
	const unsigned char *from_nameptr;
	const unsigned char *to_nameptr;
	char from_iname[32];
	char to_iname[32];
	uint32_t to_flag_special;
	uint32_t from_flag_special;
	cnid_t  from_parid;
	cnid_t  to_parid;
	int lockflags;
	int error = 0, started_tr = 0, got_cookie = 0;
	cat_cookie_t cookie;
	time_t orig_from_ctime, orig_to_ctime;
	bool have_cnode_locks = false, have_from_trunc_lock = false, have_to_trunc_lock = false;

	/*
	 * VFS does the following checks:
	 * 1. Validate that both are files.
	 * 2. Validate that both are on the same mount.
	 * 3. Validate that they're not the same vnode.
	 */

	from_cp = VTOC(from_vp);
	to_cp = VTOC(to_vp);
	hfsmp = VTOHFS(from_vp);

	orig_from_ctime = from_cp->c_ctime;
	orig_to_ctime = to_cp->c_ctime;

#if CONFIG_PROTECT
	/* 
	 * Do not allow exchangedata/F_MOVEDATAEXTENTS on data-protected filesystems 
	 * because the EAs will not be swapped.  As a result, the persistent keys would not
	 * match and the files will be garbage.
	 */
	if (cp_fs_protected (vnode_mount(from_vp))) {
		return EINVAL;
	}
#endif

#if HFS_COMPRESSION
	if (!ISSET(ap->a_options, FSOPT_EXCHANGE_DATA_ONLY)) {
		if ( hfs_file_is_compressed(from_cp, 0) ) {
			if ( 0 != ( error = decmpfs_decompress_file(from_vp, VTOCMP(from_vp), -1, 0, 1) ) ) {
				return error;
			}
		}

		if ( hfs_file_is_compressed(to_cp, 0) ) {
			if ( 0 != ( error = decmpfs_decompress_file(to_vp, VTOCMP(to_vp), -1, 0, 1) ) ) {
				return error;
			}
		}
	}
#endif // HFS_COMPRESSION

	// Resource forks cannot be exchanged.
	if (VNODE_IS_RSRC(from_vp) || VNODE_IS_RSRC(to_vp))
		return EINVAL;

	/* 
	 * Normally, we want to notify the user handlers about the event,
	 * except if it's a handler driving the event.
	 */
	if ((ap->a_options & FSOPT_EXCHANGE_DATA_ONLY) == 0) {
		check_for_tracked_file(from_vp, orig_from_ctime, NAMESPACE_HANDLER_WRITE_OP, NULL);
		check_for_tracked_file(to_vp, orig_to_ctime, NAMESPACE_HANDLER_WRITE_OP, NULL);
	} else {
		/*
		 * This is currently used by mtmd so we should tidy up the
		 * file now because the data won't be used again in the
		 * destination file.
		 */
		hfs_lock_truncate(from_cp, HFS_EXCLUSIVE_LOCK, 0);
		hfs_lock_always(from_cp, HFS_EXCLUSIVE_LOCK);
		hfs_filedone(from_vp, ap->a_context, HFS_FILE_DONE_NO_SYNC);
		hfs_unlock(from_cp);
		hfs_unlock_truncate(from_cp, 0);

		// Flush all the data from the source file
 		error = ubc_msync(from_vp, 0, ubc_getsize(from_vp), NULL,
 						  UBC_PUSHALL | UBC_SYNC);
		if (error)
			goto exit;

#if HFS_COMPRESSION
		/*
		 * If this is a compressed file, we need to do the same for
		 * the resource fork.
		 */
		if (ISSET(from_cp->c_bsdflags, UF_COMPRESSED)) {
			error = hfs_flush_rsrc(from_vp, ap->a_context);
			if (error)
				goto exit;
		}
#endif

		/*
		 * We're doing a data-swap so we need to take the truncate
		 * lock exclusively.  We need an exclusive lock because we
		 * will be completely truncating the source file and we must
		 * make sure nobody else sneaks in and trys to issue I/O
		 * whilst we don't have the cnode lock.
		 *
		 * After taking the truncate lock we do a quick check to
		 * verify there are no other references (including mmap
		 * references), but we must remember that this does not stop
		 * anybody coming in later and taking a reference.  We will
		 * have the truncate lock exclusively so that will prevent
		 * them from issuing any I/O.
		 */

		if (to_cp < from_cp) {
			hfs_lock_truncate(to_cp, HFS_EXCLUSIVE_LOCK, HFS_LOCK_DEFAULT);
			have_to_trunc_lock = true;
		}

		hfs_lock_truncate(from_cp, HFS_EXCLUSIVE_LOCK, HFS_LOCK_DEFAULT);
		have_from_trunc_lock = true;

 		/*
		 * Do an early check to verify the source is not in use by
		 * anyone.  We should be called from an FD opened as F_EVTONLY
		 * so that doesn't count as a reference.
		 */
		if (vnode_isinuse(from_vp, 0)) {
			error = EBUSY;
			goto exit;
		}

<<<<<<< HEAD
		if (to_cp >= from_cp) {
			hfs_lock_truncate(to_cp, HFS_EXCLUSIVE_LOCK, HFS_LOCK_DEFAULT);
			have_to_trunc_lock = true;
		}
=======
	/* Resource forks cannot be exchanged. */
	if (VNODE_IS_RSRC(from_vp) || VNODE_IS_RSRC(to_vp)) {
		error = EINVAL;
		goto exit;
>>>>>>> origin/10.8
	}

	if ((error = hfs_lockpair(from_cp, to_cp, HFS_EXCLUSIVE_LOCK)))
		goto exit;
	have_cnode_locks = true;

	// Don't allow modification of the journal or journal_info_block
	if (hfs_is_journal_file(hfsmp, from_cp) ||
	    hfs_is_journal_file(hfsmp, to_cp)) {
		error = EPERM;
		goto exit;
	}

	/* 
	 * Ok, now that all of the pre-flighting is done, call the underlying
	 * function if needed.
	 */
	if (ISSET(ap->a_options, FSOPT_EXCHANGE_DATA_ONLY)) {
#if HFS_COMPRESSION
		if (ISSET(from_cp->c_bsdflags, UF_COMPRESSED)) {
			error = hfs_move_compressed(from_cp, to_cp);
			goto exit;
		}
#endif

		error = hfs_move_data(from_cp, to_cp, 0);
		goto exit;
	}

	if ((error = hfs_start_transaction(hfsmp)) != 0) {
	    goto exit;
	}
	started_tr = 1;
	
	/*
	 * Reserve some space in the Catalog file.
	 */
	if ((error = cat_preflight(hfsmp, CAT_EXCHANGE, &cookie, vfs_context_proc(ap->a_context)))) {
		goto exit;
	}
	got_cookie = 1;

	/* The backend code always tries to delete the virtual
	 * extent id for exchanging files so we need to lock
	 * the extents b-tree.
	 */
	lockflags = hfs_systemfile_lock(hfsmp, SFL_CATALOG | SFL_EXTENTS | SFL_ATTRIBUTE, HFS_EXCLUSIVE_LOCK);

	/* Account for the location of the catalog objects. */
	if (from_cp->c_flag & C_HARDLINK) {
		MAKE_INODE_NAME(from_iname, sizeof(from_iname),
				from_cp->c_attr.ca_linkref);
		from_nameptr = (unsigned char *)from_iname;
		from_parid = hfsmp->hfs_private_desc[FILE_HARDLINKS].cd_cnid;
		from_cp->c_hint = 0;
	} else {
		from_nameptr = from_cp->c_desc.cd_nameptr;
		from_parid = from_cp->c_parentcnid;
	}
	if (to_cp->c_flag & C_HARDLINK) {
		MAKE_INODE_NAME(to_iname, sizeof(to_iname),
				to_cp->c_attr.ca_linkref);
		to_nameptr = (unsigned char *)to_iname;
		to_parid = hfsmp->hfs_private_desc[FILE_HARDLINKS].cd_cnid;
		to_cp->c_hint = 0;
	} else {
		to_nameptr = to_cp->c_desc.cd_nameptr;
		to_parid = to_cp->c_parentcnid;
	}

	/*
	 * ExchangeFileIDs swaps the on-disk, or in-BTree extent information 
	 * attached to two different file IDs.  It also swaps the extent 
	 * information that may live in the extents-overflow B-Tree.
	 *
	 * We do this in a transaction as this may require a lot of B-Tree nodes
	 * to do completely, particularly if one of the files in question
	 * has a lot of extents. 
	 *
	 * For example, assume "file1" has fileID 50, and "file2" has fileID 52.
	 * For the on-disk records, which are assumed to be synced, we will
	 * first swap the resident inline-8 extents as part of the catalog records.
	 * Then we will swap any extents overflow records for each file.
	 * 
	 * When ExchangeFileIDs returns successfully, "file1" will have fileID 52, 
	 * and "file2" will have fileID 50.  However, note that this is only 
	 * approximately half of the work that exchangedata(2) will need to 
	 * accomplish.  In other words, we swap "too much" of the information 
	 * because if we only called ExchangeFileIDs, both the fileID and extent 
	 * information would be the invariants of this operation.  We don't 
	 * actually want that; we want to conclude with "file1" having 
	 * file ID 50, and "file2" having fileID 52.
	 * 
	 * The remainder of hfs_vnop_exchange will swap the file ID and other cnode
	 * data back to the proper ownership, while still allowing the cnode to remain
	 * pointing at the same set of extents that it did originally.
	 */
	error = ExchangeFileIDs(hfsmp, from_nameptr, to_nameptr, from_parid,
	                        to_parid, from_cp->c_hint, to_cp->c_hint);
	hfs_systemfile_unlock(hfsmp, lockflags);

	/*
	 * Note that we don't need to exchange any extended attributes
	 * since the attributes are keyed by file ID.
	 */

	if (error != E_NONE) {
		error = MacToVFSError(error);
		goto exit;
	}

	/* Purge the vnodes from the name cache */
 	if (from_vp)
		cache_purge(from_vp);
	if (to_vp)
		cache_purge(to_vp);

	/* Bump both source and destination write counts before any swaps. */
	{
		hfs_incr_gencount (from_cp);
		hfs_incr_gencount (to_cp);
	}

	/* Save a copy of "from" attributes before swapping. */
	bcopy(&from_cp->c_desc, &tempdesc, sizeof(struct cat_desc));
	bcopy(&from_cp->c_attr, &tempattr, sizeof(struct cat_attr));
	
	/* Save whether or not each cnode is a hardlink or has EAs */
	from_flag_special = from_cp->c_flag & (C_HARDLINK | C_HASXATTRS);
	to_flag_special = to_cp->c_flag & (C_HARDLINK | C_HASXATTRS);

	/* Drop the special bits from each cnode */
	from_cp->c_flag &= ~(C_HARDLINK | C_HASXATTRS);
	to_cp->c_flag &= ~(C_HARDLINK | C_HASXATTRS);

	/*
	 * Now complete the in-memory portion of the copy.
	 *
	 * ExchangeFileIDs swaps the on-disk records involved.  We complete the
	 * operation by swapping the in-memory contents of the two files here.
	 * We swap the cnode descriptors, which contain name, BSD attributes,
	 * timestamps, etc, about the file.  
	 * 
	 * NOTE: We do *NOT* swap the fileforks of the two cnodes.  We have
	 * already swapped the on-disk extent information.  As long as we swap the 
	 * IDs, the in-line resident 8 extents that live in the filefork data
	 * structure will point to the right data for the new file ID if we leave 
	 * them alone.
	 *
	 * As a result, any file descriptor that points to a particular
	 * vnode (even though it should change names), will continue
	 * to point to the same content. 
	 */

	/* Copy the "to" -> "from" cnode */
	bcopy(&to_cp->c_desc, &from_cp->c_desc, sizeof(struct cat_desc));

	from_cp->c_hint = 0;
	/*
	 * If 'to' was a hardlink, then we copied over its link ID/CNID/(namespace ID) 
<<<<<<< HEAD
	 * when we bcopy'd the descriptor above.  However, the cnode attributes
	 * are not bcopied.  As a result, make sure to swap the file IDs of each item.
	 *
	 * Further, other hardlink attributes must be moved along in this swap:
	 * the linkcount, the linkref, and the firstlink all need to move
	 * along with the file IDs.  See note below regarding the flags and
	 * what moves vs. what does not.
	 *
	 * For Reference:
	 * linkcount == total # of hardlinks.
	 * linkref == the indirect inode pointer.
	 * firstlink == the first hardlink in the chain (written to the raw inode).
	 * These three are tied to the fileID and must move along with the rest of the data.
	 */ 
	from_cp->c_fileid = to_cp->c_attr.ca_fileid;

=======
	 * when we bcopy'd the descriptor above.  However, we need to be careful
	 * when setting up the fileID below, because we cannot assume that the
	 * file ID is the same as the CNID if either one was a hardlink.  
	 * The file ID is stored in the c_attr as the ca_fileid. So it needs 
	 * to be pulled explicitly; we cannot just use the CNID.
	 */ 
	from_cp->c_fileid = to_cp->c_attr.ca_fileid;
	
>>>>>>> origin/10.8
	from_cp->c_itime = to_cp->c_itime;
	from_cp->c_btime = to_cp->c_btime;
	from_cp->c_atime = to_cp->c_atime;
	from_cp->c_ctime = to_cp->c_ctime;
	from_cp->c_gid = to_cp->c_gid;
	from_cp->c_uid = to_cp->c_uid;
	from_cp->c_bsdflags = to_cp->c_bsdflags;
	from_cp->c_mode = to_cp->c_mode;
	from_cp->c_linkcount = to_cp->c_linkcount;
	from_cp->c_attr.ca_linkref = to_cp->c_attr.ca_linkref;
	from_cp->c_attr.ca_firstlink = to_cp->c_attr.ca_firstlink;

	/* 
	 * The cnode flags need to stay with the cnode and not get transferred
	 * over along with everything else because they describe the content; they are
	 * not attributes that reflect changes specific to the file ID.  In general, 
	 * fields that are tied to the file ID are the ones that will move.
	 * 
	 * This reflects the fact that the file may have borrowed blocks, dirty metadata, 
	 * or other extents, which may not yet have been written to the catalog.  If 
	 * they were, they would have been transferred above in the ExchangeFileIDs call above...
	 *
	 * The flags that are special are:
	 * C_HARDLINK, C_HASXATTRS
	 * 
	 * These flags move with the item and file ID in the namespace since their
	 * state is tied to that of the file ID.
	 * 
	 * So to transfer the flags, we have to take the following steps
	 * 1) Store in a localvar whether or not the special bits are set.
	 * 2) Drop the special bits from the current flags
	 * 3) swap the special flag bits to their destination
	 */	 
<<<<<<< HEAD
	from_cp->c_flag |= to_flag_special | C_MODIFIED;
=======
	from_cp->c_flag |= to_flag_special;
	
>>>>>>> origin/10.8
	from_cp->c_attr.ca_recflags = to_cp->c_attr.ca_recflags;
	bcopy(to_cp->c_finderinfo, from_cp->c_finderinfo, 32);


	/* Copy the "from" -> "to" cnode */
	bcopy(&tempdesc, &to_cp->c_desc, sizeof(struct cat_desc));
	to_cp->c_hint = 0;
	/* 
	 * Pull the file ID from the tempattr we copied above. We can't assume 
	 * it is the same as the CNID.
	 */
	to_cp->c_fileid = tempattr.ca_fileid;
	to_cp->c_itime = tempattr.ca_itime;
	to_cp->c_btime = tempattr.ca_btime;
	to_cp->c_atime = tempattr.ca_atime;
	to_cp->c_ctime = tempattr.ca_ctime;
	to_cp->c_gid = tempattr.ca_gid;
	to_cp->c_uid = tempattr.ca_uid;
	to_cp->c_bsdflags = tempattr.ca_flags;
	to_cp->c_mode = tempattr.ca_mode;
	to_cp->c_linkcount = tempattr.ca_linkcount;
	to_cp->c_attr.ca_linkref = tempattr.ca_linkref;
	to_cp->c_attr.ca_firstlink = tempattr.ca_firstlink;

	/* 
	 * Only OR in the "from" flags into our cnode flags below. 
	 * Leave the rest of the flags alone.
	 */
<<<<<<< HEAD
	to_cp->c_flag |= from_flag_special | C_MODIFIED;
=======
	to_cp->c_flag |= from_flag_special;
>>>>>>> origin/10.8

	to_cp->c_attr.ca_recflags = tempattr.ca_recflags;
	bcopy(tempattr.ca_finderinfo, to_cp->c_finderinfo, 32);


	/* Rehash the cnodes using their new file IDs */
	hfs_chash_rehash(hfsmp, from_cp, to_cp);

	/*
	 * When a file moves out of "Cleanup At Startup"
	 * we can drop its NODUMP status.
	 */
	if ((from_cp->c_bsdflags & UF_NODUMP) &&
	    (from_cp->c_parentcnid != to_cp->c_parentcnid)) {
		from_cp->c_bsdflags &= ~UF_NODUMP;
		from_cp->c_touch_chgtime = TRUE;
	}
	if ((to_cp->c_bsdflags & UF_NODUMP) &&
	    (to_cp->c_parentcnid != from_cp->c_parentcnid)) {
		to_cp->c_bsdflags &= ~UF_NODUMP;
		to_cp->c_touch_chgtime = TRUE;
	}

exit:
	if (got_cookie) {
		cat_postflight(hfsmp, &cookie, vfs_context_proc(ap->a_context));
	}
	if (started_tr) {
	    hfs_end_transaction(hfsmp);
	}

	if (have_cnode_locks)
		hfs_unlockpair(from_cp, to_cp);

	if (have_from_trunc_lock)
		hfs_unlock_truncate(from_cp, 0);

	if (have_to_trunc_lock)
		hfs_unlock_truncate(to_cp, 0);

 	return (error);
}

#if HFS_COMPRESSION
/*
 * This function is used specifically for the case when a namespace
 * handler is trying to steal data before it's deleted.  Note that we
 * don't bother deleting the xattr from the source because it will get
 * deleted a short time later anyway.
 *
 * cnodes must be locked
 */
static int hfs_move_compressed(cnode_t *from_cp, cnode_t *to_cp)
{
	int 	ret;
	void   *data 	= NULL;

	CLR(from_cp->c_bsdflags, UF_COMPRESSED);
	SET(from_cp->c_flag, C_MODIFIED);

	ret = hfs_move_data(from_cp, to_cp, HFS_MOVE_DATA_INCLUDE_RSRC);
	if (ret)
		goto exit;

	/*
	 * Transfer the xattr that decmpfs uses.  Ideally, this code
	 * should be with the other decmpfs code but it's file system
	 * agnostic and this path is currently, and likely to remain, HFS+
	 * specific.  It's easier and more performant if we implement it
	 * here.
	 */

	size_t size = MAX_DECMPFS_XATTR_SIZE;
	MALLOC(data, void *, size, M_TEMP, M_WAITOK);

	ret = hfs_xattr_read(from_cp->c_vp, DECMPFS_XATTR_NAME, data, &size);
	if (ret)
		goto exit;

	ret = hfs_xattr_write(to_cp->c_vp, DECMPFS_XATTR_NAME, data, size);
	if (ret)
		goto exit;

	SET(to_cp->c_bsdflags, UF_COMPRESSED);
	SET(to_cp->c_flag, C_MODIFIED);

exit:
	if (data)
		FREE(data, M_TEMP);

	return ret;
}
#endif // HFS_COMPRESSION

int
hfs_vnop_mmap(struct vnop_mmap_args *ap)
{
	struct vnode *vp = ap->a_vp;
	cnode_t *cp = VTOC(vp);
	int error;
	
	if (VNODE_IS_RSRC(vp)) {
		/* allow pageins of the resource fork */
	} else {
		int compressed = hfs_file_is_compressed(cp, 1); /* 1 == don't take the cnode lock */
		time_t orig_ctime = cp->c_ctime;
		
		if (!compressed && (cp->c_bsdflags & UF_COMPRESSED)) {
			error = check_for_dataless_file(vp, NAMESPACE_HANDLER_READ_OP);
			if (error != 0) {
				return error;
			}
		}

		if (ap->a_fflags & PROT_WRITE) {
			check_for_tracked_file(vp, orig_ctime, NAMESPACE_HANDLER_WRITE_OP, NULL);
		}
	}

	//
	// NOTE: we return ENOTSUP because we want the cluster layer
	//       to actually do all the real work.
	//
	return (ENOTSUP);
}

static errno_t hfs_vnop_mnomap(struct vnop_mnomap_args *ap)
{
	vnode_t vp = ap->a_vp;

	/*
	 * Whilst the file was mapped, there may not have been any
	 * page-outs so we need to increment the generation counter now.
	 * Unfortunately this may lead to a change in the generation
	 * counter when no actual change has been made, but there is
	 * little we can do about that with our current architecture.
	 */
	if (ubc_is_mapped_writable(vp)) {
		cnode_t *cp = VTOC(vp);
		hfs_lock(cp, HFS_EXCLUSIVE_LOCK, HFS_LOCK_ALLOW_NOEXISTS);
		hfs_incr_gencount(cp);

		/*
		 * We don't want to set the modification time here since a
		 * change to that is not acceptable if no changes were made.
		 * Instead we set a flag so that if we get any page-outs we
		 * know to update the modification time.  It's possible that
		 * they weren't actually because of changes made whilst the
		 * file was mapped but that's not easy to fix now.
		 */
		SET(cp->c_flag, C_MIGHT_BE_DIRTY_FROM_MAPPING);

		hfs_unlock(cp);
	}

	return 0;
}

/*
 * Mark the resource fork as needing a ubc_setsize when we drop the
 * cnode lock later.
 */
static void hfs_rsrc_setsize(cnode_t *cp)
{
<<<<<<< HEAD
	/*
	 * We need to take an iocount if we don't have one.  vnode_get
	 * will return ENOENT if the vnode is terminating which is what we
	 * want as it's not safe to call ubc_setsize in that case.
	 */
	if (cp->c_rsrc_vp && !vnode_get(cp->c_rsrc_vp)) {
		// Shouldn't happen, but better safe...
		if (ISSET(cp->c_flag, C_NEED_RVNODE_PUT))
			vnode_put(cp->c_rsrc_vp);
		SET(cp->c_flag, C_NEED_RVNODE_PUT | C_NEED_RSRC_SETSIZE);
	}
}
=======
	struct vnode *from_vp = ap->a_fvp;
	struct vnode *to_vp = ap->a_tvp;
	struct vnode *from_rvp = NULL;
	struct vnode *to_rvp = NULL;
	struct cnode *from_cp = VTOC(from_vp);
	struct cnode *to_cp = VTOC(to_vp);
	struct hfsmount *hfsmp = VTOHFS(from_vp);
	struct cat_desc tempdesc;
	struct cat_attr tempattr;
	int error = 0, started_tr = 0, grabbed_lock = 0;
>>>>>>> origin/10.2

/*
 * hfs_move_data
 *
 * This is a non-symmetric variant of exchangedata.  In this function,
 * the contents of the data fork (and optionally the resource fork)
 * are moved from from_cp to to_cp.
 * 
 * The cnodes must be locked. 
 *
 * The cnode pointed to by 'to_cp' *must* be empty prior to invoking
 * this function.  We impose this restriction because we may not be
 * able to fully delete the entire file's contents in a single
 * transaction, particularly if it has a lot of extents.  In the
 * normal file deletion codepath, the file is screened for two
 * conditions: 1) bigger than 400MB, and 2) more than 8 extents.  If
 * so, the file is relocated to the hidden directory and the deletion
 * is broken up into multiple truncates.  We can't do that here
 * because both files need to exist in the namespace. The main reason
 * this is imposed is that we may have to touch a whole lot of bitmap
 * blocks if there are many extents.
 * 
 * Any data written to 'from_cp' after this call completes is not
 * guaranteed to be moved.
 * 
 * Arguments:
 * cnode_t *from_cp : source file
 * cnode_t *to_cp   : destination file; must be empty
 * 
 * Returns:
 *  
 *  EBUSY - File has been deleted or is in use
 *	EFBIG - Destination file was not empty
 *  EIO   - An I/O error
 *	0	  - success
 *  other - Other errors that can be returned from called functions
 */
int hfs_move_data(cnode_t *from_cp, cnode_t *to_cp, 
				  hfs_move_data_options_t options)
{	
	hfsmount_t *hfsmp 			= VTOHFS(from_cp->c_vp);
	int error 					= 0;
	int lockflags				= 0;
	bool return_EIO_on_error 	= false;
	const bool include_rsrc		= ISSET(options, HFS_MOVE_DATA_INCLUDE_RSRC);

	/* Verify that neither source/dest file is open-unlinked */
	if (ISSET(from_cp->c_flag, C_DELETED | C_NOEXISTS)
		|| ISSET(to_cp->c_flag, C_DELETED | C_NOEXISTS)) {
		return EBUSY;
	}

	/* 
	 * Verify the source file is not in use by anyone besides us.
	 *
	 * This function is typically invoked by a namespace handler 
	 * process responding to a temporarily stalled system call.  
	 * The FD that it is working off of is opened O_EVTONLY, so
	 * it really has no active usecounts (the kusecount from O_EVTONLY
	 * is subtracted from the total usecounts).
	 * 
	 * As a result, we shouldn't have any active usecounts against
	 * this vnode when we go to check it below.
	 */
	if (vnode_isinuse(from_cp->c_vp, 0))
		return EBUSY;

	if (include_rsrc && from_cp->c_rsrc_vp) {
		if (vnode_isinuse(from_cp->c_rsrc_vp, 0))
			return EBUSY;

		/* 
		 * In the code below, if the destination file doesn't have a
		 * c_rsrcfork then we don't create it which means we we cannot
		 * transfer the ff_invalidranges and cf_vblocks fields.  These
		 * shouldn't be set because we flush the resource fork before
		 * calling this function but there is a tiny window when we
		 * did not have any locks...
		 */
		if (!to_cp->c_rsrcfork
			&& (!TAILQ_EMPTY(&from_cp->c_rsrcfork->ff_invalidranges)
				|| from_cp->c_rsrcfork->ff_unallocblocks)) {
			/*
			 * The file isn't really busy now but something did slip
			 * in and tinker with the file while we didn't have any
			 * locks, so this is the most meaningful return code for
			 * the caller.
			 */
			return EBUSY;
		}
	}

<<<<<<< HEAD
	// Check the destination file is empty
	if (to_cp->c_datafork->ff_blocks 
		|| to_cp->c_datafork->ff_size
		|| (include_rsrc
			&& (to_cp->c_blocks
				|| (to_cp->c_rsrcfork && to_cp->c_rsrcfork->ff_size)))) {
		return EFBIG;
	}
=======
	// XXXdbg - don't allow modification of the journal or journal_info_block
	if (hfsmp->jnl) {
		struct HFSPlusExtentDescriptor *extd;

		if (from_cp->c_datafork) {
			extd = &from_cp->c_datafork->ff_data.cf_extents[0];
			if (extd->startBlock == VTOVCB(from_vp)->vcbJinfoBlock || extd->startBlock == hfsmp->jnl_start) {
				return EPERM;
			}
		}

		if (to_cp->c_datafork) {
			extd = &to_cp->c_datafork->ff_data.cf_extents[0];
			if (extd->startBlock == VTOVCB(to_vp)->vcbJinfoBlock || extd->startBlock == hfsmp->jnl_start) {
				return EPERM;
			}
		}
	}

	from_rvp = from_cp->c_rsrc_vp;
	to_rvp = to_cp->c_rsrc_vp;
>>>>>>> origin/10.2

	if ((error = hfs_start_transaction (hfsmp)))
		return error;

	lockflags = hfs_systemfile_lock(hfsmp, SFL_CATALOG | SFL_EXTENTS | SFL_ATTRIBUTE, 
									HFS_EXCLUSIVE_LOCK);

	// filefork_t is 128 bytes which should be OK
	filefork_t rfork_buf, *from_rfork = NULL;

	if (include_rsrc) {
		from_rfork = from_cp->c_rsrcfork;

		/*
		 * Creating resource fork vnodes is expensive, so just get get
		 * the fork data if we need it.
		 */
		if (!from_rfork && hfs_has_rsrc(from_cp)) {
			from_rfork = &rfork_buf;

			from_rfork->ff_cp = from_cp;
			TAILQ_INIT(&from_rfork->ff_invalidranges);

			error = cat_idlookup(hfsmp, from_cp->c_fileid, 0, 1, NULL, NULL,
								 &from_rfork->ff_data);

			if (error)
				goto exit;
		}
	}

	/*
	 * From here on, any failures mean that we might be leaving things
	 * in a weird or inconsistent state.  Ideally, we should back out
	 * all the changes, but to do that properly we need to fix
	 * MoveData.  We'll save fixing that for another time.  For now,
	 * just return EIO in all cases to the caller so that they know.
	 */
	return_EIO_on_error = true;

<<<<<<< HEAD
	bool data_overflow_extents = overflow_extents(from_cp->c_datafork);
=======
	// XXXdbg
	hfs_global_shared_lock_acquire(hfsmp);
	grabbed_lock = 1;
	if (hfsmp->jnl) {
	    if ((error = journal_start_transaction(hfsmp->jnl)) != 0) {
			goto Err_Exit;
	    }
		started_tr = 1;
	}
	
	/* Lock catalog b-tree */
	error = hfs_metafilelocking(hfsmp, kHFSCatalogFileID, LK_EXCLUSIVE, ap->a_p);
	if (error) goto Err_Exit;
>>>>>>> origin/10.2

	// Move the data fork
	if ((error = hfs_move_fork (from_cp->c_datafork, from_cp,
								to_cp->c_datafork, to_cp))) {
		goto exit;
	}

	SET(from_cp->c_flag, C_NEED_DATA_SETSIZE);
	SET(to_cp->c_flag, C_NEED_DATA_SETSIZE);

	// We move the resource fork later

	/* 
	 * Note that because all we're doing is moving the extents around,
	 * we can probably do this in a single transaction: Each extent
	 * record (group of 8) is 64 bytes.  A extent overflow B-Tree node
	 * is typically 4k.  This means each node can hold roughly ~60
	 * extent records == (480 extents).
	 *
	 * If a file was massively fragmented and had 20k extents, this
	 * means we'd roughly touch 20k/480 == 41 to 42 nodes, plus the
	 * index nodes, for half of the operation.  (inserting or
	 * deleting). So if we're manipulating 80-100 nodes, this is
	 * basically 320k of data to write to the journal in a bad case.
	 */
	if (data_overflow_extents) {
		if ((error = MoveData(hfsmp, from_cp->c_cnid, to_cp->c_cnid, 0)))
			goto exit;
	}

	if (from_rfork && overflow_extents(from_rfork)) {
		if ((error = MoveData(hfsmp, from_cp->c_cnid, to_cp->c_cnid, 1)))
			goto exit;
	}

	// Touch times
	from_cp->c_touch_acctime = TRUE;
	from_cp->c_touch_chgtime = TRUE;
	from_cp->c_touch_modtime = TRUE;
	hfs_touchtimes(hfsmp, from_cp);

	to_cp->c_touch_acctime = TRUE;
	to_cp->c_touch_chgtime = TRUE;
	to_cp->c_touch_modtime = TRUE;
	hfs_touchtimes(hfsmp, to_cp);

	struct cat_fork dfork_buf;
	const struct cat_fork *dfork, *rfork;

	dfork = hfs_prepare_fork_for_update(to_cp->c_datafork, NULL,
										&dfork_buf, hfsmp->blockSize);
	rfork = hfs_prepare_fork_for_update(from_rfork, NULL,
										&rfork_buf.ff_data, hfsmp->blockSize);

	// Update the catalog nodes, to_cp first
	if ((error = cat_update(hfsmp, &to_cp->c_desc, &to_cp->c_attr, 
							dfork, rfork))) {
		goto exit;
	}

	CLR(to_cp->c_flag, C_MODIFIED | C_MINOR_MOD);

	// Update in-memory resource fork data here
	if (from_rfork) {
		// Update c_blocks
		uint32_t moving = from_rfork->ff_blocks + from_rfork->ff_unallocblocks;

<<<<<<< HEAD
		from_cp->c_blocks -= moving;
		to_cp->c_blocks   += moving;
=======
	/*
	 * Swap the descriptors and all non-fork related attributes.
	 * (except the modify date)
	 */
	bcopy(&to_cp->c_desc, &from_cp->c_desc, sizeof(struct cat_desc));

	from_cp->c_hint = 0;
	from_cp->c_fileid = from_cp->c_cnid;
	from_cp->c_itime = to_cp->c_itime;
	from_cp->c_btime = to_cp->c_btime;
	from_cp->c_atime = to_cp->c_atime;
	from_cp->c_ctime = to_cp->c_ctime;
	from_cp->c_gid = to_cp->c_gid;
	from_cp->c_uid = to_cp->c_uid;
	from_cp->c_flags = to_cp->c_flags;
	from_cp->c_mode = to_cp->c_mode;
	bcopy(to_cp->c_finderinfo, from_cp->c_finderinfo, 32);
>>>>>>> origin/10.2

		// Update to_cp's resource data if it has it
		filefork_t *to_rfork = to_cp->c_rsrcfork;
		if (to_rfork) {
			TAILQ_SWAP(&to_rfork->ff_invalidranges,
					   &from_rfork->ff_invalidranges, rl_entry, rl_link);
			to_rfork->ff_data = from_rfork->ff_data;

			// Deal with ubc_setsize
			hfs_rsrc_setsize(to_cp);
		}

<<<<<<< HEAD
		// Wipe out the resource fork in from_cp
		rl_init(&from_rfork->ff_invalidranges);
		bzero(&from_rfork->ff_data, sizeof(from_rfork->ff_data));

		// Deal with ubc_setsize
		hfs_rsrc_setsize(from_cp);
	}

	// Currently unnecessary, but might be useful in future...
	dfork = hfs_prepare_fork_for_update(from_cp->c_datafork, NULL, &dfork_buf,
										hfsmp->blockSize);
	rfork = hfs_prepare_fork_for_update(from_rfork, NULL, &rfork_buf.ff_data,
										hfsmp->blockSize);

	// Update from_cp
	if ((error = cat_update(hfsmp, &from_cp->c_desc, &from_cp->c_attr, 
							dfork, rfork))) {
		goto exit;
	}

	CLR(from_cp->c_flag, C_MODIFIED | C_MINOR_MOD);

exit:
	if (lockflags) {
		hfs_systemfile_unlock(hfsmp, lockflags);
		hfs_end_transaction(hfsmp);
	}

	if (error && error != EIO && return_EIO_on_error) {
		printf("hfs_move_data: encountered error %d\n", error);
		error = EIO;
	}

	return error;
=======
	hfs_chashinsert(from_cp);
	hfs_chashinsert(to_cp);

	/*
	 * When a file moves out of "Cleanup At Startup"
	 * we can drop its NODUMP status.
	 */
	if ((from_cp->c_flags & UF_NODUMP) &&
	    (from_cp->c_parentcnid != to_cp->c_parentcnid)) {
		from_cp->c_flags &= ~UF_NODUMP;
		from_cp->c_flag |= C_CHANGE;
	}

	if ((to_cp->c_flags & UF_NODUMP) &&
	    (to_cp->c_parentcnid != from_cp->c_parentcnid)) {
		to_cp->c_flags &= ~UF_NODUMP;
		to_cp->c_flag |= C_CHANGE;
	}

Err_Exit:
	if (to_rvp)
		vrele(to_rvp);
	if (from_rvp)
		vrele(from_rvp);

	// XXXdbg
	if (started_tr) {
	    journal_end_transaction(hfsmp->jnl);
	}
	if (grabbed_lock) {
		hfs_global_shared_lock_release(hfsmp);
	}

	return (error);
>>>>>>> origin/10.2
}

/* 
 * Move all of the catalog and runtime data in srcfork to dstfork.
 * 
 * This allows us to maintain the invalid ranges across the move data
 * operation so we don't need to force all of the pending IO right
 * now. In addition, we move all non overflow-extent extents into the
 * destination here.
 *
 * The destination fork must be empty and should have been checked
 * prior to calling this.
 */
static int hfs_move_fork(filefork_t *srcfork, cnode_t *src_cp,
						 filefork_t *dstfork, cnode_t *dst_cp) 
{
	// Move the invalid ranges
	TAILQ_SWAP(&dstfork->ff_invalidranges, &srcfork->ff_invalidranges,
			   rl_entry, rl_link);
	rl_remove_all(&srcfork->ff_invalidranges);

	// Move the fork data (copy whole structure)
	dstfork->ff_data = srcfork->ff_data;
	bzero(&srcfork->ff_data, sizeof(srcfork->ff_data));

	// Update c_blocks
	src_cp->c_blocks -= dstfork->ff_blocks + dstfork->ff_unallocblocks;
	dst_cp->c_blocks += dstfork->ff_blocks + dstfork->ff_unallocblocks;

<<<<<<< HEAD
	return 0;
}


#include <i386/panic_hooks.h>

struct hfs_fsync_panic_hook {
	panic_hook_t hook;
	struct cnode *cp;
};

static void hfs_fsync_panic_hook(panic_hook_t *hook_)
{
	struct hfs_fsync_panic_hook *hook = (struct hfs_fsync_panic_hook *)hook_;
	extern int kdb_log(const char *fmt, ...);

	// Get the physical region just before cp
	panic_phys_range_t range;
	uint64_t phys;

	if (panic_phys_range_before(hook->cp, &phys, &range)) {
		kdb_log("cp = %p, phys = %p, prev (%p: %p-%p)\n", 
				hook->cp, phys, range.type, range.phys_start,
				range.phys_start + range.len);
	} else
		kdb_log("cp = %p, phys = %p, prev (!)\n", hook->cp, phys);

	panic_dump_mem((void *)(((vm_offset_t)hook->cp - 4096) & ~4095), 12288);

	kdb_log("\n");
}


/*
 *  cnode must be locked
 */
int
hfs_fsync(struct vnode *vp, int waitfor, hfs_fsync_mode_t fsyncmode, struct proc *p)
=======
     */
static int
hfs_fsync(ap)
	struct vop_fsync_args /* {
		struct vnode *a_vp;
		struct ucred *a_cred;
		int a_waitfor;
		struct proc *a_p;
	} */ *ap;
>>>>>>> origin/10.2
{
	struct cnode *cp = VTOC(vp);
	struct filefork *fp = NULL;
	int retval = 0;
	struct hfsmount *hfsmp = VTOHFS(vp);
	struct timeval tv;
<<<<<<< HEAD
	int waitdata;		/* attributes necessary for data retrieval */
	int wait;		/* all other attributes (e.g. atime, etc.) */
	int lockflag;
	int took_trunc_lock = 0;
	int locked_buffers = 0;
	int fsync_default = 1;
=======
	struct buf *nbp;
	struct hfsmount *hfsmp = VTOHFS(ap->a_vp);
	int s;
	int wait;
	int retry = 0;
>>>>>>> origin/10.2

	/*
	 * Applications which only care about data integrity rather than full
	 * file integrity may opt out of (delay) expensive metadata update
	 * operations as a performance optimization.
	 */
	wait = (waitfor == MNT_WAIT);
	waitdata = (waitfor == MNT_DWAIT) | wait;

	if (always_do_fullfsync)
		fsyncmode = HFS_FSYNC_FULL;
	if (fsyncmode != HFS_FSYNC)
		fsync_default = 0;
	
	/* HFS directories don't have any data blocks. */
	if (vnode_isdir(vp))
		goto metasync;
	fp = VTOF(vp);

	/*
	 * For system files flush the B-tree header and
	 * for regular files write out any clusters
	 */
<<<<<<< HEAD
	if (vnode_issystem(vp)) {
	    if (VTOF(vp)->fcbBTCBPtr != NULL) {
			// XXXdbg
			if (hfsmp->jnl == NULL) {
				BTFlushPath(VTOF(vp));
			}
	    }
	} else if (UBCINFOEXISTS(vp)) {
		hfs_unlock(cp);
		hfs_lock_truncate(cp, HFS_SHARED_LOCK, HFS_LOCK_DEFAULT);
		took_trunc_lock = 1;
=======
	if (vp->v_flag & VSYSTEM) {
	    if (VTOF(vp)->fcbBTCBPtr != NULL) {
			// XXXdbg
			if (hfsmp->jnl) {
				if (BTIsDirty(VTOF(vp))) {
					panic("hfs: system file vp 0x%x has dirty blocks (jnl 0x%x)\n",
						  vp, hfsmp->jnl);
				}
			} else {
				BTFlushPath(VTOF(vp));
			}
	    }
	} else if (UBCINFOEXISTS(vp))
		(void) cluster_push(vp);
>>>>>>> origin/10.2

		struct hfs_fsync_panic_hook hook;
		hook.cp = cp;
		panic_hook(&hook.hook, hfs_fsync_panic_hook);

		if (fp->ff_unallocblocks != 0) {
			hfs_unlock_truncate(cp, HFS_LOCK_DEFAULT);

			hfs_lock_truncate(cp, HFS_EXCLUSIVE_LOCK, HFS_LOCK_DEFAULT);
		}

		panic_unhook(&hook.hook);

		/* Don't hold cnode lock when calling into cluster layer. */
		(void) cluster_push(vp, waitdata ? IO_SYNC : 0);

		hfs_lock(cp, HFS_EXCLUSIVE_LOCK, HFS_LOCK_ALLOW_NOEXISTS);
	}
	/*
	 * When MNT_WAIT is requested and the zero fill timeout
	 * has expired then we must explicitly zero out any areas
	 * that are currently marked invalid (holes).
	 *
	 * Files with NODUMP can bypass zero filling here.
	 */
<<<<<<< HEAD
	if (fp && (((cp->c_flag & C_ALWAYS_ZEROFILL) && !TAILQ_EMPTY(&fp->ff_invalidranges)) ||
	    ((wait || (cp->c_flag & C_ZFWANTSYNC)) &&
		((cp->c_bsdflags & UF_NODUMP) == 0) &&
		UBCINFOEXISTS(vp) && (vnode_issystem(vp) ==0) &&
		cp->c_zftimeout != 0))) {

		microuptime(&tv);
		if ((cp->c_flag & C_ALWAYS_ZEROFILL) == 0 && fsync_default && tv.tv_sec < (long)cp->c_zftimeout) {
=======
	if ((wait || (cp->c_flag & C_ZFWANTSYNC)) &&
	    ((cp->c_flags & UF_NODUMP) == 0) &&
	    UBCINFOEXISTS(vp) && (fp = VTOF(vp)) &&
	    cp->c_zftimeout != 0) {
		int devblksize;
		int was_nocache;

		if (time.tv_sec < cp->c_zftimeout) {
>>>>>>> origin/10.2
			/* Remember that a force sync was requested. */
			cp->c_flag |= C_ZFWANTSYNC;
			goto datasync;
		}
		if (!TAILQ_EMPTY(&fp->ff_invalidranges)) {
			if (!took_trunc_lock || (cp->c_truncatelockowner == HFS_SHARED_OWNER)) {
				hfs_unlock(cp);
				if (took_trunc_lock) {
					hfs_unlock_truncate(cp, HFS_LOCK_DEFAULT);
				}
				hfs_lock_truncate(cp, HFS_EXCLUSIVE_LOCK, HFS_LOCK_DEFAULT);
				hfs_lock(cp, HFS_EXCLUSIVE_LOCK, HFS_LOCK_ALLOW_NOEXISTS);
				took_trunc_lock = 1;
			}
			hfs_flush_invalid_ranges(vp);
			hfs_unlock(cp);
			(void) cluster_push(vp, waitdata ? IO_SYNC : 0);
			hfs_lock(cp, HFS_EXCLUSIVE_LOCK, HFS_LOCK_ALLOW_NOEXISTS);
		}
	}
datasync:
	if (took_trunc_lock) {
		hfs_unlock_truncate(cp, HFS_LOCK_DEFAULT);
		took_trunc_lock = 0;
	}
	/*
	 * if we have a journal and if journal_active() returns != 0 then the
	 * we shouldn't do anything to a locked block (because it is part 
	 * of a transaction).  otherwise we'll just go through the normal 
	 * code path and flush the buffer.  note journal_active() can return
	 * -1 if the journal is invalid -- however we still need to skip any 
	 * locked blocks as they get cleaned up when we finish the transaction
	 * or close the journal.
	 */
	// if (hfsmp->jnl && journal_active(hfsmp->jnl) >= 0)
	if (hfsmp->jnl)
	        lockflag = BUF_SKIP_LOCKED;
	else
	        lockflag = 0;

	/*
	 * Flush all dirty buffers associated with a vnode.
	 * Record how many of them were dirty AND locked (if necessary).
	 */
<<<<<<< HEAD
	locked_buffers = buf_flushdirtyblks_skipinfo(vp, waitdata, lockflag, "hfs_fsync");
	if ((lockflag & BUF_SKIP_LOCKED) && (locked_buffers) && (vnode_vtype(vp) == VLNK)) {
		/* 
		 * If there are dirty symlink buffers, then we may need to take action
		 * to prevent issues later on if we are journaled. If we're fsyncing a 
		 * symlink vnode then we are in one of three cases:
		 * 
		 * 1) automatic sync has fired.  In this case, we don't want the behavior to change.
		 * 
		 * 2) Someone has opened the FD for the symlink (not what it points to)
		 * and has issued an fsync against it.  This should be rare, and we don't
		 * want the behavior to change.
		 * 
		 * 3) We are being called by a vclean which is trying to reclaim this
		 * symlink vnode.  If this is the case, then allowing this fsync to 
		 * proceed WITHOUT flushing the journal could result in the vclean 
		 * invalidating the buffer's blocks before the journal transaction is
		 * written to disk.  To prevent this, we force a journal flush 
		 * if the vnode is in the middle of a recycle (VL_TERMINATE or VL_DEAD is set).
=======
loop:
	s = splbio();
	for (bp = vp->v_dirtyblkhd.lh_first; bp; bp = nbp) {
		nbp = bp->b_vnbufs.le_next;
		if ((bp->b_flags & B_BUSY))
			continue;
		if ((bp->b_flags & B_DELWRI) == 0)
			panic("hfs_fsync: bp 0x% not dirty (hfsmp 0x%x)", bp, hfsmp);
		// XXXdbg
		if (hfsmp->jnl && (bp->b_flags & B_LOCKED)) {
			if ((bp->b_flags & B_META) == 0) {
				panic("hfs: bp @ 0x%x is locked but not meta! jnl 0x%x\n",
					  bp, hfsmp->jnl);
			}
			// if journal_active() returns >= 0 then the journal is ok and we 
			// shouldn't do anything to this locked block (because it is part 
			// of a transaction).  otherwise we'll just go through the normal 
			// code path and flush the buffer.
			if (journal_active(hfsmp->jnl) >= 0) {
				continue;
			}
		}

		bremfree(bp);
		bp->b_flags |= B_BUSY;
		/* Clear B_LOCKED, should only be set on meta files */
		bp->b_flags &= ~B_LOCKED;

		splx(s);
		/*
		 * Wait for I/O associated with indirect blocks to complete,
		 * since there is no way to quickly wait for them below.
>>>>>>> origin/10.2
		 */
		if (vnode_isrecycled(vp)) {
			fsync_default = 0;
		}
	}

metasync:
	if (vnode_isreg(vp) && vnode_issystem(vp)) {
		if (VTOF(vp)->fcbBTCBPtr != NULL) {
			microuptime(&tv);
			BTSetLastSync(VTOF(vp), tv.tv_sec);
		}
		cp->c_touch_acctime = FALSE;
		cp->c_touch_chgtime = FALSE;
		cp->c_touch_modtime = FALSE;
	} else if ( !(vp->v_flag & VSWAP) ) /* User file */ {
		retval = hfs_update(vp, HFS_UPDATE_FORCE);

<<<<<<< HEAD
		/*
		 * When MNT_WAIT is requested push out the catalog record for
		 * this file.  If they asked for a full fsync, we can skip this
		 * because the journal_flush or hfs_metasync_all will push out
		 * all of the metadata changes.
		 */
		if ((retval == 0) && wait && fsync_default && cp->c_hint &&
   		    !ISSET(cp->c_flag, C_DELETED | C_NOEXISTS)) {
   			hfs_metasync(VTOHFS(vp), (daddr64_t)cp->c_hint, p);
		}

		/*
		 * If this was a full fsync, make sure all metadata
		 * changes get to stable storage.
		 */
		if (!fsync_default) {
			if (hfsmp->jnl) {
				if (fsyncmode == HFS_FSYNC_FULL)
					hfs_flush(hfsmp, HFS_FLUSH_FULL);
				else
					hfs_flush(hfsmp,
					    HFS_FLUSH_JOURNAL_BARRIER);
=======
		// XXXdbg -- is checking for hfsmp->jnl == NULL the right
		//           thing to do?
		if (hfsmp->jnl == NULL && vp->v_dirtyblkhd.lh_first) {
			/* still have some dirty buffers */
			if (retry++ > 10) {
				vprint("hfs_fsync: dirty", vp);
				splx(s);
				/*
				 * Looks like the requests are not
				 * getting queued to the driver.
				 * Retrying here causes a cpu bound loop.
				 * Yield to the other threads and hope
				 * for the best.
				 */
				(void)tsleep((caddr_t)&vp->v_numoutput,
					PRIBIO + 1, "hfs_fsync", hz/10);
				retry = 0;
>>>>>>> origin/10.2
			} else {
				retval = hfs_metasync_all(hfsmp);
				/* XXX need to pass context! */
				hfs_flush(hfsmp, HFS_FLUSH_CACHE);
			}
		}
	}

	if (!hfs_is_dirty(cp) && !ISSET(cp->c_flag, C_DELETED))
		vnode_cleardirty(vp);

	return (retval);
}


/* Sync an hfs catalog b-tree node */
int
hfs_metasync(struct hfsmount *hfsmp, daddr64_t node, __unused struct proc *p)
{
	vnode_t	vp;
	buf_t	bp;
	int lockflags;

	vp = HFSTOVCB(hfsmp)->catalogRefNum;

	// XXXdbg - don't need to do this on a journaled volume
	if (hfsmp->jnl) {
		return 0;
	}
<<<<<<< HEAD
=======

	if (hfs_metafilelocking(hfsmp, kHFSCatalogFileID, LK_EXCLUSIVE, p) != 0)
		return (0);
>>>>>>> origin/10.2

	lockflags = hfs_systemfile_lock(hfsmp, SFL_CATALOG, HFS_EXCLUSIVE_LOCK);
	/*
	 * Look for a matching node that has been delayed
	 * but is not part of a set (B_LOCKED).
	 *
	 * BLK_ONLYVALID causes buf_getblk to return a
	 * buf_t for the daddr64_t specified only if it's
	 * currently resident in the cache... the size
	 * parameter to buf_getblk is ignored when this flag
	 * is set
	 */
	bp = buf_getblk(vp, node, 0, 0, 0, BLK_META | BLK_ONLYVALID);

	if (bp) {
	        if ((buf_flags(bp) & (B_LOCKED | B_DELWRI)) == B_DELWRI)
		        (void) VNOP_BWRITE(bp);
		else
		        buf_brelse(bp);
	}

	hfs_systemfile_unlock(hfsmp, lockflags);

	return (0);
}


/*
 * Sync all hfs B-trees.  Use this instead of journal_flush for a volume
 * without a journal.  Note that the volume bitmap does not get written;
 * we rely on fsck_hfs to fix that up (which it can do without any loss
 * of data).
 */
int
hfs_metasync_all(struct hfsmount *hfsmp)
{
	int lockflags;

	/* Lock all of the B-trees so we get a mutually consistent state */
	lockflags = hfs_systemfile_lock(hfsmp,
		SFL_CATALOG|SFL_EXTENTS|SFL_ATTRIBUTE, HFS_EXCLUSIVE_LOCK);

	/* Sync each of the B-trees */
	if (hfsmp->hfs_catalog_vp)
		hfs_btsync(hfsmp->hfs_catalog_vp, 0);
	if (hfsmp->hfs_extents_vp)
		hfs_btsync(hfsmp->hfs_extents_vp, 0);
	if (hfsmp->hfs_attribute_vp)
		hfs_btsync(hfsmp->hfs_attribute_vp, 0);
	
	/* Wait for all of the writes to complete */
	if (hfsmp->hfs_catalog_vp)
		vnode_waitforwrites(hfsmp->hfs_catalog_vp, 0, 0, 0, "hfs_metasync_all");
	if (hfsmp->hfs_extents_vp)
		vnode_waitforwrites(hfsmp->hfs_extents_vp, 0, 0, 0, "hfs_metasync_all");
	if (hfsmp->hfs_attribute_vp)
		vnode_waitforwrites(hfsmp->hfs_attribute_vp, 0, 0, 0, "hfs_metasync_all");

	hfs_systemfile_unlock(hfsmp, lockflags);
	
	return 0;
}


/*ARGSUSED 1*/
static int
hfs_btsync_callback(struct buf *bp, __unused void *dummy)
{
	buf_clearflags(bp, B_LOCKED);
	(void) buf_bawrite(bp);

	return(BUF_CLAIMED);
}


int
hfs_btsync(struct vnode *vp, int sync_transaction)
{
	struct cnode *cp = VTOC(vp);
	struct timeval tv;
<<<<<<< HEAD
	int    flags = 0;

	if (sync_transaction)
	        flags |= BUF_SKIP_NONLOCKED;
	/*
	 * Flush all dirty buffers associated with b-tree.
	 */
	buf_iterate(vp, hfs_btsync_callback, flags, 0);

	microuptime(&tv);
	if (vnode_issystem(vp) && (VTOF(vp)->fcbBTCBPtr != NULL))
		(void) BTSetLastSync(VTOF(vp), tv.tv_sec);
	cp->c_touch_acctime = FALSE;
	cp->c_touch_chgtime = FALSE;
	cp->c_touch_modtime = FALSE;

	return 0;
}

/*
 * Remove a directory.
 */
int
hfs_vnop_rmdir(ap)
	struct vnop_rmdir_args /* {
		struct vnode *a_dvp;
		struct vnode *a_vp;
		struct componentname *a_cnp;
		vfs_context_t a_context;
	} */ *ap;
{
	struct vnode *dvp = ap->a_dvp;
	struct vnode *vp = ap->a_vp;
	struct cnode *dcp = VTOC(dvp);
	struct cnode *cp = VTOC(vp);
	int error;
	time_t orig_ctime;
=======
	struct buf *nbp;
	struct hfsmount *hfsmp = VTOHFS(vp);
	int s;
>>>>>>> origin/10.2

	orig_ctime = VTOC(vp)->c_ctime;

	if (!S_ISDIR(cp->c_mode)) {
		return (ENOTDIR);
	}
	if (dvp == vp) {
		return (EINVAL);
	}

<<<<<<< HEAD
	check_for_tracked_file(vp, orig_ctime, NAMESPACE_HANDLER_DELETE_OP, NULL);
	cp = VTOC(vp);

	if ((error = hfs_lockpair(dcp, cp, HFS_EXCLUSIVE_LOCK))) {
		return (error);
	}
=======
	for (bp = vp->v_dirtyblkhd.lh_first; bp; bp = nbp) {
		nbp = bp->b_vnbufs.le_next;
		if ((bp->b_flags & B_BUSY))
			continue;
		if ((bp->b_flags & B_DELWRI) == 0)
			panic("hfs_btsync: not dirty (bp 0x%x hfsmp 0x%x)", bp, hfsmp);

		// XXXdbg
		if (hfsmp->jnl && (bp->b_flags & B_LOCKED)) {
			if ((bp->b_flags & B_META) == 0) {
				panic("hfs: bp @ 0x%x is locked but not meta! jnl 0x%x\n",
					  bp, hfsmp->jnl);
			}
			// if journal_active() returns >= 0 then the journal is ok and we 
			// shouldn't do anything to this locked block (because it is part 
			// of a transaction).  otherwise we'll just go through the normal 
			// code path and flush the buffer.
			if (journal_active(hfsmp->jnl) >= 0) {
			    continue;
			}
		}

		if (sync_transaction && !(bp->b_flags & B_LOCKED))
			continue;

		bremfree(bp);
		bp->b_flags |= B_BUSY;
		bp->b_flags &= ~B_LOCKED;

		splx(s);
>>>>>>> origin/10.2

	/* Check for a race with rmdir on the parent directory */
	if (dcp->c_flag & (C_DELETED | C_NOEXISTS)) {
		hfs_unlockpair (dcp, cp);
		return ENOENT;
	}

	//
	// if the item is tracked but doesn't have a document_id, assign one and generate an fsevent for it
	//
	if ((cp->c_bsdflags & UF_TRACKED) && ((struct FndrExtendedDirInfo *)((char *)&cp->c_attr.ca_finderinfo + 16))->document_id == 0) {
		uint32_t newid;

		hfs_unlockpair(dcp, cp);

		if (hfs_generate_document_id(VTOHFS(vp), &newid) == 0) {
			hfs_lockpair(dcp, cp, HFS_EXCLUSIVE_LOCK);
			((struct FndrExtendedDirInfo *)((char *)&cp->c_attr.ca_finderinfo + 16))->document_id = newid;
#if CONFIG_FSE
			add_fsevent(FSE_DOCID_CHANGED, vfs_context_current(),
				    FSE_ARG_DEV,   VTOHFS(vp)->hfs_raw_dev,
				    FSE_ARG_INO,   (ino64_t)0,             // src inode #
				    FSE_ARG_INO,   (ino64_t)cp->c_fileid,  // dst inode #
				    FSE_ARG_INT32, newid,
				    FSE_ARG_DONE);
#endif
		} else {
			// XXXdbg - couldn't get a new docid... what to do?  can't really fail the rm...
			hfs_lockpair(dcp, cp, HFS_EXCLUSIVE_LOCK);
		}
	}

	error = hfs_removedir(dvp, vp, ap->a_cnp, 0, 0);

	hfs_unlockpair(dcp, cp);

	return (error);
}

/*
 * Remove a directory
 *
 * Both dvp and vp cnodes are locked
 */
int
hfs_removedir(struct vnode *dvp, struct vnode *vp, struct componentname *cnp,
              int skip_reserve, int only_unlink)
{
	struct cnode *cp;
	struct cnode *dcp;
	struct hfsmount * hfsmp;
<<<<<<< HEAD
	struct cat_desc desc;
	int lockflags;
	int error = 0, started_tr = 0;
=======
	struct timeval tv;
	int error = 0, started_tr = 0, grabbed_lock = 0;
>>>>>>> origin/10.2

	cp = VTOC(vp);
	dcp = VTOC(dvp);
	hfsmp = VTOHFS(vp);

	if (dcp == cp) {
		return (EINVAL);	/* cannot remove "." */
	}
<<<<<<< HEAD
	if (cp->c_flag & (C_NOEXISTS | C_DELETED)) {
		return (0);
	}
	if (cp->c_entries != 0) {
		return (ENOTEMPTY);
	}
	
	/*
	 * If the directory is open or in use (e.g. opendir() or current working
	 * directory for some process); wait for inactive/reclaim to actually
	 * remove cnode from the catalog.  Both inactive and reclaim codepaths are capable
	 * of removing open-unlinked directories from the catalog, as well as getting rid
	 * of EAs still on the element.  So change only_unlink to true, so that it will get 
	 * cleaned up below.
	 *
	 * Otherwise, we can get into a weird old mess where the directory has C_DELETED,
	 * but it really means C_NOEXISTS because the item was actually removed from the 
	 * catalog.  Then when we try to remove the entry from the catalog later on, it won't
	 * really be there anymore.  
	 */
	if (vnode_isinuse(vp, 0))  {
		only_unlink = 1;
	}

	/* Deal with directory hardlinks */
	if (cp->c_flag & C_HARDLINK) {
		/* 
		 * Note that if we have a directory which was a hardlink at any point,
		 * its actual directory data is stored in the directory inode in the hidden
		 * directory rather than the leaf element(s) present in the namespace.
		 * 
		 * If there are still other hardlinks to this directory, 
		 * then we'll just eliminate this particular link and the vnode will still exist.
		 * If this is the last link to an empty directory, then we'll open-unlink the 
		 * directory and it will be only tagged with C_DELETED (as opposed to C_NOEXISTS).
		 * 
		 * We could also return EBUSY here. 
		 */
		
		return hfs_unlink(hfsmp, dvp, vp, cnp, skip_reserve);
	}
	
	/*
	 * In a few cases, we may want to allow the directory to persist in an
	 * open-unlinked state.  If the directory is being open-unlinked (still has usecount
	 * references), or if it has EAs, or if it was being deleted as part of a rename, 
	 * then we go ahead and move it to the hidden directory. 
	 *
	 * If the directory is being open-unlinked, then we want to keep the catalog entry 
	 * alive so that future EA calls and fchmod/fstat etc. do not cause issues later.
	 * 
	 * If the directory had EAs, then we want to use the open-unlink trick so that the 
	 * EA removal is not done in one giant transaction.  Otherwise, it could cause a panic
	 * due to overflowing the journal.
	 * 
	 * Finally, if it was deleted as part of a rename, we move it to the hidden directory
	 * in order to maintain rename atomicity.  
	 * 
	 * Note that the allow_dirs argument to hfs_removefile specifies that it is
	 * supposed to handle directories for this case.
     */
		
	if (((hfsmp->hfs_attribute_vp != NULL) &&
	    ((cp->c_attr.ca_recflags & kHFSHasAttributesMask) != 0)) ||
		(only_unlink != 0)) {
		
		int ret = hfs_removefile(dvp, vp, cnp, 0, 0, 1, NULL, only_unlink);
		/* 
		 * Even though hfs_vnop_rename calls vnode_recycle for us on tvp we call 
		 * it here just in case we were invoked by rmdir() on a directory that had 
		 * EAs.  To ensure that we start reclaiming the space as soon as possible,
		 * we call vnode_recycle on the directory.
		 */
		vnode_recycle(vp);
		
		return ret;
		
	}

	dcp->c_flag |= C_DIR_MODIFICATION;

#if QUOTA
	if (hfsmp->hfs_flags & HFS_QUOTAS)
		(void)hfs_getinoquota(cp);
#endif
	if ((error = hfs_start_transaction(hfsmp)) != 0) {
	    goto out;
	}
	started_tr = 1;
=======

#if QUOTA
	(void)hfs_getinoquota(cp);
#endif

	// XXXdbg
	hfs_global_shared_lock_acquire(hfsmp);
	grabbed_lock = 1;
	if (hfsmp->jnl) {
	    if ((error = journal_start_transaction(hfsmp->jnl)) != 0) {
			goto out;
	    }
		started_tr = 1;
	}
>>>>>>> origin/10.2

	/*
	 * Verify the directory is empty (and valid).
	 * (Rmdir ".." won't be valid since
	 *  ".." will contain a reference to
	 *  the current directory and thus be
	 *  non-empty.)
	 */
	if ((dcp->c_bsdflags & APPEND) || (cp->c_bsdflags & (IMMUTABLE | APPEND))) {
		error = EPERM;
		goto out;
	}

	/* Remove the entry from the namei cache: */
	cache_purge(vp);

	/* 
	 * Protect against a race with rename by using the component
	 * name passed in and parent id from dvp (instead of using 
	 * the cp->c_desc which may have changed).
	 */
	desc.cd_nameptr = (const u_int8_t *)cnp->cn_nameptr;
	desc.cd_namelen = cnp->cn_namelen;
	desc.cd_parentcnid = dcp->c_fileid;
	desc.cd_cnid = cp->c_cnid;
	desc.cd_flags = CD_ISDIR;
	desc.cd_encoding = cp->c_encoding;
	desc.cd_hint = 0;

	if (!hfs_valid_cnode(hfsmp, dvp, cnp, cp->c_fileid, NULL, &error)) {
	    error = 0;
	    goto out;
	}

	/* Remove entry from catalog */
	lockflags = hfs_systemfile_lock(hfsmp, SFL_CATALOG | SFL_ATTRIBUTE | SFL_BITMAP, HFS_EXCLUSIVE_LOCK);

	if (!skip_reserve) {
		/*
		 * Reserve some space in the Catalog file.
		 */
		if ((error = cat_preflight(hfsmp, CAT_DELETE, NULL, 0))) {
			hfs_systemfile_unlock(hfsmp, lockflags);
			goto out;
		}
	}

	error = cat_delete(hfsmp, &desc, &cp->c_attr);

	if (!error) {
		//
		// if skip_reserve == 1 then we're being called from hfs_vnop_rename() and thus
		// we don't need to touch the document_id as it's handled by the rename code.
		// otherwise it's a normal remove and we need to save the document id in the
		// per thread struct and clear it from the cnode.
		//
		struct  doc_tombstone *ut;
		ut = get_uthread_doc_tombstone();
		if (!skip_reserve && (cp->c_bsdflags & UF_TRACKED) && should_save_docid_tombstone(ut, vp, cnp)) {
		
			if (ut->t_lastop_document_id) {
				clear_tombstone_docid(ut, hfsmp, NULL);
			}
			save_tombstone(hfsmp, dvp, vp, cnp, 1);

		}

		/* The parent lost a child */
		if (dcp->c_entries > 0)
			dcp->c_entries--;
		DEC_FOLDERCOUNT(hfsmp, dcp->c_attr);
		dcp->c_dirchangecnt++;
<<<<<<< HEAD
		hfs_incr_gencount(dcp);

=======
		{
			struct FndrExtendedDirInfo *extinfo = (struct FndrExtendedDirInfo *)((u_int8_t*)dcp->c_finderinfo + 16);
			extinfo->write_gen_counter = OSSwapHostToBigInt32(OSSwapBigToHostInt32(extinfo->write_gen_counter) + 1);
		}
>>>>>>> origin/10.9
		dcp->c_touch_chgtime = TRUE;
		dcp->c_touch_modtime = TRUE;
		dcp->c_flag |= C_MODIFIED;

		hfs_update(dcp->c_vp, 0);
	}

	hfs_systemfile_unlock(hfsmp, lockflags);

	if (error)
		goto out;

#if QUOTA
<<<<<<< HEAD
	if (hfsmp->hfs_flags & HFS_QUOTAS)
		(void)hfs_chkiq(cp, -1, NOCRED, 0);
#endif /* QUOTA */

=======
	(void)hfs_chkiq(cp, -1, NOCRED, 0);
#endif /* QUOTA */

	/* The parent lost a child */
	if (dcp->c_entries > 0)
		dcp->c_entries--;
	if (dcp->c_nlink > 0)
		dcp->c_nlink--;
	dcp->c_flag |= C_CHANGE | C_UPDATE;
	tv = time;
	(void) VOP_UPDATE(dvp, &tv, &tv, 0);

>>>>>>> origin/10.2
	hfs_volupdate(hfsmp, VOL_RMDIR, (dcp->c_cnid == kHFSRootFolderID));

	/* Mark C_NOEXISTS since the catalog entry is now gone */
	cp->c_flag |= C_NOEXISTS;

out:
<<<<<<< HEAD
	dcp->c_flag &= ~C_DIR_MODIFICATION;
	wakeup((caddr_t)&dcp->c_flag);
=======
	if (dvp) 
		vput(dvp);
	vput(vp);

	// XXXdbg
	if (started_tr) { 
	    journal_end_transaction(hfsmp->jnl);
	}
	if (grabbed_lock) {
		hfs_global_shared_lock_release(hfsmp);
	}

	return (error);
}
>>>>>>> origin/10.2

	if (started_tr) { 
	    hfs_end_transaction(hfsmp);
	}

	return (error);
}


/*
 * Remove a file or link.
 */
int
hfs_vnop_remove(ap)
	struct vnop_remove_args /* {
		struct vnode *a_dvp;
		struct vnode *a_vp;
		struct componentname *a_cnp;
		int a_flags;
		vfs_context_t a_context;
	} */ *ap;
{
	struct vnode *dvp = ap->a_dvp;
	struct vnode *vp = ap->a_vp;
	struct cnode *dcp = VTOC(dvp);
	struct cnode *cp;
	struct vnode *rvp = NULL;
	int error=0, recycle_rsrc=0;
	int recycle_vnode = 0;
	uint32_t rsrc_vid = 0;
	time_t orig_ctime;

	if (dvp == vp) {
		return (EINVAL);
	}

	orig_ctime = VTOC(vp)->c_ctime;
	if (!vnode_isnamedstream(vp) && ((ap->a_flags & VNODE_REMOVE_SKIP_NAMESPACE_EVENT) == 0)) {
		error = check_for_tracked_file(vp, orig_ctime, NAMESPACE_HANDLER_DELETE_OP, NULL);
		if (error) {
			// XXXdbg - decide on a policy for handling namespace handler failures!
			// for now we just let them proceed.
		}		
	}
	error = 0;

<<<<<<< HEAD
	cp = VTOC(vp);

<<<<<<< HEAD
relock:

	hfs_lock_truncate(cp, HFS_EXCLUSIVE_LOCK, HFS_LOCK_DEFAULT);
=======
	if ((error = hfs_lockpair(dcp, cp, HFS_EXCLUSIVE_LOCK))) {
		hfs_unlock_truncate(cp, TRUE);
		return (error);
=======
		if ((error = hfs_lock (cp, HFS_EXCLUSIVE_LOCK))) {
			return (error);
		}
		error = hfs_vgetrsrc(hfsmp, vp, &rvp, TRUE, TRUE);
		hfs_unlock(cp);
		if (error) {
			/* We may have gotten a rsrc vp out even though we got an error back. */
			if (rvp) {
				vnode_put(rvp);
				rvp = NULL;
			}
			return error;
		}
		drop_rsrc_vnode = 1;
>>>>>>> origin/10.6
	}
	error = hfs_removefile(dvp, vp, ap->a_cnp, ap->a_flags, 0, 0);
>>>>>>> origin/10.5

	if ((error = hfs_lockpair(dcp, cp, HFS_EXCLUSIVE_LOCK))) {
		hfs_unlock_truncate(cp, HFS_LOCK_DEFAULT);
		if (rvp) {
			vnode_put (rvp);
		}	
		return (error);
	}
	//
	// if the item is tracked but doesn't have a document_id, assign one and generate an fsevent for it
	//
	if ((cp->c_bsdflags & UF_TRACKED) && ((struct FndrExtendedDirInfo *)((char *)&cp->c_attr.ca_finderinfo + 16))->document_id == 0) {
		uint32_t newid;

		hfs_unlockpair(dcp, cp);

		if (hfs_generate_document_id(VTOHFS(vp), &newid) == 0) {
			hfs_lockpair(dcp, cp, HFS_EXCLUSIVE_LOCK);
			((struct FndrExtendedDirInfo *)((char *)&cp->c_attr.ca_finderinfo + 16))->document_id = newid;
#if CONFIG_FSE
			add_fsevent(FSE_DOCID_CHANGED, vfs_context_current(),
				    FSE_ARG_DEV,   VTOHFS(vp)->hfs_raw_dev,
				    FSE_ARG_INO,   (ino64_t)0,             // src inode #
				    FSE_ARG_INO,   (ino64_t)cp->c_fileid,  // dst inode #
				    FSE_ARG_INT32, newid,
				    FSE_ARG_DONE);
#endif
		} else {
			// XXXdbg - couldn't get a new docid... what to do?  can't really fail the rm...
			hfs_lockpair(dcp, cp, HFS_EXCLUSIVE_LOCK);
		}
	}
	
	/*
	 * Lazily respond to determining if there is a valid resource fork
	 * vnode attached to 'cp' if it is a regular file or symlink.  
	 * If the vnode does not exist, then we may proceed without having to
	 * create it.
	 *
	 * If, however, it does exist, then we need to acquire an iocount on the
	 * vnode after acquiring its vid.  This ensures that if we have to do I/O
	 * against it, it can't get recycled from underneath us in the middle
	 * of this call.
	 *
	 * Note: this function may be invoked for directory hardlinks, so just skip these
	 * steps if 'vp' is a directory.
	 */

	if ((vp->v_type == VLNK) || (vp->v_type == VREG)) {
		if ((cp->c_rsrc_vp) && (rvp == NULL)) {
			/* We need to acquire the rsrc vnode */
			rvp = cp->c_rsrc_vp;
			rsrc_vid = vnode_vid (rvp);

			/* Unlock everything to acquire iocount on the rsrc vnode */	
			hfs_unlock_truncate (cp, HFS_LOCK_DEFAULT);
			hfs_unlockpair (dcp, cp);
			/* Use the vid to maintain identity on rvp */
			if (vnode_getwithvid(rvp, rsrc_vid)) {
				/*
				 * If this fails, then it was recycled or 
				 * reclaimed in the interim.  Reset fields and
				 * start over.
				 */
				rvp = NULL;
				rsrc_vid = 0;
			}
			goto relock;
		}
	}

	/* 
	 * Check to see if we raced rmdir for the parent directory 
	 * hfs_removefile already checks for a race on vp/cp
	 */
	if (dcp->c_flag & (C_DELETED | C_NOEXISTS)) {
		error = ENOENT;
		goto rm_done;	
	}

	error = hfs_removefile(dvp, vp, ap->a_cnp, ap->a_flags, 0, 0, NULL, 0);
	
	/*
	 * If the remove succeeded in deleting the file, then we may need to mark
	 * the resource fork for recycle so that it is reclaimed as quickly
	 * as possible.  If it were not recycled quickly, then this resource fork
	 * vnode could keep a v_parent reference on the data fork, which prevents it
	 * from going through reclaim (by giving it extra usecounts), except in the force-
	 * unmount case.  
	 * 
	 * However, a caveat:  we need to continue to supply resource fork
	 * access to open-unlinked files even if the resource fork is not open.  This is
	 * a requirement for the compressed files work.  Luckily, hfs_vgetrsrc will handle
	 * this already if the data fork has been re-parented to the hidden directory.
	 * 
	 * As a result, all we really need to do here is mark the resource fork vnode
	 * for recycle.  If it goes out of core, it can be brought in again if needed.  
	 * If the cnode was instead marked C_NOEXISTS, then there wouldn't be any 
	 * more work.
	 */
	if (error == 0) {
		hfs_hotfile_deleted(vp);
		
	   	if (rvp) {
	    	recycle_rsrc = 1;
		}
		/* 
		 * If the target was actually removed from the catalog schedule it for
		 * full reclamation/inactivation.  We hold an iocount on it so it should just
		 * get marked with MARKTERM
		 */
		if (cp->c_flag & C_NOEXISTS) {
			recycle_vnode = 1;
		}
	}


	/*
	 * Drop the truncate lock before unlocking the cnode
	 * (which can potentially perform a vnode_put and
	 * recycle the vnode which in turn might require the
	 * truncate lock)
	 */
<<<<<<< HEAD
rm_done:
	hfs_unlockpair(dcp, cp);
	hfs_unlock_truncate(cp, HFS_LOCK_DEFAULT);

	if (recycle_rsrc) {
		/* inactive or reclaim on rvp will clean up the blocks from the rsrc fork */
=======
	hfs_unlock_truncate(cp, TRUE);
	hfs_unlockpair(dcp, cp);

	if (recycle_rsrc && vnode_getwithvid(rvp, rvid) == 0) {
		vnode_ref(rvp);
		vnode_rele(rvp);
>>>>>>> origin/10.5
		vnode_recycle(rvp);
	} 
	if (recycle_vnode) {
		vnode_recycle (vp);
	}

	if (rvp) {
		/* drop iocount on rsrc fork, was obtained at beginning of fxn */
		vnode_put(rvp);
	}

	return (error);
}


int
hfs_removefile_callback(struct buf *bp, void *hfsmp) {

        if ( !(buf_flags(bp) & B_META))
	        panic("hfs: symlink bp @ %p is not marked meta-data!\n", bp);
	/*
	 * it's part of the current transaction, kill it.
	 */
	journal_kill_block(((struct hfsmount *)hfsmp)->jnl, bp);

	return (BUF_CLAIMED);
}

/*
 * hfs_removefile
 *
 * Similar to hfs_vnop_remove except there are additional options.
 * This function may be used to remove directories if they have
 * lots of EA's -- note the 'allow_dirs' argument.
 *
 * This function is able to delete blocks & fork data for the resource
 * fork even if it does not exist in core (and have a backing vnode).  
 * It should infer the correct behavior based on the number of blocks
 * in the cnode and whether or not the resource fork pointer exists or 
 * not.  As a result, one only need pass in the 'vp' corresponding to the
 * data fork of this file (or main vnode in the case of a directory).  
 * Passing in a resource fork will result in an error.
 *
 * Because we do not create any vnodes in this function, we are not at 
 * risk of deadlocking against ourselves by double-locking.
 *
 * Requires cnode and truncate locks to be held.
 */
int
hfs_removefile(struct vnode *dvp, struct vnode *vp, struct componentname *cnp,
               int flags, int skip_reserve, int allow_dirs, 
			   __unused struct vnode *rvp, int only_unlink)
{
	struct cnode *cp;
	struct cnode *dcp;
	struct vnode *rsrc_vp = NULL;
	struct hfsmount *hfsmp;
	struct cat_desc desc;
	struct timeval tv;
	int dataforkbusy = 0;
	int rsrcforkbusy = 0;
	int lockflags;
	int error = 0;
<<<<<<< HEAD
	int started_tr = 0;
	int isbigfile = 0, defer_remove=0, isdir=0;
<<<<<<< HEAD
	int update_vh = 0;
=======
	int started_tr = 0, grabbed_lock = 0;

	/* Redirect directories to rmdir */
	if (vp->v_type == VDIR)
		return (hfs_rmdir(ap));
>>>>>>> origin/10.2
=======
>>>>>>> origin/10.5

	cp = VTOC(vp);
	dcp = VTOC(dvp);
	hfsmp = VTOHFS(vp);

	/* Check if we lost a race post lookup. */
	if (cp->c_flag & (C_NOEXISTS | C_DELETED)) {
		return (0);
	}

	if (!hfs_valid_cnode(hfsmp, dvp, cnp, cp->c_fileid, NULL, &error)) {
	    return 0;
	}

	/* Make sure a remove is permitted */
<<<<<<< HEAD
	if (VNODE_IS_RSRC(vp)) {
		return (EPERM);
	}
	else {
		/* 
		 * We know it's a data fork.
		 * Probe the cnode to see if we have a valid resource fork
		 * in hand or not.
		 */
		rsrc_vp = cp->c_rsrc_vp;
	}

	/* Don't allow deleting the journal or journal_info_block. */
	if (hfs_is_journal_file(hfsmp, cp)) {
		return (EPERM);
=======
	if ((cp->c_flags & (IMMUTABLE | APPEND)) ||
	    (VTOC(dvp)->c_flags & APPEND) ||
	    VNODE_IS_RSRC(vp)) {
		error = EPERM;
		goto out;
>>>>>>> origin/10.2
	}

	/*
	 * Hard links require special handling.
	 */
	if (cp->c_flag & C_HARDLINK) {
		if ((flags & VNODE_REMOVE_NODELETEBUSY) && vnode_isinuse(vp, 0)) {
			return (EBUSY);
		} else {
			/* A directory hard link with a link count of one is 
			 * treated as a regular directory.  Therefore it should 
			 * only be removed using rmdir().
			 */
			if ((vnode_isdir(vp) == 1) && (cp->c_linkcount == 1) && 
			    (allow_dirs == 0)) {
			    	return (EPERM);
			}
			return hfs_unlink(hfsmp, dvp, vp, cnp, skip_reserve);
		}
	}

	/* Directories should call hfs_rmdir! (unless they have a lot of attributes) */
	if (vnode_isdir(vp)) {
		if (allow_dirs == 0)
			return (EPERM);  /* POSIX */
		isdir = 1;
	}
	/* Sanity check the parent ids. */
	if ((cp->c_parentcnid != hfsmp->hfs_private_desc[FILE_HARDLINKS].cd_cnid) &&
	    (cp->c_parentcnid != dcp->c_fileid)) {
		return (EINVAL);
	}

	dcp->c_flag |= C_DIR_MODIFICATION;

	// this guy is going away so mark him as such
	cp->c_flag |= C_DELETED;

<<<<<<< HEAD

	/* Remove our entry from the namei cache. */
	cache_purge(vp);
	
=======
	// XXXdbg - don't allow deleting the journal or journal_info_block
	if (hfsmp->jnl && cp->c_datafork) {
		struct HFSPlusExtentDescriptor *extd;

<<<<<<< HEAD
		extd = &cp->c_datafork->ff_data.cf_extents[0];
		if (extd->startBlock == HFSTOVCB(hfsmp)->vcbJinfoBlock || extd->startBlock == hfsmp->jnl_start) {
			error = EPERM;
			goto out;
=======
	/*
	 * Acquire a vnode for a non-empty resource fork.
	 * (needed for hfs_truncate)
	 */
	if (isdir == 0 && (cp->c_blocks - VTOF(vp)->ff_blocks)) {
		/*
		 * We must avoid calling hfs_vgetrsrc() when we have
		 * an active resource fork vnode to avoid deadlocks
		 * when that vnode is in the VL_TERMINATE state. We
		 * can defer removing the file and its resource fork
		 * until the call to hfs_vnop_inactive() occurs.
		 */
		if (cp->c_rsrc_vp) {
			defer_remove = 1;
		} else {
			error = hfs_vgetrsrc(hfsmp, vp, &rvp, FALSE);
			if (error)
				goto out;
			/* Defer the vnode_put on rvp until the hfs_unlock(). */
			cp->c_flag |= C_NEED_RVNODE_PUT;
>>>>>>> origin/10.5
		}
	}

>>>>>>> origin/10.2
	/*
	 * If the caller was operating on a file (as opposed to a 
	 * directory with EAs), then we need to figure out
	 * whether or not it has a valid resource fork vnode.
	 * 
	 * If there was a valid resource fork vnode, then we need
	 * to use hfs_truncate to eliminate its data.  If there is
	 * no vnode, then we hold the cnode lock which would
	 * prevent it from being created.  As a result, 
	 * we can use the data deletion functions which do not
	 * require that a cnode/vnode pair exist.
	 */

	/* Check if this file is being used. */
	if (isdir == 0) {
		dataforkbusy = vnode_isinuse(vp, 0);
		/*  
		 * At this point, we know that 'vp' points to the 
		 * a data fork because we checked it up front. And if 
		 * there is no rsrc fork, rsrc_vp will be NULL.
		 */
		if (rsrc_vp && (cp->c_blocks - VTOF(vp)->ff_blocks)) {
			rsrcforkbusy = vnode_isinuse(rsrc_vp, 0);
		}
	}
	
	/* Check if we have to break the deletion into multiple pieces. */
	if (isdir == 0)
		isbigfile = cp->c_datafork->ff_size >= HFS_BIGFILE_SIZE;

	/* Check if the file has xattrs.  If it does we'll have to delete them in
	   individual transactions in case there are too many */
	if ((hfsmp->hfs_attribute_vp != NULL) &&
	    (cp->c_attr.ca_recflags & kHFSHasAttributesMask) != 0) {
	    defer_remove = 1;
<<<<<<< HEAD
	}
	
	/* If we are explicitly told to only unlink item and move to hidden dir, then do it */
	if (only_unlink) {
		defer_remove = 1;
=======
>>>>>>> origin/10.5
	}

	/*
	 * Carbon semantics prohibit deleting busy files.
	 * (enforced when VNODE_REMOVE_NODELETEBUSY is requested)
	 */
	if (dataforkbusy || rsrcforkbusy) {
		if ((flags & VNODE_REMOVE_NODELETEBUSY) ||
		    (hfsmp->hfs_private_desc[FILE_HARDLINKS].cd_cnid == 0)) {
			error = EBUSY;
			goto out;
		}
	}

#if QUOTA
<<<<<<< HEAD
	if (hfsmp->hfs_flags & HFS_QUOTAS)
		(void)hfs_getinoquota(cp);
#endif /* QUOTA */
	
	/* 
	 * Do a ubc_setsize to indicate we need to wipe contents if:
	 *  1) item is a regular file.
	 *  2) Neither fork is busy AND we are not told to unlink this. 
	 *
	 * We need to check for the defer_remove since it can be set without 
	 * having a busy data or rsrc fork   
	 */
	if (isdir == 0 && (!dataforkbusy || !rsrcforkbusy) && (defer_remove == 0)) {
		/*
		 * A ubc_setsize can cause a pagein so defer it
		 * until after the cnode lock is dropped.  The
		 * cnode lock cannot be dropped/reacquired here
		 * since we might already hold the journal lock.
		 */
		if (!dataforkbusy && cp->c_datafork->ff_blocks && !isbigfile) {
			cp->c_flag |= C_NEED_DATA_SETSIZE;
		}
		if (!rsrcforkbusy && rsrc_vp) {
			cp->c_flag |= C_NEED_RSRC_SETSIZE;
		}
	}

	if ((error = hfs_start_transaction(hfsmp)) != 0) {
	    goto out;
	}
	started_tr = 1;

	// XXXdbg - if we're journaled, kill any dirty symlink buffers 
	if (hfsmp->jnl && vnode_islnk(vp) && (defer_remove == 0)) {
	        buf_iterate(vp, hfs_removefile_callback, BUF_SKIP_NONLOCKED, (void *)hfsmp);
	}
=======
	(void)hfs_getinoquota(cp);
#endif /* QUOTA */

	// XXXdbg
	hfs_global_shared_lock_acquire(hfsmp);
	grabbed_lock = 1;
	if (hfsmp->jnl) {
	    if ((error = journal_start_transaction(hfsmp->jnl)) != 0) {
			goto out;
	    }
	    started_tr = 1;
	}

	/* Remove our entry from the namei cache. */
	cache_purge(vp);
>>>>>>> origin/10.2

	// XXXdbg - if we're journaled, kill any dirty symlink buffers 
	if (hfsmp->jnl && vp->v_type == VLNK && vp->v_dirtyblkhd.lh_first) {
	    struct buf *bp, *nbp;

	  recheck:
	    for (bp=vp->v_dirtyblkhd.lh_first; bp; bp=nbp) {
			nbp = bp->b_vnbufs.le_next;
			
			if ((bp->b_flags & B_BUSY)) {
				// if it was busy, someone else must be dealing
				// with it so just move on.
				continue;
			}

			if (!(bp->b_flags & B_META)) {
				panic("hfs: symlink bp @ 0x%x is not marked meta-data!\n", bp);
			}

			// if it's part of the current transaction, kill it.
			if (bp->b_flags & B_LOCKED) {
				bremfree(bp);
				bp->b_flags |= B_BUSY;
				journal_kill_block(hfsmp->jnl, bp);
				goto recheck;
			}
	    }
	}
	// XXXdbg

	/*
	 * Prepare to truncate any non-busy forks.  Busy forks will
	 * get truncated when their vnode goes inactive.
	 * Note that we will only enter this region if we
	 * can avoid creating an open-unlinked file.  If 
	 * either region is busy, we will have to create an open
	 * unlinked file.
	 *
	 * Since we are deleting the file, we need to stagger the runtime
	 * modifications to do things in such a way that a crash won't 
	 * result in us getting overlapped extents or any other 
	 * bad inconsistencies.  As such, we call prepare_release_storage
	 * which updates the UBC, updates quota information, and releases
	 * any loaned blocks that belong to this file.  No actual 
	 * truncation or bitmap manipulation is done until *AFTER*
	 * the catalog record is removed. 
	 */
	if (isdir == 0 && (!dataforkbusy && !rsrcforkbusy) && (only_unlink == 0)) {
		
		if (!dataforkbusy && !isbigfile && cp->c_datafork->ff_blocks != 0) {
			
			error = hfs_prepare_release_storage (hfsmp, vp);
			if (error) {
				goto out;
			}
			update_vh = 1;
		}
<<<<<<< HEAD
		
		/*
		 * If the resource fork vnode does not exist, we can skip this step.
		 */
		if (!rsrcforkbusy && rsrc_vp) {
			error = hfs_prepare_release_storage (hfsmp, rsrc_vp);
			if (error) {
				goto out;
			}
			update_vh = 1;
=======
		if (!rsrcforkbusy && rvp) {
			cp->c_mode = 0;            /* Suppress VOP_UPDATES */
			error = VOP_TRUNCATE(rvp, (off_t)0, IO_NDELAY, NOCRED, p);
			cp->c_mode = mode;
			if (error)
				goto out;
			truncated = 1;
>>>>>>> origin/10.2
		}
	}
	
	/* 
	 * Protect against a race with rename by using the component
	 * name passed in and parent id from dvp (instead of using 
	 * the cp->c_desc which may have changed).   Also, be aware that
	 * because we allow directories to be passed in, we need to special case
	 * this temporary descriptor in case we were handed a directory.
	 */
	if (isdir) {
		desc.cd_flags = CD_ISDIR;
	}
	else {
		desc.cd_flags = 0;
	}
	desc.cd_encoding = cp->c_desc.cd_encoding;
	desc.cd_nameptr = (const u_int8_t *)cnp->cn_nameptr;
	desc.cd_namelen = cnp->cn_namelen;
	desc.cd_parentcnid = dcp->c_fileid;
	desc.cd_hint = cp->c_desc.cd_hint;
	desc.cd_cnid = cp->c_cnid;
	microtime(&tv);

	/*
	 * There are two cases to consider:
<<<<<<< HEAD
	 *  1. File/Dir is busy/big/defer_remove ==> move/rename the file/dir
=======
	 *  1. File is busy/big/defer_remove ==> move/rename the file
>>>>>>> origin/10.5
	 *  2. File is not in use ==> remove the file
	 * 
	 * We can get a directory in case 1 because it may have had lots of attributes,
	 * which need to get removed here.
	 */
<<<<<<< HEAD
<<<<<<< HEAD
	if (dataforkbusy || rsrcforkbusy || isbigfile || defer_remove) {
=======

	if (cp->c_flag & C_HARDLINK) {
		struct cat_desc desc;

		if ((ap->a_cnp->cn_flags & HASBUF) == 0 ||
		    ap->a_cnp->cn_nameptr[0] == '\0') {
			error = ENOENT;	/* name missing! */
			goto out;
		}

		/* Setup a descriptor for the link */
		bzero(&desc, sizeof(desc));
		desc.cd_nameptr = ap->a_cnp->cn_nameptr;
		desc.cd_namelen = ap->a_cnp->cn_namelen;
		desc.cd_parentcnid = dcp->c_cnid;
		/* XXX - if cnid is out of sync then the wrong thread rec will get deleted. */
		desc.cd_cnid = cp->c_cnid;

		/* Lock catalog b-tree */
		error = hfs_metafilelocking(hfsmp, kHFSCatalogFileID, LK_EXCLUSIVE, p);
		if (error)
			goto out;

		/* Delete the link record */
		error = cat_delete(hfsmp, &desc, &cp->c_attr);

		if ((error == 0) && (--cp->c_nlink < 1)) {
			char inodename[32];
			char delname[32];
			struct cat_desc to_desc;
			struct cat_desc from_desc;

			/*
			 * This is now esentially an open deleted file.
			 * Rename it to reflect this state which makes
			 * orphan file cleanup easier (see hfs_remove_orphans).
			 * Note: a rename failure here is not fatal.
			 */	
			MAKE_INODE_NAME(inodename, cp->c_rdev);
			bzero(&from_desc, sizeof(from_desc));
			from_desc.cd_nameptr = inodename;
			from_desc.cd_namelen = strlen(inodename);
			from_desc.cd_parentcnid = hfsmp->hfs_private_metadata_dir;
			from_desc.cd_flags = 0;
			from_desc.cd_cnid = cp->c_fileid;

			MAKE_DELETED_NAME(delname, cp->c_fileid);		
			bzero(&to_desc, sizeof(to_desc));
			to_desc.cd_nameptr = delname;
			to_desc.cd_namelen = strlen(delname);
			to_desc.cd_parentcnid = hfsmp->hfs_private_metadata_dir;
			to_desc.cd_flags = 0;
			to_desc.cd_cnid = cp->c_fileid;
	
			(void) cat_rename(hfsmp, &from_desc, &hfsmp->hfs_privdir_desc,
			                  &to_desc, (struct cat_desc *)NULL);
			cp->c_flag |= C_DELETED;
		}

		/* Unlock the Catalog */
		(void) hfs_metafilelocking(hfsmp, kHFSCatalogFileID, LK_RELEASE, p);

		/* All done with component name... */
		if ((ap->a_cnp->cn_flags & (HASBUF | SAVENAME)) == (HASBUF | SAVENAME))
			FREE_ZONE(ap->a_cnp->cn_pnbuf, ap->a_cnp->cn_pnlen, M_NAMEI);

		if (error != 0)
			goto out;

		cp->c_flag |= C_CHANGE;
		tv = time;
		(void) VOP_UPDATE(vp, &tv, &tv, 0);

		hfs_volupdate(hfsmp, VOL_RMFILE, (dcp->c_cnid == kHFSRootFolderID));

	} else if (dataforkbusy || rsrcforkbusy) {
>>>>>>> origin/10.2
=======
	if (dataforkbusy || rsrcforkbusy || isbigfile || defer_remove) {
>>>>>>> origin/10.5
		char delname[32];
		struct cat_desc to_desc;
		struct cat_desc todir_desc;

		/*
		 * Orphan this file or directory (move to hidden directory).
		 * Again, we need to take care that we treat directories as directories,
		 * and files as files.  Because directories with attributes can be passed in
		 * check to make sure that we have a directory or a file before filling in the 
		 * temporary descriptor's flags.  We keep orphaned directories AND files in
		 * the FILE_HARDLINKS private directory since we're generalizing over all
		 * orphaned filesystem objects.
		 */
		bzero(&todir_desc, sizeof(todir_desc));
		todir_desc.cd_parentcnid = 2;

		MAKE_DELETED_NAME(delname, sizeof(delname), cp->c_fileid);
		bzero(&to_desc, sizeof(to_desc));
		to_desc.cd_nameptr = (const u_int8_t *)delname;
		to_desc.cd_namelen = strlen(delname);
		to_desc.cd_parentcnid = hfsmp->hfs_private_desc[FILE_HARDLINKS].cd_cnid;
		if (isdir) {
			to_desc.cd_flags = CD_ISDIR;
		}
		else {
			to_desc.cd_flags = 0;
		}
		to_desc.cd_cnid = cp->c_cnid;

<<<<<<< HEAD
		lockflags = hfs_systemfile_lock(hfsmp, SFL_CATALOG, HFS_EXCLUSIVE_LOCK);
		if (!skip_reserve) {
			if ((error = cat_preflight(hfsmp, CAT_RENAME, NULL, 0))) {
				hfs_systemfile_unlock(hfsmp, lockflags);
				goto out;
			}
		}
=======
		/* Lock catalog b-tree */
		error = hfs_metafilelocking(hfsmp, kHFSCatalogFileID, LK_EXCLUSIVE, p);
		if (error)
			goto out;
>>>>>>> origin/10.2

		error = cat_rename(hfsmp, &desc, &todir_desc,
				&to_desc, (struct cat_desc *)NULL);

<<<<<<< HEAD
		if (error == 0) {
			hfsmp->hfs_private_attr[FILE_HARDLINKS].ca_entries++;
			if (isdir == 1) {
				INC_FOLDERCOUNT(hfsmp, hfsmp->hfs_private_attr[FILE_HARDLINKS]);
			}
			(void) cat_update(hfsmp, &hfsmp->hfs_private_desc[FILE_HARDLINKS],
			                  &hfsmp->hfs_private_attr[FILE_HARDLINKS], NULL, NULL);

			/* Update the parent directory */
			if (dcp->c_entries > 0)
				dcp->c_entries--;
			if (isdir == 1) {
				DEC_FOLDERCOUNT(hfsmp, dcp->c_attr);
			}
			dcp->c_dirchangecnt++;
<<<<<<< HEAD
			hfs_incr_gencount(dcp);
=======
		// XXXdbg - only bump this count if we were successful
		if (error == 0) {
			hfsmp->hfs_privdir_attr.ca_entries++;
		}
		(void)cat_update(hfsmp, &hfsmp->hfs_privdir_desc,
				&hfsmp->hfs_privdir_attr, NULL, NULL);

		/* Unlock the Catalog */
		(void) hfs_metafilelocking(hfsmp, kHFSCatalogFileID, LK_RELEASE, p);
		if (error) goto out;

		cp->c_flag |= C_CHANGE | C_DELETED | C_NOEXISTS;
		--cp->c_nlink;
		tv = time;
		(void) VOP_UPDATE(vp, &tv, &tv, 0);
>>>>>>> origin/10.2

=======
			{
				struct FndrExtendedDirInfo *extinfo = (struct FndrExtendedDirInfo *)((u_int8_t*)dcp->c_finderinfo + 16);
				extinfo->write_gen_counter = OSSwapHostToBigInt32(OSSwapBigToHostInt32(extinfo->write_gen_counter) + 1);
			}
>>>>>>> origin/10.9
			dcp->c_ctime = tv.tv_sec;
			dcp->c_mtime = tv.tv_sec;
			(void) cat_update(hfsmp, &dcp->c_desc, &dcp->c_attr, NULL, NULL);

<<<<<<< HEAD
			/* Update the file or directory's state */
			cp->c_flag |= C_DELETED;
			cp->c_ctime = tv.tv_sec;
			--cp->c_linkcount;
			(void) cat_update(hfsmp, &to_desc, &cp->c_attr, NULL, NULL);
		}
		hfs_systemfile_unlock(hfsmp, lockflags);
		if (error)
			goto out;

	} 
	else {
		/*
		 * Nobody is using this item; we can safely remove everything.
		 */
		struct filefork *temp_rsrc_fork = NULL;
#if QUOTA
		off_t savedbytes;
		int blksize = hfsmp->blockSize;
#endif
		u_int32_t fileid = cp->c_fileid;
	
		/* 
		 * Figure out if we need to read the resource fork data into 
		 * core before wiping out the catalog record.  
		 *
		 * 1) Must not be a directory
		 * 2) cnode's c_rsrcfork ptr must be NULL.
		 * 3) rsrc fork must have actual blocks 
		 */
		if ((isdir == 0) && (cp->c_rsrcfork == NULL) && 
				(cp->c_blocks - VTOF(vp)->ff_blocks)) {
			/*
			 * The resource fork vnode & filefork did not exist.
			 * Create a temporary one for use in this function only. 
			 */
			MALLOC_ZONE (temp_rsrc_fork, struct filefork *, sizeof (struct filefork), M_HFSFORK, M_WAITOK);
			bzero(temp_rsrc_fork, sizeof(struct filefork));
			temp_rsrc_fork->ff_cp = cp;
			rl_init(&temp_rsrc_fork->ff_invalidranges);
		}	
=======
		if (cp->c_blocks > 0) {
			printf("hfs_remove: attempting to delete a non-empty file!");
			error = EBUSY;
			goto out;
		}

		/* Lock catalog b-tree */
		error = hfs_metafilelocking(hfsmp, kHFSCatalogFileID, LK_EXCLUSIVE, p);
		if (error)
			goto out;
>>>>>>> origin/10.2

		lockflags = hfs_systemfile_lock(hfsmp, SFL_CATALOG | SFL_ATTRIBUTE | SFL_BITMAP, HFS_EXCLUSIVE_LOCK);

<<<<<<< HEAD
		/* Look up the resource fork first, if necessary */
		if (temp_rsrc_fork) {
			error = cat_lookup (hfsmp, &desc, 1, 0, (struct cat_desc*) NULL, 
					(struct cat_attr*) NULL, &temp_rsrc_fork->ff_data, NULL);
			if (error) {
				FREE_ZONE (temp_rsrc_fork, sizeof(struct filefork), M_HFSFORK);
				hfs_systemfile_unlock (hfsmp, lockflags);
				goto out;
=======
		if (error && error != ENXIO && error != ENOENT && truncated) {
			if ((cp->c_datafork && cp->c_datafork->ff_data.cf_size != 0) ||
				(cp->c_rsrcfork && cp->c_rsrcfork->ff_data.cf_size != 0)) {
				panic("hfs: remove: couldn't delete a truncated file! (%d, data sz %lld; rsrc sz %lld)",
					  error, cp->c_datafork->ff_data.cf_size, cp->c_rsrcfork->ff_data.cf_size);
			} else {
				printf("hfs: remove: strangely enough, deleting truncated file %s (%d) got err %d\n",
					   cp->c_desc.cd_nameptr, cp->c_attr.ca_fileid, error);
>>>>>>> origin/10.2
			}
		}

		if (!skip_reserve) {
			if ((error = cat_preflight(hfsmp, CAT_DELETE, NULL, 0))) {
				if (temp_rsrc_fork) {
					FREE_ZONE (temp_rsrc_fork, sizeof(struct filefork), M_HFSFORK);
				}
				hfs_systemfile_unlock(hfsmp, lockflags);
				goto out;
			}
		}
		
		error = cat_delete(hfsmp, &desc, &cp->c_attr);
<<<<<<< HEAD
		
		if (error && error != ENXIO && error != ENOENT) {
			printf("hfs_removefile: deleting file %s (id=%d) vol=%s err=%d\n",
				   cp->c_desc.cd_nameptr, cp->c_attr.ca_fileid, hfsmp->vcbVN, error);
=======

		if (error && error != ENXIO && error != ENOENT && truncated) {
			if ((cp->c_datafork && cp->c_datafork->ff_size != 0) ||
					(cp->c_rsrcfork && cp->c_rsrcfork->ff_size != 0)) {
				off_t data_size = 0;
				off_t rsrc_size = 0;
				if (cp->c_datafork) {
					data_size = cp->c_datafork->ff_size;
				}
				if (cp->c_rsrcfork) {
					rsrc_size = cp->c_rsrcfork->ff_size;
				}
				printf("hfs: remove: couldn't delete a truncated file (%s)" 
						"(error %d, data sz %lld; rsrc sz %lld)",
					cp->c_desc.cd_nameptr, error, data_size, rsrc_size);
				hfs_mark_volume_inconsistent(hfsmp);
			} else {
				printf("hfs: remove: strangely enough, deleting truncated file %s (%d) got err %d\n",
						cp->c_desc.cd_nameptr, cp->c_attr.ca_fileid, error);
			}	
>>>>>>> origin/10.6
		}
		
		if (error == 0) {
			/* Update the parent directory */
			if (dcp->c_entries > 0)
				dcp->c_entries--;
			dcp->c_dirchangecnt++;
<<<<<<< HEAD
			hfs_incr_gencount(dcp);

=======
			{
				struct FndrExtendedDirInfo *extinfo = (struct FndrExtendedDirInfo *)((u_int8_t*)dcp->c_finderinfo + 16);
				extinfo->write_gen_counter = OSSwapHostToBigInt32(OSSwapBigToHostInt32(extinfo->write_gen_counter) + 1);
			}
>>>>>>> origin/10.9
			dcp->c_ctime = tv.tv_sec;
			dcp->c_mtime = tv.tv_sec;
			(void) cat_update(hfsmp, &dcp->c_desc, &dcp->c_attr, NULL, NULL);
		}
		hfs_systemfile_unlock(hfsmp, lockflags);

		if (error) {
			if (temp_rsrc_fork) {
				FREE_ZONE (temp_rsrc_fork, sizeof(struct filefork), M_HFSFORK);
			}
			goto out;
		}
		
		/* 
		 * Now that we've wiped out the catalog record, the file effectively doesn't
		 * exist anymore. So update the quota records to reflect the loss of the 
		 * data fork and the resource fork. 
		 */
#if QUOTA
<<<<<<< HEAD
		if (cp->c_datafork->ff_blocks > 0) {
			savedbytes = ((off_t)cp->c_datafork->ff_blocks * (off_t)blksize);
			(void) hfs_chkdq(cp, (int64_t)-(savedbytes), NOCRED, 0);
		}
		
		/*
		 * We may have just deleted the catalog record for a resource fork even 
		 * though it did not exist in core as a vnode. However, just because there 
		 * was a resource fork pointer in the cnode does not mean that it had any blocks.
		 */
		if (temp_rsrc_fork || cp->c_rsrcfork) {
			if (cp->c_rsrcfork) {
			   	if (cp->c_rsrcfork->ff_blocks > 0) {
					savedbytes = ((off_t)cp->c_rsrcfork->ff_blocks * (off_t)blksize);
					(void) hfs_chkdq(cp, (int64_t)-(savedbytes), NOCRED, 0);
				}
			}	
			else {
				/* we must have used a temporary fork */
				savedbytes = ((off_t)temp_rsrc_fork->ff_blocks * (off_t)blksize);	
				(void) hfs_chkdq(cp, (int64_t)-(savedbytes), NOCRED, 0);
			}
		}
		
		if (hfsmp->hfs_flags & HFS_QUOTAS) {
			(void)hfs_chkiq(cp, -1, NOCRED, 0);
		}
#endif
		
		/* 
		 * If we didn't get any errors deleting the catalog entry, then go ahead
		 * and release the backing store now.  The filefork pointers are still valid.
		 */
		if (temp_rsrc_fork) {	
			error = hfs_release_storage (hfsmp, cp->c_datafork, temp_rsrc_fork, fileid);
		}
		else {
			/* if cp->c_rsrcfork == NULL, hfs_release_storage will skip over it. */
			error = hfs_release_storage (hfsmp, cp->c_datafork, cp->c_rsrcfork, fileid);
		}
		if (error) {
			/* 
			 * If we encountered an error updating the extents and bitmap,
			 * mark the volume inconsistent.  At this point, the catalog record has
			 * already been deleted, so we can't recover it at this point. We need
			 * to proceed and update the volume header and mark the cnode C_NOEXISTS.
			 * The subsequent fsck should be able to recover the free space for us.
			 */
			hfs_mark_inconsistent(hfsmp, HFS_OP_INCOMPLETE);
		}
		else {
			/* reset update_vh to 0, since hfs_release_storage should have done it for us */
			update_vh = 0;
		}

		/* Get rid of the temporary rsrc fork */
		if (temp_rsrc_fork) {
			FREE_ZONE (temp_rsrc_fork, sizeof(struct filefork), M_HFSFORK);
		}
=======
		(void)hfs_chkiq(cp, -1, NOCRED, 0);
#endif /* QUOTA */
>>>>>>> origin/10.2

		cp->c_flag |= C_NOEXISTS;
		cp->c_flag &= ~C_DELETED;
		
		cp->c_touch_chgtime = TRUE;
		--cp->c_linkcount;
		
		/* 
		 * We must never get a directory if we're in this else block.  We could 
		 * accidentally drop the number of files in the volume header if we did.
		 */
		hfs_volupdate(hfsmp, VOL_RMFILE, (dcp->c_cnid == kHFSRootFolderID));
		
	}

	//
	// if skip_reserve == 1 then we're being called from hfs_vnop_rename() and thus
	// we don't need to touch the document_id as it's handled by the rename code.
	// otherwise it's a normal remove and we need to save the document id in the
	// per thread struct and clear it from the cnode.
	//
	struct  doc_tombstone *ut;
	ut = get_uthread_doc_tombstone();
	if (!error && !skip_reserve && (cp->c_bsdflags & UF_TRACKED) && should_save_docid_tombstone(ut, vp, cnp)) {

		if (ut->t_lastop_document_id) {
			clear_tombstone_docid(ut, hfsmp, NULL);
		}
		save_tombstone(hfsmp, dvp, vp, cnp, 1);

	}


	/*
	 * All done with this cnode's descriptor...
	 *
	 * Note: all future catalog calls for this cnode must be by
	 * fileid only.  This is OK for HFS (which doesn't have file
	 * thread records) since HFS doesn't support the removal of
	 * busy files.
	 */
	cat_releasedesc(&cp->c_desc);

<<<<<<< HEAD
=======
	/* In all three cases the parent lost a child */
	if (dcp->c_entries > 0)
		dcp->c_entries--;
	if (dcp->c_nlink > 0)
		dcp->c_nlink--;
	dcp->c_flag |= C_CHANGE | C_UPDATE;
	tv = time;
	(void) VOP_UPDATE(dvp, &tv, &tv, 0);

	// XXXdbg
	if (started_tr) {
	    journal_end_transaction(hfsmp->jnl);
	}
	if (grabbed_lock) {
		hfs_global_shared_lock_release(hfsmp);
	}

	if (rvp)
		vrele(rvp);
	VOP_UNLOCK(vp, 0, p);
	// XXXdbg - try to prevent the lost ubc_info panic
	if ((cp->c_flag & C_HARDLINK) == 0 || cp->c_nlink == 0) {
		(void) ubc_uncache(vp);
	}
	vrele(vp);
	vput(dvp);

	return (0);

>>>>>>> origin/10.2
out:
	if (error) {
	    cp->c_flag &= ~C_DELETED;
	}
	
<<<<<<< HEAD
	if (update_vh) {
		/* 
		 * If we bailed out earlier, we may need to update the volume header
		 * to deal with the borrowed blocks accounting. 
		 */
		hfs_volupdate (hfsmp, VOL_UPDATE, 0);
	}	

	if (started_tr) {
	    hfs_end_transaction(hfsmp);
	}

	dcp->c_flag &= ~C_DIR_MODIFICATION;
	wakeup((caddr_t)&dcp->c_flag);
=======
	/* Commit the truncation to the catalog record */
	if (truncated) {
		cp->c_flag |= C_CHANGE | C_UPDATE;
		tv = time;
		(void) VOP_UPDATE(vp, &tv, &tv, 0);
	}
	vput(vp);
	vput(dvp);

	// XXXdbg
	if (started_tr) {
	    journal_end_transaction(hfsmp->jnl);
	}
	if (grabbed_lock) {
		hfs_global_shared_lock_release(hfsmp);
	}
>>>>>>> origin/10.2

	return (error);
}


__private_extern__ void
replace_desc(struct cnode *cp, struct cat_desc *cdp)
{
	// fixes 4348457 and 4463138
	if (&cp->c_desc == cdp) {
	    return;
	}

	/* First release allocated name buffer */
	if (cp->c_desc.cd_flags & CD_HASBUF && cp->c_desc.cd_nameptr != 0) {
		const u_int8_t *name = cp->c_desc.cd_nameptr;

		cp->c_desc.cd_nameptr = 0;
		cp->c_desc.cd_namelen = 0;
		cp->c_desc.cd_flags &= ~CD_HASBUF;
		vfs_removename((const char *)name);
	}
	bcopy(cdp, &cp->c_desc, sizeof(cp->c_desc));

	/* Cnode now owns the name buffer */
	cdp->cd_nameptr = 0;
	cdp->cd_namelen = 0;
	cdp->cd_flags &= ~CD_HASBUF;
}


/*
 * Rename a cnode.
 *
 * The VFS layer guarantees that:
 *   - source and destination will either both be directories, or
 *     both not be directories.
 *   - all the vnodes are from the same file system
 *
<<<<<<< HEAD
 * When the target is a directory, HFS must ensure that its empty.
 *
 * Note that this function requires up to 6 vnodes in order to work properly
 * if it is operating on files (and not on directories).  This is because only
 * files can have resource forks, and we now require iocounts to be held on the
 * vnodes corresponding to the resource forks (if applicable) as well as
 * the files or directories undergoing rename.  The problem with not holding 
 * iocounts on the resource fork vnodes is that it can lead to a deadlock 
 * situation: The rsrc fork of the source file may be recycled and reclaimed 
 * in order to provide a vnode for the destination file's rsrc fork.  Since
 * data and rsrc forks share the same cnode, we'd eventually try to lock the
 * source file's cnode in order to sync its rsrc fork to disk, but it's already 
 * been locked.  By taking the rsrc fork vnodes up front we ensure that they 
 * cannot be recycled, and that the situation mentioned above cannot happen.
=======
 * When the target is a directory, hfs_rename must ensure
 * that it is empty.
 *
 * The rename system call is responsible for freeing
 * the pathname buffers (ie no need to call VOP_ABORTOP).
>>>>>>> origin/10.2
 */
int
hfs_vnop_rename(ap)
	struct vnop_rename_args  /* {
		struct vnode *a_fdvp;
		struct vnode *a_fvp;
		struct componentname *a_fcnp;
		struct vnode *a_tdvp;
		struct vnode *a_tvp;
		struct componentname *a_tcnp;
		vfs_context_t a_context;
	} */ *ap;
{
	struct vnode *tvp = ap->a_tvp;
	struct vnode *tdvp = ap->a_tdvp;
	struct vnode *fvp = ap->a_fvp;
	struct vnode *fdvp = ap->a_fdvp;
<<<<<<< HEAD
	/*
	 * Note that we only need locals for the target/destination's
 	 * resource fork vnode (and only if necessary).  We don't care if the
	 * source has a resource fork vnode or not.
	 */
	struct vnode *tvp_rsrc = NULLVP;	
	uint32_t tvp_rsrc_vid = 0;
=======
	struct vnode *rvp = NULLVP;
>>>>>>> origin/10.5
	struct componentname *tcnp = ap->a_tcnp;
	struct componentname *fcnp = ap->a_fcnp;
<<<<<<< HEAD
	struct proc *p = vfs_context_proc(ap->a_context);
	struct cnode *fcp;
	struct cnode *fdcp;
	struct cnode *tdcp;
	struct cnode *tcp;
	struct cnode *error_cnode;
=======
	struct proc *p = fcnp->cn_proc;
	struct cnode *fcp = NULL;
	struct cnode *fdcp = NULL;
	struct cnode *tdcp = VTOC(tdvp);
>>>>>>> origin/10.2
	struct cat_desc from_desc;
	struct cat_desc to_desc;
	struct cat_desc out_desc;
	struct hfsmount *hfsmp;
<<<<<<< HEAD
	cat_cookie_t cookie;
	int tvp_deleted = 0;
	int started_tr = 0, got_cookie = 0;
	int took_trunc_lock = 0;
	int lockflags;
	int error;
<<<<<<< HEAD
	time_t orig_from_ctime, orig_to_ctime;
	int emit_rename = 1;
	int emit_delete = 1;
<<<<<<< HEAD
	int is_tracked = 0;
	int unlocked;
<<<<<<< HEAD
=======
>>>>>>> origin/10.7
=======
>>>>>>> origin/10.9

	orig_from_ctime = VTOC(fvp)->c_ctime;
	if (tvp && VTOC(tvp)) {
		orig_to_ctime = VTOC(tvp)->c_ctime;
	} else {
		orig_to_ctime = ~0;
	}

	hfsmp = VTOHFS(tdvp);
<<<<<<< HEAD
=======
	/* 
	 * Do special case checks here.  If fvp == tvp then we need to check the
	 * cnode with locks held.
	 */
	if (fvp == tvp) {
		int is_hardlink = 0;
		/* 
		 * In this case, we do *NOT* ever emit a DELETE event.  
		 * We may not necessarily emit a RENAME event 
		 */	
		emit_delete = 0;
		if ((error = hfs_lock(VTOC(fvp), HFS_SHARED_LOCK))) {
			return error;
		}
		/* Check to see if the item is a hardlink or not */
		is_hardlink = (VTOC(fvp)->c_flag & C_HARDLINK);
		hfs_unlock (VTOC(fvp));
		
		/* 
		 * If the item is not a hardlink, then case sensitivity must be off, otherwise
		 * two names should not resolve to the same cnode unless they were case variants.
		 */
		if (is_hardlink) {
			emit_rename = 0;
			/*
			 * Hardlinks are a little trickier.  We only want to emit a rename event
			 * if the item is a hardlink, the parent directories are the same, case sensitivity
			 * is off, and the case folded names are the same.  See the fvp == tvp case below for more
			 * info.
			 */

			if ((fdvp == tdvp) && ((hfsmp->hfs_flags & HFS_CASE_SENSITIVE) == 0)) {
				if (hfs_namecmp((const u_int8_t *)fcnp->cn_nameptr, fcnp->cn_namelen,
							(const u_int8_t *)tcnp->cn_nameptr, tcnp->cn_namelen) == 0) {
					/* Then in this case only it is ok to emit a rename */
					emit_rename = 1;
				}
			}
		}
	}
	if (emit_rename) {
		check_for_tracked_file(fvp, orig_from_ctime, NAMESPACE_HANDLER_RENAME_OP, NULL);
	}

	if (tvp && VTOC(tvp)) {
		if (emit_delete) {
			check_for_tracked_file(tvp, orig_to_ctime, NAMESPACE_HANDLER_DELETE_OP, NULL);
		}
	}
	
>>>>>>> origin/10.7
	/* 
	 * Do special case checks here.  If fvp == tvp then we need to check the
	 * cnode with locks held.
	 */
<<<<<<< HEAD
	if (fvp == tvp) {
		int is_hardlink = 0;
		/* 
		 * In this case, we do *NOT* ever emit a DELETE event.  
		 * We may not necessarily emit a RENAME event 
		 */	
		emit_delete = 0;
		if ((error = hfs_lock(VTOC(fvp), HFS_SHARED_LOCK, HFS_LOCK_DEFAULT))) {
=======


	if ((vnode_isreg(fvp)) || (vnode_islnk(fvp))) {

		if ((error = hfs_lock (VTOC(fvp), HFS_EXCLUSIVE_LOCK))) {
			return (error);
		}
		
		/*
		 * We care if we race against rename/delete with this cnode, so we'll
		 * error out if this file becomes open-unlinked during this call.
		 */
		error = hfs_vgetrsrc(VTOHFS(fvp), fvp, &fvp_rsrc, TRUE, TRUE);
		hfs_unlock (VTOC(fvp));
		if (error) {
			if (fvp_rsrc) {
				vnode_put (fvp_rsrc);
			}
>>>>>>> origin/10.6
			return error;
		}
		/* Check to see if the item is a hardlink or not */
		is_hardlink = (VTOC(fvp)->c_flag & C_HARDLINK);
		hfs_unlock (VTOC(fvp));
		
		/* 
		 * If the item is not a hardlink, then case sensitivity must be off, otherwise
		 * two names should not resolve to the same cnode unless they were case variants.
		 */
<<<<<<< HEAD
		if (is_hardlink) {
			emit_rename = 0;
			/*
			 * Hardlinks are a little trickier.  We only want to emit a rename event
			 * if the item is a hardlink, the parent directories are the same, case sensitivity
			 * is off, and the case folded names are the same.  See the fvp == tvp case below for more
			 * info.
			 */

			if ((fdvp == tdvp) && ((hfsmp->hfs_flags & HFS_CASE_SENSITIVE) == 0)) {
				if (hfs_namecmp((const u_int8_t *)fcnp->cn_nameptr, fcnp->cn_namelen,
							(const u_int8_t *)tcnp->cn_nameptr, tcnp->cn_namelen) == 0) {
					/* Then in this case only it is ok to emit a rename */
					emit_rename = 1;
				}
=======
		if (hfs_lock (VTOC(tvp), HFS_EXCLUSIVE_LOCK) == 0) {
			tcp = VTOC(tvp);
			
			/* 
			 * We only care if we get an open-unlinked file on the dst so we 
			 * know to null out tvp/tcp to make the rename operation act 
			 * as if they never existed.  Because they're effectively out of the
			 * namespace already it's fine to do this.  If this is true, then
			 * make sure to unlock the cnode and drop the iocount only after the unlock.
			 */
			error = hfs_vgetrsrc(VTOHFS(tvp), tvp, &tvp_rsrc, TRUE, TRUE);
			hfs_unlock (tcp);
			if (error) {
				/*
				 * Since we specify TRUE for error-on-unlinked in hfs_vgetrsrc,
				 * we can get a rsrc fork vp even if it returns an error.
				 */
				tcp = NULL;
				tvp = NULL;
				if (tvp_rsrc) {
					vnode_put (tvp_rsrc);
					tvp_rsrc = NULLVP;
				}
				/* just bypass truncate lock and act as if we never got tcp/tvp */
				goto retry;
>>>>>>> origin/10.6
			}
		}
	}
	if (emit_rename) {
		/* c_bsdflags should only be assessed while holding the cnode lock.  
		 * This is not done consistently throughout the code and can result 
		 * in race.  This will be fixed via rdar://12181064
		 */
		if (VTOC(fvp)->c_bsdflags & UF_TRACKED) {
			is_tracked = 1;
		}
		check_for_tracked_file(fvp, orig_from_ctime, NAMESPACE_HANDLER_RENAME_OP, NULL);
	}

	if (tvp && VTOC(tvp)) {
		if (emit_delete) {
			check_for_tracked_file(tvp, orig_to_ctime, NAMESPACE_HANDLER_DELETE_OP, NULL);
		}
	}

retry:
	/* When tvp exists, take the truncate lock for hfs_removefile(). */
=======
	int rsrc_vid = 0;
	int recycle_rsrc = 0;
	
	/* When tvp exist, take the truncate lock for the hfs_removefile(). */
>>>>>>> origin/10.5
	if (tvp && (vnode_isreg(tvp) || vnode_islnk(tvp))) {
		hfs_lock_truncate(VTOC(tvp), HFS_EXCLUSIVE_LOCK, HFS_LOCK_DEFAULT);
		took_trunc_lock = 1;
	}

relock:
	error = hfs_lockfour(VTOC(fdvp), VTOC(fvp), VTOC(tdvp), tvp ? VTOC(tvp) : NULL,
	                     HFS_EXCLUSIVE_LOCK, &error_cnode);
	if (error) {
		if (took_trunc_lock) {
<<<<<<< HEAD
			hfs_unlock_truncate(VTOC(tvp), HFS_LOCK_DEFAULT);
			took_trunc_lock = 0;
		}

		/* 
		 * We hit an error path.  If we were trying to re-acquire the locks
		 * after coming through here once, we might have already obtained
		 * an iocount on tvp's resource fork vnode.  Drop that before dealing
		 * with the failure.  Note this is safe -- since we are in an
		 * error handling path, we can't be holding the cnode locks.
		 */
		if (tvp_rsrc) {
			vnode_put (tvp_rsrc);
			tvp_rsrc_vid = 0;
			tvp_rsrc = NULL;
		}

		/* 
		 * tvp might no longer exist.  If the cause of the lock failure 
		 * was tvp, then we can try again with tvp/tcp set to NULL.  
		 * This is ok because the vfs syscall will vnode_put the vnodes 
		 * after we return from hfs_vnop_rename.
		 */
		if ((error == ENOENT) && (tvp != NULL) && (error_cnode == VTOC(tvp))) {	
			tcp = NULL;
			tvp = NULL;
			goto retry;
		}

		/* If we want to reintroduce notifications for failed renames, this
		   is the place to do it. */

=======
			hfs_unlock_truncate(VTOC(tvp), TRUE);	
			took_trunc_lock = 0;
		}
        /* 
         * tvp might no longer exist. if we get ENOENT, re-check the
         * C_NOEXISTS flag  on tvp to find out whether it's still in the
         * namespace.
         */
        if (error == ENOENT && tvp) {
            /* 
             * It's okay to just check C_NOEXISTS without having a lock,
             * because we have an iocount on it from the vfs layer so it can't
             * have disappeared.
             */
            if (VTOC(tvp)->c_flag & C_NOEXISTS) {
                /*
                 * tvp is no longer in the namespace. Try again with NULL
                 * tvp/tcp (NULLing these out is fine because the vfs syscall
                 * will vnode_put the vnodes).
                 */
                tcp = NULL;
                tvp = NULL;
                goto retry;
            }
        }
>>>>>>> origin/10.5
		return (error);
	}

	fdcp = VTOC(fdvp);
	fcp = VTOC(fvp);
	tdcp = VTOC(tdvp);
	tcp = tvp ? VTOC(tvp) : NULL;
<<<<<<< HEAD

	//
	// if the item is tracked but doesn't have a document_id, assign one and generate an fsevent for it
	//
	unlocked = 0;
	if ((fcp->c_bsdflags & UF_TRACKED) && ((struct FndrExtendedDirInfo *)((char *)&fcp->c_attr.ca_finderinfo + 16))->document_id == 0) {
		uint32_t newid;

		hfs_unlockfour(VTOC(fdvp), VTOC(fvp), VTOC(tdvp), tvp ? VTOC(tvp) : NULL);
		unlocked = 1;

		if (hfs_generate_document_id(hfsmp, &newid) == 0) {
			hfs_lock(fcp, HFS_EXCLUSIVE_LOCK, HFS_LOCK_DEFAULT);
			((struct FndrExtendedDirInfo *)((char *)&fcp->c_attr.ca_finderinfo + 16))->document_id = newid;
#if CONFIG_FSE
			add_fsevent(FSE_DOCID_CHANGED, vfs_context_current(),
				    FSE_ARG_DEV,   hfsmp->hfs_raw_dev,
				    FSE_ARG_INO,   (ino64_t)0,             // src inode #
				    FSE_ARG_INO,   (ino64_t)fcp->c_fileid,  // dst inode #
				    FSE_ARG_INT32, newid,
				    FSE_ARG_DONE);
#endif
			hfs_unlock(fcp);
		} else {
			// XXXdbg - couldn't get a new docid... what to do?  can't really fail the rename...
		}

		//
		// check if we're going to need to fix tcp as well.  if we aren't, go back relock
		// everything.  otherwise continue on and fix up tcp as well before relocking.
		//
		if (tcp == NULL || !(tcp->c_bsdflags & UF_TRACKED) || ((struct FndrExtendedDirInfo *)((char *)&tcp->c_attr.ca_finderinfo + 16))->document_id != 0) {
			goto relock;
		}
	}

	//
	// same thing for tcp if it's set
	//
	if (tcp && (tcp->c_bsdflags & UF_TRACKED) && ((struct FndrExtendedDirInfo *)((char *)&tcp->c_attr.ca_finderinfo + 16))->document_id == 0) {
		uint32_t newid;

		if (!unlocked) {
			hfs_unlockfour(VTOC(fdvp), VTOC(fvp), VTOC(tdvp), tvp ? VTOC(tvp) : NULL);
			unlocked = 1;
		}

		if (hfs_generate_document_id(hfsmp, &newid) == 0) {
			hfs_lock(tcp, HFS_EXCLUSIVE_LOCK, HFS_LOCK_DEFAULT);
			((struct FndrExtendedDirInfo *)((char *)&tcp->c_attr.ca_finderinfo + 16))->document_id = newid;
#if CONFIG_FSE
			add_fsevent(FSE_DOCID_CHANGED, vfs_context_current(),
				    FSE_ARG_DEV,   hfsmp->hfs_raw_dev,
				    FSE_ARG_INO,   (ino64_t)0,             // src inode #
				    FSE_ARG_INO,   (ino64_t)tcp->c_fileid,  // dst inode #
				    FSE_ARG_INT32, newid,
				    FSE_ARG_DONE);
#endif
			hfs_unlock(tcp);
		} else {
			// XXXdbg - couldn't get a new docid... what to do?  can't really fail the rename...
		}

		// go back up and relock everything.  next time through the if statement won't be true
		// and we'll skip over this block of code.
		goto relock;
	}



	//
	// if the item is tracked but doesn't have a document_id, assign one and generate an fsevent for it
	//
	unlocked = 0;
	if ((fcp->c_bsdflags & UF_TRACKED) && ((struct FndrExtendedDirInfo *)((char *)&fcp->c_attr.ca_finderinfo + 16))->document_id == 0) {
		uint32_t newid;

		hfs_unlockfour(VTOC(fdvp), VTOC(fvp), VTOC(tdvp), tvp ? VTOC(tvp) : NULL);
		unlocked = 1;

		if (hfs_generate_document_id(hfsmp, &newid) == 0) {
			hfs_lock(fcp, HFS_EXCLUSIVE_LOCK, HFS_LOCK_DEFAULT);
			((struct FndrExtendedDirInfo *)((char *)&fcp->c_attr.ca_finderinfo + 16))->document_id = newid;
#if CONFIG_FSE
			add_fsevent(FSE_DOCID_CHANGED, vfs_context_current(),
				    FSE_ARG_DEV,   hfsmp->hfs_raw_dev,
				    FSE_ARG_INO,   (ino64_t)0,             // src inode #
				    FSE_ARG_INO,   (ino64_t)fcp->c_fileid,  // dst inode #
				    FSE_ARG_INT32, newid,
				    FSE_ARG_DONE);
#endif
			hfs_unlock(fcp);
		} else {
			// XXXdbg - couldn't get a new docid... what to do?  can't really fail the rename...
		}

		//
		// check if we're going to need to fix tcp as well.  if we aren't, go back relock
		// everything.  otherwise continue on and fix up tcp as well before relocking.
		//
		if (tcp == NULL || !(tcp->c_bsdflags & UF_TRACKED) || ((struct FndrExtendedDirInfo *)((char *)&tcp->c_attr.ca_finderinfo + 16))->document_id != 0) {
			goto relock;
		}
	}

	//
	// same thing for tcp if it's set
	//
	if (tcp && (tcp->c_bsdflags & UF_TRACKED) && ((struct FndrExtendedDirInfo *)((char *)&tcp->c_attr.ca_finderinfo + 16))->document_id == 0) {
		uint32_t newid;

		if (!unlocked) {
			hfs_unlockfour(VTOC(fdvp), VTOC(fvp), VTOC(tdvp), tvp ? VTOC(tvp) : NULL);
			unlocked = 1;
		}

		if (hfs_generate_document_id(hfsmp, &newid) == 0) {
			hfs_lock(tcp, HFS_EXCLUSIVE_LOCK, HFS_LOCK_DEFAULT);
			((struct FndrExtendedDirInfo *)((char *)&tcp->c_attr.ca_finderinfo + 16))->document_id = newid;
#if CONFIG_FSE
			add_fsevent(FSE_DOCID_CHANGED, vfs_context_current(),
				    FSE_ARG_DEV,   hfsmp->hfs_raw_dev,
				    FSE_ARG_INO,   (ino64_t)0,             // src inode #
				    FSE_ARG_INO,   (ino64_t)tcp->c_fileid,  // dst inode #
				    FSE_ARG_INT32, newid,
				    FSE_ARG_DONE);
#endif
			hfs_unlock(tcp);
		} else {
			// XXXdbg - couldn't get a new docid... what to do?  can't really fail the rename...
		}

		// go back up and relock everything.  next time through the if statement won't be true
		// and we'll skip over this block of code.
		goto relock;
	}



	/* 
	 * Acquire iocounts on the destination's resource fork vnode 
	 * if necessary. If dst/src are files and the dst has a resource 
	 * fork vnode, then we need to try and acquire an iocount on the rsrc vnode. 
	 * If it does not exist, then we don't care and can skip it.
	 */
	if ((vnode_isreg(fvp)) || (vnode_islnk(fvp))) {
		if ((tvp) && (tcp->c_rsrc_vp) && (tvp_rsrc == NULL)) {
			tvp_rsrc = tcp->c_rsrc_vp;
			/*
			 * We can look at the vid here because we're holding the 
			 * cnode lock on the underlying cnode for this rsrc vnode. 
			 */
			tvp_rsrc_vid = vnode_vid (tvp_rsrc);

			/* Unlock everything to acquire iocount on this rsrc vnode */
			if (took_trunc_lock) {
				hfs_unlock_truncate (VTOC(tvp), HFS_LOCK_DEFAULT);
				took_trunc_lock = 0;	
			}	
			hfs_unlockfour(fdcp, fcp, tdcp, tcp);

			if (vnode_getwithvid (tvp_rsrc, tvp_rsrc_vid)) {
				/* iocount acquisition failed.  Reset fields and start over.. */
				tvp_rsrc_vid = 0;
				tvp_rsrc = NULL;
			}
			goto retry;
		}
	}


=======
>>>>>>> origin/10.7

	/* Ensure we didn't race src or dst parent directories with rmdir. */
	if (fdcp->c_flag & (C_NOEXISTS | C_DELETED)) {
		error = ENOENT;
		goto out;
	}

	if (tdcp->c_flag & (C_NOEXISTS | C_DELETED)) {
		error = ENOENT;
		goto out;	
	}


	/* Check for a race against unlink.  The hfs_valid_cnode checks validate
	 * the parent/child relationship with fdcp and tdcp, as well as the
	 * component name of the target cnodes.  
	 */
	if ((fcp->c_flag & (C_NOEXISTS | C_DELETED)) || !hfs_valid_cnode(hfsmp, fdvp, fcnp, fcp->c_fileid, NULL, &error)) {
		error = ENOENT;
		goto out;
	}

	if (tcp && ((tcp->c_flag & (C_NOEXISTS | C_DELETED)) || !hfs_valid_cnode(hfsmp, tdvp, tcnp, tcp->c_fileid, NULL, &error))) {
	    //
	    // hmm, the destination vnode isn't valid any more.
	    // in this case we can just drop him and pretend he
	    // never existed in the first place.
	    //
	    if (took_trunc_lock) {
			hfs_unlock_truncate(VTOC(tvp), HFS_LOCK_DEFAULT);
			took_trunc_lock = 0;
	    }
		error = 0;

	    hfs_unlockfour(fdcp, fcp, tdcp, tcp);

	    tcp = NULL;
	    tvp = NULL;
	    
	    // retry the locking with tvp null'ed out
	    goto retry;
	}

	fdcp->c_flag |= C_DIR_MODIFICATION;
	if (fdvp != tdvp) {
	    tdcp->c_flag |= C_DIR_MODIFICATION;
	}

	/*
	 * Disallow renaming of a directory hard link if the source and 
	 * destination parent directories are different, or a directory whose 
	 * descendant is a directory hard link and the one of the ancestors
	 * of the destination directory is a directory hard link.
	 */
	if (vnode_isdir(fvp) && (fdvp != tdvp)) {
		if (fcp->c_flag & C_HARDLINK) {
			error = EPERM;
			goto out;
		}
		if (fcp->c_attr.ca_recflags & kHFSHasChildLinkMask) {
		    lockflags = hfs_systemfile_lock(hfsmp, SFL_CATALOG, HFS_SHARED_LOCK);
		    if (cat_check_link_ancestry(hfsmp, tdcp->c_fileid, 0)) {
				error = EPERM;
				hfs_systemfile_unlock(hfsmp, lockflags);
				goto out;
			}
			hfs_systemfile_unlock(hfsmp, lockflags);
		}
=======
	struct timeval tv;
	int fdvp_locked, fvp_locked, tdvp_locked;
	int tvp_deleted;
	int started_tr = 0, grabbed_lock = 0;
	int error = 0;

	hfsmp = VTOHFS(tdvp);

	/* Establish our vnode lock state. */
	tdvp_locked = 1;
	fdvp_locked = 0;
	fvp_locked = 0;
	tvp_deleted = 0;

	/*
	 * When fvp matches tvp they must be case variants
	 * or hard links.
	 *
	 * For the hardlink case there can be an extra ref on fvp.
	 */
	if (fvp == tvp) {
		if (VOP_ISLOCKED(fvp) &&
			(VTOC(fvp)->c_lock.lk_lockholder == p->p_pid) &&
			(VTOC(fvp)->c_lock.lk_lockthread == current_thread())) {
			fvp_locked = 1;
			vrele(fvp);  /* drop the extra ref */
		}
		tvp = NULL;
		/*
		 * If this a hard link and its not a case
		 * variant then keep tvp around for removal.
		 */
		if ((VTOC(fvp)->c_flag & C_HARDLINK) &&
		    ((fdvp != tdvp) ||
		     (hfs_namecmp(fcnp->cn_nameptr, fcnp->cn_namelen,
		                  tcnp->cn_nameptr, tcnp->cn_namelen) != 0))) {
			tvp = fvp;
		}
>>>>>>> origin/10.2
	}

	/*
	 * The following edge case is caught here:
	 * (to cannot be a descendent of from)
	 *
	 *       o fdvp
	 *      /
	 *     /
	 *    o fvp
	 *     \
	 *      \
	 *       o tdvp
	 *      /
	 *     /
	 *    o tvp
	 */
	if (tdcp->c_parentcnid == fcp->c_fileid) {
		error = EINVAL;
		goto out;
	}

	/*
	 * The following two edge cases are caught here:
	 * (note tvp is not empty)
	 *
	 *       o tdvp               o tdvp
	 *      /                    /
	 *     /                    /
	 *    o tvp            tvp o fdvp
	 *     \                    \
	 *      \                    \
	 *       o fdvp               o fvp
	 *      /
	 *     /
	 *    o fvp
	 */
	if (tvp && vnode_isdir(tvp) && (tcp->c_entries != 0) && fvp != tvp) {
		error = ENOTEMPTY;
		goto out;
	}

	/*
	 * The following edge case is caught here:
	 * (the from child and parent are the same)
	 *
	 *          o tdvp
	 *         /
	 *        /
	 *  fdvp o fvp
	 */
	if (fdvp == fvp) {
		error = EINVAL;
		goto out;
	}

	/*
	 * Make sure "from" vnode and its parent are changeable.
	 */
	if ((fcp->c_bsdflags & (IMMUTABLE | APPEND)) || (fdcp->c_bsdflags & APPEND)) {
		error = EPERM;
		goto out;
	}

	/*
	 * If the destination parent directory is "sticky", then the
	 * user must own the parent directory, or the destination of
	 * the rename, otherwise the destination may not be changed
	 * (except by root). This implements append-only directories.
	 *
	 * Note that checks for immutable and write access are done
	 * by the call to hfs_removefile.
	 */
<<<<<<< HEAD
	if (tvp && (tdcp->c_mode & S_ISTXT) &&
	    (suser(vfs_context_ucred(tcnp->cn_context), NULL)) &&
	    (kauth_cred_getuid(vfs_context_ucred(tcnp->cn_context)) != tdcp->c_uid) &&
	    (hfs_owner_rights(hfsmp, tcp->c_uid, vfs_context_ucred(tcnp->cn_context), p, false)) ) {
		error = EPERM;
		goto out;
	}

	/* Don't allow modification of the journal or journal_info_block */
	if (hfs_is_journal_file(hfsmp, fcp) ||
	    (tcp && hfs_is_journal_file(hfsmp, tcp))) {
		error = EPERM;
=======
	if ((fvp->v_mount != tdvp->v_mount) ||
	    (tvp && (fvp->v_mount != tvp->v_mount))) {
		error = EXDEV;
>>>>>>> origin/10.2
		goto out;
	}

#if QUOTA
	if (tvp)
		(void)hfs_getinoquota(tcp);
#endif
	/* Preflighting done, take fvp out of the name space. */
	cache_purge(fvp);

#if CONFIG_SECLUDED_RENAME
	/*
<<<<<<< HEAD
	 * Check for "secure" rename that imposes additional restrictions on the
	 * source vnode.  We wait until here to check in order to prevent a race
	 * with other threads that manage to look up fvp, but their open or link
	 * is blocked by our locks.  At this point, with fvp out of the name cache,
	 * and holding the lock on fdvp, no other thread can find fvp.
	 *
	 * TODO: Do we need to limit these checks to regular files only?
	 */
	if (fcnp->cn_flags & CN_SECLUDE_RENAME) {
		if (vnode_isdir(fvp)) {
			error = EISDIR;
	        	goto out;
	    	}
	    
		/*
		 * Neither fork of source may be open or memory mapped.
		 * We also don't want it in use by any other system call.
		 * The file must not have hard links.
		 *
		 * We can't simply use vnode_isinuse() because that does not
		 * count opens with O_EVTONLY.  We don't want a malicious
		 * process using O_EVTONLY to subvert a secluded rename.
		 */
		if (fcp->c_linkcount != 1) {
			error = EMLINK;
			goto out;
		}

		if (fcp->c_rsrc_vp && (fcp->c_rsrc_vp->v_usecount > 0 ||
				       fcp->c_rsrc_vp->v_iocount > 0)) {
			/* Resource fork is in use (including O_EVTONLY) */
			error = EBUSY;
			goto out;
		}
		if (fcp->c_vp && (fcp->c_vp->v_usecount > (fcp->c_rsrc_vp ? 1 : 0) ||
				  fcp->c_vp->v_iocount > 1)) {
			/*
			 * Data fork is in use, including O_EVTONLY, but not
			 * including a reference from the resource fork.
			 */
			error = EBUSY;
			goto out;
		}
=======
	 * Make sure "from" vnode and its parent are changeable.
	 */
	if ((VTOC(fvp)->c_flags & (IMMUTABLE | APPEND)) ||
	    (VTOC(fdvp)->c_flags & APPEND)) {
		error = EPERM;
		goto out;
>>>>>>> origin/10.2
	}
#endif
    
	bzero(&from_desc, sizeof(from_desc));
	from_desc.cd_nameptr = (const u_int8_t *)fcnp->cn_nameptr;
	from_desc.cd_namelen = fcnp->cn_namelen;
	from_desc.cd_parentcnid = fdcp->c_fileid;
	from_desc.cd_flags = fcp->c_desc.cd_flags & ~(CD_HASBUF | CD_DECOMPOSED);
	from_desc.cd_cnid = fcp->c_cnid;

<<<<<<< HEAD
	bzero(&to_desc, sizeof(to_desc));
	to_desc.cd_nameptr = (const u_int8_t *)tcnp->cn_nameptr;
	to_desc.cd_namelen = tcnp->cn_namelen;
	to_desc.cd_parentcnid = tdcp->c_fileid;
	to_desc.cd_flags = fcp->c_desc.cd_flags & ~(CD_HASBUF | CD_DECOMPOSED);
	to_desc.cd_cnid = fcp->c_cnid;

	if ((error = hfs_start_transaction(hfsmp)) != 0) {
	    goto out;
	}
	started_tr = 1;

	/* hfs_vnop_link() and hfs_vnop_rename() set kHFSHasChildLinkMask 
	 * inside a journal transaction and without holding a cnode lock.  
	 * As setting of this bit depends on being in journal transaction for 
	 * concurrency, check this bit again after we start journal transaction for rename
	 * to ensure that this directory does not have any descendant that
	 * is a directory hard link. 
	 */
	if (vnode_isdir(fvp) && (fdvp != tdvp)) {
		if (fcp->c_attr.ca_recflags & kHFSHasChildLinkMask) {
		    lockflags = hfs_systemfile_lock(hfsmp, SFL_CATALOG, HFS_SHARED_LOCK);
		    if (cat_check_link_ancestry(hfsmp, tdcp->c_fileid, 0)) {
				error = EPERM;
				hfs_systemfile_unlock(hfsmp, lockflags);
				goto out;
			}
			hfs_systemfile_unlock(hfsmp, lockflags);
		}
	}

	// if it's a hardlink then re-lookup the name so
	// that we get the correct cnid in from_desc (see
	// the comment in hfs_removefile for more details)
	//
	if (fcp->c_flag & C_HARDLINK) {
	    struct cat_desc tmpdesc;
	    cnid_t real_cnid;

	    tmpdesc.cd_nameptr = (const u_int8_t *)fcnp->cn_nameptr;
	    tmpdesc.cd_namelen = fcnp->cn_namelen;
	    tmpdesc.cd_parentcnid = fdcp->c_fileid;
	    tmpdesc.cd_hint = fdcp->c_childhint;
	    tmpdesc.cd_flags = fcp->c_desc.cd_flags & CD_ISDIR;
	    tmpdesc.cd_encoding = 0;
	    
	    lockflags = hfs_systemfile_lock(hfsmp, SFL_CATALOG, HFS_SHARED_LOCK);

	    if (cat_lookup(hfsmp, &tmpdesc, 0, 0, NULL, NULL, NULL, &real_cnid) != 0) {
		hfs_systemfile_unlock(hfsmp, lockflags);
		goto out;
	    }

	    // use the real cnid instead of whatever happened to be there
	    from_desc.cd_cnid = real_cnid;
	    hfs_systemfile_unlock(hfsmp, lockflags);
	}

	/*
	 * Reserve some space in the Catalog file.
	 */
	if ((error = cat_preflight(hfsmp, CAT_RENAME + CAT_DELETE, &cookie, p))) {
		goto out;
=======
	/*
	 * Be sure we are not renaming ".", "..", or an alias of ".".
	 */
	if ((fvp->v_type == VDIR) &&
	    (((fcnp->cn_namelen == 1) && (fcnp->cn_nameptr[0] == '.')) ||
	      (fdvp == fvp) ||
	      (fcnp->cn_flags&ISDOTDOT))) {
			error = EINVAL;
			goto out;
	}

	/*
	 * If the destination parent directory is "sticky", then the
	 * user must own the parent directory, or the destination of
	 * the rename, otherwise the destination may not be changed
	 * (except by root). This implements append-only directories.
	 *
	 * Note that checks for immutable, write access, and a non-empty
	 * target are done by the call to VOP_REMOVE.
	 */
	if (tvp && (tdcp->c_mode & S_ISTXT) &&
	    (tcnp->cn_cred->cr_uid != 0) &&
	    (tcnp->cn_cred->cr_uid != tdcp->c_uid) &&
	    (hfs_owner_rights(hfsmp, VTOC(tvp)->c_uid, tcnp->cn_cred, p, false)) ) {
			error = EPERM;
			goto out;
>>>>>>> origin/10.2
	}
	got_cookie = 1;

	/*
<<<<<<< HEAD
	 * If the destination exists then it may need to be removed.  
	 * 
	 * Due to HFS's locking system, we should always move the 
	 * existing 'tvp' element to the hidden directory in hfs_vnop_rename.
	 * Because the VNOP_LOOKUP call enters and exits the filesystem independently
	 * of the actual vnop that it was trying to do (stat, link, readlink),
	 * we must release the cnode lock of that element during the interim to 
	 * do MAC checking, vnode authorization, and other calls.  In that time, 
	 * the item can be deleted (or renamed over). However, only in the rename 
	 * case is it inappropriate to return ENOENT from any of those calls.  Either 
	 * the call should return information about the old element (stale), or get 
	 * information about the newer element that we are about to write in its place.  
	 * 
	 * HFS lookup has been modified to detect a rename and re-drive its 
	 * lookup internally. For other calls that have already succeeded in 
	 * their lookup call and are waiting to acquire the cnode lock in order 
	 * to proceed, that cnode lock will not fail due to the cnode being marked 
	 * C_NOEXISTS, because it won't have been marked as such.  It will only 
	 * have C_DELETED.  Thus, they will simply act on the stale open-unlinked
	 * element.  All future callers will get the new element.
	 *
	 * To implement this behavior, we pass the "only_unlink" argument to 
	 * hfs_removefile and hfs_removedir.  This will result in the vnode acting 
	 * as though it is open-unlinked.  Additionally, when we are done moving the 
	 * element to the hidden directory, we vnode_recycle the target so that it is 
	 * reclaimed as soon as possible.  Reclaim and inactive are both 
	 * capable of clearing out unused blocks for an open-unlinked file or dir.
	 */
	if (tvp) {
		//
		// if the destination has a document id, we need to preserve it
		//
		if (fvp != tvp) {
			uint32_t document_id;
			struct FndrExtendedDirInfo *ffip = (struct FndrExtendedDirInfo *)((char *)&fcp->c_attr.ca_finderinfo + 16);
			struct FndrExtendedDirInfo *tfip = (struct FndrExtendedDirInfo *)((char *)&tcp->c_attr.ca_finderinfo + 16);
			
			if (ffip->document_id && tfip->document_id) {
				// both documents are tracked.  only save a tombstone from tcp and do nothing else.
				save_tombstone(hfsmp, tdvp, tvp, tcnp, 0);
			} else {
				struct  doc_tombstone *ut;
				ut = get_uthread_doc_tombstone();
				
				document_id = tfip->document_id;
				tfip->document_id = 0;
			
				if (document_id != 0) {
					// clear UF_TRACKED as well since tcp is now no longer tracked
					tcp->c_bsdflags &= ~UF_TRACKED;
					(void) cat_update(hfsmp, &tcp->c_desc, &tcp->c_attr, NULL, NULL);
				}

				if (ffip->document_id == 0 && document_id != 0) {
					// printf("RENAME: preserving doc-id %d onto %s (from ino %d, to ino %d)\n", document_id, tcp->c_desc.cd_nameptr, tcp->c_desc.cd_cnid, fcp->c_desc.cd_cnid);
					fcp->c_bsdflags |= UF_TRACKED;
					ffip->document_id = document_id;
					
					(void) cat_update(hfsmp, &fcp->c_desc, &fcp->c_attr, NULL, NULL);
#if CONFIG_FSE
					add_fsevent(FSE_DOCID_CHANGED, vfs_context_current(),
						    FSE_ARG_DEV, hfsmp->hfs_raw_dev,
						    FSE_ARG_INO, (ino64_t)tcp->c_fileid,           // src inode #
						    FSE_ARG_INO, (ino64_t)fcp->c_fileid,           // dst inode #
						    FSE_ARG_INT32, (uint32_t)ffip->document_id,
						    FSE_ARG_DONE);
#endif
				} else if ((fcp->c_bsdflags & UF_TRACKED) && should_save_docid_tombstone(ut, fvp, fcnp)) {

					if (ut->t_lastop_document_id) {
						clear_tombstone_docid(ut, hfsmp, NULL);
					}
					save_tombstone(hfsmp, fdvp, fvp, fcnp, 0);

					//printf("RENAME: (dest-exists): saving tombstone doc-id %lld @ %s (ino %d)\n",
					//       ut->t_lastop_document_id, ut->t_lastop_filename, fcp->c_desc.cd_cnid);
				}
			}
		}

		/*
		 * When fvp matches tvp they could be case variants
		 * or matching hard links.
		 */
		if (fvp == tvp) {
			if (!(fcp->c_flag & C_HARDLINK)) {
				/* 
				 * If they're not hardlinks, then fvp == tvp must mean we 
				 * are using case-insensitive HFS because case-sensitive would
				 * not use the same vnode for both.  In this case we just update
				 * the catalog for: a -> A
				 */
				goto skip_rm;  /* simple case variant */

			}
		   	/* For all cases below, we must be using hardlinks */	
			else if ((fdvp != tdvp) ||
			           (hfsmp->hfs_flags & HFS_CASE_SENSITIVE)) {
				/*
				 * If the parent directories are not the same, AND the two items
				 * are hardlinks, posix says to do nothing:
				 * dir1/fred <-> dir2/bob   and the op was mv dir1/fred -> dir2/bob
				 * We just return 0 in this case.
				 *
				 * If case sensitivity is on, and we are using hardlinks 
				 * then renaming is supposed to do nothing.
				 * dir1/fred <-> dir2/FRED, and op == mv dir1/fred -> dir2/FRED
				 */
				goto out;  /* matching hardlinks, nothing to do */

			} else if (hfs_namecmp((const u_int8_t *)fcnp->cn_nameptr, fcnp->cn_namelen,
			                       (const u_int8_t *)tcnp->cn_nameptr, tcnp->cn_namelen) == 0) {
				/*
				 * If we get here, then the following must be true:
				 * a) We are running case-insensitive HFS+.
				 * b) Both paths 'fvp' and 'tvp' are in the same parent directory.
				 * c) the two names are case-variants of each other.
				 *
				 * In this case, we are really only dealing with a single catalog record
				 * whose name is being updated.
				 * 
				 * op is dir1/fred -> dir1/FRED
				 * 
				 * We need to special case the name matching, because if
				 * dir1/fred <-> dir1/bob were the two links, and the 
				 * op was dir1/fred -> dir1/bob
				 * That would fail/do nothing.
				 */
				goto skip_rm;  /* case-variant hardlink in the same dir */
			} else {
				goto out;  /* matching hardlink, nothing to do */
			}
		}

		
		if (vnode_isdir(tvp)) {
			/*
			 * hfs_removedir will eventually call hfs_removefile on the directory
			 * we're working on, because only hfs_removefile does the renaming of the
			 * item to the hidden directory.  The directory will stay around in the
			 * hidden directory with C_DELETED until it gets an inactive or a reclaim.
			 * That way, we can destroy all of the EAs as needed and allow new ones to be
			 * written.
			 */
			error = hfs_removedir(tdvp, tvp, tcnp, HFSRM_SKIP_RESERVE, 1);
		}
		else {
<<<<<<< HEAD
			error = hfs_removefile(tdvp, tvp, tcnp, 0, HFSRM_SKIP_RESERVE, 0, NULL, 1);
			
			/*
			 * If the destination file had a resource fork vnode, then we need to get rid of
			 * its blocks when there are no more references to it.  Because the call to
			 * hfs_removefile above always open-unlinks things, we need to force an inactive/reclaim
			 * on the resource fork vnode, in order to prevent block leaks.  Otherwise,
			 * the resource fork vnode could prevent the data fork vnode from going out of scope
			 * because it holds a v_parent reference on it.  So we mark it for termination
			 * with a call to vnode_recycle. hfs_vnop_reclaim has been modified so that it 
			 * can clean up the blocks of open-unlinked files and resource forks. 
			 *
			 * We can safely call vnode_recycle on the resource fork because we took an iocount
			 * reference on it at the beginning of the function. 
			 */ 
			
			if ((error == 0) && (tcp->c_flag & C_DELETED) && (tvp_rsrc)) {
				vnode_recycle(tvp_rsrc);
=======
			if (tcp){
				rvp = tcp->c_rsrc_vp;
			}
			error = hfs_removefile(tdvp, tvp, tcnp, 0, HFSRM_SKIP_RESERVE, 0);
				
			/* If the destination file had a resource fork vnode, we couldn't do 
			 * anything about it in hfs_removefile because we didn't have a reference on it.  
			 * We need to take action here to prevent it from leaking blocks.  If removefile 
			 * succeeded, then squirrel away the vid of the resource fork vnode and force a 
			 * recycle after dropping all of the locks. The vid is guaranteed not to change 
			 * at this point because we still hold the cnode lock.
			 */
			if ((error == 0) && (tcp->c_flag & C_DELETED) && rvp && !vnode_isinuse(rvp, 0)) {
				rsrc_vid = vnode_vid(rvp);	
				recycle_rsrc = 1;
>>>>>>> origin/10.5
			}
		}

		if (error) {
			goto out;
		}
		
		tvp_deleted = 1;
		
		/* Mark 'tcp' as being deleted due to a rename */
		tcp->c_flag |= C_RENAMED;
		
		/*
		 * Aggressively mark tvp/tcp for termination to ensure that we recover all blocks
		 * as quickly as possible.
		 */
		vnode_recycle(tvp);
	} else {
		struct  doc_tombstone *ut;
		ut = get_uthread_doc_tombstone();
		
		//
		// There is nothing at the destination.  If the file being renamed is
		// tracked, save a "tombstone" of the document_id.  If the file is
		// not a tracked file, then see if it needs to inherit a tombstone.
		//
		// NOTE: we do not save a tombstone if the file being renamed begins
		//       with "atmp" which is done to work-around AutoCad's bizarre
		//       5-step un-safe save behavior
		//
		if (fcp->c_bsdflags & UF_TRACKED) {
			if (should_save_docid_tombstone(ut, fvp, fcnp)) {
				save_tombstone(hfsmp, fdvp, fvp, fcnp, 0);
				
				//printf("RENAME: (no dest): saving tombstone doc-id %lld @ %s (ino %d)\n",
				//       ut->t_lastop_document_id, ut->t_lastop_filename, fcp->c_desc.cd_cnid);
			} else {
				// intentionally do nothing
			}
		} else if (   ut->t_lastop_document_id != 0
			   && tdvp == ut->t_lastop_parent
			   && vnode_vid(tdvp) == ut->t_lastop_parent_vid
			   && strcmp((char *)ut->t_lastop_filename, (char *)tcnp->cn_nameptr) == 0) {

			//printf("RENAME: %s (ino %d) inheriting doc-id %lld\n", tcnp->cn_nameptr, fcp->c_desc.cd_cnid, ut->t_lastop_document_id);
			struct FndrExtendedFileInfo *fip = (struct FndrExtendedFileInfo *)((char *)&fcp->c_attr.ca_finderinfo + 16);
			fcp->c_bsdflags |= UF_TRACKED;
			fip->document_id = ut->t_lastop_document_id;
			cat_update(hfsmp, &fcp->c_desc, &fcp->c_attr, NULL, NULL);
			
			clear_tombstone_docid(ut, hfsmp, fcp);    // will send the docid-changed fsevent

		} else if (ut->t_lastop_document_id && should_save_docid_tombstone(ut, fvp, fcnp) && should_save_docid_tombstone(ut, tvp, tcnp)) {
			// no match, clear the tombstone
			//printf("RENAME: clearing the tombstone %lld @ %s\n", ut->t_lastop_document_id, ut->t_lastop_filename);
			clear_tombstone_docid(ut, hfsmp, NULL);
		}
			   
	}
skip_rm:
	/*
<<<<<<< HEAD
	 * All done with tvp and fvp. 
	 * 
=======
	 * All done with tvp and fvp
	 *
>>>>>>> origin/10.5
	 * We also jump to this point if there was no destination observed during lookup and namei.
	 * However, because only iocounts are held at the VFS layer, there is nothing preventing a 
	 * competing thread from racing us and creating a file or dir at the destination of this rename 
	 * operation.  If this occurs, it may cause us to get a spurious EEXIST out of the cat_rename 
	 * call below.  To preserve rename's atomicity, we need to signal VFS to re-drive the 
	 * namei/lookup and restart the rename operation.  EEXIST is an allowable errno to be bubbled 
	 * out of the rename syscall, but not for this reason, since it is a synonym errno for ENOTEMPTY.
	 * To signal VFS, we return ERECYCLE (which is also used for lookup restarts). This errno
	 * will be swallowed and it will restart the operation.
	 */
	
	lockflags = hfs_systemfile_lock(hfsmp, SFL_CATALOG, HFS_EXCLUSIVE_LOCK);
	error = cat_rename(hfsmp, &from_desc, &tdcp->c_desc, &to_desc, &out_desc);
	hfs_systemfile_unlock(hfsmp, lockflags);

	if (error) {
		if (error == EEXIST) {
			error = ERECYCLE;
		}
		goto out;
	}

	/* Invalidate negative cache entries in the destination directory */
	if (tdcp->c_flag & C_NEG_ENTRIES) {
		cache_purge_negatives(tdvp);
		tdcp->c_flag &= ~C_NEG_ENTRIES;
	}

	/* Update cnode's catalog descriptor */
	replace_desc(fcp, &out_desc);
	fcp->c_parentcnid = tdcp->c_fileid;
	fcp->c_hint = 0;

	/* Now indicate this cnode needs to have date-added written to the finderinfo */
	fcp->c_flag |= C_NEEDS_DATEADDED;
	(void) hfs_update (fvp, 0);


	hfs_volupdate(hfsmp, vnode_isdir(fvp) ? VOL_RMDIR : VOL_RMFILE,
	              (fdcp->c_cnid == kHFSRootFolderID));
	hfs_volupdate(hfsmp, vnode_isdir(fvp) ? VOL_MKDIR : VOL_MKFILE,
	              (tdcp->c_cnid == kHFSRootFolderID));

	/* Update both parent directories. */
	if (fdvp != tdvp) {
		if (vnode_isdir(fvp)) {
			/* If the source directory has directory hard link 
			 * descendants, set the kHFSHasChildLinkBit in the 
			 * destination parent hierarchy 
			 */
			if ((fcp->c_attr.ca_recflags & kHFSHasChildLinkMask) && 
			    !(tdcp->c_attr.ca_recflags & kHFSHasChildLinkMask)) {

				tdcp->c_attr.ca_recflags |= kHFSHasChildLinkMask;

				error = cat_set_childlinkbit(hfsmp, tdcp->c_parentcnid);
				if (error) {
					printf ("hfs_vnop_rename: error updating parent chain for %u\n", tdcp->c_cnid);
					error = 0;
				}
			}
			INC_FOLDERCOUNT(hfsmp, tdcp->c_attr);
			DEC_FOLDERCOUNT(hfsmp, fdcp->c_attr);
		}
		tdcp->c_entries++;
		tdcp->c_dirchangecnt++;
<<<<<<< HEAD
		tdcp->c_flag |= C_MODIFIED;
		hfs_incr_gencount(tdcp);

=======
		{
			struct FndrExtendedDirInfo *extinfo = (struct FndrExtendedDirInfo *)((u_int8_t*)tdcp->c_finderinfo + 16);
			extinfo->write_gen_counter = OSSwapHostToBigInt32(OSSwapBigToHostInt32(extinfo->write_gen_counter) + 1);
		}
>>>>>>> origin/10.9
		if (fdcp->c_entries > 0)
			fdcp->c_entries--;
		fdcp->c_dirchangecnt++;
		fdcp->c_flag |= C_MODIFIED;
		fdcp->c_touch_chgtime = TRUE;
		fdcp->c_touch_modtime = TRUE;

		if (ISSET(fcp->c_flag, C_HARDLINK)) {
			hfs_relorigin(fcp, fdcp->c_fileid);
			if (fdcp->c_fileid != fdcp->c_cnid)
				hfs_relorigin(fcp, fdcp->c_cnid);
		}

		(void) hfs_update(fdvp, 0);
	}
<<<<<<< HEAD
	hfs_incr_gencount(fdcp);
=======
	{	
		struct FndrExtendedDirInfo *extinfo = (struct FndrExtendedDirInfo *)((u_int8_t*)fdcp->c_finderinfo + 16);
		extinfo->write_gen_counter = OSSwapHostToBigInt32(OSSwapBigToHostInt32(extinfo->write_gen_counter) + 1);
	}
>>>>>>> origin/10.9
		
	tdcp->c_childhint = out_desc.cd_hint;	/* Cache directory's location */
	tdcp->c_touch_chgtime = TRUE;
	tdcp->c_touch_modtime = TRUE;

	(void) hfs_update(tdvp, 0);

<<<<<<< HEAD
<<<<<<< HEAD
	/* Update the vnode's name now that the rename has completed. */
	vnode_update_identity(fvp, tdvp, tcnp->cn_nameptr, tcnp->cn_namelen, 
			tcnp->cn_hash, (VNODE_UPDATE_PARENT | VNODE_UPDATE_NAME));
	
=======

	/* Update the vnode's name now that the rename has completed. */
	vnode_update_identity(fvp, tdvp, tcnp->cn_nameptr, tcnp->cn_namelen, 
			tcnp->cn_hash, (VNODE_UPDATE_PARENT | VNODE_UPDATE_NAME));
>>>>>>> origin/10.7
=======
	/* Update the vnode's name now that the rename has completed. */
	vnode_update_identity(fvp, tdvp, tcnp->cn_nameptr, tcnp->cn_namelen, 
			tcnp->cn_hash, (VNODE_UPDATE_PARENT | VNODE_UPDATE_NAME));

>>>>>>> origin/10.8
	/* 
	 * At this point, we may have a resource fork vnode attached to the 
	 * 'from' vnode.  If it exists, we will want to update its name, because
	 * it contains the old name + _PATH_RSRCFORKSPEC. ("/..namedfork/rsrc").
	 *
	 * Note that the only thing we need to update here is the name attached to
	 * the vnode, since a resource fork vnode does not have a separate resource
	 * cnode -- it's still 'fcp'.
	 */
	if (fcp->c_rsrc_vp) {
		char* rsrc_path = NULL;
		int len;

		/* Create a new temporary buffer that's going to hold the new name */
		MALLOC_ZONE (rsrc_path, caddr_t, MAXPATHLEN, M_NAMEI, M_WAITOK);
		len = snprintf (rsrc_path, MAXPATHLEN, "%s%s", tcnp->cn_nameptr, _PATH_RSRCFORKSPEC);
		len = MIN(len, MAXPATHLEN);

		/* 
		 * vnode_update_identity will do the following for us:
		 * 1) release reference on the existing rsrc vnode's name.
		 * 2) copy/insert new name into the name cache
		 * 3) attach the new name to the resource vnode
		 * 4) update the vnode's vid
		 */
		vnode_update_identity (fcp->c_rsrc_vp, fvp, rsrc_path, len, 0, (VNODE_UPDATE_NAME | VNODE_UPDATE_CACHE));
<<<<<<< HEAD
<<<<<<< HEAD
		
		/* Free the memory associated with the resource fork's name */
		FREE_ZONE (rsrc_path, MAXPATHLEN, M_NAMEI);	
	}
=======
=======
>>>>>>> origin/10.8

		/* Free the memory associated with the resource fork's name */
		FREE_ZONE (rsrc_path, MAXPATHLEN, M_NAMEI);	
	}
<<<<<<< HEAD

>>>>>>> origin/10.7
=======
>>>>>>> origin/10.8
out:
	if (got_cookie) {
		cat_postflight(hfsmp, &cookie, p);
	}
	if (started_tr) {
	    hfs_end_transaction(hfsmp);
	}

	fdcp->c_flag &= ~C_DIR_MODIFICATION;
	wakeup((caddr_t)&fdcp->c_flag);
	if (fdvp != tdvp) {
	    tdcp->c_flag &= ~C_DIR_MODIFICATION;
	    wakeup((caddr_t)&tdcp->c_flag);
	}

	hfs_unlockfour(fdcp, fcp, tdcp, tcp);

	if (took_trunc_lock) {
		hfs_unlock_truncate(VTOC(tvp), HFS_LOCK_DEFAULT);	
	}

	/* Now vnode_put the resource forks vnodes if necessary */
	if (tvp_rsrc) {
		vnode_put(tvp_rsrc);
		tvp_rsrc = NULL;	
	}

	/* After tvp is removed the only acceptable error is EIO */
	if (error && tvp_deleted)
		error = EIO;

	/* If we want to reintroduce notifications for renames, this is the
	   place to do it. */
=======
	 * All done with preflighting.
	 *
	 * We now break the call into two transactions:
	 * 1 - Remove the destionation (if any) using VOP_REMOVE,
	 * which in itself is a complete transaction.
	 *
	 * 2 - Rename source to destination.
	 * 
	 * Since all the preflighting is done, we assume that a
	 * rename failure is unlikely once part 1 is complete.
	 * Breaking rename into two transactions buys us a much
	 * simpler implementation with respect to the locking
	 * protocol.  There are only 3 vnodes to worry about
	 * locking in the correct order (instead of 4).
	 */

	/*
	 * Part 1 - If the destination exists then it needs to be removed.
	 */
	if (tvp) {
		/*
		 * VOP_REMOVE will vput tdvp so we better bump its
		 * ref count and relockit, always set tvp to NULL
		 * afterwards to indicate that we're done with it.
		 */
		VREF(tdvp);

		if (tvp == fvp) {
			if (fvp_locked) {
				VREF(fvp);
			} else {
				error = vget(fvp, LK_EXCLUSIVE | LK_RETRY, p);
				if (error)
					goto out;
				fvp_locked = 1;
			}
		} else {
			cache_purge(tvp);
		}

		/* Clear SAVENAME to keep VOP_REMOVE from smashing tcnp. */
		tcnp->cn_flags &= ~SAVENAME;

		if (tvp->v_type == VDIR)
			error = VOP_RMDIR(tdvp, tvp, tcnp);
		else
			error = VOP_REMOVE(tdvp, tvp, tcnp);

		/* Get lock states back in sync. */
		tdvp_locked = 0;
		if (tvp == fvp)
			fvp_locked = 0;	
		tvp = NULL;	/* all done with tvp */
		tvp_deleted = 1;

		if (error)
			goto out;  /* couldn't remove destination! */
	}
	/*
	 * All done with tvp.
	 *
	 * For POSIX compliance, if tvp was removed the only 
	 * error we can return from this point on is EIO.
	 */  

	/*
	 * Part 2 - rename source to destination
	 */

	/*
	 * Lock the vnodes before starting a journal transaction.
	 */
	if (fdvp != tdvp) {
		/*
		 * fvp is a child and must be locked last.
		 */
		if (fvp_locked) {
			VOP_UNLOCK(fvp, 0, p);
			fvp_locked = 0;
		}
		/*
		 *	If fdvp is the parent of tdvp then it needs to be locked first.
		 */
		if ((VTOC(fdvp)->c_cnid == VTOC(tdvp)->c_parentcnid)) {
			if (tdvp_locked) {
				VOP_UNLOCK(tdvp, 0, p);
				tdvp_locked = 0;
			}
			if ((error = vn_lock(fdvp, LK_EXCLUSIVE | LK_RETRY, p)))
				goto out;
			fdvp_locked = 1;
			if ((error = vn_lock(tdvp, LK_EXCLUSIVE | LK_RETRY, p)))
				goto out;
			tdvp_locked = 1;

		} else /* Lock tdvp then fdvp */ {
			if (!tdvp_locked) {
				if ((error = vn_lock(tdvp, LK_EXCLUSIVE | LK_RETRY, p)))
					goto out;
				tdvp_locked = 1;
			}
			if ((error = vn_lock(fdvp, LK_EXCLUSIVE | LK_RETRY, p)))
				goto out;
			fdvp_locked = 1;
		}
	} else if (!tdvp_locked) {
			/*
			 * fvp is a child and must be locked last.
			 */
			if (fvp_locked) {
				VOP_UNLOCK(fvp, 0, p);
				fvp_locked = 0;
			}
			if ((error = vn_lock(tdvp, LK_EXCLUSIVE | LK_RETRY, p)))
				goto out;
			tdvp_locked = 1;
	}

	/* Now its safe to lock fvp */
	if (!fvp_locked) {
		if (error = vn_lock(fvp, LK_EXCLUSIVE | LK_RETRY, p))
			goto out;
		fvp_locked = 1;
	}

	fdcp = VTOC(fdvp);
	fcp = VTOC(fvp);

	/*
	 * When a file moves out of "Cleanup At Startup"
	 * we can drop its NODUMP status.
	 */
	if ((fcp->c_flags & UF_NODUMP) &&
	    (fvp->v_type == VREG) &&
	    (fdvp != tdvp) &&
	    (fdcp->c_desc.cd_nameptr != NULL) &&
	    (strcmp(fdcp->c_desc.cd_nameptr, "Cleanup At Startup") == 0)) {
		fcp->c_flags &= ~UF_NODUMP;
		fcp->c_flag |= C_CHANGE;
		tv = time;
		(void) VOP_UPDATE(fvp, &tv, &tv, 0);
	}

	hfs_global_shared_lock_acquire(hfsmp);
	grabbed_lock = 1;
	if (hfsmp->jnl) {
	    if ((error = journal_start_transaction(hfsmp->jnl)) != 0) {
			goto out;
	    }
		started_tr = 1;
	}

	cache_purge(fvp);

	bzero(&from_desc, sizeof(from_desc));
	from_desc.cd_nameptr = fcnp->cn_nameptr;
	from_desc.cd_namelen = fcnp->cn_namelen;
	from_desc.cd_parentcnid = fdcp->c_cnid;
	from_desc.cd_flags = fcp->c_desc.cd_flags & ~(CD_HASBUF | CD_DECOMPOSED);
	from_desc.cd_cnid = fcp->c_cnid;

	bzero(&to_desc, sizeof(to_desc));
	to_desc.cd_nameptr = tcnp->cn_nameptr;
	to_desc.cd_namelen = tcnp->cn_namelen;
	to_desc.cd_parentcnid = tdcp->c_cnid;
	to_desc.cd_flags = fcp->c_desc.cd_flags & ~(CD_HASBUF | CD_DECOMPOSED);
	to_desc.cd_cnid = fcp->c_cnid;

	/* Lock catalog b-tree */
	error = hfs_metafilelocking(hfsmp, kHFSCatalogFileID, LK_EXCLUSIVE, p);
	if (error)
		 goto out;

	error = cat_rename(hfsmp, &from_desc, &tdcp->c_desc, &to_desc, &out_desc);

	/* Unlock catalog b-tree */
	(void) hfs_metafilelocking(hfsmp, kHFSCatalogFileID, LK_RELEASE, p);
	if (error)
		goto out;

	/* Update cnode's catalog descriptor */
   	replace_desc(fcp, &out_desc);

	hfs_volupdate(hfsmp, fvp->v_type == VDIR ? VOL_RMDIR : VOL_RMFILE,
	              (fdcp->c_cnid == kHFSRootFolderID));
	hfs_volupdate(hfsmp, fvp->v_type == VDIR ? VOL_MKDIR : VOL_MKFILE,
	              (tdcp->c_cnid == kHFSRootFolderID));

	VOP_UNLOCK(fvp, 0, p);
	fcp = NULL;
	fvp_locked = 0;
	/* All done with fvp. */  

	/* Update both parent directories. */
	tv = time;
	if (fdvp != tdvp) {
		tdcp->c_nlink++;
		tdcp->c_entries++;
		if (fdcp->c_nlink > 0)
			fdcp->c_nlink--;
		if (fdcp->c_entries > 0)
			fdcp->c_entries--;
		fdcp->c_flag |= C_CHANGE | C_UPDATE;
		(void) VOP_UPDATE(fdvp, &tv, &tv, 0);
	}
	tdcp->c_childhint = out_desc.cd_hint;	/* Cache directory's location */
	tdcp->c_flag |= C_CHANGE | C_UPDATE;
	(void) VOP_UPDATE(tdvp, &tv, &tv, 0);

out:
	if (started_tr) {
	    journal_end_transaction(hfsmp->jnl);
	}
	if (grabbed_lock) {
		hfs_global_shared_lock_release(hfsmp);
	}

	if (fvp_locked) {
		VOP_UNLOCK(fvp, 0, p);
	}
	if (fdvp_locked) {
		VOP_UNLOCK(fdvp, 0, p);
	}
	if (tdvp_locked) {
		VOP_UNLOCK(tdvp, 0, p);
	}
	if (tvp && (tvp != fvp)) {
		if (tvp != tdvp)
			VOP_UNLOCK(tvp, 0, p);
		vrele(tvp);
	}

	vrele(fvp);
	vrele(fdvp);
	vrele(tdvp);

	/* Now that we've dropped locks, see if we need to force recycle on the old
	 * destination's rsrc fork, preventing a leak of the rsrc fork's blocks.  Note that
	 * doing the ref/rele is in order to twiddle the VL_INACTIVE bit to the vnode's flags
	 * so that on the last vnode_put for this vnode, we will force vnop_inactive to be triggered.
	 */
	if ((recycle_rsrc) && (vnode_getwithvid(rvp, rsrc_vid) == 0)) {		
		vnode_ref(rvp);
		vnode_rele(rvp);
		vnode_recycle(rvp);
		vnode_put (rvp);
	}


	/* After tvp is removed the only acceptable error is EIO */
	if ((error == ENOSPC) && tvp_deleted)
		error = EIO;

	return (error);
}
>>>>>>> origin/10.2

	return (error);
}


/*
 * Make a directory.
 */
int
hfs_vnop_mkdir(struct vnop_mkdir_args *ap)
{
	/***** HACK ALERT ********/
	ap->a_cnp->cn_flags |= MAKEENTRY;
	return hfs_makenode(ap->a_dvp, ap->a_vpp, ap->a_cnp, ap->a_vap, ap->a_context);
}


/*
 * Create a symbolic link.
 */
int
hfs_vnop_symlink(struct vnop_symlink_args *ap)
{
<<<<<<< HEAD
	struct vnode **vpp = ap->a_vpp;
	struct vnode *dvp = ap->a_dvp;
	struct vnode *vp = NULL;
	struct cnode *cp = NULL;
=======
	register struct vnode *vp, **vpp = ap->a_vpp;
>>>>>>> origin/10.2
	struct hfsmount *hfsmp;
	struct filefork *fp;
	struct buf *bp = NULL;
	char *datap;
	int started_tr = 0;
	u_int32_t len;
	int error;

	/* HFS standard disks don't support symbolic links */
	if (VTOVCB(dvp)->vcbSigWord != kHFSPlusSigWord)
		return (ENOTSUP);

	/* Check for empty target name */
	if (ap->a_target[0] == 0)
		return (EINVAL);

	hfsmp = VTOHFS(dvp);
	len = strlen(ap->a_target);

	/* Check for free space */
	if (((u_int64_t)hfs_freeblks(hfsmp, 0) * (u_int64_t)hfsmp->blockSize) < len) {
		return (ENOSPC);
	}


	hfsmp = VTOHFS(ap->a_dvp);

	/* Create the vnode */
<<<<<<< HEAD
	ap->a_vap->va_mode |= S_IFLNK;
	if ((error = hfs_makenode(dvp, vpp, ap->a_cnp, ap->a_vap, ap->a_context))) {
		goto out;
	}
=======
	if ((error = hfs_makenode(S_IFLNK | ap->a_vap->va_mode,
							  ap->a_dvp, vpp, ap->a_cnp))) {
		return (error);
	}

>>>>>>> origin/10.2
	vp = *vpp;
	if ((error = hfs_lock(VTOC(vp), HFS_EXCLUSIVE_LOCK, HFS_LOCK_DEFAULT))) {
		goto out;
	}
	cp = VTOC(vp);
	fp = VTOF(vp);

<<<<<<< HEAD
	if (cp->c_flag & (C_NOEXISTS | C_DELETED)) {
	    goto out;
	}

#if QUOTA
	(void)hfs_getinoquota(cp);
#endif /* QUOTA */

	if ((error = hfs_start_transaction(hfsmp)) != 0) {
	    goto out;
	}
	started_tr = 1;

	/*
	 * Allocate space for the link.
	 *
	 * Since we're already inside a transaction,
	 *
	 * Don't need truncate lock since a symlink is treated as a system file.
	 */
	error = hfs_truncate(vp, len, IO_NOZEROFILL, 0, ap->a_context);

	/* On errors, remove the symlink file */
	if (error) {
		/*
		 * End the transaction so we don't re-take the cnode lock
		 * below while inside a transaction (lock order violation).
		 */
		hfs_end_transaction(hfsmp);

		/* hfs_removefile() requires holding the truncate lock */
		hfs_unlock(cp);
		hfs_lock_truncate(cp, HFS_EXCLUSIVE_LOCK, HFS_LOCK_DEFAULT);
		hfs_lock(cp, HFS_EXCLUSIVE_LOCK, HFS_LOCK_ALLOW_NOEXISTS);

		if (hfs_start_transaction(hfsmp) != 0) {
			started_tr = 0;
			hfs_unlock_truncate(cp, HFS_LOCK_DEFAULT);
			goto out;
		}
		
		(void) hfs_removefile(dvp, vp, ap->a_cnp, 0, 0, 0, NULL, 0);
		hfs_unlock_truncate(cp, HFS_LOCK_DEFAULT);
		goto out;	
	}
=======
#if QUOTA
	(void)hfs_getinoquota(VTOC(vp));
#endif /* QUOTA */

	// XXXdbg
	hfs_global_shared_lock_acquire(hfsmp);
	if (hfsmp->jnl) {
	    if ((error = journal_start_transaction(hfsmp->jnl)) != 0) {
			hfs_global_shared_lock_release(hfsmp);
			vput(vp);
			return error;
	    }
	}

	/* Allocate space for the link */
	error = VOP_TRUNCATE(vp, len, IO_NOZEROFILL,
	                      ap->a_cnp->cn_cred, ap->a_cnp->cn_proc);
	if (error)
		goto out;	/* XXX need to remove link */
>>>>>>> origin/10.2

	/* Write the link to disk */
	bp = buf_getblk(vp, (daddr64_t)0, roundup((int)fp->ff_size, hfsmp->hfs_physical_block_size),
			0, 0, BLK_META);
	if (hfsmp->jnl) {
		journal_modify_block_start(hfsmp->jnl, bp);
	}
<<<<<<< HEAD
	datap = (char *)buf_dataptr(bp);
	bzero(datap, buf_size(bp));
	bcopy(ap->a_target, datap, len);

	if (hfsmp->jnl) {
		journal_modify_block_end(hfsmp->jnl, bp, NULL, NULL);
	} else {
		buf_bawrite(bp);
	}
out:
	if (started_tr)
	    hfs_end_transaction(hfsmp);
	if ((cp != NULL) && (vp != NULL)) {
		hfs_unlock(cp);
	}
	if (error) {
		if (vp) {
			vnode_put(vp);
		}
		*vpp = NULL;
	}
=======
	bzero(bp->b_data, bp->b_bufsize);
	bcopy(ap->a_target, bp->b_data, len);
	if (hfsmp->jnl) {
		journal_modify_block_end(hfsmp->jnl, bp);
	} else {
		bawrite(bp);
	}
out:
	if (hfsmp->jnl) {
		journal_end_transaction(hfsmp->jnl);
	}
	hfs_global_shared_lock_release(hfsmp);
	vput(vp);
>>>>>>> origin/10.2
	return (error);
}


/* structures to hold a "." or ".." directory entry */
struct hfs_stddotentry {
	u_int32_t	d_fileno;   /* unique file number */
	u_int16_t	d_reclen;   /* length of this structure */
	u_int8_t	d_type;     /* dirent file type */
	u_int8_t	d_namlen;   /* len of filename */
	char		d_name[4];  /* "." or ".." */
};

struct hfs_extdotentry {
	u_int64_t  d_fileno;   /* unique file number */
	u_int64_t  d_seekoff;  /* seek offset (optional, used by servers) */
	u_int16_t  d_reclen;   /* length of this structure */
	u_int16_t  d_namlen;   /* len of filename */
	u_int8_t   d_type;     /* dirent file type */
	u_char     d_name[3];  /* "." or ".." */
};

typedef union {
	struct hfs_stddotentry  std;
	struct hfs_extdotentry  ext;
} hfs_dotentry_t;

/*
 *  hfs_vnop_readdir reads directory entries into the buffer pointed
 *  to by uio, in a filesystem independent format.  Up to uio_resid
 *  bytes of data can be transferred.  The data in the buffer is a
 *  series of packed dirent structures where each one contains the
 *  following entries:
 *
 *	u_int32_t   d_fileno;              // file number of entry
 *	u_int16_t   d_reclen;              // length of this record
 *	u_int8_t    d_type;                // file type
 *	u_int8_t    d_namlen;              // length of string in d_name
 *	char        d_name[MAXNAMELEN+1];  // null terminated file name
 *
 *  The current position (uio_offset) refers to the next block of
 *  entries.  The offset can only be set to a value previously
 *  returned by hfs_vnop_readdir or zero.  This offset does not have
 *  to match the number of bytes returned (in uio_resid).
 *
 *  In fact, the offset used by HFS is essentially an index (26 bits)
 *  with a tag (6 bits).  The tag is for associating the next request
  *  with the current request.  This enables us to have multiple threads
 *  reading the directory while the directory is also being modified.
 *
 *  Each tag/index pair is tied to a unique directory hint.  The hint
 *  contains information (filename) needed to build the catalog b-tree
 *  key for finding the next set of entries.
 *
 * If the directory is marked as deleted-but-in-use (cp->c_flag & C_DELETED),
 * do NOT synthesize entries for "." and "..".
 */
int
hfs_vnop_readdir(ap)
	struct vnop_readdir_args /* {
		vnode_t a_vp;
		uio_t a_uio;
		int a_flags;
		int *a_eofflag;
		int *a_numdirent;
		vfs_context_t a_context;
	} */ *ap;
{
	struct vnode *vp = ap->a_vp;
	uio_t uio = ap->a_uio;
	struct cnode *cp;
	struct hfsmount *hfsmp;
	directoryhint_t *dirhint = NULL;
	directoryhint_t localhint;
	off_t offset;
	off_t startoffset;
	int error = 0;
	int eofflag = 0;
<<<<<<< HEAD
	user_addr_t user_start = 0;
	user_size_t user_len = 0;
	int index;
	unsigned int tag;
	int items;
	int lockflags;
	int extended;
	int nfs_cookies;
	cnid_t cnid_hint = 0;
	int bump_valence = 0;

	items = 0;
	startoffset = offset = uio_offset(uio);
	extended = (ap->a_flags & VNODE_READDIR_EXTENDED);
	nfs_cookies = extended && (ap->a_flags & VNODE_READDIR_REQSEEKOFF);

	/* Sanity check the uio data. */
	if (uio_iovcnt(uio) > 1)
		return (EINVAL);

	if (VTOC(vp)->c_bsdflags & UF_COMPRESSED) {
		int compressed = hfs_file_is_compressed(VTOC(vp), 0);  /* 0 == take the cnode lock */
		if (VTOCMP(vp) != NULL && !compressed) {
			error = check_for_dataless_file(vp, NAMESPACE_HANDLER_READ_OP);
			if (error) {
				return error;
			}
		}
	}

	cp = VTOC(vp);
	hfsmp = VTOHFS(vp);

	/* Note that the dirhint calls require an exclusive lock. */
	if ((error = hfs_lock(VTOC(vp), HFS_EXCLUSIVE_LOCK, HFS_LOCK_DEFAULT)))
		return (error);

	/* Pick up cnid hint (if any). */
	if (nfs_cookies) {
		cnid_hint = (cnid_t)(uio_offset(uio) >> 32);
		uio_setoffset(uio, uio_offset(uio) & 0x00000000ffffffffLL);
		if (cnid_hint == INT_MAX) { /* searching pass the last item */
			eofflag = 1;
			goto out;
		}
	}
	/*
	 * Synthesize entries for "." and "..", unless the directory has
	 * been deleted, but not closed yet (lazy delete in progress).
	 */
	if (offset == 0 && !(cp->c_flag & C_DELETED)) {
		hfs_dotentry_t  dotentry[2];
		size_t  uiosize;

		if (extended) {
			struct hfs_extdotentry *entry = &dotentry[0].ext;

			entry->d_fileno = cp->c_cnid;
			entry->d_reclen = sizeof(struct hfs_extdotentry);
			entry->d_type = DT_DIR;
			entry->d_namlen = 1;
			entry->d_name[0] = '.';
			entry->d_name[1] = '\0';
			entry->d_name[2] = '\0';
			entry->d_seekoff = 1;

			++entry;
			entry->d_fileno = cp->c_parentcnid;
			entry->d_reclen = sizeof(struct hfs_extdotentry);
			entry->d_type = DT_DIR;
			entry->d_namlen = 2;
			entry->d_name[0] = '.';
			entry->d_name[1] = '.';
			entry->d_name[2] = '\0';
			entry->d_seekoff = 2;
			uiosize = 2 * sizeof(struct hfs_extdotentry);
=======
	void *user_start = NULL;
	int   user_len;
 
	/* We assume it's all one big buffer... */
	if (uio->uio_iovcnt > 1 || uio->uio_resid < AVERAGE_HFSDIRENTRY_SIZE)
		return EINVAL;

	// XXXdbg
	// We have to lock the user's buffer here so that we won't
	// fault on it after we've acquired a shared lock on the
	// catalog file.  The issue is that you can get a 3-way
	// deadlock if someone else starts a transaction and then
	// tries to lock the catalog file but can't because we're
	// here and we can't service our page fault because VM is
	// blocked trying to start a transaction as a result of
	// trying to free up pages for our page fault.  It's messy
	// but it does happen on dual-procesors that are paging
	// heavily (see radar 3082639 for more info).  By locking
	// the buffer up-front we prevent ourselves from faulting
	// while holding the shared catalog file lock.
	//
	// Fortunately this and hfs_search() are the only two places
	// currently (10/30/02) that can fault on user data with a
	// shared lock on the catalog file.
	//
	if (hfsmp->jnl && uio->uio_segflg == UIO_USERSPACE) {
		user_start = uio->uio_iov->iov_base;
		user_len   = uio->uio_iov->iov_len;

		if ((retval = vslock(user_start, user_len)) != 0) {
			return retval;
		}
	}


	/* Create the entries for . and .. */
	if (uio->uio_offset < sizeof(rootdots)) {
		caddr_t dep;
		size_t dotsize;
		
		rootdots[0].d_fileno = cp->c_cnid;
		rootdots[1].d_fileno = cp->c_parentcnid;

		if (uio->uio_offset == 0) {
			dep = (caddr_t) &rootdots[0];
			dotsize = 2* sizeof(struct hfsdotentry);
		} else if (uio->uio_offset == sizeof(struct hfsdotentry)) {
			dep = (caddr_t) &rootdots[1];
			dotsize = sizeof(struct hfsdotentry);
>>>>>>> origin/10.2
		} else {
			struct hfs_stddotentry *entry = &dotentry[0].std;

			entry->d_fileno = cp->c_cnid;
			entry->d_reclen = sizeof(struct hfs_stddotentry);
			entry->d_type = DT_DIR;
			entry->d_namlen = 1;
			*(int *)&entry->d_name[0] = 0;
			entry->d_name[0] = '.';

			++entry;
			entry->d_fileno = cp->c_parentcnid;
			entry->d_reclen = sizeof(struct hfs_stddotentry);
			entry->d_type = DT_DIR;
			entry->d_namlen = 2;
			*(int *)&entry->d_name[0] = 0;
			entry->d_name[0] = '.';
			entry->d_name[1] = '.';
			uiosize = 2 * sizeof(struct hfs_stddotentry);
		}
		if ((error = uiomove((caddr_t)&dotentry, uiosize, uio))) {
			goto out;
		}
		offset += 2;
	}

	/* 
	 * Intentionally avoid checking the valence here.  If we
	 * have FS corruption that reports the valence is 0, even though it
	 * has contents, we might artificially skip over iterating 
	 * this directory.
	 */
	
	//
	// We have to lock the user's buffer here so that we won't
	// fault on it after we've acquired a shared lock on the
	// catalog file.  The issue is that you can get a 3-way
	// deadlock if someone else starts a transaction and then
	// tries to lock the catalog file but can't because we're
	// here and we can't service our page fault because VM is
	// blocked trying to start a transaction as a result of
	// trying to free up pages for our page fault.  It's messy
	// but it does happen on dual-processors that are paging
	// heavily (see radar 3082639 for more info).  By locking
	// the buffer up-front we prevent ourselves from faulting
	// while holding the shared catalog file lock.
	//
	// Fortunately this and hfs_search() are the only two places
	// currently (10/30/02) that can fault on user data with a
	// shared lock on the catalog file.
	//
	if (hfsmp->jnl && uio_isuserspace(uio)) {
		user_start = uio_curriovbase(uio);
		user_len = uio_curriovlen(uio);

		if ((error = vslock(user_start, user_len)) != 0) {
			user_start = 0;
			goto out;
		}
	}
	/* Convert offset into a catalog directory index. */
	index = (offset & HFS_INDEX_MASK) - 2;
	tag = offset & ~HFS_INDEX_MASK;

	/* Lock catalog during cat_findname and cat_getdirentries. */
	lockflags = hfs_systemfile_lock(hfsmp, SFL_CATALOG, HFS_SHARED_LOCK);

	/* When called from NFS, try and resolve a cnid hint. */
	if (nfs_cookies && cnid_hint != 0) {
		if (cat_findname(hfsmp, cnid_hint, &localhint.dh_desc) == 0) {
			if ( localhint.dh_desc.cd_parentcnid == cp->c_fileid) {
				localhint.dh_index = index - 1;
				localhint.dh_time = 0;
				bzero(&localhint.dh_link, sizeof(localhint.dh_link));
				dirhint = &localhint;  /* don't forget to release the descriptor */
			} else {
				cat_releasedesc(&localhint.dh_desc);
			}
		}
	}

	/* Get a directory hint (cnode must be locked exclusive) */
	if (dirhint == NULL) {
		dirhint = hfs_getdirhint(cp, ((index - 1) & HFS_INDEX_MASK) | tag, 0);

		/* Hide tag from catalog layer. */
		dirhint->dh_index &= HFS_INDEX_MASK;
		if (dirhint->dh_index == HFS_INDEX_MASK) {
			dirhint->dh_index = -1;
		}
	}
	
	if (index == 0) {
		dirhint->dh_threadhint = cp->c_dirthreadhint;
	} 
	else {
		/*
		 * If we have a non-zero index, there is a possibility that during the last
		 * call to hfs_vnop_readdir we hit EOF for this directory.  If that is the case
		 * then we don't want to return any new entries for the caller.  Just return 0
		 * items, mark the eofflag, and bail out.  Because we won't have done any work, the 
		 * code at the end of the function will release the dirhint for us.  
		 *
		 * Don't forget to unlock the catalog lock on the way out, too.
		 */
		if (dirhint->dh_desc.cd_flags & CD_EOF) {
			error = 0;
			eofflag = 1;
			uio_setoffset(uio, startoffset);
			hfs_systemfile_unlock (hfsmp, lockflags);

			goto seekoffcalc;
		}
	}
	else {
		/*
		 * If we have a non-zero index, there is a possibility that during the last
		 * call to hfs_vnop_readdir we hit EOF for this directory.  If that is the case
		 * then we don't want to return any new entries for the caller.  Just return 0
		 * items, mark the eofflag, and bail out.  Because we won't have done any work, the 
		 * code at the end of the function will release the dirhint for us.  
		 *
		 * Don't forget to unlock the catalog lock on the way out, too.
		 */
		if (dirhint->dh_desc.cd_flags & CD_EOF) {
			error = 0;
			eofflag = 1;
			uio_setoffset(uio, startoffset);
			hfs_systemfile_unlock (hfsmp, lockflags);

			goto seekoffcalc;
		}
	}

<<<<<<< HEAD
	/* Pack the buffer with dirent entries. */
	error = cat_getdirentries(hfsmp, cp->c_entries, dirhint, uio, ap->a_flags, &items, &eofflag);

	if (index == 0 && error == 0) {
		cp->c_dirthreadhint = dirhint->dh_threadhint;
=======
		*ap->a_ncookies = ncookies;
		*ap->a_cookies = cookies;
		
		/* handle cookies for "." and ".." */
		if (off == 0) {
			cookies[0] = 0;
			cookies[1] = sizeof(struct hfsdotentry);
		} else if (off == sizeof(struct hfsdotentry)) {
			cookies[0] = sizeof(struct hfsdotentry);
		}
>>>>>>> origin/10.3
	}

	hfs_systemfile_unlock(hfsmp, lockflags);

	if (error != 0) {
		goto out;
	}
	
	/* Get index to the next item */
	index += items;
	
	if (items >= (int)cp->c_entries) {
		eofflag = 1;
	}
	
	/* 
	 * Detect valence FS corruption.
	 *
	 * We are holding the cnode lock exclusive, so there should not be
	 * anybody modifying the valence field of this cnode.  If we enter
	 * this block, that means we observed filesystem corruption, because
	 * this directory reported a valence of 0, yet we found at least one
	 * item.  In this case, we need to minimally self-heal this
	 * directory to prevent userland from tripping over a directory
	 * that appears empty (getattr of valence reports 0), but actually 
	 * has contents. 
	 *
	 * We'll force the cnode update at the end of the function after 
	 * completing all of the normal getdirentries steps.
	 */ 
	if ((cp->c_entries == 0) && (items > 0)) {
		/* disk corruption */
		cp->c_entries++;
		/* Mark the cnode as dirty. */
		cp->c_flag |= C_MODIFIED;
		printf("hfs_vnop_readdir: repairing valence to non-zero! \n");
		bump_valence++;
	}


	/* Convert catalog directory index back into an offset. */
	while (tag == 0)
		tag = (++cp->c_dirhinttag) << HFS_INDEX_BITS;	
	uio_setoffset(uio, (index + 2) | tag);
	dirhint->dh_index |= tag;

seekoffcalc:
	cp->c_touch_acctime = TRUE;

	if (ap->a_numdirent) {
		if (startoffset == 0)
			items += 2;
		*ap->a_numdirent = items;
	}

out:
	if (user_start) {
		vsunlock(user_start, user_len, TRUE);
	}
	/* If we didn't do anything then go ahead and dump the hint. */
	if ((dirhint != NULL) &&
	    (dirhint != &localhint) &&
	    (uio_offset(uio) == startoffset)) {
		hfs_reldirhint(cp, dirhint);
		eofflag = 1;
	}
<<<<<<< HEAD
	if (ap->a_eofflag) {
=======
	
	cp->c_flag |= C_ACCESS;
															/* Bake any cookies */
	if (!retval && ap->a_ncookies != NULL) {
		struct dirent* dpStart;
		struct dirent* dpEnd;
		struct dirent* dp;
		int ncookies;
		u_long *cookies;
		u_long *cookiep;

		/*
		 * Only the NFS server uses cookies, and it loads the
		 * directory block into system space, so we can just look at
		 * it directly.
		 */
		if (uio->uio_segflg != UIO_SYSSPACE)
			panic("hfs_readdir: unexpected uio from NFS server");
		dpStart = (struct dirent *)(uio->uio_iov->iov_base - (uio->uio_offset - off));
		dpEnd = (struct dirent *) uio->uio_iov->iov_base;
		for (dp = dpStart, ncookies = 0;
		     dp < dpEnd && dp->d_reclen != 0;
		     dp = (struct dirent *)((caddr_t)dp + dp->d_reclen))
			ncookies++;
		MALLOC(cookies, u_long *, ncookies * sizeof(u_long), M_TEMP, M_WAITOK);
		for (dp = dpStart, cookiep = cookies;
		     dp < dpEnd;
		     dp = (struct dirent *)((caddr_t) dp + dp->d_reclen)) {
			off += dp->d_reclen;
			*cookiep++ = (u_long) off;
		}
		*ap->a_ncookies = ncookies;
		*ap->a_cookies = cookies;
	}

Exit:;
	if (hfsmp->jnl && user_start) {
		vsunlock(user_start, user_len, TRUE);
	}

	if (ap->a_eofflag)
>>>>>>> origin/10.2
		*ap->a_eofflag = eofflag;
	}
	if (dirhint == &localhint) {
		cat_releasedesc(&localhint.dh_desc);
	}

	if (bump_valence) {
		/* force the update before dropping the cnode lock*/
		hfs_update(vp, 0);
	}

	hfs_unlock(cp);
	
	return (error);
}


/*
 * Read contents of a symbolic link.
 */
int
hfs_vnop_readlink(ap)
	struct vnop_readlink_args /* {
		struct vnode *a_vp;
		struct uio *a_uio;
		vfs_context_t a_context;
	} */ *ap;
{
	struct vnode *vp = ap->a_vp;
	struct cnode *cp;
	struct filefork *fp;
	int error;

	if (!vnode_islnk(vp))
		return (EINVAL);
 
	if ((error = hfs_lock(VTOC(vp), HFS_EXCLUSIVE_LOCK, HFS_LOCK_DEFAULT)))
		return (error);
	cp = VTOC(vp);
	fp = VTOF(vp);
   
	/* Zero length sym links are not allowed */
	if (fp->ff_size == 0 || fp->ff_size > MAXPATHLEN) {
		error = EINVAL;
		goto exit;
	}
    
	/* Cache the path so we don't waste buffer cache resources */
	if (fp->ff_symlinkptr == NULL) {
		struct buf *bp = NULL;

		MALLOC(fp->ff_symlinkptr, char *, fp->ff_size, M_TEMP, M_WAITOK);
		if (fp->ff_symlinkptr == NULL) {
			error = ENOMEM;
			goto exit;
		}
		error = (int)buf_meta_bread(vp, (daddr64_t)0,
		                            roundup((int)fp->ff_size, VTOHFS(vp)->hfs_physical_block_size),
		                            vfs_context_ucred(ap->a_context), &bp);
		if (error) {
			if (bp)
				buf_brelse(bp);
			if (fp->ff_symlinkptr) {
				FREE(fp->ff_symlinkptr, M_TEMP);
				fp->ff_symlinkptr = NULL;
			}
			goto exit;
		}
<<<<<<< HEAD
		bcopy((char *)buf_dataptr(bp), fp->ff_symlinkptr, (size_t)fp->ff_size);

		if (VTOHFS(vp)->jnl && (buf_flags(bp) & B_LOCKED) == 0) {
		        buf_markinvalid(bp);		/* data no longer needed */
=======
		bcopy(bp->b_data, fp->ff_symlinkptr, (size_t)fp->ff_size);
		if (bp) {
			if (VTOHFS(vp)->jnl && (bp->b_flags & B_LOCKED) == 0) {
				bp->b_flags |= B_INVAL;		/* data no longer needed */
			}
			brelse(bp);
>>>>>>> origin/10.2
		}
		buf_brelse(bp);
	}
<<<<<<< HEAD
	error = uiomove((caddr_t)fp->ff_symlinkptr, (int)fp->ff_size, ap->a_uio);
=======
	retval = uiomove((caddr_t)fp->ff_symlinkptr, (int)fp->ff_size, ap->a_uio);

	return (retval);
}


/*
 * hfs abort op, called after namei() when a CREATE/DELETE isn't actually
 * done. If a buffer has been saved in anticipation of a CREATE, delete it.
#% abortop	dvp	= = =
#
 vop_abortop {
     IN struct vnode *dvp;
     IN struct componentname *cnp;

     */

/* ARGSUSED */

static int
hfs_abortop(ap)
	struct vop_abortop_args /* {
		struct vnode *a_dvp;
		struct componentname *a_cnp;
	} */ *ap;
{
	if ((ap->a_cnp->cn_flags & (HASBUF | SAVESTART)) == HASBUF) {
		FREE_ZONE(ap->a_cnp->cn_pnbuf, ap->a_cnp->cn_pnlen, M_NAMEI);
		ap->a_cnp->cn_flags &= ~HASBUF;
	}

	return (0);
}


/*
 * Lock an cnode. If its already locked, set the WANT bit and sleep.
#% lock		vp	U L U
#
 vop_lock {
     IN struct vnode *vp;
     IN int flags;
     IN struct proc *p;
     */

static int
hfs_lock(ap)
	struct vop_lock_args /* {
		struct vnode *a_vp;
		int a_flags;
		struct proc *a_p;
	} */ *ap;
{
	struct vnode *vp = ap->a_vp;
	struct cnode *cp = VTOC(vp);

	if (cp == NULL)
		panic("hfs_lock: cnode in vnode is null\n");

	return (lockmgr(&cp->c_lock, ap->a_flags, &vp->v_interlock, ap->a_p));
}

/*
 * Unlock an cnode.
#% unlock	vp	L U L
#
 vop_unlock {
     IN struct vnode *vp;
     IN int flags;
     IN struct proc *p;

     */
static int
hfs_unlock(ap)
	struct vop_unlock_args /* {
		struct vnode *a_vp;
		int a_flags;
		struct proc *a_p;
	} */ *ap;
{
	struct vnode *vp = ap->a_vp;
	struct cnode *cp = VTOC(vp);

	if (cp == NULL)
		panic("hfs_unlock: cnode in vnode is null\n");

	return (lockmgr(&cp->c_lock, ap->a_flags | LK_RELEASE,
		&vp->v_interlock, ap->a_p));
}

>>>>>>> origin/10.2

	/*
	 * Keep track blocks read
	 */
	if ((VTOHFS(vp)->hfc_stage == HFC_RECORDING) && (error == 0)) {
		
		/*
		 * If this file hasn't been seen since the start of
		 * the current sampling period then start over.
		 */
		if (cp->c_atime < VTOHFS(vp)->hfc_timebase)
			VTOF(vp)->ff_bytesread = fp->ff_size;
		else
			VTOF(vp)->ff_bytesread += fp->ff_size;
		
	//	if (VTOF(vp)->ff_bytesread > fp->ff_size)
	//		cp->c_touch_acctime = TRUE;
	}

exit:
	hfs_unlock(cp);
	return (error);
}


/*
 * Get configurable pathname variables.
 */
int
hfs_vnop_pathconf(ap)
	struct vnop_pathconf_args /* {
		struct vnode *a_vp;
		int a_name;
		int *a_retval;
		vfs_context_t a_context;
	} */ *ap;
{

	int std_hfs = (VTOHFS(ap->a_vp)->hfs_flags & HFS_STANDARD);
	switch (ap->a_name) {
	case _PC_LINK_MAX:
		if (std_hfs == 0){
			*ap->a_retval = HFS_LINK_MAX;
		}
#if CONFIG_HFS_STD
		else {
			*ap->a_retval = 1;
		}
#endif
		break;
	case _PC_NAME_MAX:
		if (std_hfs == 0) {
			*ap->a_retval = kHFSPlusMaxFileNameChars;  /* 255 */
		}
#if CONFIG_HFS_STD
		else {
			*ap->a_retval = kHFSMaxFileNameChars;  /* 31 */
		}
#endif
		break;
	case _PC_PATH_MAX:
		*ap->a_retval = PATH_MAX;  /* 1024 */
		break;
	case _PC_PIPE_BUF:
		*ap->a_retval = PIPE_BUF;
		break;
	case _PC_CHOWN_RESTRICTED:
		*ap->a_retval = 200112;		/* _POSIX_CHOWN_RESTRICTED */
		break;
	case _PC_NO_TRUNC:
		*ap->a_retval = 200112;		/* _POSIX_NO_TRUNC */
		break;
	case _PC_NAME_CHARS_MAX:
		if (std_hfs == 0) {
			*ap->a_retval = kHFSPlusMaxFileNameChars; /* 255 */
		}
#if CONFIG_HFS_STD
		else {
			*ap->a_retval = kHFSMaxFileNameChars; /* 31 */
		}
#endif
		break;
	case _PC_CASE_SENSITIVE:
		if (VTOHFS(ap->a_vp)->hfs_flags & HFS_CASE_SENSITIVE)
			*ap->a_retval = 1;
		else
			*ap->a_retval = 0;
		break;
	case _PC_CASE_PRESERVING:
		*ap->a_retval = 1;
		break;
	case _PC_FILESIZEBITS:
		/* number of bits to store max file size */
		if (std_hfs == 0) {
			*ap->a_retval = 64;	
		}
#if CONFIG_HFS_STD
		else {
			*ap->a_retval = 32;
		}
#endif
		break;
	case _PC_XATTR_SIZE_BITS:
		/* Number of bits to store maximum extended attribute size */
		*ap->a_retval = HFS_XATTR_SIZE_BITS;
		break;
	default:
		return (EINVAL);
	}

	return (0);
}

/*
 * Prepares a fork for cat_update by making sure ff_size and ff_blocks
 * are no bigger than the valid data on disk thus reducing the chance
 * of exposing uninitialised data in the event of a non clean unmount.
 * fork_buf is where to put the temporary copy if required.  (It can
 * be inside pfork.)
 */
const struct cat_fork *
hfs_prepare_fork_for_update(filefork_t *ff,
							const struct cat_fork *cf,
							struct cat_fork *cf_buf,
							uint32_t block_size)
{
	if (!ff)
		return NULL;

	if (!cf)
		cf = &ff->ff_data;
	if (!cf_buf)
		cf_buf = &ff->ff_data;

	off_t max_size = ff->ff_size;

	// Check first invalid range
	if (!TAILQ_EMPTY(&ff->ff_invalidranges))
		max_size = TAILQ_FIRST(&ff->ff_invalidranges)->rl_start;

	if (!ff->ff_unallocblocks && ff->ff_size <= max_size)
		return cf; // Nothing to do

	if (ff->ff_blocks < ff->ff_unallocblocks) {
		panic("hfs: ff_blocks %d is less than unalloc blocks %d\n",
			  ff->ff_blocks, ff->ff_unallocblocks);
	}

	struct cat_fork *out = cf_buf;

	if (out != cf)
		bcopy(cf, out, sizeof(*cf));

	// Adjust cf_blocks for cf_vblocks
	out->cf_blocks -= out->cf_vblocks;

	/*
	 * Here we trim the size with the updated cf_blocks.  This is
	 * probably unnecessary now because the invalid ranges should
	 * catch this (but that wasn't always the case).
	 */
	off_t alloc_bytes = hfs_blk_to_bytes(out->cf_blocks, block_size);
	if (out->cf_size > alloc_bytes)
		out->cf_size = alloc_bytes;

	// Trim cf_size to first invalid range
	if (out->cf_size > max_size)
		out->cf_size = max_size;

	return out;
}

/*
 * Update a cnode's on-disk metadata.
 *
 * The cnode must be locked exclusive.  See declaration for possible
 * options.
 */
int
hfs_update(struct vnode *vp, int options)
{
	struct cnode *cp = VTOC(vp);
	struct proc *p;
	const struct cat_fork *dataforkp = NULL;
	const struct cat_fork *rsrcforkp = NULL;
	struct cat_fork datafork;
<<<<<<< HEAD
<<<<<<< HEAD
=======
>>>>>>> origin/10.5
	struct cat_fork rsrcfork;
	struct hfsmount *hfsmp;
	int lockflags;
=======
	int updateflag;
	struct hfsmount *hfsmp;
>>>>>>> origin/10.2
	int error;
	uint32_t tstate = 0;

<<<<<<< HEAD
	if (ISSET(cp->c_flag, C_NOEXISTS))
		return 0;
=======
	hfsmp = VTOHFS(vp);

	/* XXX do we really want to clear the sytem cnode flags here???? */
	if ((vp->v_flag & VSYSTEM) ||
	    (VTOVFS(vp)->mnt_flag & MNT_RDONLY) ||
	    (cp->c_mode == 0)) {
		cp->c_flag &= ~(C_ACCESS | C_CHANGE | C_MODIFIED | C_UPDATE);
		return (0);
	}
>>>>>>> origin/10.2

	p = current_proc();
	hfsmp = VTOHFS(vp);

<<<<<<< HEAD
	if (((vnode_issystem(vp) && (cp->c_cnid < kHFSFirstUserCatalogNodeID))) || 
	   	hfsmp->hfs_catalog_vp == NULL){
		return (0);
	}
	if ((hfsmp->hfs_flags & HFS_READ_ONLY) || (cp->c_mode == 0)) {
		CLR(cp->c_flag, C_MODIFIED | C_MINOR_MOD | C_NEEDS_DATEADDED);
		cp->c_touch_acctime = 0;
		cp->c_touch_chgtime = 0;
		cp->c_touch_modtime = 0;
		return (0);
=======
	/* Nothing to update. */
	if (updateflag == 0) {
		return (0);
	}
	/* HFS standard doesn't have access times. */
	if ((updateflag == C_ACCESS) && (VTOVCB(vp)->vcbSigWord == kHFSSigWord)) {
		return (0);
	}
	if (updateflag & C_ACCESS) {
		/*
		 * If only the access time is changing then defer
		 * updating it on-disk util later (in hfs_inactive).
		 * If it was recently updated then skip the update.
		 */
		if (updateflag == C_ACCESS) {
			cp->c_flag &= ~C_ACCESS;
	
			/* Its going to disk or its sufficiently newer... */
			if ((cp->c_flag & C_ATIMEMOD) ||
			    (ap->a_access->tv_sec > (cp->c_atime + ATIME_ACCURACY))) {
				cp->c_atime = ap->a_access->tv_sec;
				cp->c_flag |= C_ATIMEMOD;
			}
			return (0);
		} else {
			cp->c_atime = ap->a_access->tv_sec;
		}
>>>>>>> origin/10.2
	}
	if (kdebug_enable) {
		if (cp->c_touch_acctime || cp->c_atime != cp->c_attr.ca_atimeondisk)
			tstate |= DBG_HFS_UPDATE_ACCTIME;
		if (cp->c_touch_modtime)
			tstate |= DBG_HFS_UPDATE_MODTIME;
		if (cp->c_touch_chgtime)
			tstate |= DBG_HFS_UPDATE_CHGTIME;

		if (cp->c_flag & C_MODIFIED)
			tstate |= DBG_HFS_UPDATE_MODIFIED;
		if (ISSET(options, HFS_UPDATE_FORCE))
			tstate |= DBG_HFS_UPDATE_FORCE;
		if (cp->c_flag & C_NEEDS_DATEADDED)
			tstate |= DBG_HFS_UPDATE_DATEADDED;
		if (cp->c_flag & C_MINOR_MOD)
			tstate |= DBG_HFS_UPDATE_MINOR;
	}
	hfs_touchtimes(hfsmp, cp);

	if (!ISSET(cp->c_flag, C_MODIFIED | C_MINOR_MOD)
		&& !hfs_should_save_atime(cp)) {
		// Nothing to update
		return 0;
	}

	KDBG(HFSDBG_UPDATE | DBG_FUNC_START, VM_KERNEL_ADDRPERM(vp), tstate);

	bool check_txn = false;

	if (!ISSET(options, HFS_UPDATE_FORCE) && !ISSET(cp->c_flag, C_MODIFIED)) {
		/*
		 * This must be a minor modification.  If the current
		 * transaction already has an update for this node, then we
		 * bundle in the modification.
		 */
		if (hfsmp->jnl
			&& journal_current_txn(hfsmp->jnl) == cp->c_update_txn) {
			check_txn = true;
		} else {
			tstate |= DBG_HFS_UPDATE_SKIPPED;
			error = 0;
			goto exit;
		}
	}

	if ((error = hfs_start_transaction(hfsmp)) != 0)
		goto exit;

	if (check_txn
		&& journal_current_txn(hfsmp->jnl) != cp->c_update_txn) {
		hfs_end_transaction(hfsmp);
		tstate |= DBG_HFS_UPDATE_SKIPPED;
		error = 0;
		goto exit;
	}

	if (cp->c_datafork)
		dataforkp = &cp->c_datafork->ff_data;
	if (cp->c_rsrcfork)
		rsrcforkp = &cp->c_rsrcfork->ff_data;

    /*
     * Modify the values passed to cat_update based on whether or not
     * the file has invalid ranges or borrowed blocks.
     */
    dataforkp = hfs_prepare_fork_for_update(cp->c_datafork, NULL, &datafork, hfsmp->blockSize);
	rsrcforkp = hfs_prepare_fork_for_update(cp->c_rsrcfork, NULL, &rsrcfork, hfsmp->blockSize);

<<<<<<< HEAD
	if (__improbable(kdebug_enable & KDEBUG_TRACE)) {
		long dbg_parms[NUMPARMS];
		int  dbg_namelen;

		dbg_namelen = NUMPARMS * sizeof(long);
		vn_getpath(vp, (char *)dbg_parms, &dbg_namelen);

		if (dbg_namelen < (int)sizeof(dbg_parms))
			memset((char *)dbg_parms + dbg_namelen, 0, sizeof(dbg_parms) - dbg_namelen);

		kdebug_lookup_gen_events(dbg_parms, dbg_namelen, (void *)vp, TRUE);
=======
	/*
	 * For delayed allocations updates are
	 * postponed until an fsync or the file
	 * gets written to disk.
	 *
	 * Deleted files can defer meta data updates until inactive.
	 */
	if (ISSET(cp->c_flag, C_DELETED) ||
	    (dataforkp && cp->c_datafork->ff_unallocblocks) ||
	    (rsrcforkp && cp->c_rsrcfork->ff_unallocblocks)) {
		if (updateflag & (C_CHANGE | C_UPDATE))
			hfs_volupdate(hfsmp, VOL_UPDATE, 0);	
		cp->c_flag &= ~(C_ACCESS | C_CHANGE | C_UPDATE);
		cp->c_flag |= C_MODIFIED;

		return (0);
	}


	// XXXdbg
	hfs_global_shared_lock_acquire(hfsmp);
	if (hfsmp->jnl) {
		if ((error = journal_start_transaction(hfsmp->jnl)) != 0) {
			hfs_global_shared_lock_release(hfsmp);
			return error;
	    }
	}
			

	/*
	 * For files with invalid ranges (holes) the on-disk
	 * field representing the size of the file (cf_size)
	 * must be no larger than the start of the first hole.
	 */
	if (dataforkp && !CIRCLEQ_EMPTY(&cp->c_datafork->ff_invalidranges)) {
		bcopy(dataforkp, &datafork, sizeof(datafork));
		datafork.cf_size = CIRCLEQ_FIRST(&cp->c_datafork->ff_invalidranges)->rl_start;
		dataforkp = &datafork;
>>>>>>> origin/10.2
	}

	/*
	 * For resource forks with delayed allocations, make sure
	 * the block count and file size match the number of blocks
	 * actually allocated to the file on disk.
	 */
	if (rsrcforkp && (cp->c_rsrcfork->ff_unallocblocks != 0)) {
		bcopy(rsrcforkp, &rsrcfork, sizeof(rsrcfork));
		rsrcfork.cf_blocks = (cp->c_rsrcfork->ff_blocks - cp->c_rsrcfork->ff_unallocblocks);
		rsrcfork.cf_size   = rsrcfork.cf_blocks * HFSTOVCB(hfsmp)->blockSize;
		rsrcforkp = &rsrcfork;
	}

	/*
	 * Lock the Catalog b-tree file.
	 */
<<<<<<< HEAD
	lockflags = hfs_systemfile_lock(hfsmp, SFL_CATALOG, HFS_EXCLUSIVE_LOCK);

	error = cat_update(hfsmp, &cp->c_desc, &cp->c_attr, dataforkp, rsrcforkp);

	if (hfsmp->jnl)
		cp->c_update_txn = journal_current_txn(hfsmp->jnl);

	hfs_systemfile_unlock(hfsmp, lockflags);
=======
	error = hfs_metafilelocking(hfsmp, kHFSCatalogFileID, LK_SHARED, p);
	if (error) {
		if (hfsmp->jnl) {
			journal_end_transaction(hfsmp->jnl);
		}
		hfs_global_shared_lock_release(hfsmp);
		return (error);
	}

	/* XXX - waitfor is not enforced */
	error = cat_update(hfsmp, &cp->c_desc, &cp->c_attr, dataforkp, rsrcforkp);

	 /* Unlock the Catalog b-tree file. */
	(void) hfs_metafilelocking(hfsmp, kHFSCatalogFileID, LK_RELEASE, p);

	if (updateflag & (C_CHANGE | C_UPDATE))
		hfs_volupdate(hfsmp, VOL_UPDATE, 0);	

	// XXXdbg
	if (hfsmp->jnl) {
	    journal_end_transaction(hfsmp->jnl);
	}
	hfs_global_shared_lock_release(hfsmp);
>>>>>>> origin/10.2

	CLR(cp->c_flag, C_MODIFIED | C_MINOR_MOD);

	hfs_end_transaction(hfsmp);

exit:

	KDBG(HFSDBG_UPDATE | DBG_FUNC_END, VM_KERNEL_ADDRPERM(vp), tstate, error);

	return error;
}

/*
 * Allocate a new node
 */
int
hfs_makenode(struct vnode *dvp, struct vnode **vpp, struct componentname *cnp,
             struct vnode_attr *vap, vfs_context_t ctx)
{
	struct cnode *cp = NULL;
	struct cnode *dcp = NULL;
	struct vnode *tvp;
	struct hfsmount *hfsmp;
	struct cat_desc in_desc, out_desc;
	struct cat_attr attr;
<<<<<<< HEAD
	struct timeval tv;
	int lockflags;
	int error, started_tr = 0;
=======
	int error, started_tr = 0, grabbed_lock = 0;
>>>>>>> origin/10.2
	enum vtype vnodetype;
	int mode;
	int newvnode_flags = 0;
	u_int32_t gnv_flags = 0;
	int protectable_target = 0;
	int nocache = 0;

#if CONFIG_PROTECT
	struct cprotect *entry = NULL;
	int32_t cp_class = -1;

	/* 
	 * By default, it's OK for AKS to overrride our target class preferences.
	 */
	uint32_t keywrap_flags = CP_KEYWRAP_DIFFCLASS;

	if (VATTR_IS_ACTIVE(vap, va_dataprotect_class)) {
		cp_class = (int32_t)vap->va_dataprotect_class;
		/* 
		 * Since the user specifically requested this target class be used,
		 * we want to fail this creation operation if we cannot wrap to their 
		 * target class. The CP_KEYWRAP_DIFFCLASS bit says that it is OK to
		 * use a different class than the one specified, so we turn that off
		 * now.
		 */
		keywrap_flags &= ~CP_KEYWRAP_DIFFCLASS;
	}
	int protected_mount = 0;	
#endif


	if ((error = hfs_lock(VTOC(dvp), HFS_EXCLUSIVE_LOCK, HFS_LOCK_DEFAULT)))
		return (error);

	/* set the cnode pointer only after successfully acquiring lock */
	dcp = VTOC(dvp);
<<<<<<< HEAD

	/* Don't allow creation of new entries in open-unlinked directories */
	if ((error = hfs_checkdeleted(dcp))) {
		hfs_unlock(dcp);
=======
	
	/* Don't allow creation of new entries in open-unlinked directories */
	if ((error = hfs_checkdeleted (dcp))) {
		hfs_unlock (dcp);
>>>>>>> origin/10.6
		return error;
	}

	dcp->c_flag |= C_DIR_MODIFICATION;

	hfsmp = VTOHFS(dvp);

	*vpp = NULL;
	tvp = NULL;
	out_desc.cd_flags = 0;
	out_desc.cd_nameptr = NULL;

	vnodetype = vap->va_type;
	if (vnodetype == VNON)
		vnodetype = VREG;
	mode = MAKEIMODE(vnodetype, vap->va_mode);

	if (S_ISDIR (mode) || S_ISREG (mode)) {
		protectable_target = 1;
	}
	

	/* Check if were out of usable disk space. */
	if ((hfs_freeblks(hfsmp, 1) == 0) && (vfs_context_suser(ctx) != 0)) {
		error = ENOSPC;
		goto exit;
	}

	microtime(&tv);

	/* Setup the default attributes */
	bzero(&attr, sizeof(attr));
	attr.ca_mode = mode;
	attr.ca_linkcount = 1;
	if (VATTR_IS_ACTIVE(vap, va_rdev)) {
		attr.ca_rdev = vap->va_rdev;
	}
	if (VATTR_IS_ACTIVE(vap, va_create_time)) {
		VATTR_SET_SUPPORTED(vap, va_create_time);
		attr.ca_itime = vap->va_create_time.tv_sec;
	} else {
		attr.ca_itime = tv.tv_sec;
	}
#if CONFIG_HFS_STD
	if ((hfsmp->hfs_flags & HFS_STANDARD) && gTimeZone.tz_dsttime) {
		attr.ca_itime += 3600;	/* Same as what hfs_update does */
	}
#endif
	attr.ca_atime = attr.ca_ctime = attr.ca_mtime = attr.ca_itime;
	attr.ca_atimeondisk = attr.ca_atime;
	if (VATTR_IS_ACTIVE(vap, va_flags)) {
		VATTR_SET_SUPPORTED(vap, va_flags);
		attr.ca_flags = vap->va_flags;
	}
	
	/* 
	 * HFS+ only: all files get ThreadExists
	 * HFSX only: dirs get HasFolderCount
	 */
	if (!(hfsmp->hfs_flags & HFS_STANDARD)) {
		if (vnodetype == VDIR) {
			if (hfsmp->hfs_flags & HFS_FOLDERCOUNT)
				attr.ca_recflags = kHFSHasFolderCountMask;
		} else {
			attr.ca_recflags = kHFSThreadExistsMask;
		}
	}

#if CONFIG_PROTECT	
	if (cp_fs_protected(hfsmp->hfs_mp)) {
		protected_mount = 1;
	}
	/*
	 * On a content-protected HFS+/HFSX filesystem, files and directories
	 * cannot be created without atomically setting/creating the EA that 
	 * contains the protection class metadata and keys at the same time, in
	 * the same transaction.  As a result, pre-set the "EAs exist" flag
	 * on the cat_attr for protectable catalog record creations.  This will
	 * cause the cnode creation routine in hfs_getnewvnode to mark the cnode
	 * as having EAs.
	 */
	if ((protected_mount) && (protectable_target)) {
		attr.ca_recflags |= kHFSHasAttributesMask;
		/* delay entering in the namecache */
		nocache = 1;
	}
#endif


	/* 
	 * Add the date added to the item. See above, as
	 * all of the dates are set to the itime.
	 */
	hfs_write_dateadded (&attr, attr.ca_atime);

	/* Initialize the gen counter to 1 */
	hfs_write_gencount(&attr, (uint32_t)1);

	attr.ca_uid = vap->va_uid;
	attr.ca_gid = vap->va_gid;
	VATTR_SET_SUPPORTED(vap, va_mode);
	VATTR_SET_SUPPORTED(vap, va_uid);
	VATTR_SET_SUPPORTED(vap, va_gid);

#if QUOTA
	/* check to see if this node's creation would cause us to go over
	 * quota.  If so, abort this operation.
	 */
   	if (hfsmp->hfs_flags & HFS_QUOTAS) {
		if ((error = hfs_quotacheck(hfsmp, 1, attr.ca_uid, attr.ca_gid,
									vfs_context_ucred(ctx)))) {
			goto exit;
		}
	}	
#endif


	/* Tag symlinks with a type and creator. */
	if (vnodetype == VLNK) {
		struct FndrFileInfo *fip;

		fip = (struct FndrFileInfo *)&attr.ca_finderinfo;
		fip->fdType    = SWAP_BE32(kSymLinkFileType);
		fip->fdCreator = SWAP_BE32(kSymLinkCreator);
	}

	/* Setup the descriptor */
	in_desc.cd_nameptr = (const u_int8_t *)cnp->cn_nameptr;
	in_desc.cd_namelen = cnp->cn_namelen;
	in_desc.cd_parentcnid = dcp->c_fileid;
	in_desc.cd_flags = S_ISDIR(mode) ? CD_ISDIR : 0;
	in_desc.cd_hint = dcp->c_childhint;
	in_desc.cd_encoding = 0;

<<<<<<< HEAD
#if CONFIG_PROTECT
	/*
	 * To preserve file creation atomicity with regards to the content protection EA,
	 * we must create the file in the catalog and then write out its EA in the same
	 * transaction.  
	 * 
	 * We only denote the target class in this EA; key generation is not completed
	 * until the file has been inserted into the catalog and will be done
	 * in a separate transaction.
	 */
	if ((protected_mount) && (protectable_target)) {
		error = cp_setup_newentry(hfsmp, dcp, cp_class, attr.ca_mode, &entry);
		if (error) {
			goto exit;
		}
	}
#endif

	if ((error = hfs_start_transaction(hfsmp)) != 0) {
	    goto exit;
	}
	started_tr = 1;

	// have to also lock the attribute file because cat_create() needs
	// to check that any fileID it wants to use does not have orphaned
	// attributes in it.
	lockflags = hfs_systemfile_lock(hfsmp, SFL_CATALOG | SFL_ATTRIBUTE, HFS_EXCLUSIVE_LOCK);
	cnid_t new_id;

	/* Reserve some space in the Catalog file. */
	if ((error = cat_preflight(hfsmp, CAT_CREATE, NULL, 0))) {
		hfs_systemfile_unlock(hfsmp, lockflags);
		goto exit;
	}

	if ((error = cat_acquire_cnid(hfsmp, &new_id))) {
		hfs_systemfile_unlock (hfsmp, lockflags);
=======
	// XXXdbg
	hfs_global_shared_lock_acquire(hfsmp);
	grabbed_lock = 1;
	if (hfsmp->jnl) {
	    if ((error = journal_start_transaction(hfsmp->jnl)) != 0) {
			goto exit;
	    }
		started_tr = 1;
	}

	/* Lock catalog b-tree */
	error = hfs_metafilelocking(VTOHFS(dvp), kHFSCatalogFileID, LK_EXCLUSIVE, p);
	if (error)
>>>>>>> origin/10.2
		goto exit;
	}

	error = cat_create(hfsmp, new_id, &in_desc, &attr, &out_desc);
	if (error == 0) {
		/* Update the parent directory */
		dcp->c_childhint = out_desc.cd_hint;	/* Cache directory's location */
		dcp->c_entries++;
<<<<<<< HEAD

=======
		{
			struct FndrExtendedDirInfo *extinfo = (struct FndrExtendedDirInfo *)((u_int8_t*)dcp->c_finderinfo + 16);
			extinfo->write_gen_counter = OSSwapHostToBigInt32(OSSwapBigToHostInt32(extinfo->write_gen_counter) + 1);
		}
>>>>>>> origin/10.9
		if (vnodetype == VDIR) {
			INC_FOLDERCOUNT(hfsmp, dcp->c_attr);
		}
		dcp->c_dirchangecnt++;
<<<<<<< HEAD
		hfs_incr_gencount(dcp);

		dcp->c_touch_chgtime = dcp->c_touch_modtime = true;
		dcp->c_flag |= C_MODIFIED;

		hfs_update(dcp->c_vp, 0);
=======
		{	
			struct FndrExtendedDirInfo *extinfo = (struct FndrExtendedDirInfo *)((u_int8_t*)dcp->c_finderinfo + 16);
			extinfo->write_gen_counter = OSSwapHostToBigInt32(OSSwapBigToHostInt32(extinfo->write_gen_counter) + 1);
		}
		dcp->c_ctime = tv.tv_sec;
		dcp->c_mtime = tv.tv_sec;
		(void) cat_update(hfsmp, &dcp->c_desc, &dcp->c_attr, NULL, NULL);
>>>>>>> origin/10.9

#if CONFIG_PROTECT
		/*
		 * If we are creating a content protected file, now is when
		 * we create the EA. We must create it in the same transaction
		 * that creates the file.  We can also guarantee that the file 
		 * MUST exist because we are still holding the catalog lock
		 * at this point.
		 */
		if ((attr.ca_fileid != 0) && (protected_mount) && (protectable_target)) {
			error = cp_setxattr (NULL, entry, hfsmp, attr.ca_fileid, XATTR_CREATE);
			
			if (error) {
				int delete_err;
				/* 
				 * If we fail the EA creation, then we need to delete the file. 
				 * Luckily, we are still holding all of the right locks.
				 */
				delete_err = cat_delete (hfsmp, &out_desc, &attr);
				if (delete_err == 0) {
					/* Update the parent directory */
					if (dcp->c_entries > 0)
						dcp->c_entries--;
					dcp->c_dirchangecnt++;
					dcp->c_ctime = tv.tv_sec;
					dcp->c_mtime = tv.tv_sec;
					(void) cat_update(hfsmp, &dcp->c_desc, &dcp->c_attr, NULL, NULL);
				}

				/* Emit EINVAL if we fail to create EA*/
				error = EINVAL;
			}
		}		
#endif
	}
	hfs_systemfile_unlock(hfsmp, lockflags);
	if (error)
		goto exit;
<<<<<<< HEAD

	uint32_t txn = hfsmp->jnl ? journal_current_txn(hfsmp->jnl) : 0;

	/* Invalidate negative cache entries in the directory */
	if (dcp->c_flag & C_NEG_ENTRIES) {
		cache_purge_negatives(dvp);
		dcp->c_flag &= ~C_NEG_ENTRIES;
	}
=======
	
	/* Update the parent directory */
	dcp->c_childhint = out_desc.cd_hint;	/* Cache directory's location */
	dcp->c_nlink++;
	dcp->c_entries++;
	dcp->c_flag |= C_CHANGE | C_UPDATE;
	tv = time;
	(void) VOP_UPDATE(dvp, &tv, &tv, 0);
>>>>>>> origin/10.2

	hfs_volupdate(hfsmp, vnodetype == VDIR ? VOL_MKDIR : VOL_MKFILE,
		(dcp->c_cnid == kHFSRootFolderID));

	// XXXdbg
	// have to end the transaction here before we call hfs_getnewvnode()
	// because that can cause us to try and reclaim a vnode on a different
	// file system which could cause us to start a transaction which can
	// deadlock with someone on that other file system (since we could be
	// holding two transaction locks as well as various vnodes and we did
	// not obtain the locks on them in the proper order).
<<<<<<< HEAD
	//
=======
    //
>>>>>>> origin/10.2
	// NOTE: this means that if the quota check fails or we have to update
	//       the change time on a block-special device that those changes
	//       will happen as part of independent transactions.
	//
	if (started_tr) {
<<<<<<< HEAD
	    hfs_end_transaction(hfsmp);
	    started_tr = 0;
	}

<<<<<<< HEAD
#if CONFIG_PROTECT
	/* 
	 * At this point, we must have encountered success with writing the EA.
	 * Destroy our temporary cprotect (which had no keys). 
	 */

	if ((attr.ca_fileid != 0) && (protected_mount) && (protectable_target))  {
		cp_entry_destroy (hfsmp, entry);
		entry = NULL;
	}
#endif
	gnv_flags |= GNV_CREATE;
	if (nocache) {
		gnv_flags |= GNV_NOCACHE;
=======
	/* Do not create vnode for whiteouts */
	if (S_ISWHT(mode)) {
		goto exit;
>>>>>>> origin/10.6
	}

	/*
	 * Create a vnode for the object just created.
	 * 
	 * NOTE: Maintaining the cnode lock on the parent directory is important,
	 * as it prevents race conditions where other threads want to look up entries 
	 * in the directory and/or add things as we are in the process of creating
	 * the vnode below.  However, this has the potential for causing a 
	 * double lock panic when dealing with shadow files on a HFS boot partition. 
	 * The panic could occur if we are not cleaning up after ourselves properly 
	 * when done with a shadow file or in the error cases.  The error would occur if we 
	 * try to create a new vnode, and then end up reclaiming another shadow vnode to 
	 * create the new one.  However, if everything is working properly, this should
	 * be a non-issue as we would never enter that reclaim codepath.
<<<<<<< HEAD
	 * 
=======
	 *
>>>>>>> origin/10.6
	 * The cnode is locked on successful return.
	 */
	error = hfs_getnewvnode(hfsmp, dvp, cnp, &out_desc, gnv_flags, &attr, 
							NULL, &tvp, &newvnode_flags);
	if (error)
		goto exit;

	cp = VTOC(tvp);

<<<<<<< HEAD
	cp->c_update_txn = txn;

=======
>>>>>>> origin/10.9
	struct  doc_tombstone *ut;
	ut = get_uthread_doc_tombstone();
	if (   ut->t_lastop_document_id != 0 
	    && ut->t_lastop_parent == dvp
	    && ut->t_lastop_parent_vid == vnode_vid(dvp)
<<<<<<< HEAD
	    && strcmp((char *)ut->t_lastop_filename, (const char *)cp->c_desc.cd_nameptr) == 0) {
=======
	    && strcmp((char *)ut->t_lastop_filename, (char *)cp->c_desc.cd_nameptr) == 0) {
>>>>>>> origin/10.9
		struct FndrExtendedDirInfo *fip = (struct FndrExtendedDirInfo *)((char *)&cp->c_attr.ca_finderinfo + 16);

		//printf("CREATE: preserving doc-id %lld on %s\n", ut->t_lastop_document_id, ut->t_lastop_filename);
		fip->document_id = (uint32_t)(ut->t_lastop_document_id & 0xffffffff);

		cp->c_bsdflags |= UF_TRACKED;
<<<<<<< HEAD
		cp->c_flag |= C_MODIFIED;
=======
		// mark the cnode dirty
		cp->c_flag |= C_MODIFIED | C_FORCEUPDATE;
>>>>>>> origin/10.9

		if ((error = hfs_start_transaction(hfsmp)) == 0) {
			lockflags = hfs_systemfile_lock(hfsmp, SFL_CATALOG, HFS_EXCLUSIVE_LOCK);

			(void) cat_update(hfsmp, &cp->c_desc, &cp->c_attr, NULL, NULL);

			hfs_systemfile_unlock (hfsmp, lockflags);
			(void) hfs_end_transaction(hfsmp);
		}

		clear_tombstone_docid(ut, hfsmp, cp);       // will send the docid-changed fsevent
	} else if (ut->t_lastop_document_id != 0) {
		int len = cnp->cn_namelen;
		if (len == 0) {
			len = strlen(cnp->cn_nameptr);
		}

		if (is_ignorable_temp_name(cnp->cn_nameptr, cnp->cn_namelen)) {
			// printf("CREATE: not clearing tombstone because %s is a temp name.\n", cnp->cn_nameptr);
		} else {
			// Clear the tombstone because the thread is not recreating the same path
			// printf("CREATE: clearing tombstone because %s is NOT a temp name.\n", cnp->cn_nameptr);
			clear_tombstone_docid(ut, hfsmp, NULL);
		}
<<<<<<< HEAD
=======
		journal_end_transaction(hfsmp->jnl);
		started_tr = 0;
	}
	if (grabbed_lock) {
		hfs_global_shared_lock_release(hfsmp);
		grabbed_lock = 0;
	}

	/* Create a vnode for the object just created: */
	error = hfs_getnewvnode(hfsmp, NULL, &out_desc, 0, &attr, NULL, &tvp);
	if (error)
		goto exit;


#if QUOTA
	cp = VTOC(tvp);
	/* 
	 * We call hfs_chkiq with FORCE flag so that if we
	 * fall through to the rmdir we actually have 
	 * accounted for the inode
	*/
	if ((error = hfs_getinoquota(cp)) ||
	    (error = hfs_chkiq(cp, 1, cnp->cn_cred, FORCE))) {
		if ((cnp->cn_flags & (HASBUF | SAVESTART)) == HASBUF) {
			FREE_ZONE(cnp->cn_pnbuf, cnp->cn_pnlen, M_NAMEI);
			cnp->cn_flags &= ~HASBUF;
		}
		if (tvp->v_type == VDIR)
			VOP_RMDIR(dvp,tvp, cnp);
		else
			VOP_REMOVE(dvp,tvp, cnp);

		return (error);
>>>>>>> origin/10.2
	}

	if ((hfsmp->hfs_flags & HFS_CS_HOTFILE_PIN) && (vnode_isfastdevicecandidate(dvp) && !vnode_isautocandidate(dvp))) {

		//printf("hfs: flagging %s (fileid: %d) as VFASTDEVCANDIDATE (dvp name: %s)\n",
		//       cnp->cn_nameptr ? cnp->cn_nameptr : "<NONAME>",
		//       cp->c_fileid,
		//       dvp->v_name ? dvp->v_name : "no-dir-name");

		//
		// On new files we set the FastDevCandidate flag so that
		// any new blocks allocated to it will be pinned.
		//
		cp->c_attr.ca_recflags |= kHFSFastDevCandidateMask;
		vnode_setfastdevicecandidate(tvp);

		//
		// properly inherit auto-cached flags
		//
		if (vnode_isautocandidate(dvp)) {
			cp->c_attr.ca_recflags |= kHFSAutoCandidateMask;
			vnode_setautocandidate(tvp);
		}


		//
		// We also want to add it to the hotfile adoption list so
		// that it will eventually land in the hotfile btree
		//
		(void) hfs_addhotfile(tvp);
	}
	
=======
	}

>>>>>>> origin/10.9
	*vpp = tvp;

#if CONFIG_PROTECT
	/* 
	 * Now that we have a vnode-in-hand, generate keys for this namespace item.
	 * If we fail to create the keys, then attempt to delete the item from the 
	 * namespace.  If we can't delete the item, that's not desirable but also not fatal..
	 *  All of the places which deal with restoring/unwrapping keys must also be 
	 * prepared to encounter an entry that does not have keys. 
	 */
	if ((protectable_target) && (protected_mount)) {
		struct cprotect *keyed_entry = NULL;

<<<<<<< HEAD
		if (cp->c_cpentry == NULL) {
			panic ("hfs_makenode: no cpentry for cnode (%p)", cp);
		}

		error = cp_generate_keys (hfsmp, cp, CP_CLASS(cp->c_cpentry->cp_pclass), keywrap_flags, &keyed_entry);
		if (error == 0) {
			/* 
			 * Upon success, the keys were generated and written out. 
			 * Update the cp pointer in the cnode.
			 */
			cp_replace_entry (hfsmp, cp, keyed_entry);
			if (nocache) {
				cache_enter (dvp, tvp, cnp);
			}
=======
		cp = VTOC(tvp);
		cp->c_mode = mode;
		tvp->v_type = IFTOVT(mode);
		cp->c_flag |= C_CHANGE;
		tv = time;
		if ((error = VOP_UPDATE(tvp, &tv, &tv, 1))) {
			vput(tvp);
			goto exit;
>>>>>>> origin/10.2
		}
		else {
			/* If key creation OR the setxattr failed, emit EPERM to userland */
			error = EPERM;

			/* 
			 * Beware! This slightly violates the lock ordering for the
			 * cnode/vnode 'tvp'.  Ordinarily, you must acquire the truncate lock
			 * which guards file size changes before acquiring the normal cnode lock
			 * and calling hfs_removefile on an item.  
			 * 
			 * However, in this case, we are still holding the directory lock so 
			 * 'tvp' is not lookup-able and it was a newly created vnode so it 
			 * cannot have any content yet. The only reason we are initiating 
			 * the removefile is because we could not generate content protection keys
			 * for this namespace item. Note also that we pass a '1' in the allow_dirs
			 * argument for hfs_removefile because we may be creating a directory here.
			 * 
			 * All this to say that while it is technically a violation it is 
			 * impossible to race with another thread for this cnode so it is safe.
			 */
			int err = hfs_removefile (dvp, tvp, cnp, 0, 0, 1, NULL, 0); 
			if (err) {
				printf("hfs_makenode: removefile failed (%d) for CP entry %p\n", err, tvp);
			}

			/* Release the cnode lock and mark the vnode for termination */	
			hfs_unlock (cp);
			err = vnode_recycle (tvp);
			if (err) {
				printf("hfs_makenode: vnode_recycle failed (%d) for CP entry %p\n", err, tvp);
			}

			/* Drop the iocount on the new vnode to force reclamation/recycling */
			vnode_put (tvp);
			cp = NULL;
			*vpp = NULL;
		}	
	}
#endif

#if QUOTA
	/* 
	 * Once we create this vnode, we need to initialize its quota data 
	 * structures, if necessary.  We know that it is OK to just go ahead and 
	 * initialize because we've already validated earlier (through the hfs_quotacheck 
	 * function) to see if creating this cnode/vnode would cause us to go over quota. 
	 */
	if (hfsmp->hfs_flags & HFS_QUOTAS) {
		if (cp) {
			/* cp could have been zeroed earlier */ 
			(void) hfs_getinoquota(cp); 
		}
	}
#endif

exit:
	cat_releasedesc(&out_desc);
	
#if CONFIG_PROTECT
	/*  
	 * We may have jumped here in error-handling various situations above.
	 * If we haven't already dumped the temporary CP used to initialize
	 * the file atomically, then free it now. cp_entry_destroy should null
	 * out the pointer if it was called already.
	 */
	if (entry) {
		cp_entry_destroy (hfsmp, entry);
		entry = NULL;
	}	
#endif

<<<<<<< HEAD
	/*
	 * Make sure we release cnode lock on dcp.
	 */
	if (dcp) {
		dcp->c_flag &= ~C_DIR_MODIFICATION;
		wakeup((caddr_t)&dcp->c_flag);
		
		hfs_unlock(dcp);
	}
	if (error == 0 && cp != NULL) {
		hfs_unlock(cp);
	}
	if (started_tr) {
	    hfs_end_transaction(hfsmp);
	    started_tr = 0;
=======
	if ((cnp->cn_flags & (HASBUF | SAVESTART)) == HASBUF)
        	FREE_ZONE(cnp->cn_pnbuf, cnp->cn_pnlen, M_NAMEI);

	/*
	 * Check if a file is located in the "Cleanup At Startup"
	 * directory.  If it is then tag it as NODUMP so that we
	 * can be lazy about zero filling data holes.
	 */
	if ((error == 0) && (vnodetype == VREG) &&
	    (dcp->c_desc.cd_nameptr != NULL) &&
	    (strcmp(dcp->c_desc.cd_nameptr, "Cleanup At Startup") == 0)) {
	   	struct vnode *ddvp;
		cnid_t parid;

		parid = dcp->c_parentcnid;
		vput(dvp);
		dvp = NULL;

		/*
		 * The parent of "Cleanup At Startup" should
		 * have the ASCII name of the userid.
		 */
		if (VFS_VGET(HFSTOVFS(hfsmp), &parid, &ddvp) == 0) {
                       if (VTOC(ddvp)->c_desc.cd_nameptr) {
                               uid_t uid;

                               uid = strtoul(VTOC(ddvp)->c_desc.cd_nameptr, 0, 0);
                               if (uid == cp->c_uid || uid == cnp->cn_cred->cr_uid) {
				cp->c_flags |= UF_NODUMP;
				cp->c_flag |= C_CHANGE;
                               }
			}
			vput(ddvp);
		}
	}

	if (dvp)
		vput(dvp);

	// XXXdbg
	if (started_tr) {
	    journal_end_transaction(hfsmp->jnl);
		started_tr = 0;
	}
	if (grabbed_lock) {
		hfs_global_shared_lock_release(hfsmp);
		grabbed_lock = 0;
>>>>>>> origin/10.2
	}

	return (error);
}


<<<<<<< HEAD
/*
 * hfs_vgetrsrc acquires a resource fork vnode corresponding to the
 * cnode that is found in 'vp'.  The cnode should be locked upon entry
 * and will be returned locked, but it may be dropped temporarily.
 *
 * If the resource fork vnode does not exist, HFS will attempt to acquire an
 * empty (uninitialized) vnode from VFS so as to avoid deadlocks with
 * jetsam. If we let the normal getnewvnode code produce the vnode for us
 * we would be doing so while holding the cnode lock of our cnode.
 * 
 * On success, *rvpp wlll hold the resource fork vnode with an
 * iocount.  *Don't* forget the vnode_put.
 */
int
hfs_vgetrsrc(struct hfsmount *hfsmp, struct vnode *vp, struct vnode **rvpp)
=======

/* hfs_vgetrsrc acquires a resource fork vnode corresponding to the cnode that is
 * found in 'vp'.  The rsrc fork vnode is returned with the cnode locked and iocount
 * on the rsrc vnode.
 * 
 * *rvpp is an output argument for returning the pointer to the resource fork vnode.
 * In most cases, the resource fork vnode will not be set if we return an error. 
 * However, if error_on_unlinked is set, we may have already acquired the resource fork vnode
 * before we discover the error (the file has gone open-unlinked).  In this case only,
 * we may return a vnode in the output argument despite an error.
 * 
 * If can_drop_lock is set, then it is safe for this function to temporarily drop
 * and then re-acquire the cnode lock.  We may need to do this, for example, in order to 
 * acquire an iocount or promote our lock.  
 * 
 * error_on_unlinked is an argument which indicates that we are to return an error if we 
 * discover that the cnode has gone into an open-unlinked state ( C_DELETED or C_NOEXISTS)
 * is set in the cnode flags.  This is only necessary if can_drop_lock is true, otherwise 
 * there's really no reason to double-check for errors on the cnode.
 */

__private_extern__
int
hfs_vgetrsrc(struct hfsmount *hfsmp, struct vnode *vp, 
		struct vnode **rvpp, int can_drop_lock, int error_on_unlinked)
>>>>>>> origin/10.6
{
<<<<<<< HEAD
	struct vnode *rvp = NULLVP;
	struct vnode *empty_rvp = NULLVP;
=======
	struct vnode *rvp;
>>>>>>> origin/10.5
	struct vnode *dvp = NULLVP;
	struct cnode *cp = VTOC(vp);
	int error;
	int vid;
	int delete_status = 0;


	/*
	 * Need to check the status of the cnode to validate it hasn't
	 * gone open-unlinked on us before we can actually do work with it.
	 */
	delete_status = hfs_checkdeleted (cp);
	if ((delete_status) && (error_on_unlinked)) {
		return delete_status;
	}

<<<<<<< HEAD
	if (vnode_vtype(vp) == VDIR) {
		return EINVAL;
	}
	
restart:
	/* Attempt to use existing vnode */
=======
restart:
	/* Attempt to use exising vnode */
>>>>>>> origin/10.5
	if ((rvp = cp->c_rsrc_vp)) {
		vid = vnode_vid(rvp);

		// vnode_getwithvid can block so we need to drop the cnode lock
		hfs_unlock(cp);

		error = vnode_getwithvid(rvp, vid);

<<<<<<< HEAD
		hfs_lock_always(cp, HFS_EXCLUSIVE_LOCK);

		/*
		 * When our lock was relinquished, the resource fork
		 * could have been recycled.  Check for this and try
		 * again.
		 */
		if (error == ENOENT)
			goto restart;

=======
		if (can_drop_lock) {
			(void) hfs_lock(cp, HFS_FORCE_LOCK);

			/*
			 * When we relinquished our cnode lock, the cnode could have raced
			 * with a delete and gotten deleted.  If the caller did not want
			 * us to ignore open-unlinked files, then re-check the C_DELETED
			 * state and see if we need to return an ENOENT here because the item
			 * got deleted in the intervening time.
			 */
			if (error_on_unlinked) {
				if ((delete_status = hfs_checkdeleted(cp))) {
					/* 
					 * If error == 0, this means that we succeeded in acquiring an iocount on the 
					 * rsrc fork vnode.  However, if we're in this block of code, that 
					 * means that we noticed that the cnode has gone open-unlinked.  In 
					 * this case, the caller requested that we not do any other work and 
					 * return an errno.  The caller will be responsible for dropping the 
					 * iocount we just acquired because we can't do it until we've released 
					 * the cnode lock.  
					 */
					if (error == 0) {
						*rvpp = rvp;
					}
					return delete_status;
				}
			}

			/*
			 * When our lock was relinquished, the resource fork
			 * could have been recycled.  Check for this and try
			 * again.
			 */
			if (error == ENOENT)
				goto restart;
		}
>>>>>>> origin/10.5
		if (error) {
			const char * name = (const char *)VTOC(vp)->c_desc.cd_nameptr;

			if (name)
				printf("hfs_vgetrsrc: couldn't get resource"
<<<<<<< HEAD
				       " fork for %s, vol=%s, err=%d\n", name, hfsmp->vcbVN, error);
=======
				       " fork for %s, err %d\n", name, error);
>>>>>>> origin/10.5
			return (error);
		}
	} else {
		struct cat_fork rsrcfork;
		struct componentname cn;
		struct cat_desc *descptr = NULL;
		struct cat_desc to_desc;
		char delname[32];
		int lockflags;
		int newvnode_flags = 0;

		/* 
		 * In this case, we don't currently see a resource fork vnode attached
		 * to this cnode.  In most cases, we were called from a read-only VNOP
		 * like getattr, so it should be safe to drop the cnode lock and then 
		 * re-acquire it.  
		 *
		 * Here, we drop the lock so that we can acquire an empty/husk
		 * vnode so that we don't deadlock against jetsam.  
		 *
		 * It does not currently appear possible to hold the truncate lock via
		 * FS re-entrancy when we get to this point. (8/2014)
		 */
<<<<<<< HEAD
		hfs_unlock (cp);

		error = vnode_create_empty (&empty_rvp); 

		hfs_lock_always (cp, HFS_EXCLUSIVE_LOCK);

		if (error) {
			/* If acquiring the 'empty' vnode failed, then nothing to clean up */
			return error;
=======
		if (cp->c_lockowner != current_thread()) {
			if (!can_drop_lock) {				
				return (EINVAL);
			}
			/*
			 * If the upgrade fails we lose the lock and
			 * have to take the exclusive lock on our own.
			 */
			if (lck_rw_lock_shared_to_exclusive(&cp->c_rwlock) == FALSE)
				lck_rw_lock_exclusive(&cp->c_rwlock);
			cp->c_lockowner = current_thread();
>>>>>>> origin/10.6
		}

		/* 
		 * We could have raced with another thread here while we dropped our cnode
		 * lock.  See if the cnode now has a resource fork vnode and restart if appropriate.
		 *
		 * Note: We just released the cnode lock, so there is a possibility that the
		 * cnode that we just acquired has been deleted or even removed from disk
		 * completely, though this is unlikely. If the file is open-unlinked, the 
		 * check below will resolve it for us.  If it has been completely 
		 * removed (even from the catalog!), then when we examine the catalog 
		 * directly, below, while holding the catalog lock, we will not find the
		 * item and we can fail out properly.
		 */
		if (cp->c_rsrc_vp) {
			/* Drop the empty vnode before restarting */
			vnode_put (empty_rvp);
			empty_rvp = NULL;
			rvp = NULL;
			goto restart;
		}
	
		/*
		 * hfs_vgetsrc may be invoked for a cnode that has already been marked
		 * C_DELETED.  This is because we need to continue to provide rsrc
		 * fork access to open-unlinked files.  In this case, build a fake descriptor
		 * like in hfs_removefile.  If we don't do this, buildkey will fail in
		 * cat_lookup because this cnode has no name in its descriptor. However,
		 * only do this if the caller did not specify that they wanted us to
		 * error out upon encountering open-unlinked files.
		 */
<<<<<<< HEAD
=======

		if ((error_on_unlinked) && (can_drop_lock)) {
			if ((error = hfs_checkdeleted (cp))) {
				return error;
			}
		}

>>>>>>> origin/10.6
		if ((cp->c_flag & C_DELETED ) && (cp->c_desc.cd_namelen == 0)) {
			bzero (&to_desc, sizeof(to_desc));
			bzero (delname, 32);
			MAKE_DELETED_NAME(delname, sizeof(delname), cp->c_fileid);
			to_desc.cd_nameptr = (const u_int8_t*) delname;
			to_desc.cd_namelen = strlen(delname);
			to_desc.cd_parentcnid = hfsmp->hfs_private_desc[FILE_HARDLINKS].cd_cnid;
			to_desc.cd_flags = 0;
			to_desc.cd_cnid = cp->c_cnid;

			descptr = &to_desc;
		}
		else {
			descptr = &cp->c_desc;
		}


		lockflags = hfs_systemfile_lock(hfsmp, SFL_CATALOG, HFS_SHARED_LOCK);

<<<<<<< HEAD
		/*
=======
		/* 
		 * Get resource fork data
		 *
>>>>>>> origin/10.8
		 * We call cat_idlookup (instead of cat_lookup) below because we can't
		 * trust the descriptor in the provided cnode for lookups at this point.  
		 * Between the time of the original lookup of this vnode and now, the 
		 * descriptor could have gotten swapped or replaced.  If this occurred, 
		 * the parent/name combo originally desired may not necessarily be provided
		 * if we use the descriptor.  Even worse, if the vnode represents
		 * a hardlink, we could have removed one of the links from the namespace
		 * but left the descriptor alone, since hfs_unlink does not invalidate
		 * the descriptor in the cnode if other links still point to the inode.
		 * 
		 * Consider the following (slightly contrived) scenario:
		 * /tmp/a <--> /tmp/b (hardlinks).
		 * 1. Thread A: open rsrc fork on /tmp/b.
		 * 1a. Thread A: does lookup, goes out to lunch right before calling getnamedstream.
		 * 2. Thread B does 'mv /foo/b /tmp/b'
		 * 2. Thread B succeeds.
		 * 3. Thread A comes back and wants rsrc fork info for /tmp/b.  
		 * 
		 * Even though the hardlink backing /tmp/b is now eliminated, the descriptor
		 * is not removed/updated during the unlink process.  So, if you were to
		 * do a lookup on /tmp/b, you'd acquire an entirely different record's resource
		 * fork.
		 * 
<<<<<<< HEAD
		 * As a result, we use the fileid, which should be invariant for the lifetime
		 * of the cnode (possibly barring calls to exchangedata).
=======
 		 * As a result, we use the fileid, which should be invariant for the lifetime
 		 * of the cnode (possibly barring calls to exchangedata).
>>>>>>> origin/10.8
		 *
		 * Addendum: We can't do the above for HFS standard since we aren't guaranteed to
		 * have thread records for files.  They were only required for directories.  So
		 * we need to do the lookup with the catalog name. This is OK since hardlinks were
		 * never allowed on HFS standard.
		 */

<<<<<<< HEAD
		/* Get resource fork data */
		if ((hfsmp->hfs_flags & HFS_STANDARD) == 0) {
			error = cat_idlookup (hfsmp, cp->c_fileid, 0, 1, NULL, NULL, &rsrcfork);
		}
#if CONFIG_HFS_STD
		else {
			/* 
			 * HFS standard only:
			 * 
			 * Get the resource fork for this item with a cat_lookup call, but do not 
			 * force a case lookup since HFS standard is case-insensitive only. We 
			 * don't want the descriptor; just the fork data here. If we tried to
			 * do a ID lookup (via thread record -> catalog record), then we might fail
			 * prematurely since, as noted above, thread records were not strictly required
			 * on files in HFS.
			 */
			error = cat_lookup (hfsmp, descptr, 1, 0, (struct cat_desc*)NULL, 
					(struct cat_attr*)NULL, &rsrcfork, NULL);
		}
#endif
=======
		if (hfsmp->hfs_flags & HFS_STANDARD) {
			/* 
			 * HFS standard only:
			 * 
			 * Get the resource fork for this item via catalog lookup
			 * since HFS standard was case-insensitive only. We don't want the 
			 * descriptor; just the fork data here.
			 */
			error = cat_lookup (hfsmp, descptr, 1, (struct cat_desc*)NULL, 
					(struct cat_attr*)NULL, &rsrcfork, NULL);
		}
		else {
			error = cat_idlookup (hfsmp, cp->c_fileid, 0, 1, NULL, NULL, &rsrcfork);
		}
>>>>>>> origin/10.8

		hfs_systemfile_unlock(hfsmp, lockflags);
		if (error) {
			/* Drop our 'empty' vnode ! */
			vnode_put (empty_rvp);
			return (error);
		}

		/*
		 * Supply hfs_getnewvnode with a component name. 
		 */
		cn.cn_pnbuf = NULL;
		if (descptr->cd_nameptr) {
			MALLOC_ZONE(cn.cn_pnbuf, caddr_t, MAXPATHLEN, M_NAMEI, M_WAITOK);
			cn.cn_nameiop = LOOKUP;
			cn.cn_flags = ISLASTCN | HASBUF;
			cn.cn_context = NULL;
			cn.cn_pnlen = MAXPATHLEN;
			cn.cn_nameptr = cn.cn_pnbuf;
			cn.cn_hash = 0;
			cn.cn_consume = 0;
			cn.cn_namelen = snprintf(cn.cn_nameptr, MAXPATHLEN,
						 "%s%s", descptr->cd_nameptr,
						 _PATH_RSRCFORKSPEC);
			// Should never happen because cn.cn_nameptr won't ever be long...
			if (cn.cn_namelen >= MAXPATHLEN) {
				FREE_ZONE(cn.cn_pnbuf, cn.cn_pnlen, M_NAMEI);
				/* Drop our 'empty' vnode ! */
				vnode_put (empty_rvp);
				return ENAMETOOLONG;
				
			}
		}
		dvp = vnode_getparent(vp);
<<<<<<< HEAD
		
		/* 
		 * We are about to call hfs_getnewvnode and pass in the vnode that we acquired
		 * earlier when we were not holding any locks. The semantics of GNV_USE_VP require that
		 * either hfs_getnewvnode consume the vnode and vend it back to us, properly initialized,
		 * or it will consume/dispose of it properly if it errors out.
		 */ 
		rvp = empty_rvp;
		
		error = hfs_getnewvnode(hfsmp, dvp, cn.cn_pnbuf ? &cn : NULL,
		                        descptr, (GNV_WANTRSRC | GNV_SKIPLOCK | GNV_USE_VP), 
								&cp->c_attr, &rsrcfork, &rvp, &newvnode_flags);
			
=======
		error = hfs_getnewvnode(hfsmp, dvp, cn.cn_pnbuf ? &cn : NULL,
		                        &cp->c_desc, GNV_WANTRSRC | GNV_SKIPLOCK, &cp->c_attr,
		                        &rsrcfork, &rvp);
>>>>>>> origin/10.5
		if (dvp)
			vnode_put(dvp);
		if (cn.cn_pnbuf)
			FREE_ZONE(cn.cn_pnbuf, cn.cn_pnlen, M_NAMEI);
		if (error)
			return (error);
	}  /* End 'else' for rsrc fork not existing */

	*rvpp = rvp;
	return (0);
}

/*
 * Wrapper for special device reads
 */
int
hfsspec_read(ap)
	struct vnop_read_args /* {
		struct vnode *a_vp;
		struct uio *a_uio;
		int  a_ioflag;
		vfs_context_t a_context;
	} */ *ap;
{
	/*
	 * Set access flag.
	 */
	VTOC(ap->a_vp)->c_touch_acctime = TRUE;
	return (VOCALL (spec_vnodeop_p, VOFFSET(vnop_read), ap));
}

/*
 * Wrapper for special device writes
 */
int
hfsspec_write(ap)
	struct vnop_write_args /* {
		struct vnode *a_vp;
		struct uio *a_uio;
		int  a_ioflag;
		vfs_context_t a_context;
	} */ *ap;
{
	/*
	 * Set update and change flags.
	 */
	VTOC(ap->a_vp)->c_touch_chgtime = TRUE;
	VTOC(ap->a_vp)->c_touch_modtime = TRUE;
	return (VOCALL (spec_vnodeop_p, VOFFSET(vnop_write), ap));
}

/*
 * Wrapper for special device close
 *
 * Update the times on the cnode then do device close.
 */
int
hfsspec_close(ap)
	struct vnop_close_args /* {
		struct vnode *a_vp;
		int  a_fflag;
		vfs_context_t a_context;
	} */ *ap;
{
	struct vnode *vp = ap->a_vp;
	struct cnode *cp;

	if (vnode_isinuse(ap->a_vp, 0)) {
		if (hfs_lock(VTOC(vp), HFS_EXCLUSIVE_LOCK, HFS_LOCK_DEFAULT) == 0) {
			cp = VTOC(vp);
			hfs_touchtimes(VTOHFS(vp), cp);
			hfs_unlock(cp);
		}
	}
	return (VOCALL (spec_vnodeop_p, VOFFSET(vnop_close), ap));
}

#if FIFO
/*
 * Wrapper for fifo reads
 */
static int
hfsfifo_read(ap)
	struct vnop_read_args /* {
		struct vnode *a_vp;
		struct uio *a_uio;
		int  a_ioflag;
		vfs_context_t a_context;
	} */ *ap;
{
	/*
	 * Set access flag.
	 */
	VTOC(ap->a_vp)->c_touch_acctime = TRUE;
	return (VOCALL (fifo_vnodeop_p, VOFFSET(vnop_read), ap));
}

/*
 * Wrapper for fifo writes
 */
static int
hfsfifo_write(ap)
	struct vnop_write_args /* {
		struct vnode *a_vp;
		struct uio *a_uio;
		int  a_ioflag;
		vfs_context_t a_context;
	} */ *ap;
{
	/*
	 * Set update and change flags.
	 */
	VTOC(ap->a_vp)->c_touch_chgtime = TRUE;
	VTOC(ap->a_vp)->c_touch_modtime = TRUE;
	return (VOCALL (fifo_vnodeop_p, VOFFSET(vnop_write), ap));
}

/*
 * Wrapper for fifo close
 *
 * Update the times on the cnode then do device close.
 */
static int
hfsfifo_close(ap)
	struct vnop_close_args /* {
		struct vnode *a_vp;
		int  a_fflag;
		vfs_context_t a_context;
	} */ *ap;
{
	struct vnode *vp = ap->a_vp;
	struct cnode *cp;

	if (vnode_isinuse(ap->a_vp, 1)) {
		if (hfs_lock(VTOC(vp), HFS_EXCLUSIVE_LOCK, HFS_LOCK_DEFAULT) == 0) {
			cp = VTOC(vp);
			hfs_touchtimes(VTOHFS(vp), cp);
			hfs_unlock(cp);
		}
	}
	return (VOCALL (fifo_vnodeop_p, VOFFSET(vnop_close), ap));
}


#endif /* FIFO */

/* 
 * Getter for the document_id 
 * the document_id is stored in FndrExtendedFileInfo/FndrExtendedDirInfo
 */
static u_int32_t 
hfs_get_document_id_internal(const uint8_t *finderinfo, mode_t mode)
{
<<<<<<< HEAD
	const uint8_t *finfo = NULL;
	u_int32_t doc_id = 0;
	
	/* overlay the FinderInfo to the correct pointer, and advance */
	finfo = finderinfo + 16;

	if (S_ISDIR(mode) || S_ISREG(mode)) {
		const struct FndrExtendedFileInfo *extinfo = (const struct FndrExtendedFileInfo *)finfo;
		doc_id = extinfo->document_id;
	} else if (S_ISDIR(mode)) {
		const struct FndrExtendedDirInfo *extinfo = (const struct FndrExtendedDirInfo *)finfo;
=======
	u_int8_t *finfo = NULL;
	u_int32_t doc_id = 0;
	
	/* overlay the FinderInfo to the correct pointer, and advance */
	finfo = ((uint8_t *)finderinfo) + 16;

	if (S_ISDIR(mode) || S_ISREG(mode)) {
		struct FndrExtendedFileInfo *extinfo = (struct FndrExtendedFileInfo *)finfo;
		doc_id = extinfo->document_id;
	} else if (S_ISDIR(mode)) {
		struct FndrExtendedDirInfo *extinfo = (struct FndrExtendedDirInfo *)((u_int8_t*)finderinfo + 16);
>>>>>>> origin/10.9
		doc_id = extinfo->document_id;
	}	

	return doc_id;
}


/* getter(s) for document id */
u_int32_t
hfs_get_document_id(struct cnode *cp)
{
	return (hfs_get_document_id_internal((u_int8_t*)cp->c_finderinfo,
	    cp->c_attr.ca_mode));
}

/* If you have finderinfo and mode, you can use this */
u_int32_t
hfs_get_document_id_from_blob(const uint8_t *finderinfo, mode_t mode)
{
	return (hfs_get_document_id_internal(finderinfo, mode));
}

/*
 * Synchronize a file's in-core state with that on disk.
 */
int
hfs_vnop_fsync(ap)
	struct vnop_fsync_args /* {
		struct vnode *a_vp;
		int a_waitfor;
		vfs_context_t a_context;
	} */ *ap;
{
	struct vnode* vp = ap->a_vp;
	int error;

	/* Note: We check hfs flags instead of vfs mount flag because during 
	 * read-write update, hfs marks itself read-write much earlier than
	 * the vfs, and hence won't result in skipping of certain writes like 
	 * zero'ing out of unused nodes, creation of hotfiles btree, etc. 
	 */
	if (VTOHFS(vp)->hfs_flags & HFS_READ_ONLY) {
		return 0;		
	}

	/*
	 * No need to call cp_handle_vnop to resolve fsync().  Any dirty data
	 * should have caused the keys to be unwrapped at the time the data was
	 * put into the UBC, either at mmap/pagein/read-write.  If we did manage
	 * to let this by, then strategy will auto-resolve for us.
	 * 
	 * We also need to allow ENOENT lock errors since unlink
	 * system call can call VNOP_FSYNC during vclean.
	 */
	error = hfs_lock(VTOC(vp), HFS_EXCLUSIVE_LOCK, HFS_LOCK_DEFAULT);
	if (error)
		return (0);

	error = hfs_fsync(vp, ap->a_waitfor, 0, vfs_context_proc(ap->a_context));

	hfs_unlock(VTOC(vp));
	return (error);
}

int (**hfs_vnodeop_p)(void *);

#define VOPFUNC int (*)(void *)


#if CONFIG_HFS_STD
int (**hfs_std_vnodeop_p) (void *);
static int hfs_readonly_op (__unused void* ap) { return (EROFS); }

/* 
 * In 10.6 and forward, HFS Standard is read-only and deprecated.  The vnop table below
 * is for use with HFS standard to block out operations that would modify the file system
 */

struct vnodeopv_entry_desc hfs_standard_vnodeop_entries[] = {
    { &vnop_default_desc, (VOPFUNC)vn_default_error },
    { &vnop_lookup_desc, (VOPFUNC)hfs_vnop_lookup },		/* lookup */
    { &vnop_create_desc, (VOPFUNC)hfs_readonly_op },		/* create (READONLY) */
    { &vnop_mknod_desc, (VOPFUNC)hfs_readonly_op },             /* mknod (READONLY) */
    { &vnop_open_desc, (VOPFUNC)hfs_vnop_open },			/* open */
    { &vnop_close_desc, (VOPFUNC)hfs_vnop_close },		/* close */
    { &vnop_getattr_desc, (VOPFUNC)hfs_vnop_getattr },		/* getattr */
    { &vnop_setattr_desc, (VOPFUNC)hfs_readonly_op },		/* setattr */
    { &vnop_read_desc, (VOPFUNC)hfs_vnop_read },			/* read */
    { &vnop_write_desc, (VOPFUNC)hfs_readonly_op },		/* write (READONLY) */
    { &vnop_ioctl_desc, (VOPFUNC)hfs_vnop_ioctl },		/* ioctl */
    { &vnop_select_desc, (VOPFUNC)hfs_vnop_select },		/* select */
    { &vnop_revoke_desc, (VOPFUNC)nop_revoke },			/* revoke */
    { &vnop_exchange_desc, (VOPFUNC)hfs_readonly_op },		/* exchange  (READONLY)*/
    { &vnop_mmap_desc, (VOPFUNC)err_mmap },			/* mmap */
    { &vnop_fsync_desc, (VOPFUNC)hfs_readonly_op},		/* fsync (READONLY) */
    { &vnop_remove_desc, (VOPFUNC)hfs_readonly_op },		/* remove (READONLY) */
    { &vnop_link_desc, (VOPFUNC)hfs_readonly_op },			/* link ( READONLLY) */
    { &vnop_rename_desc, (VOPFUNC)hfs_readonly_op },		/* rename (READONLY)*/
    { &vnop_mkdir_desc, (VOPFUNC)hfs_readonly_op },             /* mkdir (READONLY) */
    { &vnop_rmdir_desc, (VOPFUNC)hfs_readonly_op },		/* rmdir (READONLY) */
    { &vnop_symlink_desc, (VOPFUNC)hfs_readonly_op },         /* symlink (READONLY) */
    { &vnop_readdir_desc, (VOPFUNC)hfs_vnop_readdir },		/* readdir */
    { &vnop_readdirattr_desc, (VOPFUNC)hfs_vnop_readdirattr },	/* readdirattr */
    { &vnop_readlink_desc, (VOPFUNC)hfs_vnop_readlink },		/* readlink */
    { &vnop_inactive_desc, (VOPFUNC)hfs_vnop_inactive },		/* inactive */
    { &vnop_reclaim_desc, (VOPFUNC)hfs_vnop_reclaim },		/* reclaim */
    { &vnop_strategy_desc, (VOPFUNC)hfs_vnop_strategy },		/* strategy */
    { &vnop_pathconf_desc, (VOPFUNC)hfs_vnop_pathconf },		/* pathconf */
    { &vnop_advlock_desc, (VOPFUNC)err_advlock },		/* advlock */
    { &vnop_allocate_desc, (VOPFUNC)hfs_readonly_op },		/* allocate (READONLY) */
#if CONFIG_SEARCHFS
    { &vnop_searchfs_desc, (VOPFUNC)hfs_vnop_search },		/* search fs */
#else
    { &vnop_searchfs_desc, (VOPFUNC)err_searchfs },		/* search fs */
#endif
    { &vnop_bwrite_desc, (VOPFUNC)hfs_readonly_op },		/* bwrite (READONLY) */
    { &vnop_pagein_desc, (VOPFUNC)hfs_vnop_pagein },		/* pagein */
    { &vnop_pageout_desc,(VOPFUNC) hfs_readonly_op },		/* pageout (READONLY)  */
    { &vnop_copyfile_desc, (VOPFUNC)hfs_readonly_op },		/* copyfile (READONLY)*/
    { &vnop_blktooff_desc, (VOPFUNC)hfs_vnop_blktooff },		/* blktooff */
    { &vnop_offtoblk_desc, (VOPFUNC)hfs_vnop_offtoblk },		/* offtoblk */
    { &vnop_blockmap_desc, (VOPFUNC)hfs_vnop_blockmap },			/* blockmap */
    { &vnop_getxattr_desc, (VOPFUNC)hfs_vnop_getxattr},
    { &vnop_setxattr_desc, (VOPFUNC)hfs_readonly_op},         /* set xattr (READONLY) */
    { &vnop_removexattr_desc, (VOPFUNC)hfs_readonly_op},      /* remove xattr (READONLY) */
    { &vnop_listxattr_desc, (VOPFUNC)hfs_vnop_listxattr},
#if NAMEDSTREAMS
    { &vnop_getnamedstream_desc, (VOPFUNC)hfs_vnop_getnamedstream },
    { &vnop_makenamedstream_desc, (VOPFUNC)hfs_readonly_op }, 
    { &vnop_removenamedstream_desc, (VOPFUNC)hfs_readonly_op },
#endif
    { &vnop_getattrlistbulk_desc, (VOPFUNC)hfs_vnop_getattrlistbulk },	/* getattrlistbulk */
    { NULL, (VOPFUNC)NULL }
};

struct vnodeopv_desc hfs_std_vnodeop_opv_desc =
{ &hfs_std_vnodeop_p, hfs_standard_vnodeop_entries };
#endif

/* VNOP table for HFS+ */
struct vnodeopv_entry_desc hfs_vnodeop_entries[] = {
    { &vnop_default_desc, (VOPFUNC)vn_default_error },
    { &vnop_lookup_desc, (VOPFUNC)hfs_vnop_lookup },		/* lookup */
    { &vnop_create_desc, (VOPFUNC)hfs_vnop_create },		/* create */
    { &vnop_mknod_desc, (VOPFUNC)hfs_vnop_mknod },             /* mknod */
    { &vnop_open_desc, (VOPFUNC)hfs_vnop_open },			/* open */
    { &vnop_close_desc, (VOPFUNC)hfs_vnop_close },		/* close */
    { &vnop_getattr_desc, (VOPFUNC)hfs_vnop_getattr },		/* getattr */
    { &vnop_setattr_desc, (VOPFUNC)hfs_vnop_setattr },		/* setattr */
    { &vnop_read_desc, (VOPFUNC)hfs_vnop_read },			/* read */
    { &vnop_write_desc, (VOPFUNC)hfs_vnop_write },		/* write */
    { &vnop_ioctl_desc, (VOPFUNC)hfs_vnop_ioctl },		/* ioctl */
    { &vnop_select_desc, (VOPFUNC)hfs_vnop_select },		/* select */
    { &vnop_revoke_desc, (VOPFUNC)nop_revoke },			/* revoke */
    { &vnop_exchange_desc, (VOPFUNC)hfs_vnop_exchange },		/* exchange */
    { &vnop_mmap_desc, (VOPFUNC)hfs_vnop_mmap },			/* mmap */
    { &vnop_fsync_desc, (VOPFUNC)hfs_vnop_fsync },		/* fsync */
    { &vnop_remove_desc, (VOPFUNC)hfs_vnop_remove },		/* remove */
    { &vnop_link_desc, (VOPFUNC)hfs_vnop_link },			/* link */
    { &vnop_rename_desc, (VOPFUNC)hfs_vnop_rename },		/* rename */
    { &vnop_mkdir_desc, (VOPFUNC)hfs_vnop_mkdir },             /* mkdir */
    { &vnop_rmdir_desc, (VOPFUNC)hfs_vnop_rmdir },		/* rmdir */
    { &vnop_symlink_desc, (VOPFUNC)hfs_vnop_symlink },         /* symlink */
    { &vnop_readdir_desc, (VOPFUNC)hfs_vnop_readdir },		/* readdir */
    { &vnop_readdirattr_desc, (VOPFUNC)hfs_vnop_readdirattr },	/* readdirattr */
    { &vnop_readlink_desc, (VOPFUNC)hfs_vnop_readlink },		/* readlink */
    { &vnop_inactive_desc, (VOPFUNC)hfs_vnop_inactive },		/* inactive */
    { &vnop_reclaim_desc, (VOPFUNC)hfs_vnop_reclaim },		/* reclaim */
    { &vnop_strategy_desc, (VOPFUNC)hfs_vnop_strategy },		/* strategy */
    { &vnop_pathconf_desc, (VOPFUNC)hfs_vnop_pathconf },		/* pathconf */
    { &vnop_advlock_desc, (VOPFUNC)err_advlock },		/* advlock */
    { &vnop_allocate_desc, (VOPFUNC)hfs_vnop_allocate },		/* allocate */
#if CONFIG_SEARCHFS
    { &vnop_searchfs_desc, (VOPFUNC)hfs_vnop_search },		/* search fs */
#else
    { &vnop_searchfs_desc, (VOPFUNC)err_searchfs },		/* search fs */
#endif
    { &vnop_bwrite_desc, (VOPFUNC)hfs_vnop_bwrite },		/* bwrite */
    { &vnop_pagein_desc, (VOPFUNC)hfs_vnop_pagein },		/* pagein */
    { &vnop_pageout_desc,(VOPFUNC) hfs_vnop_pageout },		/* pageout */
    { &vnop_copyfile_desc, (VOPFUNC)err_copyfile },		/* copyfile */
    { &vnop_blktooff_desc, (VOPFUNC)hfs_vnop_blktooff },		/* blktooff */
    { &vnop_offtoblk_desc, (VOPFUNC)hfs_vnop_offtoblk },		/* offtoblk */
    { &vnop_blockmap_desc, (VOPFUNC)hfs_vnop_blockmap },			/* blockmap */
    { &vnop_getxattr_desc, (VOPFUNC)hfs_vnop_getxattr},
    { &vnop_setxattr_desc, (VOPFUNC)hfs_vnop_setxattr},
    { &vnop_removexattr_desc, (VOPFUNC)hfs_vnop_removexattr},
    { &vnop_listxattr_desc, (VOPFUNC)hfs_vnop_listxattr},
#if NAMEDSTREAMS
    { &vnop_getnamedstream_desc, (VOPFUNC)hfs_vnop_getnamedstream },
    { &vnop_makenamedstream_desc, (VOPFUNC)hfs_vnop_makenamedstream },
    { &vnop_removenamedstream_desc, (VOPFUNC)hfs_vnop_removenamedstream },
#endif
    { &vnop_getattrlistbulk_desc, (VOPFUNC)hfs_vnop_getattrlistbulk },	/* getattrlistbulk */
	{ &vnop_mnomap_desc, (VOPFUNC)hfs_vnop_mnomap },
    { NULL, (VOPFUNC)NULL }
};

struct vnodeopv_desc hfs_vnodeop_opv_desc =
{ &hfs_vnodeop_p, hfs_vnodeop_entries };


/* Spec Op vnop table for HFS+ */
int (**hfs_specop_p)(void *);
struct vnodeopv_entry_desc hfs_specop_entries[] = {
	{ &vnop_default_desc, (VOPFUNC)vn_default_error },
	{ &vnop_lookup_desc, (VOPFUNC)spec_lookup },		/* lookup */
	{ &vnop_create_desc, (VOPFUNC)spec_create },		/* create */
	{ &vnop_mknod_desc, (VOPFUNC)spec_mknod },              /* mknod */
	{ &vnop_open_desc, (VOPFUNC)spec_open },			/* open */
	{ &vnop_close_desc, (VOPFUNC)hfsspec_close },		/* close */
	{ &vnop_getattr_desc, (VOPFUNC)hfs_vnop_getattr },	/* getattr */
	{ &vnop_setattr_desc, (VOPFUNC)hfs_vnop_setattr },	/* setattr */
	{ &vnop_read_desc, (VOPFUNC)hfsspec_read },		/* read */
	{ &vnop_write_desc, (VOPFUNC)hfsspec_write },		/* write */
	{ &vnop_ioctl_desc, (VOPFUNC)spec_ioctl },		/* ioctl */
	{ &vnop_select_desc, (VOPFUNC)spec_select },		/* select */
	{ &vnop_revoke_desc, (VOPFUNC)spec_revoke },		/* revoke */
	{ &vnop_mmap_desc, (VOPFUNC)spec_mmap },			/* mmap */
	{ &vnop_fsync_desc, (VOPFUNC)hfs_vnop_fsync },		/* fsync */
	{ &vnop_remove_desc, (VOPFUNC)spec_remove },		/* remove */
	{ &vnop_link_desc, (VOPFUNC)spec_link },			/* link */
	{ &vnop_rename_desc, (VOPFUNC)spec_rename },		/* rename */
	{ &vnop_mkdir_desc, (VOPFUNC)spec_mkdir },              /* mkdir */
	{ &vnop_rmdir_desc, (VOPFUNC)spec_rmdir },		/* rmdir */
	{ &vnop_symlink_desc, (VOPFUNC)spec_symlink },          /* symlink */
	{ &vnop_readdir_desc, (VOPFUNC)spec_readdir },		/* readdir */
	{ &vnop_readlink_desc, (VOPFUNC)spec_readlink },		/* readlink */
	{ &vnop_inactive_desc, (VOPFUNC)hfs_vnop_inactive },	/* inactive */
	{ &vnop_reclaim_desc, (VOPFUNC)hfs_vnop_reclaim },	/* reclaim */
	{ &vnop_strategy_desc, (VOPFUNC)spec_strategy },		/* strategy */
	{ &vnop_pathconf_desc, (VOPFUNC)spec_pathconf },		/* pathconf */
	{ &vnop_advlock_desc, (VOPFUNC)err_advlock },		/* advlock */
	{ &vnop_bwrite_desc, (VOPFUNC)hfs_vnop_bwrite },
	{ &vnop_pagein_desc, (VOPFUNC)hfs_vnop_pagein },		/* Pagein */
	{ &vnop_pageout_desc, (VOPFUNC)hfs_vnop_pageout },	/* Pageout */
    { &vnop_copyfile_desc, (VOPFUNC)err_copyfile },		/* copyfile */
	{ &vnop_blktooff_desc, (VOPFUNC)hfs_vnop_blktooff },	/* blktooff */
	{ &vnop_offtoblk_desc, (VOPFUNC)hfs_vnop_offtoblk },	/* offtoblk */
	{ &vnop_getxattr_desc, (VOPFUNC)hfs_vnop_getxattr},
	{ &vnop_setxattr_desc, (VOPFUNC)hfs_vnop_setxattr},
	{ &vnop_removexattr_desc, (VOPFUNC)hfs_vnop_removexattr},
	{ &vnop_listxattr_desc, (VOPFUNC)hfs_vnop_listxattr},
	{ (struct vnodeop_desc*)NULL, (VOPFUNC)NULL }
};
struct vnodeopv_desc hfs_specop_opv_desc =
	{ &hfs_specop_p, hfs_specop_entries };

#if FIFO
/* HFS+ FIFO VNOP table  */
int (**hfs_fifoop_p)(void *);
struct vnodeopv_entry_desc hfs_fifoop_entries[] = {
	{ &vnop_default_desc, (VOPFUNC)vn_default_error },
	{ &vnop_lookup_desc, (VOPFUNC)fifo_lookup },		/* lookup */
	{ &vnop_create_desc, (VOPFUNC)fifo_create },		/* create */
	{ &vnop_mknod_desc, (VOPFUNC)fifo_mknod },              /* mknod */
	{ &vnop_open_desc, (VOPFUNC)fifo_open },			/* open */
	{ &vnop_close_desc, (VOPFUNC)hfsfifo_close },		/* close */
	{ &vnop_getattr_desc, (VOPFUNC)hfs_vnop_getattr },	/* getattr */
	{ &vnop_setattr_desc, (VOPFUNC)hfs_vnop_setattr },	/* setattr */
	{ &vnop_read_desc, (VOPFUNC)hfsfifo_read },		/* read */
	{ &vnop_write_desc, (VOPFUNC)hfsfifo_write },		/* write */
	{ &vnop_ioctl_desc, (VOPFUNC)fifo_ioctl },		/* ioctl */
	{ &vnop_select_desc, (VOPFUNC)fifo_select },		/* select */
	{ &vnop_revoke_desc, (VOPFUNC)fifo_revoke },		/* revoke */
	{ &vnop_mmap_desc, (VOPFUNC)fifo_mmap },			/* mmap */
	{ &vnop_fsync_desc, (VOPFUNC)hfs_vnop_fsync },		/* fsync */
	{ &vnop_remove_desc, (VOPFUNC)fifo_remove },		/* remove */
	{ &vnop_link_desc, (VOPFUNC)fifo_link },			/* link */
	{ &vnop_rename_desc, (VOPFUNC)fifo_rename },		/* rename */
	{ &vnop_mkdir_desc, (VOPFUNC)fifo_mkdir },              /* mkdir */
	{ &vnop_rmdir_desc, (VOPFUNC)fifo_rmdir },		/* rmdir */
	{ &vnop_symlink_desc, (VOPFUNC)fifo_symlink },          /* symlink */
	{ &vnop_readdir_desc, (VOPFUNC)fifo_readdir },		/* readdir */
	{ &vnop_readlink_desc, (VOPFUNC)fifo_readlink },		/* readlink */
	{ &vnop_inactive_desc, (VOPFUNC)hfs_vnop_inactive },	/* inactive */
	{ &vnop_reclaim_desc, (VOPFUNC)hfs_vnop_reclaim },	/* reclaim */
	{ &vnop_strategy_desc, (VOPFUNC)fifo_strategy },		/* strategy */
	{ &vnop_pathconf_desc, (VOPFUNC)fifo_pathconf },		/* pathconf */
	{ &vnop_advlock_desc, (VOPFUNC)err_advlock },		/* advlock */
	{ &vnop_bwrite_desc, (VOPFUNC)hfs_vnop_bwrite },
	{ &vnop_pagein_desc, (VOPFUNC)hfs_vnop_pagein },		/* Pagein */
	{ &vnop_pageout_desc, (VOPFUNC)hfs_vnop_pageout },	/* Pageout */
	{ &vnop_copyfile_desc, (VOPFUNC)err_copyfile }, 		/* copyfile */
	{ &vnop_blktooff_desc, (VOPFUNC)hfs_vnop_blktooff },	/* blktooff */
	{ &vnop_offtoblk_desc, (VOPFUNC)hfs_vnop_offtoblk },	/* offtoblk */
  	{ &vnop_blockmap_desc, (VOPFUNC)hfs_vnop_blockmap },		/* blockmap */
	{ &vnop_getxattr_desc, (VOPFUNC)hfs_vnop_getxattr},
	{ &vnop_setxattr_desc, (VOPFUNC)hfs_vnop_setxattr},
	{ &vnop_removexattr_desc, (VOPFUNC)hfs_vnop_removexattr},
	{ &vnop_listxattr_desc, (VOPFUNC)hfs_vnop_listxattr},
	{ (struct vnodeop_desc*)NULL, (VOPFUNC)NULL }
};
struct vnodeopv_desc hfs_fifoop_opv_desc =
	{ &hfs_fifoop_p, hfs_fifoop_entries };
#endif /* FIFO */



