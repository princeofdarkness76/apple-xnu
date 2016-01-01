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
#include <assym.s>
#include <debug.h>
#include <cpus.h>
#include <db_machine_commands.h>
#include <mach_rt.h>
	
#include <mach_debug.h>
#include <ppc/asm.h>
#include <ppc/proc_reg.h>
#include <ppc/exception.h>
#include <ppc/Performance.h>
#include <ppc/exception.h>
#include <ppc/pmap_internals.h>
#include <mach/ppc/vm_param.h>
#define PERFTIMES 0
	
			.text

/*
 *
 *			Random notes and musings...
 *
 *			Access to mappings via the PTEG hash must be done with the list locked.
 *			Access via the physical entries is controlled by the physent lock.
 *			Access to mappings is controlled by the PTEG lock once they are queued.
 *			If they are not on the list, they don't really exist, so
 *			only one processor at a time can find them, so no access control is needed. 
 *
 *			The second half of the PTE is kept in the physical entry.  It is done this
 *			way, because there may be multiple mappings that refer to the same physical
 *			page (i.e., address aliases or synonymns).  We must do it this way, because
 *			maintenance of the reference and change bits becomes nightmarish if each mapping
 *			has its own. One side effect of this, and not necessarily a bad one, is that
 *			all mappings for a single page can have a single WIMG, protection state, and RC bits.
 *			The only "bad" thing, is the reference bit.  With a single copy, we can not get
 *			a completely accurate working set calculation, i.e., we can't tell which mapping was
 *			used to reference the page, all we can tell is that the physical page was 
 *			referenced.
 *
 *			The master copys of the reference and change bits are kept in the phys_entry.
 *			Other than the reference and change bits, changes to the phys_entry are not
 *			allowed if it has any mappings.  The master reference and change bits must be
 *			changed via atomic update.
 *
 *			Invalidating a PTE merges the RC bits into the phys_entry.
 *
 *			Before checking the reference and/or bits, ALL mappings to the physical page are
 *			invalidated.
 *			
 *			PTEs are never explicitly validated, they are always faulted in.  They are also
 *			not visible outside of the hw_vm modules.  Complete seperation of church and state.
 *
 *			Removal of a mapping is invalidates its PTE.
 *
 *			So, how do we deal with mappings to I/O space? We don't have a physent for it.
 *			Within the mapping is a copy of the second half of the PTE.  This is used
 *			ONLY when there is no physical entry.  It is swapped into the PTE whenever
 *			it is built.  There is no need to swap it back out, because RC is not
 *			maintained for these mappings.
 *
 *			So, I'm starting to get concerned about the number of lwarx/stcwx loops in
 *			this.  Satisfying a mapped address with no stealing requires one lock.  If we 
 *			steal an entry, there's two locks and an atomic update.  Invalidation of an entry
 *			takes one lock and, if there is a PTE, another lock and an atomic update.  Other 
 *			operations are multiples (per mapping) of the above.  Maybe we should look for
 *			an alternative.  So far, I haven't found one, but I haven't looked hard.
 */


/*			hw_add_map(struct mapping *mp, space_t space, vm_offset_t va) - Adds a mapping
 *
 *			Adds a mapping to the PTEG hash list.
 *
 *			Interrupts must be disabled before calling.
 *
 *			Using the space and the virtual address, we hash into the hash table
 *			and get a lock on the PTEG hash chain.  Then we chain the 
 *			mapping to the front of the list.
 *
 */

			.align	5
			.globl	EXT(hw_add_map)

LEXT(hw_add_map)

#if PERFTIMES && DEBUG
			mr		r7,r3
			mflr	r11
			li		r3,20
			bl		EXT(dbgLog2)						; Start of hw_add_map
			mr		r3,r7
			mtlr	r11
#endif

			mfmsr	r0							/* Get the MSR */
			eqv		r6,r6,r6					/* Fill the bottom with foxes */
			rlwinm	r11,r4,6,6,25				/* Position the space for the VSID */
			mfspr	r10,sdr1					/* Get hash table base and size */
			rlwimi	r11,r5,30,2,5				/* Insert the segment no. to make a VSID */
			mfsprg	r12,2						; Get feature flags
			rlwimi	r6,r10,16,0,15				/* Make table size -1 out of mask */
			rlwinm	r7,r5,26,10,25				/* Isolate the page index */
			or		r8,r10,r6					/* Point to the last byte in table */
			rlwinm	r9,r5,4,0,3					; Move nybble 1 up to 0
			xor		r7,r7,r11					/* Get primary hash */
			mtcrf	0x04,r12					; Set the features			
			andi.	r12,r0,0x7FCF				/* Disable translation and interruptions */
			rlwinm	r11,r11,1,1,24				/* Position VSID for pte ID */
			addi	r8,r8,1						/* Point to the PTEG Control Area */
			xor		r9,r9,r5					; Splooch vaddr nybble 0 and 1 together
			and		r7,r7,r6					/* Wrap the hash */
			rlwimi	r11,r5,10,26,31				/* Move API into pte ID */
			rlwinm	r9,r9,6,27,29				; Get splooched bits in place
			add		r8,r8,r7					/* Point to our PCA entry */
			rlwinm	r10,r4,2,27,29				; Get low 3 bits of the VSID for look-aside hash
			
			bt		pfNoMSRirb,hamNoMSR			; No MSR...

			mtmsr	r12							; Translation and all off
			isync								; Toss prefetch
			b		hamNoMSRx
			
hamNoMSR:	mr		r4,r0						; Save R0
			mr		r2,r3						; Save
			li		r0,loadMSR					; Get the MSR setter SC
			mr		r3,r12						; Get new MSR
			sc									; Set it
			mr		r0,r4						; Restore
			mr		r3,r2						; Restore
hamNoMSRx:

			la		r4,PCAhash(r8)				/* Point to the mapping hash area */
			xor		r9,r9,r10					; Finish splooching nybble 0, 1, and the low bits of the VSID
			isync								/* Get rid of anything prefetched before we ref storage */
/*
 *			We've now got the address of our PCA, the hash chain anchor, our API subhash,
 *			and word 0 of the PTE (the virtual part). 
 *
 *			Now, we just lock the PCA.		
 */
			
			li		r12,1						/* Get the locked value */
			dcbt	0,r4						/* We'll need the hash area in a sec, so get it */
			add		r4,r4,r9					/* Point to the right mapping hash slot */
			
			lwarx	r10,0,r8					; ?

ptegLckx:	lwarx	r10,0,r8					/* Get the PTEG lock */
			mr.		r10,r10						/* Is it locked? */
			bne-	ptegLckwx					/* Yeah... */
			stwcx.	r12,0,r8					/* Take take it */
			bne-	ptegLckx					/* Someone else was trying, try again... */
			b		ptegSXgx					/* All done... */

			.align	4
			
ptegLckwx:	mr.		r10,r10						/* Check if it's already held */
			beq+	ptegLckx					/* It's clear... */
			lwz		r10,0(r8)					/* Get lock word again... */
			b		ptegLckwx					/* Wait... */
			
			.align	4
			
ptegSXgx:	isync								/* Make sure we haven't used anything yet */

			lwz		r7,0(r4)					/* Pick up the anchor of hash list */
			stw		r3,0(r4)					/* Save the new head */
			stw		r7,mmhashnext(r3)			/* Chain in the old head */
			
			stw		r4,mmPTEhash(r3)			/* Point to the head of the hash list */
			
			sync								/* Make sure the chain is updated */
			stw		r10,0(r8)					/* Unlock the hash list */
			mtmsr	r0							/* Restore translation and interruptions */
			isync								/* Toss anything done with DAT off */
#if PERFTIMES && DEBUG
			mflr	r11
			mr		r4,r3
			li		r3,21
			bl		EXT(dbgLog2)				; end of hw_add_map
			mr		r3,r4
			mtlr	r11
#endif
			blr									/* Leave... */


/*			mp=hw_lock_phys_vir(space, va) - Finds and locks a physical entry by vaddr.
 *
 *			Returns the mapping with the associated physent locked if found, or a
 *			zero and no lock if not.  It we timed out trying to get a the lock on
 *			the physical entry, we retun a 1.  A physical entry can never be on an
 *			odd boundary, so we can distinguish between a mapping and a timeout code.
 *
 *			Interrupts must be disabled before calling.
 *
 *			Using the space and the virtual address, we hash into the hash table
 *			and get a lock on the PTEG hash chain.  Then we search the chain for the
 *			mapping for our virtual address.  From there, we extract the pointer to
 *			the physical entry.
 *
 *			Next comes a bit of monkey business.  we need to get a lock on the physical
 *			entry.  But, according to our rules, we can't get it after we've gotten the
 *			PTEG hash lock, we could deadlock if we do.  So, we need to release the
 *			hash lock.  The problem is, though, that as soon as we release it, some 
 *			other yahoo may remove our mapping between the time that we release the
 *			hash lock and obtain the phys entry lock.  So, we can't count on the 
 *			mapping once we release the lock.  Instead, after we lock the phys entry,
 *			we search the mapping list (phys_link) for our translation.  If we don't find it,
 *			we unlock the phys entry, bail out, and return a 0 for the mapping address.  If we 
 *			did find it, we keep the lock and return the address of the mapping block.
 *
 *			What happens when a mapping is found, but there is no physical entry?
 *			This is what happens when there is I/O area mapped.  It one of these mappings
 *			is found, the mapping is returned, as is usual for this call, but we don't
 *			try to lock anything.  There could possibly be some problems here if another
 *			processor releases the mapping while we still alre using it.  Hope this 
 *			ain't gonna happen.
 *
 *			Taaa-dahhh!  Easy as pie, huh?
 *
 *			So, we have a few hacks hacks for running translate off in here. 
 *			First, when we call the lock routine, we have carnel knowlege of the registers is uses. 
 *			That way, we don't need a stack frame, which we can't have 'cause the stack is in
 *			virtual storage.  But wait, as if that's not enough...  We need one more register.  So, 
 *			we cram the LR into the CTR and return from there.
 *
 */
			.align	5
			.globl	EXT(hw_lock_phys_vir)

LEXT(hw_lock_phys_vir)

#if PERFTIMES && DEBUG
			mflr	r11
			mr		r5,r3
			li		r3,22
			bl		EXT(dbgLog2)						; Start of hw_add_map
			mr		r3,r5
			mtlr	r11
#endif
			mfmsr	r12							/* Get the MSR */
			eqv		r6,r6,r6					/* Fill the bottom with foxes */
			mfsprg	r9,2						; Get feature flags 
			rlwinm	r11,r3,6,6,25				/* Position the space for the VSID */
			mfspr	r5,sdr1						/* Get hash table base and size */
			rlwimi	r11,r4,30,2,5				/* Insert the segment no. to make a VSID */
			mtcrf	0x04,r9						; Set the features			
			rlwimi	r6,r5,16,0,15				/* Make table size -1 out of mask */
			andi.	r0,r12,0x7FCF				/* Disable translation and interruptions */
			rlwinm	r9,r4,4,0,3					; Move nybble 1 up to 0
			rlwinm	r7,r4,26,10,25				/* Isolate the page index */
			or		r8,r5,r6					/* Point to the last byte in table */
			xor		r7,r7,r11					/* Get primary hash */
			rlwinm	r11,r11,1,1,24				/* Position VSID for pte ID */
			addi	r8,r8,1						/* Point to the PTEG Control Area */
			xor		r9,r9,r4					; Splooch vaddr nybble 0 and 1 together
			and		r7,r7,r6					/* Wrap the hash */
			rlwimi	r11,r4,10,26,31				/* Move API into pte ID */
			rlwinm	r9,r9,6,27,29				; Get splooched bits in place
			add		r8,r8,r7					/* Point to our PCA entry */
			rlwinm	r10,r3,2,27,29				; Get low 3 bits of the VSID for look-aside hash

			bt		pfNoMSRirb,hlpNoMSR			; No MSR...

			mtmsr	r0							; Translation and all off
			isync								; Toss prefetch
			b		hlpNoMSRx
			
hlpNoMSR:	mr		r3,r0						; Get the new MSR
			li		r0,loadMSR					; Get the MSR setter SC
			sc									; Set it
hlpNoMSRx:

			la		r3,PCAhash(r8)				/* Point to the mapping hash area */
			xor		r9,r9,r10					; Finish splooching nybble 0, 1, and the low bits of the VSID
			isync								/* Make sure translation is off before we ref storage */

/*
 *			We've now got the address of our PCA, the hash chain anchor, our API subhash,
 *			and word 0 of the PTE (the virtual part). 
 *
 *			Now, we just lock the PCA and find our mapping, if it exists.				
 */
			
			dcbt	0,r3						/* We'll need the hash area in a sec, so get it */
			add		r3,r3,r9					/* Point to the right mapping hash slot */
			
			lwarx	r10,0,r8					; ?

ptegLcka:	lwarx	r10,0,r8					/* Get the PTEG lock */
			li		r5,1						/* Get the locked value */
			mr.		r10,r10						/* Is it locked? */
			bne-	ptegLckwa					/* Yeah... */
			stwcx.	r5,0,r8						/* Take take it */
			bne-	ptegLcka					/* Someone else was trying, try again... */
			b		ptegSXga					/* All done... */
			
			.align	4

ptegLckwa:	mr.		r10,r10						/* Check if it's already held */
			beq+	ptegLcka					/* It's clear... */
			lwz		r10,0(r8)					/* Get lock word again... */
			b		ptegLckwa					/* Wait... */
			
			.align	4

ptegSXga:	isync								/* Make sure we haven't used anything yet */

			mflr	r0							/* Get the LR */
			lwz		r9,0(r3)					/* Pick up the first mapping block */
			mtctr	r0							/* Stuff it into the CTR */
			
findmapa:	

			mr.		r3,r9						/* Did we hit the end? */
			bne+	chkmapa						/* Nope... */
			
			stw		r3,0(r8)					/* Unlock the PTEG lock
												   Note: we never saved anything while we 
												   had the lock, so we don't need a sync 
												   before we unlock it */

vbail:		mtmsr	r12							/* Restore translation and interruptions */
			isync								/* Make sure translation is cool */
#if PERFTIMES && DEBUG
			mflr	r11
			mr		r4,r3
			li		r3,23
			bl		EXT(dbgLog2)				; Start of hw_add_map
			mr		r3,r4
			mtlr	r11
#endif
			bctr								/* Return in abject failure... */
			
			.align	4

chkmapa:	lwz		r10,mmPTEv(r3)				/* Pick up our virtual ID */
			lwz		r9,mmhashnext(r3)			/* Pick up next mapping block */
			cmplw	r10,r11						/* Have we found ourself? */
			bne-	findmapa					/* Nope, still wandering... */
			
			lwz		r9,mmphysent(r3)			/* Get our physical entry pointer */
			li		r5,0						/* Clear this out */
			mr.		r9,r9						/* Is there, like, a physical entry? */
			stw		r5,0(r8)					/* Unlock the PTEG lock
												   Note: we never saved anything while we 
												   had the lock, so we don't need a sync 
												   before we unlock it */
												   
			beq-	vbail						/* If there is no physical entry, it's time
												   to leave... */
												   
/*			Here we want to call hw_lock_bit.  We don't want to use the stack, 'cause it's
 *			in virtual storage, and we're in real.  So, we've carefully looked at the code
 *			in hw_lock_bit (and unlock) and cleverly don't use any of the registers that it uses.
 *			Be very, very aware of how you change this code.  By the way, it uses:
 *			R0, R6, R7, R8, and R9.  R3, R4, and R5 contain parameters
 *			Unfortunatly, we need to stash R9 still. So... Since we know we will not be interrupted
 *			('cause we turned off interruptions and translation is off) we will use SPRG3...
 */
 
			lwz		r10,mmPTEhash(r3)			/* Save the head of the hash-alike chain.  We need it to find ourselves later */
			lis		r5,HIGH_ADDR(EXT(LockTimeOut))	/* Get address of timeout value */
			la		r3,pephyslink(r9)			/* Point to the lock word */
			ori		r5,r5,LOW_ADDR(EXT(LockTimeOut))	/* Get second half of address */
			li		r4,PHYS_LOCK				/* Get the lock bit value */
			lwz		r5,0(r5)					/* Pick up the timeout value */
			mtsprg	3,r9						/* Save R9 in SPRG3 */
			
			bl		EXT(hw_lock_bit)			/* Go do the lock */
			
			mfsprg	r9,3						/* Restore pointer to the phys_entry */		
			mr.		r3,r3						/* Did we timeout? */
			lwz		r4,pephyslink(r9)			/* Pick up first mapping block */		
			beq-	penterr						/* Bad deal, we timed out... */

			rlwinm	r4,r4,0,0,26				; Clear out the flags from first link
			
findmapb:	mr.		r3,r4						/* Did we hit the end? */
			bne+	chkmapb						/* Nope... */
			
			la		r3,pephyslink(r9)			/* Point to where the lock is */						
			li		r4,PHYS_LOCK				/* Get the lock bit value */
			bl		EXT(hw_unlock_bit)			/* Go unlock the physentry */

			li		r3,0						/* Say we failed */			
			b		vbail						/* Return in abject failure... */
			
penterr:	li		r3,1						/* Set timeout */
			b		vbail						/* Return in abject failure... */
					
			.align	5

chkmapb:	lwz		r6,mmPTEv(r3)				/* Pick up our virtual ID */
			lwz		r4,mmnext(r3)				/* Pick up next mapping block */
			cmplw	r6,r11						/* Have we found ourself? */
			lwz		r5,mmPTEhash(r3)			/* Get the start of our hash chain */
			bne-	findmapb					/* Nope, still wandering... */
			cmplw	r5,r10						/* On the same hash chain? */
			bne-	findmapb					/* Nope, keep looking... */

			b		vbail						/* Return in glorious triumph... */


/*
 *			hw_rem_map(mapping) - remove a mapping from the system.
 *
 *			Upon entry, R3 contains a pointer to a mapping block and the associated
 *			physical entry is locked if there is one.
 *
 *			If the mapping entry indicates that there is a PTE entry, we invalidate
 *			if and merge the reference and change information into the phys_entry.
 *
 *			Next, we remove the mapping from the phys_ent and the PTEG hash list.
 *
 *			Unlock any locks that are left, and exit.
 *
 *			Note that this must be done with both interruptions off and VM off
 *	
 *			Note that this code depends upon the VSID being of the format 00SXXXXX
 *			where S is the segment number.
 *
 *			  
 */

			.align	5
			.globl	EXT(hw_rem_map)

LEXT(hw_rem_map)
#if PERFTIMES && DEBUG
			mflr	r11
			mr		r4,r3
			li		r3,24
			bl		EXT(dbgLog2)				; Start of hw_add_map
			mr		r3,r4
			mtlr	r11
#endif
 			mfsprg	r9,2						; Get feature flags 
			mfmsr	r0							/* Save the MSR  */
			rlwinm	r12,r0,0,MSR_EE_BIT+1,MSR_EE_BIT-1	/* Clear interruptions */
			mtcrf	0x04,r9						; Set the features			
			rlwinm	r12,r12,0,28,25				/* Clear IR and DR */

			bt		pfNoMSRirb,lmvNoMSR			; No MSR...

			mtmsr	r12							; Translation and all off
			isync								; Toss prefetch
			b		lmvNoMSRx
			
lmvNoMSR:	
			mr		r6,r0
			mr		r4,r3
			li		r0,loadMSR					; Get the MSR setter SC
			mr		r3,r12						; Get new MSR
			sc									; Set it
			mr		r3,r4
			mr		r0,r6

lmvNoMSRx:

		
			lwz		r6,mmPTEhash(r3)			/* Get pointer to hash list anchor */
			lwz		r5,mmPTEv(r3)				/* Get the VSID */
			dcbt	0,r6						/* We'll need that chain in a bit */

			rlwinm	r7,r6,0,0,25				/* Round hash list down to PCA boundary */
			li		r12,1						/* Get the locked value */
			subi	r6,r6,mmhashnext			/* Make the anchor look like an entry */

			lwarx	r10,0,r7					; ?

ptegLck1:	lwarx	r10,0,r7					/* Get the PTEG lock */
			mr.		r10,r10						/* Is it locked? */
			bne-	ptegLckw1					/* Yeah... */
			stwcx.	r12,0,r7					/* Try to take it */
			bne-	ptegLck1					/* Someone else was trying, try again... */
			b		ptegSXg1					/* All done... */
			
			.align	4

ptegLckw1:	mr.		r10,r10						/* Check if it's already held */
			beq+	ptegLck1					/* It's clear... */
			lwz		r10,0(r7)					/* Get lock word again... */
			b		ptegLckw1					/* Wait... */
			
			.align	4

ptegSXg1:	isync								/* Make sure we haven't used anything yet */

			lwz		r12,mmhashnext(r3)			/* Prime with our forward pointer */
 			lwz		r4,mmPTEent(r3)				/* Get the pointer to the PTE now that the lock's set */

srchmaps:	mr.		r10,r6						/* Save the previous entry */
			bne+	mapok						/* No error... */
			
			lis		r0,HIGH_ADDR(Choke)			/* We have a kernel choke!!! */
			ori		r0,r0,LOW_ADDR(Choke)		
			sc									/* Firmware Heimlich manuever */

			.align	4			

mapok:		lwz		r6,mmhashnext(r6)			/* Look at the next one */
			cmplwi	cr5,r4,0					/* Is there a PTE? */
			cmplw	r6,r3						/* Have we found ourselves? */
			bne+	srchmaps					/* Nope, get your head together... */
			
			stw		r12,mmhashnext(r10)			/* Remove us from the queue */
			rlwinm	r9,r5,1,0,3					/* Move in the segment */
			rlwinm	r8,r4,6,4,19				/* Line PTEG disp up to a page */
			rlwinm	r11,r5,5,4,19				/* Line up the VSID */
			lwz		r10,mmphysent(r3)			/* Point to the physical entry */
		
			beq+	cr5,nopte					/* There's no PTE to invalidate... */
			
			xor		r8,r8,r11					/* Back hash to virt index */
			lis		r12,HIGH_ADDR(EXT(tlb_system_lock))	/* Get the TLBIE lock */
			rlwimi	r9,r5,22,4,9				/* Move in the API */
			ori		r12,r12,LOW_ADDR(EXT(tlb_system_lock))	/* Grab up the bottom part */
			mfspr	r11,pvr						/* Find out what kind of machine we are */
			rlwimi	r9,r8,0,10,19				/* Create the virtual address */
			rlwinm	r11,r11,16,16,31			/* Isolate CPU type */

			stw		r5,0(r4)					/* Make the PTE invalid */		

			cmplwi	cr1,r11,3					/* Is this a 603? */
			sync								/* Make sure the invalid is stored */
						
			lwarx	r5,0,r12					; ?

tlbhang1:	lwarx	r5,0,r12					/* Get the TLBIE lock */
			rlwinm	r11,r4,29,29,31				/* Get the bit position of entry */
			mr.		r5,r5						/* Is it locked? */
			lis		r6,0x8000					/* Start up a bit mask */
			li		r5,1						/* Get our lock word */
			bne-	tlbhang1					/* It's locked, go wait... */
			stwcx.	r5,0,r12					/* Try to get it */
			bne-	tlbhang1					/* We was beat... */
			
			srw		r6,r6,r11					/* Make a "free slot" mask */
			lwz		r5,PCAallo(r7)				/* Get the allocation control bits */
			rlwinm	r11,r6,24,8,15				/* Make the autogen bit to turn off */
			or		r5,r5,r6					/* turn on the free bit */
			rlwimi	r11,r11,24,16,23			/* Get lock bit mask to turn it off */
			
			andc	r5,r5,r11					/* Turn off the lock and autogen bits in allocation flags */
			li		r11,0						/* Lock clear value */

			tlbie	r9							/* Invalidate it everywhere */

			
			beq-	cr1,its603a					/* It's a 603, skip the tlbsync... */
			
			eieio								/* Make sure that the tlbie happens first */
			tlbsync								/* wait for everyone to catch up */
			isync								
			
its603a:	sync								/* Make sure of it all */
			stw		r11,0(r12)					/* Clear the tlbie lock */
			eieio								/* Make sure those RC bit are loaded */
			stw		r5,PCAallo(r7)				/* Show that the slot is free */
			stw		r11,mmPTEent(r3)			/* Clear the pointer to the PTE */

nopte:		mr.		r10,r10						/* See if there is a physical entry */
			la		r9,pephyslink(r10)			/* Point to the physical mapping chain */
			beq-	nophys						/* No physical entry, we're done... */
			beq-	cr5,nadamrg					/* No PTE to merge... */

			lwz		r6,4(r4)					/* Get the latest reference and change bits */
			la		r12,pepte1(r10)				/* Point right at the master copy */
			rlwinm	r6,r6,0,23,24				/* Extract just the RC bits */
			
			lwarx	r8,0,r12					; ?

mrgrc:		lwarx	r8,0,r12					/* Get the master copy */
			or		r8,r8,r6					/* Merge in latest RC */
			stwcx.	r8,0,r12					/* Save it back */
			bne-	mrgrc						/* If it changed, try again... */
			
nadamrg:	li		r11,0						/* Clear this out */
			lwz		r12,mmnext(r3)				/* Prime with our next */
			stw		r11,0(r7)					/* Unlock the hash chain now so we don't
												   lock out another processor during the 
												   our next little search */
			
			
srchpmap:	mr.		r10,r9						/* Save the previous entry */
			bne+	mapok1						/* No error... */
			
			lis		r0,HIGH_ADDR(Choke)			/* We have a kernel choke!!! */
			ori		r0,r0,LOW_ADDR(Choke)			
			sc									/* Firmware Heimlich maneuver */
			
			.align	4

mapok1:		lwz		r9,mmnext(r9)				/* Look at the next one */
			rlwinm	r8,r9,0,27,31				; Save the flags (including the lock)
			rlwinm	r9,r9,0,0,26				; Clear out the flags from first link
			cmplw	r9,r3						/* Have we found ourselves? */
			bne+	srchpmap					/* Nope, get your head together... */
			
			rlwimi	r12,r8,0,27,31				; Insert the lock and flags */
			stw		r12,mmnext(r10)				/* Remove us from the queue */
			
			mtmsr	r0							/* Interrupts and translation back on */
			isync
#if PERFTIMES && DEBUG
			mflr	r11
			li		r3,25
			bl		EXT(dbgLog2)						; Start of hw_add_map
			mtlr	r11
#endif
			blr									/* Return... */

			.align	4

nophys:		li		r4,0						/* Make sure this is 0 */
			sync								/* Make sure that chain is updated */
			stw		r4,0(r7)					/* Unlock the hash chain */
			mtmsr	r0							/* Interrupts and translation back on */
			isync
#if PERFTIMES && DEBUG
			mflr	r11
			li		r3,25
			bl		EXT(dbgLog2)						; Start of hw_add_map
			mtlr	r11
#endif
			blr									/* Return... */


/*
 *			hw_prot(physent, prot) - Change the protection of a physical page
 *
 *			Upon entry, R3 contains a pointer to a physical entry which is locked.
 *			R4 contains the PPC protection bits.
 *
 *			The first thing we do is to slam the new protection into the phys entry.
 *			Then we scan the mappings and process each one.
 *
 *			Acquire the lock on the PTEG hash list for the mapping being processed.
 *
 *			If the current mapping has a PTE entry, we invalidate
 *			it and merge the reference and change information into the phys_entry.
 *
 *			Next, slam the protection bits into the entry and unlock the hash list.
 *
 *			Note that this must be done with both interruptions off and VM off
 *	
 *			  
 */

			.align	5
			.globl	EXT(hw_prot)

LEXT(hw_prot)
#if PERFTIMES && DEBUG
			mflr	r11
			mr		r7,r3
//			lwz		r5,4(r3)
			li		r5,0x1111
			li		r3,26
			bl		EXT(dbgLog2)				; Start of hw_add_map
			mr		r3,r7
			mtlr	r11
#endif
 			mfsprg	r9,2						; Get feature flags 
			mfmsr	r0							/* Save the MSR  */
			rlwinm	r12,r0,0,MSR_EE_BIT+1,MSR_EE_BIT-1	/* Clear interruptions */
			li		r5,pepte1					/* Get displacement to the second word of master pte */
			mtcrf	0x04,r9						; Set the features			
			rlwinm	r12,r12,0,28,25				/* Clear IR and DR */

			bt		pfNoMSRirb,hpNoMSR			; No MSR...

			mtmsr	r12							; Translation and all off
			isync								; Toss prefetch
			b		hpNoMSRx
			
hpNoMSR:	
			mr		r10,r0
			mr		r7,r3
			li		r0,loadMSR					; Get the MSR setter SC
			mr		r3,r12						; Get new MSR
			sc									; Set it
			mr		r0,r10
			mr		r3,r7
hpNoMSRx:


			
			lwz		r10,pephyslink(r3)			/* Get the first mapping block */
			rlwinm	r10,r10,0,0,26				; Clear out the flags from first link

/*
 *			Note that we need to to do the interlocked update here because another processor
 *			can be updating the reference and change bits even though the physical entry
 *			is locked.  All modifications to the PTE portion of the physical entry must be
 *			done via interlocked update.
 */

			lwarx	r8,r5,r3					; ?

protcng:	lwarx	r8,r5,r3					/* Get the master copy */
			rlwimi	r8,r4,0,30,31				/* Move in the protection bits */
			stwcx.	r8,r5,r3					/* Save it back */
			bne-	protcng						/* If it changed, try again... */



protnext:	mr.		r10,r10						/* Are there any more mappings? */
			beq-	protdone					/* Naw... */
			
			lwz		r7,mmPTEhash(r10)			/* Get pointer to hash list anchor */
			lwz		r5,mmPTEv(r10)				/* Get the virtual address */
			rlwinm	r7,r7,0,0,25				/* Round hash list down to PCA boundary */

			li		r12,1						/* Get the locked value */

			lwarx	r11,0,r7					; ?

protLck1:	lwarx	r11,0,r7					/* Get the PTEG lock */
			mr.		r11,r11						/* Is it locked? */
			bne-	protLckw1					/* Yeah... */
			stwcx.	r12,0,r7					/* Try to take it */
			bne-	protLck1					/* Someone else was trying, try again... */
			b		protSXg1					/* All done... */
			
			.align	4

protLckw1:	mr.		r11,r11						/* Check if it's already held */
			beq+	protLck1					/* It's clear... */
			lwz		r11,0(r7)					/* Get lock word again... */
			b		protLckw1					/* Wait... */
			
			.align	4

