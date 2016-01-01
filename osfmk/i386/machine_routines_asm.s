/*
 * Copyright (c) 2000 Apple Computer, Inc. All rights reserved.
 *
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
 */
#include <i386/asm.h>
<<<<<<< HEAD
=======
#include <i386/rtclock.h>
#include <i386/proc_reg.h>
#include <i386/eflags.h>
       
#include <i386/postcode.h>
#include <i386/apic.h>
#include <assym.s>
>>>>>>> origin/10.5

/*
**      ml_get_timebase()
**
**      Entry   - %esp contains pointer to 64 bit structure.
**
**      Exit    - 64 bit structure filled in.
**
*/
ENTRY(ml_get_timebase)

<<<<<<< HEAD
        movl    S_ARG0, %ecx
=======
			movl    S_ARG0, %ecx
			
			lfence
			rdtsc
			lfence
			
			movl    %edx, 0(%ecx)
			movl    %eax, 4(%ecx)
			
			ret
>>>>>>> origin/10.5

        rdtsc

        movl    %edx, 0(%ecx)
        movl    %eax, 4(%ecx)

        ret


/* PCI config cycle probing
 *
 *      boolean_t ml_probe_read(vm_offset_t paddr, unsigned int *val)
 *
 *      Read the memory location at physical address paddr.
 *  This is a part of a device probe, so there is a good chance we will
 *  have a machine check here. So we have to be able to handle that.
 *  We assume that machine checks are enabled both in MSR and HIDs
 */
ENTRY(ml_probe_read)

        movl S_ARG0, %ecx
        movl S_ARG1, %eax
        movl 0(%ecx), %ecx
        movl %ecx, 0(%eax)
        movl $1, %eax

        ret


/* PCI config cycle probing - 64-bit
 *
 *      boolean_t ml_probe_read_64(addr64_t paddr, unsigned int *val)
 *
 *      Read the memory location at physical address paddr.
 *  This is a part of a device probe, so there is a good chance we will
 *  have a machine check here. So we have to be able to handle that.
 *  We assume that machine checks are enabled both in MSR and HIDs
 */
ENTRY(ml_probe_read_64)

        /* Only use lower 32 bits of address for now */
        movl S_ARG0, %ecx
        movl S_ARG2, %eax
        movl 0(%ecx), %ecx
        movl %ecx, 0(%eax)
        movl $1, %eax

        ret


/* Read physical address byte
 *
 *      unsigned int ml_phys_read_byte(vm_offset_t paddr)
 *      unsigned int ml_phys_read_byte_64(addr64_t paddr)
 *
 *      Read the byte at physical address paddr. Memory should not be cache inhibited.
 */
ENTRY(ml_phys_read_byte_64)

        /* Only use lower 32 bits of address for now */
        movl S_ARG0, %ecx
        xor %eax, %eax
        movb 0(%ecx), %eax

	ret

ENTRY(ml_phys_read_byte)

        movl S_ARG0, %ecx
        xor %eax, %eax
        movb 0(%ecx), %eax

	ret


/* Read physical address half word
 *
 *      unsigned int ml_phys_read_half(vm_offset_t paddr)
 *      unsigned int ml_phys_read_half_64(addr64_t paddr)
 *
 *      Read the half word at physical address paddr. Memory should not be cache inhibited.
 */
ENTRY(ml_phys_read_half_64)

        /* Only use lower 32 bits of address for now */
        movl S_ARG0, %ecx
        xor %eax, %eax
        movw 0(%ecx), %eax

	ret

ENTRY(ml_phys_read_half)

        movl S_ARG0, %ecx
        xor %eax, %eax
        movw 0(%ecx), %eax

	ret


/* Read physical address word
 *
 *      unsigned int ml_phys_read(vm_offset_t paddr)
 *      unsigned int ml_phys_read_64(addr64_t paddr)
 *      unsigned int ml_phys_read_word(vm_offset_t paddr)
 *      unsigned int ml_phys_read_word_64(addr64_t paddr)
 *
 *      Read the word at physical address paddr. Memory should not be cache inhibited.
 */
ENTRY(ml_phys_read_64)
ENTRY(ml_phys_read_word_64)

        /* Only use lower 32 bits of address for now */
        movl S_ARG0, %ecx
        movl 0(%ecx), %eax

	ret

ENTRY(ml_phys_read)
ENTRY(ml_phys_read_word)

        movl S_ARG0, %ecx
        movl 0(%ecx), %eax

	ret


/* Read physical address double
 *
<<<<<<< HEAD
 *      unsigned long long ml_phys_read_double(vm_offset_t paddr)
 *      unsigned long long ml_phys_read_double_64(addr64_t paddr)
=======
 * This is the same as the commpage nanotime routine, except that it uses the
 * kernel internal "rtc_nanotime_info" data instead of the commpage data.  The two copies
 * of data (one in the kernel and one in user space) are kept in sync by rtc_clock_napped().
 *
 * Warning!  There is another copy of this code in osfmk/i386/locore.s.  The
 * two versions must be kept in sync with each other!
>>>>>>> origin/10.5
 *
 *      Read the double word at physical address paddr. Memory should not be cache inhibited.
 */
ENTRY(ml_phys_read_double_64)

        /* Only use lower 32 bits of address for now */
        movl S_ARG0, %ecx
        movl 0(%ecx), %eax
        movl 4(%ecx), %edx

	ret

ENTRY(ml_phys_read_double)

        movl S_ARG0, %ecx
        movl 0(%ecx), %eax
        movl 4(%ecx), %edx

	ret


/* Write physical address byte
 *
 *      void ml_phys_write_byte(vm_offset_t paddr, unsigned int data)
 *      void ml_phys_write_byte_64(addr64_t paddr, unsigned int data)
 *
 *      Write the byte at physical address paddr. Memory should not be cache inhibited.
 */
