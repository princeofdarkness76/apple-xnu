/*
 * Copyright (c) 2000-2014 Apple Computer, Inc. All rights reserved.
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
 * Copyright (c) 1982, 1986, 1989, 1993
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
 *	@(#)buf.h	8.9 (Berkeley) 3/30/95
 */

#ifndef _SYS_BUF_H_
#define	_SYS_BUF_H_

#include <sys/cdefs.h>
#include <sys/kernel_types.h>
#include <sys/ucred.h>
#include <mach/memory_object_types.h>


#define	B_WRITE		0x00000000	/* Write buffer (pseudo flag). */
#define	B_READ		0x00000001	/* Read buffer. */
#define	B_ASYNC		0x00000002	/* Start I/O, do not wait. */
#define	B_NOCACHE	0x00000004	/* Do not cache block after use. */
#define	B_DELWRI	0x00000008	/* Delay I/O until buffer reused. */
#define	B_LOCKED	0x00000010	/* Locked in core (not reusable). */
#define	B_PHYS		0x00000020	/* I/O to user memory. */
#define B_CLUSTER	0x00000040	/* UPL based I/O generated by cluster layer */
#define	B_PAGEIO	0x00000080	/* Page in/out */
#define	B_META		0x00000100	/* buffer contains meta-data. */
#define	B_RAW		0x00000200	/* Set by physio for raw transfers. */
#define	B_FUA		0x00000400	/* Write-through disk cache(if supported) */
#define B_PASSIVE	0x00000800	/* PASSIVE I/Os are ignored by THROTTLE I/O */
#define	B_IOSTREAMING	0x00001000	/* sequential access pattern detected */
#define B_THROTTLED_IO	0x00002000	/* low priority I/O (deprecated) */
#define B_ENCRYPTED_IO	0x00004000	/* Encrypted I/O */
#define B_STATICCONTENT 0x00008000	/* Buffer is likely to remain unaltered */

/*
 * make sure to check when adding flags that
 * that the new flags don't overlap the definitions
 * in buf_internal.h
 */

__BEGIN_DECLS

/*!
 @function buf_markaged
 @abstract Mark a buffer as "aged," i.e. as a good candidate to be discarded and reused after buf_brelse().
 @param bp Buffer to mark.
 */
void	buf_markaged(buf_t);

/*!
 @function buf_markinvalid
 @abstract Mark a buffer as not having valid data and being ready for immediate reuse after buf_brelse().
 @param bp Buffer to mark.
 */
void	buf_markinvalid(buf_t);

/*!
 @function buf_markdelayed
 @abstract Mark a buffer as a delayed write: mark it dirty without actually scheduling I/O.
 @discussion Data will be flushed to disk at some later time, not with brelse(). A sync()/fsync()
 or pressure necessitating reuse of the buffer will cause it to be written back to disk.
 @param bp Buffer to mark.
 */
void	buf_markdelayed(buf_t);

void	buf_markclean(buf_t);

/*!
 @function buf_markeintr
 @abstract Mark a buffer as having been interrupted during I/O.
 @discussion Waiters for I/O to complete (buf_biowait()) will return with EINTR when woken up.  
 buf_markeintr does not itself do a wakeup.
 @param bp Buffer to mark.
 */
void	buf_markeintr(buf_t);

/*!
 @function buf_markfua
 @abstract Mark a buffer for write through disk cache, if disk supports it.
 @param bp Buffer to mark.
 */
void	buf_markfua(buf_t);

/*!
 @function buf_fua
 @abstract Check if a buffer is marked for write through disk caches.
 @param bp Buffer to test.
 @return Nonzero if buffer is marked for write-through, 0 if not.
 */
int	buf_fua(buf_t);

/*!
 @function buf_valid
 @abstract Check if a buffer contains valid data.
 @param bp Buffer to test.
 @return Nonzero if buffer has valid data, 0 if not.
 */
int	buf_valid(buf_t);

/*!
 @function buf_fromcache
 @abstract Check if a buffer's data was found in core.
 @discussion Will return truth after a buf_getblk that finds a valid buffer in the cache or the relevant
 data in core (but not in a buffer).
 @param bp Buffer to test.
 @return Nonzero if we got this buffer's data without doing I/O, 0 if not.
 */
int	buf_fromcache(buf_t);

/*!
 @function buf_upl
 @abstract Get the upl (Universal Page List) associated with a buffer.
 @discussion Buffers allocated with buf_alloc() are not returned with a upl, and
 traditional buffers only have a upl while an I/O is in progress.
 @param bp Buffer whose upl to grab.
 @return Buffer's upl if it has one, else NULL.
 */
void *	buf_upl(buf_t);

/*!
 @function buf_uploffset
 @abstract Get the offset into a UPL at which this buffer begins.
 @discussion This function should only be called on iobufs, i.e. buffers allocated with buf_alloc().
 @param bp Buffer whose uploffset to grab.
 @return Buffer's uploffset--does not check whether that value makes sense for this buffer.
 */
uint32_t buf_uploffset(buf_t);

/*!
 @function buf_rcred
 @abstract Get the credential associated with a buffer for reading.
 @discussion No reference is taken; if the credential is to be held on to persistently, an additional
 reference must be taken with kauth_cred_ref.
 @param bp Buffer whose credential to grab.
 @return Credential if it exists, else NULL.
 */
kauth_cred_t buf_rcred(buf_t);

/*!
 @function buf_wcred
 @abstract Get the credential associated with a buffer for writing.
 @discussion No reference is taken; if the credential is to be held on to persistently, an additional
 reference must be taken with kauth_cred_ref.
 @param bp Buffer whose credential to grab.
 @return Credential if it exists, else NULL.
 */
kauth_cred_t buf_wcred(buf_t);

/*!
 @function buf_proc
 @abstract Get the process associated with this buffer.
 @discussion buf_proc() will generally return NULL; a process is currently only associated with
 a buffer in the event of a physio() call.  
 @param bp Buffer whose associated process to find.
 @return Associated process, possibly NULL.
 */
proc_t	buf_proc(buf_t);

/*!
 @function buf_dirtyoff
 @abstract Get the starting offset of the dirty region associated with a buffer.
 @discussion The dirty offset is zero unless someone explicitly calls buf_setdirtyoff() (which the kernel does not).
 @param bp Buffer whose dirty offset to get.
 @return Dirty offset (0 if not explicitly changed).
 */
uint32_t buf_dirtyoff(buf_t);

/*!
 @function buf_dirtyend
 @abstract Get the ending offset of the dirty region associated with a buffer.
 @discussion If the buffer's data was found incore and dirty, the dirty end is the size of the block; otherwise, unless
 someone outside of xnu explicitly changes it by calling buf_setdirtyend(), it will be zero.
 @param bp Buffer whose dirty end to get.
 @return 0 if buffer is found clean; size of buffer if found dirty.  Can be set to any value by callers of buf_setdirtyend().
 */
