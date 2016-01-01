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
/*
 * @OSF_COPYRIGHT@
 */
/* 
 * Mach Operating System
 * Copyright (c) 1991,1990,1989 Carnegie Mellon University
 * All Rights Reserved.
 * 
 * Permission to use, copy, modify and distribute this software and its
 * documentation is hereby granted, provided that both the copyright
 * notice and this permission notice appear in all copies of the
 * software, derivative works or modified versions, and any portions
 * thereof, and that both notices appear in supporting documentation.
 * 
 * CARNEGIE MELLON ALLOWS FREE USE OF THIS SOFTWARE IN ITS "AS IS"
 * CONDITION.  CARNEGIE MELLON DISCLAIMS ANY LIABILITY OF ANY KIND FOR
 * ANY DAMAGES WHATSOEVER RESULTING FROM THE USE OF THIS SOFTWARE.
 * 
 * Carnegie Mellon requests users of this software to return to
 * 
 *  Software Distribution Coordinator  or  Software.Distribution@CS.CMU.EDU
 *  School of Computer Science
 *  Carnegie Mellon University
 *  Pittsburgh PA 15213-3890
 * 
 * any improvements or extensions that they make and grant Carnegie Mellon
 * the rights to redistribute these changes.
 */
/*
 */
#include <mach_assert.h>

#include <string.h>
#include <mach/boolean.h>
#include <mach/i386/vm_types.h>
#include <mach/i386/vm_param.h>
#include <kern/kern_types.h>
#include <kern/misc_protos.h>
#include <i386/misc_protos.h>

	/*
	 * Should be rewritten in asm anyway.
	 */


/*
 * copy 'size' bytes from physical to physical address
 * the caller must validate the physical ranges 
 *
 * if flush_action == 0, no cache flush necessary
 * if flush_action == 1, flush the source
 * if flush_action == 2, flush the dest
 * if flush_action == 3, flush both source and dest
 */

kern_return_t copyp2p(vm_offset_t source, vm_offset_t dest, unsigned int size, unsigned int flush_action) {

        switch(flush_action) {
	case 1:
	        flush_dcache(source, size, 1);
		break;
	case 2:
	        flush_dcache(dest, size, 1);
		break;
	case 3:
	        flush_dcache(source, size, 1);
	        flush_dcache(dest, size, 1);
		break;

	}
        bcopy_phys((char *)source, (char *)dest, size);	/* Do a physical copy */

        switch(flush_action) {
	case 1:
	        flush_dcache(source, size, 1);
		break;
	case 2:
	        flush_dcache(dest, size, 1);
		break;
	case 3:
	        flush_dcache(source, size, 1);
	        flush_dcache(dest, size, 1);
		break;

	}
}



/*
 *              Copies data from a physical page to a virtual page.  This is used to
 *              move data from the kernel to user state.
 *
 */

kern_return_t
copyp2v(char *from, char *to, unsigned int size) {

  return(copyout(phystokv(from), to, size));
}

/*
 * bcopy_phys - like bcopy but copies from/to physical addresses.
 *              this is trivial since all phys mem is mapped into 
 *              kernel virtual space
 */

void
bcopy_phys(const char *from, char *to, vm_size_t bytes)
{
  bcopy((char *)phystokv(from), (char *)phystokv(to), bytes);
}


/* 
 * ovbcopy - like bcopy, but recognizes overlapping ranges and handles 
 *           them correctly.
 */

void
ovbcopy(
	const char	*from,
	char		*to,
	vm_size_t	bytes)		/* num bytes to copy */
{
	/* Assume that bcopy copies left-to-right (low addr first). */
	if (from + bytes <= to || to + bytes <= from || to == from)
		bcopy_no_overwrite(from, to, bytes);	/* non-overlapping or no-op*/
	else if (from > to)
		bcopy_no_overwrite(from, to, bytes);	/* overlapping but OK */
	else {
		/* to > from: overlapping, and must copy right-to-left. */
		from += bytes - 1;
		to += bytes - 1;
		while (bytes-- > 0)
			*to-- = *from--;
	}
}

void
bcopy(
	const char	*from,
	char		*to,
	vm_size_t	bytes)		/* num bytes to copy */
{
	ovbcopy(from, to, bytes);
}