ENTRY(ml_phys_write_byte_64)

        /* Only use lower 32 bits of address for now */
        movl S_ARG0, %ecx
        movl S_ARG2, %eax
        movb %eax, 0(%ecx)

	ret

ENTRY(ml_phys_write_byte)

        movl S_ARG0, %ecx
        movl S_ARG1, %eax
        movb %eax, 0(%ecx)

	ret


/* Write physical address half word
 *
 *      void ml_phys_write_half(vm_offset_t paddr, unsigned int data)
 *      void ml_phys_write_half_64(addr64_t paddr, unsigned int data)
 *
 *      Write the byte at physical address paddr. Memory should not be cache inhibited.
 */
ENTRY(ml_phys_write_half_64)

        /* Only use lower 32 bits of address for now */
        movl S_ARG0, %ecx
        movl S_ARG2, %eax
        movw %eax, 0(%ecx)

	ret

ENTRY(ml_phys_write_half)

        movl S_ARG0, %ecx
        movl S_ARG1, %eax
        movw %eax, 0(%ecx)

	ret


/* Write physical address word
 *
 *      void ml_phys_write(vm_offset_t paddr, unsigned int data)
 *      void ml_phys_write_64(addr64_t paddr, unsigned int data)
 *      void ml_phys_write_word(vm_offset_t paddr, unsigned int data)
 *      void ml_phys_write_word_64(addr64_t paddr, unsigned int data)
 *
 *      Write the word at physical address paddr. Memory should not be cache inhibited.
 */
ENTRY(ml_phys_write_64)
ENTRY(ml_phys_write_word_64)

        /* Only use lower 32 bits of address for now */
        movl S_ARG0, %ecx
        movl S_ARG2, %eax
        movl %eax, 0(%ecx)

	ret

ENTRY(ml_phys_write)
ENTRY(ml_phys_write_word)

        movl S_ARG0, %ecx
        movl S_ARG1, %eax
        movl %eax, 0(%ecx)

	ret


/* Write physical address double word
 *
 *      void ml_phys_write_double(vm_offset_t paddr, unsigned long long data)
 *      void ml_phys_write_double_64(addr64_t paddr, unsigned long long data)
 *
 *      Write the double word at physical address paddr. Memory should not be cache inhibited.
 */
<<<<<<< HEAD
ENTRY(ml_phys_write_double_64)

        /* Only use lower 32 bits of address for now */
        movl S_ARG0, %ecx
        movl S_ARG2, %eax
        movl %eax, 0(%ecx)
        movl S_ARG3, %eax
        movl %eax, 4(%ecx)

	ret

ENTRY(ml_phys_write_double)

        movl S_ARG0, %ecx
        movl S_ARG1, %eax
        movl %eax, 0(%ecx)
        movl S_ARG2, %eax
        movl %eax, 4(%ecx)
=======
 
		.globl	EXT(_rtc_nanotime_read)
		.align	FALIGN
LEXT(_rtc_nanotime_read)
		pushl		%ebp
		movl		%esp,%ebp
		pushl		%esi
		pushl		%edi
		pushl		%ebx
		movl		8(%ebp),%edi				/* get ptr to rtc_nanotime_info */
		movl		12(%ebp),%eax				/* get "slow" flag */
		testl		%eax,%eax
		jnz		Lslow
		
		/* Processor whose TSC frequency is faster than SLOW_TSC_THRESHOLD */
		RTC_NANOTIME_READ_FAST()

		popl		%ebx
		popl		%edi
		popl		%esi
		popl		%ebp
		ret

		/* Processor whose TSC frequency is slower than or equal to SLOW_TSC_THRESHOLD */
Lslow:
		movl		RNT_GENERATION(%edi),%esi		/* get generation (0 if being changed) */
		testl		%esi,%esi				/* if being changed, loop until stable */
		jz		Lslow
		pushl		%esi					/* save generation */
		pushl		RNT_SHIFT(%edi)				/* save low 32 bits of tscFreq */

		lfence
		rdtsc	  						/* get TSC in %edx:%eax */
		lfence
		subl		RNT_TSC_BASE(%edi),%eax
		sbbl		RNT_TSC_BASE+4(%edi),%edx

		/*
		* Do the math to convert tsc ticks to nanoseconds.  We first
		* do long multiply of 1 billion times the tsc.  Then we do
		* long division by the tsc frequency
		*/
		mov		$1000000000, %ecx			/* number of nanoseconds in a second */
		mov		%edx, %ebx
		mul		%ecx
		mov		%edx, %edi
		mov		%eax, %esi
		mov		%ebx, %eax
		mul		%ecx
		add		%edi, %eax
		adc		$0, %edx				/* result in edx:eax:esi */
		mov		%eax, %edi
		popl		%ecx					/* get low 32 tscFreq */
		xor		%eax, %eax
		xchg		%edx, %eax
		div		%ecx
		xor		%eax, %eax
		mov		%edi, %eax
		div		%ecx
		mov		%eax, %ebx
		mov		%esi, %eax
		div		%ecx
		mov		%ebx, %edx				/* result in edx:eax */
		
		movl		8(%ebp),%edi				/* recover ptr to rtc_nanotime_info */
		popl		%esi					/* recover generation */

		addl		RNT_NS_BASE(%edi),%eax
		adcl		RNT_NS_BASE+4(%edi),%edx

		cmpl		RNT_GENERATION(%edi),%esi		/* have the parameters changed? */
		jne		Lslow					/* yes, loop until stable */

		pop		%ebx
		pop		%edi
		pop		%esi
		pop		%ebp
		ret							/* result in edx:eax */
>>>>>>> origin/10.5

	ret
