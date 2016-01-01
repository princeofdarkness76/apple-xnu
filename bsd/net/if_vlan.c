/*
<<<<<<< HEAD
 * Copyright (c) 2003-2014 Apple Inc. All rights reserved.
=======
 * Copyright (c) 2003 Apple Computer, Inc. All rights reserved.
>>>>>>> origin/10.3
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
 * Copyright 1998 Massachusetts Institute of Technology
 *
 * Permission to use, copy, modify, and distribute this software and
 * its documentation for any purpose and without fee is hereby
 * granted, provided that both the above copyright notice and this
 * permission notice appear in all copies, that both the above
 * copyright notice and this permission notice appear in all
 * supporting documentation, and that the name of M.I.T. not be used
 * in advertising or publicity pertaining to distribution of the
 * software without specific, written prior permission.  M.I.T. makes
 * no representations about the suitability of this software for any
 * purpose.  It is provided "as is" without express or implied
 * warranty.
 * 
 * THIS SOFTWARE IS PROVIDED BY M.I.T. ``AS IS''.  M.I.T. DISCLAIMS
 * ALL EXPRESS OR IMPLIED WARRANTIES WITH REGARD TO THIS SOFTWARE,
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE. IN NO EVENT
 * SHALL M.I.T. BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF
 * USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * $FreeBSD: src/sys/net/if_vlan.c,v 1.54 2003/10/31 18:32:08 brooks Exp $
 */

/*
 * if_vlan.c - pseudo-device driver for IEEE 802.1Q virtual LANs.
 * Might be extended some day to also handle IEEE 802.1p priority
 * tagging.  This is sort of sneaky in the implementation, since
 * we need to pretend to be enough of an Ethernet implementation
 * to make arp work.  The way we do this is by telling everyone
 * that we are an Ethernet, and then catch the packets that
 * ether_output() left on our output queue when it calls
 * if_start(), rewrite them for use by the real outgoing interface,
 * and ask it to send them.
 */


#include <sys/param.h>
#include <sys/kernel.h>
#include <sys/malloc.h>
#include <sys/mbuf.h>
#include <sys/queue.h>
#include <sys/socket.h>
#include <sys/sockio.h>
#include <sys/sysctl.h>
#include <sys/systm.h>
#include <sys/kern_event.h>
<<<<<<< HEAD
#include <sys/mcache.h>
=======
>>>>>>> origin/10.3

#include <net/bpf.h>
#include <net/ethernet.h>
#include <net/if.h>
#include <net/if_arp.h>
#include <net/if_dl.h>
#include <net/if_ether.h>
#include <net/if_types.h>
#include <net/if_vlan_var.h>
#include <libkern/OSAtomic.h>

#include <net/dlil.h>

<<<<<<< HEAD
#include <net/kpi_interface.h>
#include <net/kpi_protocol.h>

#include <kern/locks.h>
=======
#include <net/dlil.h>
>>>>>>> origin/10.3

#ifdef INET
#include <netinet/in.h>
#include <netinet/if_ether.h>
#endif

#include <net/if_media.h>
<<<<<<< HEAD
#include <net/multicast_list.h>
#include <net/ether_if_module.h>

#define VLANNAME	"vlan"

typedef int (bpf_callback_func)(struct ifnet *, struct mbuf *);
typedef int (if_set_bpf_tap_func)(struct ifnet *ifp, int mode, bpf_callback_func * func);

/**
 ** vlan locks
 **/
static __inline__ lck_grp_t *
my_lck_grp_alloc_init(const char * grp_name)
{
    lck_grp_t *		grp;
    lck_grp_attr_t *	grp_attrs;
    
    grp_attrs = lck_grp_attr_alloc_init();
    grp = lck_grp_alloc_init(grp_name, grp_attrs);
    lck_grp_attr_free(grp_attrs);
    return (grp);
}

static __inline__ lck_mtx_t *
my_lck_mtx_alloc_init(lck_grp_t * lck_grp)
{
    lck_attr_t * 	lck_attrs;
    lck_mtx_t *		lck_mtx;

    lck_attrs = lck_attr_alloc_init();
    lck_mtx = lck_mtx_alloc_init(lck_grp, lck_attrs);
    lck_attr_free(lck_attrs);
    return (lck_mtx);
}

static lck_mtx_t * 	vlan_lck_mtx;

static __inline__ void
vlan_lock_init(void)
{
    lck_grp_t *		vlan_lck_grp;

    vlan_lck_grp = my_lck_grp_alloc_init("if_vlan");
    vlan_lck_mtx = my_lck_mtx_alloc_init(vlan_lck_grp);
}

static __inline__ void
vlan_assert_lock_held(void)
{
    lck_mtx_assert(vlan_lck_mtx, LCK_MTX_ASSERT_OWNED);
    return;
}

static __inline__ void
vlan_assert_lock_not_held(void)
{
    lck_mtx_assert(vlan_lck_mtx, LCK_MTX_ASSERT_NOTOWNED);
    return;
}

static __inline__ void
vlan_lock(void)
{
    lck_mtx_lock(vlan_lck_mtx);
    return;
}

static __inline__ void
vlan_unlock(void)
{
    lck_mtx_unlock(vlan_lck_mtx);
    return;
}

/**
 ** vlan structures, types
 **/
struct vlan_parent;
LIST_HEAD(vlan_parent_list, vlan_parent);
struct ifvlan;
LIST_HEAD(ifvlan_list, ifvlan);

typedef LIST_ENTRY(vlan_parent) 
vlan_parent_entry;
typedef LIST_ENTRY(ifvlan) 
ifvlan_entry;

#define VLP_SIGNATURE		0xfaceface
typedef struct vlan_parent {
    vlan_parent_entry		vlp_parent_list;/* list of parents */
    struct ifnet *		vlp_ifp;	/* interface */
    struct ifvlan_list		vlp_vlan_list;	/* list of VLAN's */
#define VLPF_SUPPORTS_VLAN_MTU		0x00000001
#define VLPF_CHANGE_IN_PROGRESS		0x00000002
#define VLPF_DETACHING			0x00000004
#define VLPF_LINK_EVENT_REQUIRED	0x00000008
    u_int32_t			vlp_flags;
    u_int32_t			vlp_event_code;
    struct ifdevmtu		vlp_devmtu;
    int32_t			vlp_retain_count;
    u_int32_t			vlp_signature;	/* VLP_SIGNATURE */
} vlan_parent, * vlan_parent_ref;

#define IFV_SIGNATURE		0xbeefbeef
struct ifvlan {
    ifvlan_entry 		ifv_vlan_list;
    char			ifv_name[IFNAMSIZ]; /* our unique id */
    struct ifnet *		ifv_ifp;	/* our interface */
    vlan_parent_ref		ifv_vlp;	/* parent information */
    struct	ifv_linkmib {
	u_int16_t ifvm_encaplen;/* encapsulation length */
	u_int16_t ifvm_mtufudge;/* MTU fudged by this much */
	u_int16_t ifvm_proto;	/* encapsulation ethertype */
	u_int16_t ifvm_tag; 	/* tag to apply on packets leaving if */
    }	ifv_mib;
    struct multicast_list 	ifv_multicast;
#define	IFVF_PROMISC		0x1		/* promiscuous mode enabled */
#define IFVF_DETACHING		0x2		/* interface is detaching */
#define IFVF_READY		0x4		/* interface is ready */
    u_int32_t			ifv_flags;
    bpf_packet_func		ifv_bpf_input;
    bpf_packet_func		ifv_bpf_output;
    int32_t			ifv_retain_count;
    u_int32_t			ifv_signature;	/* IFV_SIGNATURE */
};

typedef struct ifvlan * ifvlan_ref;

typedef struct vlan_globals_s {
    struct vlan_parent_list	parent_list;
    int				verbose;
} * vlan_globals_ref;
    
static vlan_globals_ref	g_vlan;

#define	ifv_tag		ifv_mib.ifvm_tag
#define	ifv_encaplen	ifv_mib.ifvm_encaplen
#define	ifv_mtufudge	ifv_mib.ifvm_mtufudge

static void
vlan_parent_retain(vlan_parent_ref vlp);

static void
vlan_parent_release(vlan_parent_ref vlp);

/**
 ** vlan_parent_ref vlp_flags in-lines
 **/
static __inline__ int
vlan_parent_flags_supports_vlan_mtu(vlan_parent_ref vlp)
{
    return ((vlp->vlp_flags & VLPF_SUPPORTS_VLAN_MTU) != 0);
}

static __inline__ void
vlan_parent_flags_set_supports_vlan_mtu(vlan_parent_ref vlp)
{
    vlp->vlp_flags |= VLPF_SUPPORTS_VLAN_MTU;
    return;
}

static __inline__ int
vlan_parent_flags_change_in_progress(vlan_parent_ref vlp)
{
    return ((vlp->vlp_flags & VLPF_CHANGE_IN_PROGRESS) != 0);
}

static __inline__ void
vlan_parent_flags_set_change_in_progress(vlan_parent_ref vlp)
{
    vlp->vlp_flags |= VLPF_CHANGE_IN_PROGRESS;
    return;
}

static __inline__ void
vlan_parent_flags_clear_change_in_progress(vlan_parent_ref vlp)
{
    vlp->vlp_flags &= ~VLPF_CHANGE_IN_PROGRESS;
    return;
}

static __inline__ int
vlan_parent_flags_detaching(struct vlan_parent * vlp)
{
    return ((vlp->vlp_flags & VLPF_DETACHING) != 0);
}

static __inline__ void
vlan_parent_flags_set_detaching(struct vlan_parent * vlp)
{
    vlp->vlp_flags |= VLPF_DETACHING;
    return;
}

static __inline__ int
vlan_parent_flags_link_event_required(vlan_parent_ref vlp)
{
    return ((vlp->vlp_flags & VLPF_LINK_EVENT_REQUIRED) != 0);
}

static __inline__ void
vlan_parent_flags_set_link_event_required(vlan_parent_ref vlp)
{
    vlp->vlp_flags |= VLPF_LINK_EVENT_REQUIRED;
    return;
}

static __inline__ void
vlan_parent_flags_clear_link_event_required(vlan_parent_ref vlp)
{
    vlp->vlp_flags &= ~VLPF_LINK_EVENT_REQUIRED;
    return;
}


/**
 ** ifvlan_flags in-lines routines
 **/
static __inline__ int
ifvlan_flags_promisc(ifvlan_ref ifv)
{
    return ((ifv->ifv_flags & IFVF_PROMISC) != 0);
}

static __inline__ void
ifvlan_flags_set_promisc(ifvlan_ref ifv)
{
    ifv->ifv_flags |= IFVF_PROMISC;
    return;
}

static __inline__ void
ifvlan_flags_clear_promisc(ifvlan_ref ifv)
{
    ifv->ifv_flags &= ~IFVF_PROMISC;
    return;
}

static __inline__ int
ifvlan_flags_ready(ifvlan_ref ifv)
{
    return ((ifv->ifv_flags & IFVF_READY) != 0);
}

static __inline__ void
ifvlan_flags_set_ready(ifvlan_ref ifv)
{
    ifv->ifv_flags |= IFVF_READY;
    return;
}

static __inline__ int
ifvlan_flags_detaching(ifvlan_ref ifv)
{
    return ((ifv->ifv_flags & IFVF_DETACHING) != 0);
}

static __inline__ void
ifvlan_flags_set_detaching(ifvlan_ref ifv)
{
    ifv->ifv_flags |= IFVF_DETACHING;
    return;
}

#if 0
SYSCTL_DECL(_net_link);
SYSCTL_NODE(_net_link, IFT_L2VLAN, vlan, CTLFLAG_RW|CTLFLAG_LOCKED, 0, "IEEE 802.1Q VLAN");
SYSCTL_NODE(_net_link_vlan, PF_LINK, link, CTLFLAG_RW|CTLFLAG_LOCKED, 0, "for consistency");
#endif

#define M_VLAN 		M_DEVBUF

static	int vlan_clone_create(struct if_clone *, u_int32_t, void *);
static	int vlan_clone_destroy(struct ifnet *);
static	int vlan_input(ifnet_t ifp, protocol_family_t protocol,
					   mbuf_t m, char *frame_header);
static	int vlan_output(struct ifnet *ifp, struct mbuf *m);
static	int vlan_ioctl(ifnet_t ifp, u_long cmd, void * addr);
static  int vlan_set_bpf_tap(ifnet_t ifp, bpf_tap_mode mode,
			     bpf_packet_func func);
static 	int vlan_attach_protocol(struct ifnet *ifp);
static	int vlan_detach_protocol(struct ifnet *ifp);
static	int vlan_setmulti(struct ifnet *ifp);
static	int vlan_unconfig(ifvlan_ref ifv, int need_to_wait);
static 	int vlan_config(struct ifnet * ifp, struct ifnet * p, int tag);
static	void vlan_if_free(struct ifnet * ifp);
static 	int vlan_remove(ifvlan_ref ifv, int need_to_wait);

static struct if_clone vlan_cloner = IF_CLONE_INITIALIZER(VLANNAME,
							  vlan_clone_create, 
							  vlan_clone_destroy, 
							  0, 
							  IF_MAXUNIT);
static	void interface_link_event(struct ifnet * ifp, u_int32_t event_code);
static	void vlan_parent_link_event(struct ifnet * p,
				    u_int32_t event_code);

static 	int ifvlan_new_mtu(ifvlan_ref ifv, int mtu);

/**
 ** ifvlan_ref routines
 **/
static void
ifvlan_retain(ifvlan_ref ifv)
{
    if (ifv->ifv_signature != IFV_SIGNATURE) {
	panic("ifvlan_retain: bad signature\n");
    }
    if (ifv->ifv_retain_count == 0) {
	panic("ifvlan_retain: retain count is 0\n");
    }
    OSIncrementAtomic(&ifv->ifv_retain_count);
}

static void
ifvlan_release(ifvlan_ref ifv)
{
    u_int32_t		old_retain_count;

    if (ifv->ifv_signature != IFV_SIGNATURE) {
	panic("ifvlan_release: bad signature\n");
    }
    old_retain_count = OSDecrementAtomic(&ifv->ifv_retain_count);
    switch (old_retain_count) {
    case 0:
	panic("ifvlan_release: retain count is 0\n");
	break;
    case 1:
	if (g_vlan->verbose) {
	    printf("ifvlan_release(%s)\n", ifv->ifv_name);
	}
	ifv->ifv_signature = 0;
	FREE(ifv, M_VLAN);
	break;
    default:
	break;
    }
    return;
}

static vlan_parent_ref
ifvlan_get_vlan_parent_retained(ifvlan_ref ifv)
{
    vlan_parent_ref	vlp = ifv->ifv_vlp;

    if (vlp == NULL || vlan_parent_flags_detaching(vlp)) {
	return (NULL);
    }
    vlan_parent_retain(vlp);
    return (vlp);
}

/**
 ** ifnet_* routines
 **/

static ifvlan_ref
ifnet_get_ifvlan(struct ifnet * ifp)
{
    ifvlan_ref		ifv;

    ifv = (ifvlan_ref)ifnet_softc(ifp);
    return (ifv);
}