int bcmp(
	const char	*a,
	const char	*b,
	vm_size_t	len)
{
	if (len == 0)
		return 0;

	do
		if (*a++ != *b++)
			break;
	while (--len);

	return len;
}

int
memcmp(s1, s2, n)
	register char *s1, *s2;
	register n;
{
	while (--n >= 0)
		if (*s1++ != *s2++)
			return (*--s1 - *--s2);
	return (0);
}

/*
 * Abstract:
 * strlen returns the number of characters in "string" preceeding
 * the terminating null character.
 */

size_t
strlen(
	register const char *string)
{
	register const char *ret = string;

	while (*string++ != '\0')
		continue;
	return string - 1 - ret;
}

#include <libkern/OSAtomic.h>

uint32_t
hw_atomic_add(
	uint32_t	*dest,
	uint32_t	delt)
{
	uint32_t	oldValue;
	uint32_t	newValue;
	
	do {
		oldValue = *dest;
		newValue = (oldValue + delt);
	} while (!OSCompareAndSwap((UInt32)oldValue,
									(UInt32)newValue, (UInt32 *)dest));
	
	return newValue;
}

uint32_t
hw_atomic_sub(
	uint32_t	*dest,
	uint32_t	delt)
{
	uint32_t	oldValue;
	uint32_t	newValue;
	
	do {
		oldValue = *dest;
		newValue = (oldValue - delt);
	} while (!OSCompareAndSwap((UInt32)oldValue,
									(UInt32)newValue, (UInt32 *)dest));
	
	return newValue;
}

uint32_t
hw_atomic_or(
	uint32_t	*dest,
	uint32_t	mask)
{
	uint32_t	oldValue;
	uint32_t	newValue;
	
	do {
		oldValue = *dest;
		newValue = (oldValue | mask);
	} while (!OSCompareAndSwap((UInt32)oldValue,
									(UInt32)newValue, (UInt32 *)dest));
	
	return newValue;
}

uint32_t
hw_atomic_and(
	uint32_t	*dest,
	uint32_t	mask)
{
	uint32_t	oldValue;
	uint32_t	newValue;
	
	do {
		oldValue = *dest;
		newValue = (oldValue & mask);
	} while (!OSCompareAndSwap((UInt32)oldValue,
									(UInt32)newValue, (UInt32 *)dest));
	
	return newValue;
}

uint32_t
hw_compare_and_store(
	uint32_t	oldval,
	uint32_t	newval,
	uint32_t	*dest)
{
	return OSCompareAndSwap((UInt32)oldval, (UInt32)newval, (UInt32 *)dest);
}

#if	MACH_ASSERT

/*
 * Machine-dependent routine to fill in an array with up to callstack_max
 * levels of return pc information.
 */
<<<<<<< HEAD
void machine_callstack(
	natural_t	*buf,
	vm_size_t	callstack_max)
=======

extern int copyout_user(const char *, vm_offset_t, vm_size_t);
extern int copyout_kern(const char *, vm_offset_t, vm_size_t);
extern int copyin_user(const vm_offset_t, char *, vm_size_t);
extern int copyin_kern(const vm_offset_t, char *, vm_size_t);
extern int copyoutphys_user(const char *, vm_offset_t, vm_size_t);
extern int copyoutphys_kern(const char *, vm_offset_t, vm_size_t);
extern int copyinphys_user(const vm_offset_t, char *, vm_size_t);
extern int copyinphys_kern(const vm_offset_t, char *, vm_size_t);
extern int copyinstr_user(const vm_offset_t, char *, vm_size_t, vm_size_t *);
extern int copyinstr_kern(const vm_offset_t, char *, vm_size_t, vm_size_t *);

static int copyio(int, user_addr_t, char *, vm_size_t, vm_size_t *, int);
static int copyio_phys(addr64_t, addr64_t, vm_size_t, int);


#define COPYIN		0
#define COPYOUT		1
#define COPYINSTR	2
#define COPYINPHYS	3
#define COPYOUTPHYS	4


void inval_copy_windows(thread_t thread)
{
        int	i;
	
	for (i = 0; i < NCOPY_WINDOWS; i++) {
                thread->machine.copy_window[i].user_base = -1;
	}
	thread->machine.nxt_window = 0;
	thread->machine.copyio_state = WINDOWS_DIRTY;

	KERNEL_DEBUG(0xeff70058 | DBG_FUNC_NONE, (uintptr_t)thread_tid(thread), (int)thread->map, 0, 0, 0);
}


