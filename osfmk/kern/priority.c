/*
 * Copyright (c) 2000-2010 Apple Inc. All rights reserved.
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
/*
 * @OSF_COPYRIGHT@
 */
/* 
 * Mach Operating System
 * Copyright (c) 1991,1990,1989,1988,1987 Carnegie Mellon University
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
/*
 *	File:	priority.c
 *	Author:	Avadis Tevanian, Jr.
 *	Date:	1986
 *
 *	Priority related scheduler bits.
 */

#include <mach/boolean.h>
#include <mach/kern_return.h>
#include <mach/machine.h>
#include <kern/host.h>
#include <kern/mach_param.h>
#include <kern/sched.h>
#include <sys/kdebug.h>
#include <kern/spl.h>
#include <kern/thread.h>
#include <kern/processor.h>
#include <kern/ledger.h>
#include <machine/machparam.h>
#include <kern/machine.h>

#ifdef CONFIG_MACH_APPROXIMATE_TIME
#include <machine/commpage.h>  /* for commpage_update_mach_approximate_time */
#endif

/*
 *	thread_quantum_expire:
 *
 *	Recalculate the quantum and priority for a thread.
 *
 *	Called at splsched.
 */

void
thread_quantum_expire(
	timer_call_param_t	p0,
	timer_call_param_t	p1)
{
	processor_t			processor = p0;
	thread_t			thread = p1;
	ast_t				preempt;
	uint64_t			ctime;
	int					urgency;
	uint64_t			ignore1, ignore2;

	assert(processor == current_processor());
	assert(thread == current_thread());

	SCHED_STATS_QUANTUM_TIMER_EXPIRATION(processor);

	/*
	 * We bill CPU time to both the individual thread and its task.
	 *
	 * Because this balance adjustment could potentially attempt to wake this very
	 * thread, we must credit the ledger before taking the thread lock. The ledger
	 * pointers are only manipulated by the thread itself at the ast boundary.
	 */
	ledger_credit(thread->t_ledger, task_ledgers.cpu_time, thread->quantum_remaining);
	ledger_credit(thread->t_threadledger, thread_ledgers.cpu_time, thread->quantum_remaining);
#ifdef CONFIG_BANK
	if (thread->t_bankledger) {
		ledger_credit(thread->t_bankledger, bank_ledgers.cpu_time,
				(thread->quantum_remaining - thread->t_deduct_bank_ledger_time));
	}
	thread->t_deduct_bank_ledger_time = 0;
#endif

	ctime = mach_absolute_time();

#ifdef CONFIG_MACH_APPROXIMATE_TIME
	commpage_update_mach_approximate_time(ctime);
#endif

	thread_lock(thread);

	/*
	 * We've run up until our quantum expiration, and will (potentially)
	 * continue without re-entering the scheduler, so update this now.
	 */
	processor->last_dispatch = ctime;
	thread->last_run_time = ctime;

	/*
	 *	Check for fail-safe trip.
	 */
 	if ((thread->sched_mode == TH_MODE_REALTIME || thread->sched_mode == TH_MODE_FIXED) && 
 	    !(thread->sched_flags & TH_SFLAG_PROMOTED_MASK) &&
 	    !(thread->options & TH_OPT_SYSTEM_CRITICAL)) {
 		uint64_t new_computation;
  
 		new_computation = ctime - thread->computation_epoch;
 		new_computation += thread->computation_metered;
 		if (new_computation > max_unsafe_computation) {
			KERNEL_DEBUG_CONSTANT(MACHDBG_CODE(DBG_MACH_SCHED, MACH_FAILSAFE)|DBG_FUNC_NONE,
					(uintptr_t)thread->sched_pri, (uintptr_t)thread->sched_mode, 0, 0, 0);

			thread->safe_release = ctime + sched_safe_duration;

			sched_thread_mode_demote(thread, TH_SFLAG_FAILSAFE);
		}
	}

	/*
	 *	Recompute scheduled priority if appropriate.
	 */
	if (SCHED(can_update_priority)(thread))
		SCHED(update_priority)(thread);
	else
		SCHED(lightweight_update_priority)(thread);

	if (thread->sched_mode != TH_MODE_REALTIME)
		SCHED(quantum_expire)(thread);

	processor->current_pri = thread->sched_pri;
	processor->current_thmode = thread->sched_mode;

	/* Tell platform layer that we are still running this thread */
	urgency = thread_get_urgency(thread, &ignore1, &ignore2);
	machine_thread_going_on_core(thread, urgency, 0);

	/*
	 *	This quantum is up, give this thread another.
	 */
	processor->first_timeslice = FALSE;

	thread_quantum_init(thread);

	/* Reload precise timing global policy to thread-local policy */
	thread->precise_user_kernel_time = use_precise_user_kernel_time(thread);

	/*
	 * Since non-precise user/kernel time doesn't update the state/thread timer
	 * during privilege transitions, synthesize an event now.
	 */
	if (!thread->precise_user_kernel_time) {
		timer_switch(PROCESSOR_DATA(processor, current_state),
					 ctime,
					 PROCESSOR_DATA(processor, current_state));
		timer_switch(PROCESSOR_DATA(processor, thread_timer),
					 ctime,
					 PROCESSOR_DATA(processor, thread_timer));
	}

	processor->quantum_end = ctime + thread->quantum_remaining;

	/*
	 *	Context switch check.
	 */
	if ((preempt = csw_check(processor, AST_QUANTUM)) != AST_NONE)
		ast_on(preempt);

	thread_unlock(thread);

	timer_call_enter1(&processor->quantum_timer, thread,
	    processor->quantum_end, TIMER_CALL_SYS_CRITICAL | TIMER_CALL_LOCAL);

#if defined(CONFIG_SCHED_TIMESHARE_CORE)
	sched_timeshare_consider_maintenance(ctime);
#endif /* CONFIG_SCHED_TIMESHARE_CORE */

}

