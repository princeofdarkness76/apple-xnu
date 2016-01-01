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
 * @OSF_FREE_COPYRIGHT@
 */
/*
 * @APPLE_FREE_COPYRIGHT@
 */

/*
 *	Author: Bill Angell, Apple
 *	Date:	9/auht-aught
 *
 * Random diagnostics
 */


#include <kern/machine.h>
#include <kern/processor.h>
#include <mach/machine.h>
#include <mach/processor_info.h>
#include <mach/mach_types.h>
#include <mach/boolean.h>
#include <kern/thread.h>
#include <kern/task.h>
#include <mach/vm_param.h>
#include <vm/vm_kern.h>
#include <vm/vm_map.h>
#include <vm/vm_page.h>
#include <vm/pmap.h>
#include <ppc/exception.h>
#include <ppc/Firmware.h>
#include <ppc/low_trace.h>
#include <ppc/db_low_trace.h>
#include <ppc/mappings.h>
#include <ppc/pmap.h>
#include <ppc/mem.h>
#include <ppc/pmap_internals.h>
#include <ppc/savearea.h>
#include <ppc/Diagnostics.h>
#include <ppc/machine_cpu.h>
#include <pexpert/pexpert.h>
#include <ppc/POWERMAC/video_console.h>
#include <ppc/trap.h>

extern struct vc_info vinfo;
extern uint32_t warFlags;
#define warDisMBpoff	0x80000000

kern_return_t testPerfTrap(int trapno, struct savearea *ss, 
	unsigned int dsisr, unsigned int dar);