protSXg1:	isync								/* Make sure we haven't used anything yet */

 			lwz		r6,mmPTEent(r10)			/* Get the pointer to the PTE now that the lock's set */

			rlwinm	r9,r5,1,0,3					/* Move in the segment */
			lwz		r2,mmPTEr(r10)				; Get the mapping copy of the PTE
			mr.		r6,r6						/* See if there is a PTE here */
			rlwinm	r8,r5,31,2,25				/* Line it up */
			rlwimi	r2,r4,0,30,31				; Move protection bits into the mapping copy
		
			beq+	protul						/* There's no PTE to invalidate... */
			
			xor		r8,r8,r6					/* Back hash to virt index */
			rlwimi	r9,r5,22,4,9				/* Move in the API */
			lis		r12,HIGH_ADDR(EXT(tlb_system_lock))	/* Get the TLBIE lock */
			rlwinm	r5,r5,0,1,31				/* Clear the valid bit */
			ori		r12,r12,LOW_ADDR(EXT(tlb_system_lock))	/* Grab up the bottom part */
			mfspr	r11,pvr						/* Find out what kind of machine we are */
			rlwimi	r9,r8,6,10,19				/* Create the virtual address */
			rlwinm	r11,r11,16,16,31			/* Isolate CPU type */

			stw		r5,0(r6)					/* Make the PTE invalid */		
			cmplwi	cr1,r11,3					/* Is this a 603? */
			sync								/* Make sure the invalid is stored */
						
			lwarx	r11,0,r12					; ?

tlbhangp:	lwarx	r11,0,r12					/* Get the TLBIE lock */
			rlwinm	r8,r6,29,29,31				/* Get the bit position of entry */
			mr.		r11,r11						/* Is it locked? */
			lis		r5,0x8000					/* Start up a bit mask */
			li		r11,1						/* Get our lock word */
			bne-	tlbhangp					/* It's locked, go wait... */
			stwcx.	r11,0,r12					/* Try to get it */
			bne-	tlbhangp					/* We was beat... */
			
			li		r11,0						/* Lock clear value */

			tlbie	r9							/* Invalidate it everywhere */

			beq-	cr1,its603p					/* It's a 603, skip the tlbsync... */
			
			eieio								/* Make sure that the tlbie happens first */
			tlbsync								/* wait for everyone to catch up */
			isync								
			
its603p:	stw		r11,0(r12)					/* Clear the lock */
			srw		r5,r5,r8					/* Make a "free slot" mask */
			sync								/* Make sure of it all */

			lwz		r6,4(r6)					/* Get the latest reference and change bits */
			stw		r11,mmPTEent(r10)			/* Clear the pointer to the PTE */
			rlwinm	r6,r6,0,23,24				/* Extract the RC bits */
			lwz		r9,PCAallo(r7)				/* Get the allocation control bits */
			rlwinm	r8,r5,24,8,15				/* Make the autogen bit to turn off */
			rlwimi	r2,r6,0,23,24				; Put the latest RC bit in mapping copy
			or		r9,r9,r5					/* Set the slot free */
			rlwimi	r8,r8,24,16,23				/* Get lock bit mask to turn it off */
			andc	r9,r9,r8					/* Clear the auto and lock bits */
			li		r5,pepte1					/* Get displacement to the second word of master pte */
			stw		r9,PCAallo(r7)				/* Store the allocation controls */
			
			lwarx	r11,r5,r3					; ?
protmod:	lwarx	r11,r5,r3					/* Get the master copy */
			or		r11,r11,r6					/* Merge in latest RC */
			stwcx.	r11,r5,r3					/* Save it back */
			bne-	protmod						/* If it changed, try again... */
			
			sync								/* Make sure that chain is updated */

protul:		li		r4,0						/* Get a 0 */
			stw		r2,mmPTEr(r10)				; Save the updated mapping PTE
			lwz		r10,mmnext(r10)				/* Get the next */
			stw		r4,0(r7)					/* Unlock the hash chain */
			b		protnext					/* Go get the next one */
			
			.align	4

protdone:	mtmsr	r0							/* Interrupts and translation back on */
			isync
#if PERFTIMES && DEBUG
			mflr	r11
			li		r3,27
			bl		EXT(dbgLog2)				; Start of hw_add_map
			mtlr	r11
#endif
			blr									/* Return... */


/*
 *			hw_prot_virt(mapping, prot) - Change the protection of single page
 *
 *			Upon entry, R3 contains a pointer (real) to a mapping.
 *			R4 contains the PPC protection bits.
 *
 *			Acquire the lock on the PTEG hash list for the mapping being processed.
 *
 *			If the current mapping has a PTE entry, we invalidate
 *			it and merge the reference and change information into the phys_entry.
 *
 *			Next, slam the protection bits into the entry, merge the RC bits, 
 *			and unlock the hash list.
 *
 *			Note that this must be done with both interruptions off and VM off
 *	
 *			  
 */

			.align	5
			.globl	EXT(hw_prot_virt)

LEXT(hw_prot_virt)
#if PERFTIMES && DEBUG
			mflr	r11
			mr		r7,r3
//			lwz		r5,4(r3)
			li		r5,0x1111
			li		r3,40
			bl		EXT(dbgLog2)						; Start of hw_add_map
			mr		r3,r7
			mtlr	r11
#endif
 			mfsprg	r9,2						; Get feature flags 
			mfmsr	r0							/* Save the MSR  */
			rlwinm	r12,r0,0,MSR_EE_BIT+1,MSR_EE_BIT-1	/* Clear interruptions */
			mtcrf	0x04,r9						; Set the features			
			rlwinm	r12,r12,0,28,25				/* Clear IR and DR */
			
			bt		pfNoMSRirb,hpvNoMSR			; No MSR...

			mtmsr	r12							; Translation and all off
			isync								; Toss prefetch
			b		hpvNoMSRx
			
hpvNoMSR:	
			mr		r5,r0
			mr		r7,r3
			li		r0,loadMSR					; Get the MSR setter SC
			mr		r3,r12						; Get new MSR
			sc									; Set it
			mr		r3,r7
			mr		r0,r5
hpvNoMSRx:


			
/*
 *			Note that we need to to do the interlocked update here because another processor
 *			can be updating the reference and change bits even though the physical entry
 *			is locked.  All modifications to the PTE portion of the physical entry must be
 *			done via interlocked update.
 */
			
			lwz		r7,mmPTEhash(r3)			/* Get pointer to hash list anchor */
			lwz		r5,mmPTEv(r3)				/* Get the virtual address */
			rlwinm	r7,r7,0,0,25				/* Round hash list down to PCA boundary */

			li		r12,1						/* Get the locked value */

			lwarx	r11,0,r7					; ?

protvLck1:	lwarx	r11,0,r7					/* Get the PTEG lock */
			mr.		r11,r11						/* Is it locked? */
			bne-	protvLckw1					/* Yeah... */
			stwcx.	r12,0,r7					/* Try to take it */
			bne-	protvLck1					/* Someone else was trying, try again... */
			b		protvSXg1					/* All done... */
			
			.align	4

protvLckw1:	mr.		r11,r11						/* Check if it's already held */
			beq+	protvLck1					/* It's clear... */
			lwz		r11,0(r7)					/* Get lock word again... */
			b		protvLckw1					/* Wait... */
			
			.align	4

protvSXg1:	isync								/* Make sure we haven't used anything yet */

 			lwz		r6,mmPTEent(r3)				/* Get the pointer to the PTE now that the lock's set */
 			lwz		r2,mmPTEr(r3)				; Get the mapping copy if the real part

			rlwinm	r9,r5,1,0,3					/* Move in the segment */
			cmplwi	cr7,r6,0					; Any PTE to invalidate?
			rlwimi	r2,r4,0,30,31				; Move in the new protection bits
			rlwinm	r8,r5,31,2,25				/* Line it up */
		
			beq+	cr7,pvnophys				/* There's no PTE to invalidate... */
			
			xor		r8,r8,r6					/* Back hash to virt index */
			rlwimi	r9,r5,22,4,9				/* Move in the API */
			lis		r12,HIGH_ADDR(EXT(tlb_system_lock))	/* Get the TLBIE lock */
			rlwinm	r5,r5,0,1,31				/* Clear the valid bit */
			ori		r12,r12,LOW_ADDR(EXT(tlb_system_lock))	/* Grab up the bottom part */
			mfspr	r11,pvr						/* Find out what kind of machine we are */
			rlwimi	r9,r8,6,10,19				/* Create the virtual address */
			rlwinm	r11,r11,16,16,31			/* Isolate CPU type */

			stw		r5,0(r6)					/* Make the PTE invalid */		
			cmplwi	cr1,r11,3					/* Is this a 603? */
			sync								/* Make sure the invalid is stored */
						
			lwarx	r11,0,r12					; ?

tlbhangpv:	lwarx	r11,0,r12					/* Get the TLBIE lock */
			rlwinm	r8,r6,29,29,31				/* Get the bit position of entry */
			mr.		r11,r11						/* Is it locked? */
			lis		r5,0x8000					/* Start up a bit mask */
			li		r11,1						/* Get our lock word */
			bne-	tlbhangpv					/* It's locked, go wait... */
			stwcx.	r11,0,r12					/* Try to get it */
			bne-	tlbhangpv					/* We was beat... */
			
			li		r11,0						/* Lock clear value */

			tlbie	r9							/* Invalidate it everywhere */

			beq-	cr1,its603pv				/* It's a 603, skip the tlbsync... */
			
			eieio								/* Make sure that the tlbie happens first */
			tlbsync								/* wait for everyone to catch up */
			isync								
			
its603pv:	stw		r11,0(r12)					/* Clear the lock */
			srw		r5,r5,r8					/* Make a "free slot" mask */
			sync								/* Make sure of it all */

			lwz		r6,4(r6)					/* Get the latest reference and change bits */
			stw		r11,mmPTEent(r3)			/* Clear the pointer to the PTE */
			rlwinm	r6,r6,0,23,24				/* Extract the RC bits */
			lwz		r9,PCAallo(r7)				/* Get the allocation control bits */
			rlwinm	r8,r5,24,8,15				/* Make the autogen bit to turn off */
			lwz		r10,mmphysent(r3)			; Get any physical entry
			or		r9,r9,r5					/* Set the slot free */
			rlwimi	r8,r8,24,16,23				/* Get lock bit mask to turn it off */
			andc	r9,r9,r8					/* Clear the auto and lock bits */
			mr.		r10,r10						; Is there a physical entry?
			li		r5,pepte1					/* Get displacement to the second word of master pte */
			stw		r9,PCAallo(r7)				/* Store the allocation controls */
			rlwimi	r2,r6,0,23,24				; Stick in RC bits
			beq-	pvnophys					; No physical entry...
			
			
			lwarx	r11,r5,r10					; ?

protvmod:	lwarx	r11,r5,r10					/* Get the master copy */
			or		r11,r11,r6					/* Merge in latest RC */
			stwcx.	r11,r5,r10					/* Save it back */
			bne-	protvmod					/* If it changed, try again... */
			
			sync								/* Make sure that chain is updated */

pvnophys:	li		r4,0						/* Get a 0 */
			stw		r2,mmPTEr(r3)				; Set the real part of the PTE
			stw		r4,0(r7)					/* Unlock the hash chain */
			mtmsr	r0							; Restore interrupts and translation
			isync

#if PERFTIMES && DEBUG
			mflr	r11
			li		r3,41
			bl		EXT(dbgLog2)				
			mtlr	r11
#endif
			blr									/* Return... */


/*
 *			hw_attr_virt(mapping, attr) - Change the attributes of single page
 *
 *			Upon entry, R3 contains a pointer (real) to a mapping.
 *			R4 contains the WIMG bits.
 *
 *			Acquire the lock on the PTEG hash list for the mapping being processed.
 *
 *			If the current mapping has a PTE entry, we invalidate
 *			it and merge the reference and change information into the phys_entry.
 *
 *			Next, slam the WIMG bits into the entry, merge the RC bits, 
 *			and unlock the hash list.
 *
 *			Note that this must be done with both interruptions off and VM off
 *	
 *			  
 */

			.align	5
			.globl	EXT(hw_attr_virt)

LEXT(hw_attr_virt)
#if PERFTIMES && DEBUG
			mflr	r11
			mr		r7,r3
//			lwz		r5,4(r3)
			li		r5,0x1111
			li		r3,40
			bl		EXT(dbgLog2)						; Start of hw_add_map
			mr		r3,r7
			mtlr	r11
#endif
			mfsprg	r9,2						; Get feature flags 
 			mfmsr	r0							/* Save the MSR  */
			mtcrf	0x04,r9						; Set the features			
			rlwinm	r12,r0,0,MSR_EE_BIT+1,MSR_EE_BIT-1	/* Clear interruptions */
			rlwinm	r12,r12,0,28,25				/* Clear IR and DR */

			bt		pfNoMSRirb,havNoMSR			; No MSR...

			mtmsr	r12							; Translation and all off
			isync								; Toss prefetch
			b		havNoMSRx
			
havNoMSR:	
			mr		r5,r0
			mr		r7,r3
			li		r0,loadMSR					; Get the MSR setter SC
			mr		r3,r12						; Get new MSR
			sc									; Set it
			mr		r3,r7
			mr		r0,r5
havNoMSRx:

/*
 *			Note that we need to to do the interlocked update here because another processor
 *			can be updating the reference and change bits even though the physical entry
 *			is locked.  All modifications to the PTE portion of the physical entry must be
 *			done via interlocked update.
 */
			
			lwz		r7,mmPTEhash(r3)			/* Get pointer to hash list anchor */
			lwz		r5,mmPTEv(r3)				/* Get the virtual address */
			rlwinm	r7,r7,0,0,25				/* Round hash list down to PCA boundary */

			li		r12,1						/* Get the locked value */

			lwarx	r11,0,r7					; ?

attrvLck1:	lwarx	r11,0,r7					/* Get the PTEG lock */
			mr.		r11,r11						/* Is it locked? */
			bne-	attrvLckw1					/* Yeah... */
			stwcx.	r12,0,r7					/* Try to take it */
			bne-	attrvLck1					/* Someone else was trying, try again... */
			b		attrvSXg1					/* All done... */
			
			.align	4

attrvLckw1:	mr.		r11,r11						/* Check if it's already held */
			beq+	attrvLck1					/* It's clear... */
			lwz		r11,0(r7)					/* Get lock word again... */
			b		attrvLckw1					/* Wait... */
			
			.align	4

attrvSXg1:	isync								/* Make sure we haven't used anything yet */

 			lwz		r6,mmPTEent(r3)				/* Get the pointer to the PTE now that the lock's set */
 			lwz		r2,mmPTEr(r3)				; Get the mapping copy if the real part

			rlwinm	r9,r5,1,0,3					/* Move in the segment */
			mr.		r6,r6						/* See if there is a PTE here */
			rlwimi	r2,r4,0,25,28				; Move in the new attribute bits
			rlwinm	r8,r5,31,2,25				/* Line it up and check if empty */
		
			beq+	avnophys					/* There's no PTE to invalidate... */
			
			xor		r8,r8,r6					/* Back hash to virt index */
			rlwimi	r9,r5,22,4,9				/* Move in the API */
			lis		r12,HIGH_ADDR(EXT(tlb_system_lock))	/* Get the TLBIE lock */
			rlwinm	r5,r5,0,1,31				/* Clear the valid bit */
			ori		r12,r12,LOW_ADDR(EXT(tlb_system_lock))	/* Grab up the bottom part */
			mfspr	r11,pvr						/* Find out what kind of machine we are */
			rlwimi	r9,r8,6,10,19				/* Create the virtual address */
			rlwinm	r11,r11,16,16,31			/* Isolate CPU type */
			stw		r5,0(r6)					/* Make the PTE invalid */		
			cmplwi	cr1,r11,3					/* Is this a 603? */
			sync								/* Make sure the invalid is stored */
						
			lwarx	r11,0,r12					; ?

tlbhangav:	lwarx	r11,0,r12					/* Get the TLBIE lock */
			rlwinm	r8,r6,29,29,31				/* Get the bit position of entry */
			mr.		r11,r11						/* Is it locked? */
			lis		r5,0x8000					/* Start up a bit mask */
			li		r11,1						/* Get our lock word */
			bne-	tlbhangav					/* It's locked, go wait... */
			stwcx.	r11,0,r12					/* Try to get it */
			bne-	tlbhangav					/* We was beat... */
			
			li		r11,0						/* Lock clear value */

			tlbie	r9							/* Invalidate it everywhere */

			beq-	cr1,its603av				/* It's a 603, skip the tlbsync... */
			
			eieio								/* Make sure that the tlbie happens first */
			tlbsync								/* wait for everyone to catch up */
			isync								
			
its603av:	stw		r11,0(r12)					/* Clear the lock */
			srw		r5,r5,r8					/* Make a "free slot" mask */
			sync								/* Make sure of it all */

			lwz		r6,4(r6)					/* Get the latest reference and change bits */
			stw		r11,mmPTEent(r3)			/* Clear the pointer to the PTE */
			rlwinm	r6,r6,0,23,24				/* Extract the RC bits */
			lwz		r9,PCAallo(r7)				/* Get the allocation control bits */
			rlwinm	r8,r5,24,8,15				/* Make the autogen bit to turn off */
			lwz		r10,mmphysent(r3)			; Get any physical entry
			or		r9,r9,r5					/* Set the slot free */
			rlwimi	r8,r8,24,16,23				/* Get lock bit mask to turn it off */
			andc	r9,r9,r8					/* Clear the auto and lock bits */
			mr.		r10,r10						; Is there a physical entry?
			li		r5,pepte1					/* Get displacement to the second word of master pte */
			stw		r9,PCAallo(r7)				/* Store the allocation controls */
			rlwimi	r2,r6,0,23,24				; Stick in RC bits
			beq-	avnophys					; No physical entry...			
			
			lwarx	r11,r5,r10					; ?

attrvmod:	lwarx	r11,r5,r10					/* Get the master copy */
			or		r11,r11,r6					/* Merge in latest RC */
			stwcx.	r11,r5,r10					/* Save it back */
			bne-	attrvmod					/* If it changed, try again... */
			
			sync								/* Make sure that chain is updated */

avnophys:	li		r4,0						/* Get a 0 */
			stw		r2,mmPTEr(r3)				; Set the real part of the PTE
			stw		r4,0(r7)					/* Unlock the hash chain */
			
			rlwinm	r2,r2,0,0,19				; Clear back to page boundary
			
attrflsh:	cmplwi	r4,(4096-32)				; Are we about to do the last line on page?
			dcbst	r2,r4						; Flush cache because we changed attributes
			addi	r4,r4,32					; Bump up cache
			blt+	attrflsh					; Do the whole page...
			sync

			li		r4,0
attrimvl:	cmplwi	r4,(4096-32)				; Are we about to do the last line on page?
			dcbi	r2,r4						; Invalidate dcache because we changed attributes
			icbi	r2,r4						; Invalidate icache because we changed attributes
			icbi	r2,r4						; Invalidate icache because we changed attributes
			addi	r4,r4,32					; Bump up cache
			blt+	attrimvl					; Do the whole page...
			sync

			mtmsr	r0							; Restore interrupts and translation
			isync

#if PERFTIMES && DEBUG
			mflr	r11
			li		r3,41
			bl		EXT(dbgLog2)				
			mtlr	r11
#endif
			blr									/* Return... */


/*
 *			hw_pte_comm(physent) - Do something to the PTE pointing to a physical page
 *
 *			Upon entry, R3 contains a pointer to a physical entry which is locked.
 *			Note that this must be done with both interruptions off and VM off
 *
<<<<<<< HEAD
 *			First, we set up CRs 5 and 7 to indicate which of the 7 calls this is.
=======
 */

			.align	5
			.globl	EXT(hw_purge_map)

LEXT(hw_purge_map)
			stwu	r1,-(FM_ALIGN(hrmStackSize)+FM_SIZE)(r1)	; Make some space on the stack
			mflr	r0							; Save the link register
			stw		r15,FM_ARG0+0x00(r1)		; Save a register
			stw		r16,FM_ARG0+0x04(r1)		; Save a register
			stw		r17,FM_ARG0+0x08(r1)		; Save a register
			stw		r18,FM_ARG0+0x0C(r1)		; Save a register
			stw		r19,FM_ARG0+0x10(r1)		; Save a register
 			mfsprg	r19,2						; Get feature flags 
			stw		r20,FM_ARG0+0x14(r1)		; Save a register
			stw		r21,FM_ARG0+0x18(r1)		; Save a register
			mtcrf	0x02,r19					; move pf64Bit cr6
			stw		r22,FM_ARG0+0x1C(r1)		; Save a register
			stw		r23,FM_ARG0+0x20(r1)		; Save a register
			stw		r24,FM_ARG0+0x24(r1)		; Save a register
			stw		r25,FM_ARG0+0x28(r1)		; Save a register
			stw		r26,FM_ARG0+0x2C(r1)		; Save a register
			stw		r27,FM_ARG0+0x30(r1)		; Save a register
			stw		r28,FM_ARG0+0x34(r1)		; Save a register
			stw		r29,FM_ARG0+0x38(r1)		; Save a register
			stw		r30,FM_ARG0+0x3C(r1)		; Save a register
			stw		r31,FM_ARG0+0x40(r1)		; Save a register
			stw		r6,FM_ARG0+0x44(r1)			; Save address to save next mapped vaddr
			stw		r0,(FM_ALIGN(hrmStackSize)+FM_SIZE+FM_LR_SAVE)(r1)	; Save the return

 			bt++	pf64Bitb,hpmSF1				; skip if 64-bit (only they take the hint)
			lwz		r9,pmapvr+4(r3)				; Get conversion mask
			b		hpmSF1x						; Done...
			
hpmSF1:		ld		r9,pmapvr(r3)				; Get conversion mask

hpmSF1x:	
			bl		EXT(mapSetUp)				; Turn off interrupts, translation, and possibly enter 64-bit

			xor		r28,r3,r9					; Convert the pmap to physical addressing

			mr		r17,r11						; Save the MSR

			la		r3,pmapSXlk(r28)			; Point to the pmap search lock
			bl		sxlkExclusive				; Go get an exclusive lock on the mapping lists
			mr.		r3,r3						; Did we get the lock?
			bne--	hrmBadLock					; Nope...
;
;			Note that we do a full search (i.e., no shortcut level skips, etc.)
;			here so that we will know the previous elements so we can dequeue them
;			later.
;
hpmSearch:
			mr		r3,r28						; Pass in pmap to search
			mr		r29,r4						; Top half of vaddr
			mr		r30,r5						; Bottom half of vaddr
			bl		EXT(mapSearchFull)			; Rescan the list
			mr.		r31,r3						; Did we? (And remember mapping address for later)
			or		r0,r4,r5					; Are we beyond the end?
			mr		r15,r4						; Save top of next vaddr
			cmplwi	cr1,r0,0					; See if there is another
			mr		r16,r5						; Save bottom of next vaddr
			bne--	hpmGotOne					; We found one, go check it out...

hpmCNext:	bne++	cr1,hpmSearch				; There is another to check...
			b		hrmNotFound					; No more in pmap to check...

hpmGotOne:	lwz		r20,mpFlags(r3)				; Get the flags
			andi.	r9,r20,lo16(mpSpecial|mpNest|mpPerm|mpBlock)	; Are we allowed to remove it?
			rlwinm	r21,r20,8,24,31				; Extract the busy count
			cmplwi	cr2,r21,0					; Is it busy?
			crand	cr0_eq,cr2_eq,cr0_eq		; not busy and can be removed?
			beq++	hrmGotX						; Found, branch to remove the mapping...
			b		hpmCNext					; Nope...

/*
 *			mapping *hw_purge_space(physent, pmap) - remove a mapping from the system based upon address space
>>>>>>> origin/10.3
 *
 *			Now we scan the mappings to invalidate any with an active PTE.
 *
 *				Acquire the lock on the PTEG hash list for the mapping being processed.
 *
 *				If the current mapping has a PTE entry, we invalidate
 *				it and merge the reference and change information into the phys_entry.
 *
 *				Next, unlock the hash list and go on to the next mapping.
 *
 *	
 *			  
 */

			.align	5
			.globl	EXT(hw_inv_all)

LEXT(hw_inv_all)
	
			li		r9,0x800					/* Indicate invalidate all */
			li		r2,0						; No inadvertant modifications please
			b		hw_pte_comm					/* Join in the fun... */


			.align	5
			.globl	EXT(hw_tst_mod)

LEXT(hw_tst_mod)

			lwz		r8,pepte1(r3)				; Get the saved PTE image
			li		r9,0x400					/* Indicate test modify */
			li		r2,0						; No inadvertant modifications please
			rlwinm.	r8,r8,25,31,31				; Make change bit into return code
			beq+	hw_pte_comm					; Assume we do not know if it is set...
			mr		r3,r8						; Set the return code
			blr									; Return quickly...

 			.align	5
			.globl	EXT(hw_tst_ref)

LEXT(hw_tst_ref)
			lwz		r8,pepte1(r3)				; Get the saved PTE image
			li		r9,0x200					/* Indicate test reference bit */
			li		r2,0						; No inadvertant modifications please
			rlwinm.	r8,r8,24,31,31				; Make reference bit into return code
			beq+	hw_pte_comm					; Assume we do not know if it is set...
			mr		r3,r8						; Set the return code
			blr									; Return quickly...

/*
 *			Note that the following are all in one CR for ease of use later
 */
			.align	4
			.globl	EXT(hw_set_mod)

LEXT(hw_set_mod)
			
			li		r9,0x008					/* Indicate set modify bit */
			li		r2,0x4						; Set set C, clear none
			b		hw_pte_comm					/* Join in the fun... */


			.align	4
			.globl	EXT(hw_clr_mod)

LEXT(hw_clr_mod)
			
			li		r9,0x004					/* Indicate clear modify bit */
			li		r2,0x1						; Set set none, clear C
			b		hw_pte_comm					/* Join in the fun... */


			.align	4
			.globl	EXT(hw_set_ref)

LEXT(hw_set_ref)
			
			li		r9,0x002					/* Indicate set reference */
			li		r2,0x8						; Set set R, clear none
			b		hw_pte_comm					/* Join in the fun... */

			.align	5
			.globl	EXT(hw_clr_ref)

LEXT(hw_clr_ref)
			
			li		r9,0x001					/* Indicate clear reference bit */
			li		r2,0x2						; Set set none, clear R
			b		hw_pte_comm					/* Join in the fun... */


/*
 *			This is the common stuff.
 */

			.align	5

hw_pte_comm:									/* Common routine for pte tests and manips */
 
#if PERFTIMES && DEBUG
			mflr	r11
			mr		r7,r3
			lwz		r4,4(r3)
			mr		r5,r9			
			li		r3,28
			bl		EXT(dbgLog2)						; Start of hw_add_map
			mr		r3,r7
			mtlr	r11
#endif
 			mfsprg	r8,2						; Get feature flags 
			lwz		r10,pephyslink(r3)			/* Get the first mapping block */
			mfmsr	r0							/* Save the MSR  */
			rlwinm.	r10,r10,0,0,26				; Clear out the flags from first link and see if we are mapped
			rlwinm	r12,r0,0,MSR_EE_BIT+1,MSR_EE_BIT-1	/* Clear interruptions */
			mtcrf	0x04,r8						; Set the features			
			rlwinm	r12,r12,0,28,25				/* Clear IR and DR */
			beq-	comnmap						; No mapping
			dcbt	br0,r10						; Touch the first mapping in before the isync
			
comnmap:

			bt		pfNoMSRirb,hpcNoMSR			; No MSR...

			mtmsr	r12							; Translation and all off
			isync								; Toss prefetch
			b		hpcNoMSRx
			
hpcNoMSR:	
			mr		r5,r0
			mr		r7,r3
			li		r0,loadMSR					; Get the MSR setter SC
			mr		r3,r12						; Get new MSR
			sc									; Set it
			mr		r3,r7
			mr		r0,r5
hpcNoMSRx:

			mtcrf	0x05,r9						/* Set the call type flags into cr5 and 7 */

			beq-	commdone					; Nothing us mapped to this page...
			b		commnext					; Jump to first pass (jump here so we can align loop)
		
			.align	5	

commnext:	lwz		r11,mmnext(r10)				; Get the pointer to the next mapping (if any)
			lwz		r7,mmPTEhash(r10)			/* Get pointer to hash list anchor */
			lwz		r5,mmPTEv(r10)				/* Get the virtual address */
			mr.		r11,r11						; More mappings to go?
			rlwinm	r7,r7,0,0,25				/* Round hash list down to PCA boundary */
			beq-	commnxtch					; No more mappings...
			dcbt	br0,r11						; Touch the next mapping

commnxtch:	li		r12,1						/* Get the locked value */

			lwarx	r11,0,r7					; ?

commLck1:	lwarx	r11,0,r7					/* Get the PTEG lock */
			mr.		r11,r11						/* Is it locked? */
			bne-	commLckw1					/* Yeah... */
			stwcx.	r12,0,r7					/* Try to take it */
			bne-	commLck1					/* Someone else was trying, try again... */
			b		commSXg1					/* All done... */
			
			.align	4

commLckw1:	mr.		r11,r11						/* Check if it's already held */
			beq+	commLck1					/* It's clear... */
			lwz		r11,0(r7)					/* Get lock word again... */
			b		commLckw1					/* Wait... */
			
			.align	4

commSXg1:	isync								/* Make sure we haven't used anything yet */

 			lwz		r6,mmPTEent(r10)			/* Get the pointer to the PTE now that the lock's set */

			rlwinm	r9,r5,1,0,3					/* Move in the segment */
			mr.		r6,r6						/* See if there is a PTE entry here */
			rlwinm	r8,r5,31,2,25				/* Line it up and check if empty */
		
			beq+	commul						/* There's no PTE to invalidate... */
			
			xor		r8,r8,r6					/* Back hash to virt index */
			rlwimi	r9,r5,22,4,9				/* Move in the API */
			lis		r12,HIGH_ADDR(EXT(tlb_system_lock))		/* Get the TLBIE lock */
			rlwinm	r5,r5,0,1,31				/* Clear the valid bit */
			ori		r12,r12,LOW_ADDR(EXT(tlb_system_lock))	/* Grab up the bottom part */
			rlwimi	r9,r8,6,10,19				/* Create the virtual address */

			stw		r5,0(r6)					/* Make the PTE invalid */		
			mfspr	r4,pvr						/* Find out what kind of machine we are */
			sync								/* Make sure the invalid is stored */
						
			lwarx	r11,0,r12					; ?

