/*
 * Copyright (c) 2010 Apple, Inc. All rights reserved.
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
/*
 * Copyright (c) 1988 Stephen Deering.
 * Copyright (c) 1992, 1993
 *	The Regents of the University of California.  All rights reserved.
 *
 * This code is derived from software contributed to Berkeley by
 * Stephen Deering of Stanford University.
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
 *	@(#)igmp.h	8.1 (Berkeley) 6/10/93
 * $FreeBSD: src/sys/netinet/igmp.h,v 1.10 1999/08/28 00:49:15 peter Exp $
 */

#ifndef _NETINET_IGMP_H_
#define _NETINET_IGMP_H_
#include <sys/appleapiopts.h>

/*
 * Internet Group Management Protocol (IGMP) definitions.
 *
 * Written by Steve Deering, Stanford, May 1988.
 *
 * MULTICAST Revision: 3.5.1.2
 */

/* Minimum length of any IGMP protocol message. */
#define IGMP_MINLEN			8

/*
 * IGMPv1/v2 query and host report format.
 */
struct igmp {
	u_char		igmp_type;	/* version & type of IGMP message  */
	u_char		igmp_code;	/* subtype for routing msgs        */
	u_short		igmp_cksum;	/* IP-style checksum               */
	struct in_addr	igmp_group;	/* group address being reported    */
};					/*  (zero for queries)             */

/*
 * IGMP v3 query format.
 */
struct igmpv3 {
	u_char		igmp_type;	/* version & type of IGMP message  */
	u_char		igmp_code;	/* subtype for routing msgs        */
	u_short		igmp_cksum;	/* IP-style checksum               */
	struct in_addr	igmp_group;	/* group address being reported    */
					/*  (zero for queries)             */
	u_char		igmp_misc;	/* reserved/suppress/robustness    */
	u_char		igmp_qqi;	/* querier's query interval        */
	u_short		igmp_numsrc;	/* number of sources               */
	/*struct in_addr	igmp_sources[1];*/ /* source addresses */
};
#define IGMP_V3_QUERY_MINLEN		12
#define IGMP_EXP(x)			(((x) >> 4) & 0x07)
#define IGMP_MANT(x)			((x) & 0x0f)
#define IGMP_QRESV(x)			(((x) >> 4) & 0x0f)
#define IGMP_SFLAG(x)			(((x) >> 3) & 0x01)
#define IGMP_QRV(x)			((x) & 0x07)

struct igmp_grouprec {
	u_char		ig_type;	/* record type */
	u_char		ig_datalen;	/* length of auxiliary data */
	u_short		ig_numsrc;	/* number of sources */
	struct in_addr	ig_group;	/* group address being reported */
	/*struct in_addr	ig_sources[1];*/ /* source addresses */
};
#define IGMP_GRPREC_HDRLEN		8

/*
 * IGMPv3 host membership report header.
 */
struct igmp_report {
	u_char		ir_type;	/* IGMP_v3_HOST_MEMBERSHIP_REPORT */
	u_char		ir_rsv1;	/* must be zero */
	u_short		ir_cksum;	/* checksum */
	u_short		ir_rsv2;	/* must be zero */
	u_short		ir_numgrps;	/* number of group records */
	/*struct	igmp_grouprec ir_groups[1];*/	/* group records */
};
#define IGMP_V3_REPORT_MINLEN		8
#define IGMP_V3_REPORT_MAXRECS		65535

/*
 * Message types, including version number.
 */
#define IGMP_HOST_MEMBERSHIP_QUERY	0x11	/* membership query         */
#define IGMP_v1_HOST_MEMBERSHIP_REPORT	0x12	/* Ver. 1 membership report */
#define IGMP_DVMRP			0x13	/* DVMRP routing message    */
#define IGMP_PIM			0x14	/* PIMv1 message (historic) */
#define IGMP_v2_HOST_MEMBERSHIP_REPORT	0x16	/* Ver. 2 membership report */
#define IGMP_HOST_LEAVE_MESSAGE		0x17	/* Leave-group message     */
#define IGMP_MTRACE_REPLY		0x1e	/* mtrace(8) reply */
#define IGMP_MTRACE_QUERY		0x1f	/* mtrace(8) probe */
#define IGMP_v3_HOST_MEMBERSHIP_REPORT	0x22	/* Ver. 3 membership report */

/*
 * IGMPv3 report modes.
 */
#define IGMP_DO_NOTHING			0	/* don't send a record */
#define IGMP_MODE_IS_INCLUDE		1	/* MODE_IN */
#define IGMP_MODE_IS_EXCLUDE		2	/* MODE_EX */
#define IGMP_CHANGE_TO_INCLUDE_MODE	3	/* TO_IN */
#define IGMP_CHANGE_TO_EXCLUDE_MODE	4	/* TO_EX */
#define IGMP_ALLOW_NEW_SOURCES		5	/* ALLOW_NEW */
#define IGMP_BLOCK_OLD_SOURCES		6	/* BLOCK_OLD */

/*
 * IGMPv3 query types.
 */
#define IGMP_V3_GENERAL_QUERY		1
#define IGMP_V3_GROUP_QUERY		2
#define IGMP_V3_GROUP_SOURCE_QUERY	3

/*
 * Maximum report interval for IGMP v1/v2 host membership reports [RFC 1112]
 */
#define IGMP_V1V2_MAX_RI		10
#define IGMP_MAX_HOST_REPORT_DELAY	IGMP_V1V2_MAX_RI

/*
 * IGMP_TIMER_SCALE denotes that the igmp code field specifies
 * time in tenths of a second.
 */
#define IGMP_TIMER_SCALE		10

#endif /* _NETINET_IGMP_H_ */