/*
 *	sched_set_thread_base_priority:
 *
 *	Set the base priority of the thread
 *	and reset its scheduled priority.
 *
 *	This is the only path to change base_pri.
 *
 *	Called with the thread locked.
 */
void
sched_set_thread_base_priority(thread_t thread, int priority)
{
	thread->base_pri = priority;

	thread_recompute_sched_pri(thread, FALSE);
}

/*
 *	thread_recompute_sched_pri:
 *
 *	Reset the scheduled priority of the thread
 *	according to its base priority if the
 *	thread has not been promoted or depressed.
 *
 *	This is the standard way to push base_pri changes into sched_pri,
 *	or to recalculate the appropriate sched_pri after clearing
 *	a promotion or depression.
 *
 *	Called at splsched with the thread locked.
 */
void
thread_recompute_sched_pri(
                           thread_t thread,
                           boolean_t override_depress)
{
	int priority;

	if (thread->sched_mode == TH_MODE_TIMESHARE)
		priority = SCHED(compute_timeshare_priority)(thread);
	else
		priority = thread->base_pri;

	if ((!(thread->sched_flags & TH_SFLAG_PROMOTED_MASK)  || (priority > thread->sched_pri)) &&
	    (!(thread->sched_flags & TH_SFLAG_DEPRESSED_MASK) || override_depress)) {
		set_sched_pri(thread, priority);
	}
}

void
sched_default_quantum_expire(thread_t thread __unused)
{
      /*
       * No special behavior when a timeshare, fixed, or realtime thread
       * uses up its entire quantum
       */
}

#if defined(CONFIG_SCHED_TIMESHARE_CORE)

/*
 *	lightweight_update_priority:
 *
 *	Update the scheduled priority for
 *	a timesharing thread.
 *
 *	Only for use on the current thread.
 *
 *	Called with the thread locked.
 */