tlbhangco:	lwarx	r11,0,r12					/* Get the TLBIE lock */
			rlwinm	r8,r6,29,29,31				/* Get the bit position of entry */
			mr.		r11,r11						/* Is it locked? */
			lis		r5,0x8000					/* Start up a bit mask */
			li		r11,1						/* Get our lock word */
			bne-	tlbhangco					/* It's locked, go wait... */
			stwcx.	r11,0,r12					/* Try to get it */
			bne-	tlbhangco					/* We was beat... */
			
			rlwinm	r4,r4,16,16,31				/* Isolate CPU type */
			li		r11,0						/* Lock clear value */
			cmplwi	r4,3						/* Is this a 603? */

			tlbie	r9							/* Invalidate it everywhere */

			beq-	its603co					/* It's a 603, skip the tlbsync... */
			
			eieio								/* Make sure that the tlbie happens first */
			tlbsync								/* wait for everyone to catch up */
			isync								
			
its603co:	stw		r11,0(r12)					/* Clear the lock */
			srw		r5,r5,r8					/* Make a "free slot" mask */
			sync								/* Make sure of it all */

			lwz		r6,4(r6)					/* Get the latest reference and change bits */
			lwz		r9,PCAallo(r7)				/* Get the allocation control bits */
			stw		r11,mmPTEent(r10)			/* Clear the pointer to the PTE */
			rlwinm	r8,r5,24,8,15				/* Make the autogen bit to turn off */
			or		r9,r9,r5					/* Set the slot free */
			rlwimi	r8,r8,24,16,23				/* Get lock bit mask to turn it off */
			rlwinm	r4,r6,0,23,24				/* Extract the RC bits */
			andc	r9,r9,r8					/* Clear the auto and lock bits */
			li		r5,pepte1					/* Get displacement to the second word of master pte */
			stw		r9,PCAallo(r7)				/* Store the allocation controls */
			
			lwarx	r11,r5,r3					; ?
commmod:	lwarx	r11,r5,r3					/* Get the master copy */
			or		r11,r11,r4					/* Merge in latest RC */
			stwcx.	r11,r5,r3					/* Save it back */
			bne-	commmod						/* If it changed, try again... */

			sync								/* Make sure that chain is updated */
			b		commulnl					; Skip loading the old real part...

commul:		lwz		r6,mmPTEr(r10)				; Get the real part

commulnl:	rlwinm	r12,r2,5,23,24				; Get the "set" bits
			rlwinm	r11,r2,7,23,24				; Get the "clear" bits
			
			or		r6,r6,r12					; Set the bits to come on
			andc	r6,r6,r11					; Clear those to come off

			stw		r6,mmPTEr(r10)				; Set the new RC

			lwz		r10,mmnext(r10)				/* Get the next */
			li		r4,0						/* Make sure this is 0 */
			mr.		r10,r10						; Is there another mapping?
			stw		r4,0(r7)					/* Unlock the hash chain */
			bne+	commnext					; Go get the next if there is one...
			
/*
 *			Now that all PTEs have been invalidated and the master RC bits are updated,
 *			we go ahead and figure out what the original call was and do that.  Note that
 *			another processor could be messing around and may have entered one of the 
 *			PTEs we just removed into the hash table.  Too bad...  You takes yer chances.
 *			If there's a problem with that, it's because some higher level was trying to
 *			do something with a mapping that it shouldn't.  So, the problem's really
 *			there, nyaaa, nyaaa, nyaaa... nyaaa, nyaaa... nyaaa! So there!
 */

commdone:	li		r5,pepte1					/* Get displacement to the second word of master pte */
			blt		cr5,commfini				/* We're finished, it was invalidate all... */
			bgt		cr5,commtst					/* It was a test modified... */
			beq		cr5,commtst					/* It was a test reference... */

/*
 *			Note that we need to to do the interlocked update here because another processor
 *			can be updating the reference and change bits even though the physical entry
 *			is locked.  All modifications to the PTE portion of the physical entry must be
 *			done via interlocked update.
 */

			rlwinm	r12,r2,5,23,24				; Get the "set" bits
			rlwinm	r11,r2,7,23,24				; Get the "clear" bits

			lwarx	r8,r5,r3					; ?

commcng:	lwarx	r8,r5,r3					/* Get the master copy */
			or		r8,r8,r12					; Set the bits to come on
			andc	r8,r8,r11					; Clear those to come off
			stwcx.	r8,r5,r3					/* Save it back */
			bne-	commcng						/* If it changed, try again... */

			mtmsr	r0							/* Interrupts and translation back on */
			isync
#if PERFTIMES && DEBUG
			mflr	r11
			mr		r4,r3
			li		r3,29
			bl		EXT(dbgLog2)						; Start of hw_add_map
			mr		r3,r4
			mtlr	r11
#endif
			blr									/* Return... */

			.align	4

commtst:	lwz		r8,pepte1(r3)				/* Get the PTE */
			bne-	cr5,commtcb					; This is for the change bit...
			mtmsr	r0							; Interrupts and translation back on
			rlwinm	r3,r8,24,31,31				; Copy reference bit to bit 31
			isync								; Toss prefetching
#if PERFTIMES && DEBUG
			mflr	r11
			mr		r4,r3
			li		r3,29
			bl		EXT(dbgLog2)						; Start of hw_add_map
			mr		r3,r4
			mtlr	r11
#endif
			blr									; Return...

			.align	4

commtcb:	rlwinm	r3,r8,25,31,31				; Copy change bit to bit 31

commfini:	mtmsr	r0							; Interrupts and translation back on
			isync								; Toss prefetching

#if PERFTIMES && DEBUG
			mflr	r11
			mr		r4,r3
			li		r3,29
			bl		EXT(dbgLog2)						; Start of hw_add_map
			mr		r3,r4
			mtlr	r11
#endif
			blr									; Return...

/*
 *			unsigned int hw_test_rc(mapping *mp, boolean_t reset);
 *
 *			Test the RC bits for a specific mapping.  If reset is non-zero, clear them.
 *			We return the RC value in the mapping if there is no PTE or if C is set.
 *			(Note: R is always set with C.) Otherwise we invalidate the PTE and
 *			collect the RC bits from there, also merging them into the global copy.
 *			
 *			For now, we release the PTE slot and leave it invalid.  In the future, we
 *			may consider re-validating and not releasing the slot.  It would be faster,
 *			but our current implementation says that we will have not PTEs valid
 *			without the reference bit set.
 *
 *			We will special case C==1 && not reset to just return the RC.
 *
 *			Probable state is worst performance state: C bit is off and there is a PTE.
 */

#define		htrReset 31

			.align	5
			.globl	EXT(hw_test_rc)

LEXT(hw_test_rc)

 			mfsprg	r9,2						; Get feature flags 
			mfmsr	r0							; Save the MSR 
 			mr.		r4,r4						; See if we have a reset to do later
			rlwinm	r12,r0,0,MSR_EE_BIT+1,MSR_EE_BIT-1	; Clear interruption mask
			crnot	htrReset,cr0_eq				; Remember reset
			mtcrf	0x04,r9						; Set the features			
			rlwinm	r12,r12,0,28,25				; Clear IR and DR
			
			bt		pfNoMSRirb,htrNoMSR			; No MSR...

			mtmsr	r12							; Translation and all off
			isync								; Toss prefetch
			b		htrNoMSRx
			
htrNoMSR:	
			mr		r2,r0
			mr		r7,r3
			li		r0,loadMSR					; Get the MSR setter SC
			mr		r3,r12						; Get new MSR
			sc									; Set it
			mr		r3,r7
			mr		r0,r2
htrNoMSRx:
			
			lwz		r2,mmPTEr(r3)				; Get the real part
			lwz		r7,mmPTEhash(r3)			; Get pointer to hash list anchor
			rlwinm.	r12,r2,0,24,24				; Is the change bit on?
			lwz		r5,mmPTEv(r3)				; Get the virtual address
			crnor	cr0_eq,cr0_eq,htrReset		; Set if C=1 && not reset
			rlwinm	r7,r7,0,0,25				; Round hash list down to PCA boundary 
			bt		cr0_eq,htrcset				; Special case changed but no reset case...

			li		r12,1						; Get the locked value

htrLck1:	lwarx	r11,0,r7					; Get the PTEG lock
			mr.		r11,r11						; Is it locked?
			bne-	htrLckw1					; Yeah...
			stwcx.	r12,0,r7					; Try to take it
			bne-	htrLck1						; Someone else was trying, try again...
			b		htrSXg1						; All done...
			
			.align	4

htrLckw1:	mr.		r11,r11						; Check if it is already held 
			beq+	htrLck1						; It is clear... 
			lwz		r11,0(r7)					; Get lock word again... 
			b		htrLckw1					; Wait... 
			
			.align	4

htrSXg1:	isync								; Make sure we have not used anything yet

 			lwz		r6,mmPTEent(r3)				; Get the pointer to the PTE now that the lock is set
 			lwz		r2,mmPTEr(r3)				; Get the mapping copy of the real part

			rlwinm	r9,r5,1,0,3					; Move in the segment
			mr.		r6,r6						; Any PTE to invalidate?
			rlwinm	r8,r5,31,2,25				; Line it up 
		
			beq+	htrnopte					; There is no PTE to invalidate...
			
			xor		r8,r8,r6					; Back hash to virt index
			rlwimi	r9,r5,22,4,9				; Move in the API
			lis		r12,HIGH_ADDR(EXT(tlb_system_lock))	; Get the TLBIE lock
			rlwinm	r5,r5,0,1,31				; Clear the valid bit
			ori		r12,r12,LOW_ADDR(EXT(tlb_system_lock))	; Grab up the bottom part
			mfspr	r11,pvr						; Find out what kind of machine we are
			rlwimi	r9,r8,6,10,19				; Create the virtual address
			rlwinm	r11,r11,16,16,31			; Isolate CPU type 

			stw		r5,0(r6)					; Make the PTE invalid	
			cmplwi	cr1,r11,3					; Is this a 603?
			sync								; Make sure the invalid is stored
						
htrtlbhang:	lwarx	r11,0,r12					; Get the TLBIE lock
			rlwinm	r8,r6,29,29,31				; Get the bit position of entry 
			mr.		r11,r11						; Is it locked?
			lis		r5,0x8000					; Start up a bit mask
			li		r11,1						; Get our lock word 
			bne-	htrtlbhang					; It is locked, go wait...
			stwcx.	r11,0,r12					; Try to get it
			bne-	htrtlbhang					; We was beat...
			
			li		r11,0						; Lock clear value 

			tlbie	r9							;Invalidate it everywhere

			beq-	cr1,htr603					; It is a 603, skip the tlbsync... 
			
			eieio								; Make sure that the tlbie happens first
			tlbsync								; wait for everyone to catch up
			isync								
			
htr603:		stw		r11,0(r12)					; Clear the lock
			srw		r5,r5,r8					; Make a "free slot" mask 
			sync								; Make sure of it all 

			lwz		r6,4(r6)					; Get the latest reference and change bits
			stw		r11,mmPTEent(r3)			; Clear the pointer to the PTE 
			rlwinm	r6,r6,0,23,24				; Extract the RC bits 
			lwz		r9,PCAallo(r7)				; Get the allocation control bits 
			rlwinm	r8,r5,24,8,15				; Make the autogen bit to turn off
			lwz		r10,mmphysent(r3)			; Get any physical entry
			or		r9,r9,r5					; Set the slot free 
			rlwimi	r8,r8,24,16,23				; Get lock bit mask to turn it off
			andc	r9,r9,r8					; Clear the auto and lock bits 
			mr.		r10,r10						; Is there a physical entry?
			li		r5,pepte1					; Get displacement to the second word of master pte
			stw		r9,PCAallo(r7)				; Store the allocation controls
			rlwimi	r2,r6,0,23,24				; Stick in RC bits
			beq-	htrnopte					; No physical entry...
			
htrmrc:		lwarx	r11,r5,r10					; Get the master copy
			or		r11,r11,r6					; Merge in latest RC
			stwcx.	r11,r5,r10					; Save it back
			bne-	htrmrc						; If it changed, try again... 
			
			sync								; Make sure that chain update is stored

htrnopte:	rlwinm	r3,r2,25,30,31				; Position RC and mask off
			bf		htrReset,htrnorst			; No reset to do...
			rlwinm	r2,r2,0,25,22				; Clear the RC if requested
			
htrnorst:	li		r4,0						; Get a 0 
			stw		r2,mmPTEr(r3)				; Set the real part of the PTE
			stw		r4,0(r7)					; Unlock the hash chain
	
			mtmsr	r0							; Restore interrupts and translation
			isync
			blr									; Return...

			.align	4

htrcset:	rlwinm	r3,r2,25,30,31				; Position RC and mask off
			mtmsr	r0							; Restore interrupts and translation
			isync
			blr									; Return...


/*
 *			hw_phys_attr(struct phys_entry *pp, vm_prot_t prot, unsigned int wimg) - Sets the default physical page attributes
 *
 *			Note that this must be done with both interruptions off and VM off
 *			Move the passed in attributes into the pte image in the phys entry
 *	
 *			  
 */

			.align	5
			.globl	EXT(hw_phys_attr)

LEXT(hw_phys_attr)

#if PERFTIMES && DEBUG
			mflr	r11
			mr		r8,r3
			mr		r7,r5
			mr		r5,r4
//			lwz		r4,4(r3)
			li		r4,0x1111
			li		r3,30
			bl		EXT(dbgLog2)						; Start of hw_add_map
			mr		r3,r8
			mr		r4,r5
			mr		r5,r7
			mtlr	r11
#endif
			mfsprg	r9,2						; Get feature flags 
			mfmsr	r0							/* Save the MSR  */
			andi.	r5,r5,0x0078				/* Clean up the WIMG */
			rlwinm	r12,r0,0,MSR_EE_BIT+1,MSR_EE_BIT-1	/* Clear interruptions */
			mtcrf	0x04,r9						; Set the features			
			rlwimi	r5,r4,0,30,31				/* Move the protection into the wimg register */
			la		r6,pepte1(r3)				/* Point to the default pte */
			rlwinm	r12,r12,0,28,25				/* Clear IR and DR */

			bt		pfNoMSRirb,hpaNoMSR			; No MSR...

			mtmsr	r12							; Translation and all off
			isync								; Toss prefetch
			b		hpaNoMSRx
			
hpaNoMSR:	
			mr		r10,r0
			mr		r4,r3
			li		r0,loadMSR					; Get the MSR setter SC
			mr		r3,r12						; Get new MSR
			sc									; Set it
			mr		r3,r4
			mr		r0,r10
hpaNoMSRx:

atmattr:	lwarx	r10,0,r6					/* Get the pte */
			rlwimi	r10,r5,0,25,31				/* Move in the new attributes */
			stwcx.	r10,0,r6					/* Try it on for size */
			bne-	atmattr						/* Someone else was trying, try again... */
		
			mtmsr	r0							/* Interrupts and translation back on */
			isync
#if PERFTIMES && DEBUG
			mflr	r11
			mr		r4,r10
			li		r3,31
			bl		EXT(dbgLog2)						; Start of hw_add_map
			mtlr	r11
#endif
			blr									/* All done... */



/*
 *			handlePF - handle a page fault interruption
 *
 *			If the fault can be handled, this routine will RFI directly,
 *			otherwise it will return with all registers as in entry.
 *
 *			Upon entry, state and all registers have been saved in savearea.
 *			This is pointed to by R13.
 *			IR and DR are off, interrupts are masked,
 *			Floating point be disabled.
 *			R3 is the interrupt code.
 *
 *			If we bail, we must restore cr5, and all registers except 6 and
 *			3.
 *
 */
	
			.align	5
			.globl	EXT(handlePF)

LEXT(handlePF)

/*
 *			This first part does a quick check to see if we can handle the fault.
 *			We can't handle any kind of protection exceptions here, so we pass
 *			them up to the next level.
 *
 *			The mapping lists are kept in MRS (most recently stolen)
 *			order on queues anchored within from the
 *			PTEG to which the virtual address hashes.  This is further segregated by
 *			the low-order 3 bits of the VSID XORed with the segment number and XORed
 *			with bits 4-7 of the vaddr in an attempt to keep the searches
 *			short.
 *			
 *			MRS is handled by moving the entry to the head of its list when stolen in the
 *			assumption that it will be revalidated soon.  Entries are created on the head 
 *			of the list because they will be used again almost immediately.
 *
 *			We need R13 set to the savearea, R3 set to the interrupt code, and R2
 *			set to the per_proc.
 *
 *			NOTE: In order for a page-fault redrive to work, the translation miss
 *			bit must be set in the DSISR (or SRR1 for IFETCH).  That must occur
 *			before we come here.
 */

			cmplwi	r3,T_INSTRUCTION_ACCESS		/* See if this is for the instruction */
			lwz		r8,savesrr1(r13)			; Get the MSR to determine mode
			beq-	gotIfetch					; We have an IFETCH here...
			
			lwz		r7,savedsisr(r13)			/* Get the DSISR */
			lwz		r6,savedar(r13)				/* Get the fault address */
			b		ckIfProt					; Go check if this is a protection fault...

gotIfetch:	mr		r7,r8						; IFETCH info is in SRR1
			lwz		r6,savesrr0(r13)			/* Get the instruction address */

ckIfProt:	rlwinm.	r7,r7,0,1,1					; Is this a protection exception?
			beqlr-								; Yes... (probably not though)

/*
 *			We will need to restore registers if we bail after this point.
 *			Note that at this point several SRs have been changed to the kernel versions.
 *			Therefore, for these we must build these values.
 */

#if PERFTIMES && DEBUG
			mflr	r11
			mr		r5,r6
			mr		r4,r3
			li		r3,32
			bl		EXT(dbgLog2)						; Start of hw_add_map
			mr		r3,r4
			mtlr	r11
			mfsprg	r2,0
#endif
			lwz		r3,PP_USERPMAP(r2)			; Get the user pmap (not needed if kernel access, but optimize for user??)
			rlwinm.	r8,r8,0,MSR_PR_BIT,MSR_PR_BIT	; Supervisor state access?
			rlwinm	r5,r6,6,26,29				; Get index to the segment slot
			eqv		r1,r1,r1					; Fill the bottom with foxes
			bne+	notsuper					; Go do the user mode interrupt stuff...
			
			cmplwi	cr1,r5,SR_COPYIN_NUM*4		; See if this is the copyin/copyout segment
			rlwinm	r3,r6,24,8,11				; Make the kernel VSID
			bne+	cr1,havevsid				; We are done if we do not want the copyin/out guy...
			
			mfsr	r3,SR_COPYIN				; Get the copy vsid
			b		havevsid					; Join up...

			.align	5

notsuper:	addi	r5,r5,PMAP_SEGS				; Get offset to table
			lwzx	r3,r3,r5					; Get the VSID

havevsid:	mfspr	r5,sdr1						/* Get hash table base and size */
			cror	cr1_eq,cr0_eq,cr0_eq		; Remember if kernel fault for later
			rlwinm	r9,r6,2,2,5					; Move nybble 1 up to 0 (keep aligned with VSID)
			rlwimi	r1,r5,16,0,15				/* Make table size -1 out of mask */
			rlwinm	r3,r3,6,2,25				/* Position the space for the VSID */
			rlwinm	r7,r6,26,10,25				/* Isolate the page index */
			xor		r9,r9,r3					; Splooch vaddr nybble 0 (from VSID) and 1 together
			or		r8,r5,r1					/* Point to the last byte in table */
			xor		r7,r7,r3					/* Get primary hash */
			rlwinm	r3,r3,1,1,24				/* Position VSID for pte ID */
			addi	r8,r8,1						/* Point to the PTEG Control Area */
			rlwinm	r9,r9,8,27,29				; Get splooched bits in place
			and		r7,r7,r1					/* Wrap the hash */
			rlwimi	r3,r6,10,26,31				/* Move API into pte ID */
			add		r8,r8,r7					/* Point to our PCA entry */
			rlwinm	r12,r3,27,27,29				; Get low 3 bits of the VSID for look-aside hash
			la		r11,PCAhash(r8)				/* Point to the mapping hash area */
			xor		r9,r9,r12					; Finish splooching nybble 0, 1, and the low bits of the VSID


/*
 *			We have about as much as we need to start searching the autogen (aka block maps)
 *			and mappings.  From here on, any kind of failure will bail, and
 *			contention will either bail or restart from here.
 *
 *			
 */
			
			li		r12,1						/* Get the locked value */
			dcbt	0,r11						/* We'll need the hash area in a sec, so get it */
			add		r11,r11,r9					/* Point to the right mapping hash slot */
			
			lwarx	r10,0,r8					; ?
ptegLck:	lwarx	r10,0,r8					/* Get the PTEG lock */
			mr.		r10,r10						/* Is it locked? */
			bne-	ptegLckw					/* Yeah... */
			stwcx.	r12,0,r8					/* Take take it */
			bne-	ptegLck						/* Someone else was trying, try again... */
			b		ptegSXg						/* All done... */
			
			.align	4

ptegLckw:	mr.		r10,r10						/* Check if it's already held */
			beq+	ptegLck						/* It's clear... */
			lwz		r10,0(r8)					/* Get lock word again... */
			b		ptegLckw					/* Wait... */
			
			.align	5
			
			nop									; Force ISYNC to last instruction in IFETCH
			nop									
			nop

ptegSXg:	isync								/* Make sure we haven't used anything yet */

			lwz		r9,0(r11)					/* Pick up first mapping block */
			mr		r5,r11						/* Get the address of the anchor */
			mr		r7,r9						/* Save the first in line */
			b		findmap						; Take space and force loop to cache line
		
findmap:	mr.		r12,r9						/* Are there more? */
			beq-	tryAuto						/* Nope, nothing in mapping list for us... */
			
			lwz		r10,mmPTEv(r12)				/* Get unique PTE identification */
			lwz		r9,mmhashnext(r12)			/* Get the chain, just in case */
			cmplw	r10,r3						/* Did we hit our PTE? */
 			lwz		r0,mmPTEent(r12)			/* Get the pointer to the hash table entry */
			mr		r5,r12						/* Save the current as previous */
			bne-	findmap						; Nothing here, try the next...

;			Cache line boundary here

			cmplwi	cr1,r0,0					/* Is there actually a PTE entry in the hash? */
			lwz		r2,mmphysent(r12)			/* Get the physical entry */
			bne-	cr1,MustBeOK				/* There's an entry in the hash table, so, this must 
												   have been taken care of already... */
			lis		r4,0x8000					; Tell PTE inserter that this was not an auto
			cmplwi	cr2,r2,0					/* Is there a physical entry? */
			li		r0,0x0100					/* Force on the reference bit whenever we make a PTE valid */
			bne+	cr2,gotphys					/* Skip down if we have a physical entry */
			li		r0,0x0180					/* When there is no physical entry, force on
												   both R and C bits to keep hardware from
												   updating the PTE to set them.  We don't
												   keep track of RC for I/O areas, so this is ok */
			
gotphys:	lwz		r2,mmPTEr(r12)				; Get the second part of the PTE
			b		insert						/* Go insert into the PTEG... */

MustBeOK:	li		r10,0						/* Get lock clear value */
			li		r3,T_IN_VAIN				/* Say that we handled it */
			stw		r10,PCAlock(r8)				/* Clear the PTEG lock */
			sync
#if PERFTIMES && DEBUG
			mflr	r11
			mr		r4,r3
			li		r3,33
			bl		EXT(dbgLog2)						; Start of hw_add_map
			mr		r3,r4
			mtlr	r11
#endif
			blr									/* Blow back and handle exception */


			
/*
 *			We couldn't find it in the mapping list.  As a last try, we will
 *			see if we can autogen it from the block mapped list.
 *	
 *			A block mapped area is defined as a contiguous virtual area that is mapped to 
 *			a contiguous physical area.  The olde-tyme IBM VM/XA Interpretive Execution
 *			architecture referred to this as a V=F, or Virtual = Fixed area. 
 *
 *			We consider a V=F area to be a single entity, adjacent areas can not be merged
 *			or overlapped.  The protection and memory attributes are the same and reference
 *			and change indications are not kept. The areas are not considered part of the
 *			physical RAM of the machine and do not have any associated physical table
 *			entries. Their primary use is intended for mapped I/O areas (e.g., framebuffers)
 *			although certain areas of RAM, such as the kernel V=R memory, can be mapped.
 *
 *			We also have a problem in the case of copyin/out: that access is done
 *			within the kernel for a user address. Unfortunately, the user isn't
 *			necessarily the current guy.  That means that we don't have access to the
 *			right autogen list. We can't support this kind of access. So, we need to do
 *			a quick check here and cause a fault if an attempt to copyin or out to
 *			any autogenned area.
 *
 *			The lists must be kept short.
 *
 *			NOTE:  kernel_pmap_store must be in V=R storage!!!!!!!!!!!!!!
 */
 
			.align	5

tryAuto:	rlwinm.	r11,r3,0,5,24				; Check if this is a kernel VSID
			lis		r10,HIGH_ADDR(EXT(kernel_pmap_store)+PMAP_BMAPS)	; Get the top part of kernel block map anchor
			crandc	cr0_eq,cr1_eq,cr0_eq		; Set if kernel access and non-zero VSID (copyin or copyout)
			mfsprg	r11,0						; Get the per_proc area
			beq-	cr0,realFault					; Can not autogen for copyin/copyout...
			ori		r10,r10,LOW_ADDR(EXT(kernel_pmap_store)+PMAP_BMAPS)	; Get the bottom part
			beq-	cr1,bmInKernel				; We are in kernel... (cr1 set way back at entry)
			
			lwz		r10,PP_USERPMAP(r11)		; Get the user pmap
			la		r10,PMAP_BMAPS(r10)			; Point to the chain anchor
			b		bmInKernel					; Jump over alignment gap...
			nop
			nop
			nop
			nop
			nop
			nop						
bmInKernel:
#ifndef CHIP_ERRATA_MAX_V1
			lwarx	r9,0,r10	
#endif /* CHIP_ERRATA_MAX_V1 */

bmapLck:	lwarx	r9,0,r10					; Get the block map anchor and lock
			rlwinm.	r5,r9,0,31,31				; Is it locked?
			ori		r5,r5,1						; Set the lock
			bne-	bmapLckw					; Yeah...
			stwcx.	r5,0,r10					; Lock the bmap list
			bne-	bmapLck						; Someone else was trying, try again...
			b		bmapSXg						; All done...
			
			.align	4

bmapLckw:	rlwinm.	r5,r9,0,31,31				; Check if it is still held
			beq+	bmapLck						; Not no more...
			lwz		r9,0(r10)					; Get lock word again...
			b		bmapLckw					; Check it out...
			
			.align	5
			
			nop									; Force ISYNC to last instruction in IFETCH
			nop									
			nop

bmapSXg:	rlwinm.	r4,r9,0,0,26				; Clear out flags and lock
			isync								; Make sure we have not used anything yet
			bne+	findAuto					; We have something, let us go...
			
bmapNone:	stw		r9,0(r10)					; Unlock it, we have nothing here
												; No sync here because we have not changed anything
			
/*
 *			When we come here, we know that we can't handle this.  Restore whatever
 *			state that we trashed and go back to continue handling the interrupt.
 */

realFault:	li		r10,0						/* Get lock clear value */
			lwz		r3,saveexception(r13)		/* Figure out the exception code again */
			stw		r10,PCAlock(r8)				/* Clear the PTEG lock */
#if PERFTIMES && DEBUG
			mflr	r11
			mr		r4,r3
			li		r3,33
			bl		EXT(dbgLog2)						; Start of hw_add_map
			mr		r3,r4
			mtlr	r11
#endif
			blr									/* Blow back and handle exception */
			
			.align	5
			
findAuto:	mr.		r4,r4						; Is there more?
			beq-	bmapNone					; No more...
			lwz		r5,bmstart(r4)				; Get the bottom of range
			lwz		r11,bmend(r4)				; Get the top of range
			cmplw	cr0,r6,r5					; Are we before the entry?
			cmplw	cr1,r6,r11					; Are we after the entry?
			cror	cr1_eq,cr0_lt,cr1_gt		; Set cr1_eq if new not in range
			bne+	cr1,faGot					; Found it...
			
			lwz		r4,bmnext(r4)				; Get the next one
			b		findAuto					; Check it out...
			
faGot:		rlwinm	r6,r6,0,0,19				; Round to page
			lwz		r2,bmPTEr(r4)				; Get the real part of the PTE
			sub		r5,r6,r5					; Get offset into area
			stw		r9,0(r10)					; Unlock it, we are done with it (no sync needed)
			add		r2,r2,r5					; Adjust the real address
			
			lis		r4,0x8080					/* Indicate that this was autogened */
			li		r0,0x0180					/* Autogenned areas always set RC bits.
												   This keeps the hardware from having
												   to do two storage writes */
			
/*
 *			Here where we insert the PTE into the hash.  The PTE image is in R3, R2. 
 *			The PTEG allocation controls are a bit map of the state of the PTEG. The
 *			PCAlock bits are a temporary lock for the specified PTE.  PCAfree indicates that
 *			the PTE slot is empty. PCAauto means that it comes from an autogen area.  These
 *			guys do not keep track of reference and change and are actually "wired".
 *			They're easy to maintain. PCAsteal
 *			is a sliding position mask used to "randomize" PTE slot stealing.  All 4 of these
 *			fields fit in a single word and are loaded and stored under control of the
 *			PTEG control area lock (PCAlock).
 *
 *			Note that PCAauto does not contribute to the steal calculations at all.  Originally
 *			it did, autogens were second in priority.  This can result in a pathalogical
 *			case where an instruction can not make forward progress, or one PTE slot
 *			thrashes.
 *
 *			Physically, the fields are arranged:
 *				0: PCAfree
 *				1: PCAauto
 *				2: PCAlock
 *				3: PCAsteal
 */
			