uint32_t buf_dirtyend(buf_t);

/*!
 @function buf_setdirtyoff
 @abstract Set the starting offset of the dirty region associated with a buffer.
 @discussion This value is zero unless someone set it explicitly.
 @param bp Buffer whose dirty end to set.
 @return void.
 */
void	buf_setdirtyoff(buf_t, uint32_t);

/*!
 @function buf_setdirtyend
 @abstract Set the ending offset of the dirty region associated with a buffer.
 @discussion If the buffer's data was found incore and dirty, the dirty end is the size of the block; otherwise, unless
 someone outside of xnu explicitly changes it by calling buf_setdirtyend(), it will be zero.
 @param bp Buffer whose dirty end to set.
 @return void.
 */
void	buf_setdirtyend(buf_t, uint32_t);

/*!
 @function buf_error
 @abstract Get the error value associated with a buffer.
 @discussion Errors are set with buf_seterror().
 @param bp Buffer whose error value to retrieve.
 @return Error value, directly.
 */
errno_t	buf_error(buf_t);

/*!
 @function buf_seterror
 @abstract Set an error value on a buffer.
 @param bp Buffer whose error value to set.
 @return void.
 */
void	buf_seterror(buf_t, errno_t);

/*!
 @function buf_setflags
 @abstract Set flags on a buffer.
 @discussion buffer_flags |= flags
 @param bp Buffer whose flags to set.
 @param flags Flags to add to buffer's mask. B_LOCKED/B_NOCACHE/B_ASYNC/B_READ/B_WRITE/B_PAGEIO/B_FUA
 @return void.
 */
void	buf_setflags(buf_t, int32_t);

/*!
 @function buf_clearflags
 @abstract Clear flags on a buffer.
 @discussion buffer_flags &= ~flags
 @param bp Buffer whose flags to clear.
 @param flags Flags to remove from buffer's mask. B_LOCKED/B_NOCACHE/B_ASYNC/B_READ/B_WRITE/B_PAGEIO/B_FUA
 @return void.
 */
void	buf_clearflags(buf_t, int32_t);

/*!
 @function buf_flags
 @abstract Get flags set on a buffer.
 @discussion Valid flags are B_LOCKED/B_NOCACHE/B_ASYNC/B_READ/B_WRITE/B_PAGEIO/B_FUA.
 @param bp Buffer whose flags to grab.
 @return flags.
 */
int32_t	buf_flags(buf_t);

/*!
 @function buf_reset
 @abstract Reset I/O flag state on a buffer.
 @discussion Clears current flags on a buffer (internal and external) and allows some new flags to be set.
 Used perhaps to prepare an iobuf for reuse.
 @param bp Buffer whose flags to grab.  
 @param flags Flags to set on buffer: B_READ, B_WRITE, B_ASYNC, B_NOCACHE.
 @return void.
 */
void	buf_reset(buf_t, int32_t);

/*!
 @function buf_map
 @abstract Get virtual mappings for buffer data.
 @discussion For buffers created through buf_getblk() (i.e. traditional buffer cache usage), 
 buf_map() just returns the address at which data was mapped by but_getblk().  For a B_CLUSTER buffer, i.e. an iobuf
 whose upl state is managed manually, there are two possibilities.  If the buffer was created
 with an underlying "real" buffer through cluster_bp(), the mapping of the "real" buffer is returned.
 Otherwise, the buffer was created with buf_alloc() and buf_setupl() was subsequently called; buf_map()
 will call ubc_upl_map() to get a mapping for the buffer's upl and return the start of that mapping
 plus the buffer's upl offset (set in buf_setupl()).  In the last case, buf_unmap() must later be called
 to tear down the mapping.  NOTE: buf_map() does not set the buffer data pointer; this must be done with buf_setdataptr().
 @param bp Buffer whose mapping to find or create.
 @param io_addr Destination for mapping address.
 @return 0 for success, ENOMEM if unable to map the buffer.
 */
errno_t	buf_map(buf_t, caddr_t *);

/*!
 @function buf_unmap
 @abstract Release mappings for buffer data.
 @discussion For buffers created through buf_getblk() (i.e. traditional buffer cache usage), 
 buf_unmap() does nothing; buf_brelse() will take care of unmapping.  For a B_CLUSTER buffer, i.e. an iobuf
 whose upl state is managed manually, there are two possibilities.  If the buffer was created
 with an underlying "real" buffer through cluster_bp(), buf_unmap() does nothing; buf_brelse() on the
 underlying buffer will tear down the mapping. Otherwise, the buffer was created with buf_alloc() and 
 buf_setupl() was subsequently called; buf_map() created the mapping.  In this case, buf_unmap() will
 unmap the buffer.
 @param bp Buffer whose mapping to find or create.
 @param io_addr Destination for mapping address.
 @return 0 for success, EINVAL if unable to unmap buffer.
 */
errno_t	buf_unmap(buf_t);

/*!
 @function buf_setdrvdata
 @abstract Set driver-specific data on a buffer.
 @param bp Buffer whose driver-data to set.
 @param drvdata Opaque driver data.
 @return void.
 */
void 	buf_setdrvdata(buf_t, void *);

/*!
 @function buf_setdrvdata
 @abstract Get driver-specific data from a buffer.
 @param bp Buffer whose driver data to get.
 @return Opaque driver data.
 */
void *	buf_drvdata(buf_t);

/*!
 @function buf_setfsprivate
 @abstract Set filesystem-specific data on a buffer.
 @param bp Buffer whose filesystem data to set.
 @param fsprivate Opaque filesystem data.
 @return void.
 */
void 	buf_setfsprivate(buf_t, void *);

/*!
 @function buf_fsprivate
 @abstract Get filesystem-specific data from a buffer.
 @param bp Buffer whose filesystem data to get.
 @return Opaque filesystem data.
 */
void *	buf_fsprivate(buf_t);

/*!
 @function buf_blkno
 @abstract Get physical block number associated with a buffer, in the sense of VNOP_BLOCKMAP.
 @discussion When a buffer's physical block number is the same is its logical block number, then the physical
 block number is considered uninitialized.  A physical block number of -1 indicates that there is no valid
 physical mapping (e.g. the logical block is invalid or corresponds to a sparse region in a file).  Physical
 block number is normally set by the cluster layer or by buf_getblk().
 @param bp Buffer whose physical block number to get.
 @return Block number.
 */
daddr64_t buf_blkno(buf_t);

/*!
 @function buf_lblkno
 @abstract Get logical block number associated with a buffer.
 @discussion Logical block number is set on traditionally-used buffers by an argument passed to buf_getblk(),
 for example by buf_bread().
 @param bp Buffer whose logical block number to get.
 @return Block number.
 */