void
lightweight_update_priority(thread_t thread)
{
	assert(thread->runq == PROCESSOR_NULL);
	assert(thread == current_thread());

	if (thread->sched_mode == TH_MODE_TIMESHARE) {
		int priority;
		uint32_t delta;

		thread_timer_delta(thread, delta);

		/*
		 *	Accumulate timesharing usage only
		 *	during contention for processor
		 *	resources.
		 */
		if (thread->pri_shift < INT8_MAX)
			thread->sched_usage += delta;

		thread->cpu_delta += delta;

		priority = sched_compute_timeshare_priority(thread);

		/*
		 * Adjust the scheduled priority like thread_recompute_sched_pri,
		 * except with the benefit of knowing the thread is on this core.
		 */
		if ((!(thread->sched_flags & TH_SFLAG_PROMOTED_MASK)  || (priority > thread->sched_pri)) &&
		    (!(thread->sched_flags & TH_SFLAG_DEPRESSED_MASK)) &&
		    priority != thread->sched_pri) {

			thread->sched_pri = priority;

			KERNEL_DEBUG_CONSTANT(MACHDBG_CODE(DBG_MACH_SCHED, MACH_SCHED_CHANGE_PRIORITY),
			                      (uintptr_t)thread_tid(thread),
			                      thread->base_pri,
			                      thread->sched_pri,
			                      0, /* eventually, 'reason' */
			                      0);
		}
	}
}

/*
 *	Define shifts for simulating (5/8) ** n
 *
 *	Shift structures for holding update shifts.  Actual computation
 *	is  usage = (usage >> shift1) +/- (usage >> abs(shift2))  where the
 *	+/- is determined by the sign of shift 2.
 */
struct shift_data {
	int	shift1;
	int	shift2;
};

#define SCHED_DECAY_TICKS	32
static struct shift_data	sched_decay_shifts[SCHED_DECAY_TICKS] = {
	{1,1},{1,3},{1,-3},{2,-7},{3,5},{3,-5},{4,-8},{5,7},
	{5,-7},{6,-10},{7,10},{7,-9},{8,-11},{9,12},{9,-11},{10,-13},
	{11,14},{11,-13},{12,-15},{13,17},{13,-15},{14,-17},{15,19},{16,18},
	{16,-19},{17,22},{18,20},{18,-20},{19,26},{20,22},{20,-22},{21,-27}
};

/*
 *	sched_compute_timeshare_priority:
 *
 *	Calculate the timesharing priority based upon usage and load.
 */
extern int sched_pri_decay_band_limit;


int
sched_compute_timeshare_priority(thread_t thread)
{
	/* start with base priority */
	int priority = thread->base_pri - (thread->sched_usage >> thread->pri_shift);

	if (priority < MINPRI_USER)
		priority = MINPRI_USER;
	else if (priority > MAXPRI_KERNEL)
		priority = MAXPRI_KERNEL;

	return priority;
}


/*
 *	can_update_priority
 *
 *	Make sure we don't do re-dispatches more frequently than a scheduler tick.
 *
 *	Called with the thread locked.
 */
boolean_t
can_update_priority(
					thread_t	thread)
{
	if (sched_tick == thread->sched_stamp)
		return (FALSE);
	else
		return (TRUE);
}

/*
 *	update_priority
 *
 *	Perform housekeeping operations driven by scheduler tick.
 *
 *	Called with the thread locked.
 */