static int
copyio(int copy_type, user_addr_t user_addr, char *kernel_addr,
       vm_size_t nbytes, vm_size_t *lencopied, int use_kernel_map)
{
        thread_t	thread;
	pmap_t		pmap;
	pt_entry_t	*updp;
	pt_entry_t	*kpdp;
	user_addr_t 	user_base;
	vm_offset_t 	user_offset;
	vm_offset_t 	kern_vaddr;
	vm_size_t	cnt;
	vm_size_t	bytes_copied;
	int		error = 0;
	int		window_index;
	int		copyio_state;
        boolean_t	istate;
#if KDEBUG
	int		debug_type = 0xeff70010;
	debug_type += (copy_type << 2);
#endif

	thread = current_thread();

	KERNEL_DEBUG(debug_type | DBG_FUNC_START, (int)(user_addr >> 32), (int)user_addr,
		     (int)nbytes, thread->machine.copyio_state, 0);

	if (nbytes == 0) {
	        KERNEL_DEBUG(debug_type | DBG_FUNC_END, (unsigned)user_addr,
			     (unsigned)kernel_addr, (unsigned)nbytes, 0, 0);
	        return (0);
	}
        pmap = thread->map->pmap;

        if (pmap == kernel_pmap || use_kernel_map) {

	        kern_vaddr = (vm_offset_t)user_addr;
	  
	        switch (copy_type) {

		case COPYIN:
		        error = copyin_kern(kern_vaddr, kernel_addr, nbytes);
			break;

		case COPYOUT:
		        error = copyout_kern(kernel_addr, kern_vaddr, nbytes);
			break;

		case COPYINSTR:
		        error = copyinstr_kern(kern_vaddr, kernel_addr, nbytes, lencopied);
			break;

		case COPYINPHYS:
		        error = copyinphys_kern(kern_vaddr, kernel_addr, nbytes);
			break;

		case COPYOUTPHYS:
		        error = copyoutphys_kern(kernel_addr, kern_vaddr, nbytes);
			break;
		}
		KERNEL_DEBUG(debug_type | DBG_FUNC_END, (unsigned)kern_vaddr,
			     (unsigned)kernel_addr, (unsigned)nbytes,
			     error | 0x80000000, 0);
		return (error);
	}

#if CONFIG_DTRACE
	thread->machine.specFlags |= CopyIOActive;
#endif /* CONFIG_DTRACE */

	if ((nbytes && (user_addr + nbytes <= user_addr)) ||
	    (user_addr          < vm_map_min(thread->map)) ||
	    (user_addr + nbytes > vm_map_max(thread->map))) {
		error = EFAULT;
		goto done;
	}

	user_base = user_addr & ~((user_addr_t)(NBPDE - 1));
	user_offset = (vm_offset_t)(user_addr & (NBPDE - 1));

	KERNEL_DEBUG(debug_type | DBG_FUNC_NONE, (int)(user_base >> 32), (int)user_base,
		     (int)user_offset, 0, 0);

	cnt = NBPDE - user_offset;

	if (cnt > nbytes)
	        cnt = nbytes;

	istate = ml_set_interrupts_enabled(FALSE);

	copyio_state = thread->machine.copyio_state;
	thread->machine.copyio_state = WINDOWS_OPENED;

	(void) ml_set_interrupts_enabled(istate);


	for (;;) {

	        for (window_index = 0; window_index < NCOPY_WINDOWS; window_index++) {
		        if (thread->machine.copy_window[window_index].user_base == user_base)
			        break;
		}
	        if (window_index >= NCOPY_WINDOWS) {

		        window_index = thread->machine.nxt_window;
			thread->machine.nxt_window++;

			if (thread->machine.nxt_window >= NCOPY_WINDOWS)
			        thread->machine.nxt_window = 0;
			thread->machine.copy_window[window_index].user_base = user_base;

			/*
			 * it's necessary to disable pre-emption
			 * since I have to compute the kernel descriptor pointer
			 * for the new window
			 */
			istate = ml_set_interrupts_enabled(FALSE);

		        updp = pmap_pde(pmap, user_base);

			kpdp = current_cpu_datap()->cpu_copywindow_pdp;
			kpdp += window_index;

			pmap_store_pte(kpdp, updp ? *updp : 0);

			(void) ml_set_interrupts_enabled(istate);

		        copyio_state = WINDOWS_DIRTY;

			KERNEL_DEBUG(0xeff70040 | DBG_FUNC_NONE, window_index,
				     (unsigned)user_base, (unsigned)updp,
				     (unsigned)kpdp, 0);

		}
#if JOE_DEBUG
		else {
			istate = ml_set_interrupts_enabled(FALSE);

		        updp = pmap_pde(pmap, user_base);

			kpdp = current_cpu_datap()->cpu_copywindow_pdp;

			kpdp += window_index;

			if ((*kpdp & PG_FRAME) != (*updp & PG_FRAME)) {
				panic("copyio: user pdp mismatch - kpdp = 0x%qx,  updp = 0x%qx\n", *kpdp, *updp);
			}
			(void) ml_set_interrupts_enabled(istate);
		}
#endif
		if (copyio_state == WINDOWS_DIRTY) {
		        flush_tlb();

		        copyio_state = WINDOWS_CLEAN;

			KERNEL_DEBUG(0xeff70054 | DBG_FUNC_NONE, window_index, 0, 0, 0, 0);
		}
		user_offset += (window_index * NBPDE);

		KERNEL_DEBUG(0xeff70044 | DBG_FUNC_NONE, (unsigned)user_offset,
			     (unsigned)kernel_addr, cnt, 0, 0);

	        switch (copy_type) {

		case COPYIN:
		        error = copyin_user(user_offset, kernel_addr, cnt);
			break;
			
		case COPYOUT:
		        error = copyout_user(kernel_addr, user_offset, cnt);
			break;

		case COPYINPHYS:
		        error = copyinphys_user(user_offset, kernel_addr, cnt);
			break;
			
		case COPYOUTPHYS:
		        error = copyoutphys_user(kernel_addr, user_offset, cnt);
			break;

		case COPYINSTR:
		        error = copyinstr_user(user_offset, kernel_addr, cnt, &bytes_copied);

			/*
			 * lencopied should be updated on success
			 * or ENAMETOOLONG...  but not EFAULT
			 */
			if (error != EFAULT)
			        *lencopied += bytes_copied;

			/*
			 * if we still have room, then the ENAMETOOLONG
			 * is just an artifact of the buffer straddling
			 * a window boundary and we should continue
			 */
			if (error == ENAMETOOLONG && nbytes > cnt)
			        error = 0;

			if (error) {
#if KDEBUG
			        nbytes = *lencopied;
#endif
			        break;
			}
			if (*(kernel_addr + bytes_copied - 1) == 0) {
			        /*
				 * we found a NULL terminator... we're done
				 */
#if KDEBUG
			        nbytes = *lencopied;
#endif
				goto done;
			}
			if (cnt == nbytes) {
			        /*
				 * no more room in the buffer and we haven't
				 * yet come across a NULL terminator
				 */
#if KDEBUG
			        nbytes = *lencopied;
#endif
			        error = ENAMETOOLONG;
				break;
			}
			assert(cnt == bytes_copied);

			break;
		}
		if (error)
		        break;
		if ((nbytes -= cnt) == 0)
		        break;

		kernel_addr += cnt;
		user_base += NBPDE;
		user_offset = 0;

		if (nbytes > NBPDE)
		        cnt = NBPDE;
		else
		        cnt = nbytes;
	}
done:
	thread->machine.copyio_state = WINDOWS_CLOSED;

	KERNEL_DEBUG(debug_type | DBG_FUNC_END, (unsigned)user_addr,
		     (unsigned)kernel_addr, (unsigned)nbytes, error, 0);

#if CONFIG_DTRACE
	thread->machine.specFlags &= ~CopyIOActive;
#endif /* CONFIG_DTRACE */

	return (error);
}