daddr64_t buf_lblkno(buf_t);

/*!
 @function buf_setblkno
 @abstract Set physical block number associated with a buffer.
 @discussion Physical block number is generally set by the cluster layer or by buf_getblk().
 @param bp Buffer whose physical block number to set.
 @param blkno Block number to set.
 @return void.
 */
void	buf_setblkno(buf_t, daddr64_t);

/*!
 @function buf_setlblkno
 @abstract Set logical block number associated with a buffer.
 @discussion Logical block number is set on traditionally-used buffers by an argument passed to buf_getblk(),
 for example by buf_bread().
 @param bp Buffer whose logical block number to set.
 @param lblkno Block number to set.
 @return void.
 */
void	buf_setlblkno(buf_t, daddr64_t);

/*!
 @function buf_count
 @abstract Get count of valid bytes in a buffer.  This may be less than the space allocated to the buffer.
 @param bp Buffer whose byte count to get.
 @return Byte count.
 */
uint32_t buf_count(buf_t);

/*!
 @function buf_size
 @abstract Get size of data region allocated to a buffer.
 @discussion May be larger than amount of valid data in buffer.
 @param bp Buffer whose size to get.
 @return Size.
 */
uint32_t buf_size(buf_t);

/*!
 @function buf_resid
 @abstract Get a count of bytes which were not consumed by an I/O on a buffer.
 @discussion Set when an I/O operations completes.
 @param bp Buffer whose outstanding count to get.
 @return Count of unwritten/unread bytes.
 */
uint32_t buf_resid(buf_t);

/*!
 @function buf_setcount
 @abstract Set count of valid bytes in a buffer.  This may be less than the space allocated to the buffer.
 @param bp Buffer whose byte count to set.
 @param bcount Count to set.
 @return void.
 */
void	buf_setcount(buf_t, uint32_t);

/*!
 @function buf_setsize
 @abstract Set size of data region allocated to a buffer.
 @discussion May be larger than amount of valid data in buffer.  Should be used by 
 code which is manually providing storage for an iobuf, one allocated with buf_alloc().
 @param bp Buffer whose size to set.
 @return void.
 */
void	buf_setsize(buf_t, uint32_t);

/*!
 @function buf_setresid
 @abstract Set a count of bytes outstanding for I/O in a buffer.
 @discussion Set when an I/O operations completes.  Examples: called by IOStorageFamily when I/O
 completes, often called on an "original" buffer when using a manipulated buffer to perform I/O 
 on behalf of the first.
 @param bp Buffer whose outstanding count to set.
 @return Count of unwritten/unread bytes.
 */
void	buf_setresid(buf_t, uint32_t);

/*!
 @function buf_setdataptr
 @abstract Set the address at which a buffer's data will be stored.
 @discussion In traditional buffer use, the data pointer will be set automatically. This routine is
 useful with iobufs (allocated with buf_alloc()).
 @param bp Buffer whose data pointer to set.
 @param data Pointer to data region.
 @return void.
 */
void	buf_setdataptr(buf_t, uintptr_t);

/*!
 @function buf_dataptr
 @abstract Get the address at which a buffer's data is stored; for iobufs, this must
 be set with buf_setdataptr().  See buf_map().
 @param bp Buffer whose data pointer to retrieve.
 @return Data pointer; NULL if unset.
 */
uintptr_t buf_dataptr(buf_t);

/*!
 @function buf_vnode
 @abstract Get the vnode associated with a buffer.
 @discussion Every buffer is associated with a file.  Because there is an I/O in flight,
 there is an iocount on this vnode; it is returned WITHOUT an extra iocount, and vnode_put()
 need NOT be called.
 @param bp Buffer whose vnode to retrieve.
 @return Buffer's vnode.
 */
vnode_t	buf_vnode(buf_t);

/*!
 @function buf_setvnode
 @abstract Set the vnode associated with a buffer.
 @discussion This call need not be used on traditional buffers; it is for use with iobufs.
 @param bp Buffer whose vnode to set.
 @param vp The vnode to attach to the buffer.
 @return void.
 */
void	buf_setvnode(buf_t, vnode_t);

/*!
 @function buf_device
 @abstract Get the device ID associated with a buffer.
 @discussion In traditional buffer use, this value is NODEV until buf_strategy() is called unless
 buf_getblk() was passed a device vnode.  It is set on an iobuf if buf_alloc() is passed a device 
 vnode or if buf_setdevice() is called.
 @param bp Buffer whose device ID to retrieve.
 @return Device id.
 */
dev_t	buf_device(buf_t);

/*!
 @function buf_setdevice
 @abstract Set the device associated with a buffer.
 @discussion A buffer's device is set in buf_strategy() (or in buf_getblk() if the file is a device).  
 It is also set on an iobuf if buf_alloc() is passed a device vnode.
 @param bp Buffer whose device ID to set.
 @param vp Device to set on the buffer.
 @return 0 for success, EINVAL if vp is not a device file.
 */
errno_t	buf_setdevice(buf_t, vnode_t);

/*!
 @function buf_strategy
 @abstract Pass an I/O request for a buffer down to the device layer.
 @discussion This is one of the most important routines in the buffer cache layer.  For buffers obtained
 through buf_getblk, it handles finding physical block numbers for the I/O (with VNOP_BLKTOOFF and 
 VNOP_BLOCKMAP), packaging the I/O into page-sized chunks, and initiating I/O on the disk by calling 
 the device's strategy routine. If a buffer's UPL has been set manually with buf_setupl(), it assumes 
 that the request is already correctly configured with a block number and a size divisible by page size
 and will just call directly to the device.
 @param devvp Device on which to perform I/O
 @param ap vnop_strategy_args structure (most importantly, a buffer).
 @return 0 for success, or errors from filesystem or device layers.
 */
errno_t	buf_strategy(vnode_t, void *);

/* 
 * Flags for buf_invalblkno() 
 */
#define	BUF_WAIT	0x01

/*!
 @function buf_invalblkno
 @abstract Invalidate a filesystem logical block in a file.
 @discussion buf_invalblkno() tries to make the data for a given block in a file
 invalid; if the buffer for that block is found in core and is not busy, we mark it 
 invalid and call buf_brelse() (see "flags" param for what happens if the buffer is busy).
 buf_brelse(), noticing that it is invalid, will 
 will return the buffer to the empty-buffer list and tell the VM subsystem to abandon 
 the relevant pages.  Data will not be written to backing store--it will be cast aside.
 Note that this function will only work if the block in question has been
 obtained with a buf_getblk().  If data has been read into core without using
 traditional buffer cache routines, buf_invalblkno() will not be able to invalidate it--this
 includes the use of iobufs.
 @param bp Buffer whose block to invalidate.
 @param lblkno Logical block number.
 @param flags BUF_WAIT: wait for busy buffers to become unbusy and invalidate them then.  Otherwise,
 just return EBUSY for busy blocks.
 @return 0 for success, EINVAL if vp is not a device file.
 */