void
update_priority(
	register thread_t	thread)
{
	register unsigned	ticks;
	register uint32_t	delta;

	ticks = sched_tick - thread->sched_stamp;
	assert(ticks != 0);
	thread->sched_stamp += ticks;
	if (sched_use_combined_fgbg_decay)
		thread->pri_shift = sched_combined_fgbg_pri_shift;
	else if (thread->sched_flags & TH_SFLAG_THROTTLED)
		thread->pri_shift = sched_background_pri_shift;
	else
		thread->pri_shift = sched_pri_shift;

	/* If requested, accelerate aging of sched_usage */
	if (sched_decay_usage_age_factor > 1)
		ticks *= sched_decay_usage_age_factor;

	/*
	 *	Gather cpu usage data.
	 */
	thread_timer_delta(thread, delta);
	if (ticks < SCHED_DECAY_TICKS) {
		register struct shift_data	*shiftp;

		/*
		 *	Accumulate timesharing usage only
		 *	during contention for processor
		 *	resources.
		 */
		if (thread->pri_shift < INT8_MAX)
			thread->sched_usage += delta;

		thread->cpu_usage += delta + thread->cpu_delta;
		thread->cpu_delta = 0;

		shiftp = &sched_decay_shifts[ticks];
		if (shiftp->shift2 > 0) {
		    thread->cpu_usage =
						(thread->cpu_usage >> shiftp->shift1) +
						(thread->cpu_usage >> shiftp->shift2);
		    thread->sched_usage =
						(thread->sched_usage >> shiftp->shift1) +
						(thread->sched_usage >> shiftp->shift2);
		}
		else {
		    thread->cpu_usage =
						(thread->cpu_usage >> shiftp->shift1) -
						(thread->cpu_usage >> -(shiftp->shift2));
		    thread->sched_usage =
						(thread->sched_usage >> shiftp->shift1) -
						(thread->sched_usage >> -(shiftp->shift2));
		}
	}
	else {
		thread->cpu_usage = thread->cpu_delta = 0;
		thread->sched_usage = 0;
	}

	/*
	 *	Check for fail-safe release.
	 */
	if ((thread->sched_flags & TH_SFLAG_FAILSAFE) &&
	    mach_absolute_time() >= thread->safe_release) {
		sched_thread_mode_undemote(thread, TH_SFLAG_FAILSAFE);
	}

	/*
	 *	Recompute scheduled priority if appropriate.
	 */
	if (thread->sched_mode == TH_MODE_TIMESHARE) {
		int priority = sched_compute_timeshare_priority(thread);

		/*
		 * Adjust the scheduled priority like thread_recompute_sched_pri,
		 * except without setting an AST.
		 */
		if ((!(thread->sched_flags & TH_SFLAG_PROMOTED_MASK)  || (priority > thread->sched_pri)) &&
		    (!(thread->sched_flags & TH_SFLAG_DEPRESSED_MASK)) &&
		    priority != thread->sched_pri) {

			boolean_t removed = thread_run_queue_remove(thread);

			thread->sched_pri = priority;

			KERNEL_DEBUG_CONSTANT(MACHDBG_CODE(DBG_MACH_SCHED, MACH_SCHED_CHANGE_PRIORITY),
			                      (uintptr_t)thread_tid(thread),
			                      thread->base_pri,
			                      thread->sched_pri,
			                      0, /* eventually, 'reason' */
			                      0);

			if (removed)
				thread_run_queue_reinsert(thread, SCHED_TAILQ);
		}
	}

	return;
}

#endif /* CONFIG_SCHED_TIMESHARE_CORE */

#if MACH_ASSERT
/* sched_mode == TH_MODE_TIMESHARE controls whether a thread has a timeshare count when it has a run count */

void sched_share_incr(thread_t thread) {
	assert((thread->state & (TH_RUN|TH_IDLE)) == TH_RUN);
	assert(thread->sched_mode == TH_MODE_TIMESHARE);
	assert(thread->SHARE_COUNT == 0);
	thread->SHARE_COUNT++;
	(void)hw_atomic_add(&sched_share_count, 1);
}

void sched_share_decr(thread_t thread) {
	assert((thread->state & (TH_RUN|TH_IDLE)) != TH_RUN || thread->sched_mode != TH_MODE_TIMESHARE);
	assert(thread->SHARE_COUNT == 1);
	(void)hw_atomic_sub(&sched_share_count, 1);
	thread->SHARE_COUNT--;
}