int diagCall(struct savearea *save) {

	union {
		unsigned long long tbase;
		unsigned int tb[2];
	} ttt, adj;
	natural_t tbu, tbu2, tbl;
	struct per_proc_info *per_proc;					/* Area for my per_proc address */
<<<<<<< HEAD
	int cpu;
	unsigned int tstrt, tend, temp, temp2;
=======
	int cpu, ret, subc;
	unsigned int tstrt, tend, temp, temp2, oldwar;
	addr64_t src, snk;
	uint64_t scom, hid1, hid4, srrwrk, stat;
	scomcomm sarea;
>>>>>>> origin/10.3

	if(!(dgWork.dgFlags & enaDiagSCs)) return 0;	/* If not enabled, cause an exception */

	switch(save->save_r3) {							/* Select the routine */
	
/*
 *		Adjust the timebase for drift recovery testing
 */
		case dgAdjTB:								/* Adjust the timebase */

			adj.tb[0] = 0;							/* Clear high part */
			adj.tb[1] = save->save_r4;				/* Set low order */
			if(adj.tb[1] & 0x80000000) adj.tb[0] = 0xFFFFFFFF;	/* Propagate sign bit */
						
			do {									/* Read current time */
				asm volatile("	mftbu %0" : "=r" (tbu));
				asm volatile("	mftb %0" : "=r" (tbl));
				asm volatile("	mftbu %0" : "=r" (tbu2));
			} while (tbu != tbu2);
			
			ttt.tb[0] = tbu;						/* Set high */
			ttt.tb[1] = tbl;						/* Set low */
			
			ttt.tbase = ttt.tbase + adj.tbase;		/* Increment or decrement the TB */
			
			tbu = ttt.tb[0];						/* Save in regular variable */
			tbl = ttt.tb[1];						/* Save in regular variable */

			mttb(0);								/* Set low to keep from ticking */
			mttbu(tbu);								/* Set adjusted high */
			mttb(tbl);								/* Set adjusted low */
			
			return -1;								/* Return no AST checking... */
			
/*
 *		Return physical address of a page
 */
		case dgLRA:
		
			save->save_r3 = pmap_extract(current_act()->map->pmap, save->save_r4);	/* Get read address */
			
			return -1;								/* Return no AST checking... */
			
/*
 *		Copy physical to virtual
 */
		case dgpcpy:
		
#if 0
			save->save_r3 = copyp2v(save->save_r4, save->save_r5, save->save_r6);	/* Copy the physical page */
#endif			
			return 1;								/* Return and check for ASTs... */
			
			
/*
 *		Soft reset processor
 */
		case dgreset:
		
			cpu = save->save_r4;					/* Get the requested CPU number */
			
			if(cpu >= NCPUS) {						/* Check for bogus cpu */
				save->save_r3 = KERN_FAILURE;		/* Set failure */
				return 1;
			}
		
			if(!machine_slot[cpu].running) return KERN_FAILURE;	/* It is not running */	

			per_proc = &per_proc_info[cpu];			/* Point to the processor */
			
			(void)PE_cpu_start(per_proc->cpu_id, 
						per_proc->start_paddr, (vm_offset_t)per_proc);
			
			save->save_r3 = KERN_SUCCESS;			/* Set scuuess */

			return 1;								/* Return and check for ASTs... */

/*
 *		Force cache flush
 */
		case dgFlush:
		
#if 1
			cacheInit();							/* Blow cache */
#else
			asm volatile("	mftb %0" : "=r" (tstrt));
			tend = tstrt;			
			while((tend - tstrt) < 0x000A2837) {
				asm volatile("	mftb %0" : "=r" (tend));
			}

#endif
			return 1;								/* Return and check for ASTs... */

/*
 *		various hack tests
 */
		case dgtest:
		
			if(save->save_r4) perfTrapHook = testPerfTrap;
			else perfTrapHook = 0;

			return 1;								/* Return and check for ASTs... */
			
		

/*
 *		Create a physical block map into the current task
 *		Don't bother to check for any errors.
 *		parms - vaddr, paddr, size, prot, attributes
 */
		case dgBMphys:
		
			pmap_map_block(current_act()->map->pmap, save->save_r4, save->save_r5, save->save_r6,	/* Map in the block */
				save->save_r7, save->save_r8, 0);

			return 1;								/* Return and check for ASTs... */
		

/*
 *		Remove any mapping from the current task
 *		Don't bother to check for any errors.
 *		parms - vaddr
 */
		case dgUnMap:
		
			(void)mapping_remove(current_act()->map->pmap, save->save_r4);	/* Remove mapping */
			return 1;								/* Return and check for ASTs... */
	
			
/*
 *		Allows direct control of alignment handling.
 *
 *		The bottom two bits of the parameter are used to set the two control bits:
 *		0b00 - !trapUnalignbit - !notifyUnalignbit - default - instruction is emulated
 *		0b01 - !trapUnalignbit -  notifyUnalignbit - emulation is done, but traps afterwards
 *		0b10 -  trapUnalignbit - !notifyUnalignbit - no emulation - causes exception
 *		0b11 -  trapUnalignbit -  notifyUnalignbit - no emulation - causes exception
 */
		case dgAlign:
		
			temp = current_act()->mact.specFlags;	/* Save the old values */
			
			temp = ((current_act()->mact.specFlags >> (31 - trapUnalignbit - 1)) 	/* Reformat them to pass back */
				| (current_act()->mact.specFlags >> (31 - notifyUnalignbit))) & 3;
				
			temp2 = ((save->save_r4 << (31 - trapUnalignbit - 1)) & trapUnalign)	/* Move parms into flag format */
				| ((save->save_r4 << (31 - notifyUnalignbit)) & notifyUnalign);

			current_act()->mact.specFlags &= ~(trapUnalign | notifyUnalign);		/* Clean the old ones */
			current_act()->mact.specFlags |= temp2;									/* Set the new ones */
			
			per_proc_info[cpu_number()].spcFlags = current_act()->mact.specFlags;
			
			save->save_r3 = temp;
			
			return 1;								/* Return and check for ASTs... */
			
/*
 *		Return info for boot screen
 */
		case dgBootScreen:
			
#if 0
			ml_set_interrupts_enabled(1);
			(void)copyout((char *)&vinfo, (char *)save->save_r4, sizeof(struct vc_info));	/* Copy out the video info */
			ml_set_interrupts_enabled(0);
#endif
			return 1;								/* Return and check for ASTs... */
			
		
<<<<<<< HEAD
=======
			setPmon(save->save_r4, save->save_r5);	/* Go load up MMCR0 and MMCR1 */
			return -1;								/* Regurn and don't check for ASTs */

/*
 *		Map a page
 *		Don't bother to check for any errors.
 *		parms - vaddr, paddr, prot, attributes
 */
		case dgMapPage:
					
			(void)mapping_map(current_act()->map->pmap, /* Map in the page */ 
				(addr64_t)(((save->save_r5 & 0xFFFFFFFF) << 32) | (save->save_r5 & 0xFFFFFFFF)), save->save_r6, 0, 1, VM_PROT_READ|VM_PROT_WRITE);

			return -1;								/* Return and check for ASTs... */
		
/*
 *		SCOM interface
 *		parms - pointer to scomcomm
 */
		case dgScom:
					
			ret = copyin((unsigned int)(save->save_r4), &sarea, sizeof(scomcomm));	/* Get the data */
			if(ret) return 0;						/* Copyin failed - return an exception */
			
			sarea.scomstat = 0xFFFFFFFFFFFFFFFFULL;	/* Clear status */
			cpu = cpu_number();						/* Get us */
			
			if((sarea.scomcpu < NCPUS) && machine_slot[sarea.scomcpu].running) {
				if(sarea.scomcpu == cpu) {			/* Is it us? */
					if(sarea.scomfunc) {			/* Are we writing */
						sarea.scomstat = ml_scom_write(sarea.scomreg, sarea.scomdata);	/* Write scom */
					}
					else {
						sarea.scomstat = ml_scom_read(sarea.scomreg, &sarea.scomdata);	/* Read scom */
					}
				}
				else {									/* Otherwise, tell the other processor */
					(void)cpu_signal(sarea.scomcpu, SIGPcpureq, CPRQscom ,(unsigned int)&sarea);	/* Ask him to do this */
					(void)hw_cpu_sync((unsigned long)&sarea.scomstat, LockTimeOut);	/* Wait for the other processor to get its temperature */
				}
			}

			ret = copyout(&sarea, (unsigned int)(save->save_r4), sizeof(scomcomm));	/* Get the data */
			if(ret) return 0;						/* Copyin failed - return an exception */
	
			return -1;								/* Return and check for ASTs... */
		
		case dgWar:									/* Set or reset workaround flags */
		
			save->save_r3 = (uint32_t)warFlags;		/* Get the old flags */
			oldwar = warFlags;						/* Remember the old war flags */
			
			subc = (int32_t)save->save_r4;			/* Extract the subcommand */
			switch(subc) {							/* Do what we need */
				case 1:								/* Replace all */
					warFlags = (uint32_t)save->save_r5;	/* Do them all */
					break;
				
				case 2:								/* Turn on selected workarounds */
					warFlags = warFlags | (uint32_t)save->save_r5;
					break;
					
				case 3:								/* Turn off selected workarounds */
					warFlags = warFlags & ~((uint32_t)save->save_r5);
					break;
				
				case 4:								/* Start up selected workaround */
					break;
				
				case 5:								/* Stop selected workaround */
					break;
				
				case 6:								/* Reset specific workaround parameters to default */
					break;
				
				case 7:								/* Set workaround parameters */
					break;

				default:
				
					break;
					
			}

			save->save_r3 = oldwar;					/* Pass back original */
			return -1;				
		

>>>>>>> origin/10.3
		default:									/* Handle invalid ones */
			return 0;								/* Return an exception */
		
	}

};

kern_return_t testPerfTrap(int trapno, struct savearea *ss, 
	unsigned int dsisr, unsigned int dar) {

	if(trapno != T_ALIGNMENT) return KERN_FAILURE;

	kprintf("alignment exception at %08X, srr1 = %08X, dsisr = %08X, dar = %08X\n", ss->save_srr0,
		ss->save_srr1, dsisr, dar);
		
	return KERN_SUCCESS;

}

