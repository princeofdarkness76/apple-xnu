/*
 * Copyright (c) 2000-2005 Apple Computer, Inc. All rights reserved.
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
 */
#include <sys/param.h>
#include <sys/systm.h>
#include <sys/kernel.h>
#include <sys/malloc.h>
#include <sys/types.h>
<<<<<<< HEAD
=======
#include <sys/ubc.h>
#include <sys/vm.h>
#include "hfs.h"
#include "hfs_dbg.h"
>>>>>>> origin/10.1

#include <pexpert/pexpert.h>

#include "hfs.h"
#include "hfs_dbg.h"
#include "hfscommon/headers/FileMgrInternal.h"

/* 
 * gTimeZone should only be used for HFS volumes!
 * It is initialized when an HFS volume is mounted.
 */
struct timezone gTimeZone = {8*60,1};

<<<<<<< HEAD
=======

/*										*/
/*	Creates a new vnode to hold a psuedo file like an extents tree file	*/
/*										*/

OSStatus  GetInitializedVNode(struct hfsmount *hfsmp, struct vnode **tmpvnode)
{

    struct hfsnode	*hp;
    struct vnode 	*vp = NULL;
    int				rtn;

    DBG_ASSERT(hfsmp != NULL);
    DBG_ASSERT(tmpvnode != NULL);

    /* Allocate a new hfsnode. */
    /*
	 * Must do malloc() before getnewvnode(), since malloc() can block
	 * and could cause other part of the system to access v_data
	 * which has not been initialized yet
	 */
    MALLOC_ZONE(hp, struct hfsnode *, sizeof(struct hfsnode), M_HFSNODE, M_WAITOK);
    if(hp == NULL) {
        rtn = ENOMEM;
        goto Err_Exit;
    }
    bzero((caddr_t)hp, sizeof(struct hfsnode));
    lockinit(&hp->h_lock, PINOD, "hfsnode", 0, 0);

    MALLOC_ZONE(hp->h_meta, struct hfsfilemeta *, 
		sizeof(struct hfsfilemeta), M_HFSFMETA, M_WAITOK);
    /* Allocate a new vnode. */
    if ((rtn = getnewvnode(VT_HFS, HFSTOVFS(hfsmp), hfs_vnodeop_p, &vp))) {
		FREE_ZONE(hp->h_meta, sizeof(struct hfsfilemeta), M_HFSFMETA);
		FREE_ZONE(hp, sizeof(struct hfsnode), M_HFSNODE);
        goto Err_Exit;
    }

    /* Init the structure */
    bzero(hp->h_meta, sizeof(struct hfsfilemeta));

    hp->h_vp = vp;									/* Make HFSTOV work */
    hp->h_meta->h_devvp = hfsmp->hfs_devvp;
    hp->h_meta->h_dev = hfsmp->hfs_raw_dev;
    hp->h_meta->h_usecount++;
    hp->h_nodeflags |= IN_ACCESS | IN_CHANGE | IN_UPDATE;
	rl_init(&hp->h_invalidranges);
#if HFS_DIAGNOSTIC
    hp->h_valid = HFS_VNODE_MAGIC;
#endif
    vp->v_data = hp;								/* Make VTOH work */
    vp->v_type = VREG;
	/*
	 * Metadata files are VREG but not available for IO
	 * through mapped IO as will as POSIX IO APIs.
	 * Hence we do not initialize UBC for those files
	 */
	vp->v_ubcinfo = UBC_NOINFO;

    *tmpvnode = vp;
    
    VREF(hp->h_meta->h_devvp);

    return noErr;

Err_Exit:
    
    *tmpvnode = NULL;

    return rtn;
}

OSErr	C_FlushMDB( ExtendedVCB *volume)
{
	short	err;

	if (volume->vcbSigWord == kHFSPlusSigWord)
		err = hfs_flushvolumeheader(VCBTOHFS(volume), 0);
	else
		err = hfs_flushMDB(VCBTOHFS(volume), 0);

	return err;
}


>>>>>>> origin/10.1
/*
 * GetTimeUTC - get the GMT Mac OS time (in seconds since 1/1/1904)
 *
 * called by the Catalog Manager when creating/updating HFS Plus records
 */
u_int32_t GetTimeUTC(void)
{
	struct timeval tv;

	microtime(&tv);

	return (tv.tv_sec + MAC_GMT_FACTOR);
}


/*
 * LocalToUTC - convert from Mac OS local time to Mac OS GMT time.
 * This should only be called for HFS volumes (not for HFS Plus).
 */
u_int32_t LocalToUTC(u_int32_t localTime)
{
	u_int32_t gtime = localTime;
	
	if (gtime != 0) {
		gtime += (gTimeZone.tz_minuteswest * 60);
	/*
	 * We no longer do DST adjustments here since we don't
	 * know if time supplied needs adjustment!
	 *
	 * if (gTimeZone.tz_dsttime)
	 *     gtime -= 3600;
	 */
	}
    return (gtime);
}

/*
 * UTCToLocal - convert from Mac OS GMT time to Mac OS local time.
 * This should only be called for HFS volumes (not for HFS Plus).
 */
u_int32_t UTCToLocal(u_int32_t utcTime)
{
	u_int32_t ltime = utcTime;
	
	if (ltime != 0) {
		ltime -= (gTimeZone.tz_minuteswest * 60);
	/*
	 * We no longer do DST adjustments here since we don't
	 * know if time supplied needs adjustment!
	 *
	 * if (gTimeZone.tz_dsttime)
	 *     ltime += 3600;
	 */
	}
    return (ltime);
}

/*
 * to_bsd_time - convert from Mac OS time (seconds since 1/1/1904)
 *		 to BSD time (seconds since 1/1/1970)
 */
time_t to_bsd_time(u_int32_t hfs_time)
{
	u_int32_t gmt = hfs_time;

	if (gmt > MAC_GMT_FACTOR)
		gmt -= MAC_GMT_FACTOR;
	else
		gmt = 0;	/* don't let date go negative! */

	return (time_t)gmt;
}

/*
 * to_hfs_time - convert from BSD time (seconds since 1/1/1970)
 *		 to Mac OS time (seconds since 1/1/1904)
 */
u_int32_t to_hfs_time(time_t bsd_time)
{
	u_int32_t hfs_time = (u_int32_t)bsd_time;

	/* don't adjust zero - treat as uninitialzed */
	if (hfs_time != 0)
		hfs_time += MAC_GMT_FACTOR;

	return (hfs_time);
}


Ptr  NewPtrSysClear (Size byteCount)
{
    Ptr		tmptr;
    MALLOC (tmptr, Ptr, byteCount, M_TEMP, M_WAITOK);
    if (tmptr)
        bzero(tmptr, byteCount);
    return tmptr;
}



Ptr  NewPtr (Size byteCount)
{
    Ptr		tmptr;
    MALLOC (tmptr, Ptr, byteCount, M_TEMP, M_WAITOK);
    return tmptr;
}


void DisposePtr (Ptr p)
{
    FREE (p, M_TEMP);
}


void
DebugStr(
	const char * debuggerMsg
	)
{
    kprintf ("*** Mac OS Debugging Message: %s\n", debuggerMsg);
	DEBUG_BREAK;
}

