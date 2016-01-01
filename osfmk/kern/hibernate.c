/*
 * Copyright (c) 2004-2005 Apple Computer, Inc. All rights reserved.
 *
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
 */

#include <kern/kalloc.h>
#include <kern/machine.h>
#include <kern/misc_protos.h>
#include <kern/thread.h>
#include <kern/processor.h>
#include <mach/machine.h>
#include <mach/processor_info.h>
#include <mach/mach_types.h>
#include <default_pager/default_pager_internal.h>
#include <IOKit/IOPlatformExpert.h>

#include <IOKit/IOHibernatePrivate.h>
#include <vm/vm_page.h>
#include <vm/vm_pageout.h>
#include <vm/vm_purgeable_internal.h>
#include <vm/vm_compressor.h>

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

boolean_t	need_to_unlock_decompressor = FALSE;

kern_return_t 
hibernate_alloc_page_lists(
		hibernate_page_list_t ** page_list_ret,
		hibernate_page_list_t ** page_list_wired_ret,
		hibernate_page_list_t ** page_list_pal_ret)
{
<<<<<<< HEAD
    kern_return_t	retval = KERN_SUCCESS;
=======
    uint64_t start, end, nsec;
    vm_page_t m;
    uint32_t pages = page_list->page_count;
    uint32_t count_zf = 0, count_throttled = 0;
    uint32_t count_inactive = 0, count_active = 0, count_speculative = 0;
    uint32_t count_wire = pages;
    uint32_t count_discard_active    = 0;
    uint32_t count_discard_inactive  = 0;
    uint32_t count_discard_purgeable = 0;
    uint32_t count_discard_speculative = 0;
    uint32_t i;
    uint32_t             bank;
    hibernate_bitmap_t * bitmap;
    hibernate_bitmap_t * bitmap_wired;


    HIBLOG("hibernate_page_list_setall start\n");

    clock_get_uptime(&start);

    hibernate_page_list_zero(page_list);
    hibernate_page_list_zero(page_list_wired);

    m = (vm_page_t) hibernate_gobble_queue;
    while(m)
    {
	pages--;
	count_wire--;
	hibernate_page_bitset(page_list,       TRUE, m->phys_page);
	hibernate_page_bitset(page_list_wired, TRUE, m->phys_page);
	m = (vm_page_t) m->pageq.next;
    }

    for( i = 0; i < vm_colors; i++ )
    {
	queue_iterate(&vm_page_queue_free[i],
		      m,
		      vm_page_t,
		      pageq)
	{
	    pages--;
	    count_wire--;
	    hibernate_page_bitset(page_list,       TRUE, m->phys_page);
	    hibernate_page_bitset(page_list_wired, TRUE, m->phys_page);
	}
    }
>>>>>>> origin/10.5

    hibernate_page_list_t * page_list = NULL;
    hibernate_page_list_t * page_list_wired = NULL;
    hibernate_page_list_t * page_list_pal = NULL;

    page_list = hibernate_page_list_allocate(TRUE);
    if (!page_list) {

<<<<<<< HEAD
	    retval = KERN_RESOURCE_SHORTAGE;
	    goto done;
=======
    queue_iterate( &vm_page_queue_zf,
                    m,
                    vm_page_t,
		    pageq )
    {
        if ((kIOHibernateModeDiscardCleanInactive & gIOHibernateMode) 
         && consider_discard(m))
        {
            hibernate_page_bitset(page_list, TRUE, m->phys_page);
	    if (m->dirty)
		count_discard_purgeable++;
	    else
		count_discard_inactive++;
        }
        else
            count_zf++;
	count_wire--;
	hibernate_page_bitset(page_list_wired, TRUE, m->phys_page);
>>>>>>> origin/10.5
    }
    page_list_wired = hibernate_page_list_allocate(FALSE);
    if (!page_list_wired)
    {
	    kfree(page_list, page_list->list_size);

<<<<<<< HEAD
	    retval = KERN_RESOURCE_SHORTAGE;
	    goto done;
=======
    for( i = 0; i <= VM_PAGE_MAX_SPECULATIVE_AGE_Q; i++ )
    {
	queue_iterate(&vm_page_queue_speculative[i].age_q,
		      m,
		      vm_page_t,
		      pageq)
	{
	    if ((kIOHibernateModeDiscardCleanInactive & gIOHibernateMode) 
	     && consider_discard(m))
	    {
		hibernate_page_bitset(page_list, TRUE, m->phys_page);
		count_discard_speculative++;
	    }
	    else
		count_speculative++;
	    count_wire--;
	    hibernate_page_bitset(page_list_wired, TRUE, m->phys_page);
	}
    }

    queue_iterate( &vm_page_queue_active,
                    m,
                    vm_page_t,
                    pageq )
    {
        if ((kIOHibernateModeDiscardCleanActive & gIOHibernateMode) 
         && consider_discard(m))
        {
            hibernate_page_bitset(page_list, TRUE, m->phys_page);
	    if (m->dirty)
		count_discard_purgeable++;
	    else
		count_discard_active++;
        }
        else
            count_active++;
	count_wire--;
	hibernate_page_bitset(page_list_wired, TRUE, m->phys_page);
>>>>>>> origin/10.5
    }
    page_list_pal = hibernate_page_list_allocate(FALSE);
    if (!page_list_pal)
    {
<<<<<<< HEAD
	    kfree(page_list, page_list->list_size);
	    kfree(page_list_wired, page_list_wired->list_size);

	    retval = KERN_RESOURCE_SHORTAGE;
	    goto done;
=======
	for (i = 0; i < bitmap->bitmapwords; i++)
	    bitmap->bitmap[i] = bitmap->bitmap[i] | ~bitmap_wired->bitmap[i];
	bitmap       = (hibernate_bitmap_t *) &bitmap->bitmap      [bitmap->bitmapwords];
	bitmap_wired = (hibernate_bitmap_t *) &bitmap_wired->bitmap[bitmap_wired->bitmapwords];
    }

    // machine dependent adjustments
    hibernate_page_list_setall_machine(page_list, page_list_wired, &pages);

    clock_get_uptime(&end);
    absolutetime_to_nanoseconds(end - start, &nsec);
    HIBLOG("hibernate_page_list_setall time: %qd ms\n", nsec / 1000000ULL);

    HIBLOG("pages %d, wire %d, act %d, inact %d, spec %d, zf %d, throt %d, could discard act %d inact %d purgeable %d spec %d\n", 
                pages, count_wire, count_active, count_inactive, count_speculative, count_zf, count_throttled,
                count_discard_active, count_discard_inactive, count_discard_purgeable, count_discard_speculative);

    *pagesOut = pages - count_discard_active - count_discard_inactive - count_discard_purgeable - count_discard_speculative;
}

void
hibernate_page_list_discard(hibernate_page_list_t * page_list)
{
    uint64_t  start, end, nsec;
    vm_page_t m;
    vm_page_t next;
    uint32_t  i;
    uint32_t  count_discard_active    = 0;
    uint32_t  count_discard_inactive  = 0;
    uint32_t  count_discard_purgeable = 0;
    uint32_t  count_discard_speculative = 0;

    clock_get_uptime(&start);

    m = (vm_page_t) queue_first(&vm_page_queue_zf);
    while (m && !queue_end(&vm_page_queue_zf, (queue_entry_t)m))
    {
        next = (vm_page_t) m->pageq.next;
        if (hibernate_page_bittst(page_list, m->phys_page))
        {
	    if (m->dirty)
		count_discard_purgeable++;
	    else
		count_discard_inactive++;
            discard_page(m);
        }
        m = next;
    }

    for( i = 0; i <= VM_PAGE_MAX_SPECULATIVE_AGE_Q; i++ )
    {
	m = (vm_page_t) queue_first(&vm_page_queue_speculative[i].age_q);
	while (m && !queue_end(&vm_page_queue_speculative[i].age_q, (queue_entry_t)m))
	{
	    next = (vm_page_t) m->pageq.next;
	    if (hibernate_page_bittst(page_list, m->phys_page))
	    {
		count_discard_speculative++;
		discard_page(m);
	    }
	    m = next;
	}
    }

    m = (vm_page_t) queue_first(&vm_page_queue_inactive);
    while (m && !queue_end(&vm_page_queue_inactive, (queue_entry_t)m))
    {
        next = (vm_page_t) m->pageq.next;
        if (hibernate_page_bittst(page_list, m->phys_page))
        {
	    if (m->dirty)
		count_discard_purgeable++;
	    else
		count_discard_inactive++;
            discard_page(m);
        }
        m = next;
>>>>>>> origin/10.5
    }
    *page_list_ret        = page_list;
    *page_list_wired_ret  = page_list_wired;
    *page_list_pal_ret    = page_list_pal;

done:
    return (retval);

<<<<<<< HEAD
=======
    clock_get_uptime(&end);
    absolutetime_to_nanoseconds(end - start, &nsec);
    HIBLOG("hibernate_page_list_discard time: %qd ms, discarded act %d inact %d purgeable %d spec %d\n",
                nsec / 1000000ULL,
                count_discard_active, count_discard_inactive, count_discard_purgeable, count_discard_speculative);
>>>>>>> origin/10.5
}

extern int sync_internal(void);

kern_return_t 
hibernate_setup(IOHibernateImageHeader * header,
                        boolean_t vmflush,
			hibernate_page_list_t * page_list __unused,
			hibernate_page_list_t * page_list_wired __unused,
			hibernate_page_list_t * page_list_pal __unused)
{
    kern_return_t	retval = KERN_SUCCESS;

    hibernate_create_paddr_map();

    hibernate_reset_stats();
    
    if (vmflush && (COMPRESSED_PAGER_IS_ACTIVE || dp_isssd)) {
	    
	    sync_internal();

	    if (COMPRESSED_PAGER_IS_ACTIVE) {
		    vm_decompressor_lock();
		    need_to_unlock_decompressor = TRUE;
	    }
	    hibernate_flush_memory();
    }

    // no failures hereafter

    hibernate_processor_setup(header);

    HIBLOG("hibernate_alloc_pages act %d, inact %d, anon %d, throt %d, spec %d, wire %d, wireinit %d\n",
    	    vm_page_active_count, vm_page_inactive_count, 
	    vm_page_anonymous_count,  vm_page_throttled_count, vm_page_speculative_count,
	    vm_page_wire_count, vm_page_wire_count_initial);

    if (retval != KERN_SUCCESS && need_to_unlock_decompressor == TRUE) {
	    need_to_unlock_decompressor = FALSE;
	    vm_decompressor_unlock();
    }
    return (retval);
}

kern_return_t 
hibernate_teardown(hibernate_page_list_t * page_list,
                    hibernate_page_list_t * page_list_wired,
		    hibernate_page_list_t * page_list_pal)
{
    hibernate_free_gobble_pages();

    if (page_list)
        kfree(page_list, page_list->list_size);
    if (page_list_wired)
        kfree(page_list_wired, page_list_wired->list_size);
    if (page_list_pal)
        kfree(page_list_pal, page_list_pal->list_size);

    if (COMPRESSED_PAGER_IS_ACTIVE) {
	    if (need_to_unlock_decompressor == TRUE) {
		    need_to_unlock_decompressor = FALSE;
		    vm_decompressor_unlock();
	    }
	    vm_compressor_delay_trim();
    }
    return (KERN_SUCCESS);
}
