/*
<<<<<<< HEAD
 * Copyright (c) 2000-2007 Apple Inc. All rights reserved.
 *
 * @APPLE_OSREFERENCE_LICENSE_HEADER_START@
=======
 * Copyright (c) 2000 Apple Computer, Inc. All rights reserved.
 *
 * @APPLE_LICENSE_HEADER_START@
 * 
 * Copyright (c) 1999-2003 Apple Computer, Inc.  All Rights Reserved.
>>>>>>> origin/10.2
 * 
 * This file contains Original Code and/or Modifications of Original Code
 * as defined in and that are subject to the Apple Public Source License
 * Version 2.0 (the 'License'). You may not use this file except in
<<<<<<< HEAD
 * compliance with the License. The rights granted to you under the License
 * may not be used to create, or enable the creation or redistribution of,
 * unlawful or unlicensed copies of an Apple operating system, or to
 * circumvent, violate, or enable the circumvention or violation of, any
 * terms of an Apple operating system software license agreement.
 * 
 * Please obtain a copy of the License at
 * http://www.opensource.apple.com/apsl/ and read it before using this file.
=======
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
<<<<<<< HEAD
 * @APPLE_OSREFERENCE_LICENSE_HEADER_END@
=======
 * @APPLE_LICENSE_HEADER_END@
>>>>>>> origin/10.2
 */

#ifndef _MACHINE_COMMPAGE_H
#define _MACHINE_COMMPAGE_H

<<<<<<< HEAD
#if defined (__i386__) || defined (__x86_64__)
=======
#if defined (__ppc__)
#include "ppc/commpage/commpage.h"
#elif defined (__i386__)
>>>>>>> origin/10.2
#include "i386/commpage/commpage.h"
#else
#error architecture not supported
#endif

#ifndef	__ASSEMBLER__

extern	void	commpage_populate( void ); 	/* called once during startup */
<<<<<<< HEAD
extern  void	commpage_text_populate( void );
=======

>>>>>>> origin/10.2
#endif	/* __ASSEMBLER__ */

#endif	/* _MACHINE_COMMPAGE_H */
