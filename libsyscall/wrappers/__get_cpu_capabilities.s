/*
 * Copyright (c) 2003 Apple Computer, Inc. All rights reserved.
 *
 * @APPLE_LICENSE_HEADER_START@
 * 
<<<<<<< HEAD:libsyscall/wrappers/__get_cpu_capabilities.s
 * This file contains Original Code and/or Modifications of Original Code
 * as defined in and that are subject to the Apple Public Source License
 * Version 2.0 (the 'License'). You may not use this file except in
 * compliance with the License. Please obtain a copy of the License at
 * http://www.opensource.apple.com/apsl/ and read it before using this
 * file.
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
>>>>>>> origin/10.3:osfmk/i386/commpage/pthreads.s
 * EXPRESS OR IMPLIED, AND APPLE HEREBY DISCLAIMS ALL SUCH WARRANTIES,
 * INCLUDING WITHOUT LIMITATION, ANY WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE OR NON-INFRINGEMENT.  Please see the
 * License for the specific language governing rights and limitations
 * under the License.
 * 
 * @APPLE_LICENSE_HEADER_END@
 */

/* Get the cpu_capabilities bit vector out of the comm page */

#define	__APPLE_API_PRIVATE
#include <machine/cpu_capabilities.h>
#undef	__APPLE_API_PRIVATE

#if defined(__x86_64__)

	.text
	.align 2, 0x90
	.globl __get_cpu_capabilities
__get_cpu_capabilities:
	movq	$(_COMM_PAGE_CPU_CAPABILITIES64), %rax
	movq	(%rax), %rax
	ret

#elif defined(__i386__)

	.text
	.align 2, 0x90
	.globl __get_cpu_capabilities
__get_cpu_capabilities:
	movl	_COMM_PAGE_CPU_CAPABILITIES64, %eax
	movl	_COMM_PAGE_CPU_CAPABILITIES64+4, %edx
	ret

#else
#error Unsupported architecture
#endif