static int
copyio_phys(addr64_t source, addr64_t sink, vm_size_t csize, int which)
{
        pmap_paddr_t paddr;
	user_addr_t vaddr;
	char        *window_offset;
	pt_entry_t  pentry;
	int         ctype;
	int	    retval;
	boolean_t   istate;

	if (which & cppvPsnk) {
		paddr  = (pmap_paddr_t)sink;
	        vaddr  = (user_addr_t)source;
		ctype  = COPYINPHYS;
		pentry = (pt_entry_t)(INTEL_PTE_VALID | (paddr & PG_FRAME) | INTEL_PTE_RW);
	} else {
	        paddr  = (pmap_paddr_t)source;
		vaddr  = (user_addr_t)sink;
		ctype  = COPYOUTPHYS;
		pentry = (pt_entry_t)(INTEL_PTE_VALID | (paddr & PG_FRAME));
	}
	window_offset = (char *)((uint32_t)paddr & (PAGE_SIZE - 1));

	assert(!((current_thread()->machine.specFlags & CopyIOActive) && ((which & cppvKmap) == 0)));

	if (current_thread()->machine.physwindow_busy) {
	        pt_entry_t	old_pentry;

	        KERNEL_DEBUG(0xeff70048 | DBG_FUNC_NONE, paddr, csize, 0, -1, 0);
		/*
		 * we had better be targeting wired memory at this point
		 * we will not be able to handle a fault with interrupts
		 * disabled... we disable them because we can't tolerate
		 * being preempted during this nested use of the window
		 */
		istate = ml_set_interrupts_enabled(FALSE);

		old_pentry = *(current_cpu_datap()->cpu_physwindow_ptep);
		pmap_store_pte((current_cpu_datap()->cpu_physwindow_ptep), pentry);

		invlpg((uintptr_t)current_cpu_datap()->cpu_physwindow_base);

		retval = copyio(ctype, vaddr, window_offset, csize, NULL, which & cppvKmap);

		pmap_store_pte((current_cpu_datap()->cpu_physwindow_ptep), old_pentry);

		invlpg((uintptr_t)current_cpu_datap()->cpu_physwindow_base);

		(void) ml_set_interrupts_enabled(istate);
	} else {
	        /*
		 * mark the window as in use... if an interrupt hits while we're
		 * busy, or we trigger another coyppv from the fault path into
		 * the driver on a user address space page fault due to a copyin/out
		 * then we need to save and restore the current window state instead
		 * of caching the window preserving it across context switches
		 */
	        current_thread()->machine.physwindow_busy = 1;

	        if (current_thread()->machine.physwindow_pte != pentry) {
		        KERNEL_DEBUG(0xeff70048 | DBG_FUNC_NONE, paddr, csize, 0, 0, 0);

			current_thread()->machine.physwindow_pte = pentry;
			
			/*
			 * preemption at this point would be bad since we
			 * could end up on the other processor after we grabbed the
			 * pointer to the current cpu data area, but before we finished
			 * using it to stuff the page table entry since we would
			 * be modifying a window that no longer belonged to us
			 * the invlpg can be done unprotected since it only flushes
			 * this page address from the tlb... if it flushes the wrong
			 * one, no harm is done, and the context switch that moved us
			 * to the other processor will have already take care of 
			 * flushing the tlb after it reloaded the page table from machine.physwindow_pte
			 */
			istate = ml_set_interrupts_enabled(FALSE);

			pmap_store_pte((current_cpu_datap()->cpu_physwindow_ptep), pentry);
			(void) ml_set_interrupts_enabled(istate);

			invlpg((uintptr_t)current_cpu_datap()->cpu_physwindow_base);
		}
#if JOE_DEBUG
		else {
		        if (pentry !=
			    (*(current_cpu_datap()->cpu_physwindow_ptep) & (INTEL_PTE_VALID | PG_FRAME | INTEL_PTE_RW)))
			        panic("copyio_phys: pentry != *physwindow_ptep");
		}
#endif
		retval = copyio(ctype, vaddr, window_offset, csize, NULL, which & cppvKmap);

	        current_thread()->machine.physwindow_busy = 0;
	}
	return (retval);
}