insert:		lwz		r10,PCAallo(r8)				/* Get the PTEG controls */
			eqv		r6,r6,r6					/* Get all ones */		
			mr		r11,r10						/* Make a copy */
			rlwimi	r6,r10,8,16,23				/* Insert sliding steal position */
			rlwimi	r11,r11,24,24,31			/* Duplicate the locked field */
			addi	r6,r6,-256					/* Form mask */
			rlwimi	r11,r11,16,0,15				/* This gives us a quadrupled lock mask */
			rlwinm	r5,r10,31,24,0				/* Slide over the mask for next time */
			mr		r9,r10						/* Make a copy to test */
			not		r11,r11						/* Invert the quadrupled lock */
			or		r2,r2,r0					/* Force on R, and maybe C bit */
			and		r9,r9,r11					/* Remove the locked guys */
			rlwimi	r5,r5,8,24,24				/* Wrap bottom bit to top in mask */
			rlwimi	r9,r11,0,16,31				/* Put two copies of the unlocked entries at the end */
			rlwinm	r6,r6,0,16,7				; Remove the autogens from the priority calculations
			rlwimi	r10,r5,0,24,31				/* Move steal map back in */
			and		r9,r9,r6					/* Set the starting point for stealing */

/*			So, now we have in R9:
				byte 0 = ~locked & free 
				byte 1 = 0 
				byte 2 = ~locked & (PCAsteal - 1)
				byte 3 = ~locked

				Each bit position represents (modulo 8) a PTE. If it is 1, it is available for 
				allocation at its priority level, left to right.  
				
			Additionally, the PCA steal field in R10 has been rotated right one bit.
*/
			

			rlwinm	r21,r10,8,0,7				; Isolate just the old autogen bits
			cntlzw	r6,r9						/* Allocate a slot */
			mr		r14,r12						/* Save our mapping for later */
			cmplwi	r6,32						; Was there anything available?
			rlwinm	r7,r6,29,30,31				/* Get the priority slot we got this from */
			rlwinm	r6,r6,0,29,31				; Isolate bit position
			srw		r11,r4,r6					/* Position the PTEG control bits */
			slw		r21,r21,r6					; Move corresponding old autogen flag to bit 0
			mr		r22,r11						; Get another copy of the selected slot
			
			beq-	realFault					/* Arghh, no slots! Take the long way 'round... */
			
												/* Remember, we've already set up the mask pattern
												   depending upon how we got here:
												     if got here from simple mapping, R4=0x80000000,
												     if we got here from autogen it is 0x80800000. */
			
			rlwinm	r6,r6,3,26,28				/* Start calculating actual PTE address */
			rlwimi	r22,r22,24,8,15				; Duplicate selected slot in second byte
			rlwinm.	r11,r11,0,8,15				/* Isolate just the auto bit (remember about it too) */
			andc	r10,r10,r22					/* Turn off the free and auto bits */
			add		r6,r8,r6					/* Get position into PTEG control area */
 			cmplwi	cr1,r7,1					/* Set the condition based upon the old PTE type */
			sub		r6,r6,r1					/* Switch it to the hash table */
			or		r10,r10,r11					/* Turn auto on if it is (PTEG control all set up now) */			
			subi	r6,r6,1						/* Point right */
			stw		r10,PCAallo(r8)				/* Allocate our slot */
			dcbt	br0,r6						; Touch in the PTE
			bne		wasauto						/* This was autogenned... */
			
			stw		r6,mmPTEent(r14)			/* Link the mapping to the PTE slot */
			
/*
 *			So, now we're here and what exactly do we have?  We've got: 
 *				1)	a full PTE entry, both top and bottom words in R3 and R2
 *				2)	an allocated slot in the PTEG.
 *				3)	R8 still points to the PTEG Control Area (PCA)
 *				4)	R6 points to the PTE entry.
 *				5)	R1 contains length of the hash table-1. We use this to back-translate
 *					a PTE to a virtual address so we can invalidate TLBs.
 *				6)	R11 has a copy of the PCA controls we set.
 *				7a)	R7 indicates what the PTE slot was before we got to it. 0 shows
 *					that it was empty and 2 or 3, that it was
 *					a we've stolen a live one. CR1 is set to LT for empty and GT
 *					otherwise.
 *				7b)	Bit 0 of R21 is 1 if the stolen PTE was autogenned
 *				8)	So far as our selected PTE, it should be valid if it was stolen
 *					and invalid if not.  We could put some kind of assert here to
 *					check, but I think that I'd rather leave it in as a mysterious,
 *					non-reproducable bug.
 *				9)	The new PTE's mapping has been moved to the front of its PTEG hash list
 *					so that it's kept in some semblance of a MRU list.
 *			   10)	R14 points to the mapping we're adding.
 *
 *			So, what do we have to do yet?
 *				1)	If we stole a slot, we need to invalidate the PTE completely.
 *				2)	If we stole one AND it was not an autogen, 
 *					copy the entire old PTE (including R and C bits) to its mapping.
 *				3)	Set the new PTE in the PTEG and make sure it is valid.
 *				4)	Unlock the PTEG control area.
 *				5)	Go back to the interrupt handler, changing the interrupt
 *					code to "in vain" which will restore the registers and bail out.
 *
 */
wasauto:	oris	r3,r3,0x8000				/* Turn on the valid bit */
			blt+	cr1,slamit					/* It was empty, go slam it on in... */
			
			lwz		r10,0(r6)					/* Grab the top part of the PTE */
			rlwinm	r12,r6,6,4,19				/* Match up the hash to a page boundary */
			rlwinm	r5,r10,5,4,19				/* Extract the VSID to a page boundary */
			rlwinm	r10,r10,0,1,31				/* Make it invalid */
			xor		r12,r5,r12					/* Calculate vaddr */
			stw		r10,0(r6)					/* Invalidate the PTE */
			rlwinm	r5,r10,7,27,29				; Move nybble 0 up to subhash position
			rlwimi	r12,r10,1,0,3				/* Move in the segment portion */
			lis		r9,HIGH_ADDR(EXT(tlb_system_lock))	/* Get the TLBIE lock */
			xor		r5,r5,r10					; Splooch nybble 0 and 1
			rlwimi	r12,r10,22,4,9				/* Move in the API */
			ori		r9,r9,LOW_ADDR(EXT(tlb_system_lock))	/* Grab up the bottom part */
			rlwinm	r4,r10,27,27,29				; Get low 3 bits of the VSID for look-aside hash
			
			sync								/* Make sure the invalid is stored */

			xor		r4,r4,r5					; Finish splooching nybble 0, 1, and the low bits of the VSID
						
			lwarx	r5,0,r9						; ?

tlbhang:	lwarx	r5,0,r9						/* Get the TLBIE lock */
		
			rlwinm	r4,r4,0,27,29				; Clean up splooched hash value

			mr.		r5,r5						/* Is it locked? */
			add		r4,r4,r8					/* Point to the offset into the PCA area */
			li		r5,1						/* Get our lock word */
			bne-	tlbhang						/* It's locked, go wait... */
			
			la		r4,PCAhash(r4)				/* Point to the start of the hash chain for the PTE we're replacing */
			
			stwcx.	r5,0,r9						/* Try to get it */
			bne-	tlbhang						/* We was beat... */
			
			mfspr	r7,pvr						/* Find out what kind of machine we are */
			li		r5,0						/* Lock clear value */
			rlwinm	r7,r7,16,16,31				/* Isolate CPU type */

			tlbie	r12							/* Invalidate it everywhere */

			cmplwi	r7,3						/* Is this a 603? */
			stw		r5,0(r9)					/* Clear the lock */
			
			beq-	its603						/* It's a 603, skip the tlbsync... */
			
			eieio								/* Make sure that the tlbie happens first */
			tlbsync								/* wait for everyone to catch up */
			isync								
			
its603:		rlwinm.	r21,r21,0,0,0				; See if we just stole an autogenned entry
			sync								/* Make sure of it all */

			bne		slamit						; The old was an autogen, time to slam the new in...
			
			lwz		r9,4(r6)					/* Get the real portion of old PTE */
			lwz		r7,0(r4)					/* Get the first element.  We can't get to here
												   if we aren't working with a mapping... */
			mr		r0,r7						; Save pointer to first element
												   
findold:	mr		r1,r11						; Save the previous guy
			mr.		r11,r7						/* Copy and test the chain */
			beq-	bebad						/* Assume it's not zero... */
			
			lwz		r5,mmPTEv(r11)				/* See if this is the old active one */
			cmplw	cr2,r11,r14					/* Check if this is actually the new one */
			cmplw	r5,r10						/* Is this us?  (Note: valid bit kept off in mappings) */
			lwz		r7,mmhashnext(r11)			/* Get the next one in line */
			beq-	cr2,findold					/* Don't count the new one... */
			cmplw	cr2,r11,r0					; Check if we are first on the list
			bne+	findold						/* Not it (and assume the worst)... */
			
			lwz		r12,mmphysent(r11)			/* Get the pointer to the physical entry */
			beq-	cr2,nomove					; We are first, no need to requeue...

			stw		r11,0(r4)					; Chain us to the head
			stw		r0,mmhashnext(r11)			; Chain the old head to us
			stw		r7,mmhashnext(r1)			; Unlink us

nomove:		li		r5,0						/* Clear this on out */
			
			mr.		r12,r12						/* Is there a physical entry? */
			stw		r5,mmPTEent(r11)			; Clear the PTE entry pointer
			li		r5,pepte1					/* Point to the PTE last half */
			stw		r9,mmPTEr(r11)				; Squirrel away the whole thing (RC bits are in here)
			
			beq-	mrgmrcx						; No physical entry for this one...
			
			rlwinm	r11,r9,0,23,24				/* Keep only the RC bits */

			lwarx	r9,r5,r12					; ?

mrgmrcx:	lwarx	r9,r5,r12					/* Get the master copy */
			or		r9,r9,r11					/* Merge in latest RC */
			stwcx.	r9,r5,r12					/* Save it back */
			bne-	mrgmrcx						/* If it changed, try again... */

/*
 *			Here's where we finish up.  We save the real part of the PTE, eieio it, to make sure it's
 *			out there before the top half (with the valid bit set).
 */

slamit:		stw		r2,4(r6)					/* Stash the real part */
			li		r4,0						/* Get a lock clear value */
			eieio								/* Erect a barricade */
			stw		r3,0(r6)					/* Stash the virtual part and set valid on */

			stw		r4,PCAlock(r8)				/* Clear the PCA lock */

			li		r3,T_IN_VAIN				/* Say that we handled it */
			sync								/* Go no further until the stores complete */
#if PERFTIMES && DEBUG
			mflr	r11
			mr		r4,r3
			li		r3,33
			bl		EXT(dbgLog2)						; Start of hw_add_map
			mr		r3,r4
			mtlr	r11
#endif
			blr									/* Back to the fold... */
					
bebad:		lis		r0,HIGH_ADDR(Choke)			/* We have a kernel choke!!! */
			ori		r0,r0,LOW_ADDR(Choke)				
			sc									/* Firmware Heimlich maneuver */
			
/*
 *			This walks the hash table or DBATs to locate the physical address of a virtual one.
 *			The space is provided.  If it is the kernel space, the DBATs are searched first.  Failing
 *			that, the hash table is accessed. Zero is returned for failure, so it must be special cased.
 *			This is usually used for debugging, so we try not to rely
 *			on anything that we don't have to.
 */

ENTRY(LRA, TAG_NO_FRAME_USED)

			mfsprg	r8,2						; Get feature flags 
			mfmsr	r10							/* Save the current MSR */
			mtcrf	0x04,r8						; Set the features			
			xoris	r5,r3,HIGH_ADDR(PPC_SID_KERNEL)		/* Clear the top half if equal */
			andi.	r9,r10,0x7FCF				/* Turn off interrupts and translation */
			eqv		r12,r12,r12					/* Fill the bottom with foxes */

			bt		pfNoMSRirb,lraNoMSR			; No MSR...

			mtmsr	r9							; Translation and all off
			isync								; Toss prefetch
			b		lraNoMSRx
			
lraNoMSR:	
			mr		r7,r3
			li		r0,loadMSR					; Get the MSR setter SC
			mr		r3,r9						; Get new MSR
			sc									; Set it
			mr		r3,r7
lraNoMSRx:

			cmplwi	r5,LOW_ADDR(PPC_SID_KERNEL)	/* See if this is kernel space */
			rlwinm	r11,r3,6,6,25				/* Position the space for the VSID */
			isync								/* Purge pipe */
			bne-	notkernsp					/* This is not for the kernel... */		
			
			mfspr	r5,dbat0u					/* Get the virtual address and length */
			eqv		r8,r8,r8					/* Get all foxes */
			rlwinm.	r0,r5,0,30,30				/* Check if valid for supervisor state */
			rlwinm	r7,r5,0,0,14				/* Clean up the base virtual address */
			beq-	ckbat1						/* not valid, skip this one... */
			sub		r7,r4,r7					/* Subtract out the base */
			rlwimi	r8,r5,15,0,14				/* Get area length - 1 */
			mfspr	r6,dbat0l					/* Get the real part */
			cmplw	r7,r8						/* Check if it is in the range */
			bng+	fndbat						/* Yup, she's a good un... */

ckbat1:		mfspr	r5,dbat1u					/* Get the virtual address and length */			
			eqv		r8,r8,r8					/* Get all foxes */
			rlwinm.	r0,r5,0,30,30				/* Check if valid for supervisor state */
			rlwinm	r7,r5,0,0,14				/* Clean up the base virtual address */
			beq-	ckbat2						/* not valid, skip this one... */
			sub		r7,r4,r7					/* Subtract out the base */
			rlwimi	r8,r5,15,0,14				/* Get area length - 1 */
			mfspr	r6,dbat1l					/* Get the real part */
			cmplw	r7,r8						/* Check if it is in the range */
			bng+	fndbat						/* Yup, she's a good un... */
			
ckbat2:		mfspr	r5,dbat2u					/* Get the virtual address and length */
			eqv		r8,r8,r8					/* Get all foxes */
			rlwinm.	r0,r5,0,30,30				/* Check if valid for supervisor state */
			rlwinm	r7,r5,0,0,14				/* Clean up the base virtual address */
			beq-	ckbat3						/* not valid, skip this one... */
			sub		r7,r4,r7					/* Subtract out the base */
			rlwimi	r8,r5,15,0,14				/* Get area length - 1 */
			mfspr	r6,dbat2l					/* Get the real part */
			cmplw	r7,r8						/* Check if it is in the range */
			bng-	fndbat						/* Yup, she's a good un... */
			
ckbat3:		mfspr	r5,dbat3u					/* Get the virtual address and length */
			eqv		r8,r8,r8					/* Get all foxes */
			rlwinm.	r0,r5,0,30,30				/* Check if valid for supervisor state */
			rlwinm	r7,r5,0,0,14				/* Clean up the base virtual address */
			beq-	notkernsp					/* not valid, skip this one... */
			sub		r7,r4,r7					/* Subtract out the base */
			rlwimi	r8,r5,15,0,14				/* Get area length - 1 */
			mfspr	r6,dbat3l					/* Get the real part */
			cmplw	r7,r8						/* Check if it is in the range */
			bgt+	notkernsp					/* No good... */
			
fndbat:		rlwinm	r6,r6,0,0,14				/* Clean up the real address */
			mtmsr	r10							/* Restore state */
			add		r3,r7,r6					/* Relocate the offset to real */
			isync								/* Purge pipe */
			blr									/* Bye, bye... */
notkernsp:	mfspr	r5,sdr1						/* Get hash table base and size */
			rlwimi	r11,r4,30,2,5				/* Insert the segment no. to make a VSID */
			rlwimi	r12,r5,16,0,15				/* Make table size -1 out of mask */
			rlwinm	r7,r4,26,10,25				/* Isolate the page index */
			andc	r5,r5,r12					/* Clean up the hash table */
			xor		r7,r7,r11					/* Get primary hash */
			rlwinm	r11,r11,1,1,24				/* Position VSID for pte ID */
			and		r7,r7,r12					/* Wrap the hash */
			rlwimi	r11,r4,10,26,31				/* Move API into pte ID */
			add		r5,r7,r5					/* Point to the PTEG */
			oris	r11,r11,0x8000				/* Slam on valid bit so's we don't match an invalid one */

			li		r9,8						/* Get the number of PTEs to check */
			lwz		r6,0(r5)					/* Preload the virtual half */
			
fndpte:		subi	r9,r9,1						/* Count the pte */
			lwz		r3,4(r5)					/* Get the real half */
			cmplw	cr1,r6,r11					/* Is this what we want? */
			lwz		r6,8(r5)					/* Start to get the next virtual half */
			mr.		r9,r9						/* Any more to try? */
			addi	r5,r5,8						/* Bump to next slot */
			beq		cr1,gotxlate				/* We found what we were looking for... */
			bne+	fndpte						/* Go try the next PTE... */
			
			mtmsr	r10							/* Restore state */
			li		r3,0						/* Show failure */
			isync								/* Purge pipe */
			blr									/* Leave... */

gotxlate:	mtmsr	r10							/* Restore state */
			rlwimi	r3,r4,0,20,31				/* Cram in the page displacement */
			isync								/* Purge pipe */
			blr									/* Return... */



/*
 *			struct blokmap *hw_add_blk(pmap_t pmap, struct blokmap *bmr)
 *	
 *			This is used to add a block mapping entry to the MRU list whose top
 *			node is anchored at bmaps.  This is a real address and is also used as
 *			the lock.
 *
 *			Overlapping areas are not allowed.  If we find one, we return it's address and
 *			expect the upper layers to panic.  We only check this for a debug build...
 *
 */

			.align	5
			.globl	EXT(hw_add_blk)

LEXT(hw_add_blk)

 			mfsprg	r9,2						; Get feature flags 
			lwz		r6,PMAP_PMAPVR(r3)			; Get the v to r translation
			mfmsr	r0							/* Save the MSR  */
			rlwinm	r12,r0,0,MSR_EE_BIT+1,MSR_EE_BIT-1	/* Clear interruptions */
			mtcrf	0x04,r9						; Set the features			
			xor		r3,r3,r6					; Get real address of bmap anchor
			rlwinm	r12,r12,0,28,25				/* Clear IR and DR */
			la		r3,PMAP_BMAPS(r3)			; Point to bmap header
			
			bt		pfNoMSRirb,habNoMSR			; No MSR...

			mtmsr	r12							; Translation and all off
			isync								; Toss prefetch
			b		habNoMSRx
			
habNoMSR:	
			mr		r9,r0
			mr		r8,r3
			li		r0,loadMSR					; Get the MSR setter SC
			mr		r3,r12						; Get new MSR
			sc									; Set it
			mr		r3,r8
			mr		r0,r9
habNoMSRx:
			
abLck:		lwarx	r9,0,r3						; Get the block map anchor and lock
			rlwinm.	r8,r9,0,31,31				; Is it locked?
			ori		r8,r9,1						; Set the lock
			bne-	abLckw						; Yeah...
			stwcx.	r8,0,r3						; Lock the bmap list
			bne-	abLck						; Someone else was trying, try again...
			b		abSXg						; All done...
			
			.align	4

abLckw:		rlwinm.	r5,r9,0,31,31				; Check if it is still held
			beq+	abLck						; Not no more...
			lwz		r9,0(r3)					; Get lock word again...
			b		abLckw						; Check it out...
			
			.align	5
			
			nop									; Force ISYNC to last instruction in IFETCH
			nop									

abSXg:		rlwinm	r11,r9,0,0,26				; Clear out flags and lock
			isync								; Make sure we have not used anything yet

;
;
;

			lwz		r7,bmstart(r4)				; Get start
			lwz		r8,bmend(r4)				; Get end		
			mr		r2,r11						; Get chain
	
abChk:		mr.		r10,r2						; End of chain?
			beq		abChkD						; Yes, chain is ok...
			lwz		r5,bmstart(r10)				; Get start of current area
			lwz		r6,bmend(r10)				; Get end of current area
			
			cmplw	cr0,r8,r5					; Is the end of the new before the old?
			cmplw	cr1,r8,r6					; Is the end of the new after the old?
			cmplw	cr6,r6,r7					; Is the end of the old before the new?
			cror	cr1_eq,cr0_lt,cr1_gt		; Set cr1_eq if new not in old
			cmplw	cr7,r6,r8					; Is the end of the old after the new?
			lwz		r2,bmnext(r10)				; Get pointer to the next
			cror	cr6_eq,cr6_lt,cr7_gt		; Set cr2_eq if old not in new
			crand	cr1_eq,cr1_eq,cr6_eq		; Set cr1_eq if no overlap
			beq+	cr1,abChk					; Ok check the next...
			
			stw		r9,0(r3)					; Unlock
			mtmsr	r0							; Restore xlation and rupts
			mr		r3,r10						; Pass back the overlap
			isync								;  
			blr									; Return...

abChkD:		stw		r11,bmnext(r4)				; Chain this on in
			rlwimi	r4,r9,0,27,31				; Copy in locks and flags
			sync								; Make sure that is done
			
			stw		r4,0(r3)					; Unlock and chain the new first one
			mtmsr	r0							; Restore xlation and rupts
			li		r3,0						; Pass back a no failure return code
			isync
			blr									; Return...


/*
 *			struct blokmap *hw_rem_blk(pmap_t pmap, vm_offset_t sva, vm_offset_t eva)
 *	
 *			This is used to remove a block mapping entry from the list that
 *			is anchored at bmaps.  bmaps is a virtual address and is also used as
 *			the lock.
 *
 *			Note that this function clears a single block that contains
 *			any address within the range sva to eva (inclusive).  To entirely
 *			clear any range, hw_rem_blk must be called repeatedly until it
 *			returns a 0.
 *
 *			The block is removed from the list and all hash table entries
 *			corresponding to the mapped block are invalidated and the TLB
 *			entries are purged.  If the block is large, this could take
 *			quite a while. We need to hash every possible address in the
 *			range and lock down the PCA.
 *
 *			If we attempt to remove a permanent entry, we will not do it.
 *			The block address will be ored with 1 and returned.
 *
 *
 */

			.align	5
			.globl	EXT(hw_rem_blk)

LEXT(hw_rem_blk)

  			mfsprg	r9,2						; Get feature flags
			lwz		r6,PMAP_PMAPVR(r3)			; Get the v to r translation
			mfmsr	r0							/* Save the MSR  */
			rlwinm	r12,r0,0,MSR_EE_BIT+1,MSR_EE_BIT-1	/* Clear interruptions */
			mtcrf	0x04,r9						; Set the features			
			xor		r3,r3,r6					; Get real address of bmap anchor
			rlwinm	r12,r12,0,28,25				/* Clear IR and DR */
			la		r3,PMAP_BMAPS(r3)			; Point to the bmap chain head

			bt		pfNoMSRirb,hrbNoMSR			; No MSR...

			mtmsr	r12							; Translation and all off
			isync								; Toss prefetch
			b		hrbNoMSRx
			
hrbNoMSR:	
			mr		r9,r0
			mr		r8,r3
			li		r0,loadMSR					; Get the MSR setter SC
			mr		r3,r12						; Get new MSR
			sc									; Set it
			mr		r3,r8
			mr		r0,r9
hrbNoMSRx:

rbLck:		lwarx	r9,0,r3						; Get the block map anchor and lock
			rlwinm.	r8,r9,0,31,31				; Is it locked?
			ori		r8,r9,1						; Set the lock
			bne-	rbLckw						; Yeah...
			stwcx.	r8,0,r3						; Lock the bmap list
			bne-	rbLck						; Someone else was trying, try again...
			b		rbSXg						; All done...
			
			.align	4

rbLckw:		rlwinm.	r11,r9,0,31,31				; Check if it is still held
			beq+	rbLck						; Not no more...
			lwz		r9,0(r3)					; Get lock word again...
			b		rbLckw						; Check it out...
			
			.align	5
			
			nop									; Force ISYNC to last instruction in IFETCH
			nop									

rbSXg:		rlwinm.	r2,r9,0,0,26				; Clear out flags and lock
			mr		r10,r3						; Keep anchor as previous pointer
			isync								; Make sure we have not used anything yet
			
			beq-	rbMT						; There is nothing in the list
			
rbChk:		mr		r12,r10						; Save the previous
			mr.		r10,r2						; End of chain?
			beq		rbMT						; Yes, nothing to do...
			lwz		r11,bmstart(r10)			; Get start of current area
			lwz		r6,bmend(r10)				; Get end of current area
			
			cmplw	cr0,r5,r11					; Is the end of range before the start of the area?
			cmplw	cr1,r4,r6					; Is the start of range after the end of the area?
			cror	cr1_eq,cr0_lt,cr1_gt		; Set cr1_eq if new not in range
			lwz		r2,bmnext(r10)				; Get the next one
			beq+	cr1,rbChk					; Not this one, check the next...
		
			lwz		r8,blkFlags(r10)			; Get the flags
			
			cmplw	cr1,r12,r3					; Did we delete the first one?
			rlwinm.	r8,r8,0,blkPermbit,blkPermbit	; is this a permanent block?
			bne		cr1,rbnFirst				; Nope...
			rlwimi	r9,r2,0,0,26				; Change the lock value
			ori		r2,r9,1						; Turn on the lock bit
			
rbnFirst:	bne-	rbPerm						; This is permanent, do not remove...
			lwz		r8,bmspace(r10)				; Get the VSID
			stw		r2,bmnext(r12)				; Unchain us
			
			eqv		r4,r4,r4					; Fill the bottom with foxes
			mfspr	r12,sdr1					; Get hash table base and size
			rlwinm	r8,r8,6,0,25				; Align VSID to PTEG
			rlwimi	r4,r12,16,0,15				; Make table size - 1 out of mask
			andc	r12,r12,r4					; Clean up address of hash table
			rlwinm	r5,r11,26,6,25				; Rotate virtual start address into PTEG units
			add		r12,r12,r4					; Point to PCA - 1
			rlwinm	r6,r6,26,6,25				; Rotate virtual end address into PTEG units
			addi	r12,r12,1					; Point to PCA base
			sub		r6,r6,r5					; Get the total number of PTEGs to clear
			cmplw	r6,r4						; See if this wraps all the way around
			blt		rbHash						; Nope, length is right
			subi	r6,r4,32+31					; Back down to correct length
			
rbHash:		rlwinm	r5,r5,0,10,25				; Keep only the page index
			xor		r2,r8,r5					; Hash into table
			and		r2,r2,r4					; Wrap into the table
			add		r2,r2,r12					; Point right at the PCA

rbLcka:		lwarx	r7,0,r2						; Get the PTEG lock
			mr.		r7,r7						; Is it locked?
			bne-	rbLckwa						; Yeah...
			li		r7,1						; Get the locked value
			stwcx.	r7,0,r2						; Take it
			bne-	rbLcka						; Someone else was trying, try again...
			b		rbSXga						; All done... 

rbLckwa:	mr.		r7,r7						; Check if it is already held
			beq+	rbLcka						; It is clear...
			lwz		r7,0(r2)					; Get lock word again...
			b		rbLckwa						; Wait...
			
rbSXga:		isync								; Make sure nothing used yet
			lwz		r7,PCAallo(r2)				; Get the allocation word
			rlwinm.	r11,r7,8,0,7				; Isolate the autogenerated PTEs
			or		r7,r7,r11					; Release the autogen slots
			beq+	rbAintNone					; There are not any here
			mtcrf	0xC0,r11					; Set the branch masks for autogens
			sub		r11,r2,r4					; Move back to the hash table + 1
			rlwinm	r7,r7,0,16,7				; Clear the autogen field
			subi	r11,r11,1					; Point to the PTEG
			stw		r7,PCAallo(r2)				; Update the flags
			li		r7,0						; Get an invalid PTE value

			bf		0,rbSlot1					; No autogen here
			stw		r7,0x00(r11)				; Invalidate PTE
rbSlot1:	bf		1,rbSlot2					; No autogen here
			stw		r7,0x08(r11)				; Invalidate PTE
rbSlot2:	bf		2,rbSlot3					; No autogen here
			stw		r7,0x10(r11)				; Invalidate PTE
rbSlot3:	bf		3,rbSlot4					; No autogen here
			stw		r7,0x18(r11)				; Invalidate PTE
rbSlot4:	bf		4,rbSlot5					; No autogen here
			stw		r7,0x20(r11)				; Invalidate PTE
rbSlot5:	bf		5,rbSlot6					; No autogen here
			stw		r7,0x28(r11)				; Invalidate PTE
rbSlot6:	bf		6,rbSlot7					; No autogen here
			stw		r7,0x30(r11)				; Invalidate PTE
rbSlot7:	bf		7,rbSlotx					; No autogen here
			stw		r7,0x38(r11)				; Invalidate PTE
rbSlotx:

rbAintNone:	li		r7,0						; Clear this out
			sync								; To make SMP happy
			addic.	r6,r6,-64					; Decrement the count
			stw		r7,PCAlock(r2)				; Release the PTEG lock
			addi	r5,r5,64					; Move up by adjusted page number
			bge+	rbHash						; Not done...
	
			sync								; Make sure the memory is quiet
			
;
;			Here we take the easy way out and just purge the entire TLB. This is 
;			certainly faster and definitly easier than blasting just the correct ones
;			in the range, we only need one lock and one TLBSYNC. We would hope
;			that most blocks are more than 64 pages (256K) and on every machine
;			up to Book E, 64 TLBIEs will invalidate the entire table.
;

			li		r5,64						; Get number of TLB entries to purge
			lis		r12,HIGH_ADDR(EXT(tlb_system_lock))	; Get the TLBIE lock
			li		r6,0						; Start at 0
			ori		r12,r12,LOW_ADDR(EXT(tlb_system_lock))	; Grab up the bottom part
						
rbTlbL:		lwarx	r2,0,r12					; Get the TLBIE lock
			mr.		r2,r2						; Is it locked?
			li		r2,1						; Get our lock value
			bne-	rbTlbL						; It is locked, go wait...
			stwcx.	r2,0,r12					; Try to get it
			bne-	rbTlbL						; We was beat...
	
rbTlbN:		addic.	r5,r5,-1					; See if we did them all
			tlbie	r6							; Invalidate it everywhere
			addi	r6,r6,0x1000				; Up to the next page
			bgt+	rbTlbN						; Make sure we have done it all...
			
			mfspr	r5,pvr						; Find out what kind of machine we are
			li		r2,0						; Lock clear value
			
			rlwinm	r5,r5,16,16,31				; Isolate CPU type
			cmplwi	r5,3						; Is this a 603?
			sync								; Make sure all is quiet
			beq-	rbits603a					; It is a 603, skip the tlbsync...
			
			eieio								; Make sure that the tlbie happens first
			tlbsync								; wait for everyone to catch up
			isync								

rbits603a:	sync								; Wait for quiet again
			stw		r2,0(r12)					; Unlock invalidates
			
			sync								; Make sure that is done
			
			stw		r9,0(r3)					; Unlock and chain the new first one
			mtmsr	r0							; Restore xlation and rupts
			mr		r3,r10						; Pass back the removed block
			isync
			blr									; Return...
			