/* TH_SFLAG_THROTTLED controls whether a thread has a background count when it has a run count and a share count */

void sched_background_incr(thread_t thread) {
	assert((thread->state & (TH_RUN|TH_IDLE)) == TH_RUN);
	assert(thread->sched_mode == TH_MODE_TIMESHARE);
	assert((thread->sched_flags & TH_SFLAG_THROTTLED) == TH_SFLAG_THROTTLED);

	assert(thread->BG_COUNT == 0);
	thread->BG_COUNT++;
	int val = hw_atomic_add(&sched_background_count, 1);
	assert(val >= 0);

	/* Always do the background change while holding a share count */
	assert(thread->SHARE_COUNT == 1);
}

void sched_background_decr(thread_t thread) {
	if ((thread->state & (TH_RUN|TH_IDLE)) == TH_RUN && thread->sched_mode == TH_MODE_TIMESHARE)
		assert((thread->sched_flags & TH_SFLAG_THROTTLED) != TH_SFLAG_THROTTLED);
	assert(thread->BG_COUNT == 1);
	int val = hw_atomic_sub(&sched_background_count, 1);
	thread->BG_COUNT--;
	assert(val >= 0);
	assert(thread->BG_COUNT == 0);

	/* Always do the background change while holding a share count */
	assert(thread->SHARE_COUNT == 1);
}


void
assert_thread_sched_count(thread_t thread) {
	/* Only 0 or 1 are acceptable values */
	assert(thread->BG_COUNT    == 0 || thread->BG_COUNT    == 1);
	assert(thread->SHARE_COUNT == 0 || thread->SHARE_COUNT == 1);

	/* BG is only allowed when you already have a share count */
	if (thread->BG_COUNT == 1)
		assert(thread->SHARE_COUNT == 1);
	if (thread->SHARE_COUNT == 0)
		assert(thread->BG_COUNT == 0);

	if ((thread->state & (TH_RUN|TH_IDLE)) != TH_RUN ||
	    (thread->sched_mode != TH_MODE_TIMESHARE))
		assert(thread->SHARE_COUNT == 0);

	if ((thread->state & (TH_RUN|TH_IDLE)) == TH_RUN &&
	    (thread->sched_mode == TH_MODE_TIMESHARE))
		assert(thread->SHARE_COUNT == 1);

	if ((thread->state & (TH_RUN|TH_IDLE)) != TH_RUN ||
	    (thread->sched_mode != TH_MODE_TIMESHARE)    ||
	    !(thread->sched_flags & TH_SFLAG_THROTTLED))
		assert(thread->BG_COUNT == 0);

	if ((thread->state & (TH_RUN|TH_IDLE)) == TH_RUN &&
	    (thread->sched_mode == TH_MODE_TIMESHARE)    &&
	    (thread->sched_flags & TH_SFLAG_THROTTLED))
		assert(thread->BG_COUNT == 1);
}

#endif /* MACH_ASSERT */

/*
 * Set the thread's true scheduling mode
 * Called with thread mutex and thread locked
 * The thread has already been removed from the runqueue.
 *
 * (saved_mode is handled before this point)
 */
void
sched_set_thread_mode(thread_t thread, sched_mode_t new_mode)
{
	assert_thread_sched_count(thread);
	assert(thread->runq == PROCESSOR_NULL);

	sched_mode_t old_mode = thread->sched_mode;

	thread->sched_mode = new_mode;

	switch (new_mode) {
		case TH_MODE_FIXED:
		case TH_MODE_REALTIME:
			if (old_mode == TH_MODE_TIMESHARE) {
				if ((thread->state & (TH_RUN|TH_IDLE)) == TH_RUN) {
					if (thread->sched_flags & TH_SFLAG_THROTTLED)
						sched_background_decr(thread);

					sched_share_decr(thread);
				}
			}
			break;

		case TH_MODE_TIMESHARE:
			if (old_mode != TH_MODE_TIMESHARE) {
				if ((thread->state & (TH_RUN|TH_IDLE)) == TH_RUN) {
					sched_share_incr(thread);

					if (thread->sched_flags & TH_SFLAG_THROTTLED)
						sched_background_incr(thread);
				}
			}
			break;

		default:
			panic("unexpected mode: %d", new_mode);
			break;
	}

	assert_thread_sched_count(thread);
}