int
copyinmsg(const user_addr_t user_addr, char *kernel_addr, vm_size_t nbytes)
{
        return (copyio(COPYIN, user_addr, kernel_addr, nbytes, NULL, 0));
}    

int
copyin(const user_addr_t user_addr, char *kernel_addr, vm_size_t nbytes)
{
        return (copyio(COPYIN, user_addr, kernel_addr, nbytes, NULL, 0));
}

int
copyinstr(const user_addr_t user_addr,  char *kernel_addr, vm_size_t nbytes, vm_size_t *lencopied)
{
	*lencopied = 0;

        return (copyio(COPYINSTR, user_addr, kernel_addr, nbytes, lencopied, 0));
}

int
copyoutmsg(const char *kernel_addr, user_addr_t user_addr, vm_size_t nbytes)
{
	return (copyio(COPYOUT, user_addr, (char *)(uintptr_t)kernel_addr, nbytes, NULL, 0));
}

int
copyout(const void *kernel_addr, user_addr_t user_addr, vm_size_t nbytes)
{
	return (copyio(COPYOUT, user_addr, (char *)(uintptr_t)kernel_addr, nbytes, NULL, 0));
}


kern_return_t
copypv(addr64_t src64, addr64_t snk64, unsigned int size, int which)
{
	unsigned int lop, csize;
	int bothphys = 0;
	
	KERNEL_DEBUG(0xeff7004c | DBG_FUNC_START, (unsigned)src64,
		     (unsigned)snk64, size, which, 0);

	if ((which & (cppvPsrc | cppvPsnk)) == 0 )				/* Make sure that only one is virtual */
		panic("copypv: no more than 1 parameter may be virtual\n");	/* Not allowed */

	if ((which & (cppvPsrc | cppvPsnk)) == (cppvPsrc | cppvPsnk))
	        bothphys = 1;							/* both are physical */

	while (size) {
	  
	        if (bothphys) {
		        lop = (unsigned int)(PAGE_SIZE - (snk64 & (PAGE_SIZE - 1)));		/* Assume sink smallest */

			if (lop > (unsigned int)(PAGE_SIZE - (src64 & (PAGE_SIZE - 1))))
			        lop = (unsigned int)(PAGE_SIZE - (src64 & (PAGE_SIZE - 1)));	/* No, source is smaller */
		} else {
		        /*
			 * only need to compute the resid for the physical page
			 * address... we don't care about where we start/finish in
			 * the virtual since we just call the normal copyin/copyout
			 */
		        if (which & cppvPsrc)
			        lop = (unsigned int)(PAGE_SIZE - (src64 & (PAGE_SIZE - 1)));
			else
			        lop = (unsigned int)(PAGE_SIZE - (snk64 & (PAGE_SIZE - 1)));
		}
		csize = size;						/* Assume we can copy it all */
		if (lop < size)
		        csize = lop;					/* Nope, we can't do it all */
#if 0		
		/*
		 * flush_dcache64 is currently a nop on the i386... 
		 * it's used when copying to non-system memory such
		 * as video capture cards... on PPC there was a need
		 * to flush due to how we mapped this memory... not
		 * sure if it's needed on i386.
		 */
		if (which & cppvFsrc)
		        flush_dcache64(src64, csize, 1);		/* If requested, flush source before move */
		if (which & cppvFsnk)
		        flush_dcache64(snk64, csize, 1);		/* If requested, flush sink before move */
#endif
		if (bothphys)
		        bcopy_phys(src64, snk64, csize);		/* Do a physical copy, virtually */
		else {
		        if (copyio_phys(src64, snk64, csize, which))
			        return (KERN_FAILURE);
		}
#if 0
		if (which & cppvFsrc)
		        flush_dcache64(src64, csize, 1);	/* If requested, flush source after move */
		if (which & cppvFsnk)
		        flush_dcache64(snk64, csize, 1);	/* If requested, flush sink after move */
#endif
		size   -= csize;					/* Calculate what is left */
		snk64 += csize;					/* Bump sink to next physical address */
		src64 += csize;					/* Bump source to next physical address */
	}
	KERNEL_DEBUG(0xeff7004c | DBG_FUNC_END, (unsigned)src64,
		     (unsigned)snk64, size, which, 0);

	return KERN_SUCCESS;
}

#if !MACH_KDP
void
kdp_register_callout(void)
{
}
#endif

#if !CONFIG_VMX
int host_vmxon(boolean_t exclusive __unused)
>>>>>>> origin/10.6
{
}

#endif	/* MACH_ASSERT */