rbMT:		stw		r9,0(r3)					; Unlock
			mtmsr	r0							; Restore xlation and rupts
			li		r3,0						; Say we did not find one
			isync
			blr									; Return...
			
rbPerm:		stw		r9,0(r3)					; Unlock
			mtmsr	r0							; Restore xlation and rupts
			ori		r3,r10,1					; Say we did not remove it
			isync
			blr									; Return...


/*
 *			vm_offset_t hw_cvp_blk(pmap_t pmap, vm_offset_t va)
 *	
 *			This is used to translate a virtual address within a block mapping entry
 *			to a physical address.  If not found, 0 is returned.
 *
 */

			.align	5
			.globl	EXT(hw_cvp_blk)

LEXT(hw_cvp_blk)

 			mfsprg	r9,2						; Get feature flags
 			lwz		r6,PMAP_PMAPVR(r3)			; Get the v to r translation
			mfmsr	r0							/* Save the MSR  */
			rlwinm	r12,r0,0,MSR_EE_BIT+1,MSR_EE_BIT-1	/* Clear interruptions */
			mtcrf	0x04,r9						; Set the features			
			xor		r3,r3,r6					; Get real address of bmap anchor
			rlwinm	r12,r12,0,28,25				/* Clear IR and DR */
			la		r3,PMAP_BMAPS(r3)			; Point to chain header

			bt		pfNoMSRirb,hcbNoMSR			; No MSR...

			mtmsr	r12							; Translation and all off
			isync								; Toss prefetch
			b		hcbNoMSRx
			
hcbNoMSR:	
			mr		r9,r0
			mr		r8,r3
			li		r0,loadMSR					; Get the MSR setter SC
			mr		r3,r12						; Get new MSR
			sc									; Set it
			mr		r3,r8
			mr		r0,r9
hcbNoMSRx:

cbLck:		lwarx	r9,0,r3						; Get the block map anchor and lock
			rlwinm.	r8,r9,0,31,31				; Is it locked?
			ori		r8,r9,1						; Set the lock
			bne-	cbLckw						; Yeah...
			stwcx.	r8,0,r3						; Lock the bmap list
			bne-	cbLck						; Someone else was trying, try again...
			b		cbSXg						; All done...
			
			.align	4

cbLckw:		rlwinm.	r5,r9,0,31,31				; Check if it is still held
			beq+	cbLck						; Not no more...
			lwz		r9,0(r3)					; Get lock word again...
			b		cbLckw						; Check it out...
			
			.align	5
			
			nop									; Force ISYNC to last instruction in IFETCH
			nop
			nop
			nop
			nop

cbSXg:		rlwinm.	r11,r9,0,0,26				; Clear out flags and lock
			li		r2,0						; Assume we do not find anything			
			isync								; Make sure we have not used anything yet

cbChk:		mr.		r11,r11						; Is there more?
			beq-	cbDone						; No more...
			lwz		r5,bmstart(r11)				; Get the bottom of range
			lwz		r12,bmend(r11)				; Get the top of range
			cmplw	cr0,r4,r5					; Are we before the entry?
			cmplw	cr1,r4,r12					; Are we after of the entry?
			cror	cr1_eq,cr0_lt,cr1_gt		; Set cr1_eq if new not in range
			beq-	cr1,cbNo					; We are not in the range...

			lwz		r2,bmPTEr(r11)				; Get the real part of the PTE
			sub		r5,r4,r5					; Get offset into area
			rlwinm	r2,r2,0,0,19				; Clean out everything but the page
			add		r2,r2,r5					; Adjust the real address

cbDone:		stw		r9,0(r3)					; Unlock it, we are done with it (no sync needed)
			mtmsr	r0							; Restore translation and interrupts...
			isync								; Make sure it is on
			mr		r3,r2						; Set return physical address
			blr									; Leave...
			
			.align	5
			
cbNo:		lwz		r11,bmnext(r11)				; Link next
			b		cbChk						; Check it out...
			
			
/*
 *			hw_set_user_space(pmap) 
 *			hw_set_user_space_dis(pmap) 
 *
 * 			Indicate whether memory space needs to be switched.
 *			We really need to turn off interrupts here, because we need to be non-preemptable
 *
 *			hw_set_user_space_dis is used when interruptions are already disabled. Mind the
 *			register usage here.   The VMM switch code in vmachmon.s that calls this
 *			know what registers are in use.  Check that if these change.
 */


	
			.align	5
			.globl	EXT(hw_set_user_space)

LEXT(hw_set_user_space)

			mfmsr	r10							/* Get the current MSR */
			rlwinm	r9,r10,0,MSR_EE_BIT+1,MSR_EE_BIT-1	/* Turn off 'rupts */
			mtmsr	r9							/* Disable 'em */
 			lwz		r7,PMAP_PMAPVR(r3)			; Get the v to r translation
			lwz		r4,PMAP_SPACE(r3)			; Get the space
			mfsprg	r6,0						/* Get the per_proc_info address */
			xor		r3,r3,r7					; Get real address of bmap anchor
			stw		r4,PP_USERSPACE(r6)			/* Show our new address space */
			stw		r3,PP_USERPMAP(r6)			; Show our real pmap address
			mtmsr	r10							/* Restore interruptions */
			blr									/* Return... */
	
			.align	5
			.globl	EXT(hw_set_user_space_dis)

LEXT(hw_set_user_space_dis)

 			lwz		r7,PMAP_PMAPVR(r3)			; Get the v to r translation
			lwz		r4,PMAP_SPACE(r3)			; Get the space
			mfsprg	r6,0						; Get the per_proc_info address
			xor		r3,r3,r7					; Get real address of bmap anchor
			stw		r4,PP_USERSPACE(r6)			; Show our new address space
			stw		r3,PP_USERPMAP(r6)			; Show our real pmap address
			blr									; Return...
	

/*			struct mapping *hw_cpv(struct mapping *mp) - Converts a physcial mapping CB address to virtual
 *
 */

			.align	5
			.globl	EXT(hw_cpv)

LEXT(hw_cpv)
			
			rlwinm.	r4,r3,0,0,19				; Round back to the mapping block allocation control block
			mfmsr	r10							; Get the current MSR
			beq-	hcpvret						; Skip if we are passed a 0...
			andi.	r9,r10,0x7FEF				; Turn off interrupts and data translation
			mtmsr	r9							; Disable DR and EE
			isync
			
			lwz		r4,mbvrswap(r4)				; Get the conversion value
			mtmsr	r10							; Interrupts and DR back on
			isync
			xor		r3,r3,r4					; Convert to physical

hcpvret:	rlwinm	r3,r3,0,0,26				; Clean out any flags
			blr


/*			struct mapping *hw_cvp(struct mapping *mp) - Converts a virtual mapping CB address to physcial
 *
 *			Translation must be on for this
 *
 */

			.align	5
			.globl	EXT(hw_cvp)

LEXT(hw_cvp)
			
			rlwinm	r4,r3,0,0,19				; Round back to the mapping block allocation control block			
			rlwinm	r3,r3,0,0,26				; Clean out any flags
			lwz		r4,mbvrswap(r4)				; Get the conversion value
			xor		r3,r3,r4					; Convert to virtual
			blr


/*			int mapalc(struct mappingblok *mb) - Finds, allocates, and checks a free mapping entry in a block
 *
 *			Lock must already be held on mapping block list
 *			returns 0 if all slots filled.
 *			returns n if a slot is found and it is not the last
 *			returns -n if a slot os found and it is the last
 *			when n and -n are returned, the corresponding bit is cleared
 *
 */

			.align	5
			.globl	EXT(mapalc)

LEXT(mapalc)
			
<<<<<<< HEAD
			lwz		r4,mbfree(r3)				; Get the first mask 
			lis		r0,0x8000					; Get the mask to clear the first free bit
			lwz		r5,mbfree+4(r3)				; Get the second mask 
			mr		r12,r3						; Save the return
			cntlzw	r8,r4						; Get first free field
			lwz		r6,mbfree+8(r3)				; Get the third mask 
			srw.	r9,r0,r8					; Get bit corresponding to first free one
			lwz		r7,mbfree+12(r3)			; Get the fourth mask 
			cntlzw	r10,r5						; Get first free field in second word
			andc	r4,r4,r9					; Turn it off
			bne		malcfnd0					; Found one...
=======
			sth		r5,14(r3)					; Store updated RC
			eieio								; Make sure we do not reorder
			std		r4,0(r3)					; Revalidate the PTE

			eieio								; Make sure all updates come first
			stw		r6,0(r7)					; Unlock PCA

htrNoOld64:	la		r3,pmapSXlk(r28)			; Point to the pmap search lock
			bl		sxlkUnlock					; Unlock the search list
			li		r3,mapRtOK					; Set normal return		
			b		htrR64						; Join common...

			.align	5							
						
htrReturn:	bt++	pf64Bitb,htrR64				; Yes...

htrR32:		mtmsr	r27							; Restore enables/translation/etc.
			isync
			b		htrReturnC					; Join common...

htrR64:		mtmsrd	r27							; Restore enables/translation/etc.
			isync								
			
htrReturnC:	lwz		r0,(FM_ALIGN((31-24+1)*4)+FM_SIZE+FM_LR_SAVE)(r1)	; Save the return
			or		r3,r3,r25					; Send the RC bits back
			lwz		r24,FM_ARG0+0x00(r1)		; Save a register
			lwz		r25,FM_ARG0+0x04(r1)		; Save a register
			lwz		r26,FM_ARG0+0x08(r1)		; Save a register
			mtlr	r0							; Restore the return
			lwz		r27,FM_ARG0+0x0C(r1)		; Save a register
			lwz		r28,FM_ARG0+0x10(r1)		; Save a register
			lwz		r29,FM_ARG0+0x14(r1)		; Save a register
			lwz		r30,FM_ARG0+0x18(r1)		; Save a register
			lwz		r31,FM_ARG0+0x1C(r1)		; Save a register
			lwz		r1,0(r1)					; Pop the stack
			blr									; Leave...
			
			.align	5
			
htrBadLock:	li		r3,mapRtBadLk				; Set lock time out error code
			b		htrReturn					; Leave....
			
htrNotFound:	
			la		r3,pmapSXlk(r28)			; Point to the pmap search lock
			bl		sxlkUnlock					; Unlock the search list
			
			li		r3,mapRtNotFnd				; Set that we did not find the requested page
			b		htrReturn					; Leave....



;
;			mapPhysFindLock - find physent list and lock it
;			R31 points to mapping
;
			.align	5
			
mapPhysFindLock:	
			lbz		r4,mpFlags+1(r31)			; Get the index into the physent bank table
			lis		r3,ha16(EXT(pmap_mem_regions))	; Get high order of physent table (note use of ha16 to get value appropriate for an addi of low part)
			rlwinm	r4,r4,2,0,29				; Change index into byte offset
			addi	r4,r4,lo16(EXT(pmap_mem_regions))	; Get low part of address of entry
			add		r3,r3,r4					; Point to table entry
			lwz		r5,mpPAddr(r31)				; Get physical page number
			lwz		r7,mrStart(r3)				; Get the start of range
			lwz		r3,mrPhysTab(r3)			; Get the start of the entries for this bank
			sub		r6,r5,r7					; Get index to physent
			rlwinm	r6,r6,3,0,28				; Get offset to physent
			add		r3,r3,r6					; Point right to the physent
			b		mapPhysLock					; Join in the lock...

;
;			mapPhysLock - lock a physent list
;			R3 contains list header
;
			.align	5

mapPhysLockS:
			li		r2,lgKillResv				; Get a spot to kill reservation
			stwcx.	r2,0,r2						; Kill it...
			
mapPhysLockT:
			lwz		r2,ppLink(r3)				; Get physent chain header
			rlwinm.	r2,r2,0,0,0					; Is lock clear?
			bne--	mapPhysLockT				; Nope, still locked...
			
mapPhysLock:	
			lwarx	r2,0,r3						; Get the lock
			rlwinm.	r0,r2,0,0,0					; Is it locked?
			oris	r0,r2,0x8000				; Set the lock bit
			bne--	mapPhysLockS				; It is locked, spin on it...
			stwcx.	r0,0,r3						; Try to stuff it back...
			bne--	mapPhysLock					; Collision, try again...
			isync								; Clear any speculations
			blr									; Leave...
			

;
;			mapPhysUnlock - unlock a physent list
;			R3 contains list header
;
			.align	5
			
mapPhysUnlock:	
			lwz		r0,ppLink(r3)				; Get physent chain header
			rlwinm	r0,r0,0,1,31				; Clear the lock bit
			eieio								; Make sure unlock comes last
			stw		r0,ppLink(r3)				; Unlock the list
			blr

;
;			mapPhysMerge - merge the RC bits into the master copy
;			R3 points to the physent 
;			R4 contains the RC bits
;
;			Note: we just return if RC is 0
;
			.align	5
			
mapPhysMerge:	
			rlwinm.	r4,r4,PTE1_REFERENCED_BIT+(64-ppRb),ppRb-32,ppCb-32	; Isolate RC bits
			la		r5,ppLink+4(r3)				; Point to the RC field
			beqlr--								; Leave if RC is 0...
			
mapPhysMergeT:
			lwarx	r6,0,r5						; Get the RC part
			or		r6,r6,r4					; Merge in the RC
			stwcx.	r6,0,r5						; Try to stuff it back...
			bne--	mapPhysMergeT				; Collision, try again...
			blr									; Leave...

;
;			Sets the physent link pointer and preserves all flags
;			The list is locked
;			R3 points to physent
;			R4 has link to set
;

			.align	5

mapPhyCSet32:
			la		r5,ppLink+4(r3)				; Point to the link word

mapPhyCSetR:
			lwarx	r2,0,r5						; Get the link and flags
			rlwimi	r4,r2,0,26,31				; Insert the flags
			stwcx.	r4,0,r5						; Stick them back
			bne--	mapPhyCSetR					; Someone else did something, try again...
			blr									; Return...

			.align	5

mapPhyCSet64:
			li		r0,0xFF						; Get mask to clean up mapping pointer
			rldicl	r0,r0,62,0					; Rotate clean up mask to get 0xC0000000000000003F
		
mapPhyCSet64x:
			ldarx	r2,0,r3						; Get the link and flags
			and		r5,r2,r0					; Isolate the flags
			or		r6,r4,r5					; Add them to the link
			stdcx.	r6,0,r3						; Stick them back
			bne--	mapPhyCSet64x				; Someone else did something, try again...
			blr									; Return...						

;
;			mapBumpBusy - increment the busy count on a mapping
;			R3 points to mapping
;

			.align	5

mapBumpBusy:
			lwarx	r4,0,r3						; Get mpBusy
			addis	r4,r4,0x0100				; Bump the busy count
			stwcx.	r4,0,r3						; Save it back
			bne--	mapBumpBusy					; This did not work, try again...
			blr									; Leave...

;
;			mapDropBusy - increment the busy count on a mapping
;			R3 points to mapping
;

			.globl	EXT(mapping_drop_busy)
			.align	5

LEXT(mapping_drop_busy)
mapDropBusy:
			lwarx	r4,0,r3						; Get mpBusy
			addis	r4,r4,0xFF00				; Drop the busy count
			stwcx.	r4,0,r3						; Save it back
			bne--	mapDropBusy					; This did not work, try again...
			blr									; Leave...

;
;			mapDrainBusy - drain the busy count on a mapping
;			R3 points to mapping
;			Note: we already have a busy for ourselves. Only one
;			busy per processor is allowed, so we just spin here
;			waiting for the count to drop to 1.
;			Also, the mapping can not be on any lists when we do this
;			so all we are doing is waiting until it can be released.
;

			.align	5

mapDrainBusy:
			lwz		r4,mpFlags(r3)				; Get mpBusy
			rlwinm	r4,r4,8,24,31				; Clean it up
			cmplwi	r4,1						; Is is just our busy?
			beqlr++								; Yeah, it is clear...
			b		mapDrainBusy				; Try again...


	
;
;			handleDSeg - handle a data segment fault
;			handleISeg - handle an instruction segment fault
;
;			All that we do here is to map these to DSI or ISI and insure
;			that the hash bit is not set.  This forces the fault code
;			to also handle the missing segment.
;
;			At entry R2 contains per_proc, R13 contains savarea pointer,
;			and R11 is the exception code.
;

			.align	5
			.globl	EXT(handleDSeg)

LEXT(handleDSeg)

			li		r11,T_DATA_ACCESS			; Change fault to DSI
			stw		r11,saveexception(r13)		; Change the exception code from seg fault to PTE miss
			b		EXT(handlePF)				; Join common...

			.align	5
			.globl	EXT(handleISeg)

LEXT(handleISeg)

			li		r11,T_INSTRUCTION_ACCESS	; Change fault to ISI
			stw		r11,saveexception(r13)		; Change the exception code from seg fault to PTE miss
			b		EXT(handlePF)				; Join common...


/*
 *			handlePF - handle a page fault interruption
 *
 *			At entry R2 contains per_proc, R13 contains savarea pointer,
 *			and R11 is the exception code.
 *
 *			This first part does a quick check to see if we can handle the fault.
 *			We canot handle any kind of protection exceptions here, so we pass
 *			them up to the next level.
 *
 *			NOTE: In order for a page-fault redrive to work, the translation miss
 *			bit must be set in the DSISR (or SRR1 for IFETCH).  That must occur
 *			before we come here.
 */

			.align	5
			.globl	EXT(handlePF)

LEXT(handlePF)

 			mfsprg	r12,2						; Get feature flags 
			cmplwi	r11,T_INSTRUCTION_ACCESS		; See if this is for the instruction 
			lwz		r8,savesrr1+4(r13)			; Get the MSR to determine mode
			mtcrf	0x02,r12					; move pf64Bit to cr6
			lis		r0,hi16(dsiNoEx|dsiProt|dsiInvMode|dsiAC)	; Get the types that we cannot handle here
			lwz		r18,SAVflags(r13)			; Get the flags
			
			beq--	gotIfetch					; We have an IFETCH here...
			
			lwz		r27,savedsisr(r13)			; Get the DSISR
			lwz		r29,savedar(r13)			; Get the first half of the DAR
			lwz		r30,savedar+4(r13)			; And second half

			b		ckIfProt					; Go check if this is a protection fault...

gotIfetch:	andis.	r27,r8,hi16(dsiValid)		; Clean this up to construct a DSISR value
			lwz		r29,savesrr0(r13)			; Get the first half of the instruction address
			lwz		r30,savesrr0+4(r13)			; And second half
			stw		r27,savedsisr(r13)			; Save the "constructed" DSISR

ckIfProt:	and.	r4,r27,r0					; Is this a non-handlable exception?
			li		r20,64						; Set a limit of 64 nests for sanity check
			bne--	hpfExit						; Yes... (probably not though)

;
;			Note: if the RI is on, we are accessing user space from the kernel, therefore we
;			should be loading the user pmap here.
;

			andi.	r0,r8,lo16(MASK(MSR_PR)|MASK(MSR_RI))	; Are we addressing user or kernel space?
			lis		r8,hi16(EXT(kernel_pmap_phys))	; Assume kernel
			mr		r19,r2						; Remember the per_proc
			ori		r8,r8,lo16(EXT(kernel_pmap_phys))	; Assume kernel (bottom of address)
			mr		r23,r30						; Save the low part of faulting address
			beq--	hpfInKern					; Skip if we are in the kernel
			la		r8,ppUserPmap(r19)			; Point to the current user pmap
			
hpfInKern:	mr		r22,r29						; Save the high part of faulting address
			
			bt--	pf64Bitb,hpf64a				; If 64-bit, skip the next bit...

;
;			On 32-bit machines we emulate a segment exception by loading unused SRs with a
;			predefined value that corresponds to no address space.  When we see that value
;			we turn off the PTE miss bit in the DSISR to drive the code later on that will
;			cause the proper SR to be loaded.
;

			lwz		r28,4(r8)					; Pick up the pmap
			rlwinm.	r18,r18,0,SAVredriveb,SAVredriveb	; Was this a redrive?
			mr		r25,r28						; Save the original pmap (in case we nest)
			bne		hpfNest						; Segs are not ours if so...
			mfsrin	r4,r30						; Get the SR that was used for translation
			cmplwi	r4,invalSpace				; Is this a simulated segment fault?
			bne++	hpfNest						; No...
			
			rlwinm	r27,r27,0,dsiMissb+1,dsiMissb-1	; Clear the PTE miss bit in DSISR
			b		hpfNest						; Join on up...
			
			.align	5

			nop									; Push hpfNest to a 32-byte boundary
			nop									; Push hpfNest to a 32-byte boundary
			nop									; Push hpfNest to a 32-byte boundary
			nop									; Push hpfNest to a 32-byte boundary
			nop									; Push hpfNest to a 32-byte boundary
			nop									; Push hpfNest to a 32-byte boundary

hpf64a:		ld		r28,0(r8)					; Get the pmap pointer (64-bit)
			mr		r25,r28						; Save the original pmap (in case we nest)

;
;			This is where we loop descending nested pmaps
;

hpfNest:	la		r3,pmapSXlk(r28)			; Point to the pmap search lock
			addi	r20,r20,-1					; Count nest try
			bl		sxlkShared					; Go get a shared lock on the mapping lists
			mr.		r3,r3						; Did we get the lock?
			bne--	hpfBadLock					; Nope...

			mr		r3,r28						; Get the pmap pointer
			mr		r4,r22						; Get top of faulting vaddr
			mr		r5,r23						; Get bottom of faulting vaddr
			bl		EXT(mapSearch)				; Go see if we can find it (R7 gets mpFlags)

			rlwinm	r0,r7,0,mpRIPb,mpRIPb		; Are we removing this one?
			mr.		r31,r3						; Save the mapping if we found it
			cmplwi	cr1,r0,0					; Check for removal
			crorc	cr0_eq,cr0_eq,cr1_eq		; Merge not found and removing
			
			bt--	cr0_eq,hpfNotFound			; Not found or removing...
			
			rlwinm.	r0,r7,0,mpNestb,mpNestb		; Are we nested?
			mr		r26,r7						; Get the flags for this mapping (passed back from search call)
			
			lhz		r21,mpSpace(r31)			; Get the space

			beq++	hpfFoundIt					; No, we found our guy...
			

#if pmapTransSize != 12
#error pmapTrans entry size is not 12 bytes!!!!!!!!!!!! It is pmapTransSize
#endif
			rlwinm.	r0,r26,0,mpSpecialb,mpSpecialb	; Special handling?
			cmplwi	cr1,r20,0					; Too many nestings?
			bne--	hpfSpclNest					; Do we need to do special handling?

hpfCSrch:	lhz		r21,mpSpace(r31)			; Get the space
			lwz		r8,mpNestReloc(r31)			; Get the vaddr relocation
			lwz		r9,mpNestReloc+4(r31)		; Get the vaddr relocation bottom half
			la		r3,pmapSXlk(r28)			; Point to the old pmap search lock
			lis		r0,0x8000					; Get 0xFFFFFFFF80000000
			lis		r10,hi16(EXT(pmapTrans))	; Get the translate table
			add		r0,r0,r0					; Get 0xFFFFFFFF00000000 for 64-bit or 0 for 32-bit
			blt--	cr1,hpfNestTooMuch			; Too many nestings, must be a loop...
			or		r23,r23,r0					; Make sure a carry will propagate all the way in 64-bit
			slwi	r11,r21,3					; Multiply space by 8
			ori		r10,r10,lo16(EXT(pmapTrans))	; Get the translate table low part
			addc	r23,r23,r9					; Relocate bottom half of vaddr
			lwz		r10,0(r10)					; Get the actual translation map
			slwi	r12,r21,2					; Multiply space by 4
			add		r10,r10,r11					; Add in the higher part of the index
			rlwinm	r23,r23,0,0,31				; Clean up the relocated address (does nothing in 32-bit)
			adde	r22,r22,r8					; Relocate the top half of the vaddr
			add		r12,r12,r10					; Now we are pointing at the space to pmap translation entry
			bl		sxlkUnlock					; Unlock the search list
			
			lwz		r28,pmapPAddr+4(r12)		; Get the physical address of the new pmap
			bf--	pf64Bitb,hpfNest			; Done if 32-bit...
			
			ld		r28,pmapPAddr(r12)			; Get the physical address of the new pmap
			b		hpfNest						; Go try the new pmap...

;
;			Error condition.  We only allow 64 nestings.  This keeps us from having to 
;			check for recusive nests when we install them.
;
		
			.align	5

hpfNestTooMuch:
			lwz		r20,savedsisr(r13)			; Get the DSISR
			la		r3,pmapSXlk(r28)			; Point to the pmap search lock
			bl		sxlkUnlock					; Unlock the search list (R3 good from above)
			ori		r20,r20,1					; Indicate that there was a nesting problem 
			stw		r20,savedsisr(r13)			; Stash it
			lwz		r11,saveexception(r13)		; Restore the exception code
			b		EXT(PFSExit)				; Yes... (probably not though)

;
;			Error condition - lock failed - this is fatal
;
		
			.align	5

hpfBadLock:
			lis		r0,hi16(Choke)				; System abend
			ori		r0,r0,lo16(Choke)			; System abend
			li		r3,failMapping				; Show mapping failure
			sc
;
;			Did not find any kind of mapping
;

			.align	5
			
hpfNotFound:
			la		r3,pmapSXlk(r28)			; Point to the pmap search lock
			bl		sxlkUnlock					; Unlock it
			lwz		r11,saveexception(r13)		; Restore the exception code
			
hpfExit:										; We need this because we can not do a relative branch
			b		EXT(PFSExit)				; Yes... (probably not though)


;
;			Here is where we handle special mappings.  So far, the only use is to load a 
;			processor specific segment register for copy in/out handling.
;
;			The only (so far implemented) special map is used for copyin/copyout.
;			We keep a mapping of a "linkage" mapping in the per_proc.
;			The linkage mapping is basically a nested pmap that is switched in
;			as part of context switch.  It relocates the appropriate user address
;			space slice into the right place in the kernel.
;

			.align	5

hpfSpclNest:	
			la		r31,ppCIOmp(r19)			; Just point to the mapping
			oris	r27,r27,hi16(dsiSpcNest)	; Show that we had a special nesting here
			b		hpfCSrch					; Go continue search...


;
;			We have now found a mapping for the address we faulted on. 
;			

;
;			Here we go about calculating what the VSID should be. We concatanate
;			the space ID (14 bits wide) 3 times.  We then slide the vaddr over
;			so that bits 0:35 are in 14:49 (leaves a hole for one copy of the space ID).
;			Then we XOR and expanded space ID and the shifted vaddr.  This gives us
;			the VSID.  
;
;			This is used both for segment handling and PTE handling
;


#if maxAdrSpb != 14
#error maxAdrSpb (address space id size) is not 14 bits!!!!!!!!!!!!
#endif

			.align	5
			
hpfFoundIt:	lwz		r12,pmapFlags(r28)			; Get the pmap flags so we can find the keys for this segment
			rlwinm.	r0,r27,0,dsiMissb,dsiMissb	; Did we actually miss the segment?
			rlwinm	r15,r23,18,14,17			; Shift 32:35 (0:3) of vaddr just above space ID
			rlwinm	r20,r21,28,22,31			; Shift upper 10 bits of space into high order
			rlwinm	r14,r22,18,14,31			; Shift 0:17 of vaddr over
			rlwinm	r0,r27,0,dsiSpcNestb,dsiSpcNestb	; Isolate special nest flag
			rlwimi	r21,r21,14,4,17				; Make a second copy of space above first
			cmplwi	cr5,r0,0					; Did we just do a special nesting?
			rlwimi	r15,r22,18,0,13				; Shift 18:31 of vaddr just above shifted 32:35	
			crorc	cr0_eq,cr0_eq,cr5_eq		; Force outselves through the seg load code if special nest
			rlwimi	r21,r21,28,0,3				; Get low order of 3rd copy of space at top of register
			xor		r14,r14,r20					; Calculate the top half of VSID
			xor		r15,r15,r21					; Calculate the bottom half of the VSID
			rlwinm	r14,r14,12,15,19			; Slide the top of the VSID over to correct position (trim for 65 bit addressing)
			rlwinm	r12,r12,9,20,22				; Isolate and position key for cache entry
			rlwimi	r14,r15,12,20,31			; Slide top of bottom of VSID over into the top
			rlwinm	r15,r15,12,0,19				; Slide the last nybble into the low order segment position
			or		r12,r12,r15					; Add key into the bottom of VSID
;
;			Note: ESID is in R22:R23 pair; VSID is in R14:R15; cache form VSID is R14:R12
			
			bne++	hpfPteMiss					; Nope, normal PTE miss...