errno_t	buf_invalblkno(vnode_t, daddr64_t, int);

/*!
 @function buf_callback
 @abstract Get the function set to be called when I/O on a buffer completes.
 @discussion A function returned by buf_callback was originally set with buf_setcallback().
 @param bp Buffer whose callback to get.
 @return 0 for success, or errors from filesystem or device layers.
 */
void * buf_callback(buf_t);

/*!
 @function buf_setcallback
 @abstract Set a function to be called once when I/O on a buffer completes.
 @discussion A one-shot callout set with buf_setcallback() will be called from buf_biodone()
 when I/O completes. It will be passed the "transaction" argument as well as the buffer.
 buf_setcallback() also marks the buffer as B_ASYNC.
 @param bp Buffer whose callback to set.
 @param callback function to use as callback.
 @param transaction Additional argument to callback function.
 @return 0; always succeeds.
 */
errno_t	buf_setcallback(buf_t, void (*)(buf_t, void *), void *);

/*!
 @function buf_setupl
 @abstract Set the UPL (Universal Page List), and offset therein, on a buffer.
 @discussion buf_setupl() should only be called on buffers allocated with buf_alloc().
 A subsequent call to buf_map() will map the UPL and give back the address at which data
 begins. After buf_setupl() is called, a buffer is marked B_CLUSTER; when this is the case,
 buf_strategy() assumes that a buffer is correctly configured to be passed to the device
 layer without modification. Passing a NULL upl will clear the upl and the B_CLUSTER flag on the
 buffer.
 @param bp Buffer whose upl to set.
 @param upl UPL to set in the buffer.
 @parma offset Offset within upl at which relevant data begin.
 @return 0 for success, EINVAL if the buffer was not allocated with buf_alloc().
 */
errno_t	buf_setupl(buf_t, upl_t, uint32_t);

/*!
 @function buf_clone
 @abstract Clone a buffer with a restricted range and an optional callback.
 @discussion Generates a buffer which is identical to its "bp" argument except that
 it spans a subset of the data of the original.  The buffer to be cloned should
 have been allocated with buf_alloc().  Checks its arguments to make sure
 that the data subset is coherent. Optionally, adds a callback function and argument to it
 to be called when I/O completes (as with buf_setcallback(), but B_ASYNC is not set).  If the original buffer had
 a upl set through buf_setupl(), this upl is copied to the new buffer; otherwise, the original's
 data pointer is used raw. The buffer must be released with buf_free().
 @param bp Buffer to clone.
 @param io_offset Offset, relative to start of data in original buffer, at which new buffer's data will begin. 
 @param io_size Size of buffer region in new buffer, in the sense of buf_count().
 @param iodone Callback to be called from buf_biodone() when I/O completes, in the sense of buf_setcallback().
 @param arg Argument to pass to iodone() callback.
 @return NULL if io_offset/io_size combination is invalid for the buffer to be cloned; otherwise, the new buffer.
 */
buf_t	buf_clone(buf_t, int, int, void (*)(buf_t, void *), void *);


<<<<<<< HEAD
/*!
 @function buf_create_shadow
 @abstract Create a shadow buffer with optional private storage and an optional callback.
 @param bp Buffer to shadow.
 @param force_copy If TRUE, do not link the shadaow to 'bp' and if 'external_storage' == NULL,
 force a copy of the data associated with 'bp'.
 @param external_storage If non-NULL, associate it with the new buffer as its storage instead of the 
 storage currently associated with 'bp'.
 @param iodone Callback to be called from buf_biodone() when I/O completes, in the sense of buf_setcallback().
 @param arg Argument to pass to iodone() callback.
 @return NULL if the buffer to be shadowed is not B_META or a primary buffer (i.e. not a shadow buffer); otherwise, the new buffer.
*/

buf_t	buf_create_shadow(buf_t bp, boolean_t force_copy, uintptr_t external_storage, void (*iodone)(buf_t, void *), void *arg);


/*!
 @function buf_shadow
 @abstract returns true if 'bp' is a shadow of another buffer.
 @param bp Buffer to query.
 @return 1 if 'bp' is a shadow, 0 otherwise.
*/
int	buf_shadow(buf_t bp);


/*!
 @function buf_alloc
 @abstract Allocate an uninitialized buffer.
 @discussion A buffer returned by buf_alloc() is marked as busy and as an iobuf; it has no storage set up and must be
 set up using buf_setdataptr() or buf_setupl()/buf_map().
 @param vp vnode to associate with the buffer: optionally NULL.  If vp is a device file, then
 the buffer's associated device will be set. If vp is NULL, it can be set later with buf_setvnode().
 @return New buffer.
 */
buf_t 	buf_alloc(vnode_t);

/*!
 @function buf_free
 @abstract Free a buffer that was allocated with buf_alloc().
 @discussion The storage (UPL, data pointer) associated with an iobuf must be freed manually.
 @param bp The buffer to free.
 @return void.
 */
void	buf_free(buf_t);
=======
/* cluster_io definitions for use with io bufs */
#define b_uploffset  b_bufsize
#define b_trans_head b_freelist.tqe_prev
#define b_trans_next b_freelist.tqe_next
#define b_real_bp    b_saveaddr
#define b_iostate    b_rcred

/* journaling uses this cluster i/o field for its own
 * purposes because meta data buf's should never go
 * through the clustering code.
 */
#define b_transaction b_vectorlist

   
>>>>>>> origin/10.2

/*
<<<<<<< HEAD
 * flags for buf_invalidateblks
 */
#define	BUF_WRITE_DATA	0x0001		/* write data blocks first */
#define	BUF_SKIP_META	0x0002		/* skip over metadata blocks */
#define BUF_INVALIDATE_LOCKED	0x0004	/* force B_LOCKED blocks to be invalidated */
=======
 * These flags are kept in b_flags.
 */
