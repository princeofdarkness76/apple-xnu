/*
<<<<<<< HEAD
 * Copyright (c) 1997-2013 Apple Inc. All rights reserved.
=======
 * Copyright (c) 2000-2002 Apple Computer, Inc. All rights reserved.
>>>>>>> origin/10.1
 *
 * @APPLE_OSREFERENCE_LICENSE_HEADER_START@
 * 
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
 * EXPRESS OR IMPLIED, AND APPLE HEREBY DISCLAIMS ALL SUCH WARRANTIES,
 * INCLUDING WITHOUT LIMITATION, ANY WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE, QUIET ENJOYMENT OR NON-INFRINGEMENT.
 * Please see the License for the specific language governing rights and
 * limitations under the License.
 * 
 * @APPLE_OSREFERENCE_LICENSE_HEADER_END@
 *
 *	@(#)hfs_search.c
 */
/*
 * NOTICE: This file was modified by SPARTA, Inc. in 2005 to introduce
 * support for mandatory and extensible security protections.  This notice
 * is included in support of clause 2.2 (b) of the Apple Public License,
 * Version 2.0.
 */

#include <sys/param.h>
#include <sys/systm.h>
#include <sys/kernel.h>
#include <sys/file.h>
#include <sys/proc.h>
#include <sys/conf.h>
#include <mach/machine/vm_types.h>
#include <sys/vnode.h>
#include <sys/malloc.h>
#include <sys/signalvar.h>
#include <sys/attr.h>
#include <sys/utfconv.h>
#include <sys/kauth.h>
#include <sys/vnode_internal.h>
#include <sys/mount_internal.h>

#if CONFIG_MACF
#include <security/mac_framework.h>
#endif

#include "hfs.h"
#include "hfs_dbg.h"
#include "hfs_catalog.h"
#include "hfs_attrlist.h"
#include "hfs_endian.h"

#include "hfscommon/headers/FileMgrInternal.h"
#include "hfscommon/headers/HFSUnicodeWrappers.h"
#include "hfscommon/headers/BTreesPrivate.h"
#include "hfscommon/headers/BTreeScanner.h"
<<<<<<< HEAD
#include "hfscommon/headers/CatalogPrivate.h"

#if CONFIG_SEARCHFS

/* Search criterea. */
struct directoryInfoSpec
{
	u_int32_t   numFiles;
};

struct fileInfoSpec
{
	off_t		dataLogicalLength;
	off_t		dataPhysicalLength;
	off_t		resourceLogicalLength;
	off_t		resourcePhysicalLength;
};

struct searchinfospec
{
	u_char			name[kHFSPlusMaxFileNameBytes];
	u_int32_t			nameLength;
	char			attributes;		// see IM:Files 2-100
	u_int32_t			nodeID;
	u_int32_t			parentDirID;
	struct timespec		creationDate;		
	struct timespec		modificationDate;		
	struct timespec		changeDate;	
	struct timespec		accessDate;		
	struct timespec		lastBackupDate;	
	u_int8_t		finderInfo[32];
	uid_t			uid;	
	gid_t			gid;
	mode_t			mask;
	struct fileInfoSpec	f;
	struct directoryInfoSpec d;
};
typedef struct searchinfospec searchinfospec_t;
=======
>>>>>>> origin/10.1

static void ResolveHardlink(struct hfsmount *hfsmp, HFSPlusCatalogFile *recp);


<<<<<<< HEAD
static int UnpackSearchAttributeBlock(struct hfsmount *hfsmp, struct attrlist *alist,
		searchinfospec_t *searchInfo, void *attributeBuffer, int firstblock);
=======
Boolean CheckCriteria(	ExtendedVCB *vcb,
						u_long searchBits, 
						struct attrlist *attrList,
						CatalogNodeData *cnp, 
						CatalogKey *key,
						searchinfospec_t *searchInfo1, 
						searchinfospec_t *searchInfo2,
						Boolean lookForDup);
>>>>>>> origin/10.1

static int CheckCriteria(	ExtendedVCB *vcb, 
							u_long searchBits,
							struct attrlist *attrList, 
							CatalogRecord *rec,
							CatalogKey *key, 
							searchinfospec_t *searchInfo1,
							searchinfospec_t *searchInfo2,
							struct vfs_context *ctx);

static int CheckAccess(ExtendedVCB *vcb, u_long searchBits, CatalogKey *key, struct vfs_context *ctx);

static int InsertMatch(struct hfsmount *hfsmp, uio_t a_uio, CatalogRecord *rec,
			CatalogKey *key, struct attrlist *returnAttrList,
			void *attributesBuffer, void *variableBuffer,
			uint32_t * nummatches );

static Boolean CompareRange(u_long val, u_long low, u_long high);
static Boolean CompareWideRange(u_int64_t val, u_int64_t low, u_int64_t high);

static Boolean CompareRange( u_long val, u_long low, u_long high )
{
	return( (val >= low) && (val <= high) );
}

static Boolean CompareWideRange( u_int64_t val, u_int64_t low, u_int64_t high )
{
	return( (val >= low) && (val <= high) );
}
//#define CompareRange(val, low, high)	((val >= low) && (val <= high))
<<<<<<< HEAD
			
=======

static Boolean IsTargetName( searchinfospec_t * searchInfoPtr, Boolean isHFSPlus );

>>>>>>> origin/10.1

/************************************************************************/
/* Entry for searchfs()                                                 */
/************************************************************************/

#define	errSearchBufferFull	101	/* Internal search errors */
/*
#
#% searchfs	vp	L L L
#
vnop_searchfs {
    IN struct vnode *vp;
    IN off_t length;
    IN int flags;
    IN kauth_cred_t cred;
    IN struct proc *p;
};
*/