;
;			Here is the only place that we make an entry in the pmap segment cache.
;
;			Note that we do not make an entry in the segment cache for special
;			nested mappings.  This makes the copy in/out segment get refreshed
;			when switching threads.
;
;			The first thing that we do is to look up the ESID we are going to load
;			into a segment in the pmap cache.  If it is already there, this is
;			a segment that appeared since the last time we switched address spaces.
;			If all is correct, then it was another processors that made the cache
;			entry.  If not, well, it is an error that we should die on, but I have
;			not figured a good way to trap it yet.
;
;			If we get a hit, we just bail, otherwise, lock the pmap cache, select
;			an entry based on the generation number, update the cache entry, and 
;			also update the pmap sub-tag as well.  The sub-tag is a table of 4 bit
;			entries that correspond to the last 4 bits (32:35 for 64-bit and 
;			0:3 for 32-bit) of the ESID.
;
;			Then we unlock and bail.
;
;			First lock it.  Then select a free slot or steal one based on the generation
;			number. Then store it, update the allocation flags, and unlock.
;
;			The cache entry contains an image of the ESID/VSID pair we would load for
;			64-bit architecture.  For 32-bit, it is a simple transform to an SR image.
;
;			Remember, this cache entry goes in the ORIGINAL pmap (saved in R25), not
;			the current one, which may have changed because we nested.
;
;			Also remember that we do not store the valid bit in the ESID.  If we 
;			od, this will break some other stuff.
;

			bne--	cr5,hpfNoCacheEnt2			; Skip the cache entry if this is a "special nest" fault....
			
			mr		r3,r25						; Point to the pmap
			mr		r4,r22						; ESID high half
			mr		r5,r23						; ESID low half
			bl		pmapCacheLookup				; Go see if this is in the cache already
			
			mr.		r3,r3						; Did we find it?
			mr		r4,r11						; Copy this to a different register

			bne--	hpfNoCacheEnt				; Yes, we found it, no need to make another entry...
			
			lwz		r10,pmapSCSubTag(r25)		; Get the first part of the sub-tag lookup table
			lwz		r11,pmapSCSubTag+4(r25)		; Get the second part of the sub-tag lookup table
			
			cntlzw	r7,r4						; Find a free slot

			subi	r6,r7,pmapSegCacheUse		; We end up with a negative if we find one
			rlwinm	r30,r30,0,0,3				; Clean up the ESID
			srawi	r6,r6,31					; Get 0xFFFFFFFF if we have one, 0 if not
			addi	r5,r4,1						; Bump the generation number
			and		r7,r7,r6					; Clear bit number if none empty
			andc	r8,r4,r6					; Clear generation count if we found an empty
			rlwimi	r4,r5,0,17,31				; Insert the new generation number into the control word			
			or		r7,r7,r8					; Select a slot number
			li		r8,0						; Clear
			andi.	r7,r7,pmapSegCacheUse-1		; Wrap into the number we are using
			oris	r8,r8,0x8000				; Get the high bit on
			la		r9,pmapSegCache(r25)		; Point to the segment cache
			slwi	r6,r7,4						; Get index into the segment cache
			slwi	r2,r7,2						; Get index into the segment cache sub-tag index
			srw		r8,r8,r7					; Get the mask
			cmplwi	r2,32						; See if we are in the first or second half of sub-tag
			li		r0,0						; Clear
			rlwinm	r2,r2,0,27,31				; Wrap shift so we do not shift cache entries 8-F out
			oris	r0,r0,0xF000				; Get the sub-tag mask
			add		r9,r9,r6					; Point to the cache slot
			srw		r0,r0,r2					; Slide sub-tag mask to right slot (shift work for either half)
			srw		r5,r30,r2					; Slide sub-tag to right slot (shift work for either half)
			
			stw		r29,sgcESID(r9)				; Save the top of the ESID
			andc	r10,r10,r0					; Clear sub-tag slot in case we are in top
			andc	r11,r11,r0					; Clear sub-tag slot in case we are in bottom
			stw		r30,sgcESID+4(r9)			; Save the bottom of the ESID
			or		r10,r10,r5					; Stick in subtag in case top half
			or		r11,r11,r5					; Stick in subtag in case bottom half
			stw		r14,sgcVSID(r9)				; Save the top of the VSID
			andc	r4,r4,r8					; Clear the invalid bit for the slot we just allocated
			stw		r12,sgcVSID+4(r9)			; Save the bottom of the VSID and the key
			bge		hpfSCSTbottom				; Go save the bottom part of sub-tag
			
			stw		r10,pmapSCSubTag(r25)		; Save the top of the sub-tag
			b		hpfNoCacheEnt				; Go finish up...
			
hpfSCSTbottom:
			stw		r11,pmapSCSubTag+4(r25)		; Save the bottom of the sub-tag


hpfNoCacheEnt:	
			eieio								; Make sure cache is updated before lock
			stw		r4,pmapCCtl(r25)			; Unlock, allocate, and bump generation number


hpfNoCacheEnt2:
			lwz		r4,ppMapFlags(r19)			; Get the protection key modifier
			bt++	pf64Bitb,hpfLoadSeg64		; If 64-bit, go load the segment...
						
;
;			Make and enter 32-bit segment register
;

			lwz		r16,validSegs(r19)			; Get the valid SR flags
			xor		r12,r12,r4					; Alter the storage key before loading segment register
			rlwinm	r2,r30,4,28,31				; Isolate the segment we are setting
			rlwinm	r6,r12,19,1,3				; Insert the keys and N bit			
			lis		r0,0x8000					; Set bit 0
			rlwimi	r6,r12,20,12,31				; Insert 4:23 the VSID
			srw		r0,r0,r2					; Get bit corresponding to SR
			rlwimi	r6,r14,20,8,11				; Get the last nybble of the SR contents			
			or		r16,r16,r0					; Show that SR is valid
		
			mtsrin	r6,r30						; Set the actual SR
			
			stw		r16,validSegs(r19)			; Set the valid SR flags
		
			b		hpfPteMiss					; SR loaded, go do a PTE...
			
;
;			Make and enter 64-bit segment look-aside buffer entry.
;			Note that the cache entry is the right format except for valid bit.
;			We also need to convert from long long to 64-bit register values.
;


			.align	5
			
hpfLoadSeg64:
			ld		r16,validSegs(r19)			; Get the valid SLB entry flags
			sldi	r8,r29,32					; Move high order address over
			sldi	r10,r14,32					; Move high part of VSID over
			
			not		r3,r16						; Make valids be 0s
			li		r0,1						; Prepare to set bit 0
			
			cntlzd	r17,r3						; Find a free SLB	
			xor		r12,r12,r4					; Alter the storage key before loading segment table entry
			or		r9,r8,r30					; Form full 64-bit address
			cmplwi	r17,63						; Did we find a free SLB entry?		
			sldi	r0,r0,63					; Get bit 0 set
			or		r10,r10,r12					; Move in low part and keys
			addi	r17,r17,1					; Skip SLB 0 always
			blt++	hpfFreeSeg					; Yes, go load it...

;
;			No free SLB entries, select one that is in use and invalidate it
;
			lwz		r4,ppSegSteal(r19)			; Get the next slot to steal
			addi	r17,r4,pmapSegCacheUse+1	; Select stealee from non-cached slots only
			addi	r4,r4,1						; Set next slot to steal
			slbmfee	r7,r17						; Get the entry that is in the selected spot
			subi	r2,r4,63-pmapSegCacheUse	; Force steal to wrap
			rldicr	r7,r7,0,35					; Clear the valid bit and the rest
			srawi	r2,r2,31					; Get -1 if steal index still in range
			slbie	r7							; Invalidate the in-use SLB entry
			and		r4,r4,r2					; Reset steal index when it should wrap
			isync								; 
			
			stw		r4,ppSegSteal(r19)			; Set the next slot to steal
;
;			We are now ready to stick the SLB entry in the SLB and mark it in use
;

hpfFreeSeg:	
			subi	r4,r17,1					; Adjust shift to account for skipping slb 0
			mr		r7,r9						; Get a copy of the ESID with bits 36:63 clear
			srd		r0,r0,r4					; Set bit mask for allocation
			oris	r9,r9,0x0800				; Turn on the valid bit
			or		r16,r16,r0					; Turn on the allocation flag
			rldimi	r9,r17,0,58					; Copy in the SLB entry selector
			
			beq++	cr5,hpfNoBlow				; Skip blowing away the SLBE if this is not a special nest...
			slbie	r7							; Blow away a potential duplicate
			
hpfNoBlow:	slbmte	r10,r9						; Make that SLB entry

			std		r16,validSegs(r19)			; Mark as valid
			b		hpfPteMiss					; STE loaded, go do a PTE...
			
;
;			The segment has been set up and loaded if need be.  Now we are ready to build the
;			PTE and get it into the hash table.
;
;			Note that there is actually a race here.  If we start fault processing on
;			a different pmap, i.e., we have descended into a nested pmap, it is possible
;			that the nest could have been removed from the original pmap.  We would
;			succeed with this translation anyway.  I do not think we need to worry
;			about this (famous last words) because nobody should be unnesting anything 
;			if there are still people activily using them.  It should be up to the
;			higher level VM system to put the kibosh on this.
;
;			There is also another race here: if we fault on the same mapping on more than
;			one processor at the same time, we could end up with multiple PTEs for the same
;			mapping.  This is not a good thing....   We really only need one of the
;			fault handlers to finish, so what we do is to set a "fault in progress" flag in
;			the mapping.  If we see that set, we just abandon the handler and hope that by
;			the time we restore context and restart the interrupted code, the fault has
;			been resolved by the other guy.  If not, we will take another fault.
;
		
;
;			NOTE: IMPORTANT - CR7 contains a flag indicating if we have a block mapping or not.
;			It is required to stay there until after we call mapSelSlot!!!!
;

			.align	5
			
hpfPteMiss:	lwarx	r0,0,r31					; Load the mapping flag field
			lwz		r12,mpPte(r31)				; Get the quick pointer to PTE
			li		r3,mpHValid					; Get the PTE valid bit
			andi.	r2,r0,lo16(mpFIP)			; Are we handling a fault on the other side?
			ori		r2,r0,lo16(mpFIP)			; Set the fault in progress flag
			crnot	cr1_eq,cr0_eq				; Remember if FIP was on
			and.	r12,r12,r3					; Isolate the valid bit
			crorc	cr0_eq,cr1_eq,cr0_eq		; Bail if FIP is on.  Then, if already have PTE, bail...
			beq--	hpfAbandon					; Yes, other processor is or already has handled this...
			andi.	r0,r2,mpBlock				; Is this a block mapping?
			crmove	cr7_eq,cr0_eq				; Remember if we have a block mapping
			stwcx.	r2,0,r31					; Store the flags
			bne--	hpfPteMiss					; Collision, try again...

			bt++	pf64Bitb,hpfBldPTE64		; Skip down to the 64 bit stuff...

;
;			At this point we are about to do the 32-bit PTE generation.
;
;			The following is the R14:R15 pair that contains the "shifted" VSID:
;
;                             1        2        3        4        4        5      6 
;           0        8        6        4        2        0        8        6      3
;          +--------+--------+--------+--------+--------+--------+--------+--------+
;          |00000000|0000000V|VVVVVVVV|VVVVVVVV|VVVVVVVV|VVVVVVVV|VVVV////|////////|    
;          +--------+--------+--------+--------+--------+--------+--------+--------+                   
;
;			The 24 bits of the 32-bit architecture VSID is in the following:
;
;                             1        2        3        4        4        5      6 
;           0        8        6        4        2        0        8        6      3
;          +--------+--------+--------+--------+--------+--------+--------+--------+
;          |////////|////////|////////|////VVVV|VVVVVVVV|VVVVVVVV|VVVV////|////////|    
;          +--------+--------+--------+--------+--------+--------+--------+--------+                   
;


hpfBldPTE32:
			lwz		r25,mpVAddr+4(r31)			; Grab the base virtual address for the mapping (32-bit portion)	
			lwz		r24,mpPAddr(r31)			; Grab the base physical page number for the mapping	

			mfsdr1	r27							; Get the hash table base address

			rlwinm	r0,r23,0,4,19				; Isolate just the page index
			rlwinm	r18,r23,10,26,31			; Extract the API
			xor		r19,r15,r0					; Calculate hash << 12
			mr		r2,r25						; Save the flag part of the mapping
			rlwimi	r18,r14,27,1,4				; Move bits 28:31 of the "shifted" VSID into the PTE image
			rlwinm	r16,r27,16,7,15				; Extract the hash table size
			rlwinm	r25,r25,0,0,19				; Clear out the flags
			slwi	r24,r24,12					; Change ppnum to physical address (note: 36-bit addressing no supported)
			sub		r25,r23,r25					; Get offset in mapping to page (0 unless block map)
			ori		r16,r16,lo16(0xFFC0)		; Slap in the bottom of the mask
			rlwinm	r27,r27,0,0,15				; Extract the hash table base
			rlwinm	r19,r19,26,6,25				; Shift hash over to make offset into hash table
			add		r24,r24,r25					; Adjust to true physical address
			rlwimi	r18,r15,27,5,24				; Move bits 32:31 of the "shifted" VSID into the PTE image
			rlwimi	r24,r2,0,20,31				; Slap in the WIMG and prot
			and		r19,r19,r16					; Wrap hash table offset into the hash table
			ori		r24,r24,lo16(mpR)			; Turn on the reference bit right now
			rlwinm	r20,r19,28,10,29			; Shift hash over to make offset into PCA
			add		r19,r19,r27					; Point to the PTEG
			subfic	r20,r20,-4					; Get negative offset to PCA
			oris	r18,r18,lo16(0x8000)		; Make sure the valid bit is on
			add		r20,r20,r27					; Point to the PCA slot
		
;
;			We now have a valid PTE pair in R18/R24.  R18 is PTE upper and R24 is PTE lower.
;			R19 contains the offset of the PTEG in the hash table. R20 has offset into the PCA.
;		
;			We need to check PTE pointer (mpPte) again after we lock the PTEG.  It is possible 
;			that some other processor beat us and stuck in a PTE or that 
;			all we had was a simple segment exception and the PTE was there the whole time.
;			If we find one a pointer, we are done.
;

			mr		r7,r20						; Copy the PCA pointer
			bl		mapLockPteg					; Lock the PTEG
	
			lwz		r12,mpPte(r31)				; Get the offset to the PTE
			mr		r17,r6						; Remember the PCA image
			mr		r16,r6						; Prime the post-select PCA image
			andi.	r0,r12,mpHValid				; Is there a PTE here already?
			li		r21,8						; Get the number of slots

			bne-	cr7,hpfNoPte32				; Skip this for a block mapping...

			bne-	hpfBailOut					; Someone already did this for us...

;
;			The mapSelSlot function selects a PTEG slot to use. As input, it uses R3 as a 
;			pointer to the PCA.  When it returns, R3 contains 0 if an unoccupied slot was
;			selected, 1 if it stole a non-block PTE, or 2 if it stole a block mapped PTE.
;			R4 returns the slot index.
;
;			REMEMBER: CR7 indicates that we are building a block mapping.
;

hpfNoPte32:	subic.	r21,r21,1					; See if we have tried all slots
			mr		r6,r17						; Get back the original PCA
			rlwimi	r6,r16,0,8,15				; Insert the updated steal slot
			blt-	hpfBailOut					; Holy Cow, all slots are locked...
			
			bl		mapSelSlot					; Go select a slot (note that the PCA image is already set up)

			cmplwi	cr5,r3,1					; Did we steal a slot?
			rlwimi	r19,r4,3,26,28				; Insert PTE index into PTEG address yielding PTE address
			mr		r16,r6						; Remember the PCA image after selection
			blt+	cr5,hpfInser32				; Nope, no steal...
			
			lwz		r6,0(r19)					; Get the old PTE
			lwz		r7,4(r19)					; Get the real part of the stealee
			rlwinm	r6,r6,0,1,31				; Clear the valid bit
			bgt		cr5,hpfNipBM				; Do not try to lock a non-existant physent for a block mapping...
			srwi	r3,r7,12					; Change phys address to a ppnum
			bl		mapFindPhyTry				; Go find and try to lock physent (note: if R3 is 0, there is no physent for this page)
			cmplwi	cr1,r3,0					; Check if this is in RAM
			bne-	hpfNoPte32					; Could not get it, try for another...
			
			crmove	cr5_gt,cr1_eq				; If we did not find a physent, pretend that this is a block map
			
hpfNipBM:	stw		r6,0(r19)					; Set the invalid PTE

			sync								; Make sure the invalid is stored
			li		r9,tlbieLock				; Get the TLBIE lock
			rlwinm	r10,r6,21,0,3				; Shift last 4 bits of space to segment part
			
hpfTLBIE32:	lwarx	r0,0,r9						; Get the TLBIE lock 
			mfsprg	r4,0						; Get the per_proc
			rlwinm	r8,r6,25,18,31				; Extract the space ID
			rlwinm	r11,r6,25,18,31				; Extract the space ID
			lwz		r7,hwSteals(r4)				; Get the steal count
			srwi	r2,r6,7						; Align segment number with hash
			rlwimi	r11,r11,14,4,17				; Get copy above ourselves
			mr.		r0,r0						; Is it locked? 
			srwi	r0,r19,6					; Align PTEG offset for back hash
			xor		r2,r2,r11					; Get the segment number (plus a whole bunch of extra bits)
 			xor		r11,r11,r0					; Hash backwards to partial vaddr
			rlwinm	r12,r2,14,0,3				; Shift segment up
			mfsprg	r2,2						; Get feature flags 
			li		r0,1						; Get our lock word 
			rlwimi	r12,r6,22,4,9				; Move up the API
			bne-	hpfTLBIE32					; It is locked, go wait...
			rlwimi	r12,r11,12,10,19			; Move in the rest of the vaddr
			
			stwcx.	r0,0,r9						; Try to get it
			bne-	hpfTLBIE32					; We was beat...
			addi	r7,r7,1						; Bump the steal count
			
			rlwinm.	r0,r2,0,pfSMPcapb,pfSMPcapb	; Can this be an MP box?
			li		r0,0						; Lock clear value 

			tlbie	r12							; Invalidate it everywhere 

			beq-	hpfNoTS32					; Can not have MP on this machine...
			
			eieio								; Make sure that the tlbie happens first 
			tlbsync								; Wait for everyone to catch up 
			sync								; Make sure of it all
			
hpfNoTS32:	
			stw		r0,tlbieLock(0)				; Clear the tlbie lock
			
			stw		r7,hwSteals(r4)				; Save the steal count
			bgt		cr5,hpfInser32				; We just stole a block mapping...
			
			lwz		r4,4(r19)					; Get the RC of the just invalidated PTE
			
			la		r11,ppLink+4(r3)			; Point to the master RC copy
			lwz		r7,ppLink+4(r3)				; Grab the pointer to the first mapping
			rlwinm	r2,r4,27,ppRb-32,ppCb-32	; Position the new RC

hpfMrgRC32:	lwarx	r0,0,r11					; Get the master RC
			or		r0,r0,r2					; Merge in the new RC
			stwcx.	r0,0,r11					; Try to stick it back
			bne-	hpfMrgRC32					; Try again if we collided...
			
			
hpfFPnch:	rlwinm.	r7,r7,0,0,25				; Clean and test mapping address
			beq-	hpfLostPhys					; We could not find our mapping.  Kick the bucket...
			
			lhz		r10,mpSpace(r7)				; Get the space
			lwz		r9,mpVAddr+4(r7)			; And the vaddr
			cmplw	cr1,r10,r8					; Is this one of ours?
			xor		r9,r12,r9					; Compare virtual address
			cmplwi	r9,0x1000					; See if we really match
			crand	cr0_eq,cr1_eq,cr0_lt		; See if both space and vaddr match
			beq+	hpfFPnch2					; Yes, found ours...
			
			lwz		r7,mpAlias+4(r7)			; Chain on to the next
			b		hpfFPnch					; Check it out...

hpfFPnch2:	sub		r0,r19,r27					; Get offset to the PTEG
			stw		r0,mpPte(r7)				; Invalidate the quick pointer (keep quick pointer pointing to PTEG)
			bl		mapPhysUnlock				; Unlock the physent now
			
hpfInser32:	oris	r18,r18,lo16(0x8000)		; Make sure the valid bit is on

			stw		r24,4(r19)					; Stuff in the real part of the PTE
			eieio								; Make sure this gets there first

			stw		r18,0(r19)					; Stuff the virtual part of the PTE and make it valid
			mr		r17,r16						; Get the PCA image to save
			b		hpfFinish					; Go join the common exit code...
			
			
;
;			At this point we are about to do the 64-bit PTE generation.
;
;			The following is the R14:R15 pair that contains the "shifted" VSID:
;
;                             1        2        3        4        4        5      6 
;           0        8        6        4        2        0        8        6      3
;          +--------+--------+--------+--------+--------+--------+--------+--------+
;          |00000000|0000000V|VVVVVVVV|VVVVVVVV|VVVVVVVV|VVVVVVVV|VVVV////|////////|    
;          +--------+--------+--------+--------+--------+--------+--------+--------+                   
;
;

			.align	5

hpfBldPTE64:
			ld		r10,mpVAddr(r31)			; Grab the base virtual address for the mapping 
			lwz		r24,mpPAddr(r31)			; Grab the base physical page number for the mapping	

			mfsdr1	r27							; Get the hash table base address

			sldi	r11,r22,32					; Slide top of adjusted EA over
			sldi	r14,r14,32					; Slide top of VSID over
			rlwinm	r5,r27,0,27,31				; Isolate the size
			eqv		r16,r16,r16					; Get all foxes here
			rlwimi	r15,r23,16,20,24			; Stick in EA[36:40] to make AVPN	
			mr		r2,r10						; Save the flag part of the mapping
			or		r11,r11,r23					; Stick in bottom of adjusted EA for full 64-bit value	
			rldicr	r27,r27,0,45				; Clean up the hash table base
			or		r15,r15,r14					; Stick in bottom of AVPN for full 64-bit value	
			rlwinm	r0,r11,0,4,19				; Clear out everything but the page
			subfic	r5,r5,46					; Get number of leading zeros
			xor		r19,r0,r15					; Calculate hash
			ori		r15,r15,1					; Turn on valid bit in AVPN to make top of PTE
			srd		r16,r16,r5					; Shift over to get length of table
			srdi	r19,r19,5					; Convert page offset to hash table offset
			rldicr	r16,r16,0,56				; Clean up lower bits in hash table size			
			rldicr	r10,r10,0,51				; Clear out flags
			sldi	r24,r24,12					; Change ppnum to physical address
			sub		r11,r11,r10					; Get the offset from the base mapping
			and		r19,r19,r16					; Wrap into hash table
			add		r24,r24,r11					; Get actual physical address of this page
			srdi	r20,r19,5					; Convert PTEG offset to PCA offset
			rldimi	r24,r2,0,52					; Insert the keys, WIMG, RC, etc.
			subfic	r20,r20,-4					; Get negative offset to PCA
			ori		r24,r24,lo16(mpR)			; Force on the reference bit
			add		r20,r20,r27					; Point to the PCA slot		
			add		r19,r19,r27					; Point to the PTEG
			
;
;			We now have a valid PTE pair in R15/R24.  R15 is PTE upper and R24 is PTE lower.
;			R19 contains the offset of the PTEG in the hash table. R20 has offset into the PCA.
;		
;			We need to check PTE pointer (mpPte) again after we lock the PTEG.  It is possible 
;			that some other processor beat us and stuck in a PTE or that 
;			all we had was a simple segment exception and the PTE was there the whole time.
;			If we find one a pointer, we are done.
;
			
			mr		r7,r20						; Copy the PCA pointer
			bl		mapLockPteg					; Lock the PTEG
	
			lwz		r12,mpPte(r31)				; Get the offset to the PTE
			mr		r17,r6						; Remember the PCA image
			mr		r18,r6						; Prime post-selection PCA image
			andi.	r0,r12,mpHValid				; See if we have a PTE now
			li		r21,8						; Get the number of slots
		
			bne--	cr7,hpfNoPte64				; Skip this for a block mapping...

			bne--	hpfBailOut					; Someone already did this for us...

;
;			The mapSelSlot function selects a PTEG slot to use. As input, it uses R3 as a 
;			pointer to the PCA.  When it returns, R3 contains 0 if an unoccupied slot was
;			selected, 1 if it stole a non-block PTE, or 2 if it stole a block mapped PTE.
;			R4 returns the slot index.
;
;			REMEMBER: CR7 indicates that we are building a block mapping.
;

hpfNoPte64:	subic.	r21,r21,1					; See if we have tried all slots
			mr		r6,r17						; Restore original state of PCA
			rlwimi	r6,r18,0,8,15				; Insert the updated steal slot
			blt-	hpfBailOut					; Holy Cow, all slots are locked...
			
			bl		mapSelSlot					; Go select a slot

			cmplwi	cr5,r3,1					; Did we steal a slot?			
			mr		r18,r6						; Remember the PCA image after selection
			insrdi	r19,r4,3,57					; Insert slot index into PTEG address bits 57:59, forming the PTE address
			lwz		r10,hwSteals(r2)			; Get the steal count
			blt++	cr5,hpfInser64				; Nope, no steal...

			ld		r6,0(r19)					; Get the old PTE
			ld		r7,8(r19)					; Get the real part of the stealee
			rldicr	r6,r6,0,62					; Clear the valid bit
			bgt		cr5,hpfNipBMx				; Do not try to lock a non-existant physent for a block mapping...
			srdi	r3,r7,12					; Change page address to a page address
			bl		mapFindPhyTry				; Go find and try to lock physent (note: if R3 is 0, there is no physent for this page)
			cmplwi	cr1,r3,0					; Check if this is in RAM
			bne--	hpfNoPte64					; Could not get it, try for another...
			
			crmove	cr5_gt,cr1_eq				; If we did not find a physent, pretend that this is a block map
			
hpfNipBMx:	std		r6,0(r19)					; Set the invalid PTE
			li		r9,tlbieLock				; Get the TLBIE lock

			srdi	r11,r6,5					; Shift VSID over for back hash
			mfsprg	r4,0						; Get the per_proc
			xor		r11,r11,r19					; Hash backwards to get low bits of VPN
			sync								; Make sure the invalid is stored
			
			sldi	r12,r6,16					; Move AVPN to EA position
			sldi	r11,r11,5					; Move this to the page position
			
hpfTLBIE64:	lwarx	r0,0,r9						; Get the TLBIE lock 
			mr.		r0,r0						; Is it locked? 
			li		r0,1						; Get our lock word
			bne--	hpfTLBIE65					; It is locked, go wait...
			
			stwcx.	r0,0,r9						; Try to get it
			rldimi	r12,r11,0,41				; Stick the low part of the page number into the AVPN
			rldicl	r8,r6,52,50					; Isolate the address space ID
			bne--	hpfTLBIE64					; We was beat...
			addi	r10,r10,1					; Bump the steal count
			
			rldicl	r11,r12,0,16				; Clear cause the book says so
			li		r0,0						; Lock clear value 

			tlbie	r11							; Invalidate it everywhere 

			mr		r7,r8						; Get a copy of the space ID
			eieio								; Make sure that the tlbie happens first
			rldimi	r7,r7,14,36					; Copy address space to make hash value
			tlbsync								; Wait for everyone to catch up
			rldimi	r7,r7,28,22					; Add in a 3rd copy of the hash up top
			isync								
			srdi	r2,r6,26					; Shift original segment down to bottom
			
			ptesync								; Make sure of it all

			stw		r0,tlbieLock(0)				; Clear the tlbie lock

			xor		r7,r7,r2					; Compute original segment

			stw		r10,hwSteals(r4)			; Save the steal count
			bgt		cr5,hpfInser64				; We just stole a block mapping...
			
			rldimi	r12,r7,28,0					; Insert decoded segment
			rldicl	r4,r12,0,13					; Trim to max supported address
			
			ld		r12,8(r19)					; Get the RC of the just invalidated PTE			

			la		r11,ppLink+4(r3)			; Point to the master RC copy
			ld		r7,ppLink(r3)				; Grab the pointer to the first mapping
			rlwinm	r2,r12,27,ppRb-32,ppCb-32	; Position the new RC

hpfMrgRC64:	lwarx	r0,0,r11					; Get the master RC
			li		r12,0xFF					; Get mask to clean up alias pointer
			or		r0,r0,r2					; Merge in the new RC
			rldicl	r12,r12,62,0				; Rotate clean up mask to get 0xC0000000000000003F
			stwcx.	r0,0,r11					; Try to stick it back
			bne--	hpfMrgRC64					; Try again if we collided...
	
hpfFPnchx:	andc.	r7,r7,r12					; Clean and test mapping address
			beq--	hpfLostPhys					; We could not find our mapping.  Kick the bucket...
			
			lhz		r10,mpSpace(r7)				; Get the space
			ld		r9,mpVAddr(r7)				; And the vaddr
			cmplw	cr1,r10,r8					; Is this one of ours?
			xor		r9,r4,r9					; Compare virtual address
			cmpldi	r9,0x1000					; See if we really match
			crand	cr0_eq,cr1_eq,cr0_lt		; See if both space and vaddr match
			beq++	hpfFPnch2x					; Yes, found ours...
			
			ld		r7,mpAlias(r7)				; Chain on to the next
			b		hpfFPnchx					; Check it out...

			.align	5

hpfTLBIE65:	li		r7,lgKillResv				; Point to the reservatio kill area
			stwcx.	r7,0,r7						; Kill reservation		
			
hpfTLBIE63: lwz		r0,0(r9)					; Get the TLBIE lock
			mr.		r0,r0						; Is it locked?
			beq++	hpfTLBIE64					; Yup, wait for it...
			b		hpfTLBIE63					; Nope, try again..



hpfFPnch2x:	sub		r0,r19,r27					; Get offset to PTEG
			stw		r0,mpPte(r7)				; Invalidate the quick pointer (keep pointing at PTEG though)
			bl		mapPhysUnlock				; Unlock the physent now
			

hpfInser64:	std		r24,8(r19)					; Stuff in the real part of the PTE
			eieio								; Make sure this gets there first
			std		r15,0(r19)					; Stuff the virtual part of the PTE and make it valid
			mr		r17,r18						; Get the PCA image to set
			b		hpfFinish					; Go join the common exit code...

hpfLostPhys:
			lis		r0,hi16(Choke)				; System abend - we must find the stolen mapping or we are dead
			ori		r0,r0,lo16(Choke)			; System abend
			sc
			
;
;			This is the common code we execute when we are finished setting up the PTE.
;
	
			.align	5
			
hpfFinish:	sub		r4,r19,r27					; Get offset of PTE
			ori		r4,r4,lo16(mpHValid)		; Add valid bit to PTE offset
			bne		cr7,hpfBailOut				; Do not set the PTE pointer for a block map
			stw		r4,mpPte(r31)				; Remember our PTE
			
hpfBailOut:	eieio								; Make sure all updates come first
			stw		r17,0(r20)					; Unlock and set the final PCA
			
;
;			This is where we go if we have started processing the fault, but find that someone
;			else has taken care of it.
;

hpfIgnore:	lwz		r2,mpFlags(r31)				; Get the mapping flags
			rlwinm	r2,r2,0,mpFIPb+1,mpFIPb-1	; Clear the "fault in progress" flag
			sth		r2,mpFlags+2(r31)			; Set it
			
			la		r3,pmapSXlk(r28)			; Point to the pmap search lock
			bl		sxlkUnlock					; Unlock the search list

			li		r11,T_IN_VAIN				; Say that it was handled
			b		EXT(PFSExit)				; Leave...

;
;			This is where we go when we  find that someone else
;			is in the process of handling the fault.
;

hpfAbandon:	li		r3,lgKillResv				; Kill off any reservation
			stwcx.	r3,0,r3						; Do it
			
			la		r3,pmapSXlk(r28)			; Point to the pmap search lock
			bl		sxlkUnlock					; Unlock the search list

			li		r11,T_IN_VAIN				; Say that it was handled
			b		EXT(PFSExit)				; Leave...
			

			