#define	B_AGE		0x00000001	/* Move to age queue when I/O done. */
#define	B_NEEDCOMMIT	0x00000002	/* Append-write in progress. */
#define	B_ASYNC		0x00000004	/* Start I/O, do not wait. */
#define	B_BAD		0x00000008	/* Bad block revectoring in progress. */
#define	B_BUSY		0x00000010	/* I/O in progress. */
#define	B_CACHE		0x00000020	/* Bread found us in the cache. */
#define	B_CALL		0x00000040	/* Call b_iodone from biodone. */
#define	B_DELWRI	0x00000080	/* Delay I/O until buffer reused. */
#define	B_DIRTY		0x00000100	/* Dirty page to be pushed out async. */
#define	B_DONE		0x00000200	/* I/O completed. */
#define	B_EINTR		0x00000400	/* I/O was interrupted */
#define	B_ERROR		0x00000800	/* I/O error occurred. */
#define	B_WASDIRTY	0x00001000	/* page was found dirty in the VM cache */
#define	B_INVAL		0x00002000	/* Does not contain valid info. */
#define	B_LOCKED	0x00004000	/* Locked in core (not reusable). */
#define	B_NOCACHE	0x00008000	/* Do not cache block after use. */
#define	B_PAGEOUT	0x00010000	/* Page out indicator... */
#define	B_PGIN		0x00020000	/* Pagein op, so swap() can count it. */
#define	B_PHYS		0x00040000	/* I/O to user memory. */
#define	B_RAW		0x00080000	/* Set by physio for raw transfers. */
#define	B_READ		0x00100000	/* Read buffer. */
#define	B_TAPE		0x00200000	/* Magnetic tape I/O. */
#define	B_PAGELIST	0x00400000	/* Buffer describes pagelist I/O. */
#define	B_WANTED	0x00800000	/* Process wants this buffer. */
#define	B_WRITE		0x00000000	/* Write buffer (pseudo flag). */
#define	B_WRITEINPROG	0x01000000	/* Write in progress. */
#define	B_HDRALLOC	0x02000000	/* zone allocated buffer header */
#define	B_NORELSE	0x04000000	/* don't brelse() in bwrite() */
#define B_NEED_IODONE   0x08000000
								/* need to do a biodone on the */
								/* real_bp associated with a cluster_io */
#define B_COMMIT_UPL    0x10000000
								/* commit pages in upl when */
								/* I/O completes/fails */
#define	B_ZALLOC	0x20000000	/* b_data is zalloc()ed */
#define	B_META		0x40000000	/* buffer contains meta-data. */
#define	B_VECTORLIST	0x80000000	/* Used by device drivers. */
>>>>>>> origin/10.1

/*!
 @function buf_invalidateblks
 @abstract Invalidate all the blocks associated with a vnode.
 @discussion This function does for all blocks associated with a vnode what buf_invalblkno does for one block.
 Again, it will only be able to invalidate data which were populated with traditional buffer cache routines,
 i.e. by buf_getblk() and callers thereof. Unlike buf_invalblkno(), it can be made to write dirty data to disk
 rather than casting it aside.
 @param bp The buffer whose data to invalidate.
 @param flags BUF_WRITE_DATA: write dirty data to disk with VNOP_BWRITE() before kicking buffer cache entries out.
 BUF_SKIP_META: do not invalidate metadata blocks.
 @param slpflag Flags to pass to "msleep" while waiting to acquire busy buffers. 
 @param slptimeo Timeout in "hz" (1/100 second) to wait for a buffer to become unbusy before waking from sleep
 and re-starting the scan.
 @return 0 for success, error values from msleep().
 */
int	buf_invalidateblks(vnode_t, int, int, int);

/*
 * flags for buf_flushdirtyblks and buf_iterate
 */
#define BUF_SKIP_NONLOCKED	0x01
#define BUF_SKIP_LOCKED		0x02
#define BUF_SCAN_CLEAN		0x04	/* scan the clean buffers */
#define BUF_SCAN_DIRTY		0x08	/* scan the dirty buffers */
#define BUF_NOTIFY_BUSY		0x10	/* notify the caller about the busy pages during the scan */


#define	BUF_RETURNED		0
#define BUF_RETURNED_DONE	1
#define BUF_CLAIMED		2
#define	BUF_CLAIMED_DONE	3
/*!
 @function buf_flushdirtyblks
 @abstract Write dirty file blocks to disk.
 @param vp The vnode whose blocks to flush.
 @param wait Wait for writes to complete before returning.
 @param flags Can pass zero, meaning "flush all dirty buffers."  
 BUF_SKIP_NONLOCKED: Skip buffers which are not busy when we encounter them.
 BUF_SKIP_LOCKED: Skip buffers which are busy when we encounter them.
 @param msg String to pass to msleep().
 @return void.
 */
void	buf_flushdirtyblks(vnode_t, int, int, const char *);

/*!
 @function buf_iterate
 @abstract Perform some operation on all buffers associated with a vnode.
 @param vp The vnode whose buffers to scan.
 @param callout Function to call on each buffer.  Should return one of:
 BUF_RETURNED: buf_iterate() should call buf_brelse() on the buffer.
 BUF_RETURNED_DONE: buf_iterate() should call buf_brelse() on the buffer and then stop iterating.
 BUF_CLAIMED: buf_iterate() should continue iterating (and not call buf_brelse()).
 BUF_CLAIMED_DONE: buf_iterate() should stop iterating (and not call buf_brelse()).
 @param flag
 BUF_SKIP_NONLOCKED: Skip buffers which are not busy when we encounter them. BUF_SKIP_LOCKED: Skip buffers which are busy when we encounter them.
 BUF_SCAN_CLEAN: Call out on clean buffers.
 BUF_SCAN_DIRTY: Call out on dirty buffers.
 BUF_NOTIFY_BUSY: If a buffer cannot be acquired, pass a NULL buffer to callout; otherwise,
 that buffer will be silently skipped.
 @param arg Argument to pass to callout in addition to buffer.
 @return void.
 */
void	buf_iterate(vnode_t, int (*)(buf_t, void *), int, void *);

/*!
 @function buf_clear
 @abstract Zero out the storage associated with a buffer. 
 @discussion Calls buf_map() to get the buffer's data address; for a B_CLUSTER
 buffer (one which has had buf_setupl() called on it), it tries to map the buffer's
 UPL into memory; should only be called once during the life cycle of an iobuf (one allocated
 with buf_alloc()).  
 @param bp The buffer to zero out.
 @return void.
 */
void	buf_clear(buf_t);

/*!
 @function buf_bawrite
 @abstract Start an asychronous write on a buffer.
 @discussion Calls VNOP_BWRITE to start the process of propagating an asynchronous write down to the device layer.
 Callers can wait for writes to complete at their discretion using buf_biowait().  When this function is called,
 data should already have been written to the buffer's data region.
 @param bp The buffer on which to initiate I/O.
 @param throttle If "throttle" is nonzero and more than VNODE_ASYNC_THROTTLE writes are in progress on this file,
 buf_bawrite() will block until the write count drops below VNODE_ASYNC_THROTTLE.  If "throttle" is zero and the write
 count is high, it will fail with EWOULDBLOCK; the caller can decide whether to make a blocking call or pursue
 other opportunities.
 @return EWOULDBLOCK if write count is high and "throttle" is zero; otherwise, errors from VNOP_BWRITE.
 */
errno_t	buf_bawrite(buf_t);