static ifvlan_ref
ifnet_get_ifvlan_retained(struct ifnet * ifp)
{
    ifvlan_ref		ifv;

    ifv = ifnet_get_ifvlan(ifp);
    if (ifv == NULL) {
	return (NULL);
    }
    if (ifvlan_flags_detaching(ifv)) {
	return (NULL);
    }
    ifvlan_retain(ifv);
    return (ifv);
}

static int
ifnet_ifvlan_vlan_parent_ok(struct ifnet * ifp, ifvlan_ref ifv,
			    vlan_parent_ref vlp)
{
    ifvlan_ref		check_ifv;

    check_ifv = ifnet_get_ifvlan(ifp);
    if (check_ifv != ifv || ifvlan_flags_detaching(ifv)) {
	/* ifvlan_ref no longer valid */
	return (FALSE);
    }
    if (ifv->ifv_vlp != vlp) {
	/* vlan_parent no longer valid */
	return (FALSE);
    }
    if (vlan_parent_flags_detaching(vlp)) {
	/* parent is detaching */
	return (FALSE);
    }
    return (TRUE);
}

/**
 ** vlan, etc. routines
 **/

static int
vlan_globals_init(void)
{
    vlan_globals_ref	v;

    vlan_assert_lock_not_held();

    if (g_vlan != NULL) {
	return (0);
    }
    v = _MALLOC(sizeof(*v), M_VLAN, M_WAITOK);
    if (v != NULL) {
	LIST_INIT(&v->parent_list);
	v->verbose = 0;
    }
    vlan_lock();
    if (g_vlan != NULL) {
	vlan_unlock();
	if (v != NULL) {
	    _FREE(v, M_VLAN);
	}
	return (0);
    }
    g_vlan = v;
    vlan_unlock();
    if (v == NULL) {
	return (ENOMEM);
    }
    return (0);
}

static int
siocgifdevmtu(struct ifnet * ifp, struct ifdevmtu * ifdm_p)
{
    struct ifreq	ifr;
    int 		error;

    bzero(&ifr, sizeof(ifr));
    error = ifnet_ioctl(ifp, 0,SIOCGIFDEVMTU, &ifr);
    if (error == 0) {
	*ifdm_p = ifr.ifr_devmtu;
    }
    return (error);
}

static int
siocsifaltmtu(struct ifnet * ifp, int mtu)
{
    struct ifreq	ifr;

    bzero(&ifr, sizeof(ifr));
    ifr.ifr_mtu = mtu;
    return (ifnet_ioctl(ifp, 0, SIOCSIFALTMTU, &ifr));
}