int
hfs_vnop_search(ap)
	struct vnop_searchfs_args *ap; /*
		struct vnodeop_desc *a_desc;
		struct vnode *a_vp;
		void *a_searchparams1;
		void *a_searchparams2;
		struct attrlist *a_searchattrs;
		u_long a_maxmatches;
		struct timeval *a_timelimit;
		struct attrlist *a_returnattrs;
		u_long *a_nummatches;
		u_long a_scriptcode;
		u_long a_options;
		struct uio *a_uio;
		struct searchstate *a_searchstate;
		vfs_context_t a_context;
	*/
{
<<<<<<< HEAD
	ExtendedVCB *vcb = VTOVCB(ap->a_vp);
	struct hfsmount *hfsmp;
	FCB * catalogFCB;
	searchinfospec_t searchInfo1;
	searchinfospec_t searchInfo2;
	void *attributesBuffer = NULL;
	void *variableBuffer;
	u_int32_t fixedBlockSize;
	u_int32_t eachReturnBufferSize;
	struct proc *p = current_proc();
	int err = E_NONE;
	int isHFSPlus;
	CatalogKey * myCurrentKeyPtr;
	CatalogRecord * myCurrentDataPtr;
	CatPosition * myCatPositionPtr;
	BTScanState myBTScanState;
<<<<<<< HEAD
	user_addr_t user_start = 0;
	user_size_t user_len = 0;
	int32_t searchTime;
	int lockflags;
	struct uthread	*ut;
	boolean_t timerExpired = FALSE;
	boolean_t needThrottle = FALSE;
=======
	FCB*				catalogFCB;
	searchinfospec_t	searchInfo1;
	searchinfospec_t	searchInfo2;
	void				*attributesBuffer;
	void				*variableBuffer;
	short				recordSize;
	short				operation;
	u_long				fixedBlockSize;
	u_long				eachReturnBufferSize;
	struct proc			*p = current_proc();
	CatalogNodeData		myCNodeData;
	CatalogNodeData	*	myCNodeDataPtr;
	CatalogKey * 		myCurrentKeyPtr;
	CatalogRecord * 	myCurrentDataPtr;
	CatPosition * 		myCatPositionPtr;
	BTScanState 		myBTScanState;
	Boolean				timerExpired = false;
	Boolean				doQuickExit = false;
	u_long				lastNodeNum = 0XFFFFFFFF;
	ExtendedVCB			*vcb = VTOVCB(ap->a_vp);
	int					err = E_NONE;
	int					isHFSPlus;
>>>>>>> origin/10.1
=======
	void *user_start = NULL;
	int   user_len;
>>>>>>> origin/10.2

	/* XXX Parameter check a_searchattrs? */

	*(ap->a_nummatches) = 0;

	if (ap->a_options & ~SRCHFS_VALIDOPTIONSMASK) {
		return (EINVAL);
<<<<<<< HEAD
	}

	/*
	 * Fail requests for attributes that HFS does not support for the
	 * items that match the search criteria.  Note that these checks
	 * are for the OUTBOUND attributes to be returned (not search criteria).
	 */
	if ((ap->a_returnattrs->commonattr & ~HFS_ATTR_CMN_VALID) ||
	    (ap->a_returnattrs->volattr != 0) ||
	    (ap->a_returnattrs->dirattr & ~HFS_ATTR_DIR_VALID) ||
	    (ap->a_returnattrs->fileattr & ~HFS_ATTR_FILE_VALID) ||
	    (ap->a_returnattrs->forkattr != 0)) {

		return (EINVAL);
	}

	/* SRCHFS_SKIPLINKS requires root access.
	 * This option cannot be used with either
	 * the ATTR_CMN_NAME or ATTR_CMN_PAROBJID
	 * attributes.
	 */
	if (ap->a_options & SRCHFS_SKIPLINKS) {
		attrgroup_t attrs;

		attrs = ap->a_searchattrs->commonattr | ap->a_returnattrs->commonattr;
		if (attrs & (ATTR_CMN_NAME | ATTR_CMN_PAROBJID)) {
			return (EINVAL);
		}

		if ((err = vfs_context_suser(ap->a_context))) {
			return (err);
		}
	}

	// If both 32-bit and 64-bit parent ids or file ids are given	   
	// then return an error.

	attrgroup_t test_attrs=ap->a_searchattrs->commonattr;

	if (((test_attrs & ATTR_CMN_OBJID) && (test_attrs & ATTR_CMN_FILEID)) ||
			((test_attrs & ATTR_CMN_PARENTID) && (test_attrs & ATTR_CMN_PAROBJID))) {
		return (EINVAL);
	}

	if (uio_resid(ap->a_uio) <= 0) {
		return (EINVAL);
	}

	isHFSPlus = (vcb->vcbSigWord == kHFSPlusSigWord);
	hfsmp = VTOHFS(ap->a_vp);
	
	searchTime = kMaxMicroSecsInKernel;
	if (ap->a_timelimit->tv_sec == 0 &&
	    ap->a_timelimit->tv_usec > 0 &&
	    ap->a_timelimit->tv_usec < kMaxMicroSecsInKernel) {
		searchTime = ap->a_timelimit->tv_usec;
	}

	/* UnPack the search boundries, searchInfo1, searchInfo2 */
	err = UnpackSearchAttributeBlock(hfsmp, ap->a_searchattrs,
				&searchInfo1, ap->a_searchparams1, 1);
	if (err) { 
		return err;
	}
	err = UnpackSearchAttributeBlock(hfsmp, ap->a_searchattrs,
				&searchInfo2, ap->a_searchparams2, 0);
	if (err) {
		return err;
	}
	//shadow search bits if 64-bit file/parent ids are used	
	if (ap->a_searchattrs->commonattr & ATTR_CMN_FILEID) 
		ap->a_searchattrs->commonattr |= ATTR_CMN_OBJID;
	if (ap->a_searchattrs->commonattr & ATTR_CMN_PARENTID) 
		ap->a_searchattrs->commonattr |= ATTR_CMN_PAROBJID;

	fixedBlockSize = sizeof(u_int32_t) + hfs_attrblksize(ap->a_returnattrs);	/* u_int32_t for length word */

=======

	isHFSPlus = (vcb->vcbSigWord == kHFSPlusSigWord);

	/* UnPack the search boundries, searchInfo1, searchInfo2 */
	err = UnpackSearchAttributeBlock( ap->a_vp, ap->a_searchattrs, &searchInfo1, ap->a_searchparams1 );
	if (err) return err;
	err = UnpackSearchAttributeBlock( ap->a_vp, ap->a_searchattrs, &searchInfo2, ap->a_searchparams2 );
	if (err) return err;

	fixedBlockSize = sizeof(u_long) + AttributeBlockSize( ap->a_returnattrs );	/* u_long for length longword */
>>>>>>> origin/10.1
	eachReturnBufferSize = fixedBlockSize;

	if ( ap->a_returnattrs->commonattr & ATTR_CMN_NAME )	/* XXX should be more robust! */
		eachReturnBufferSize += kHFSPlusMaxFileNameBytes + 1;

	MALLOC( attributesBuffer, void *, eachReturnBufferSize, M_TEMP, M_WAITOK );
	if (attributesBuffer == NULL) {
		err = ENOMEM;
		goto ExitThisRoutine;
	}
	bzero(attributesBuffer, eachReturnBufferSize);
	variableBuffer = (void*)((char*) attributesBuffer + fixedBlockSize);

	// XXXdbg - have to lock the user's buffer so we don't fault
	// while holding the shared catalog file lock.  see the comment
	// in hfs_readdir() for more details.
	//
<<<<<<< HEAD
	if (hfsmp->jnl && uio_isuserspace(ap->a_uio)) {
		user_start = uio_curriovbase(ap->a_uio);
		user_len = uio_curriovlen(ap->a_uio);

		if ((err = vslock(user_start, user_len)) != 0) {
			user_start = 0;
=======
	if (VTOHFS(ap->a_vp)->jnl && ap->a_uio->uio_segflg == UIO_USERSPACE) {
		user_start = ap->a_uio->uio_iov->iov_base;
		user_len   = ap->a_uio->uio_iov->iov_len;

		if ((err = vslock(user_start, user_len)) != 0) {
			user_start = NULL;
>>>>>>> origin/10.2
			goto ExitThisRoutine;
		}
	}

<<<<<<< HEAD
	lockflags = hfs_systemfile_lock(hfsmp, SFL_CATALOG, HFS_SHARED_LOCK);
=======
	/* Lock catalog b-tree */
	err = hfs_metafilelocking(VTOHFS(ap->a_vp), kHFSCatalogFileID, LK_SHARED, p);
	if (err)
		goto ExitThisRoutine;
>>>>>>> origin/10.2

	catalogFCB = GetFileControlBlock(vcb->catalogRefNum);
	myCurrentKeyPtr = NULL;
	myCurrentDataPtr = NULL;
	myCatPositionPtr = (CatPosition *)ap->a_searchstate;

	if (ap->a_options & SRCHFS_START) {
		/* Starting a new search. */
		/* Make sure the on-disk Catalog file is current */
		(void) hfs_fsync(vcb->catalogRefNum, MNT_WAIT, 0, p);
		if (hfsmp->jnl) {
		    hfs_systemfile_unlock(hfsmp, lockflags);
		    hfs_flush(hfsmp, HFS_FLUSH_JOURNAL);
		    lockflags = hfs_systemfile_lock(hfsmp, SFL_CATALOG, HFS_SHARED_LOCK);
		}

		ap->a_options &= ~SRCHFS_START;
		bzero((caddr_t)myCatPositionPtr, sizeof(*myCatPositionPtr));
		err = BTScanInitialize(catalogFCB, 0, 0, 0, kCatSearchBufferSize, &myBTScanState);
		if (err) {
			hfs_systemfile_unlock(hfsmp, lockflags);
			goto ExitThisRoutine;
		}
	} else {
		/* Resuming a search. */
		err = BTScanInitialize(catalogFCB, myCatPositionPtr->nextNode, 
					myCatPositionPtr->nextRecord, 
					myCatPositionPtr->recordsFound,
					kCatSearchBufferSize, 
					&myBTScanState);
		/* Make sure Catalog hasn't changed. */
		if (err == 0
		&&  myCatPositionPtr->writeCount != myBTScanState.btcb->writeCount) {
			myCatPositionPtr->writeCount = myBTScanState.btcb->writeCount;
			err = EBUSY; /* catChangedErr */
		}
	}
	hfs_systemfile_unlock(hfsmp, lockflags);

	if (err)
		goto ExitThisRoutine;

<<<<<<< HEAD
	if (throttle_get_io_policy(&ut) == IOPOL_THROTTLE)
		needThrottle = TRUE;
=======
	catalogFCB = VTOFCB( vcb->catalogRefNum );
	myCurrentKeyPtr = NULL;
	myCurrentDataPtr = NULL;
	myCatPositionPtr = (CatPosition *)ap->a_searchstate;

	if (ap->a_options & SRCHFS_START) {
		/* Starting a new search. */
		/* make sure our meta data is synced up */
		err = VOP_FSYNC(vcb->catalogRefNum, NOCRED, MNT_WAIT, p);
		ap->a_options &= ~SRCHFS_START;
		bzero( (caddr_t)myCatPositionPtr, sizeof( *myCatPositionPtr ) );
		err = BTScanInitialize(catalogFCB, 0, 0, 0, kCatSearchBufferSize, &myBTScanState);

#if 1 // Installer workaround
		// hack to get around installer problems when the installer expects search results 
		// to be in key order.  At this point the problem appears to be limited to 
		// searches for "Library".  The idea here is to go get the "Library" at root
		// and return it first to the caller then continue the search as normal with
		// the exception of taking care not to return a duplicate hit (see CheckCriteria) 
		if ( err == E_NONE &&
			 (ap->a_searchattrs->commonattr & ATTR_CMN_NAME) != 0 &&
			 IsTargetName( &searchInfo1, isHFSPlus )  )
		{
			CatalogRecord		rec;
			BTreeIterator       iterator;
			FSBufferDescriptor  btrec;
			CatalogKey *		keyp;
			UInt16              reclen;
			OSErr				result;
		
			bzero( (caddr_t)&iterator, sizeof( iterator ) );
			keyp = (CatalogKey *) &iterator.key;
			(void) BuildCatalogKeyUTF8(vcb, kRootDirID, "Library", kUndefinedStrLen, keyp, NULL);

			btrec.bufferAddress = &rec;
			btrec.itemCount = 1;
			btrec.itemSize = sizeof( rec );

			result = BTSearchRecord( catalogFCB, &iterator, kInvalidMRUCacheKey, 
									 &btrec, &reclen, &iterator );
			if ( result == E_NONE ) {
				if ( isHFSPlus ) {
					// HFSPlus vols have CatalogRecords that map exactly to CatalogNodeData so there is no need 
					// to copy.
					myCNodeDataPtr = (CatalogNodeData *) &rec;
				} else {
					CopyCatalogNodeData( vcb, &rec, &myCNodeData );
					myCNodeDataPtr = &myCNodeData;
				}
					
				if (CheckCriteria(vcb, ap->a_options, ap->a_searchattrs, myCNodeDataPtr,
								  keyp, &searchInfo1, &searchInfo2, false) &&
					CheckAccess(myCNodeDataPtr, keyp, ap->a_uio->uio_procp)) {
		
					result = InsertMatch(ap->a_vp, ap->a_uio, myCNodeDataPtr, 
									  keyp, ap->a_returnattrs,
									  attributesBuffer, variableBuffer,
									  eachReturnBufferSize, ap->a_nummatches);
					if (result == E_NONE && *(ap->a_nummatches) >= ap->a_maxmatches)
						doQuickExit = true;
				}
			}
		}
#endif // Installer workaround
	} else {
		/* Resuming a search. */
		err = BTScanInitialize(catalogFCB, myCatPositionPtr->nextNode, 
					myCatPositionPtr->nextRecord, 
					myCatPositionPtr->recordsFound,
					kCatSearchBufferSize, 
					&myBTScanState);
#if 0
		/* Make sure Catalog hasn't changed. */
		if (err == 0
		&&  myCatPositionPtr->writeCount != myBTScanState.btcb->writeCount) {
			myCatPositionPtr->writeCount = myBTScanState.btcb->writeCount;
			err = EBUSY; /* catChangedErr */
		}
#endif
	}

	/* Unlock catalog b-tree */
	(void) hfs_metafilelocking(VTOHFS(ap->a_vp), kHFSCatalogFileID, LK_RELEASE, p);
	if (err)
		goto ExitThisRoutine;
	if ( doQuickExit )
		goto QuickExit;
>>>>>>> origin/10.1
	/*
	 * Check all the catalog btree records...
	 *   return the attributes for matching items
	 */
	for (;;) {
		struct timeval myCurrentTime;
		struct timeval myElapsedTime;
		
		err = BTScanNextRecord(&myBTScanState, timerExpired, 
			(void **)&myCurrentKeyPtr, (void **)&myCurrentDataPtr, 
			NULL);
		if (err)
			break;

<<<<<<< HEAD
		/* Resolve any hardlinks */
		if (isHFSPlus && (ap->a_options & SRCHFS_SKIPLINKS) == 0) {
			ResolveHardlink(vcb, (HFSPlusCatalogFile *)myCurrentDataPtr);
		}
		if (CheckCriteria( vcb, ap->a_options, ap->a_searchattrs, myCurrentDataPtr,
				myCurrentKeyPtr, &searchInfo1, &searchInfo2, ap->a_context )
		&&  CheckAccess(vcb, ap->a_options, myCurrentKeyPtr, ap->a_context)) {
			err = InsertMatch(hfsmp, ap->a_uio, myCurrentDataPtr, 
					myCurrentKeyPtr, ap->a_returnattrs,
					attributesBuffer, variableBuffer, ap->a_nummatches);
=======
		if ( isHFSPlus ) {
			// HFSPlus vols have CatalogRecords that map exactly to CatalogNodeData so there is no need 
			// to copy.
			myCNodeDataPtr = (CatalogNodeData *) myCurrentDataPtr;
		} else {
			CopyCatalogNodeData( vcb, myCurrentDataPtr, &myCNodeData );
			myCNodeDataPtr = &myCNodeData;
		}
			
		if (CheckCriteria(vcb, ap->a_options, ap->a_searchattrs, myCNodeDataPtr,
		                  myCurrentKeyPtr, &searchInfo1, &searchInfo2, true) &&
		    CheckAccess(myCNodeDataPtr, myCurrentKeyPtr, ap->a_uio->uio_procp)) {

			err = InsertMatch(ap->a_vp, ap->a_uio, myCNodeDataPtr, 
			                  myCurrentKeyPtr, ap->a_returnattrs,
			                  attributesBuffer, variableBuffer,
			                  eachReturnBufferSize, ap->a_nummatches);
>>>>>>> origin/10.1
			if (err) {
				/*
				 * The last match didn't fit so come back
				 * to this record on the next trip.
				 */
				--myBTScanState.recordsFound;
				--myBTScanState.recordNum;
				break;
			}
<<<<<<< HEAD

			if (*(ap->a_nummatches) >= ap->a_maxmatches)
				break;
		}
		if (timerExpired == FALSE) {
			/*
			 * Check our elapsed time and bail if we've hit the max.
			 * The idea here is to throttle the amount of time we
			 * spend in the kernel.
			 */
			microuptime(&myCurrentTime);
			timersub(&myCurrentTime, &myBTScanState.startTime, &myElapsedTime);
			/*
			 * Note: assumes kMaxMicroSecsInKernel is less than 1,000,000
			 */
			if (myElapsedTime.tv_sec > 0
			    ||  myElapsedTime.tv_usec >= searchTime) {
				timerExpired = TRUE;
			} else if (needThrottle == TRUE) {
				if (throttle_io_will_be_throttled(ut->uu_lowpri_window, HFSTOVFS(hfsmp)))
					timerExpired = TRUE;
			}
=======
			if (*(ap->a_nummatches) >= ap->a_maxmatches)
				break;
		}

		/*
		 * Check our elapsed time and bail if we've hit the max.
		 * The idea here is to throttle the amount of time we
		 * spend in the kernel.
		 */
		myCurrentTime = time;
		timersub(&myCurrentTime, &myBTScanState.startTime, &myElapsedTime);
		/* Note: assumes kMaxMicroSecsInKernel is less than 1,000,000 */
		if (myElapsedTime.tv_sec > 0
		||  myElapsedTime.tv_usec >= kMaxMicroSecsInKernel) {
			timerExpired = true;
>>>>>>> origin/10.1
		}
	}
QuickExit:
	/* Update catalog position */
	myCatPositionPtr->writeCount = myBTScanState.btcb->writeCount;

<<<<<<< HEAD
	/* Update catalog position */
	myCatPositionPtr->writeCount = myBTScanState.btcb->writeCount;

	BTScanTerminate(&myBTScanState, &myCatPositionPtr->nextNode, 
			&myCatPositionPtr->nextRecord, 
			&myCatPositionPtr->recordsFound);
=======
	BTScanTerminate(&myBTScanState, &myCatPositionPtr->nextNode,
					&myCatPositionPtr->nextRecord, 
					&myCatPositionPtr->recordsFound);
>>>>>>> origin/10.1

	if ( err == E_NONE ) {
		err = EAGAIN;	/* signal to the user to call searchfs again */
	} else if ( err == errSearchBufferFull ) {
		if ( *(ap->a_nummatches) > 0 )
			err = EAGAIN;
 		else
			err = ENOBUFS;
	} else if ( err == btNotFound ) {
		err = E_NONE;	/* the entire disk has been searched */
	} else if ( err == fsBTTimeOutErr ) {
		err = EAGAIN;
	}

ExitThisRoutine:
	if (attributesBuffer)
		FREE(attributesBuffer, M_TEMP);

<<<<<<< HEAD
	if (user_start) {
		vsunlock(user_start, user_len, TRUE);
	}

	if (VTOHFS(ap->a_vp)->jnl && user_start) {
		vsunlock(user_start, user_len, TRUE);
	}

	return (MacToVFSError(err));
}


static void
ResolveHardlink(struct hfsmount *hfsmp, HFSPlusCatalogFile *recp)
{
	u_int32_t type, creator;
	int isdirlink = 0;
	int isfilelink = 0;
	time_t filecreatedate;
 
	if (recp->recordType != kHFSPlusFileRecord) {
		return;
	}
	type = SWAP_BE32(recp->userInfo.fdType);
	creator = SWAP_BE32(recp->userInfo.fdCreator);
	filecreatedate = to_bsd_time(recp->createDate);

	if ((type == kHardLinkFileType && creator == kHFSPlusCreator) &&
	    (filecreatedate == (time_t)hfsmp->hfs_itime ||
	     filecreatedate == (time_t)hfsmp->hfs_metadata_createdate)) {
		isfilelink = 1;
	} else if ((type == kHFSAliasType && creator == kHFSAliasCreator) &&
	           (recp->flags & kHFSHasLinkChainMask) &&
	           (filecreatedate == (time_t)hfsmp->hfs_itime ||
	            filecreatedate == (time_t)hfsmp->hfs_metadata_createdate)) {
		isdirlink = 1;
	}

	if (isfilelink || isdirlink) {
		cnid_t saved_cnid;
		int lockflags;

		/* Export link's cnid (a unique value) instead of inode's cnid */
		saved_cnid = recp->fileID;
		lockflags = hfs_systemfile_lock(hfsmp, SFL_CATALOG, HFS_SHARED_LOCK);

		(void) cat_resolvelink(hfsmp, recp->hl_linkReference, isdirlink, recp);

		recp->fileID = saved_cnid;
		hfs_systemfile_unlock(hfsmp, lockflags);
	}
=======
	return (MacToVFSError(err));
>>>>>>> origin/10.1
}


static Boolean
CompareMasked(const u_int32_t *thisValue, const u_int32_t *compareData,
		const u_int32_t *compareMask, u_int32_t count)
{
	Boolean	matched;
	u_int32_t	i;
	
	matched = true;		/* Assume it will all match */
	
	for (i=0; i<count; i++) {
		if (((*thisValue++ ^ *compareData++) & *compareMask++) != 0) {
			matched = false;
			break;
		}
	}
	
	return matched;
}


static Boolean
ComparePartialUnicodeName (register ConstUniCharArrayPtr str, register ItemCount s_len,
			   register ConstUniCharArrayPtr find, register ItemCount f_len, int caseSensitive )
{
	if (f_len == 0 || s_len == 0) {
		return FALSE;
	}

	if (caseSensitive) {
		do {
			if (s_len-- < f_len)
				return FALSE;
		} while (UnicodeBinaryCompare(str++, f_len, find, f_len) != 0);
	}
	else {
		do {
			if (s_len-- < f_len)
				return FALSE;
		} while (FastUnicodeCompare(str++, f_len, find, f_len) != 0);
	}

	return TRUE;
}

#if CONFIG_HFS_STD
static Boolean
ComparePartialPascalName ( register ConstStr31Param str, register ConstStr31Param find )
{
	register u_char s_len = str[0];
	register u_char f_len = find[0];
	register u_char *tsp;
	Str31 tmpstr;

	if (f_len == 0 || s_len == 0)
		return FALSE;

	bcopy(str, tmpstr, s_len + 1);
	tsp = &tmpstr[0];

	while (s_len-- >= f_len) {
		*tsp = f_len;

		if (FastRelString(tsp++, find) == 0)
			return TRUE;
	}

	return FALSE;
}
#endif


/*
 * Check to see if caller has access rights to this item
 */

static int
CheckAccess(ExtendedVCB *theVCBPtr, u_long searchBits, CatalogKey *theKeyPtr, struct vfs_context *ctx)
{
	Boolean				isHFSPlus;
	int					myErr;
	int					myResult; 	
	HFSCatalogNodeID 	myNodeID;
	hfsmount_t *		hfsmp;
	struct FndrDirInfo	*finfop;
	struct vnode * 		vp = NULL;

	myResult = 0;	/* default to "no access" */
		
	if (!vfs_context_suser(ctx))  {
		myResult = 1;	/* allow access */
		goto ExitThisRoutine; /* root always has access */
	}

	hfsmp = VCBTOHFS( theVCBPtr );
	isHFSPlus = ( theVCBPtr->vcbSigWord == kHFSPlusSigWord );
	if ( isHFSPlus )
		myNodeID = theKeyPtr->hfsPlus.parentID;
#if CONFIG_HFS_STD
	else
		myNodeID = theKeyPtr->hfs.parentID;
#endif
	
	while ( myNodeID >= kRootDirID ) {
		cnode_t *	cp;
		
		/* now go get catalog data for this directory */
		myErr = hfs_vget(hfsmp, myNodeID, &vp, 0, 0);
		if ( myErr ) {
			goto ExitThisRoutine;	/* no access */
		}

		cp = VTOC(vp);
		finfop = (struct FndrDirInfo *)&cp->c_attr.ca_finderinfo[0];

		if ( searchBits & SRCHFS_SKIPPACKAGES ) {
		    if (   (SWAP_BE16(finfop->frFlags) & kHasBundle)
			|| (cp->c_desc.cd_nameptr != NULL
			    && is_package_name((const char *)cp->c_desc.cd_nameptr, cp->c_desc.cd_namelen)) ) {
				myResult = 0;
				goto ExitThisRoutine;
		    }
		}

		if ( searchBits & SRCHFS_SKIPINAPPROPRIATE ) {
		    if ( cp->c_parentcnid == kRootDirID && cp->c_desc.cd_nameptr != NULL &&
			     vn_searchfs_inappropriate_name((const char *)cp->c_desc.cd_nameptr, cp->c_desc.cd_namelen) ) {
				myResult = 0;
				goto ExitThisRoutine;
		    }
		}

		if ( (searchBits & SRCHFS_SKIPINVISIBLE) && 
			 (SWAP_BE16(finfop->frFlags) & kIsInvisible) ) {
		    myResult = 0;
		    goto ExitThisRoutine;
		}

		myNodeID = cp->c_parentcnid;	/* move up the hierarchy */
		hfs_unlock(VTOC(vp));

#if CONFIG_MACF
		if (vp->v_type == VDIR) {
			myErr = mac_vnode_check_readdir(ctx, vp);
		} else {
			myErr = mac_vnode_check_stat(ctx, NOCRED, vp);
		}
		if (myErr) {
			vnode_put(vp);
			vp = NULL;
			goto ExitThisRoutine;
		}
#endif /* MAC */

		if (vp->v_type == VDIR) {
		    myErr = vnode_authorize(vp, NULL, (KAUTH_VNODE_SEARCH | KAUTH_VNODE_LIST_DIRECTORY), ctx);
		} else {
		    myErr = vnode_authorize(vp, NULL, (KAUTH_VNODE_SEARCH), ctx);
		}
		vnode_put(vp);
		vp = NULL;
		if ( myErr ) {
			goto ExitThisRoutine;	/* no access */
		}
	}
	myResult = 1;	/* allow access */

ExitThisRoutine:
	if ( vp != NULL ) {
		hfs_unlock(VTOC(vp));
		vnode_put(vp);
	}
	return ( myResult );
	
}

<<<<<<< HEAD
static int
CheckCriteria(	ExtendedVCB *vcb, 
				u_long searchBits,
				struct attrlist *attrList, 
				CatalogRecord *rec, 
				CatalogKey *key,
				searchinfospec_t  *searchInfo1, 
				searchinfospec_t *searchInfo2,
				struct vfs_context *ctx)
=======
Boolean
CheckCriteria(	ExtendedVCB *vcb,
				u_long searchBits, 
				struct attrlist *attrList,
				CatalogNodeData *cnp, 
				CatalogKey *key,
				searchinfospec_t *searchInfo1, 
				searchinfospec_t *searchInfo2,
				Boolean lookForDup )
>>>>>>> origin/10.1
{
	Boolean matched, atleastone;
	Boolean isHFSPlus;
	attrgroup_t searchAttributes;
	struct cat_attr c_attr;
	struct cat_fork datafork;
	struct cat_fork rsrcfork;
	int force_case_sensitivity = proc_is_forcing_hfs_case_sensitivity(vfs_context_proc(ctx));
	
	bzero(&c_attr, sizeof(c_attr));
	isHFSPlus = (vcb->vcbSigWord == kHFSPlusSigWord);

	switch (rec->recordType) {

#if CONFIG_HFS_STD
		case kHFSFolderRecord:
			if ( (searchBits & SRCHFS_MATCHDIRS) == 0 ) {	/* If we are NOT searching folders */
				matched = false;
				goto TestDone;
			}
			break;

		case kHFSFileRecord:
			if ( (searchBits & SRCHFS_MATCHFILES) == 0 ) {	/* If we are NOT searching files */
				matched = false;
				goto TestDone;
			}
			break;
#endif

		case kHFSPlusFolderRecord:
			if ( (searchBits & SRCHFS_MATCHDIRS) == 0 ) {	/* If we are NOT searching folders */
				matched = false;
				goto TestDone;
			}
			break;

		case kHFSPlusFileRecord:
			/* Check if hardlink links should be skipped. */
			if (searchBits & SRCHFS_SKIPLINKS) {
				cnid_t parid = key->hfsPlus.parentID;
				HFSPlusCatalogFile *filep = (HFSPlusCatalogFile *)rec;

				if ((SWAP_BE32(filep->userInfo.fdType) == kHardLinkFileType) &&
						(SWAP_BE32(filep->userInfo.fdCreator) == kHFSPlusCreator)) {
					return (false);	/* skip over file link records */
				} else if ((parid == vcb->hfs_private_desc[FILE_HARDLINKS].cd_cnid) &&
						(filep->bsdInfo.special.linkCount == 0)) {
					return (false);	/* skip over unlinked files */
				} else if ((SWAP_BE32(filep->userInfo.fdType) == kHFSAliasType) &&
						(SWAP_BE32(filep->userInfo.fdCreator) == kHFSAliasCreator) &&
						(filep->flags & kHFSHasLinkChainMask)) {
					return (false);	/* skip over dir link records */
				}
			} else if (key->hfsPlus.parentID == vcb->hfs_private_desc[FILE_HARDLINKS].cd_cnid) {
				return (false);	/* skip over private files */
			} else if (key->hfsPlus.parentID == vcb->hfs_private_desc[DIR_HARDLINKS].cd_cnid) {
				return (false);	/* skip over private files */
			}

			if ( (searchBits & SRCHFS_MATCHFILES) == 0 ) {	/* If we are NOT searching files */
				matched = false;
				goto TestDone;
			}
			break;

		default:	/* Never match a thread record or any other type. */
			return( false );	/* Not a file or folder record, so can't search it */
	}
	
	matched = true;		/* Assume we got a match */
	atleastone = false;	/* Dont insert unless we match at least one criteria */
	
	/* First, attempt to match the name -- either partial or complete */
	if ( attrList->commonattr & ATTR_CMN_NAME ) {
		if (isHFSPlus) {
			int case_sensitive = 0;

			/*
			 * Longstanding default behavior here is to use a non-case-sensitive 
			 * search, even on case-sensitive filesystems. 
			 * 
			 * We only force case sensitivity if the controlling process has explicitly
			 * asked for it in the proc flags, and only if they are not doing
			 * a partial name match.  Consider that if you are doing a partial
			 * name match ("all files that begin with 'image'"), the likelihood is 
			 * high that you would want to see all matches, even those that do not
			 * explicitly match the case.
			 */
			if (force_case_sensitivity) {
				case_sensitive = 1;
			}

			/* Check for partial/full HFS Plus name match */

			if ( searchBits & SRCHFS_MATCHPARTIALNAMES ) {
				/* always use a case-INSENSITIVE search here */
				matched = ComparePartialUnicodeName(key->hfsPlus.nodeName.unicode,
								    key->hfsPlus.nodeName.length,
								    (UniChar*)searchInfo1->name,
								    searchInfo1->nameLength, 0);
			} 
			else {
				/* Full name match.  Are we HFSX (case sensitive) or HFS+ ? */
				if (case_sensitive) {
					matched = (UnicodeBinaryCompare(key->hfsPlus.nodeName.unicode,
								key->hfsPlus.nodeName.length,
								(UniChar*)searchInfo1->name,
								searchInfo1->nameLength ) == 0);
				}
				else {
					matched = (FastUnicodeCompare(key->hfsPlus.nodeName.unicode,
								key->hfsPlus.nodeName.length,
								(UniChar*)searchInfo1->name,
								searchInfo1->nameLength ) == 0);
				}
			}
		}
#if CONFIG_HFS_STD
		else {
			/* Check for partial/full HFS name match */

			if ( searchBits & SRCHFS_MATCHPARTIALNAMES )
				matched = ComparePartialPascalName(key->hfs.nodeName, (u_char*)searchInfo1->name);
			else /* full HFS name match */
				matched = (FastRelString(key->hfs.nodeName, (u_char*)searchInfo1->name) == 0);
		}
<<<<<<< HEAD
#endif

=======

#if 1 // Installer workaround
		if ( lookForDup ) {
			HFSCatalogNodeID parentID;
			if (isHFSPlus)
				parentID = key->hfsPlus.parentID;
			else
				parentID = key->hfs.parentID;
	
			if ( matched && parentID == kRootDirID && 
				 IsTargetName( searchInfo1, isHFSPlus )  )
				matched = false;
		}
#endif // Installer workaround
		
>>>>>>> origin/10.1
		if ( matched == false || (searchBits & ~SRCHFS_MATCHPARTIALNAMES) == 0 )
			goto TestDone;	/* no match, or nothing more to compare */

		atleastone = true;
	}

	/* Convert catalog record into cat_attr format. */
	cat_convertattr(VCBTOHFS(vcb), rec, &c_attr, &datafork, &rsrcfork);
	
	if (searchBits & SRCHFS_SKIPINVISIBLE) {
	    int flags;
	    
	    switch (rec->recordType) {
#if CONFIG_HFS_STD
			case kHFSFolderRecord:
				{
					struct FndrDirInfo *finder_info;

					finder_info = (struct FndrDirInfo *)&c_attr.ca_finderinfo[0];
					flags = SWAP_BE16(finder_info->frFlags);
					break;
				}

			case kHFSFileRecord:
				{
					struct FndrFileInfo *finder_info;

					finder_info = (struct FndrFileInfo *)&c_attr.ca_finderinfo[0];
					flags = SWAP_BE16(finder_info->fdFlags);
					break;
				}
#endif

			case kHFSPlusFolderRecord: 
				{
					struct FndrDirInfo *finder_info;

					finder_info = (struct FndrDirInfo *)&c_attr.ca_finderinfo[0];
					flags = SWAP_BE16(finder_info->frFlags);
					break;
				}

			case kHFSPlusFileRecord: 
				{
					struct FndrFileInfo *finder_info;

					finder_info = (struct FndrFileInfo *)&c_attr.ca_finderinfo[0];
					flags = SWAP_BE16(finder_info->fdFlags);
					break;
				}

			default: 
				{
					flags = kIsInvisible;
					break;
				}
		}

		if (flags & kIsInvisible) {
			matched = false;
			goto TestDone;
		}
	}
	
		    

	/* Now that we have a record worth searching, see if it matches the search attributes */
<<<<<<< HEAD
#if CONFIG_HFS_STD
	if (rec->recordType == kHFSFileRecord ||
	    rec->recordType == kHFSPlusFileRecord) {
#else
	if (rec->recordType == kHFSPlusFileRecord) {
#endif

=======
	if (cnp->cnd_type == kCatalogFileNode) {
>>>>>>> origin/10.1
		if ((attrList->fileattr & ~ATTR_FILE_VALIDMASK) != 0) {	/* attr we do know about  */
			matched = false;
			goto TestDone;
		}
		else if ((attrList->fileattr & ATTR_FILE_VALIDMASK) != 0) {
		searchAttributes = attrList->fileattr;

#if HFS_COMPRESSION
			if ( c_attr.ca_flags & UF_COMPRESSED ) {
				/* for compressed files, set the data length to the uncompressed data size */
				if (( searchAttributes & ATTR_FILE_DATALENGTH ) || 
					( searchAttributes & ATTR_FILE_DATAALLOCSIZE ) ) {
					if ( 0 == hfs_uncompressed_size_of_compressed_file(vcb, NULL, c_attr.ca_fileid, &datafork.cf_size, 1) ) { /* 1 == don't take the cnode lock */
						datafork.cf_blocks = rsrcfork.cf_blocks;
					}	
				}
			/* treat compressed files as if their resource fork is empty */
				if (( searchAttributes & ATTR_FILE_RSRCLENGTH ) || 
					( searchAttributes & ATTR_FILE_RSRCALLOCSIZE ) ) {
					rsrcfork.cf_size = 0;
					rsrcfork.cf_blocks = 0;
				}
			}
#endif /* HFS_COMPRESSION */
			
		/* File logical length (data fork) */
		if ( searchAttributes & ATTR_FILE_DATALENGTH ) {
			matched = CompareWideRange(
			    datafork.cf_size,
			    searchInfo1->f.dataLogicalLength,
			    searchInfo2->f.dataLogicalLength);
			if (matched == false) goto TestDone;
				atleastone = true;
		}
	
		/* File physical length (data fork) */
		if ( searchAttributes & ATTR_FILE_DATAALLOCSIZE ) {
			matched = CompareWideRange(
			    (u_int64_t)datafork.cf_blocks * (u_int64_t)vcb->blockSize,
			    searchInfo1->f.dataPhysicalLength,
			    searchInfo2->f.dataPhysicalLength);
			if (matched == false) goto TestDone;
				atleastone = true;
		}

		/* File logical length (resource fork) */
		if ( searchAttributes & ATTR_FILE_RSRCLENGTH ) {
			matched = CompareWideRange(
			    rsrcfork.cf_size,
			    searchInfo1->f.resourceLogicalLength,
			    searchInfo2->f.resourceLogicalLength);
			if (matched == false) goto TestDone;
				atleastone = true;
		}
		
		/* File physical length (resource fork) */
		if ( searchAttributes & ATTR_FILE_RSRCALLOCSIZE ) {
			matched = CompareWideRange(
			    (u_int64_t)rsrcfork.cf_blocks * (u_int64_t)vcb->blockSize,
			    searchInfo1->f.resourcePhysicalLength,
			    searchInfo2->f.resourcePhysicalLength);
			if (matched == false) goto TestDone;
				atleastone = true;
			}
		}
		else {
			atleastone = true;	/* to match SRCHFS_MATCHFILES */
		}
	}
	/*
	 * Check the directory attributes
	 */
#if CONFIG_HFS_STD
	else if (rec->recordType == kHFSFolderRecord ||
	         rec->recordType == kHFSPlusFolderRecord) {
#else
	else if (rec->recordType == kHFSPlusFolderRecord) {
#endif
		if ((attrList->dirattr & ~ATTR_DIR_VALIDMASK) != 0) {	/* attr we do know about  */
			matched = false;
			goto TestDone;
		}
		else if ((attrList->dirattr & ATTR_DIR_VALIDMASK) != 0) {
		searchAttributes = attrList->dirattr;
		
		/* Directory valence */
		if ( searchAttributes & ATTR_DIR_ENTRYCOUNT ) {
			matched = CompareRange(c_attr.ca_entries,
					searchInfo1->d.numFiles,
					searchInfo2->d.numFiles );
			if (matched == false) goto TestDone;
				atleastone = true;
			}
		}
		else {
			atleastone = true;		/* to match SRCHFS_MATCHDIRS */
		}
	}
	
	/*
	 * Check the common attributes
	 */
	searchAttributes = attrList->commonattr;
	if ( (searchAttributes & ATTR_CMN_VALIDMASK) != 0 ) {
		/* node ID */
		if ( searchAttributes & ATTR_CMN_OBJID ) {
			matched = CompareRange(c_attr.ca_fileid,
					searchInfo1->nodeID,
					searchInfo2->nodeID );
			if (matched == false) goto TestDone;
			atleastone = true;
		}

		/* Parent ID */
		if ( searchAttributes & ATTR_CMN_PAROBJID ) {
			HFSCatalogNodeID parentID;
			
			if (isHFSPlus)
				parentID = key->hfsPlus.parentID;
#if CONFIG_HFS_STD
			else
				parentID = key->hfs.parentID;
#endif
				
			matched = CompareRange(parentID, searchInfo1->parentDirID,
					searchInfo2->parentDirID );
			if (matched == false) goto TestDone;
			atleastone = true;
		}

		/* Finder Info & Extended Finder Info where extFinderInfo is last 32 bytes */
		if ( searchAttributes & ATTR_CMN_FNDRINFO ) {
			u_int32_t *thisValue;
			thisValue = (u_int32_t *) &c_attr.ca_finderinfo;

			/* 
			 * Note: ioFlFndrInfo and ioDrUsrWds have the same offset in search info, so
			 * no need to test the object type here.
			 */
			matched = CompareMasked(thisValue,
					(u_int32_t *)&searchInfo1->finderInfo,
					(u_int32_t *) &searchInfo2->finderInfo, 8);
			if (matched == false) goto TestDone;
			atleastone = true;
		}

		/* Create date */
		if ( searchAttributes & ATTR_CMN_CRTIME ) {
			matched = CompareRange(c_attr.ca_itime,
					searchInfo1->creationDate.tv_sec,
					searchInfo2->creationDate.tv_sec);
			if (matched == false) goto TestDone;
			atleastone = true;
		}
	
		/* Mod date */
		if ( searchAttributes & ATTR_CMN_MODTIME ) {
			matched = CompareRange(c_attr.ca_mtime,
					searchInfo1->modificationDate.tv_sec,
					searchInfo2->modificationDate.tv_sec);
			if (matched == false) goto TestDone;
			atleastone = true;
		}
	
		/* Change Time */
		if ( searchAttributes & ATTR_CMN_CHGTIME ) {
			matched = CompareRange(c_attr.ca_ctime,
					searchInfo1->changeDate.tv_sec,
					searchInfo2->changeDate.tv_sec);
			if (matched == false) goto TestDone;
			atleastone = true;
		}
	
		/* Access date */
		if ( searchAttributes & ATTR_CMN_ACCTIME ) {
			matched = CompareRange(c_attr.ca_atime,
					searchInfo1->accessDate.tv_sec,
					searchInfo2->accessDate.tv_sec);
			if (matched == false) goto TestDone;
			atleastone = true;
		}

		/* Backup date */
		if ( searchAttributes & ATTR_CMN_BKUPTIME ) {
			matched = CompareRange(c_attr.ca_btime,
					searchInfo1->lastBackupDate.tv_sec,
					searchInfo2->lastBackupDate.tv_sec);
			if (matched == false) goto TestDone;
			atleastone = true;
		}
	
		/* User ID */
		if ( searchAttributes & ATTR_CMN_OWNERID ) {
			matched = CompareRange(c_attr.ca_uid,
					searchInfo1->uid, searchInfo2->uid);
			if (matched == false) goto TestDone;
			atleastone = true;
		}

		/* Group ID */
		if ( searchAttributes & ATTR_CMN_GRPID ) {
			matched = CompareRange(c_attr.ca_gid,
					searchInfo1->gid, searchInfo2->gid);
			if (matched == false) goto TestDone;
			atleastone = true;
		}

		/* mode */
		if ( searchAttributes & ATTR_CMN_ACCESSMASK ) {
			matched = CompareRange((u_int32_t)c_attr.ca_mode, 
					(u_int32_t)searchInfo1->mask,
					(u_int32_t)searchInfo2->mask);
			if (matched == false) goto TestDone;
			atleastone = true;
		}
	}

	/* If we got here w/o matching any, then set to false */
	if (! atleastone)
		matched = false;
	
TestDone:
	/*
	 * Finally, determine whether we need to negate the sense of the match
	 * (i.e. find all objects that DON'T match).
	 */
	if ( searchBits & SRCHFS_NEGATEPARAMS )
		matched = !matched;
	
	return( matched );
}


/*
 * Adds another record to the packed array for output
 */
static int
InsertMatch(struct hfsmount *hfsmp, uio_t a_uio, CatalogRecord *rec,
            CatalogKey *key, struct attrlist *returnAttrList,
            void *attributesBuffer, void *variableBuffer, uint32_t * nummatches)
{
	int err;
	void *rovingAttributesBuffer;
	void *rovingVariableBuffer;
	long packedBufferSize;
	struct attrblock attrblk;
	struct cat_desc c_desc;
	struct cat_attr c_attr;
	struct cat_fork datafork;
	struct cat_fork rsrcfork;

	bzero(&c_desc, sizeof(c_desc));
	bzero(&c_attr, sizeof(c_attr));
	rovingAttributesBuffer = (char*)attributesBuffer + sizeof(u_int32_t); /* Reserve space for length field */
	rovingVariableBuffer = variableBuffer;
<<<<<<< HEAD

	/* Convert catalog record into cat_attr format. */
	cat_convertattr(hfsmp, rec, &c_attr, &datafork, &rsrcfork);

	/* Hide our private meta data directories */
	if (c_attr.ca_fileid == hfsmp->hfs_private_desc[FILE_HARDLINKS].cd_cnid ||
	    c_attr.ca_fileid == hfsmp->hfs_private_desc[DIR_HARDLINKS].cd_cnid) {
		err = 0;
		goto exit;
	}

	/* Hide the private journal files */
	if (hfsmp->jnl &&
	    ((c_attr.ca_fileid == hfsmp->hfs_jnlfileid) ||
	     (c_attr.ca_fileid == hfsmp->hfs_jnlinfoblkid))) {
		err = 0;
		goto exit;
	}

	/* Hide the private journal files */
	if (VTOHFS(root_vp)->jnl &&
	    ((c_attr.ca_fileid == VTOHFS(root_vp)->hfs_jnlfileid) ||
	     (c_attr.ca_fileid == VTOHFS(root_vp)->hfs_jnlinfoblkid))) {
		err = 0;
		goto exit;
	}

	if (returnAttrList->commonattr & ATTR_CMN_NAME) {
		err = cat_convertkey(hfsmp, key, rec, &c_desc);
		if (err) {
			/* This means that we probably had a CNID error */
			goto exit;
=======
	
	INIT_CATALOGDATA(&catalogInfo.nodeData, 0);
	catalogInfo.nodeData.cnd_iNodeNumCopy = 0;

	/* The packing call below expects a struct hfsCatalogInfo */
	bcopy(cnp, &catalogInfo.nodeData, (cnp->cnd_type == kCatalogFileNode) ?
	      sizeof(HFSPlusCatalogFile) : sizeof(HFSPlusCatalogFolder));

	catalogInfo.nodeData.cnm_parID = isHFSPlus ? key->hfsPlus.parentID : key->hfs.parentID;

	/* hide open files that have been deleted */
	if ((privateDir != 0) && (catalogInfo.nodeData.cnm_parID == privateDir))
		return (0);

	/* hide our private meta data directory */
	if ((privateDir != 0) && (catalogInfo.nodeData.cnd_nodeID == privateDir))
		return (0);

	if ( returnAttrList->commonattr & ATTR_CMN_NAME ) {
		size_t utf8len = 0;

		catalogInfo.nodeData.cnm_nameptr = catalogInfo.nodeData.cnm_namespace;

		/* Return result in UTF-8 */
		if ( isHFSPlus ) {
			err = utf8_encodestr(key->hfsPlus.nodeName.unicode,
					key->hfsPlus.nodeName.length * sizeof(UniChar),
					catalogInfo.nodeData.cnm_namespace,
					&utf8len,
					MAXHFSVNODELEN + 1, ':', 0);
			if (err == ENAMETOOLONG) {
				utf8len = utf8_encodelen(key->hfsPlus.nodeName.unicode,
						key->hfsPlus.nodeName.length * sizeof(UniChar), ':', 0);
				MALLOC(catalogInfo.nodeData.cnm_nameptr, char *, utf8len+1, M_TEMP, M_WAITOK);
				catalogInfo.nodeData.cnm_flags |= kCatNameIsAllocated;
				err = utf8_encodestr(key->hfsPlus.nodeName.unicode,
						key->hfsPlus.nodeName.length * sizeof(UniChar),
						catalogInfo.nodeData.cnm_nameptr,
						&utf8len,
						utf8len + 1, ':', 0);
			}
		 } else {
			err = hfs_to_utf8(vcb, 
					key->hfs.nodeName,
					MAXHFSVNODELEN + 1,
					(ByteCount*) &utf8len, 
					catalogInfo.nodeData.cnm_namespace);
			if (err == ENAMETOOLONG) {
				MALLOC(catalogInfo.nodeData.cnm_nameptr, char *, utf8len+1, M_TEMP, M_WAITOK);
				catalogInfo.nodeData.cnm_flags |= kCatNameIsAllocated;
				err = hfs_to_utf8(vcb, 
						key->hfs.nodeName, 
						utf8len + 1, 
						(ByteCount*) &utf8len, 
						catalogInfo.nodeData.cnm_nameptr);
			} else if (err) {
				/*
				 * When an HFS name cannot be encoded with the current
				 * volume encoding we use MacRoman as a fallback.
				 */
				err = mac_roman_to_utf8(key->hfs.nodeName, MAXHFSVNODELEN + 1,
				                        (ByteCount*) &utf8len,
				                         catalogInfo.nodeData.cnm_namespace);
			}
		}
		catalogInfo.nodeData.cnm_length = utf8len;
		if (err && (catalogInfo.nodeData.cnm_flags & kCatNameIsAllocated))
		{
			DisposePtr(catalogInfo.nodeData.cnm_nameptr);
			catalogInfo.nodeData.cnm_flags &= ~kCatNameIsAllocated;
                        catalogInfo.nodeData.cnm_nameptr = catalogInfo.nodeData.cnm_namespace;
                        catalogInfo.nodeData.cnm_namespace[0] = 0;
>>>>>>> origin/10.1
		}
	} else {
		c_desc.cd_cnid = c_attr.ca_fileid;
		if ((hfsmp->hfs_flags & HFS_STANDARD) == 0) 
			c_desc.cd_parentcnid = key->hfsPlus.parentID;
#if CONFIG_HFS_STD
		else
			c_desc.cd_parentcnid = key->hfs.parentID;
#endif

	}

	attrblk.ab_attrlist = returnAttrList;
	attrblk.ab_attrbufpp = &rovingAttributesBuffer;
	attrblk.ab_varbufpp = &rovingVariableBuffer;
	attrblk.ab_flags = 0;
	attrblk.ab_blocksize = 0;
	attrblk.ab_context = vfs_context_current();

	hfs_packattrblk(&attrblk, hfsmp, NULL, &c_desc, &c_attr, &datafork, &rsrcfork, vfs_context_current());

	packedBufferSize = (char*)rovingVariableBuffer - (char*)attributesBuffer;

	if ( packedBufferSize > uio_resid(a_uio) )
		return( errSearchBufferFull );

   	(* nummatches)++;
	
	*((u_int32_t *)attributesBuffer) = packedBufferSize;	/* Store length of fixed + var block */
	
	err = uiomove( (caddr_t)attributesBuffer, packedBufferSize, a_uio );
exit:
	cat_releasedesc(&c_desc);
	
	return( err );
}


static int
UnpackSearchAttributeBlock( struct hfsmount *hfsmp, struct attrlist	*alist, 
		searchinfospec_t *searchInfo, void *attributeBuffer, int firstblock)
{
	attrgroup_t		a;
	u_int32_t			bufferSize;
	boolean_t       is_64_bit;

    DBG_ASSERT(searchInfo != NULL);
    
    is_64_bit = proc_is64bit(current_proc());

    bufferSize = *((u_int32_t *)attributeBuffer);
	if (bufferSize == 0)
		return (EINVAL);	/* XXX -DJB is a buffer size of zero ever valid for searchfs? */

	attributeBuffer = (u_int32_t *)attributeBuffer + 1; /* advance past the size */
	
	/* 
	 * UnPack common attributes
	 */
	a = alist->commonattr;
	if ( a != 0 ) {
		if ( a & ATTR_CMN_NAME ) {
			if (firstblock) {
				/* Only use the attrreference_t for the first searchparams */
				char *s;
				u_int32_t len;

				s = (char*) attributeBuffer + ((attrreference_t *) attributeBuffer)->attr_dataoffset;
				len = ((attrreference_t *) attributeBuffer)->attr_length;

				if (len > sizeof(searchInfo->name))
					return (EINVAL);


				if ((hfsmp->hfs_flags & HFS_STANDARD) == 0) {
					size_t ucslen;
					/* Convert name to Unicode to match HFS Plus B-Tree names */

					if (len > 0) {
						if (utf8_decodestr((u_int8_t *)s, len-1, (UniChar*)searchInfo->name, &ucslen,
									sizeof(searchInfo->name), ':', UTF_DECOMPOSED | UTF_ESCAPE_ILLEGAL))
							return (EINVAL);

						searchInfo->nameLength = ucslen / sizeof(UniChar);
					} else {
						searchInfo->nameLength = 0;
					}
				}
#if CONFIG_HFS_STD
				else {
					/* Convert name to pascal string to match HFS (Standard) B-Tree names */

					if (len > 0) {
						if (utf8_to_hfs(HFSTOVCB(hfsmp), len-1, (u_char *)s, (u_char*)searchInfo->name) != 0)
							return (EINVAL);

						searchInfo->nameLength = searchInfo->name[0];
					} else {
						searchInfo->name[0] = searchInfo->nameLength = 0;
					}
				}
#endif	
			}
			attributeBuffer = (attrreference_t*) attributeBuffer +1;
		}
		if ( a & ATTR_CMN_OBJID ) {
			searchInfo->nodeID = ((fsobj_id_t *) attributeBuffer)->fid_objno;	/* ignore fid_generation */
			attributeBuffer = (fsobj_id_t *)attributeBuffer + 1;
		}
		if ( a & ATTR_CMN_PAROBJID ) {
			searchInfo->parentDirID = ((fsobj_id_t *) attributeBuffer)->fid_objno;  /* ignore fid_generation */
			attributeBuffer = (fsobj_id_t *)attributeBuffer + 1;
		}

		if ( a & ATTR_CMN_CRTIME ) {
            if (is_64_bit) {
                struct user64_timespec tmp;
                tmp = *((struct user64_timespec *)attributeBuffer);
                searchInfo->creationDate.tv_sec = (time_t)tmp.tv_sec;
                searchInfo->creationDate.tv_nsec = tmp.tv_nsec;
				attributeBuffer = (struct user64_timespec *)attributeBuffer + 1;
            }
            else {
                struct user32_timespec tmp;
                tmp = *((struct user32_timespec *)attributeBuffer);
                searchInfo->creationDate.tv_sec = (time_t)tmp.tv_sec;
                searchInfo->creationDate.tv_nsec = tmp.tv_nsec;
				attributeBuffer = (struct user32_timespec *)attributeBuffer + 1;
            }
		}
		if ( a & ATTR_CMN_MODTIME ) {
            if (is_64_bit) {
                struct user64_timespec tmp;
                tmp = *((struct user64_timespec *)attributeBuffer);
                searchInfo->modificationDate.tv_sec = (time_t)tmp.tv_sec;
                searchInfo->modificationDate.tv_nsec = tmp.tv_nsec;
				attributeBuffer = (struct user64_timespec *)attributeBuffer + 1;
            }
            else {
                struct user32_timespec tmp;
                tmp = *((struct user32_timespec *)attributeBuffer);
                searchInfo->modificationDate.tv_sec = (time_t)tmp.tv_sec;
                searchInfo->modificationDate.tv_nsec = tmp.tv_nsec;
				attributeBuffer = (struct user32_timespec *)attributeBuffer + 1;
            }
		}
		if ( a & ATTR_CMN_CHGTIME ) {
            if (is_64_bit) {
                struct user64_timespec tmp;
                tmp = *((struct user64_timespec *)attributeBuffer);
                searchInfo->changeDate.tv_sec = (time_t)tmp.tv_sec;
                searchInfo->changeDate.tv_nsec = tmp.tv_nsec;
				attributeBuffer = (struct user64_timespec *)attributeBuffer + 1;
            }
            else {
                struct user32_timespec tmp;
                tmp = *((struct user32_timespec *)attributeBuffer);
                searchInfo->changeDate.tv_sec = (time_t)tmp.tv_sec;
                searchInfo->changeDate.tv_nsec = tmp.tv_nsec;
				attributeBuffer = (struct user32_timespec *)attributeBuffer + 1;
            }
		}
		if ( a & ATTR_CMN_ACCTIME ) {
            if (is_64_bit) {
                struct user64_timespec tmp;
                tmp = *((struct user64_timespec *)attributeBuffer);
                searchInfo->accessDate.tv_sec = (time_t)tmp.tv_sec;
                searchInfo->accessDate.tv_nsec = tmp.tv_nsec;
				attributeBuffer = (struct user64_timespec *)attributeBuffer + 1;
            }
            else {
                struct user32_timespec tmp;
                tmp = *((struct user32_timespec *)attributeBuffer);
                searchInfo->accessDate.tv_sec = (time_t)tmp.tv_sec;
                searchInfo->accessDate.tv_nsec = tmp.tv_nsec;
				attributeBuffer = (struct user32_timespec *)attributeBuffer + 1;
            }
		}
		if ( a & ATTR_CMN_BKUPTIME ) {
            if (is_64_bit) {
                struct user64_timespec tmp;
                tmp = *((struct user64_timespec *)attributeBuffer);
                searchInfo->lastBackupDate.tv_sec = (time_t)tmp.tv_sec;
                searchInfo->lastBackupDate.tv_nsec = tmp.tv_nsec;
				attributeBuffer = (struct user64_timespec *)attributeBuffer + 1;
            }
            else {
                struct user32_timespec tmp;
                tmp = *((struct user32_timespec *)attributeBuffer);
                searchInfo->lastBackupDate.tv_sec = (time_t)tmp.tv_sec;
                searchInfo->lastBackupDate.tv_nsec = tmp.tv_nsec;
				attributeBuffer = (struct user32_timespec *)attributeBuffer + 1;
            }
		}
		if ( a & ATTR_CMN_FNDRINFO ) {
			bcopy( attributeBuffer, searchInfo->finderInfo, sizeof(searchInfo->finderInfo) );
			attributeBuffer = (u_int8_t *)attributeBuffer + 32;
		}
		if ( a & ATTR_CMN_OWNERID ) {
			searchInfo->uid = *((uid_t *)attributeBuffer);
			attributeBuffer = (uid_t *)attributeBuffer + 1;
		}
		if ( a & ATTR_CMN_GRPID ) {
			searchInfo->gid = *((gid_t *)attributeBuffer);
			attributeBuffer = (gid_t *)attributeBuffer + 1;
		}
		if ( a & ATTR_CMN_ACCESSMASK ) {
			searchInfo->mask = *((mode_t *)attributeBuffer);
			attributeBuffer = (mode_t *)attributeBuffer + 1;
		}
		if ( a & ATTR_CMN_FILEID ) {
			searchInfo->nodeID = (u_int32_t)*((u_int64_t *) attributeBuffer);	
			attributeBuffer = (u_int64_t *)attributeBuffer + 1;
		}
		if ( a & ATTR_CMN_PARENTID ) {
			searchInfo->parentDirID = (u_int32_t)*((u_int64_t *) attributeBuffer);	
			attributeBuffer = (u_int64_t *)attributeBuffer + 1;
		}
	}

	a = alist->dirattr;
	if ( a != 0 ) {
		if ( a & ATTR_DIR_ENTRYCOUNT ) {
			searchInfo->d.numFiles = *((u_int32_t *)attributeBuffer);
			attributeBuffer = (u_int32_t *)attributeBuffer + 1;
		}
	}

	a = alist->fileattr;
	if ( a != 0 ) {
		if ( a & ATTR_FILE_DATALENGTH ) {
			searchInfo->f.dataLogicalLength = *((off_t *)attributeBuffer);
			attributeBuffer = (off_t *)attributeBuffer + 1;
		}
		if ( a & ATTR_FILE_DATAALLOCSIZE ) {
			searchInfo->f.dataPhysicalLength = *((off_t *)attributeBuffer);
			attributeBuffer = (off_t *)attributeBuffer + 1;
		}
		if ( a & ATTR_FILE_RSRCLENGTH ) {
			searchInfo->f.resourceLogicalLength = *((off_t *)attributeBuffer);
			attributeBuffer = (off_t *)attributeBuffer + 1;
		}
		if ( a & ATTR_FILE_RSRCALLOCSIZE ) {
			searchInfo->f.resourcePhysicalLength = *((off_t *)attributeBuffer);
			attributeBuffer = (off_t *)attributeBuffer + 1;
		}
	}

	return (0);
}
<<<<<<< HEAD
#endif	/* CONFIG_SEARCHFS */
=======


/* this routine was added as part of the work around where some installers would fail */
/* because they incorrectly assumed search results were in some kind of order.  */
/* This routine is used to indentify the problematic target.  At this point we */
/* only know of one.  This routine could be modified for more (I hope not). */
static Boolean IsTargetName( searchinfospec_t * searchInfoPtr, Boolean isHFSPlus )
{
	if ( searchInfoPtr->name == NULL )
		return( false );
		
	if (isHFSPlus) {
		HFSUniStr255 myName = {
			7, 	/* number of unicode characters */
			{
				'L','i','b','r','a','r','y'
			}
		};		
		if ( FastUnicodeCompare( myName.unicode, myName.length,
								 (UniChar*)searchInfoPtr->name,
								 searchInfoPtr->nameLength ) == 0 )  {
			return( true );
		}
						  
	} else {
		u_char		myName[32] = {
			0x07,'L','i','b','r','a','r','y'
		};
		if ( FastRelString(myName, (u_char*)searchInfoPtr->name) == 0 )  {
			return( true );
		}
	}
	return( false );
	
} /* IsTargetName */


>>>>>>> origin/10.1