/*!
 @function buf_bdwrite
 @abstract Mark a buffer for delayed write.
 @discussion Marks a buffer as waiting for delayed write and the current I/O as complete; data will be written to backing store
 before the buffer is reused, but it will not be queued for I/O immediately.  Note that for buffers allocated
 with buf_alloc(), there are no such guarantees; you must take care of your own flushing to disk.  If
 the number of delayed writes pending on the system is greater than an internal limit and the caller has not
 requested otherwise [see return_error] , buf_bdwrite() will unilaterally launch an asynchronous I/O with buf_bawrite() to keep the pile of
 delayed writes from getting too large.
 @param bp The buffer to mark for delayed write.
 @param return_error If the number of pending delayed writes systemwide is larger than an internal limit,
 return EAGAIN rather than doing an asynchronous write.
 @return EAGAIN for return_error != 0 case, 0 for succeess, errors from buf_bawrite.
 */
errno_t	buf_bdwrite(buf_t);

/*!
 @function buf_bwrite
 @abstract Write a buffer's data to backing store.
 @discussion Once the data in a buffer has been modified, buf_bwrite() starts sending it to disk by calling
 VNOP_STRATEGY.  Unless B_ASYNC has been set on the buffer (by buf_setflags() or otherwise), data will have 
 been written to disk when buf_bwrite() returns.  See Bach (p 56).
 @param bp The buffer to write to disk.
 @return 0 for success; errors from buf_biowait().
 */
errno_t	buf_bwrite(buf_t);

/*!
 @function buf_biodone
 @abstract Mark an I/O as completed.
 @discussion buf_biodone() should be called by whosoever decides that an I/O on a buffer is complete; for example,
 IOStorageFamily.  It clears the dirty flag on a buffer and signals on the vnode that a write has completed
 with vnode_writedone(). If a callout or filter has been set on the buffer, that function is called.  In the case
 of a callout, that function is expected to take care of cleaning up and freeing the buffer.
 Otherwise, if the buffer is marked B_ASYNC (e.g. it was passed to buf_bawrite()), then buf_biodone()
 considers itself justified in calling buf_brelse() to return it to free lists--no one is waiting for it.  Finally, 
 waiters on the bp (e.g. in buf_biowait()) are woken up.
 @param bp The buffer to mark as done with I/O.
 @return void.
 */
void	buf_biodone(buf_t);

/*!
 @function buf_biowait
 @abstract Wait for I/O on a buffer to complete.
 @discussion Waits for I/O on a buffer to finish, as marked by a buf_biodone() call.  
 @param bp The buffer to wait on.
 @return 0 for a successful wait; nonzero the buffer has been marked as EINTR or had an error set on it.
 */
errno_t	buf_biowait(buf_t);

/*!
 @function buf_brelse
 @abstract Release any claim to a buffer, sending it back to free lists.
 @discussion buf_brelse() cleans up buffer state and releases a buffer to the free lists.  If the buffer
 is not marked invalid and its pages are dirty (e.g. a delayed write was made), its data will be commited
 to backing store. If it is marked invalid, its data will be discarded completely.  
 A valid, cacheable buffer will be put on a list and kept in the buffer hash so it
 can be found again; otherwise, it will be dissociated from its vnode and treated as empty.  Which list a valid
 buffer is placed on depends on the use of buf_markaged(), whether it is metadata, and the B_LOCKED flag.  A
 B_LOCKED buffer will not be available for reuse by other files, though its data may be paged out.
 Note that buf_brelse() is intended for use with traditionally allocated buffers.
 @param bp The buffer to release.
 @retrn void.
 */
void	buf_brelse(buf_t);

/*!
 @function buf_bread
 @abstract Synchronously read a block of a file.
 @discussion buf_bread() is the traditional way to read a single logical block of a file through the buffer cache. 
 It tries to find the buffer and corresponding page(s) in core, calls VNOP_STRATEGY if necessary to bring the data
 into memory, and waits for I/O to complete.  It should not be used to read blocks of greater than 4K (one VM page) 
 in size; use cluster routines for large reads.  Indeed, the cluster layer is a more efficient choice for reading DATA
 unless you need some finely-tuned semantics that it cannot provide.
 @param vp The file from which to read.
 @param blkno The logical (filesystem) block number to read.
 @param size Size of block; do not use for sizes > 4K.
 @param cred Credential to store and use for reading from disk if data are not already in core.
 @param bpp Destination pointer for buffer.
 @return 0 for success, or an error from buf_biowait().
 */
errno_t	buf_bread(vnode_t, daddr64_t, int, kauth_cred_t, buf_t *);

/*!
 @function buf_breadn
 @abstract Read a block from a file with read-ahead.
 @discussion buf_breadn() reads one block synchronously in the style of buf_bread() and fires
 off a specified set of asynchronous reads to improve the likelihood of future cache hits.  
 It should not be used to read blocks of greater than 4K (one VM page) in size; use cluster 
 routines for large reads.  Indeed, the cluster layer is a more efficient choice for reading DATA
 unless you need some finely-tuned semantics that it cannot provide.
 @param vp The file from which to read.
 @param blkno The logical (filesystem) block number to read synchronously.
 @param size Size of block; do not use for sizes > 4K.
 @param rablks Array of logical block numbers for asynchronous read-aheads.
 @param rasizes Array of block sizes for asynchronous read-aheads, each index corresponding to same index in "rablks."
 @param nrablks Number of entries in read-ahead arrays.
 @param cred Credential to store and use for reading from disk if data are not already in core.
 @param bpp Destination pointer for buffer.
 @return 0 for success, or an error from buf_biowait().
 */
errno_t	buf_breadn(vnode_t, daddr64_t, int, daddr64_t *, int *, int, kauth_cred_t, buf_t *);

/*!
 @function buf_meta_bread
 @abstract Synchronously read a metadata block of a file.
 @discussion buf_meta_bread() is the traditional way to read a single logical block of a file through the buffer cache. 
 It tries to find the buffer and corresponding page(s) in core, calls VNOP_STRATEGY if necessary to bring the data
 into memory, and waits for I/O to complete.  It should not be used to read blocks of greater than 4K (one VM page) 
 in size; use cluster routines for large reads.  Reading meta-data through the traditional buffer cache, unlike
 reading data, is efficient and encouraged, especially if the blocks being read are significantly smaller than page size.
 @param vp The file from which to read.
 @param blkno The logical (filesystem) block number to read.
 @param size Size of block; do not use for sizes > 4K.
 @param cred Credential to store and use for reading from disk if data are not already in core.
 @param bpp Destination pointer for buffer.
 @return 0 for success, or an error from buf_biowait().
 */
errno_t	buf_meta_bread(vnode_t, daddr64_t, int, kauth_cred_t, buf_t *);