/*
 * Demote the true scheduler mode to timeshare (called with the thread locked)
 */
void
sched_thread_mode_demote(thread_t thread, uint32_t reason)
{
	assert(reason & TH_SFLAG_DEMOTED_MASK);
	assert((thread->sched_flags & reason) != reason);
	assert_thread_sched_count(thread);

	if (thread->policy_reset)
		return;

	if (thread->sched_flags & TH_SFLAG_DEMOTED_MASK) {
		/* Another demotion reason is already active */
		thread->sched_flags |= reason;
		return;
	}

	assert(thread->saved_mode == TH_MODE_NONE);

	boolean_t removed = thread_run_queue_remove(thread);

	thread->sched_flags |= reason;

	thread->saved_mode = thread->sched_mode;

	sched_set_thread_mode(thread, TH_MODE_TIMESHARE);

	thread_recompute_priority(thread);

	if (removed)
		thread_run_queue_reinsert(thread, SCHED_TAILQ);

	assert_thread_sched_count(thread);
}

/*
 * Un-demote the true scheduler mode back to the saved mode (called with the thread locked)
 */
void
sched_thread_mode_undemote(thread_t thread, uint32_t reason)
{
	assert(reason & TH_SFLAG_DEMOTED_MASK);
	assert((thread->sched_flags & reason) == reason);
	assert(thread->saved_mode != TH_MODE_NONE);
	assert(thread->sched_mode == TH_MODE_TIMESHARE);
	assert(thread->policy_reset == 0);

	assert_thread_sched_count(thread);

	thread->sched_flags &= ~reason;

	if (thread->sched_flags & TH_SFLAG_DEMOTED_MASK) {
		/* Another demotion reason is still active */
		return;
	}

	boolean_t removed = thread_run_queue_remove(thread);

	sched_set_thread_mode(thread, thread->saved_mode);

	thread->saved_mode = TH_MODE_NONE;

	thread_recompute_priority(thread);

	if (removed)
		thread_run_queue_reinsert(thread, SCHED_TAILQ);
}

/*
 * Set the thread to be categorized as 'background'
 * Called with thread mutex and thread lock held
 *
 * TODO: Eventually, 'background' should be a true sched_mode.
 */
void
sched_set_thread_throttled(thread_t thread, boolean_t wants_throttle)
{
	if (thread->policy_reset)
		return;

	assert(((thread->sched_flags & TH_SFLAG_THROTTLED) ? TRUE : FALSE) != wants_throttle);

	assert_thread_sched_count(thread);

	/*
	 * When backgrounding a thread, iOS has the semantic that
	 * realtime and fixed priority threads should be demoted
	 * to timeshare background threads.
	 *
	 * On OSX, realtime and fixed priority threads don't lose their mode.
	 */

	if (wants_throttle) {
		thread->sched_flags |= TH_SFLAG_THROTTLED;
		if ((thread->state & (TH_RUN|TH_IDLE)) == TH_RUN && thread->sched_mode == TH_MODE_TIMESHARE) {
			sched_background_incr(thread);
		}

		assert_thread_sched_count(thread);

	} else {
		thread->sched_flags &= ~TH_SFLAG_THROTTLED;
		if ((thread->state & (TH_RUN|TH_IDLE)) == TH_RUN && thread->sched_mode == TH_MODE_TIMESHARE) {
			sched_background_decr(thread);
		}

		assert_thread_sched_count(thread);

	}

	assert_thread_sched_count(thread);
}