/*
 *			hw_set_user_space(pmap) 
 *			hw_set_user_space_dis(pmap) 
 *
 * 			Indicate whether memory space needs to be switched.
 *			We really need to turn off interrupts here, because we need to be non-preemptable
 *
 *			hw_set_user_space_dis is used when interruptions are already disabled. Mind the
 *			register usage here.   The VMM switch code in vmachmon.s that calls this
 *			know what registers are in use.  Check that if these change.
 */


	
			.align	5
			.globl	EXT(hw_set_user_space)

LEXT(hw_set_user_space)

			lis		r8,hi16(MASK(MSR_VEC))		; Get the vector enable
			mfmsr	r10							; Get the current MSR 
			ori		r8,r8,lo16(MASK(MSR_FP))	; Add in FP
			ori		r9,r8,lo16(MASK(MSR_EE))	; Add in the EE
			andc	r10,r10,r8					; Turn off VEC, FP for good
			andc	r9,r10,r9					; Turn off EE also
			mtmsr	r9							; Disable them 
 			isync								; Make sure FP and vec are off
 			mfsprg	r6,0						; Get the per_proc_info address
			lwz		r2,ppUserPmapVirt(r6)		; Get our virtual pmap address
			mfsprg	r4,2						; The the feature flags
			lwz		r7,pmapvr(r3)				; Get the v to r translation
 			lwz		r8,pmapvr+4(r3)				; Get the v to r translation
 			mtcrf	0x80,r4						; Get the Altivec flag
			xor		r4,r3,r8					; Get bottom of the real address of bmap anchor
			cmplw	cr1,r3,r2					; Same address space as before?
			stw		r7,ppUserPmap(r6)			; Show our real pmap address
			crorc	cr1_eq,cr1_eq,pfAltivecb	; See if same address space or not altivec machine
			stw		r4,ppUserPmap+4(r6)			; Show our real pmap address
			stw		r3,ppUserPmapVirt(r6)		; Show our virtual pmap address
			mtmsr	r10							; Restore interruptions 
			beqlr--	cr1							; Leave if the same address space or not Altivec

			dssall								; Need to kill all data streams if adrsp changed
			sync
			blr									; Return... 
	
			.align	5
			.globl	EXT(hw_set_user_space_dis)

LEXT(hw_set_user_space_dis)

 			lwz		r7,pmapvr(r3)				; Get the v to r translation
 			mfsprg	r4,2						; The the feature flags
			lwz		r8,pmapvr+4(r3)				; Get the v to r translation
			mfsprg	r6,0						; Get the per_proc_info address
			lwz		r2,ppUserPmapVirt(r6)		; Get our virtual pmap address
 			mtcrf	0x80,r4						; Get the Altivec flag
			xor		r4,r3,r8					; Get bottom of the real address of bmap anchor
			cmplw	cr1,r3,r2					; Same address space as before?
			stw		r7,ppUserPmap(r6)			; Show our real pmap address
			crorc	cr1_eq,cr1_eq,pfAltivecb	; See if same address space or not altivec machine
			stw		r4,ppUserPmap+4(r6)			; Show our real pmap address
			stw		r3,ppUserPmapVirt(r6)		; Show our virtual pmap address
			beqlr--	cr1							; Leave if the same

			dssall								; Need to kill all data streams if adrsp changed
			sync
			blr									; Return...
	
/*			int mapalc1(struct mappingblok *mb) - Finds, allocates, and zeros a free 1-bit mapping entry
 *
 *			Lock must already be held on mapping block list
 *			returns 0 if all slots filled.
 *			returns n if a slot is found and it is not the last
 *			returns -n if a slot is found and it is the last
 *			when n and -n are returned, the corresponding bit is cleared
 *			the mapping is zeroed out before return
 *
 */

			.align	5
			.globl	EXT(mapalc1)

LEXT(mapalc1)
			lwz		r4,mbfree(r3)				; Get the 1st mask 
			lis		r0,0x8000					; Get the mask to clear the first free bit
			lwz		r5,mbfree+4(r3)				; Get the 2nd mask 
			mr		r12,r3						; Save the block ptr
			cntlzw	r3,r4						; Get first 1-bit in 1st word
			srw.	r9,r0,r3					; Get bit corresponding to first free one
			cntlzw	r10,r5						; Get first free field in second word
			andc	r4,r4,r9					; Turn 1-bit off in 1st word
			bne		mapalc1f					; Found one in 1st word
			
			srw.	r9,r0,r10					; Get bit corresponding to first free one in 2nd word
            li		r3,0						; assume failure return
			andc	r5,r5,r9					; Turn it off
			beqlr--								; There are no 1 bits left...
            addi	r3,r10,32					; set the correct number
            
mapalc1f:
            or.		r0,r4,r5					; any more bits set?
            stw		r4,mbfree(r12)				; update bitmasks
            stw		r5,mbfree+4(r12)
            
            slwi	r6,r3,6						; get (n * mpBasicSize), ie offset of mapping in block
            addi	r7,r6,32
            dcbz	r6,r12						; clear the 64-byte mapping
            dcbz	r7,r12
            
            bnelr++								; return if another bit remains set
            
            neg		r3,r3						; indicate we just returned the last bit
            blr


/*			int mapalc2(struct mappingblok *mb) - Finds, allocates, and zero's a free 2-bit mapping entry
 *
 *			Lock must already be held on mapping block list
 *			returns 0 if all slots filled.
 *			returns n if a slot is found and it is not the last
 *			returns -n if a slot is found and it is the last
 *			when n and -n are returned, the corresponding bits are cleared
 * 			We find runs of 2 consecutive 1 bits by cntlzw(n & (n<<1)).
 *			the mapping is zero'd out before return
 */

			.align	5
			.globl	EXT(mapalc2)
LEXT(mapalc2)
			lwz		r4,mbfree(r3)				; Get the first mask 
			lis		r0,0x8000					; Get the mask to clear the first free bit
			lwz		r5,mbfree+4(r3)				; Get the second mask 
			mr		r12,r3						; Save the block ptr
            slwi	r6,r4,1						; shift first word over
            and		r6,r4,r6					; lite start of double bit runs in 1st word
            slwi	r7,r5,1						; shift 2nd word over
			cntlzw	r3,r6						; Get first free 2-bit run in 1st word
            and		r7,r5,r7					; lite start of double bit runs in 2nd word
			srw.	r9,r0,r3					; Get bit corresponding to first run in 1st word
			cntlzw	r10,r7						; Get first free field in second word
            srwi	r11,r9,1					; shift over for 2nd bit in 1st word
			andc	r4,r4,r9					; Turn off 1st bit in 1st word
            andc	r4,r4,r11					; turn off 2nd bit in 1st word
			bne		mapalc2a					; Found two consecutive free bits in 1st word
			
			srw.	r9,r0,r10					; Get bit corresponding to first free one in second word
            li		r3,0						; assume failure
            srwi	r11,r9,1					; get mask for 2nd bit
			andc	r5,r5,r9					; Turn off 1st bit in 2nd word
            andc	r5,r5,r11					; turn off 2nd bit in 2nd word
			beq--	mapalc2c					; There are no runs of 2 bits in 2nd word either
            addi	r3,r10,32					; set the correct number
            
mapalc2a:
            or.		r0,r4,r5					; any more bits set?
            stw		r4,mbfree(r12)				; update bitmasks
            stw		r5,mbfree+4(r12)
            slwi	r6,r3,6						; get (n * mpBasicSize), ie offset of mapping in block
            addi	r7,r6,32
            addi	r8,r6,64
            addi	r9,r6,96
            dcbz	r6,r12						; zero out the 128-byte mapping
            dcbz	r7,r12						; we use the slow 32-byte dcbz even on 64-bit machines
            dcbz	r8,r12						; because the mapping may not be 128-byte aligned
            dcbz	r9,r12
            
            bnelr++								; return if another bit remains set
            
            neg		r3,r3						; indicate we just returned the last bit
            blr
            
mapalc2c:
            rlwinm	r7,r5,1,31,31				; move bit 0 of 2nd word to bit 31
            and.	r0,r4,r7					; is the 2-bit field that spans the 2 words free?
            beqlr								; no, we failed
            rlwinm	r4,r4,0,0,30				; yes, turn off bit 31 of 1st word
            rlwinm	r5,r5,0,1,31				; turn off bit 0 of 2nd word
            li		r3,31						; get index of this field
            b		mapalc2a
			

;
;			This routine initialzes the hash table and PCA.
;			It is done here because we may need to be 64-bit to do it.
;

			.align	5
			.globl	EXT(hw_hash_init)

LEXT(hw_hash_init)

 			mfsprg	r10,2						; Get feature flags 
			lis		r12,hi16(EXT(hash_table_size))		; Get hash table size address
			mtcrf	0x02,r10					; move pf64Bit to cr6
			lis		r11,hi16(EXT(hash_table_base))		; Get hash table base address
			lis		r4,0xFF01					; Set all slots free and start steal at end
			ori		r12,r12,lo16(EXT(hash_table_size))	; Get hash table size address
			ori		r11,r11,lo16(EXT(hash_table_base))	; Get hash table base address

			lwz		r12,0(r12)					; Get hash table size
			li		r3,0						; Get start
			bt++	pf64Bitb,hhiSF				; skip if 64-bit (only they take the hint)

			lwz		r11,4(r11)					; Get hash table base
			
hhiNext32:	cmplw	r3,r12						; Have we reached the end?
			bge-	hhiCPCA32					; Yes...			
			dcbz	r3,r11						; Clear the line
			addi	r3,r3,32					; Next one...
			b		hhiNext32					; Go on...

hhiCPCA32:	rlwinm	r12,r12,28,4,29				; Get number of slots * 4
			li		r3,-4						; Displacement to first PCA entry
			neg		r12,r12						; Get negative end of PCA	
			
hhiNPCA32:	stwx	r4,r3,r11					; Initialize the PCA entry
			subi	r3,r3,4						; Next slot
			cmpw	r3,r12						; Have we finished?
			bge+	hhiNPCA32					; Not yet...
			blr									; Leave...

hhiSF:		mfmsr	r9							; Save the MSR
			li		r8,1						; Get a 1
			mr		r0,r9						; Get a copy of the MSR
			ld		r11,0(r11)					; Get hash table base
			rldimi	r0,r8,63,MSR_SF_BIT			; Set SF bit (bit 0)
			mtmsrd	r0							; Turn on SF
			isync
			
			
hhiNext64:	cmpld	r3,r12						; Have we reached the end?
			bge--	hhiCPCA64					; Yes...			
			dcbz128	r3,r11						; Clear the line
			addi	r3,r3,128					; Next one...
			b		hhiNext64					; Go on...

hhiCPCA64:	rlwinm	r12,r12,27,5,29				; Get number of slots * 4
			li		r3,-4						; Displacement to first PCA entry
			neg		r12,r12						; Get negative end of PCA	
		
hhiNPCA64:	stwx	r4,r3,r11					; Initialize the PCA entry
			subi	r3,r3,4						; Next slot
			cmpd	r3,r12						; Have we finished?
			bge++	hhiNPCA64					; Not yet...

			mtmsrd	r9							; Turn off SF if it was off
			isync
			blr									; Leave...
			
			
;
;			This routine sets up the hardware to start translation.
;			Note that we do NOT start translation.
;

			.align	5
			.globl	EXT(hw_setup_trans)

LEXT(hw_setup_trans)

 			mfsprg	r11,0						; Get the per_proc block
 			mfsprg	r12,2						; Get feature flags 
 			li		r0,0						; Get a 0
 			li		r2,1						; And a 1
			mtcrf	0x02,r12					; Move pf64Bit to cr6
			stw		r0,validSegs(r11)			; Make sure we think all SR/STEs are invalid
			stw		r0,validSegs+4(r11)			; Make sure we think all SR/STEs are invalid, part deux
			sth		r2,ppInvSeg(r11)			; Force a reload of the SRs
			sth		r0,ppCurSeg(r11)			; Set that we are starting out in kernel
			
			bt++	pf64Bitb,hstSF				; skip if 64-bit (only they take the hint)

			li		r9,0						; Clear out a register
			sync
			isync
			mtdbatu 0,r9						; Invalidate maps
			mtdbatl 0,r9						; Invalidate maps
			mtdbatu 1,r9						; Invalidate maps
			mtdbatl 1,r9						; Invalidate maps
			mtdbatu 2,r9						; Invalidate maps
			mtdbatl 2,r9						; Invalidate maps
			mtdbatu 3,r9						; Invalidate maps
			mtdbatl 3,r9						; Invalidate maps

			mtibatu 0,r9						; Invalidate maps
			mtibatl 0,r9						; Invalidate maps
			mtibatu 1,r9						; Invalidate maps
			mtibatl 1,r9						; Invalidate maps
			mtibatu 2,r9						; Invalidate maps
			mtibatl 2,r9						; Invalidate maps
			mtibatu 3,r9						; Invalidate maps
			mtibatl 3,r9						; Invalidate maps

			lis		r11,hi16(EXT(hash_table_base))		; Get hash table base address
			lis		r12,hi16(EXT(hash_table_size))		; Get hash table size address
			ori		r11,r11,lo16(EXT(hash_table_base))	; Get hash table base address
			ori		r12,r12,lo16(EXT(hash_table_size))	; Get hash table size address
			lwz		r11,4(r11)					; Get hash table base
			lwz		r12,0(r12)					; Get hash table size
			subi	r12,r12,1					; Back off by 1
			rlwimi	r11,r12,16,23,31			; Stick the size into the sdr1 image
			
			mtsdr1	r11							; Ok, we now have the hash table set up
			sync
			
			li		r12,invalSpace				; Get the invalid segment value
			li		r10,0						; Start low
			
hstsetsr:	mtsrin	r12,r10						; Set the SR
			addis	r10,r10,0x1000				; Bump the segment
			mr.		r10,r10						; Are we finished?
			bne+	hstsetsr					; Nope...	
			sync
			blr									; Return...

;
;			64-bit version
;

hstSF:		lis		r11,hi16(EXT(hash_table_base))		; Get hash table base address
			lis		r12,hi16(EXT(hash_table_size))		; Get hash table size address
			ori		r11,r11,lo16(EXT(hash_table_base))	; Get hash table base address
			ori		r12,r12,lo16(EXT(hash_table_size))	; Get hash table size address
			ld		r11,0(r11)					; Get hash table base
			lwz		r12,0(r12)					; Get hash table size
			cntlzw	r10,r12						; Get the number of bits
			subfic	r10,r10,13					; Get the extra bits we need
			or		r11,r11,r10					; Add the size field to SDR1
			
			mtsdr1	r11							; Ok, we now have the hash table set up
			sync

			li		r0,0						; Set an SLB slot index of 0
			slbia								; Trash all SLB entries (except for entry 0 that is)
			slbmfee	r7,r0						; Get the entry that is in SLB index 0
			rldicr	r7,r7,0,35					; Clear the valid bit and the rest
			slbie	r7							; Invalidate it

			blr									; Return...


;
;			This routine turns on translation for the first time on a processor
;

			.align	5
			.globl	EXT(hw_start_trans)

LEXT(hw_start_trans)

			
			mfmsr	r10							; Get the msr
			ori		r10,r10,lo16(MASK(MSR_IR) | MASK(MSR_DR))	; Turn on translation

			mtmsr	r10							; Everything falls apart here
			isync
			
			blr									; Back to it.



;
;			This routine validates a segment register.
;				hw_map_seg(pmap_t pmap, addr64_t seg, addr64_t va)
;
;				r3 = virtual pmap
;				r4 = segment[0:31]
;				r5 = segment[32:63]
;				r6 = va[0:31]
;				r7 = va[32:63]
;
;			Note that we transform the addr64_t (long long) parameters into single 64-bit values.
;			Note that there is no reason to apply the key modifier here because this is only
;			used for kernel accesses.
;

			.align	5
			.globl	EXT(hw_map_seg)

LEXT(hw_map_seg)

			lwz		r0,pmapSpace(r3)			; Get the space, we will need it soon
			lwz		r9,pmapFlags(r3)			; Get the flags for the keys now
 			mfsprg	r10,2						; Get feature flags 
			mfsprg	r12,0						; Get the per_proc

;
;			Note: the following code would problably be easier to follow if I split it,
;			but I just wanted to see if I could write this to work on both 32- and 64-bit
;			machines combined.
;
			
;
;			Here we enter with va[0:31] in r6[0:31] (or r6[32:63] on 64-bit machines)
;			and va[32:63] in r7[0:31] (or r7[32:63] on 64-bit machines)

			rlwinm	r4,r4,0,1,0					; Copy seg[0:31] into r4[0;31] - no-op for 32-bit
			rlwinm	r7,r7,18,14,17				; Slide va[32:35] east to just west of space ID
			mtcrf	0x02,r10					; Move pf64Bit and pfNoMSRirb to cr5 and 6
			srwi	r8,r6,14					; Slide va[0:17] east to just west of the rest
			rlwimi	r7,r6,18,0,13				; Slide va[18:31] east to just west of slid va[32:25]
			rlwimi	r0,r0,14,4,17				; Dup address space ID above itself
			rlwinm	r8,r8,0,1,0					; Dup low part into high (does nothing on 32-bit machines)
			rlwinm	r2,r0,28,0,31				; Rotate rotate low nybble to top of low half
			rlwimi	r2,r2,0,1,0					; Replicate bottom 32 into top 32
			rlwimi	r8,r7,0,0,31				; Join va[0:17] with va[18:35] (just like mr on 32-bit machines)			

			rlwimi	r2,r0,0,4,31				; We should now have 4 copies of the space
												; concatenated together.   There is garbage
												; at the top for 64-bit but we will clean
												; that out later.
			rlwimi	r4,r5,0,0,31				; Copy seg[32:63] into r4[32:63] - just like mr for 32-bit

			
;
;			Here we exit with va[0:35] shifted into r8[14:51], zeros elsewhere, or
;			va[18:35] shifted into r8[0:17], zeros elsewhere on 32-bit machines
;			
												
;
;			What we have now is:
;
;					 0        0        1        2        3        4        4        5      6
;					 0        8        6        4        2        0        8        6      3	- for 64-bit machines
;					+--------+--------+--------+--------+--------+--------+--------+--------+
;			r2 =	|xxxx0000|AAAAAAAA|AAAAAABB|BBBBBBBB|BBBBCCCC|CCCCCCCC|CCDDDDDD|DDDDDDDD|	- hash value
;					+--------+--------+--------+--------+--------+--------+--------+--------+
;														 0        0        1        2      3	- for 32-bit machines
;														 0        8        6        4      1
;
;					 0        0        1        2        3        4        4        5      6
;					 0        8        6        4        2        0        8        6      3	- for 64-bit machines
;					+--------+--------+--------+--------+--------+--------+--------+--------+
;			r8 =	|00000000|000000SS|SSSSSSSS|SSSSSSSS|SSSSSSSS|SSSSSSSS|SS000000|00000000|	- shifted and cleaned EA
;					+--------+--------+--------+--------+--------+--------+--------+--------+
;														 0        0        1        2      3	- for 32-bit machines
;														 0        8        6        4      1
;
;					 0        0        1        2        3        4        4        5      6
;					 0        8        6        4        2        0        8        6      3	- for 64-bit machines
;					+--------+--------+--------+--------+--------+--------+--------+--------+
;			r4 =	|SSSSSSSS|SSSSSSSS|SSSSSSSS|SSSSSSSS|SSSS0000|00000000|00000000|00000000|	- Segment
;					+--------+--------+--------+--------+--------+--------+--------+--------+
;														 0        0        1        2      3	- for 32-bit machines
;														 0        8        6        4      1


			xor		r8,r8,r2					; Calculate VSID
			
			bf--	pf64Bitb,hms32bit			; Skip out if 32-bit...
			
			li		r0,1						; Prepare to set bit 0 (also to clear EE)
			mfmsr	r6							; Get current MSR
			li		r2,MASK(MSR_IR)|MASK(MSR_DR)	; Get the translation bits
			mtmsrd	r0,1						; Set only the EE bit to 0
			rlwinm	r6,r6,0,MSR_EE_BIT,MSR_EE_BIT	; See if EE bit is on
			mfmsr	r11							; Get the MSR right now, after disabling EE
			andc	r2,r11,r2					; Turn off translation now
			rldimi	r2,r0,63,0					; Get bit 64-bit turned on
			or		r11,r11,r6					; Turn on the EE bit if it was on
			mtmsrd	r2							; Make sure translation and EE are off and 64-bit is on
			isync								; Hang out a bit
						
			ld		r6,validSegs(r12)			; Get the valid SLB entry flags
			sldi	r9,r9,9						; Position the key and noex bit
			
			rldimi	r5,r8,12,0					; Form the VSID/key
			
			not		r3,r6						; Make valids be 0s
			
			cntlzd	r7,r3						; Find a free SLB	
			cmplwi	r7,63						; Did we find a free SLB entry?		
			
			slbie	r4							; Since this ESID may still be in an SLBE, kill it

			oris	r4,r4,0x0800				; Turn on the valid bit in ESID
			addi	r7,r7,1						; Make sure we skip slb 0
			blt++	hmsFreeSeg					; Yes, go load it...

;
;			No free SLB entries, select one that is in use and invalidate it
;
			lwz		r2,ppSegSteal(r12)			; Get the next slot to steal
			addi	r7,r2,pmapSegCacheUse+1		; Select stealee from non-cached slots only
			addi	r2,r2,1						; Set next slot to steal
			slbmfee	r3,r7						; Get the entry that is in the selected spot
			subi	r8,r2,64-(pmapSegCacheUse+1)	; Force steal to wrap
			rldicr	r3,r3,0,35					; Clear the valid bit and the rest
			srawi	r8,r8,31					; Get -1 if steal index still in range
			slbie	r3							; Invalidate the in-use SLB entry
			and		r2,r2,r8					; Reset steal index when it should wrap
			isync								; 
			
			stw		r2,ppSegSteal(r12)			; Set the next slot to steal
;
;			We are now ready to stick the SLB entry in the SLB and mark it in use
;

hmsFreeSeg:	subi	r2,r7,1						; Adjust for skipped slb 0
			rldimi	r4,r7,0,58					; Copy in the SLB entry selector
			srd		r0,r0,r2					; Set bit mask for allocation
			rldicl	r5,r5,0,15					; Clean out the unsupported bits
			or		r6,r6,r0					; Turn on the allocation flag
			
			slbmte	r5,r4						; Make that SLB entry

			std		r6,validSegs(r12)			; Mark as valid
			mtmsrd	r11							; Restore the MSR
			isync
			blr									; Back to it...

			.align	5

hms32bit:	rlwinm	r8,r8,0,8,31				; Clean up the VSID
			rlwinm	r2,r4,4,28,31				; Isolate the segment we are setting
			lis		r0,0x8000					; Set bit 0
			rlwimi	r8,r9,28,1,3				; Insert the keys and N bit			
			srw		r0,r0,r2					; Get bit corresponding to SR
			addi	r7,r12,validSegs			; Point to the valid segment flags directly
		
			mtsrin	r8,r4						; Set the actual SR	
			isync								; Need to make sure this is done
		
hmsrupt:	lwarx	r6,0,r7						; Get and reserve the valid segment flags
			or		r6,r6,r0					; Show that SR is valid
			stwcx.	r6,0,r7						; Set the valid SR flags
			bne--	hmsrupt						; Had an interrupt, need to get flags again...

			blr									; Back to it...


;
;			This routine invalidates a segment register.
;

			.align	5
			.globl	EXT(hw_blow_seg)

LEXT(hw_blow_seg)

 			mfsprg	r10,2						; Get feature flags 
			mfsprg	r12,0						; Get the per_proc
			mtcrf	0x02,r10					; move pf64Bit and pfNoMSRirb to cr5 and 6
		
			addi	r7,r12,validSegs			; Point to the valid segment flags directly
			rlwinm	r9,r4,0,0,3					; Save low segment address and make sure it is clean
			
			bf--	pf64Bitb,hbs32bit			; Skip out if 32-bit...
			
			li		r0,1						; Prepare to set bit 0 (also to clear EE)
			mfmsr	r6							; Get current MSR
			li		r2,MASK(MSR_IR)|MASK(MSR_DR)	; Get the translation bits
			mtmsrd	r0,1						; Set only the EE bit to 0
			rlwinm	r6,r6,0,MSR_EE_BIT,MSR_EE_BIT	; See if EE bit is on
			mfmsr	r11							; Get the MSR right now, after disabling EE
			andc	r2,r11,r2					; Turn off translation now
			rldimi	r2,r0,63,0					; Get bit 64-bit turned on
			or		r11,r11,r6					; Turn on the EE bit if it was on
			mtmsrd	r2							; Make sure translation and EE are off and 64-bit is on
			isync								; Hang out a bit

			rldimi	r9,r3,32,0					; Insert the top part of the ESID
			
			slbie	r9							; Invalidate the associated SLB entry
			
			mtmsrd	r11							; Restore the MSR
			isync
			blr									; Back to it.

			.align	5

hbs32bit:	lwarx	r4,0,r7						; Get and reserve the valid segment flags
			rlwinm	r6,r9,4,28,31				; Convert segment to number
			lis		r2,0x8000					; Set up a mask
			srw		r2,r2,r6					; Make a mask
			and.	r0,r4,r2					; See if this is even valid
			li		r5,invalSpace				; Set the invalid address space VSID
			beqlr								; Leave if already invalid...
			
			mtsrin	r5,r9						; Slam the segment register
			isync								; Need to make sure this is done
		
hbsrupt:	andc	r4,r4,r2					; Clear the valid bit for this segment
			stwcx.	r4,0,r7						; Set the valid SR flags
			beqlr++								; Stored ok, no interrupt, time to leave...
			
			lwarx	r4,0,r7						; Get and reserve the valid segment flags again
			b		hbsrupt						; Try again...

;
;			This routine invadates the entire pmap segment cache
;
;			Translation is on, interrupts may or may not be enabled.
;

			.align	5
			.globl	EXT(invalidateSegs)

LEXT(invalidateSegs)

			la		r10,pmapCCtl(r3)			; Point to the segment cache control
			eqv		r2,r2,r2					; Get all foxes
			
isInv:		lwarx	r4,0,r10					; Get the segment cache control value
			rlwimi	r4,r2,0,0,15				; Slam in all invalid bits
			rlwinm.	r0,r4,0,pmapCCtlLckb,pmapCCtlLckb	; Is it already locked?
			bne--	isInv0						; Yes, try again...
			
			stwcx.	r4,0,r10					; Try to invalidate it
			bne--	isInv						; Someone else just stuffed it...
			blr									; Leave...
			

isInv0:		li		r4,lgKillResv				; Get reservation kill zone
			stwcx.	r4,0,r4						; Kill reservation

isInv1:		lwz		r4,pmapCCtl(r3)				; Get the segment cache control
			rlwinm.	r0,r4,0,pmapCCtlLckb,pmapCCtlLckb	; Is it already locked?
			bne--	isInv						; Nope...
			b		isInv1						; Still locked do it again...
			
;
;			This routine switches segment registers between kernel and user.
;			We have some assumptions and rules:
;				We are in the exception vectors
;				pf64Bitb is set up
;				R3 contains the MSR we going to
;				We can not use R4, R13, R20, R21, R29
;				R13 is the savearea
;				R29 has the per_proc
;
;			We return R3 as 0 if we did not switch between kernel and user
;			We also maintain and apply the user state key modifier used by VMM support;	
;			If we go to the kernel it is set to 0, otherwise it follows the bit 
;			in spcFlags.
;

			.align	5
			.globl	EXT(switchSegs)

LEXT(switchSegs)

			lwz		r22,ppInvSeg(r29)			; Get the ppInvSeg (force invalidate) and ppCurSeg (user or kernel segments indicator)
			lwz		r9,spcFlags(r29)			; Pick up the special user state flags
			rlwinm	r2,r3,MSR_PR_BIT+1,31,31	; Isolate the problem mode bit
			rlwinm	r3,r3,MSR_RI_BIT+1,31,31	; Isolate the recoverable interrupt bit
			lis		r8,hi16(EXT(kernel_pmap_phys))	; Assume kernel
			or		r2,r2,r3					; This will 1 if we will be using user segments
			li		r3,0						; Get a selection mask
			cmplw	r2,r22						; This will be EQ if same state and not ppInvSeg
			ori		r8,r8,lo16(EXT(kernel_pmap_phys))	; Assume kernel (bottom of address)
			sub		r3,r3,r2					; Form select mask - 0 if kernel, -1 if user
			la		r19,ppUserPmap(r29)			; Point to the current user pmap

;			The following line is an exercise of a generally unreadable but recompile-friendly programing practice
			rlwinm	r30,r9,userProtKeybit+1+(63-sgcVSKeyUsr),sgcVSKeyUsr-32,sgcVSKeyUsr-32	; Isolate the user state protection key 

			andc	r8,r8,r3					; Zero kernel pmap ptr if user, untouched otherwise
			and		r19,r19,r3					; Zero user pmap ptr if kernel, untouched otherwise
			and		r30,r30,r3					; Clear key modifier if kernel, leave otherwise
			or		r8,r8,r19					; Get the pointer to the pmap we are using

			beqlr								; We are staying in the same mode, do not touch segs...

			lwz		r28,0(r8)					; Get top half of pmap address
			lwz		r10,4(r8)					; Get bottom half

			stw		r2,ppInvSeg(r29)			; Clear request for invalidate and save ppCurSeg
			rlwinm	r28,r28,0,1,0				; Copy top to top
			stw		r30,ppMapFlags(r29)			; Set the key modifier
			rlwimi	r28,r10,0,0,31				; Insert bottom
			
			la		r10,pmapCCtl(r28)			; Point to the segment cache control
			la		r9,pmapSegCache(r28)		; Point to the segment cache

ssgLock:	lwarx	r15,0,r10					; Get and reserve the segment cache control
			rlwinm.	r0,r15,0,pmapCCtlLckb,pmapCCtlLckb	; Someone have the lock?
			ori		r16,r15,lo16(pmapCCtlLck)	; Set lock bit
			bne--	ssgLock0					; Yup, this is in use...

			stwcx.	r16,0,r10					; Try to set the lock
			bne--	ssgLock						; Did we get contention?
			
			not		r11,r15						; Invert the invalids to valids
			li		r17,0						; Set a mask for the SRs we are loading
			isync								; Make sure we are all caught up

			bf--	pf64Bitb,ssg32Enter			; If 32-bit, jump into it...
		
			li		r0,0						; Clear
			slbia								; Trash all SLB entries (except for entry 0 that is)
			li		r17,1						; Get SLB index to load (skip slb 0)
			oris	r0,r0,0x8000				; Get set for a mask
			b		ssg64Enter					; Start on a cache line...

			.align	5