/*!
 @function buf_meta_breadn
 @abstract Read a metadata block from a file with read-ahead.
 @discussion buf_meta_breadn() reads one block synchronously in the style of buf_meta_bread() and fires
 off a specified set of asynchronous reads to improve the likelihood of future cache hits.  
 It should not be used to read blocks of greater than 4K (one VM page) in size; use cluster 
 routines for large reads.
 @param vp The file from which to read.
 @param blkno The logical (filesystem) block number to read synchronously.
 @param size Size of block; do not use for sizes > 4K.
 @param rablks Array of logical block numbers for asynchronous read-aheads.
 @param rasizes Array of block sizes for asynchronous read-aheads, each index corresponding to same index in "rablks."
 @param nrablks Number of entries in read-ahead arrays.
 @param cred Credential to store and use for reading from disk if data are not already in core.
 @param bpp Destination pointer for buffer.
 @return 0 for success, or an error from buf_biowait().
 */
errno_t	buf_meta_breadn(vnode_t, daddr64_t, int, daddr64_t *, int *, int, kauth_cred_t, buf_t *);

/*!
 @function minphys
 @abstract Adjust a buffer's count to be no more than maximum physical I/O transfer size for the host architecture.
 @discussion physio() takes as a parameter a function to bound transfer sizes for each VNOP_STRATEGY() call.  minphys()
 is a default implementation.  It calls buf_setcount() to make the buffer's count the min() of its current count
 and the max I/O size for the host architecture.
 @param bp The buffer whose byte count to modify.
 @return New byte count.
 */
u_int	minphys(buf_t bp);

/*!
 @function physio 
 @abstract Perform I/O on a device to/from target memory described by a uio.
 @discussion physio() allows I/O directly from a device to user-space memory.  It waits
 for all I/O to complete before returning.
 @param f_strategy Strategy routine to call to initiate I/O.
 @param bp Buffer to configure and pass to strategy routine; can be NULL.
 @param dev Device on which to perform I/O.
 @param flags B_READ or B_WRITE.
 @param f_minphys Function which calls buf_setcount() to set a byte count which is suitably
 small for the device in question.  Returns byte count that has been set (or unchanged) on the buffer.
 @param uio UIO describing the I/O operation.
 @param blocksize Logical block size for this vnode.
 @return 0 for success; EFAULT for an invalid uio; errors from buf_biowait().
 */
int	physio(void (*)(buf_t), buf_t, dev_t, int ,  u_int (*)(buf_t), struct uio *, int );


/*
 * Flags for operation type in getblk()
 */
#define	BLK_READ	0x01	/* buffer for read */
#define	BLK_WRITE	0x02	/* buffer for write */
#define	BLK_META	0x10	/* buffer for metadata */
/*
 * modifier for above flags...  if set, getblk will only return
 * a bp that is already valid... i.e. found in the cache
 */
#define BLK_ONLYVALID	0x80000000
                                  
/*!
 @function buf_getblk
 @abstract Traditional buffer cache routine to get a buffer corresponding to a logical block in a file.
 @discussion buf_getblk() gets a buffer, not necessarily containing valid data, representing a block in a file.  
 A metadata buffer will be returned with its own zone-allocated storage, managed by the traditional buffer-cache
 layer, whereas data buffers will be returned hooked into backing by the UBC (which in fact controls the caching of data).
 buf_getblk() first looks for the buffer header in cache; if the buffer is in-core but busy, buf_getblk() will wait for it to become
 unbusy, depending on the slpflag and slptimeo parameters. If the buffer is found unbusy and is a metadata buffer,
 it must already contain valid data and will be returned directly; data buffers will have a UPL configured to
 prepare for interaction with the underlying UBC.  If the buffer is found in core, it will be marked as such
 and buf_fromcache() will return truth. A buffer is allocated and initialized (but not filled with data)
 if none is found in core. buf_bread(), buf_breadn(), buf_meta_bread(), and buf_meta_breadn() all 
 return buffers obtained with buf_getblk().
 @param vp File for which to get block.
 @param blkno Logical block number.
 @param size Size of block.
 @param slpflag Flag to pass to msleep() while waiting for buffer to become unbusy.
 @param slptimeo Time, in milliseconds, to wait for buffer to become unbusy.  0 means to wait indefinitely.
 @param operation BLK_READ: want a read buffer.  BLK_WRITE: want a write buffer.  BLK_META: want a metadata buffer.  BLK_ONLYVALID: 
 only return buffers which are found in core (do not allocate anew), and do not change buffer size.  The last remark means
 that if a given logical block is found in core with a different size than what is requested, the buffer size will not be modified.
 @return Buffer found in core or newly allocated, either containing valid data or ready for I/O.
 */
buf_t	buf_getblk(vnode_t, daddr64_t, int, int, int, int);

/*!
 @function buf_geteblk
 @abstract Get a metadata buffer which is marked invalid and not associated with any vnode.
 @discussion A buffer is returned with zone-allocated storage of the specified size, marked B_META and invalid. 
 It has no vnode and is not visible in the buffer hash.  
 @param size Size of buffer.
 @return Always returns a new buffer.
 */
buf_t	buf_geteblk(int);

/*!
 @function buf_clear_redundancy_flags
 @abstract Clear flags on a buffer.
 @discussion buffer_redundancy_flags &= ~flags
 @param bp Buffer whose flags to clear.
 @param flags Flags to remove from buffer's mask
 @return void.
 */
void	buf_clear_redundancy_flags(buf_t, uint32_t);

/*!
 @function buf_redundancyflags
 @abstract Get redundancy flags set on a buffer.
 @param bp Buffer whose redundancy flags to grab.
 @return flags.
 */
<<<<<<< HEAD
uint32_t	buf_redundancy_flags(buf_t);

/*!
 @function buf_setredundancyflags
 @abstract Set redundancy flags on a buffer.
 @discussion buffer_redundancy_flags |= flags
 @param bp Buffer whose flags to set.
 @param flags Flags to add to buffer's redundancy flags
 @return void.
 */
void	buf_set_redundancy_flags(buf_t, uint32_t);

/*!
 @function buf_attr
 @abstract Gets the attributes for this buf.
 @param bp Buffer whose attributes to get.
 @return bufattr_t.
 */
bufattr_t buf_attr(buf_t);

/*!
 @function buf_markstatic
 @abstract Mark a buffer as being likely to contain static data.
 @param bp Buffer to mark.
 @return void.
 */
 void buf_markstatic(buf_t);

/*!
 @function buf_static
 @abstract Check if a buffer contains static data.
 @param bp Buffer to test.
 @return Nonzero if buffer has static data, 0 otherwise.
 */
int	buf_static(buf_t);

#ifdef KERNEL_PRIVATE
<<<<<<< HEAD
void	buf_setfilter(buf_t, void (*)(buf_t, void *), void *, void (**)(buf_t, void *), void **);

/* bufattr allocation/duplication/deallocation functions */
bufattr_t bufattr_alloc(void);
bufattr_t bufattr_dup (bufattr_t bap);
void bufattr_free(bufattr_t bap);