static __inline__ void 
vlan_bpf_output(struct ifnet * ifp, struct mbuf * m, 
		bpf_packet_func func)
{
    if (func != NULL) {
	(*func)(ifp, m);
=======

#define	ETHER_VLAN_ENCAP_LEN	4	/* len of 802.1Q VLAN encapsulation */
#define	IF_MAXUNIT	0x7fff		/* historical value */

#define IFP2AC(p) ((struct arpcom *)p)

#define VLAN_PROTO_FAMILY	0x766c616e /* 'vlan' */

#define VLANNAME	"vlan"

typedef int (bpf_callback_func)(struct ifnet *, struct mbuf *);
typedef int (if_set_bpf_tap_func)(struct ifnet *ifp, int mode, bpf_callback_func * func);

struct vlan_mc_entry {
    struct ether_addr		mc_addr;
    SLIST_ENTRY(vlan_mc_entry)	mc_entries;
};

struct	ifvlan {
    char	ifv_name[IFNAMSIZ]; /* our unique id */
    struct  ifnet *ifv_ifp;  /* our interface */
    struct	ifnet *ifv_p;	/* parent interface of this vlan */
    struct	ifv_linkmib {
	int	ifvm_parent;
	int	ifvm_encaplen;	/* encapsulation length */
	int	ifvm_mtufudge;	/* MTU fudged by this much */
	int	ifvm_mintu;	/* min transmission unit */
	u_int16_t ifvm_proto; /* encapsulation ethertype */
	u_int16_t ifvm_tag; /* tag to apply on packets leaving if */
    }	ifv_mib;
    SLIST_HEAD(__vlan_mchead, vlan_mc_entry)	vlan_mc_listhead;
    LIST_ENTRY(ifvlan) ifv_list;
    int	ifv_flags;
    int ifv_detaching;
    u_long ifv_filter_id;
    int ifv_filter_valid;
    bpf_callback_func *	ifv_bpf_input;
    bpf_callback_func * ifv_bpf_output;
};

#define	ifv_tag	ifv_mib.ifvm_tag
#define	ifv_encaplen	ifv_mib.ifvm_encaplen
#define	ifv_mtufudge	ifv_mib.ifvm_mtufudge
#define	ifv_mintu	ifv_mib.ifvm_mintu

#define	IFVF_PROMISC		0x01		/* promiscuous mode enabled */

#if 0
SYSCTL_DECL(_net_link);
SYSCTL_NODE(_net_link, IFT_L2VLAN, vlan, CTLFLAG_RW, 0, "IEEE 802.1Q VLAN");
SYSCTL_NODE(_net_link_vlan, PF_LINK, link, CTLFLAG_RW, 0, "for consistency");
#endif 0

#define M_VLAN 		M_DEVBUF

MALLOC_DEFINE(M_VLAN, VLANNAME, "802.1Q Virtual LAN Interface");

static LIST_HEAD(, ifvlan) ifv_list;

#if 0
/*
 * Locking: one lock is used to guard both the ifv_list and modification
 * to vlan data structures.  We are rather conservative here; probably
 * more than necessary.
 */
static struct mtx ifv_mtx;
#define	VLAN_LOCK_INIT()	mtx_init(&ifv_mtx, VLANNAME, NULL, MTX_DEF)
#define	VLAN_LOCK_DESTROY()	mtx_destroy(&ifv_mtx)
#define	VLAN_LOCK_ASSERT()	mtx_assert(&ifv_mtx, MA_OWNED)
#define	VLAN_LOCK()	mtx_lock(&ifv_mtx)
#define	VLAN_UNLOCK()	mtx_unlock(&ifv_mtx)
#else
#define	VLAN_LOCK_INIT()
#define	VLAN_LOCK_DESTROY()
#define	VLAN_LOCK_ASSERT()
#define	VLAN_LOCK()
#define	VLAN_UNLOCK()
#endif 0

static	int vlan_clone_create(struct if_clone *, int);
static	void vlan_clone_destroy(struct ifnet *);
static	int vlan_output(struct ifnet *ifp, struct mbuf *m);
static	void vlan_ifinit(void *foo);
static	int vlan_ioctl(struct ifnet *ifp, u_long cmd, void * addr);
static  int vlan_set_bpf_tap(struct ifnet * ifp, int mode,
			     bpf_callback_func * func);
static 	int vlan_attach_protocol(struct ifnet *ifp);
static	int vlan_detach_protocol(struct ifnet *ifp);
static 	int vlan_attach_filter(struct ifnet * ifp, u_long * filter_id);
static 	int vlan_detach_filter(u_long filter_id);
static	int vlan_setmulti(struct ifnet *ifp);
static	int vlan_unconfig(struct ifnet *ifp);
static	int vlan_config(struct ifvlan *ifv, struct ifnet *p, int tag);
static	int vlan_if_free(struct ifnet * ifp);

static struct if_clone vlan_cloner = IF_CLONE_INITIALIZER(VLANNAME,
							  vlan_clone_create, vlan_clone_destroy, 0, IF_MAXUNIT);

static	if_set_bpf_tap_func nop_if_bpf;
static	int nop_if_free(struct ifnet *);
static	int nop_if_ioctl(struct ifnet *, u_long, void *);
static	int nop_if_output(struct ifnet * ifp, struct mbuf * m);

static 	void interface_link_event(struct ifnet * ifp, u_long event_code);

static __inline__ void 
vlan_bpf_output(struct ifnet * ifp, struct mbuf * m, 
		bpf_callback_func func)
{
    if (func != NULL) {
	func(ifp, m);
>>>>>>> origin/10.3
    }
    return;
}

static __inline__ void 
vlan_bpf_input(struct ifnet * ifp, struct mbuf * m, 
<<<<<<< HEAD
	       bpf_packet_func func, char * frame_header,
=======
	       bpf_callback_func func, char * frame_header,
>>>>>>> origin/10.3
	       int frame_header_len, int encap_len)
{
    if (func != NULL) {
	if (encap_len > 0) {
	    /* present the right header to bpf */
	    bcopy(frame_header, frame_header + encap_len, frame_header_len);
	}
	m->m_data -= frame_header_len;
	m->m_len += frame_header_len;
<<<<<<< HEAD
	(*func)(ifp, m);
=======
	func(ifp, m);
>>>>>>> origin/10.3
	m->m_data += frame_header_len;
	m->m_len -= frame_header_len;
	if (encap_len > 0) {
	    /* restore the header */
	    bcopy(frame_header + encap_len, frame_header, frame_header_len);
	}
    }
    return;
}

<<<<<<< HEAD
/**
 ** vlan_parent synchronization routines
 **/
static void
vlan_parent_retain(vlan_parent_ref vlp)
{
    if (vlp->vlp_signature != VLP_SIGNATURE) {
	panic("vlan_parent_retain: signature is bad\n");
    }
    if (vlp->vlp_retain_count == 0) {
	panic("vlan_parent_retain: retain count is 0\n");
    }
    OSIncrementAtomic(&vlp->vlp_retain_count);
}

static void
vlan_parent_release(vlan_parent_ref vlp)
{
    u_int32_t		old_retain_count;

    if (vlp->vlp_signature != VLP_SIGNATURE) {
	panic("vlan_parent_release: signature is bad\n");
    }
    old_retain_count = OSDecrementAtomic(&vlp->vlp_retain_count);
    switch (old_retain_count) {
    case 0:
	panic("vlan_parent_release: retain count is 0\n");
	break;
    case 1:
	if (g_vlan->verbose) {
	    struct ifnet * ifp = vlp->vlp_ifp;
	    printf("vlan_parent_release(%s%d)\n", ifnet_name(ifp),
		   ifnet_unit(ifp));
	}
	vlp->vlp_signature = 0;
	FREE(vlp, M_VLAN);
	break;
    default:
	break;
    }
    return;
}

/*
 * Function: vlan_parent_wait
 * Purpose:
 *   Allows a single thread to gain exclusive access to the vlan_parent
 *   data structure.  Some operations take a long time to complete, 
 *   and some have side-effects that we can't predict.  Holding the
 *   vlan_lock() across such operations is not possible.
 *
 * Notes:
 *   Before calling, you must be holding the vlan_lock and have taken
 *   a reference on the vlan_parent_ref.
 */
static void
vlan_parent_wait(vlan_parent_ref vlp, const char * msg)
{
    int		waited = 0;

    /* other add/remove/multicast-change in progress */
    while (vlan_parent_flags_change_in_progress(vlp)) {
	if (g_vlan->verbose) {
	    struct ifnet * ifp = vlp->vlp_ifp;

	    printf("%s%d: %s msleep\n", ifnet_name(ifp), ifnet_unit(ifp), msg);
	}
	waited = 1;
	(void)msleep(vlp, vlan_lck_mtx, PZERO, msg, 0);
    }
    /* prevent other vlan parent remove/add from taking place */
    vlan_parent_flags_set_change_in_progress(vlp);
    if (g_vlan->verbose && waited) {
	struct ifnet * ifp = vlp->vlp_ifp;

	printf("%s%d: %s woke up\n", ifnet_name(ifp), ifnet_unit(ifp), msg);
    }
    return;
}

/*
 * Function: vlan_parent_signal
 * Purpose:
 *   Allows the thread that previously invoked vlan_parent_wait() to 
 *   give up exclusive access to the vlan_parent data structure, and wake up
 *   any other threads waiting to access
 * Notes:
 *   Before calling, you must be holding the vlan_lock and have taken
 *   a reference on the vlan_parent_ref.
 */
static void
vlan_parent_signal(vlan_parent_ref vlp, const char * msg)
{
    struct ifnet * vlp_ifp = vlp->vlp_ifp;

    if (vlan_parent_flags_link_event_required(vlp)) {
	vlan_parent_flags_clear_link_event_required(vlp);
	if (!vlan_parent_flags_detaching(vlp)) {
	    u_int32_t		event_code = vlp->vlp_event_code;
	    ifvlan_ref 		ifv;

	    vlan_unlock();

	    /* we can safely walk the list unlocked */
	    LIST_FOREACH(ifv, &vlp->vlp_vlan_list, ifv_vlan_list) {
		struct ifnet *	ifp = ifv->ifv_ifp;
		
		interface_link_event(ifp, event_code);
	    }
	    if (g_vlan->verbose) {
		printf("%s%d: propagated link event to vlans\n",
		       ifnet_name(vlp_ifp), ifnet_unit(vlp_ifp));
	    }
	    vlan_lock();
	}
    }
    vlan_parent_flags_clear_change_in_progress(vlp);
    wakeup((caddr_t)vlp);
    if (g_vlan->verbose) {
	printf("%s%d: %s wakeup\n",
	       ifnet_name(vlp_ifp), ifnet_unit(vlp_ifp), msg);
    }
    return;
=======
static struct ifaddr * 
ifaddr_byindex(unsigned int i)
{
    if (i > if_index || i == 0) {
	return (NULL);
    }
    return (ifnet_addrs[i - 1]);
>>>>>>> origin/10.3
}

/*
 * Program our multicast filter. What we're actually doing is
 * programming the multicast filter of the parent. This has the
 * side effect of causing the parent interface to receive multicast
 * traffic that it doesn't really want, which ends up being discarded
 * later by the upper protocol layers. Unfortunately, there's no way
 * to avoid this: there really is only one physical interface.
 */
static int
<<<<<<< HEAD
vlan_setmulti(struct ifnet * ifp)
{
    int			error = 0;
    ifvlan_ref 		ifv;
    struct ifnet *	p;
    vlan_parent_ref	vlp = NULL;

    vlan_lock();
    ifv = ifnet_get_ifvlan_retained(ifp);
    if (ifv == NULL) {
	goto unlock_done;
    }
    vlp = ifvlan_get_vlan_parent_retained(ifv);
    if (vlp == NULL) {
	/* no parent, no need to program the multicast filter */
	goto unlock_done;
    }
    vlan_parent_wait(vlp, "vlan_setmulti");

    /* check again, things could have changed */
    if (ifnet_ifvlan_vlan_parent_ok(ifp, ifv, vlp) == FALSE) {
	goto signal_done;
    }
    p = vlp->vlp_ifp;
    vlan_unlock();

    /* update parent interface with our multicast addresses */
    error = multicast_list_program(&ifv->ifv_multicast, ifp, p);

    vlan_lock();

 signal_done:
    vlan_parent_signal(vlp, "vlan_setmulti");

 unlock_done:
    vlan_unlock();
    if (ifv != NULL) {
	ifvlan_release(ifv);
    }
    if (vlp != NULL) {
	vlan_parent_release(vlp);
    }
    return (error);
}

/**
 ** vlan_parent list manipulation/lookup routines
 **/
static vlan_parent_ref
parent_list_lookup(struct ifnet * p)
{
    vlan_parent_ref	vlp;

    LIST_FOREACH(vlp, &g_vlan->parent_list, vlp_parent_list) {
	if (vlp->vlp_ifp == p) {
	    return (vlp);
	}
    }
    return (NULL);
}

static ifvlan_ref
vlan_parent_lookup_tag(vlan_parent_ref vlp, int tag)
{
    ifvlan_ref		ifv;

    LIST_FOREACH(ifv, &vlp->vlp_vlan_list, ifv_vlan_list) {
	if (tag == ifv->ifv_tag) {
	    return (ifv);
	}
    }
    return (NULL);
}

static ifvlan_ref 
vlan_lookup_parent_and_tag(struct ifnet * p, int tag)
{
    vlan_parent_ref	vlp;

    vlp = parent_list_lookup(p);
    if (vlp != NULL) {
	return (vlan_parent_lookup_tag(vlp, tag));
    }
    return (NULL);
}

static int
vlan_parent_find_max_mtu(vlan_parent_ref vlp, ifvlan_ref exclude_ifv)
{
    int			max_mtu = 0;
    ifvlan_ref		ifv;

    LIST_FOREACH(ifv, &vlp->vlp_vlan_list, ifv_vlan_list) {
	int	req_mtu;

	if (exclude_ifv == ifv) {
	    continue;
	}
	req_mtu = ifnet_mtu(ifv->ifv_ifp) + ifv->ifv_mtufudge;
	if (req_mtu > max_mtu) {
	    max_mtu = req_mtu;
	}
    }
    return (max_mtu);
}

/*
 * Function: vlan_parent_create
 * Purpose:
 *   Create a vlan_parent structure to hold the VLAN's for the given
 *   interface.  Add it to the list of VLAN parents.
 */
static int
vlan_parent_create(struct ifnet * p, vlan_parent_ref * ret_vlp)
{
    int			error;
    vlan_parent_ref	vlp;

    *ret_vlp = NULL;
    vlp = _MALLOC(sizeof(*vlp), M_VLAN, M_WAITOK | M_ZERO);
    if (vlp == NULL) {
	return (ENOMEM);
    }
    error = siocgifdevmtu(p, &vlp->vlp_devmtu);
    if (error != 0) {
	printf("vlan_parent_create (%s%d): siocgifdevmtu failed, %d\n",
	       ifnet_name(p), ifnet_unit(p), error);
	FREE(vlp, M_VLAN);
	return (error);
    }
    LIST_INIT(&vlp->vlp_vlan_list);
    vlp->vlp_ifp = p;
    vlp->vlp_retain_count = 1;
    vlp->vlp_signature = VLP_SIGNATURE;
    if (ifnet_offload(p)
	& (IF_HWASSIST_VLAN_MTU | IF_HWASSIST_VLAN_TAGGING)) {
	vlan_parent_flags_set_supports_vlan_mtu(vlp);
    }
    *ret_vlp = vlp;
=======
vlan_setmulti(struct ifnet *ifp)
{
    struct ifnet		*p;
    struct ifmultiaddr	*ifma, *rifma = NULL;
    struct ifvlan		*sc;
    struct vlan_mc_entry	*mc = NULL;
    struct sockaddr_dl	sdl;
    int			error;

    /* Find the parent. */
    sc = ifp->if_private;
    p = sc->ifv_p;
    if (p == NULL) {
	/* no parent, so no need to program the multicast filter */
	return (0);
    }

    bzero((char *)&sdl, sizeof sdl);
    sdl.sdl_len = sizeof sdl;
    sdl.sdl_family = AF_LINK;
    sdl.sdl_index = p->if_index;
    sdl.sdl_type = IFT_ETHER;
    sdl.sdl_alen = ETHER_ADDR_LEN;

    /* First, remove any existing filter entries. */
    while (SLIST_FIRST(&sc->vlan_mc_listhead) != NULL) {
	mc = SLIST_FIRST(&sc->vlan_mc_listhead);
	bcopy((char *)&mc->mc_addr, LLADDR(&sdl), ETHER_ADDR_LEN);
	error = if_delmulti(p, (struct sockaddr *)&sdl);
	if (error)
	    return(error);
	SLIST_REMOVE_HEAD(&sc->vlan_mc_listhead, mc_entries);
	FREE(mc, M_VLAN);
    }

    /* Now program new ones. */
    LIST_FOREACH(ifma, &ifp->if_multiaddrs, ifma_link) {
	if (ifma->ifma_addr->sa_family != AF_LINK)
	    continue;
	mc = _MALLOC(sizeof(struct vlan_mc_entry), M_VLAN, M_WAITOK);
	bcopy(LLADDR((struct sockaddr_dl *)ifma->ifma_addr),
	      (char *)&mc->mc_addr, ETHER_ADDR_LEN);
	SLIST_INSERT_HEAD(&sc->vlan_mc_listhead, mc, mc_entries);
	bcopy(LLADDR((struct sockaddr_dl *)ifma->ifma_addr),
	      LLADDR(&sdl), ETHER_ADDR_LEN);
	error = if_addmulti(p, (struct sockaddr *)&sdl, &rifma);
	if (error)
	    return(error);
    }

    return(0);
}

#if 0
/*
 * VLAN support can be loaded as a module.  The only place in the
 * system that's intimately aware of this is ether_input.  We hook
 * into this code through vlan_input_p which is defined there and
 * set here.  Noone else in the system should be aware of this so
 * we use an explicit reference here.
 *
 * NB: Noone should ever need to check if vlan_input_p is null or
 *     not.  This is because interfaces have a count of the number
 *     of active vlans (if_nvlans) and this should never be bumped
 *     except by vlan_config--which is in this module so therefore
 *     the module must be loaded and vlan_input_p must be non-NULL.
 */
extern	void (*vlan_input_p)(struct ifnet *, struct mbuf *);

static int
vlan_modevent(module_t mod, int type, void *data) 
{ 

    switch (type) { 
    case MOD_LOAD: 
	LIST_INIT(&ifv_list);
	VLAN_LOCK_INIT();
	vlan_input_p = vlan_input;
	if_clone_attach(&vlan_cloner);
	break; 
    case MOD_UNLOAD: 
	if_clone_detach(&vlan_cloner);
	vlan_input_p = NULL;
	while (!LIST_EMPTY(&ifv_list))
	    vlan_clone_destroy(LIST_FIRST(&ifv_list)->ifv_ifp);
	VLAN_LOCK_DESTROY();
	break;
    } 
    return 0; 
} 

static moduledata_t vlan_mod = { 
    "if_vlan", 
    vlan_modevent, 
    0
}; 

DECLARE_MODULE(if_vlan, vlan_mod, SI_SUB_PSEUDO, SI_ORDER_ANY);

#endif 0

static struct ifvlan * 
vlan_lookup_ifp_and_tag(struct ifnet * ifp, int tag)
{
    struct ifvlan * ifv;

    LIST_FOREACH(ifv, &ifv_list, ifv_list) {
	if (ifp == ifv->ifv_p && tag == ifv->ifv_tag) {
	    return (ifv);
	}
    }
    return (NULL);
}

static struct ifvlan * 
vlan_lookup_ifp(struct ifnet * ifp)
{
    struct ifvlan * ifv;

    LIST_FOREACH(ifv, &ifv_list, ifv_list) {
	if (ifp == ifv->ifv_p) {
	    return (ifv);
	}
    }
    return (NULL);
}

static void
vlan_clone_attach(void)
{
    if_clone_attach(&vlan_cloner);
    return;
}

static int
vlan_clone_create(struct if_clone *ifc, int unit)
{
    int error;
    struct ifvlan *ifv;
    struct ifnet *ifp;

    ifv = _MALLOC(sizeof(struct ifvlan), M_VLAN, M_WAITOK);
    bzero(ifv, sizeof(struct ifvlan));
    SLIST_INIT(&ifv->vlan_mc_listhead);

    /* use the interface name as the unique id for ifp recycle */
    if (snprintf(ifv->ifv_name, sizeof(ifv->ifv_name), "%s%d",
		 ifc->ifc_name, unit) >= sizeof(ifv->ifv_name)) {
	FREE(ifv, M_VLAN);
	return (EINVAL);
    }
    error = dlil_if_acquire(APPLE_IF_FAM_VLAN,
			    ifv->ifv_name,
			    strlen(ifv->ifv_name),
			    &ifp);
    if (error) {
	FREE(ifv, M_VLAN);
	return (error);
    }
    ifv->ifv_ifp = ifp;
    ifp->if_private = ifv;
    ifp->if_name = (char *)ifc->ifc_name;
    ifp->if_unit = unit;
    ifp->if_family = APPLE_IF_FAM_VLAN;

#if 0
    /* NB: flags are not set here */
    ifp->if_linkmib = &ifv->ifv_mib;
    ifp->if_linkmiblen = sizeof ifv->ifv_mib;
    /* NB: mtu is not set here */
#endif 0

    ifp->if_ioctl = vlan_ioctl;
    ifp->if_set_bpf_tap = vlan_set_bpf_tap;
    ifp->if_free = nop_if_free;
    ifp->if_output = nop_if_output;
    ifp->if_hwassist = 0;
    ifp->if_addrlen = ETHER_ADDR_LEN; /* XXX ethernet specific */
    ifp->if_baudrate = 0;
    ifp->if_type = IFT_L2VLAN;
    ifp->if_hdrlen = ETHER_VLAN_ENCAP_LEN;
    error = dlil_if_attach(ifp);
    if (error) {
	dlil_if_release(ifp);
	FREE(ifv, M_VLAN);
	return (error);
    }

    /* attach as ethernet */
    bpfattach(ifp, DLT_EN10MB, sizeof(struct ether_header));

    VLAN_LOCK();
    LIST_INSERT_HEAD(&ifv_list, ifv, ifv_list);
    VLAN_UNLOCK();

>>>>>>> origin/10.3
    return (0);
}

static void
<<<<<<< HEAD
vlan_parent_remove_all_vlans(struct ifnet * p)
{
    ifvlan_ref 		ifv;
    int			need_vlp_release = 0;
    ifvlan_ref		next;
    vlan_parent_ref	vlp;

    vlan_lock();
    vlp = parent_list_lookup(p);
    if (vlp == NULL || vlan_parent_flags_detaching(vlp)) {
	/* no VLAN's */
	vlan_unlock();
	return;
    }
    vlan_parent_flags_set_detaching(vlp);
    vlan_parent_retain(vlp);
    vlan_parent_wait(vlp, "vlan_parent_remove_all_vlans");
    need_vlp_release++;
    vlp = parent_list_lookup(p);
    /* check again */
    if (vlp == NULL) {
	goto signal_done;
    }

    for (ifv = LIST_FIRST(&vlp->vlp_vlan_list); ifv != NULL; ifv = next) {
	struct ifnet *	ifp = ifv->ifv_ifp;
	int		removed;

	next = LIST_NEXT(ifv, ifv_vlan_list);
	removed = vlan_remove(ifv, FALSE);
	if (removed) {
	    vlan_unlock();
	    ifnet_detach(ifp);
	    vlan_lock();
	}
    }

    /* the vlan parent has no more VLAN's */
    ifnet_set_eflags(p, 0, IFEF_VLAN); /* clear IFEF_VLAN */

    LIST_REMOVE(vlp, vlp_parent_list);
    need_vlp_release++;	/* one for being in the list */
    need_vlp_release++; /* final reference */

 signal_done:
    vlan_parent_signal(vlp, "vlan_parent_remove_all_vlans");
    vlan_unlock();

    while (need_vlp_release--) {
	vlan_parent_release(vlp);
    }
    return;
}

static __inline__ int
vlan_parent_no_vlans(vlan_parent_ref vlp)
{
    return (LIST_EMPTY(&vlp->vlp_vlan_list));
}

static void
vlan_parent_add_vlan(vlan_parent_ref vlp, ifvlan_ref ifv, int tag)
{
    LIST_INSERT_HEAD(&vlp->vlp_vlan_list, ifv, ifv_vlan_list);
    ifv->ifv_vlp = vlp;
    ifv->ifv_tag = tag;
    return;
}

static void
vlan_parent_remove_vlan(__unused vlan_parent_ref vlp, ifvlan_ref ifv)
{
    ifv->ifv_vlp = NULL;
    LIST_REMOVE(ifv, ifv_vlan_list);
    return;
}

static int
vlan_clone_attach(void)
{
    int error;

    error = if_clone_attach(&vlan_cloner);
    if (error != 0)
        return error;
    vlan_lock_init();
    return 0;
}

static int
vlan_clone_create(struct if_clone *ifc, u_int32_t unit, __unused void *params)
{
	int							error;
	ifvlan_ref					ifv;
	ifnet_t						ifp;
	struct ifnet_init_eparams	vlan_init;
	
	error = vlan_globals_init();
	if (error != 0) {
		return (error);
	}
	ifv = _MALLOC(sizeof(struct ifvlan), M_VLAN, M_WAITOK | M_ZERO);
	if (ifv == NULL)
		return ENOBUFS;
	ifv->ifv_retain_count = 1;
	ifv->ifv_signature = IFV_SIGNATURE;
	multicast_list_init(&ifv->ifv_multicast);
	
	/* use the interface name as the unique id for ifp recycle */
	if ((unsigned int)
	    snprintf(ifv->ifv_name, sizeof(ifv->ifv_name), "%s%d",
		     ifc->ifc_name, unit) >= sizeof(ifv->ifv_name)) {
	    ifvlan_release(ifv);
	    return (EINVAL);
	}
	
	bzero(&vlan_init, sizeof(vlan_init));
	vlan_init.ver = IFNET_INIT_CURRENT_VERSION;
	vlan_init.len = sizeof (vlan_init);
	vlan_init.flags = IFNET_INIT_LEGACY;
	vlan_init.uniqueid = ifv->ifv_name;
	vlan_init.uniqueid_len = strlen(ifv->ifv_name);
	vlan_init.name = ifc->ifc_name;
	vlan_init.unit = unit;
	vlan_init.family = IFNET_FAMILY_VLAN;
	vlan_init.type = IFT_L2VLAN;
	vlan_init.output = vlan_output;
	vlan_init.demux = ether_demux;
	vlan_init.add_proto = ether_add_proto;
	vlan_init.del_proto = ether_del_proto;
	vlan_init.check_multi = ether_check_multi;
	vlan_init.framer_extended = ether_frameout_extended;
	vlan_init.softc = ifv;
	vlan_init.ioctl = vlan_ioctl;
	vlan_init.set_bpf_tap = vlan_set_bpf_tap;
	vlan_init.detach = vlan_if_free;
	vlan_init.broadcast_addr = etherbroadcastaddr;
	vlan_init.broadcast_len = ETHER_ADDR_LEN;
	error = ifnet_allocate_extended(&vlan_init, &ifp);
	
	if (error) {
	    ifvlan_release(ifv);
	    return (error);
	}
	
	ifnet_set_offload(ifp, 0);
	ifnet_set_addrlen(ifp, ETHER_ADDR_LEN); /* XXX ethernet specific */
	ifnet_set_baudrate(ifp, 0);
	ifnet_set_hdrlen(ifp, ETHER_VLAN_ENCAP_LEN);
	
	error = ifnet_attach(ifp, NULL);
	if (error) {
	    ifnet_release(ifp);
	    ifvlan_release(ifv);
	    return (error);
	}
	ifv->ifv_ifp = ifp;
	
	/* attach as ethernet */
	bpfattach(ifp, DLT_EN10MB, sizeof(struct ether_header));
	return (0);
}

static int
vlan_remove(ifvlan_ref ifv, int need_to_wait)
{
    vlan_assert_lock_held();
    if (ifvlan_flags_detaching(ifv)) {
	return (0);
    }
    ifvlan_flags_set_detaching(ifv);
    vlan_unconfig(ifv, need_to_wait);
    return (1);
}


static int
vlan_clone_destroy(struct ifnet *ifp)
{
    ifvlan_ref ifv;

    vlan_lock();
    ifv = ifnet_get_ifvlan_retained(ifp);
    if (ifv == NULL) {
	vlan_unlock();
	return 0;
    }
    if (vlan_remove(ifv, TRUE) == 0) {
	vlan_unlock();
	ifvlan_release(ifv);
	return 0;
    }
    vlan_unlock();
    ifvlan_release(ifv);
    ifnet_detach(ifp);

    return 0;
}

static int 
vlan_set_bpf_tap(ifnet_t ifp, bpf_tap_mode mode, bpf_packet_func func)
{
    ifvlan_ref	ifv;

    vlan_lock();
    ifv = ifnet_get_ifvlan_retained(ifp);
    if (ifv == NULL) {
	vlan_unlock();
	return (ENODEV);
    }
    switch (mode) {
        case BPF_TAP_DISABLE:
            ifv->ifv_bpf_input = ifv->ifv_bpf_output = NULL;
            break;

        case BPF_TAP_INPUT:
            ifv->ifv_bpf_input = func;
            break;

        case BPF_TAP_OUTPUT:
	    ifv->ifv_bpf_output = func;
            break;
        
        case BPF_TAP_INPUT_OUTPUT:
            ifv->ifv_bpf_input = ifv->ifv_bpf_output = func;
            break;
        default:
            break;
    }
    vlan_unlock();
    ifvlan_release(ifv);
    return 0;
}

static int
vlan_output(struct ifnet * ifp, struct mbuf * m)
{
    bpf_packet_func 		bpf_func;
    struct ether_vlan_header *	evl;
    int				encaplen;
    ifvlan_ref			ifv;
    struct ifnet *		p;
    int 			soft_vlan;
    u_short			tag;
    vlan_parent_ref		vlp = NULL;
    int				err;
    struct flowadv		adv = { FADV_SUCCESS };
	
    if (m == 0) {
	return (0);
    }
    if ((m->m_flags & M_PKTHDR) == 0) {
	m_freem_list(m);
	return (0);
    }
    vlan_lock();
    ifv = ifnet_get_ifvlan_retained(ifp);
    if (ifv == NULL || ifvlan_flags_ready(ifv) == 0) {
	goto unlock_done;
    }
    vlp = ifvlan_get_vlan_parent_retained(ifv);
    if (vlp == NULL) {
	goto unlock_done;
    }
    p = vlp->vlp_ifp;
    (void)ifnet_stat_increment_out(ifp, 1, m->m_pkthdr.len, 0);
    soft_vlan = (ifnet_offload(p) & IF_HWASSIST_VLAN_TAGGING) == 0;
    bpf_func = ifv->ifv_bpf_output;
    tag = ifv->ifv_tag;
    encaplen = ifv->ifv_encaplen;
    vlan_unlock();

    ifvlan_release(ifv);
    vlan_parent_release(vlp);

    vlan_bpf_output(ifp, m, bpf_func);
	
    /* do not run parent's if_output() if the parent is not up */
    if ((ifnet_flags(p) & (IFF_UP | IFF_RUNNING)) != (IFF_UP | IFF_RUNNING)) {
	m_freem(m);
	atomic_add_64(&ifp->if_collisions, 1);
	return (0);
    }
    /*
     * If underlying interface can do VLAN tag insertion itself,
     * just pass the packet along. However, we need some way to
     * tell the interface where the packet came from so that it
     * knows how to find the VLAN tag to use.  We use a field in
     * the mbuf header to store the VLAN tag, and a bit in the
     * csum_flags field to mark the field as valid.
     */
    if (soft_vlan == 0) {
	m->m_pkthdr.csum_flags |= CSUM_VLAN_TAG_VALID;
	m->m_pkthdr.vlan_tag = tag;
    } else {
	M_PREPEND(m, encaplen, M_DONTWAIT, 1);
	if (m == NULL) {
	    printf("%s%d: unable to prepend VLAN header\n", ifnet_name(ifp),
		   ifnet_unit(ifp));
	    atomic_add_64(&ifp->if_oerrors, 1);
	    return (0);
	}
	/* M_PREPEND takes care of m_len, m_pkthdr.len for us */
	if (m->m_len < (int)sizeof(*evl)) {
	    m = m_pullup(m, sizeof(*evl));
	    if (m == NULL) {
		printf("%s%d: unable to pullup VLAN header\n", ifnet_name(ifp),
		       ifnet_unit(ifp));
		atomic_add_64(&ifp->if_oerrors, 1);
		return (0);
	    }
	}
		
	/*
	 * Transform the Ethernet header into an Ethernet header
	 * with 802.1Q encapsulation.
	 */
	bcopy(mtod(m, char *) + encaplen,
	      mtod(m, char *), ETHER_HDR_LEN);
	evl = mtod(m, struct ether_vlan_header *);
	evl->evl_proto = evl->evl_encap_proto;
	evl->evl_encap_proto = htons(ETHERTYPE_VLAN);
	evl->evl_tag = htons(tag);
    }

    err = dlil_output(p, PF_VLAN, m, NULL, NULL, 1, &adv);

    if (err == 0) {
	if (adv.code == FADV_FLOW_CONTROLLED) {
	    err = EQFULL;
	} else if (adv.code == FADV_SUSPENDED) {
	    err = EQSUSPENDED;
	}
    }

    return (err);

 unlock_done:
    vlan_unlock();
    if (ifv != NULL) {
	ifvlan_release(ifv);
    }
    if (vlp != NULL) {
	vlan_parent_release(vlp);
    }
    m_freem_list(m);
    return (0);

}

static int
vlan_input(ifnet_t p, __unused protocol_family_t protocol,
					   mbuf_t m, char *frame_header)
{
    bpf_packet_func 		bpf_func = NULL;
    struct ether_vlan_header *	evl;
    struct ifnet *		ifp = NULL;
    int 			soft_vlan = 0;
    u_int 			tag = 0;

    if (m->m_pkthdr.csum_flags & CSUM_VLAN_TAG_VALID) {
	/*
	 * Packet is tagged, m contains a normal
	 * Ethernet frame; the tag is stored out-of-band.
	 */
	m->m_pkthdr.csum_flags &= ~CSUM_VLAN_TAG_VALID;
	tag = EVL_VLANOFTAG(m->m_pkthdr.vlan_tag);
	m->m_pkthdr.vlan_tag = 0;
    } else {
	soft_vlan = 1;
	switch (ifnet_type(p)) {
	case IFT_ETHER:
	    if (m->m_len < ETHER_VLAN_ENCAP_LEN) {
		m_freem(m);
		return 0;
	    }
	    evl = (struct ether_vlan_header *)(void *)frame_header;
	    if (ntohs(evl->evl_proto) == ETHERTYPE_VLAN) {
		/* don't allow VLAN within VLAN */
		m_freem(m);
		return (0);
	    }
	    tag = EVL_VLANOFTAG(ntohs(evl->evl_tag));
		
	    /*
	     * Restore the original ethertype.  We'll remove
	     * the encapsulation after we've found the vlan
	     * interface corresponding to the tag.
	     */
	    evl->evl_encap_proto = evl->evl_proto;
	    break;
	default:
	    printf("vlan_demux: unsupported if type %u", 
		   ifnet_type(p));
	    m_freem(m);
	    return 0;
	    break;
	}
    }
    if (tag != 0) {
	ifvlan_ref		ifv;

	if ((ifnet_eflags(p) & IFEF_VLAN) == 0) {
	    /* don't bother looking through the VLAN list */
	    m_freem(m);
	    return 0;
	}
	vlan_lock();
	ifv = vlan_lookup_parent_and_tag(p, tag);
	if (ifv != NULL) {
	    ifp = ifv->ifv_ifp;
	}
	if (ifv == NULL 
	    || ifvlan_flags_ready(ifv) == 0
	    || (ifnet_flags(ifp) & IFF_UP) == 0) {
	    vlan_unlock();
	    m_freem(m);
	    return 0;
	}
	bpf_func = ifv->ifv_bpf_input;
	vlan_unlock();
    }
    if (soft_vlan) {
	/*
	 * Packet had an in-line encapsulation header;
	 * remove it.  The original header has already
	 * been fixed up above.
	 */
	m->m_len -= ETHER_VLAN_ENCAP_LEN;
	m->m_data += ETHER_VLAN_ENCAP_LEN;
	m->m_pkthdr.len -= ETHER_VLAN_ENCAP_LEN;
	m->m_pkthdr.csum_flags = 0; /* can't trust hardware checksum */
    }
    if (tag != 0) {
	m->m_pkthdr.rcvif = ifp;
	m->m_pkthdr.pkt_hdr = frame_header;
	(void)ifnet_stat_increment_in(ifp, 1, 
				      m->m_pkthdr.len + ETHER_HDR_LEN, 0);
	vlan_bpf_input(ifp, m, bpf_func, frame_header, ETHER_HDR_LEN, 
		       soft_vlan ? ETHER_VLAN_ENCAP_LEN : 0);
	/* We found a vlan interface, inject on that interface. */
	dlil_input_packet_list(ifp, m);
    } else {
<<<<<<< HEAD
	m->m_pkthdr.pkt_hdr = frame_header;
=======
	m->m_pkthdr.header = frame_header;
>>>>>>> origin/10.5
	/* Send priority-tagged packet up through the parent */
	dlil_input_packet_list(p, m);
    }
    return 0;
}

static int
vlan_config(struct ifnet * ifp, struct ifnet * p, int tag)
{
    int			error;
    int			first_vlan = FALSE;
    ifvlan_ref 		ifv = NULL;
    int			ifv_added = FALSE;
    int			need_vlp_release = 0;
    vlan_parent_ref	new_vlp = NULL;
    ifnet_offload_t	offload;
    u_int16_t		parent_flags;
    vlan_parent_ref	vlp = NULL;

    /* pre-allocate space for vlan_parent, in case we're first */
    error = vlan_parent_create(p, &new_vlp);
    if (error != 0) {
	return (error);
    }

    vlan_lock();
    ifv = ifnet_get_ifvlan_retained(ifp);
    if (ifv == NULL || ifv->ifv_vlp != NULL) {
	vlan_unlock();
	if (ifv != NULL) {
	    ifvlan_release(ifv);
	}
	vlan_parent_release(new_vlp);
	return (EBUSY);
    }
    vlp = parent_list_lookup(p);
    if (vlp != NULL) {
	vlan_parent_retain(vlp);
	need_vlp_release++;
	if (vlan_parent_lookup_tag(vlp, tag) != NULL) {
	    /* already a VLAN with that tag on this interface */
	    error = EADDRINUSE;
	    goto unlock_done;
	}
    }
    else {
	/* one for being in the list */
	vlan_parent_retain(new_vlp);

	/* we're the first VLAN on this interface */
	LIST_INSERT_HEAD(&g_vlan->parent_list, new_vlp, vlp_parent_list);
	vlp = new_vlp;

	vlan_parent_retain(vlp);
	need_vlp_release++;
    }

    /* need to wait to ensure no one else is trying to add/remove */
    vlan_parent_wait(vlp, "vlan_config");

    if (ifnet_get_ifvlan(ifp) != ifv) {
	error = EINVAL;
	goto signal_done;
    }

    /* check again because someone might have gotten in */
    if (parent_list_lookup(p) != vlp) {
	error = EBUSY;
	goto signal_done;
    }

    if (vlan_parent_flags_detaching(vlp)
	|| ifvlan_flags_detaching(ifv) || ifv->ifv_vlp != NULL) {
	error = EBUSY;
	goto signal_done;
    }

    /* check again because someone might have gotten the tag */
    if (vlan_parent_lookup_tag(vlp, tag) != NULL) {
	/* already a VLAN with that tag on this interface */
	error = EADDRINUSE;
	goto signal_done;
    }

    if (vlan_parent_no_vlans(vlp)) {
	first_vlan = TRUE;
    }
    vlan_parent_add_vlan(vlp, ifv, tag);
    ifvlan_retain(ifv);	/* parent references ifv */
    ifv_added = TRUE;

    /* check whether bond interface is using parent interface */
    ifnet_lock_exclusive(p);
    if ((ifnet_eflags(p) & IFEF_BOND) != 0) {
	ifnet_lock_done(p);
	/* don't allow VLAN over interface that's already part of a bond */
	error = EBUSY;
	goto signal_done;
    }
    /* prevent BOND interface from using it */
    /* Can't use ifnet_set_eflags because that would take the lock */
    p->if_eflags |= IFEF_VLAN;
    ifnet_lock_done(p);
    vlan_unlock();

    if (first_vlan) {
	/* attach our VLAN "protocol" to the interface */
	error = vlan_attach_protocol(p);
	if (error) {
	    vlan_lock();
	    goto signal_done;
	}
    }

    /* configure parent to receive our multicast addresses */
    error = multicast_list_program(&ifv->ifv_multicast, ifp, p);
    if (error != 0) {
	if (first_vlan) {
	    (void)vlan_detach_protocol(p);
	}
	vlan_lock();
	goto signal_done;
    }

    /* set our ethernet address to that of the parent */
    ifnet_set_lladdr_and_type(ifp, IF_LLADDR(p), ETHER_ADDR_LEN, IFT_ETHER);

    /* no failures past this point */
    vlan_lock();

    ifv->ifv_encaplen = ETHER_VLAN_ENCAP_LEN;
    ifv->ifv_flags = 0;
    if (vlan_parent_flags_supports_vlan_mtu(vlp)) {
	ifv->ifv_mtufudge = 0;
    } else {
	/*
	 * Fudge the MTU by the encapsulation size.  This
	 * makes us incompatible with strictly compliant
	 * 802.1Q implementations, but allows us to use
	 * the feature with other NetBSD implementations,
	 * which might still be useful.
	 */
	ifv->ifv_mtufudge = ifv->ifv_encaplen;
    }
    ifnet_set_mtu(ifp, ETHERMTU - ifv->ifv_mtufudge);

    /*
     * Copy only a selected subset of flags from the parent.
     * Other flags are none of our business.
     */
    parent_flags = ifnet_flags(p) 
	& (IFF_BROADCAST | IFF_MULTICAST | IFF_SIMPLEX);
    ifnet_set_flags(ifp, parent_flags,
		    IFF_BROADCAST | IFF_MULTICAST | IFF_SIMPLEX);

    /* use hwassist bits from parent interface, but exclude VLAN bits */
    offload = ifnet_offload(p) & ~(IFNET_VLAN_TAGGING | IFNET_VLAN_MTU);
    ifnet_set_offload(ifp, offload);

    ifnet_set_flags(ifp, IFF_RUNNING, IFF_RUNNING);
    ifvlan_flags_set_ready(ifv);
    vlan_parent_signal(vlp, "vlan_config");
    vlan_unlock();
    if (new_vlp != vlp) {
	/* throw it away, it wasn't needed */
	vlan_parent_release(new_vlp);
    }
    if (ifv != NULL) {
	ifvlan_release(ifv);
    }
    if (first_vlan) {
	/* mark the parent interface up */
	ifnet_set_flags(p, IFF_UP, IFF_UP);
	(void)ifnet_ioctl(p, 0, SIOCSIFFLAGS, (caddr_t)NULL);
    }
    return 0;

 signal_done:
    vlan_assert_lock_held();

    if (ifv_added) {
	vlan_parent_remove_vlan(vlp, ifv);
	if (!vlan_parent_flags_detaching(vlp) && vlan_parent_no_vlans(vlp)) {
	    /* the vlan parent has no more VLAN's */
	    ifnet_set_eflags(p, 0, IFEF_VLAN);
	    LIST_REMOVE(vlp, vlp_parent_list);
	    /* release outside of the lock below */
	    need_vlp_release++;

	    /* one for being in the list */
	    need_vlp_release++;
	}
    }
    vlan_parent_signal(vlp, "vlan_config");

 unlock_done:
    vlan_unlock();

    while (need_vlp_release--) {
	vlan_parent_release(vlp);
    }
    if (new_vlp != vlp) {
	vlan_parent_release(new_vlp);
    }
    if (ifv != NULL) {
	if (ifv_added) {
	    ifvlan_release(ifv);
	}
	ifvlan_release(ifv);
    }
    return (error);
}

static void
vlan_link_event(struct ifnet * ifp, struct ifnet * p)
{
    struct ifmediareq ifmr;

    /* generate a link event based on the state of the underlying interface */
    bzero(&ifmr, sizeof(ifmr));
    snprintf(ifmr.ifm_name, sizeof(ifmr.ifm_name),
	     "%s%d", ifnet_name(p), ifnet_unit(p));
    if (ifnet_ioctl(p, 0, SIOCGIFMEDIA, &ifmr) == 0
	&& ifmr.ifm_count > 0 && ifmr.ifm_status & IFM_AVALID) {
	u_int32_t	event;
	
	event = (ifmr.ifm_status & IFM_ACTIVE)
	    ? KEV_DL_LINK_ON : KEV_DL_LINK_OFF;
	interface_link_event(ifp, event);
    }
    return;
}

static int
vlan_unconfig(ifvlan_ref ifv, int need_to_wait)
{
    struct ifnet *	ifp = ifv->ifv_ifp;
    int			last_vlan = FALSE;
    int			need_ifv_release = 0;
    int			need_vlp_release = 0;
    struct ifnet *	p;
    vlan_parent_ref	vlp;

    vlan_assert_lock_held();
    vlp = ifv->ifv_vlp;
    if (vlp == NULL) {
	return (0);
    }
    if (need_to_wait) {
	need_vlp_release++;
	vlan_parent_retain(vlp);
	vlan_parent_wait(vlp, "vlan_unconfig");

        /* check again because another thread could be in vlan_unconfig */
	if (ifv != ifnet_get_ifvlan(ifp)) {
	    goto signal_done;
	}
	if (ifv->ifv_vlp != vlp) {
	    /* vlan parent changed */
	    goto signal_done;
	}
    }

    /* ifv has a reference on vlp, need to remove it */
    need_vlp_release++;
    p = vlp->vlp_ifp;

    /* remember whether we're the last VLAN on the parent */
    if (LIST_NEXT(LIST_FIRST(&vlp->vlp_vlan_list), ifv_vlan_list) == NULL) {
	if (g_vlan->verbose) {
	    printf("vlan_unconfig: last vlan on %s%d\n",
		   ifnet_name(p), ifnet_unit(p));
	}
	last_vlan = TRUE;
    }

    /* back-out any effect our mtu might have had on the parent */
    (void)ifvlan_new_mtu(ifv, ETHERMTU - ifv->ifv_mtufudge);

    vlan_unlock();

    /* un-join multicast on parent interface */
    (void)multicast_list_remove(&ifv->ifv_multicast);

    /* Clear our MAC address. */
    ifnet_set_lladdr_and_type(ifp, NULL, 0, IFT_L2VLAN);

    /* detach VLAN "protocol" */
    if (last_vlan) {
	(void)vlan_detach_protocol(p);
    }

    vlan_lock();

    /* return to the state we were in before SIFVLAN */
    ifnet_set_mtu(ifp, 0);
    ifnet_set_flags(ifp, 0, 
		    IFF_BROADCAST | IFF_MULTICAST | IFF_SIMPLEX | IFF_RUNNING);
    ifnet_set_offload(ifp, 0);
    ifv->ifv_mtufudge = 0;

    /* Disconnect from parent. */
    vlan_parent_remove_vlan(vlp, ifv);
    ifv->ifv_flags = 0;

    /* vlan_parent has reference to ifv, remove it */
    need_ifv_release++;

    /* from this point on, no more referencing ifv */
    if (last_vlan && !vlan_parent_flags_detaching(vlp)) {
	/* the vlan parent has no more VLAN's */
	ifnet_set_eflags(p, 0, IFEF_VLAN);
	LIST_REMOVE(vlp, vlp_parent_list);

	/* one for being in the list */
	need_vlp_release++;

	/* release outside of the lock below */
	need_vlp_release++;
    }

 signal_done:
    if (need_to_wait) {
	vlan_parent_signal(vlp, "vlan_unconfig");
    }
    vlan_unlock();
    while (need_ifv_release--) {
	ifvlan_release(ifv);
    }
    while (need_vlp_release--) {	/* references to vlp */
	vlan_parent_release(vlp);
    }
    vlan_lock();
    return (0);
=======
vlan_remove(struct ifvlan * ifv)
{
    VLAN_LOCK_ASSERT();
    ifv->ifv_detaching = 1;
    vlan_unconfig(ifv->ifv_ifp);
    LIST_REMOVE(ifv, ifv_list);
    return;
}

static void
vlan_if_detach(struct ifnet * ifp)
{
    ifp->if_output = nop_if_output;
    ifp->if_ioctl = nop_if_ioctl;
    ifp->if_set_bpf_tap = &nop_if_bpf;
    if (dlil_if_detach(ifp) == DLIL_WAIT_FOR_FREE) {
	ifp->if_free = vlan_if_free;
    } else {
	vlan_if_free(ifp);
    }
    return;
}

static void
vlan_clone_destroy(struct ifnet *ifp)
{
    struct ifvlan *ifv = ifp->if_private;

    if (ifv == NULL || ifp->if_type != IFT_L2VLAN) {
	return;
    }
    VLAN_LOCK();
    if (ifv->ifv_detaching) {
	VLAN_UNLOCK();
	return;
    }
    vlan_remove(ifv);
    VLAN_UNLOCK();
    vlan_if_detach(ifp);
    return;
}

static int 
vlan_set_bpf_tap(struct ifnet * ifp, int mode, bpf_callback_func * func)
{
    struct ifvlan *ifv = ifp->if_private;

    switch (mode) {
        case BPF_TAP_DISABLE:
            ifv->ifv_bpf_input = ifv->ifv_bpf_output = NULL;
            break;

        case BPF_TAP_INPUT:
            ifv->ifv_bpf_input = func;
            break;

        case BPF_TAP_OUTPUT:
	    ifv->ifv_bpf_output = func;
            break;
        
        case BPF_TAP_INPUT_OUTPUT:
            ifv->ifv_bpf_input = ifv->ifv_bpf_output = func;
            break;
        default:
            break;
    }
    return 0;
}

static void
vlan_ifinit(void *foo)
{
    return;
}

static int
vlan_output(struct ifnet *ifp, struct mbuf *m)
{
    struct ifvlan *ifv;
    struct ifnet *p;
    struct ether_vlan_header *evl;
    int soft_vlan;

    ifv = ifp->if_private;
    p = ifv->ifv_p;
    if (p == NULL) {
	return (nop_if_output(ifp, m));
    }
    if (m == 0) {
	printf("%s: NULL output mbuf\n", ifv->ifv_name);
	return (EINVAL);
    }
    if ((m->m_flags & M_PKTHDR) == 0) {
	printf("%s: M_PKTHDR bit not set\n", ifv->ifv_name);
	m_freem(m);
	return (EINVAL);
    }
    ifp->if_obytes += m->m_pkthdr.len;
    ifp->if_opackets++;
    soft_vlan = (p->if_hwassist & IF_HWASSIST_VLAN_TAGGING) == 0;
    vlan_bpf_output(ifp, m, ifv->ifv_bpf_output);

    /* do not run parent's if_output() if the parent is not up */
    if ((p->if_flags & (IFF_UP | IFF_RUNNING)) != (IFF_UP | IFF_RUNNING)) {
	m_freem(m);
	ifp->if_collisions++;
	return (0);
    }
    /*
     * If underlying interface can do VLAN tag insertion itself,
     * just pass the packet along. However, we need some way to
     * tell the interface where the packet came from so that it
     * knows how to find the VLAN tag to use.  We use a field in
     * the mbuf header to store the VLAN tag, and a bit in the
     * csum_flags field to mark the field as valid.
     */
    if (soft_vlan == 0) {
	m->m_pkthdr.csum_flags |= CSUM_VLAN_TAG_VALID;
	m->m_pkthdr.vlan_tag = ifv->ifv_tag;
    } else {
	M_PREPEND(m, ifv->ifv_encaplen, M_DONTWAIT);
	if (m == NULL) {
	    printf("%s: unable to prepend VLAN header\n", 
		   ifv->ifv_name);
	    ifp->if_ierrors++;
	    return (0);
	}
	/* M_PREPEND takes care of m_len, m_pkthdr.len for us */
	if (m->m_len < sizeof(*evl)) {
	    m = m_pullup(m, sizeof(*evl));
	    if (m == NULL) {
		printf("%s: cannot pullup VLAN header\n",
		       ifv->ifv_name);
		ifp->if_ierrors++;
		return (0);
	    }
	}
		
	/*
	 * Transform the Ethernet header into an Ethernet header
	 * with 802.1Q encapsulation.
	 */
	bcopy(mtod(m, char *) + ifv->ifv_encaplen,
	      mtod(m, char *), ETHER_HDR_LEN);
	evl = mtod(m, struct ether_vlan_header *);
	evl->evl_proto = evl->evl_encap_proto;
	evl->evl_encap_proto = htons(ETHERTYPE_VLAN);
	evl->evl_tag = htons(ifv->ifv_tag);
	m->m_pkthdr.len += ifv->ifv_encaplen;
    }
	
    /*
     * Send it, precisely as ether_output() would have.
     * We are already running at splimp.
     */
    return ((*p->if_output)(p, m));
}

extern int 
vlan_demux(struct ifnet * ifp, struct mbuf * m, 
	   char * frame_header, struct if_proto * * proto)
{
    register struct ether_header *eh = (struct ether_header *)frame_header;
    struct ether_vlan_header *evl;
    struct ifvlan *ifv = NULL;
    int soft_vlan = 0;
    u_int tag;

    if (m->m_pkthdr.csum_flags & CSUM_VLAN_TAG_VALID) {
	/*
	 * Packet is tagged, m contains a normal
	 * Ethernet frame; the tag is stored out-of-band.
	 */
	m->m_pkthdr.csum_flags &= ~CSUM_VLAN_TAG_VALID;
	tag = EVL_VLANOFTAG(m->m_pkthdr.vlan_tag);
	m->m_pkthdr.vlan_tag = 0;
    } else {
	soft_vlan = 1;

	switch (ifp->if_type) {
	case IFT_ETHER:
	    if (m->m_len < ETHER_VLAN_ENCAP_LEN) {
		m_freem(m);
		return (EJUSTRETURN);
	    }
	    evl = (struct ether_vlan_header *)frame_header;
	    if (ntohs(evl->evl_proto) == ETHERTYPE_VLAN) {
		/* don't allow VLAN within VLAN */
		m_freem(m);
		return (EJUSTRETURN);
	    }
	    tag = EVL_VLANOFTAG(ntohs(evl->evl_tag));

	    /*
	     * Restore the original ethertype.  We'll remove
	     * the encapsulation after we've found the vlan
	     * interface corresponding to the tag.
	     */
	    evl->evl_encap_proto = evl->evl_proto;
	    break;
	default:
	    printf("vlan_demux: unsupported if type %u", 
		   ifp->if_type);
	    m_freem(m);
	    return (EJUSTRETURN);
	    break;
	}
    }
    if (tag != 0) {
	if (ifp->if_nvlans == 0) {
	    /* don't bother looking through the VLAN list */
	    m_freem(m);
	    ifp->if_noproto++;
	    return (EJUSTRETURN);
	}
	VLAN_LOCK();
	ifv = vlan_lookup_ifp_and_tag(ifp, tag);
	if (ifv == NULL || (ifv->ifv_ifp->if_flags & IFF_UP) == 0) {
	    VLAN_UNLOCK();
	    m_freem(m);
	    ifp->if_noproto++;
	    return (EJUSTRETURN);
	}
	VLAN_UNLOCK();		/* XXX extend below? */
    }
    if (soft_vlan) {
	/*
	 * Packet had an in-line encapsulation header;
	 * remove it.  The original header has already
	 * been fixed up above.
	 */
	m->m_len -= ETHER_VLAN_ENCAP_LEN;
	m->m_data += ETHER_VLAN_ENCAP_LEN;
	m->m_pkthdr.len -= ETHER_VLAN_ENCAP_LEN;
	m->m_pkthdr.csum_flags = 0; /* can't trust hardware checksum */
    }
    if (tag != 0) {
	/* we found a vlan interface above, so send it up */
	m->m_pkthdr.rcvif = ifv->ifv_ifp;
	ifv->ifv_ifp->if_ipackets++;
	ifv->ifv_ifp->if_ibytes += m->m_pkthdr.len;

	vlan_bpf_input(ifv->ifv_ifp, m, ifv->ifv_bpf_input, frame_header,
		       ETHER_HDR_LEN, soft_vlan ? ETHER_VLAN_ENCAP_LEN : 0);

	/* Pass it back through the parent's demux routine. */
	return ((*ifp->if_demux)(ifv->ifv_ifp, m, frame_header, proto));
    }
    /* Pass it back through calling demux routine. */
    return ((*ifp->if_demux)(ifp, m, frame_header, proto));
}

static int
vlan_config(struct ifvlan *ifv, struct ifnet *p, int tag)
{
    struct ifnet * ifp;
    struct ifaddr *ifa1, *ifa2;
    struct sockaddr_dl *sdl1, *sdl2;
    int supports_vlan_mtu = 0;

    VLAN_LOCK_ASSERT();
    if (p->if_data.ifi_type != IFT_ETHER)
	return EPROTONOSUPPORT;
    if (ifv->ifv_p != NULL || ifv->ifv_detaching) {
	return EBUSY;
    }
    if (vlan_lookup_ifp_and_tag(p, tag) != NULL) {
	/* already a VLAN with that tag on this interface */
	return (EADDRINUSE);
    }
    ifp = ifv->ifv_ifp;
    ifv->ifv_encaplen = ETHER_VLAN_ENCAP_LEN;
    ifv->ifv_mintu = ETHERMIN;
    ifv->ifv_flags = 0;

    /*
     * If the parent supports the VLAN_MTU capability,
     * i.e. can Tx/Rx larger than ETHER_MAX_LEN frames,
     * enable it.
     */
    if (p->if_hwassist & (IF_HWASSIST_VLAN_MTU | IF_HWASSIST_VLAN_TAGGING)) {
	supports_vlan_mtu = 1;
    }
    if (p->if_nvlans == 0) {
	u_long	dltag;
	u_long	filter_id;
	int 	error;

	/* attach our VLAN "interface filter" to the interface */
	error = vlan_attach_filter(p, &filter_id);
	if (error) {
	    return (error);
	}

	/* attach our VLAN "protocol" to the interface */
	error = vlan_attach_protocol(p);
	if (error) {
	    (void)vlan_detach_filter(filter_id);
	    return (error);
	}
	ifv->ifv_filter_id = filter_id;
	ifv->ifv_filter_valid = TRUE;
#if 0
	if (supports_vlan_mtu) {
	    /*
	     * Enable Tx/Rx of VLAN-sized frames.
	     */
	    p->if_capenable |= IFCAP_VLAN_MTU;
	    if (p->if_flags & IFF_UP) {
		struct ifreq ifr;
		int error;
		
		ifr.ifr_flags = p->if_flags;
		error = (*p->if_ioctl)(p, SIOCSIFFLAGS,
				       (caddr_t) &ifr);
		if (error) {
		    if (p->if_nvlans == 0)
			p->if_capenable &= ~IFCAP_VLAN_MTU;
		    return (error);
		}
	    }
	}
#endif 0
    } else {
	struct ifvlan * 	other_ifv;

	other_ifv = vlan_lookup_ifp(p);
	if (other_ifv == NULL) {
	    printf("vlan: other_ifv can't be NULL\n");
	    return (EINVAL);
	}
	ifv->ifv_filter_id = other_ifv->ifv_filter_id;
	ifv->ifv_filter_valid = TRUE;
    }
    p->if_nvlans++;
    if (supports_vlan_mtu) {
	ifv->ifv_mtufudge = 0;
    } else {
	/*
	 * Fudge the MTU by the encapsulation size.  This
	 * makes us incompatible with strictly compliant
	 * 802.1Q implementations, but allows us to use
	 * the feature with other NetBSD implementations,
	 * which might still be useful.
	 */
	ifv->ifv_mtufudge = ifv->ifv_encaplen;
    }

    ifv->ifv_p = p;
    ifp->if_mtu = p->if_mtu - ifv->ifv_mtufudge;
    /*
     * Copy only a selected subset of flags from the parent.
     * Other flags are none of our business.
     */
    ifp->if_flags |= (p->if_flags &
		      (IFF_BROADCAST | IFF_MULTICAST | IFF_SIMPLEX));
    /*
     * If the parent interface can do hardware-assisted
     * VLAN encapsulation, then propagate its hardware-
     * assisted checksumming flags.
     */
    if (p->if_hwassist & IF_HWASSIST_VLAN_TAGGING) {
	ifp->if_hwassist |= IF_HWASSIST_CSUM_FLAGS(p->if_hwassist);
    }
    /*
     * Set up our ``Ethernet address'' to reflect the underlying
     * physical interface's.
     */
    ifa1 = ifaddr_byindex(ifp->if_index);
    ifa2 = ifaddr_byindex(p->if_index);
    sdl1 = (struct sockaddr_dl *)ifa1->ifa_addr;
    sdl2 = (struct sockaddr_dl *)ifa2->ifa_addr;
    sdl1->sdl_type = IFT_ETHER;
    sdl1->sdl_alen = ETHER_ADDR_LEN;
    bcopy(LLADDR(sdl2), LLADDR(sdl1), ETHER_ADDR_LEN);
    bcopy(LLADDR(sdl2), IFP2AC(ifp)->ac_enaddr, ETHER_ADDR_LEN);

    /*
     * Configure multicast addresses that may already be
     * joined on the vlan device.
     */
    (void)vlan_setmulti(ifp);
    ifp->if_output = vlan_output;
    ifv->ifv_tag = tag;

    return 0;
}

static void
vlan_link_event(struct ifnet * ifp, struct ifnet * p)
{
    struct ifmediareq ifmr;

    /* generate a link event based on the state of the underlying interface */
    bzero(&ifmr, sizeof(ifmr));
    snprintf(ifmr.ifm_name, sizeof(ifmr.ifm_name),
	     "%s%d", p->if_name, p->if_unit);
    if ((*p->if_ioctl)(p, SIOCGIFMEDIA, (caddr_t)&ifmr) == 0
	&& ifmr.ifm_count > 0 && ifmr.ifm_status & IFM_AVALID) {
	u_long	event;
	
	event = (ifmr.ifm_status & IFM_ACTIVE)
	    ? KEV_DL_LINK_ON : KEV_DL_LINK_OFF;
	interface_link_event(ifp, event);
    }
    return;
>>>>>>> origin/10.3
}

static int
vlan_set_promisc(struct ifnet * ifp)
{
<<<<<<< HEAD
    int 			error = 0;
    ifvlan_ref			ifv;
    vlan_parent_ref		vlp;

    vlan_lock();
    ifv = ifnet_get_ifvlan_retained(ifp);
    if (ifv == NULL) {
	error = EBUSY;
	goto done;
    }

    vlp = ifv->ifv_vlp;
    if (vlp == NULL) {
	goto done;
    }
    if ((ifnet_flags(ifp) & IFF_PROMISC) != 0) {
	if (!ifvlan_flags_promisc(ifv)) {
	    error = ifnet_set_promiscuous(vlp->vlp_ifp, 1);
	    if (error == 0) {
		ifvlan_flags_set_promisc(ifv);
	    }
	}
    } else {
	if (ifvlan_flags_promisc(ifv)) {
	    error = ifnet_set_promiscuous(vlp->vlp_ifp, 0);
	    if (error == 0) {
		ifvlan_flags_clear_promisc(ifv);
	    }
	}
    }
 done:
    vlan_unlock();
    if (ifv != NULL) {
	ifvlan_release(ifv);
    }
    return (error);
}

static int
ifvlan_new_mtu(ifvlan_ref ifv, int mtu)
{
    struct ifdevmtu *	devmtu_p;
    int			error = 0;
    struct ifnet * 	ifp = ifv->ifv_ifp;
    int			max_mtu;
    int			new_mtu = 0;
    int			req_mtu;
    vlan_parent_ref	vlp;

    vlan_assert_lock_held();
    vlp = ifv->ifv_vlp;
    devmtu_p = &vlp->vlp_devmtu;
    req_mtu = mtu + ifv->ifv_mtufudge;
    if (req_mtu > devmtu_p->ifdm_max || req_mtu < devmtu_p->ifdm_min) {
	return (EINVAL);
    }
    max_mtu = vlan_parent_find_max_mtu(vlp, ifv);
    if (req_mtu > max_mtu) {
	new_mtu = req_mtu;
    }
    else if (max_mtu < devmtu_p->ifdm_current) {
	new_mtu = max_mtu;
    }
    if (new_mtu != 0) {
	struct ifnet * 	p = vlp->vlp_ifp;
	vlan_unlock();
	error = siocsifaltmtu(p, new_mtu);
	vlan_lock();
    }
    if (error == 0) {
	if (new_mtu != 0) {
	    devmtu_p->ifdm_current = new_mtu;
	}
	ifnet_set_mtu(ifp, mtu);
    }
    return (error);
}

static int
vlan_set_mtu(struct ifnet * ifp, int mtu)
{
    int			error = 0;
    ifvlan_ref		ifv;
    vlan_parent_ref	vlp;

    if (mtu < IF_MINMTU) {
	return (EINVAL);
    }
    vlan_lock();
    ifv = ifnet_get_ifvlan_retained(ifp);
    if (ifv == NULL) {
	vlan_unlock();
	return (EBUSY);
    }
    vlp = ifvlan_get_vlan_parent_retained(ifv);
    if (vlp == NULL) {
	vlan_unlock();
	ifvlan_release(ifv);
	if (mtu != 0) {
	    return (EINVAL);
	}
	return (0);
    }
    vlan_parent_wait(vlp, "vlan_set_mtu");

    /* check again, something might have changed */
    if (ifnet_get_ifvlan(ifp) != ifv
	|| ifvlan_flags_detaching(ifv)) {
	error = EBUSY;
	goto signal_done;
    }
    if (ifv->ifv_vlp != vlp) {
	/* vlan parent changed */
	goto signal_done;
    }
    if (vlan_parent_flags_detaching(vlp)) {
	if (mtu != 0) {
	    error = EINVAL;
	}
	goto signal_done;
    }
    error = ifvlan_new_mtu(ifv, mtu);

 signal_done:
    vlan_parent_signal(vlp, "vlan_set_mtu");
    vlan_unlock();
    vlan_parent_release(vlp);
    ifvlan_release(ifv);
=======
    struct ifaddr *ifa;
    struct sockaddr_dl *sdl;
    struct vlan_mc_entry *mc;
    struct ifvlan *ifv;
    struct ifnet *p;
    int error;

    VLAN_LOCK_ASSERT();

    ifv = ifp->if_private;

    /* Disconnect from parent. */
    p = ifv->ifv_p;
    ifv->ifv_p = NULL;

    if (p != NULL) {
	struct sockaddr_dl sdl;

	/*
	 * Since the interface is being unconfigured, we need to
	 * empty the list of multicast groups that we may have joined
	 * while we were alive from the parent's list.
	 */
	bzero((char *)&sdl, sizeof sdl);
	sdl.sdl_len = sizeof sdl;
	sdl.sdl_family = AF_LINK;
	sdl.sdl_index = p->if_index;
	sdl.sdl_type = IFT_ETHER;
	sdl.sdl_alen = ETHER_ADDR_LEN;

	while (SLIST_FIRST(&ifv->vlan_mc_listhead) != NULL) {
	    mc = SLIST_FIRST(&ifv->vlan_mc_listhead);
	    bcopy((char *)&mc->mc_addr, LLADDR(&sdl), ETHER_ADDR_LEN);
	    error = if_delmulti(p, (struct sockaddr *)&sdl);
	    if (error) {
		printf("vlan_unconfig: if_delmulti %s failed, %d\n", 
		       ifv->ifv_name, error);
	    }
	    SLIST_REMOVE_HEAD(&ifv->vlan_mc_listhead, mc_entries);
	    FREE(mc, M_VLAN);
	}
	p->if_nvlans--;
	if (p->if_nvlans == 0) {
	    /* detach our VLAN "protocol" from the interface */
	    if (ifv->ifv_filter_valid) {
		(void)vlan_detach_filter(ifv->ifv_filter_id);
	    }
	    (void)vlan_detach_protocol(p);
#if 0
	    /*
	     * Disable Tx/Rx of VLAN-sized frames.
	     */
	    p->if_capenable &= ~IFCAP_VLAN_MTU;
	    if (p->if_flags & IFF_UP) {
		struct ifreq ifr;
		
		ifr.ifr_flags = p->if_flags;
		(*p->if_ioctl)(p, SIOCSIFFLAGS, (caddr_t) &ifr);
	    }
#endif 0
	}
    }

    /* return to the state we were in before SETVLAN */
    ifp->if_mtu = 0;
    ifp->if_flags &= ~(IFF_BROADCAST | IFF_MULTICAST 
		       | IFF_SIMPLEX | IFF_RUNNING);
    ifv->ifv_ifp->if_hwassist = 0;
    ifv->ifv_flags = 0;
    ifv->ifv_ifp->if_output = nop_if_output;
    ifv->ifv_mtufudge = 0;
    ifv->ifv_filter_valid = FALSE;

    /* Clear our MAC address. */
    ifa = ifaddr_byindex(ifv->ifv_ifp->if_index);
    sdl = (struct sockaddr_dl *)(ifa->ifa_addr);
    sdl->sdl_type = IFT_L2VLAN;
    sdl->sdl_alen = 0;
    bzero(LLADDR(sdl), ETHER_ADDR_LEN);
    bzero(IFP2AC(ifv->ifv_ifp)->ac_enaddr, ETHER_ADDR_LEN);

    /* send a link down event */
    if (p != NULL) {
	interface_link_event(ifv->ifv_ifp, KEV_DL_LINK_OFF);
    }
    return 0;
}

static int
vlan_set_promisc(struct ifnet *ifp)
{
    struct ifvlan *ifv = ifp->if_private;
    int error = 0;

    if ((ifp->if_flags & IFF_PROMISC) != 0) {
	if ((ifv->ifv_flags & IFVF_PROMISC) == 0) {
	    error = ifpromisc(ifv->ifv_p, 1);
	    if (error == 0)
		ifv->ifv_flags |= IFVF_PROMISC;
	}
    } else {
	if ((ifv->ifv_flags & IFVF_PROMISC) != 0) {
	    error = ifpromisc(ifv->ifv_p, 0);
	    if (error == 0)
		ifv->ifv_flags &= ~IFVF_PROMISC;
	}
    }
>>>>>>> origin/10.3

    return (error);
}

static int
<<<<<<< HEAD
vlan_ioctl(ifnet_t ifp, u_long cmd, void * data)
{
    struct ifdevmtu *	devmtu_p;
    int 		error = 0;
    struct ifaddr *	ifa;
    struct ifmediareq	*ifmr;
    struct ifreq *	ifr;
    ifvlan_ref		ifv;
    struct ifnet *	p;
    u_short		tag;
    user_addr_t		user_addr;
    vlan_parent_ref	vlp;
    struct vlanreq 	vlr;

    if (ifnet_type(ifp) != IFT_L2VLAN) {
	return (EOPNOTSUPP);
    }
    ifr = (struct ifreq *)data;
    ifa = (struct ifaddr *)data;

    switch (cmd) {
    case SIOCSIFADDR:
    ifnet_set_flags(ifp, IFF_UP, IFF_UP);
	break;

    case SIOCGIFMEDIA32:
    case SIOCGIFMEDIA64:
	vlan_lock();
	ifv = (ifvlan_ref)ifnet_softc(ifp);
	if (ifv == NULL || ifvlan_flags_detaching(ifv)) {
	    vlan_unlock();
	    return (ifv == NULL ? EOPNOTSUPP : EBUSY);
	}
	p = (ifv->ifv_vlp == NULL) ? NULL : ifv->ifv_vlp->vlp_ifp;
	vlan_unlock();
	ifmr = (struct ifmediareq *)data;
	user_addr =  (cmd == SIOCGIFMEDIA64) ?
	    ((struct ifmediareq64 *)ifmr)->ifmu_ulist :
	    CAST_USER_ADDR_T(((struct ifmediareq32 *)ifmr)->ifmu_ulist);
	if (p != NULL) {
	    struct ifmediareq p_ifmr;

	    bzero(&p_ifmr, sizeof(p_ifmr));
	    error = ifnet_ioctl(p, 0, SIOCGIFMEDIA, &p_ifmr);
	    if (error == 0) {
		ifmr->ifm_active = p_ifmr.ifm_active;
		ifmr->ifm_current = p_ifmr.ifm_current;
		ifmr->ifm_mask = p_ifmr.ifm_mask;
		ifmr->ifm_status = p_ifmr.ifm_status;
		ifmr->ifm_count = p_ifmr.ifm_count;
		/* Limit the result to the parent's current config. */
		if (ifmr->ifm_count >= 1 && user_addr != USER_ADDR_NULL) {
		    ifmr->ifm_count = 1;
		    error = copyout(&ifmr->ifm_current, user_addr, 
=======
vlan_ioctl(struct ifnet *ifp, u_long cmd, void * data)
{
    struct ifaddr *ifa;
    struct ifnet *p;
    struct ifreq *ifr;
    struct ifvlan *ifv;
    struct vlanreq vlr;
    int error = 0;

    ifr = (struct ifreq *)data;
    ifa = (struct ifaddr *)data;
    ifv = (struct ifvlan *)ifp->if_private;

    switch (cmd) {
    case SIOCSIFADDR:
	ifp->if_flags |= IFF_UP;
	break;

    case SIOCGIFMEDIA:
	VLAN_LOCK();
	if (ifv->ifv_p != NULL) {
	    error = (*ifv->ifv_p->if_ioctl)(ifv->ifv_p,
					    SIOCGIFMEDIA, data);
	    VLAN_UNLOCK();
	    /* Limit the result to the parent's current config. */
	    if (error == 0) {
		struct ifmediareq *ifmr;

		ifmr = (struct ifmediareq *) data;
		if (ifmr->ifm_count >= 1 && ifmr->ifm_ulist) {
		    ifmr->ifm_count = 1;
		    error = copyout(&ifmr->ifm_current,
				    ifmr->ifm_ulist, 
>>>>>>> origin/10.3
				    sizeof(int));
		}
	    }
	} else {
<<<<<<< HEAD
	    ifmr->ifm_active = ifmr->ifm_current = IFM_NONE;
	    ifmr->ifm_mask = 0;
	    ifmr->ifm_status = IFM_AVALID;
	    ifmr->ifm_count = 1;
	    if (user_addr != USER_ADDR_NULL) {
		error = copyout(&ifmr->ifm_current, user_addr, sizeof(int));
	    }
=======
	    struct ifmediareq *ifmr;
	    VLAN_UNLOCK();
	    
	    ifmr = (struct ifmediareq *) data;
	    ifmr->ifm_current = 0;
	    ifmr->ifm_mask = 0;
	    ifmr->ifm_status = IFM_AVALID;
	    ifmr->ifm_active = 0;
	    ifmr->ifm_count = 1;
	    if (ifmr->ifm_ulist) {
		error = copyout(&ifmr->ifm_current,
				ifmr->ifm_ulist, 
				sizeof(int));
	    }
	    error = 0;
>>>>>>> origin/10.3
	}
	break;

    case SIOCSIFMEDIA:
<<<<<<< HEAD
	error = EOPNOTSUPP;
	break;

    case SIOCGIFDEVMTU:
	vlan_lock();
	ifv = (ifvlan_ref)ifnet_softc(ifp);
	if (ifv == NULL || ifvlan_flags_detaching(ifv)) {
	    vlan_unlock();
	    return (ifv == NULL ? EOPNOTSUPP : EBUSY);
	}
	vlp = ifv->ifv_vlp;
	if (vlp != NULL) {
	    int		min_mtu = vlp->vlp_devmtu.ifdm_min - ifv->ifv_mtufudge;
	    devmtu_p = &ifr->ifr_devmtu;
	    devmtu_p->ifdm_current = ifnet_mtu(ifp);
	    devmtu_p->ifdm_min = max(min_mtu, IF_MINMTU);
	    devmtu_p->ifdm_max = vlp->vlp_devmtu.ifdm_max - ifv->ifv_mtufudge;
	}
	else {
	    devmtu_p = &ifr->ifr_devmtu;
	    devmtu_p->ifdm_current = 0;
	    devmtu_p->ifdm_min = 0;
	    devmtu_p->ifdm_max = 0;
	}
	vlan_unlock();
	break;

    case SIOCSIFMTU:
	error = vlan_set_mtu(ifp, ifr->ifr_mtu);
	break;

    case SIOCSIFVLAN:
	user_addr = proc_is64bit(current_proc()) 
	    ? ifr->ifr_data64 : CAST_USER_ADDR_T(ifr->ifr_data);
	error = copyin(user_addr, &vlr, sizeof(vlr));
	if (error) {
	    break;
	}
	p = NULL;
	if (vlr.vlr_parent[0] != '\0') {
	    if (vlr.vlr_tag & ~EVL_VLID_MASK) {
		/*
		 * Don't let the caller set up a VLAN tag with
		 * anything except VLID bits.
		 */
		error = EINVAL;
		break;
	    }
	    p = ifunit(vlr.vlr_parent);
	    if (p == NULL) {
		error = ENXIO;
		break;
	    }
	    /* can't do VLAN over anything but ethernet or ethernet aggregate */
	    if (ifnet_type(p) != IFT_ETHER 
		&& ifnet_type(p) != IFT_IEEE8023ADLAG) {
		error = EPROTONOSUPPORT;
		break;
	    }
	    error = vlan_config(ifp, p, vlr.vlr_tag);
	    if (error) {
		break;
	    }
	    
	    /* Update promiscuous mode, if necessary. */
	    (void)vlan_set_promisc(ifp);
	    
	    /* generate a link event based on the state of the parent */
	    vlan_link_event(ifp, p);
	} 
	else {
	    int		need_link_event = FALSE;

	    vlan_lock();
	    ifv = (ifvlan_ref)ifnet_softc(ifp);
	    if (ifv == NULL || ifvlan_flags_detaching(ifv)) {
		vlan_unlock();
		error = (ifv == NULL ? EOPNOTSUPP : EBUSY);
		break;
	    }
	    need_link_event = vlan_remove(ifv, TRUE);
	    vlan_unlock();
	    if (need_link_event) {
		interface_link_event(ifp, KEV_DL_LINK_OFF);
	    }
	}
	break;
		
    case SIOCGIFVLAN:
	bzero(&vlr, sizeof vlr);
	vlan_lock();
	ifv = (ifvlan_ref)ifnet_softc(ifp);
	if (ifv == NULL || ifvlan_flags_detaching(ifv)) {
	    vlan_unlock();
	    return (ifv == NULL ? EOPNOTSUPP : EBUSY);
	}
	p = (ifv->ifv_vlp == NULL) ? NULL : ifv->ifv_vlp->vlp_ifp;
	tag = ifv->ifv_tag;
	vlan_unlock();
	if (p != NULL) {
	    snprintf(vlr.vlr_parent, sizeof(vlr.vlr_parent),
		     "%s%d", ifnet_name(p), ifnet_unit(p));
	    vlr.vlr_tag = tag;
	}
	user_addr = proc_is64bit(current_proc()) 
	    ? ifr->ifr_data64 : CAST_USER_ADDR_T(ifr->ifr_data);
	error = copyout(&vlr, user_addr, sizeof(vlr));
	break;
		
    case SIOCSIFFLAGS:
	/*
	 * For promiscuous mode, we enable promiscuous mode on
	 * the parent if we need promiscuous on the VLAN interface.
	 */
	error = vlan_set_promisc(ifp);
	break;

    case SIOCADDMULTI:
    case SIOCDELMULTI:
	error = vlan_setmulti(ifp);
	break;
    default:
	error = EOPNOTSUPP;
    }
    return error;
}

static void 
vlan_if_free(struct ifnet * ifp)
{
    ifvlan_ref	ifv;

    if (ifp == NULL) {
	return;
    }
    ifv = (ifvlan_ref)ifnet_softc(ifp);
    if (ifv == NULL) {
	return;
    }
    ifvlan_release(ifv);
    ifnet_release(ifp);
    return;
}

static void
vlan_event(struct ifnet	* p, __unused protocol_family_t protocol,
		   const struct kev_msg * event)
{
    int			event_code;

    /* Check if the interface we are attached to is being detached */
    if (event->vendor_code != KEV_VENDOR_APPLE
	|| event->kev_class != KEV_NETWORK_CLASS
	|| event->kev_subclass != KEV_DL_SUBCLASS) {
	return;
    }
    event_code = event->event_code;
    switch (event_code) {
    case KEV_DL_LINK_OFF:
    case KEV_DL_LINK_ON:
	vlan_parent_link_event(p, event_code);
	break;
    default:
	return;
    }
    return;
}

static errno_t
vlan_detached(ifnet_t p, __unused protocol_family_t protocol)
{
    if (ifnet_is_attached(p, 0) == 0) {
	/* if the parent isn't attached, remove all VLANs */
	vlan_parent_remove_all_vlans(p);
    }
    return (0);
}

static void
interface_link_event(struct ifnet * ifp, u_int32_t event_code)
{
    struct {
	struct kern_event_msg	header;
	u_int32_t			unit;
	char			if_name[IFNAMSIZ];
    } event;

    bzero(&event, sizeof(event));
    event.header.total_size    = sizeof(event);
    event.header.vendor_code   = KEV_VENDOR_APPLE;
    event.header.kev_class     = KEV_NETWORK_CLASS;
    event.header.kev_subclass  = KEV_DL_SUBCLASS;
    event.header.event_code    = event_code;
    event.header.event_data[0] = ifnet_family(ifp);
    event.unit                 = (u_int32_t) ifnet_unit(ifp);
    strlcpy(event.if_name, ifnet_name(ifp), IFNAMSIZ);
    ifnet_event(ifp, &event.header);
    return;
}

static void
vlan_parent_link_event(struct ifnet * p, u_int32_t event_code)
{
    vlan_parent_ref 	vlp;

    vlan_lock();
    if ((ifnet_eflags(p) & IFEF_VLAN) == 0) {
	vlan_unlock();
	/* no VLAN's */
	return;
    }
    vlp = parent_list_lookup(p);
    if (vlp == NULL) {
	/* no VLAN's */
	vlan_unlock();
	return;
    }
    vlan_parent_flags_set_link_event_required(vlp);
    vlp->vlp_event_code = event_code;
    if (vlan_parent_flags_change_in_progress(vlp)) {
	/* don't block waiting to generate an event */
	vlan_unlock();
	return;
    }
    vlan_parent_retain(vlp);
    vlan_parent_wait(vlp, "vlan_parent_link_event");
    vlan_parent_signal(vlp, "vlan_parent_link_event");
    vlan_unlock();
    vlan_parent_release(vlp);
    return;

}

/*
 * Function: vlan_attach_protocol
 * Purpose:
 *   Attach a DLIL protocol to the interface, using the ETHERTYPE_VLAN
 *   demux ether type.
 *
 *	 The ethernet demux actually special cases VLAN to support hardware.
 *	 The demux here isn't used. The demux will return PF_VLAN for the
 *	 appropriate packets and our vlan_input function will be called.
 */
static int
vlan_attach_protocol(struct ifnet *ifp)
{
    int								error;
    struct ifnet_attach_proto_param	reg;
	
    bzero(&reg, sizeof(reg));
    reg.input            = vlan_input;
    reg.event            = vlan_event;
    reg.detached         = vlan_detached;
    error = ifnet_attach_protocol(ifp, PF_VLAN, &reg);
    if (error) {
	printf("vlan_proto_attach(%s%d) ifnet_attach_protocol failed, %d\n",
	       ifnet_name(ifp), ifnet_unit(ifp), error);
    }
    return (error);
}

/*
 * Function: vlan_detach_protocol
 * Purpose:
 *   Detach our DLIL protocol from an interface
 */
static int
vlan_detach_protocol(struct ifnet *ifp)
{
    int         error;

    error = ifnet_detach_protocol(ifp, PF_VLAN);
    if (error) {
	printf("vlan_proto_detach(%s%d) ifnet_detach_protocol failed, %d\n",
	       ifnet_name(ifp), ifnet_unit(ifp), error);
    }
	
    return (error);
}

/*
 * DLIL interface family functions
 *   We use the ethernet plumb functions, since that's all we support.
 *   If we wanted to handle multiple LAN types (tokenring, etc.), we'd
 *   call the appropriate routines for that LAN type instead of hard-coding
 *   ethernet.
 */
static errno_t
vlan_attach_inet(struct ifnet *ifp, protocol_family_t protocol_family)
{
    return (ether_attach_inet(ifp, protocol_family));
}

static void
vlan_detach_inet(struct ifnet *ifp, protocol_family_t protocol_family)
{
    ether_detach_inet(ifp, protocol_family);
}

#if INET6
static errno_t
vlan_attach_inet6(struct ifnet *ifp, protocol_family_t protocol_family)
{
    return (ether_attach_inet6(ifp, protocol_family));
}

static void
vlan_detach_inet6(struct ifnet *ifp, protocol_family_t protocol_family)
{
    ether_detach_inet6(ifp, protocol_family);
}
#endif /* INET6 */

__private_extern__ int
vlan_family_init(void)
{
    int error=0;

    error = proto_register_plumber(PF_INET, IFNET_FAMILY_VLAN, 
				   vlan_attach_inet, vlan_detach_inet);
    if (error != 0) {
	printf("proto_register_plumber failed for AF_INET error=%d\n",
	       error);
	goto done;
    }
#if INET6
    error = proto_register_plumber(PF_INET6, IFNET_FAMILY_VLAN, 
				   vlan_attach_inet6, vlan_detach_inet6);
    if (error != 0) {
	printf("proto_register_plumber failed for AF_INET6 error=%d\n",
	       error);
	goto done;
    }
#endif
    error = vlan_clone_attach();
    if (error != 0) {
        printf("proto_register_plumber failed vlan_clone_attach error=%d\n",
               error);
        goto done;
    }


 done:
=======
	error = EINVAL;
	break;

    case SIOCSIFMTU:
	/*
	 * Set the interface MTU.
	 */
	VLAN_LOCK();
	if (ifv->ifv_p != NULL) {
	    if (ifr->ifr_mtu > (ifv->ifv_p->if_mtu - ifv->ifv_mtufudge)
		|| ifr->ifr_mtu < (ifv->ifv_mintu - ifv->ifv_mtufudge)) {
		error = EINVAL;
	    } else {
		ifp->if_mtu = ifr->ifr_mtu;
	    }
	} else {
	    error = EINVAL;
	}
	VLAN_UNLOCK();
	break;

    case SIOCSETVLAN:
	error = copyin(ifr->ifr_data, &vlr, sizeof(vlr));
	if (error)
	    break;
	if (vlr.vlr_parent[0] == '\0') {
	    VLAN_LOCK();
	    vlan_unconfig(ifp);
#if 0
	    if (ifp->if_flags & IFF_UP)
		if_down(ifp);
	    ifp->if_flags &= ~IFF_RUNNING;
#endif 0
	    VLAN_UNLOCK();
	    break;
	}
	p = ifunit(vlr.vlr_parent);
	if (p == 0) {
	    error = ENOENT;
	    break;
	}
	/*
	 * Don't let the caller set up a VLAN tag with
	 * anything except VLID bits.
	 */
	if (vlr.vlr_tag & ~EVL_VLID_MASK) {
	    error = EINVAL;
	    break;
	}
	VLAN_LOCK();
	error = vlan_config(ifv, p, vlr.vlr_tag);
	if (error) {
	    VLAN_UNLOCK();
	    break;
	}
	ifp->if_flags |= IFF_RUNNING;
	VLAN_UNLOCK();

	/* Update promiscuous mode, if necessary. */
	vlan_set_promisc(ifp);

	/* generate a link event */
	vlan_link_event(ifp, p);
	break;
		
    case SIOCGETVLAN:
	bzero(&vlr, sizeof vlr);
	VLAN_LOCK();
	if (ifv->ifv_p != NULL) {
	    snprintf(vlr.vlr_parent, sizeof(vlr.vlr_parent),
		     "%s%d", ifv->ifv_p->if_name, 
		     ifv->ifv_p->if_unit);
	    vlr.vlr_tag = ifv->ifv_tag;
	}
	VLAN_UNLOCK();
	error = copyout(&vlr, ifr->ifr_data, sizeof vlr);
	break;
		
    case SIOCSIFFLAGS:
	/*
	 * For promiscuous mode, we enable promiscuous mode on
	 * the parent if we need promiscuous on the VLAN interface.
	 */
	if (ifv->ifv_p != NULL)
	    error = vlan_set_promisc(ifp);
	break;

    case SIOCADDMULTI:
    case SIOCDELMULTI:
	error = vlan_setmulti(ifp);
	break;
    default:
	error = EOPNOTSUPP;
    }
    return error;
}

static int 
nop_if_ioctl(struct ifnet * ifp, u_long cmd, void * data)
{
    return EOPNOTSUPP;
}

static int 
nop_if_bpf(struct ifnet *ifp, int mode, bpf_callback_func * func)
{
    return ENODEV;
}

static int 
nop_if_free(struct ifnet * ifp)
{
    return 0;
}

static int 
nop_if_output(struct ifnet * ifp, struct mbuf * m)
{
    if (m != NULL) {
	m_freem_list(m);
    }
    return 0;
}

static int 
vlan_if_free(struct ifnet * ifp)
{
    struct ifvlan *ifv;

    if (ifp == NULL) {
	return 0;
    }
    ifv = (struct ifvlan *)ifp->if_private;
    if (ifv == NULL) {
	return 0;
    }
    ifp->if_private = NULL;
    dlil_if_release(ifp);
    FREE(ifv, M_VLAN);
    return 0;
}

/*
 * Function: vlan_if_filter_detach
 * Purpose:
 *   Destroy all vlan interfaces that refer to the interface
 */
static int
vlan_if_filter_detach(caddr_t cookie)
{
    struct ifnet * 	ifp;
    struct ifvlan *	ifv;
    struct ifnet * 	p = (struct ifnet *)cookie;

    VLAN_LOCK();
    while (TRUE) {
	ifv = vlan_lookup_ifp(p);
	if (ifv == NULL) {
	    break;
	}
	if (ifv->ifv_detaching) {
	    continue;
	}
	/* make sure we don't invoke vlan_detach_filter */
	ifv->ifv_filter_valid = FALSE;
	vlan_remove(ifv);
	ifp = ifv->ifv_ifp;
	VLAN_UNLOCK();
	vlan_if_detach(ifp);
	VLAN_LOCK();
    }
    VLAN_UNLOCK();
    return (0);
}

/*
 * Function: vlan_attach_filter
 * Purpose:
 *   We attach an interface filter to detect when the underlying interface
 *   goes away.  We are forced to do that because dlil does not call our
 *   protocol's dl_event function for KEV_DL_IF_DETACHING.
 */

static int
vlan_attach_filter(struct ifnet * ifp, u_long * filter_id)
{
    int				error;
    struct dlil_if_flt_str	filt;

    bzero(&filt, sizeof(filt));
    filt.filter_detach = vlan_if_filter_detach;
    filt.cookie = (caddr_t)ifp;
    error = dlil_attach_interface_filter(ifp, &filt, filter_id, 
					 DLIL_LAST_FILTER);
    if (error) {
	printf("vlan: dlil_attach_interface_filter(%s%d) failed, %d\n",
	       ifp->if_name, ifp->if_unit, error);
    }
    return (error);
}

/*
 * Function: vlan_detach_filter
 * Purpose:
 *   Remove our interface filter.
 */
static int
vlan_detach_filter(u_long filter_id)
{
    int 	error;

    error = dlil_detach_filter(filter_id);
    if (error) {
	printf("vlan: dlil_detach_filter failed, %d\n", error);
    }
    return (error);
}

/*
 * Function: vlan_proto_input
 * Purpose:
 *   This function is never called.  We aren't allowed to leave the
 *   function pointer NULL, so this function simply free's the mbuf.
 */
static int
vlan_proto_input(m, frame_header, ifp, dl_tag, sync_ok)
    struct mbuf  *m;
    char         *frame_header;
    struct ifnet *ifp;
    u_long	     dl_tag;
    int          sync_ok;
{
    m_freem(m);
    return (EJUSTRETURN);
}

static struct ifnet *
find_if_name_unit(const char * if_name, int unit)
{
    struct ifnet * ifp;
    
    TAILQ_FOREACH(ifp, &ifnet, if_link) {
	if (strcmp(if_name, ifp->if_name) == 0 && unit == ifp->if_unit) {
	    return (ifp);
	}
    }
    return (ifp);
}

static void
interface_link_event(struct ifnet * ifp, u_long event_code)
{
    struct {
	struct kern_event_msg	header;
	u_long			unit;
	char			if_name[IFNAMSIZ];
    } event;

    event.header.total_size    = sizeof(event);
    event.header.vendor_code   = KEV_VENDOR_APPLE;
    event.header.kev_class     = KEV_NETWORK_CLASS;
    event.header.kev_subclass  = KEV_DL_SUBCLASS;
    event.header.event_code    = event_code;
    event.header.event_data[0] = ifp->if_family;
    event.unit                 = (u_long) ifp->if_unit;
    strncpy(event.if_name, ifp->if_name, IFNAMSIZ);
    dlil_event(ifp, &event.header);
    return;
}

static void
parent_link_event(struct ifnet * p, u_long event_code)
{
    struct ifvlan * ifv;

    LIST_FOREACH(ifv, &ifv_list, ifv_list) {
	if (p == ifv->ifv_p) {
	    interface_link_event(ifv->ifv_ifp, event_code);
	}
    }
    return;

}

/* 
 * Function: vlan_dl_event
 * Purpose:
 *   Process DLIL events that interest us.  Currently, that is
 *   just the interface UP and DOWN.  Ideally, this would also
 *   include the KEV_DL_IF_DETACH{ING} messages, which would eliminate
 *   the need for an interface filter.
 */
static int
vlan_dl_event(struct kern_event_msg * event, u_long dl_tag)
{
    struct ifnet *		p;
    struct net_event_data *	net_event;

    if (event->vendor_code != KEV_VENDOR_APPLE
	|| event->kev_class != KEV_NETWORK_CLASS
	|| event->kev_subclass != KEV_DL_SUBCLASS) {
	goto done;
    }
    net_event = (struct net_event_data *)(event->event_data);
    switch (event->event_code) {
    case KEV_DL_LINK_OFF:
    case KEV_DL_LINK_ON:
	p = find_if_name_unit(net_event->if_name, net_event->if_unit);
	if (p != NULL) {
	    parent_link_event(p, event->event_code);
	}
	break;
#if 0
    case KEV_DL_IF_DETACHING:
    case KEV_DL_IF_DETACHED:
	/* we don't get these, unfortunately */
	break;
#endif 0
    default:
	break;
    }

 done:
    return (0);
}

/*
 * Function: vlan_attach_protocol
 * Purpose:
 *   Attach a DLIL protocol to the interface, using the ETHERTYPE_VLAN
 *   demux ether type.  We're not a real protocol, we'll never receive
 *   any packets because they're intercepted by ether_demux before 
 *   our input routine would be called.
 *
 *   The reasons for attaching a protocol to the interface are:
 *   1) add a protocol reference to the interface so that the underlying
 *      interface automatically gets marked up while we're attached
 *   2) receive link status events which we can propagate to our
 *      VLAN interfaces.
 */
static int
vlan_attach_protocol(struct ifnet *ifp)
{
    struct dlil_demux_desc      desc;
    u_long			dl_tag;
    u_short 			en_native = ETHERTYPE_VLAN;
    int  			error;
    int 			i;
    struct dlil_proto_reg_str   reg;

    TAILQ_INIT(&reg.demux_desc_head);
    desc.type = DLIL_DESC_RAW;
    desc.variants.bitmask.proto_id_length = 0;
    desc.variants.bitmask.proto_id = 0;
    desc.variants.bitmask.proto_id_mask = 0;
    desc.native_type = (char *) &en_native;
    TAILQ_INSERT_TAIL(&reg.demux_desc_head, &desc, next);
    reg.interface_family = ifp->if_family;
    reg.unit_number      = ifp->if_unit;
    reg.input            = vlan_proto_input;
    reg.pre_output       = 0;
    reg.event            = vlan_dl_event;
    reg.offer            = 0;
    reg.ioctl            = 0;
    reg.default_proto    = 0;
    reg.protocol_family  = VLAN_PROTO_FAMILY;

    error = dlil_attach_protocol(&reg, &dl_tag);
    if (error) {
	printf("vlan_proto_attach(%s%d) dlil_attach_protocol failed, %d\n",
	       ifp->if_name, ifp->if_unit, error);
    }
    return (error);
}

/*
 * Function: vlan_detach_protocol
 * Purpose:
 *   Detach our DLIL protocol from an interface
 */
static int
vlan_detach_protocol(struct ifnet *ifp)
{
    u_long	dl_tag;
    int         error;

    error = dlil_find_dltag(ifp->if_family, ifp->if_unit, 
			    VLAN_PROTO_FAMILY, &dl_tag);
    if (error) {
	printf("vlan_proto_detach(%s%d) dlil_find_dltag failed, %d\n",
	       ifp->if_name, ifp->if_unit, error);
    } else {
        error = dlil_detach_protocol(dl_tag);
	if (error) {
	    printf("vlan_proto_detach(%s%d) dlil_detach_protocol failed, %d\n",
		   ifp->if_name, ifp->if_unit, error);
	}
    }
    return (error);
}

/*
 * DLIL interface family functions
 *   We use the ethernet dlil functions, since that's all we support.
 *   If we wanted to handle multiple LAN types (tokenring, etc.), we'd
 *   call the appropriate routines for that LAN type instead of hard-coding
 *   ethernet.
 */
extern int ether_add_if(struct ifnet *ifp);
extern int ether_del_if(struct ifnet *ifp);
extern int ether_init_if(struct ifnet *ifp);
extern int ether_add_proto(struct ddesc_head_str *desc_head, 
			   struct if_proto *proto, u_long dl_tag);
extern int ether_del_proto(struct if_proto *proto, u_long dl_tag);
extern int ether_ifmod_ioctl(struct ifnet *ifp, u_long command,
			     caddr_t data);
extern int ether_del_proto(struct if_proto *proto, u_long dl_tag);
extern int ether_add_proto(struct ddesc_head_str *desc_head, struct if_proto *proto, u_long dl_tag);

extern int ether_attach_inet(struct ifnet *ifp, u_long *dl_tag);
extern int ether_detach_inet(struct ifnet *ifp, u_long dl_tag);
extern int ether_attach_inet6(struct ifnet *ifp, u_long *dl_tag);
extern int ether_detach_inet6(struct ifnet *ifp, u_long dl_tag);

static int
vlan_attach_inet(struct ifnet *ifp, u_long *dl_tag)
{
    return (ether_attach_inet(ifp, dl_tag));
}

static int
vlan_detach_inet(struct ifnet *ifp, u_long dl_tag)
{
    return (ether_detach_inet(ifp, dl_tag));
}

static int
vlan_attach_inet6(struct ifnet *ifp, u_long *dl_tag)
{
    return (ether_attach_inet6(ifp, dl_tag));
}

static int
vlan_detach_inet6(struct ifnet *ifp, u_long dl_tag)
{
    return (ether_detach_inet6(ifp, dl_tag));
}

static int
vlan_add_if(struct ifnet *ifp)
{
    return (ether_add_if(ifp));
}

static int
vlan_del_if(struct ifnet *ifp)
{
    return (ether_del_if(ifp));
}

static int
vlan_init_if(struct ifnet *ifp)
{
    return (0);
}

static int
vlan_shutdown()
{
    return 0;
}

__private_extern__ int
vlan_family_init()
{
    int  i, error=0;
    struct dlil_ifmod_reg_str  ifmod_reg;
    struct dlil_protomod_reg_str vlan_protoreg;

#if 0
    /* VLAN family is built-in, called from ether_family_init */
    thread_funnel_switch(KERNEL_FUNNEL, NETWORK_FUNNEL);
#endif 0

    bzero(&ifmod_reg, sizeof(ifmod_reg));
    ifmod_reg.add_if = vlan_add_if;
    ifmod_reg.del_if = vlan_del_if;
    ifmod_reg.init_if = vlan_init_if;
    ifmod_reg.add_proto = ether_add_proto;
    ifmod_reg.del_proto = ether_del_proto;
    ifmod_reg.ifmod_ioctl = ether_ifmod_ioctl;
    ifmod_reg.shutdown    = vlan_shutdown;

    if (dlil_reg_if_modules(APPLE_IF_FAM_VLAN, &ifmod_reg)) {
	printf("WARNING: vlan_family_init -- "
	       "Can't register if family modules\n");
	error = EIO;
	goto done;
    }

    /* Register protocol registration functions */
    bzero(&vlan_protoreg, sizeof(vlan_protoreg));
    vlan_protoreg.attach_proto = vlan_attach_inet;
    vlan_protoreg.detach_proto = vlan_detach_inet;
	
    if (error = dlil_reg_proto_module(PF_INET, APPLE_IF_FAM_VLAN, 
				      &vlan_protoreg) != 0) {
	kprintf("dlil_reg_proto_module failed for AF_INET6 error=%d\n",
		error);
	goto done;
    }
    vlan_protoreg.attach_proto = vlan_attach_inet6;
    vlan_protoreg.detach_proto = vlan_detach_inet6;
	
    if (error = dlil_reg_proto_module(PF_INET6, APPLE_IF_FAM_VLAN, 
				      &vlan_protoreg) != 0) {
	kprintf("dlil_reg_proto_module failed for AF_INET6 error=%d\n",
		error);
	goto done;
    }
    vlan_clone_attach();

 done:
#if 0
    thread_funnel_switch(NETWORK_FUNNEL, KERNEL_FUNNEL);
#endif 0
>>>>>>> origin/10.3
    return (error);
}