ssgLock0:	li		r15,lgKillResv				; Killing field
			stwcx.	r15,0,r15					; Kill reservation

ssgLock1:	lwz		r15,pmapCCtl(r28)			; Get the segment cache controls
			rlwinm.	r15,r15,0,pmapCCtlLckb,pmapCCtlLckb	; Someone have the lock?
			beq++	ssgLock						; Yup, this is in use...
			b		ssgLock1					; Nope, try again...
;
;			This is the 32-bit address space switch code.
;			We take a reservation on the segment cache and walk through.
;			For each entry, we load the specified entries and remember which
;			we did with a mask.  Then, we figure out which segments should be
;			invalid and then see which actually are.  Then we load those with the
;			defined invalid VSID. 
;			Afterwards, we unlock the segment cache.
;

			.align	5

ssg32Enter:	cntlzw	r12,r11						; Find the next slot in use
			cmplwi	r12,pmapSegCacheUse			; See if we are done
			slwi	r14,r12,4					; Index to the cache slot
			lis		r0,0x8000					; Get set for a mask
			add		r14,r14,r9					; Point to the entry
		
			bge-	ssg32Done					; All done...
		
			lwz		r5,sgcESID+4(r14)			; Get the ESID part
			srw		r2,r0,r12					; Form a mask for the one we are loading
			lwz		r7,sgcVSID+4(r14)			; And get the VSID bottom

			andc	r11,r11,r2					; Clear the bit
			lwz		r6,sgcVSID(r14)				; And get the VSID top

			rlwinm	r2,r5,4,28,31				; Change the segment number to a number

			xor		r7,r7,r30					; Modify the key before we actually set it
			srw		r0,r0,r2					; Get a mask for the SR we are loading
			rlwinm	r8,r7,19,1,3				; Insert the keys and N bit			
			or		r17,r17,r0					; Remember the segment
			rlwimi	r8,r7,20,12,31				; Insert 4:23 the VSID
			rlwimi	r8,r6,20,8,11				; Get the last nybble of the SR contents			

			mtsrin	r8,r5						; Load the segment
			b		ssg32Enter					; Go enter the next...
			
			.align	5
			
ssg32Done:	lwz		r16,validSegs(r29)			; Get the valid SRs flags
			stw		r15,pmapCCtl(r28)			; Unlock the segment cache controls

			lis		r0,0x8000					; Get set for a mask
			li		r2,invalSpace				; Set the invalid address space VSID

			nop									; Align loop
			nop									; Align loop
			andc	r16,r16,r17					; Get list of SRs that were valid before but not now
			nop									; Align loop

ssg32Inval:	cntlzw	r18,r16						; Get the first one to invalidate
			cmplwi	r18,16						; Have we finished?
			srw		r22,r0,r18					; Get the mask bit
			rlwinm	r23,r18,28,0,3				; Get the segment register we need
			andc	r16,r16,r22					; Get rid of the guy we just did
			bge		ssg32Really					; Yes, we are really done now...

			mtsrin	r2,r23						; Invalidate the SR
			b		ssg32Inval					; Do the next...
			
			.align	5

ssg32Really:
			stw		r17,validSegs(r29)			; Set the valid SR flags
			li		r3,1						; Set kernel/user transition
			blr

;
;			This is the 64-bit address space switch code.
;			First we blow away all of the SLB entries.
;			Walk through,
;			loading the SLB.  Afterwards, we release the cache lock
;
;			Note that because we have to treat SLBE 0 specially, we do not ever use it...
;			Its a performance thing...
;

			.align	5

ssg64Enter:	cntlzw	r12,r11						; Find the next slot in use
			cmplwi	r12,pmapSegCacheUse			; See if we are done
			slwi	r14,r12,4					; Index to the cache slot
			srw		r16,r0,r12					; Form a mask for the one we are loading
			add		r14,r14,r9					; Point to the entry
			andc	r11,r11,r16					; Clear the bit
			bge--	ssg64Done					; All done...

			ld		r5,sgcESID(r14)				; Get the ESID part
			ld		r6,sgcVSID(r14)				; And get the VSID part
			oris	r5,r5,0x0800				; Turn on the valid bit
			or		r5,r5,r17					; Insert the SLB slot
			xor		r6,r6,r30					; Modify the key before we actually set it
			addi	r17,r17,1					; Bump to the next slot
			slbmte	r6,r5						; Make that SLB entry
			b		ssg64Enter					; Go enter the next...
			
			.align	5
			
ssg64Done:	stw		r15,pmapCCtl(r28)			; Unlock the segment cache controls

			eqv		r16,r16,r16					; Load up with all foxes
			subfic	r17,r17,64					; Get the number of 1 bits we need

			sld		r16,r16,r17					; Get a mask for the used SLB entries
			li		r3,1						; Set kernel/user transition
			std		r16,validSegs(r29)			; Set the valid SR flags
			blr

;
;			mapSetUp - this function sets initial state for all mapping functions.
;			We turn off all translations (physical), disable interruptions, and 
;			enter 64-bit mode if applicable.
;
;			We also return the original MSR in r11, the feature flags in R12,
;			and CR6 set up so we can do easy branches for 64-bit
;

			.align	5
			.globl	EXT(mapSetUp)

LEXT(mapSetUp)

			lis		r0,hi16(MASK(MSR_VEC))		; Get the vector mask
 			mfsprg	r12,2						; Get feature flags 
 			ori		r0,r0,lo16(MASK(MSR_FP))	; Get the FP as well
			mtcrf	0x04,r12					; move pf64Bit and pfNoMSRirb to cr5 and 6
			mfmsr	r11							; Save the MSR 
			mtcrf	0x02,r12					; move pf64Bit and pfNoMSRirb to cr5 and 6
			andc	r11,r11,r0					; Clear VEC and FP for good
			ori		r0,r0,lo16(MASK(MSR_EE)|MASK(MSR_DR)|MASK(MSR_IR))	; Get rid of EE, IR, and DR
			li		r2,1						; Prepare for 64 bit
			andc	r0,r11,r0					; Clear the rest
			bt		pfNoMSRirb,msuNoMSR			; No MSR...
			bt++	pf64Bitb,msuSF				; skip if 64-bit (only they take the hint)

			mtmsr	r0							; Translation and all off
			isync								; Toss prefetch
			blr									; Return...

			.align	5

msuSF:		rldimi	r0,r2,63,MSR_SF_BIT			; set SF bit (bit 0)
			mtmsrd	r0							; set 64-bit mode, turn off EE, DR, and IR
			isync								; synchronize
			blr									; Return...

			.align	5

msuNoMSR:	mr		r2,r3						; Save R3 across call
			mr		r3,r0						; Get the new MSR value
			li		r0,loadMSR					; Get the MSR setter SC
			sc									; Set it
			mr		r3,r2						; Restore R3
			blr									; Go back all set up...
			

;
;			Find the physent based on a physical page and try to lock it (but not too hard) 
;			Note that this table always has an entry that with a 0 table pointer at the end 
;			
;			R3 contains ppnum on entry
;			R3 is 0 if no entry was found
;			R3 is physent if found
;			cr0_eq is true if lock was obtained or there was no entry to lock
;			cr0_eq is false of there was an entry and it was locked
;	

			.align	5
			
mapFindPhyTry:	
			lis		r9,hi16(EXT(pmap_mem_regions))		; Point to the start of the region table
			mr		r2,r3						; Save our target
			ori		r9,r9,lo16(EXT(pmap_mem_regions))	; Point to the start of the region table			

mapFindPhz:	lwz		r3,mrPhysTab(r9)			; Get the actual table address
			lwz		r5,mrStart(r9)				; Get start of table entry
			lwz		r0,mrEnd(r9)				; Get end of table entry
			addi	r9,r9,mrSize				; Point to the next slot
			cmplwi	cr2,r3,0					; Are we at the end of the table?
			cmplw	r2,r5						; See if we are in this table
			cmplw	cr1,r2,r0					; Check end also
			sub		r4,r2,r5					; Calculate index to physical entry
			beq--	cr2,mapFindNo				; Leave if we did not find an entry...
			cror	cr0_lt,cr0_lt,cr1_gt		; Set CR0_LT if it is NOT this entry
			slwi	r4,r4,3						; Get offset to physical entry

			blt--	mapFindPhz					; Did not find it...
			
			add		r3,r3,r4					; Point right to the slot
	
mapFindOv:	lwz		r2,0(r3)					; Get the lock contents right now
			rlwinm.	r0,r2,0,0,0					; Is it locked?
			bnelr--								; Yes it is...
			
			lwarx	r2,0,r3						; Get the lock
			rlwinm.	r0,r2,0,0,0					; Is it locked?
			oris	r0,r2,0x8000				; Set the lock bit
			bne--	mapFindKl					; It is locked, go get rid of reservation and leave...
			stwcx.	r0,0,r3						; Try to stuff it back...
			bne--	mapFindOv					; Collision, try again...
			isync								; Clear any speculations
			blr									; Leave...

mapFindKl:	li		r2,lgKillResv				; Killing field
			stwcx.	r2,0,r2						; Trash reservation...
			crclr	cr0_eq						; Make sure we do not think we got the lock
			blr									; Leave...

mapFindNo:	crset	cr0_eq						; Make sure that we set this
			li		r3,0						; Show that we did not find it
			blr									; Leave...			
;
;			pmapCacheLookup - This function will look up an entry in the pmap segment cache.
;
;			How the pmap cache lookup works:
;
;			We use a combination of three things: a mask of valid entries, a sub-tag, and the
;			ESID (aka the "tag").  The mask indicates which of the cache slots actually contain
;			an entry.  The sub-tag is a 16 entry 4 bit array that contains the low order 4 bits
;			of the ESID, bits 32:36 of the effective for 64-bit and 0:3 for 32-bit.  The cache
;			entry contains the full 36 bit ESID.
;
;			The purpose of the sub-tag is to limit the number of searches necessary when looking
;			for an existing cache entry.  Because there are 16 slots in the cache, we could end up
;			searching all 16 if an match is not found.  
;
;			Essentially, we will search only the slots that have a valid entry and whose sub-tag
;			matches. More than likely, we will eliminate almost all of the searches.
;		
;			Inputs:
;				R3 = pmap
;				R4 = ESID high half
;				R5 = ESID low half
;
;			Outputs:
;				R3 = pmap cache slot if found, 0 if not
;				R10 = pmapCCtl address
;				R11 = pmapCCtl image
;				pmapCCtl locked on exit
;

			.align	5

pmapCacheLookup:		
			la		r10,pmapCCtl(r3)			; Point to the segment cache control

pmapCacheLookuq:		
			lwarx	r11,0,r10					; Get the segment cache control value
			rlwinm.	r0,r11,0,pmapCCtlLckb,pmapCCtlLckb	; Is it already locked?
			ori		r0,r11,lo16(pmapCCtlLck)	; Turn on the lock bit
			bne--	pmapCacheLookur				; Nope...
			stwcx.	r0,0,r10					; Try to take the lock
			bne--	pmapCacheLookuq				; Someone else just stuffed it, try again...

			isync								; Make sure we get reservation first
			lwz		r9,pmapSCSubTag(r3)			; Get the high part of the sub-tag
			rlwimi	r5,r5,28,4,7				; Copy sub-tag just to right of itself (XX------)
			lwz		r10,pmapSCSubTag+4(r3)		; And the bottom half
			rlwimi	r5,r5,24,8,15				; Copy doubled sub-tag to right of itself (XXXX----)
			lis		r8,0x8888					; Get some eights
			rlwimi	r5,r5,16,16,31				; Copy quadrupled sub-tags to the right
			ori		r8,r8,0x8888				; Fill the rest with eights

			eqv		r10,r10,r5					; Get 0xF where we hit in bottom half
			eqv		r9,r9,r5					; Get 0xF where we hit in top half
			
			rlwinm	r2,r10,1,0,30				; Shift over 1
			rlwinm	r0,r9,1,0,30				; Shift over 1
			and		r2,r2,r10					; AND the even/odd pair into the even
			and		r0,r0,r9					; AND the even/odd pair into the even
			rlwinm	r10,r2,2,0,28				; Shift over 2
			rlwinm	r9,r0,2,0,28				; Shift over 2
			and		r10,r2,r10					; AND the even of the ANDed pairs giving the AND of all 4 bits in 0, 4, ...
			and		r9,r0,r9					; AND the even of the ANDed pairs giving the AND of all 4 bits in 0, 4, ...
			
			and		r10,r10,r8					; Clear out extras
			and		r9,r9,r8					; Clear out extras
			
			rlwinm	r0,r10,3,1,28				; Slide adjacent next to each other
			rlwinm	r2,r9,3,1,28				; Slide adjacent next to each other
			or		r10,r0,r10					; Merge them
			or		r9,r2,r9					; Merge them
			rlwinm	r0,r10,6,2,26				; Slide adjacent pairs next to each other
			rlwinm	r2,r9,6,2,26				; Slide adjacent pairs next to each other
			or		r10,r0,r10					; Merge them
			or		r9,r2,r9					; Merge them
			rlwimi	r10,r10,12,4,7				; Stick in the low-order adjacent quad
			rlwimi	r9,r9,12,4,7				; Stick in the low-order adjacent quad
			not		r6,r11						; Turn invalid into valid
			rlwimi	r9,r10,24,8,15				; Merge in the adjacent octs giving a hit mask
			
			la		r10,pmapSegCache(r3)		; Point at the cache slots
			and.	r6,r9,r6					; Get mask of valid and hit
			li		r0,0						; Clear
			li		r3,0						; Assume not found
			oris	r0,r0,0x8000				; Start a mask
			beqlr++								; Leave, should usually be no hits...
			
pclNextEnt:	cntlzw	r5,r6						; Find an in use one
			cmplwi	cr1,r5,pmapSegCacheUse		; Did we find one?
			rlwinm	r7,r5,4,0,27				; Index to the cache entry
			srw		r2,r0,r5					; Get validity mask bit
			add		r7,r7,r10					; Point to the cache slot
			andc	r6,r6,r2					; Clear the validity bit we just tried
			bgelr--	cr1							; Leave if there are no more to check...
			
			lwz		r5,sgcESID(r7)				; Get the top half
			
			cmplw	r5,r4						; Only need to check top because sub-tag is the entire other half
			
			bne++	pclNextEnt					; Nope, try again...

			mr		r3,r7						; Point to the slot
			blr									; Leave....

			.align	5

pmapCacheLookur:
			li		r11,lgKillResv				; The killing spot
			stwcx.	r11,0,r11					; Kill the reservation
			
pmapCacheLookus:		
			lwz		r11,pmapCCtl(r3)			; Get the segment cache control
			rlwinm.	r0,r11,0,pmapCCtlLckb,pmapCCtlLckb	; Is it already locked?
			beq++	pmapCacheLookup				; Nope...
			b		pmapCacheLookus				; Yup, keep waiting...

			


;
;			This routine, given a mapping, will find and lock the PTEG
;			If mpPte does not point to a PTE (checked before and after lock), it will unlock the
;			PTEG and return.  In this case we will have undefined in R4
;			and the low 12 bits of mpVAddr valid in R5.  R3 will contain 0.
;
;			If the mapping is still valid, we will invalidate the PTE and merge
;			the RC bits into the physent and also save them into the mapping.
;
;			We then return with R3 pointing to the PTE slot, R4 is the
;			top of the PTE and R5 is the bottom.  R6 contains the PCA.
;			R7 points to the PCA entry.
;
;			Note that we should NEVER be called on a block or special mapping.
;			We could do many bad things.
;

			.align	5

mapInvPte32:
			lwz		r0,mpPte(r31)				; Grab the PTE offset
			mfsdr1	r7							; Get the pointer to the hash table
			lwz		r5,mpVAddr+4(r31)			; Grab the virtual address
			rlwinm	r10,r7,0,0,15				; Clean up the hash table base
			andi.	r3,r0,mpHValid				; Is there a possible PTE?
			srwi	r7,r0,4						; Convert to PCA units
			rlwinm	r7,r7,0,0,29				; Clean up PCA offset
			mflr	r2							; Save the return
			subfic	r7,r7,-4					; Convert to -4 based negative index
			add		r7,r10,r7					; Point to the PCA directly
			beqlr--								; There was no PTE to start with...
			
			bl		mapLockPteg					; Lock the PTEG

			lwz		r0,mpPte(r31)				; Grab the PTE offset
			mtlr	r2							; Restore the LR
			andi.	r3,r0,mpHValid				; Is there a possible PTE?
			beq-	mIPUnlock					; There is no PTE, someone took it so just unlock and leave...

			rlwinm	r3,r0,0,0,30				; Clear the valid bit
			add		r3,r3,r10					; Point to actual PTE
			lwz		r4,0(r3)					; Get the top of the PTE
			
			li		r8,tlbieLock				; Get the TLBIE lock
			rlwinm	r0,r4,0,1,31				; Clear the valid bit
			stw		r0,0(r3)					; Invalidate the PTE

			sync								; Make sure everyone sees the invalidate
			
mITLBIE32:	lwarx	r0,0,r8						; Get the TLBIE lock 
			mfsprg	r2,2						; Get feature flags 
			mr.		r0,r0						; Is it locked? 
			li		r0,1						; Get our lock word 
			bne-	mITLBIE32					; It is locked, go wait...
			
			stwcx.	r0,0,r8						; Try to get it
			bne-	mITLBIE32					; We was beat...
			
			rlwinm.	r0,r2,0,pfSMPcapb,pfSMPcapb	; Can this be an MP box?
			li		r0,0						; Lock clear value 

			tlbie	r5							; Invalidate it everywhere 

			beq-	mINoTS32					; Can not have MP on this machine...
			
			eieio								; Make sure that the tlbie happens first 
			tlbsync								; Wait for everyone to catch up 
			sync								; Make sure of it all
			
mINoTS32:	
			stw		r0,tlbieLock(0)				; Clear the tlbie lock
			
			lwz		r5,4(r3)					; Get the real part
			srwi	r10,r5,12					; Change physical address to a ppnum

mINmerge:	lbz		r11,mpFlags+1(r31)			; Get the offset to the physical entry table
			lwz		r0,mpVAddr+4(r31)			; Get the flags part of the field
			lis		r8,hi16(EXT(pmap_mem_regions))	; Get the top of the region table
			ori		r8,r8,lo16(EXT(pmap_mem_regions))	; Get the bottom of the region table
			rlwinm	r11,r11,2,0,29				; Change index into byte offset
			add		r11,r11,r8					; Point to the bank table
			lwz		r2,mrPhysTab(r11)			; Get the physical table bank pointer
			lwz		r11,mrStart(r11)			; Get the start of bank
			rlwimi	r0,r5,0,mpRb-32,mpCb-32		; Copy in the RC
			addi	r2,r2,4						; Offset to last half of field
			stw		r0,mpVAddr+4(r31)			; Set the new RC into the field
			sub		r11,r10,r11					; Get the index into the table
			rlwinm	r11,r11,3,0,28				; Get offset to the physent


mImrgRC:	lwarx	r10,r11,r2					; Get the master RC
			rlwinm	r0,r5,27,ppRb-32,ppCb-32	; Position the new RC
			or		r0,r0,r10					; Merge in the new RC
			stwcx.	r0,r11,r2					; Try to stick it back
			bne--	mImrgRC						; Try again if we collided...
			
			blr									; Leave with the PCA still locked up...

mIPUnlock:	eieio								; Make sure all updates come first
				
			stw		r6,0(r7)					; Unlock
			blr

;
;			64-bit version
;
			.align	5

mapInvPte64:
			lwz		r0,mpPte(r31)				; Grab the PTE offset
			ld		r5,mpVAddr(r31)				; Grab the virtual address
			mfsdr1	r7							; Get the pointer to the hash table
			rldicr	r10,r7,0,45					; Clean up the hash table base
			andi.	r3,r0,mpHValid				; Is there a possible PTE?
			srdi	r7,r0,5						; Convert to PCA units
			rldicr	r7,r7,0,61					; Clean up PCA
			subfic	r7,r7,-4					; Convert to -4 based negative index
			mflr	r2							; Save the return
			add		r7,r10,r7					; Point to the PCA directly
			beqlr--								; There was no PTE to start with...
			
			bl		mapLockPteg					; Lock the PTEG
			
			lwz		r0,mpPte(r31)				; Grab the PTE offset again
			mtlr	r2							; Restore the LR
			andi.	r3,r0,mpHValid				; Is there a possible PTE?
			beq--	mIPUnlock					; There is no PTE, someone took it so just unlock and leave...

			rlwinm	r3,r0,0,0,30				; Clear the valid bit
			add		r3,r3,r10					; Point to the actual PTE
			ld		r4,0(r3)					; Get the top of the PTE

			li		r8,tlbieLock				; Get the TLBIE lock
			rldicr	r0,r4,0,62					; Clear the valid bit
			std		r0,0(r3)					; Invalidate the PTE
			
			rldicr	r2,r4,16,35					; Shift the AVPN over to match VPN
			sync								; Make sure everyone sees the invalidate
			rldimi	r2,r5,0,36					; Cram in the page portion of the EA
			
mITLBIE64:	lwarx	r0,0,r8						; Get the TLBIE lock 
			mr.		r0,r0						; Is it locked? 
			li		r0,1						; Get our lock word 
			bne--	mITLBIE64a					; It is locked, toss reservation and wait...
			
			stwcx.	r0,0,r8						; Try to get it
			bne--	mITLBIE64					; We was beat...

			rldicl	r2,r2,0,16					; Clear bits 0:15 because we are under orders
			
			li		r0,0						; Lock clear value 

			tlbie	r2							; Invalidate it everywhere 

			eieio								; Make sure that the tlbie happens first 
			tlbsync								; Wait for everyone to catch up 
			isync								
			ptesync								; Wait for quiet again
			
mINoTS64:	
			stw		r0,tlbieLock(0)				; Clear the tlbie lock
			
			sync								; Make sure of it all

			ld		r5,8(r3)					; Get the real part
			srdi	r10,r5,12					; Change physical address to a ppnum
			b		mINmerge					; Join the common 32-64-bit code...

mITLBIE64a:	li		r5,lgKillResv				; Killing field
			stwcx.	r5,0,r5						; Kill reservation
			
mITLBIE64b:	lwz		r0,0(r8)					; Get the TLBIE lock
			mr.		r0,r0						; Is it locked?
			beq++	mITLBIE64					; Nope, try again...
			b		mITLBIE64b					; Yup, wait for it...

;
;			mapLockPteg - Locks a PTEG
;			R7 points to PCA entry
;			R6 contains PCA on return
;
;

			.align	5
			
mapLockPteg:
			lwarx	r6,0,r7						; Pick up the PCA
			rlwinm.	r0,r6,0,PCAlockb,PCAlockb	; Is the PTEG locked?
			ori		r0,r6,PCAlock				; Set the lock bit
			bne--	mLSkill						; It is locked...
			
			stwcx.	r0,0,r7						; Try to lock the PTEG
			bne--	mapLockPteg					; We collided...
			
			isync								; Nostradamus lied
			blr									; Leave...
				
mLSkill:	li		r6,lgKillResv				; Get killing field
			stwcx.	r6,0,r6						; Kill it

mapLockPteh:
			lwz		r6,0(r7)					; Pick up the PCA
			rlwinm.	r0,r6,0,PCAlockb,PCAlockb	; Is the PTEG locked?
			beq++	mapLockPteg					; Nope, try again...
			b		mapLockPteh					; Yes, wait for it...
			

;
;			The mapSelSlot function selects a PTEG slot to use. As input, it expects R6 
;			to contain the PCA.  When it returns, R3 contains 0 if an unoccupied slot was
;			selected, 1 if it stole a non-block PTE, or 2 if it stole a block mapped PTE.
;			R4 returns the slot index.
;
;			CR7 also indicates that we have a block mapping
;
;			The PTEG allocation controls are a bit map of the state of the PTEG. 
;			PCAfree indicates that the PTE slot is empty. 
;			PCAauto means that it comes from an autogen area.  These
;			guys do not keep track of reference and change and are actually "wired".
;			They are easy to maintain. PCAsteal
;			is a sliding position mask used to "randomize" PTE slot stealing.  All 4 of these
;			fields fit in a single word and are loaded and stored under control of the
;			PTEG control area lock (PCAlock).
;
;			Note that PCAauto does not contribute to the steal calculations at all.  Originally
;			it did, autogens were second in priority.  This can result in a pathalogical
;			case where an instruction can not make forward progress, or one PTE slot
;			thrashes.
;
;			Note that the PCA must be locked when we get here.
;
;			Physically, the fields are arranged:
;				0: PCAfree
;				1: PCAsteal
;				2: PCAauto
;				3: PCAmisc
;				
;
;			At entry, R6 contains new unlocked PCA image (real PCA is locked and untouched)
;
;			At exit:
;
;			R3 = 0 - no steal
;			R3 = 1 - steal regular
;			R3 = 2 - steal autogen
;			R4 contains slot number
;			R6 contains updated PCA image
;

			.align	5
>>>>>>> origin/10.3
			
			srw.	r9,r0,r10					; Get bit corresponding to first free one in second word
			cntlzw	r11,r6						; Get first free field in third word
			andc	r5,r5,r9					; Turn it off
			bne		malcfnd1					; Found one...
			
			srw.	r9,r0,r11					; Get bit corresponding to first free one in third word
			cntlzw	r10,r7						; Get first free field in fourth word
			andc	r6,r6,r9					; Turn it off
			bne		malcfnd2					; Found one...
			
			srw.	r9,r0,r10					; Get bit corresponding to first free one in second word
			li		r3,0						; Assume abject failure
			andc	r7,r7,r9					; Turn it off
			beqlr								; There are none any left...
			
			addi	r3,r10,96					; Set the correct bit number
			stw		r7,mbfree+12(r12)			; Actually allocate the slot
			
mapafin:	or		r4,r4,r5					; Merge the first two allocation maps
			or		r6,r6,r7					; Then the last two
			or.		r4,r4,r6					; Merge both halves
			bnelr+								; Return if some left for next time...
			
			neg		r3,r3						; Indicate we just allocated the last one
			blr									; Leave...
			
malcfnd0:	stw		r4,mbfree(r12)				; Actually allocate the slot
			mr		r3,r8						; Set the correct bit number
			b		mapafin						; Exit now...
			
malcfnd1:	stw		r5,mbfree+4(r12)			; Actually allocate the slot
			addi	r3,r10,32					; Set the correct bit number
			b		mapafin						; Exit now...
			
malcfnd2:	stw		r6,mbfree+8(r12)			; Actually allocate the slot
			addi	r3,r11,64					; Set the correct bit number
			b		mapafin						; Exit now...
			

/*
 * Log out all memory usage
 */

			.align	5
			.globl	EXT(logmem)

LEXT(logmem)

			mfmsr	r2							; Get the MSR	
			lis		r10,hi16(EXT(DebugWork))		; High part of area
			lis		r12,hi16(EXT(mem_actual))	; High part of actual
			andi.	r0,r10,0x7FCF				; Interrupts and translation off
			ori		r10,r10,lo16(EXT(DebugWork))	; Get the entry
			mtmsr	r0							; Turn stuff off
			ori		r12,r12,lo16(EXT(mem_actual))	; Get the actual
			li		r0,1						; Get a one
	
			isync

			stw		r0,4(r10)					; Force logging off
			lwz		r0,0(r12)					; Get the end of memory
			
			lis		r12,hi16(EXT(mem_size))		; High part of defined memory
			ori		r12,r12,lo16(EXT(mem_size))	; Low part of defined memory
			lwz		r12,0(r12)					; Make it end of defined
			
			cmplw	r0,r12						; Is there room for the data?
			ble-	logmemexit					; No, do not even try...

			stw		r12,0(r12)					; Set defined memory size
			stw		r0,4(r12)					; Set the actual amount of memory
			
			lis		r3,hi16(EXT(hash_table_base))	; Hash table address
			lis		r4,hi16(EXT(hash_table_size))	; Hash table size
			lis		r5,hi16(EXT(pmap_mem_regions))	; Memory regions
			lis		r6,hi16(EXT(mapCtl))		; Mappings
			ori		r3,r3,lo16(EXT(hash_table_base))	
			ori		r4,r4,lo16(EXT(hash_table_size))	
			ori		r5,r5,lo16(EXT(pmap_mem_regions))	
			ori		r6,r6,lo16(EXT(mapCtl))	
			lwz		r3,0(r3)
			lwz		r4,0(r4)
			lwz		r5,4(r5)					; Get the pointer to the phys_ent table
			lwz		r6,0(r6)					; Get the pointer to the current mapping block
			stw		r3,8(r12)					; Save the hash table address
			stw		r4,12(r12)					; Save the hash table size
			stw		r5,16(r12)					; Save the physent pointer
			stw		r6,20(r12)					; Save the mappings
			
			addi	r11,r12,0x1000				; Point to area to move hash table and PCA
			
			add		r4,r4,r4					; Double size for both
			
copyhash:	lwz		r7,0(r3)					; Copy both of them
			lwz		r8,4(r3)
			lwz		r9,8(r3)
			lwz		r10,12(r3)
			subic.	r4,r4,0x10
			addi	r3,r3,0x10
			stw		r7,0(r11)
			stw		r8,4(r11)
			stw		r9,8(r11)
			stw		r10,12(r11)
			addi	r11,r11,0x10
			bgt+	copyhash
			
			rlwinm	r4,r12,20,12,31				; Get number of phys_ents

copyphys:	lwz		r7,0(r5)					; Copy physents
			lwz		r8,4(r5)
			subic.	r4,r4,1
			addi	r5,r5,8
			stw		r7,0(r11)
			stw		r8,4(r11)
			addi	r11,r11,8
			bgt+	copyphys
			
			addi	r11,r11,4095				; Round up to next page
			rlwinm	r11,r11,0,0,19

			lwz		r4,4(r6)					; Get the size of the mapping area
			
copymaps:	lwz		r7,0(r6)					; Copy the mappings
			lwz		r8,4(r6)
			lwz		r9,8(r6)
			lwz		r10,12(r6)
			subic.	r4,r4,0x10
			addi	r6,r6,0x10
			stw		r7,0(r11)
			stw		r8,4(r11)
			stw		r9,8(r11)
			stw		r10,12(r11)
			addi	r11,r11,0x10
			bgt+	copymaps

			sub		r11,r11,r12					; Get the total length we saved
			stw		r11,24(r12)					; Save the size
			
logmemexit:	mtmsr	r2							; Back to normal
			li		r3,0
			isync
			blr


