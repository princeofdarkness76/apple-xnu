/*
 * Copyright (c) 2000 Apple Computer, Inc. All rights reserved.
 *
<<<<<<< HEAD
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
 * EXPRESS OR IMPLIED, AND APPLE HEREBY DISCLAIMS ALL SUCH WARRANTIES,
 * INCLUDING WITHOUT LIMITATION, ANY WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE, QUIET ENJOYMENT OR NON-INFRINGEMENT.
 * Please see the License for the specific language governing rights and
 * limitations under the License.
 * 
 * @APPLE_OSREFERENCE_LICENSE_HEADER_END@
=======
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
>>>>>>> origin/10.1
 */
/* Copyright (c) 1997, 1998 Apple Computer, Inc. All Rights Reserved */
/*
 *	@(#)ndrv.h	1.1 (MacOSX) 6/10/43
 * Justin Walker - 970604
 */

#ifndef _NET_NDRV_VAR_H
#define _NET_NDRV_VAR_H
<<<<<<< HEAD
#ifdef PRIVATE

/*
 * structure for storing a linked list of multicast addresses
 * registered by this socket. May be variable in length.
 */

struct ndrv_multiaddr
{
    struct ndrv_multiaddr*	next;
    ifmultiaddr_t			ifma;
    struct sockaddr			addr;
};
=======
>>>>>>> origin/10.1

/*
 * The cb is plugged into the socket (so_pcb), and the ifnet structure
 *  of BIND is plugged in here.
 * For now, it looks like a raw_cb up front...
 */
struct ndrv_cb
{
<<<<<<< HEAD
	TAILQ_ENTRY(ndrv_cb)	nd_next;
	struct socket *nd_socket;	/* Back to the socket */
	u_int32_t nd_signature;	/* Just double-checking */
=======
    struct ndrv_cb *nd_next;	/* Doubly-linked list */
	struct ndrv_cb *nd_prev;
	struct socket *nd_socket;	/* Back to the socket */
	unsigned int nd_signature;	/* Just double-checking */
>>>>>>> origin/10.1
	struct sockaddr_ndrv *nd_faddr;
	struct sockaddr_ndrv *nd_laddr;
	struct sockproto nd_proto;	/* proto family, protocol */
	int nd_descrcnt;		/* # elements in nd_dlist - Obsolete */
	TAILQ_HEAD(dlist, dlil_demux_desc) nd_dlist; /* Descr. list */
<<<<<<< HEAD
	u_int32_t nd_dlist_cnt; /* Descr. list count */
	struct ifnet *nd_if; /* obsolete, maintained for binary compatibility */
	u_int32_t nd_proto_family;
	u_int32_t nd_family;
	struct ndrv_multiaddr* nd_multiaddrs;
	short nd_unit;
=======
	struct ifnet *nd_if;
    u_long	nd_send_tag;
    u_long	nd_tag;
    u_long	nd_family;
    short	nd_unit;
>>>>>>> origin/10.1
};

#define	sotondrvcb(so)		((struct ndrv_cb *)(so)->so_pcb)
#define NDRV_SIGNATURE	0x4e445256 /* "NDRV" */

/* Nominal allocated space for NDRV sockets */
#define NDRVSNDQ	 8192
#define NDRVRCVQ	 8192

<<<<<<< HEAD
#endif /* PRIVATE */
=======
#if KERNEL
extern struct ndrv_cb ndrvl;		/* Head of controlblock list */
#endif
>>>>>>> origin/10.1
#endif	/* _NET_NDRV_VAR_H */