/*!
 @function bufattr_cpx
 @abstract Returns a pointer to a cpx_t structure.
 @param bap Buffer Attribute whose cpx_t structure you wish to get.
 @return Returns a cpx_t structure, or NULL if not valid
 */
struct cpx *bufattr_cpx(bufattr_t);

/*!
 @function bufattr_setcpx
 @abstract Set the cp_ctx on a buffer attribute.
 @param bap Buffer Attribute that you wish to change
 @return void
 */
void bufattr_setcpx(bufattr_t, struct cpx *cpx);
=======
void	buf_setfilter(buf_t, void (*)(buf_t, void *), void *, void **, void **);

/*!
 @function buf_getcpaddr
 @abstract Set the address of cp_entry on a buffer.
 @param bp Buffer whose cp entry value has to be set
 @return void.
 */
void buf_setcpaddr(buf_t, void *);

/*!
 @function buf_getcpaddr
 @abstract Get the address of cp_entry on a buffer.
 @param bp Buffer whose error value to set.
 @return int.
 */
void *buf_getcpaddr(buf_t);
#endif /* KERNEL_PRIVATE */
>>>>>>> origin/10.6

/*!
 @function bufattr_cpoff
 @abstract Gets the file offset on the buffer.
 @param bap Buffer Attribute whose file offset value is used
 @return void.
 */
uint64_t bufattr_cpoff(bufattr_t);

/*!
 @function bufattr_setcpoff
 @abstract Set the file offset for a content protected I/O on 
 a buffer attribute.
 @param bap Buffer Attribute whose cp file offset has to be set
 @return void.
 */
void bufattr_setcpoff(bufattr_t, uint64_t);

/*!
 @function bufattr_rawencrypted
 @abstract Check if a buffer contains raw encrypted data.
 @param bap Buffer attribute to test.
 @return Nonzero if buffer has raw encrypted data, 0 otherwise.
 */
int bufattr_rawencrypted(bufattr_t bap);

/*!
 @function bufattr_markgreedymode
 @abstract Mark a buffer to use the greedy mode for writing.
 @param bap Buffer attributes to mark.
 @discussion Greedy Mode: request improved write performance from the underlying device at the expense of storage efficiency
 @return void.
 */
 void bufattr_markgreedymode(bufattr_t bap);

/*!
 @function bufattr_greedymode
 @abstract Check if a buffer is written using the Greedy Mode
 @param bap Buffer attributes to test.
 @discussion Greedy Mode: request improved write performance from the underlying device at the expense of storage efficiency
 @return Nonzero if buffer uses greedy mode, 0 otherwise.
 */
int	bufattr_greedymode(bufattr_t bap);

/*!
 @function bufattr_markisochronous
 @abstract Mark a buffer to use the isochronous throughput mode for writing.
 @param bap Buffer attributes to mark.
 @discussion isochronous mode: request improved write performance from the underlying device at the expense of storage efficiency
 @return void.
 */
 void bufattr_markisochronous(bufattr_t bap);

 /*!
 @function bufattr_isochronous
 @abstract Check if a buffer is written using the isochronous
 @param bap Buffer attributes to test.
 @discussion isochronous mode: request improved write performance from the underlying device at the expense of storage efficiency
 @return Nonzero if buffer uses isochronous mode, 0 otherwise.
 */
int	bufattr_isochronous(bufattr_t bap);


/*!
 @function bufattr_throttled
 @abstract Check if a buffer is throttled.
 @param bap Buffer attribute to test.
 @return Nonzero if the buffer is throttled, 0 otherwise.
 */
int bufattr_throttled(bufattr_t bap);

/*!
 @function bufattr_passive
 @abstract Check if a buffer is marked passive.
 @param bap Buffer attribute to test.
 @return Nonzero if the buffer is marked passive, 0 otherwise.
 */
int bufattr_passive(bufattr_t bap);

/*!
 @function bufattr_nocache
 @abstract Check if a buffer has nocache attribute.
 @param bap Buffer attribute to test.
 @return Nonzero if the buffer is not cached, 0 otherwise.
 */
int bufattr_nocache(bufattr_t bap);

/*!
 @function bufattr_meta
 @abstract Check if a buffer has the bufattr meta attribute.
 @param bap Buffer attribute to test.
 @return Nonzero if the buffer has meta attribute, 0 otherwise.
 */

int bufattr_meta(bufattr_t bap);

/*!
 @function bufattr_markmeta
 @abstract Set the bufattr meta attribute.
 @param bap Buffer attribute to manipulate.
 @return void
 */
void bufattr_markmeta(bufattr_t bap);


/*!
 @function bufattr_delayidlesleep
 @abstract Check if a buffer is marked to delay idle sleep on disk IO.
 @param bap Buffer attribute to test.
 @return Nonzero if the buffer is marked to delay idle sleep on disk IO, 0 otherwise.
 */
int bufattr_delayidlesleep(bufattr_t bap);

/*!
 @function buf_kernel_addrperm_addr
 @abstract Obfuscate the buf pointers.
 @param addr Buf_t pointer.
 @return Obfuscated pointer if addr is non zero, 0 otherwise.
 */
vm_offset_t buf_kernel_addrperm_addr(void * addr);

/*!
 @function bufattr_markquickcomplete
 @abstract Mark a buffer to hint quick completion to the driver.
 @discussion This flag hints the storage driver that some thread is waiting for this I/O to complete.
 It should therefore attempt to complete it as soon as possible at the cost of device efficiency.
 @param bap Buffer attributes to mark.
 @return void.
 */
void bufattr_markquickcomplete(bufattr_t bap);

/*!
 @function bufattr_quickcomplete
 @abstract Check if a buffer is marked for quick completion
 @discussion This flag hints the storage driver that some thread is waiting for this I/O to complete.
 It should therefore attempt to complete it as soon as possible at the cost of device efficiency.
 @param bap Buffer attribute to test.
 @return Nonzero if the buffer is marked for quick completion, 0 otherwise.
 */
int bufattr_quickcomplete(bufattr_t bap);

#endif /* KERNEL_PRIVATE */
=======
#define	BQUEUES		6		/* number of free buffer queues */

#define	BQ_LOCKED	0		/* super-blocks &c */
#define	BQ_LRU		1		/* lru, useful buffers */
#define	BQ_AGE		2		/* rubbish */
#define	BQ_EMPTY	3		/* buffer headers with no memory */
#define BQ_META		4		/* buffer containing metadata */
#define BQ_LAUNDRY	5		/* buffers that need cleaning */
>>>>>>> origin/10.1

__END_DECLS


/* Macros to clear/set/test flags. */
#define	SET(t, f)	(t) |= (f)
#define	CLR(t, f)	(t) &= ~(f)
#define	ISSET(t, f)	((t) & (f))


#endif /* !_SYS_BUF_H_ */
