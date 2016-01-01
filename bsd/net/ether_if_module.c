/*
<<<<<<< HEAD
 * Copyright (c) 2000-2013 Apple Inc. All rights reserved.
=======
 * Copyright (c) 2000-2009 Apple Inc. All rights reserved.
>>>>>>> origin/10.6
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
 * Copyright (c) 1982, 1989, 1993
 *	The Regents of the University of California.  All rights reserved.
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
 */

#include <sys/param.h>
#include <sys/systm.h>
#include <sys/kernel.h>
#include <sys/malloc.h>
#include <sys/mbuf.h>
#include <sys/socket.h>
#include <sys/sockio.h>
#include <sys/sysctl.h>

#include <pexpert/pexpert.h>

#define etherbroadcastaddr	fugly
#include <net/if.h>
#include <net/route.h>
#include <net/if_llc.h>
#include <net/if_dl.h>
#include <net/if_types.h>
<<<<<<< HEAD
#include <net/if_ether.h>
#include <net/if_gif.h>
=======
>>>>>>> origin/10.1
#include <netinet/if_ether.h>
#include <netinet/in.h>	/* For M_LOOP */
#include <net/kpi_interface.h>
#include <net/kpi_protocol.h>
#undef etherbroadcastaddr

/*
#if INET
#include <netinet/in.h>
#include <netinet/in_var.h>

#include <netinet/in_systm.h>
#include <netinet/ip.h>
#endif
*/
#include <net/ether_if_module.h>
#include <sys/socketvar.h>
#include <net/if_vlan_var.h>
<<<<<<< HEAD
#if BOND
#include <net/if_bond_internal.h>
#endif /* BOND */
#if IF_BRIDGE
#include <net/if_bridgevar.h>
#endif /* IF_BRIDGE */

#include <net/dlil.h>

<<<<<<< HEAD
#define memcpy(x,y,z)	bcopy(y, x, z)
=======

#include <net/dlil.h>

extern int  vlan_demux(struct ifnet * ifp, struct mbuf *, 
		       char * frame_header, struct if_proto * * proto);
>>>>>>> origin/10.3

SYSCTL_DECL(_net_link);
SYSCTL_NODE(_net_link, IFT_ETHER, ether, CTLFLAG_RW|CTLFLAG_LOCKED, 0,
    "Ethernet");

<<<<<<< HEAD
struct en_desc {
	u_int16_t type;			/* Type of protocol stored in data */
	u_int32_t protocol_family;	/* Protocol family */
	u_int32_t data[2];		/* Protocol data */
};
=======
#include <netat/at_pat.h>
#if NETAT
extern struct ifqueue atalkintrq;
#endif


<<<<<<< HEAD
#if BRIDGE
#include <net/bridge.h>
#endif

static u_long lo_dlt = 0;
=======
#define memcpy(x,y,z)	bcopy(y, x, z)

>>>>>>> origin/10.6

#define IFP2AC(IFP) ((struct arpcom *)IFP)
>>>>>>> origin/10.3

/* descriptors are allocated in blocks of ETHER_DESC_BLK_SIZE */
#define ETHER_DESC_BLK_SIZE (10)
=======
struct en_desc {
    u_int16_t		type;		/* Type of protocol stored in data */
    struct if_proto *proto;		/* Protocol structure */
    u_long			data[2];	/* Protocol data */
};

#define ETHER_DESC_BLK_SIZE (10)
#define MAX_INTERFACES 50
>>>>>>> origin/10.1

/*
 * Header for the demux list, hangs off of IFP at if_family_cookie
 */
struct ether_desc_blk_str {
<<<<<<< HEAD
	u_int32_t  n_max_used;
	u_int32_t	n_count;
	u_int32_t	n_used;
	struct en_desc  block_ptr[1];
};

<<<<<<< HEAD
/* Size of the above struct before the array of struct en_desc */
#define ETHER_DESC_HEADER_SIZE	\
	((size_t) offsetof(struct ether_desc_blk_str, block_ptr))

__private_extern__ u_char etherbroadcastaddr[ETHER_ADDR_LEN] =
    { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff };
=======
static __inline__ int
_ether_cmp(const void * a, const void * b)
{
	const u_int16_t * a_s = (const u_int16_t *)a;
	const u_int16_t * b_s = (const u_int16_t *)b;
	
	if (a_s[0] != b_s[0]
	    || a_s[1] != b_s[1]
	    || a_s[2] != b_s[2]) {
		return (1);
	}
	return (0);
}
>>>>>>> origin/10.5

/*
 * Release all descriptor entries owned by this protocol (there may be several).
 * Setting the type to 0 releases the entry. Eventually we should compact-out
 * the unused entries.
 */
int
ether_del_proto(ifnet_t ifp, protocol_family_t protocol_family)
{
	struct ether_desc_blk_str *desc_blk =
	    (struct ether_desc_blk_str *)ifp->if_family_cookie;
	u_int32_t current = 0;
	int found = 0;

	if (desc_blk == NULL)
		return (0);

	for (current = desc_blk->n_max_used; current > 0; current--) {
		if (desc_blk->block_ptr[current - 1].protocol_family ==
		    protocol_family) {
			found = 1;
			desc_blk->block_ptr[current - 1].type = 0;
			desc_blk->n_used--;
		}
	}

	if (desc_blk->n_used == 0) {
		FREE(ifp->if_family_cookie, M_IFADDR);
		ifp->if_family_cookie = 0;
	} else {
		/* Decrement n_max_used */
		for (; desc_blk->n_max_used > 0 &&
		    desc_blk->block_ptr[desc_blk->n_max_used - 1].type == 0;
		    desc_blk->n_max_used--)
			;
	}

	return (0);
}

static int
ether_add_proto_internal(struct ifnet *ifp, protocol_family_t protocol,
    const struct ifnet_demux_desc *demux)
{
	struct en_desc *ed;
	struct ether_desc_blk_str *desc_blk =
	    (struct ether_desc_blk_str *)ifp->if_family_cookie;
	u_int32_t i;

	switch (demux->type) {
	/* These types are supported */
	/* Top three are preferred */
	case DLIL_DESC_ETYPE2:
		if (demux->datalen != 2)
			return (EINVAL);
		break;

	case DLIL_DESC_SAP:
		if (demux->datalen != 3)
			return (EINVAL);
		break;

	case DLIL_DESC_SNAP:
		if (demux->datalen != 5)
			return (EINVAL);
		break;

	default:
		return (ENOTSUP);
	}

	/* Verify a matching descriptor does not exist */
	if (desc_blk != NULL) {
		switch (demux->type) {
		case DLIL_DESC_ETYPE2:
			for (i = 0; i < desc_blk->n_max_used; i++) {
				if (desc_blk->block_ptr[i].type ==
				    DLIL_DESC_ETYPE2 &&
				    desc_blk->block_ptr[i].data[0] ==
				    *(u_int16_t*)demux->data) {
					return (EADDRINUSE);
				}
			}
			break;
		case DLIL_DESC_SAP:
		case DLIL_DESC_SNAP:
			for (i = 0; i < desc_blk->n_max_used; i++) {
				if (desc_blk->block_ptr[i].type ==
				    demux->type &&
				    bcmp(desc_blk->block_ptr[i].data,
				    demux->data, demux->datalen) == 0) {
					return (EADDRINUSE);
				}
			}
			break;
		}
	}

	/* Check for case where all of the descriptor blocks are in use */
	if (desc_blk == NULL || desc_blk->n_used == desc_blk->n_count) {
		struct ether_desc_blk_str *tmp;
		u_int32_t new_count = ETHER_DESC_BLK_SIZE;
		u_int32_t new_size;
		u_int32_t old_size = 0;
=======
    u_long  n_max_used;
    u_long	n_count;
    struct en_desc  *block_ptr;
};


static struct ether_desc_blk_str ether_desc_blk[MAX_INTERFACES];


/*
 * Release all descriptor entries owned by this dl_tag (there may be several).
 * Setting the type to 0 releases the entry. Eventually we should compact-out
 * the unused entries.
 */
__private_extern__ int
ether_del_proto(struct if_proto *proto, u_long dl_tag)
{
    struct en_desc*	ed = ether_desc_blk[proto->ifp->family_cookie].block_ptr;
    u_long	current = 0;
    int found = 0;
    
    for (current = ether_desc_blk[proto->ifp->family_cookie].n_max_used;
            current > 0; current--) {
        if (ed[current - 1].proto == proto) {
            found = 1;
            ed[current - 1].type = 0;
            
            if (current == ether_desc_blk[proto->ifp->family_cookie].n_max_used) {
                ether_desc_blk[proto->ifp->family_cookie].n_max_used--;
            }
        }
    }
    
    return found;
 }
>>>>>>> origin/10.1

		i = 0;

		if (desc_blk) {
			new_count += desc_blk->n_count;
			old_size = desc_blk->n_count * sizeof (struct en_desc) +
			    ETHER_DESC_HEADER_SIZE;
			i = desc_blk->n_used;
		}

<<<<<<< HEAD
<<<<<<< HEAD
		new_size = new_count * sizeof (struct en_desc) +
		    ETHER_DESC_HEADER_SIZE;
=======
static int
=======

__private_extern__ int
>>>>>>> origin/10.3
ether_add_proto(struct ddesc_head_str *desc_head, struct if_proto *proto, u_long dl_tag)
{
   char *current_ptr;
   struct dlil_demux_desc  *desc;
   struct en_desc	   *ed;
   struct en_desc *last;
   u_long		   *bitmask;
   u_long		   *proto_id;
   u_long		   i;
   short		   total_length;
   u_long		   block_count;
   u_long                  *tmp;


    TAILQ_FOREACH(desc, desc_head, next) {
        switch (desc->type) {
            /* These types are supported */
            /* Top three are preferred */
            case DLIL_DESC_ETYPE2:
                if (desc->variants.native_type_length != 2)
                    return EINVAL;
                break;
                
            case DLIL_DESC_SAP:
                if (desc->variants.native_type_length != 3)
                    return EINVAL;
                break;
                
            case DLIL_DESC_SNAP:
                if (desc->variants.native_type_length != 5)
                    return EINVAL;
                break;
                
            case DLIL_DESC_802_2:
            case DLIL_DESC_802_2_SNAP:
                break;
            
            case DLIL_DESC_RAW:
                if (desc->variants.bitmask.proto_id_length == 0)
                    break;
                /* else fall through, bitmask variant not supported */
            
            default:
                ether_del_proto(proto, dl_tag);
                return EINVAL;
        }
    
        ed = ether_desc_blk[proto->ifp->family_cookie].block_ptr;
        
        /* Find a free entry */
        for (i = 0; i < ether_desc_blk[proto->ifp->family_cookie].n_count; i++) {
            if (ed[i].type == 0) {
                break;
            }
        }
        
        if (i >= ether_desc_blk[proto->ifp->family_cookie].n_count) {
            u_long	new_count = ETHER_DESC_BLK_SIZE +
                        ether_desc_blk[proto->ifp->family_cookie].n_count;
            tmp = _MALLOC((new_count * (sizeof(*ed))), M_IFADDR, M_WAITOK);
            if (tmp  == 0) {
                /*
                * Remove any previous descriptors set in the call.
                */
                ether_del_proto(proto, dl_tag);
                return ENOMEM;
            }
            
            bzero(tmp, new_count * sizeof(*ed));
            bcopy(ether_desc_blk[proto->ifp->family_cookie].block_ptr, 
                tmp, ether_desc_blk[proto->ifp->family_cookie].n_count * sizeof(*ed));
            FREE(ether_desc_blk[proto->ifp->family_cookie].block_ptr, M_IFADDR);
            ether_desc_blk[proto->ifp->family_cookie].n_count = new_count;
            ether_desc_blk[proto->ifp->family_cookie].block_ptr = (struct en_desc*)tmp;
	    ed = ether_desc_blk[proto->ifp->family_cookie].block_ptr;
        }
        
        /* Bump n_max_used if appropriate */
        if (i + 1 > ether_desc_blk[proto->ifp->family_cookie].n_max_used) {
            ether_desc_blk[proto->ifp->family_cookie].n_max_used = i + 1;
        }
        
        ed[i].proto	= proto;
        ed[i].data[0]	= 0;
        ed[i].data[1] = 0;
        
        switch (desc->type) {
            case DLIL_DESC_RAW:
                /* 2 byte ethernet raw protocol type is at native_type */
                /* protocol is not in network byte order */
                ed[i].type = DLIL_DESC_ETYPE2;
                ed[i].data[0] = htons(*(u_int16_t*)desc->native_type);
                break;
                
            case DLIL_DESC_ETYPE2:
                /* 2 byte ethernet raw protocol type is at native_type */
                /* prtocol must be in network byte order */
                ed[i].type = DLIL_DESC_ETYPE2;
                ed[i].data[0] = *(u_int16_t*)desc->native_type;
                break;
            
            case DLIL_DESC_802_2:
                ed[i].type = DLIL_DESC_SAP;
                ed[i].data[0] = *(u_int32_t*)&desc->variants.desc_802_2;
                ed[i].data[0] &= htonl(0xFFFFFF00);
                break;
            
            case DLIL_DESC_SAP:
                ed[i].type = DLIL_DESC_SAP;
                bcopy(desc->native_type, &ed[i].data[0], 3);
                break;
    
            case DLIL_DESC_802_2_SNAP:
                ed[i].type = DLIL_DESC_SNAP;
                desc->variants.desc_802_2_SNAP.protocol_type =
                    htons(desc->variants.desc_802_2_SNAP.protocol_type);
                bcopy(&desc->variants.desc_802_2_SNAP, &ed[i].data[0], 8);
                ed[i].data[0] &= htonl(0x000000FF);
                desc->variants.desc_802_2_SNAP.protocol_type =
                    ntohs(desc->variants.desc_802_2_SNAP.protocol_type);
                break;
            
            case DLIL_DESC_SNAP: {
                u_int8_t*	pDest = ((u_int8_t*)&ed[i].data[0]) + 3;
                ed[i].type = DLIL_DESC_SNAP;
                bcopy(&desc->native_type, pDest, 5);
            }
            break;
        }
    }
    
    return 0;
} 


static
int  ether_shutdown()
{
    return 0;
}
>>>>>>> origin/10.1

		tmp = _MALLOC(new_size, M_IFADDR, M_WAITOK);
		if (tmp  == NULL) {
			/*
			 * Remove any previous descriptors set in the call.
			 */
			return (ENOMEM);
		}

<<<<<<< HEAD
		bzero(((char *)tmp) + old_size, new_size - old_size);
		if (desc_blk) {
			bcopy(desc_blk, tmp, old_size);
			FREE(desc_blk, M_IFADDR);
		}
		desc_blk = tmp;
		ifp->if_family_cookie = (uintptr_t)desc_blk;
		desc_blk->n_count = new_count;
	} else {
		/* Find a free entry */
		for (i = 0; i < desc_blk->n_count; i++) {
			if (desc_blk->block_ptr[i].type == 0) {
				break;
			}
		}
	}

	/* Bump n_max_used if appropriate */
	if (i + 1 > desc_blk->n_max_used) {
		desc_blk->n_max_used = i + 1;
	}

	ed = &desc_blk->block_ptr[i];
	ed->protocol_family = protocol;
	ed->data[0] = 0;
	ed->data[1] = 0;

	switch (demux->type) {
	case DLIL_DESC_ETYPE2:
		/* 2 byte ethernet raw protocol type is at native_type */
		/* prtocol must be in network byte order */
		ed->type = DLIL_DESC_ETYPE2;
		ed->data[0] = *(u_int16_t*)demux->data;
		break;

	case DLIL_DESC_SAP:
		ed->type = DLIL_DESC_SAP;
		bcopy(demux->data, &ed->data[0], 3);
		break;

	case DLIL_DESC_SNAP: {
		u_int8_t*	pDest = ((u_int8_t*)&ed->data[0]) + 3;
		ed->type = DLIL_DESC_SNAP;
		bcopy(demux->data, pDest, 5);
		break;
	}
	}

	desc_blk->n_used++;
=======
int ether_demux(ifp, m, frame_header, proto)
    struct ifnet *ifp;
    struct mbuf  *m;
    char         *frame_header;
    struct if_proto **proto;
>>>>>>> origin/10.1

	return (0);
}

int
ether_add_proto(ifnet_t	 ifp, protocol_family_t	protocol,
    const struct ifnet_demux_desc *demux_list, u_int32_t demux_count)
{
<<<<<<< HEAD
	int error = 0;
	u_int32_t i;

	for (i = 0; i < demux_count; i++) {
		error = ether_add_proto_internal(ifp, protocol, &demux_list[i]);
		if (error) {
			ether_del_proto(ifp, protocol);
			break;
		}
	}

	return (error);
}

int
ether_demux(ifnet_t ifp, mbuf_t m, char *frame_header,
    protocol_family_t *protocol_family)
{
	struct ether_header *eh = (struct ether_header *)(void *)frame_header;
	u_short	 ether_type = eh->ether_type;
	u_int16_t type;
	u_int8_t *data;
	u_int32_t i = 0;
	struct ether_desc_blk_str *desc_blk =
	    (struct ether_desc_blk_str *)ifp->if_family_cookie;
	u_int32_t maxd = desc_blk ? desc_blk->n_max_used : 0;
	struct en_desc	*ed = desc_blk ? desc_blk->block_ptr : NULL;
	u_int32_t extProto1 = 0;
	u_int32_t extProto2 = 0;

	if (eh->ether_dhost[0] & 1) {
		/* Check for broadcast */
		if (_ether_cmp(etherbroadcastaddr, eh->ether_dhost) == 0)
			m->m_flags |= M_BCAST;
		else
			m->m_flags |= M_MCAST;
	}

	if (m->m_flags & M_HASFCS) {
                /*
                 * If the M_HASFCS is set by the driver we want to make sure
                 * that we strip off the trailing FCS data before handing it
                 * up the stack.
                 */
                m_adj(m, -ETHER_CRC_LEN);
	        m->m_flags &= ~M_HASFCS;
        }

	if (ifp->if_eflags & IFEF_BOND) {
		/* if we're bonded, bond "protocol" gets all the packets */
		*protocol_family = PF_BOND;
		return (0);
	}

	if ((eh->ether_dhost[0] & 1) == 0) {
		/*
		 * When the driver is put into promiscuous mode we may receive
		 * unicast frames that are not intended for our interfaces.
		 * They are marked here as being promiscuous so the caller may
		 * dispose of them after passing the packets to any interface
		 * filters.
		 */
		if (_ether_cmp(eh->ether_dhost, IF_LLADDR(ifp))) {
			m->m_flags |= M_PROMISC;
		}
	}
<<<<<<< HEAD

=======
	
>>>>>>> origin/10.5
	/* check for VLAN */
	if ((m->m_pkthdr.csum_flags & CSUM_VLAN_TAG_VALID) != 0) {
		if (EVL_VLANOFTAG(m->m_pkthdr.vlan_tag) != 0) {
			*protocol_family = PF_VLAN;
			return (0);
		}
		/* the packet is just priority-tagged, clear the bit */
		m->m_pkthdr.csum_flags &= ~CSUM_VLAN_TAG_VALID;
<<<<<<< HEAD
	} else if (ether_type == htons(ETHERTYPE_VLAN)) {
		struct ether_vlan_header *	evl;

		evl = (struct ether_vlan_header *)(void *)frame_header;
		if (m->m_len < ETHER_VLAN_ENCAP_LEN ||
		    ntohs(evl->evl_proto) == ETHERTYPE_VLAN ||
		    EVL_VLANOFTAG(ntohs(evl->evl_tag)) != 0) {
			*protocol_family = PF_VLAN;
			return (0);
=======
	}
	else if (ether_type == htons(ETHERTYPE_VLAN)) {
		struct ether_vlan_header *	evl;

		evl = (struct ether_vlan_header *)frame_header;
		if (m->m_len < ETHER_VLAN_ENCAP_LEN
		    || ntohs(evl->evl_proto) == ETHERTYPE_VLAN
		    || EVL_VLANOFTAG(ntohs(evl->evl_tag)) != 0) {
			*protocol_family = PF_VLAN;
			return 0;
>>>>>>> origin/10.5
		}
		/* the packet is just priority-tagged */

		/* make the encapsulated ethertype the actual ethertype */
		ether_type = evl->evl_encap_proto = evl->evl_proto;

		/* remove the encapsulation header */
		m->m_len -= ETHER_VLAN_ENCAP_LEN;
		m->m_data += ETHER_VLAN_ENCAP_LEN;
		m->m_pkthdr.len -= ETHER_VLAN_ENCAP_LEN;
		m->m_pkthdr.csum_flags = 0; /* can't trust hardware checksum */
<<<<<<< HEAD
	} else if (ether_type == htons(ETHERTYPE_ARP))
		m->m_pkthdr.pkt_flags |= PKTF_INET_RESOLVE; /* ARP packet */

=======
	}
	
>>>>>>> origin/10.5
	data = mtod(m, u_int8_t*);
=======
    register struct ether_header *eh = (struct ether_header *)frame_header;
    u_short			ether_type = eh->ether_type;
    u_short			ether_type_host;
    u_int16_t		type;
    u_int8_t		*data;
    u_long			i = 0;
    u_long			max = ether_desc_blk[ifp->family_cookie].n_max_used;
    struct en_desc	*ed = ether_desc_blk[ifp->family_cookie].block_ptr;
    u_int32_t		extProto1 = 0;
    u_int32_t		extProto2 = 0;
    
    if (eh->ether_dhost[0] & 1) {
        /* Check for broadcast */
        if (*(u_int32_t*)eh->ether_dhost == 0xFFFFFFFF &&
            *(u_int16_t*)(eh->ether_dhost + sizeof(u_int32_t)) == 0xFFFF)
            m->m_flags |= M_BCAST;
        else
            m->m_flags |= M_MCAST;
    } else {
        /*
         * When the driver is put into promiscuous mode we may receive unicast
         * frames that are not intended for our interfaces.  They are filtered
         * here to keep them from traveling further up the stack to code that
         * is not expecting them or prepared to deal with them.  In the near
         * future, the filtering done here will be moved even further down the
         * stack into the IONetworkingFamily, preventing even interface
         * filter NKE's from receiving promiscuous packets.  Please use BPF.
         */
        #define ETHER_CMP(x, y) ( ((u_int16_t *) x)[0] != ((u_int16_t *) y)[0] || \
                                  ((u_int16_t *) x)[1] != ((u_int16_t *) y)[1] || \
                                  ((u_int16_t *) x)[2] != ((u_int16_t *) y)[2] )
    
        if (ETHER_CMP(eh->ether_dhost, ((struct arpcom *) ifp)->ac_enaddr)) {
            m_freem(m);
            return EJUSTRETURN;
        }
    }
    ether_type_host = ntohs(ether_type);
    if ((m->m_pkthdr.csum_flags & CSUM_VLAN_TAG_VALID)
	|| ether_type_host == ETHERTYPE_VLAN) {
	return (vlan_demux(ifp, m, frame_header, proto));
    }
    data = mtod(m, u_int8_t*);

    /*
     * Determine the packet's protocol type and stuff the protocol into
     * longs for quick compares.
     */
<<<<<<< HEAD
    
    if (ntohs(ether_type) < 1500) {
=======
    if (ether_type_host <= 1500) {
>>>>>>> origin/10.3
        extProto1 = *(u_int32_t*)data;
        
        // SAP or SNAP
        if ((extProto1 & htonl(0xFFFFFF00)) == htonl(0xAAAA0300)) {
            // SNAP
            type = DLIL_DESC_SNAP;
            extProto2 = *(u_int32_t*)(data + sizeof(u_int32_t));
            extProto1 &= htonl(0x000000FF);
        } else {
            type = DLIL_DESC_SAP;
            extProto1 &= htonl(0xFFFFFF00);
        }
    } else {
        type = DLIL_DESC_ETYPE2;
    }
    
    /* 
     * Search through the connected protocols for a match. 
     */
    
    switch (type) {
        case DLIL_DESC_ETYPE2:
            for (i = 0; i < max; i++) {
                if ((ed[i].type == type) && (ed[i].data[0] == ether_type)) {
                    *proto = ed[i].proto;
                    return 0;
                }
            }
            break;
        
        case DLIL_DESC_SAP:
            for (i = 0; i < max; i++) {
                if ((ed[i].type == type) && (ed[i].data[0] == extProto1)) {
                    *proto = ed[i].proto;
                    return 0;
                }
            }
            break;
        
        case DLIL_DESC_SNAP:
            for (i = 0; i < max; i++) {
                if ((ed[i].type == type) && (ed[i].data[0] == extProto1) &&
                    (ed[i].data[1] == extProto2)) {
                    *proto = ed[i].proto;
                    return 0;
                }
            }
            break;
    }
    
    return ENOENT;
<<<<<<< HEAD
}			
>>>>>>> origin/10.1
=======
}
>>>>>>> origin/10.3

	/*
	* Determine the packet's protocol type and stuff the protocol into
	* longs for quick compares.
	*/
	if (ntohs(ether_type) <= 1500) {
		bcopy(data, &extProto1, sizeof (u_int32_t));

		/* SAP or SNAP */
		if ((extProto1 & htonl(0xFFFFFF00)) == htonl(0xAAAA0300)) {
			/* SNAP */
			type = DLIL_DESC_SNAP;
			bcopy(data + sizeof (u_int32_t), &extProto2,
			    sizeof (u_int32_t));
			extProto1 &= htonl(0x000000FF);
		} else {
			type = DLIL_DESC_SAP;
			extProto1 &= htonl(0xFFFFFF00);
		}
	} else {
		type = DLIL_DESC_ETYPE2;
	}

	/*
	* Search through the connected protocols for a match.
	*/
	switch (type) {
	case DLIL_DESC_ETYPE2:
		for (i = 0; i < maxd; i++) {
			if ((ed[i].type == type) &&
			    (ed[i].data[0] == ether_type)) {
				*protocol_family = ed[i].protocol_family;
				return (0);
			}
		}
		break;

	case DLIL_DESC_SAP:
		for (i = 0; i < maxd; i++) {
			if ((ed[i].type == type) &&
			    (ed[i].data[0] == extProto1)) {
				*protocol_family = ed[i].protocol_family;
				return (0);
			}
		}
		break;

	case DLIL_DESC_SNAP:
		for (i = 0; i < maxd; i++) {
			if ((ed[i].type == type) &&
			    (ed[i].data[0] == extProto1) &&
				(ed[i].data[1] == extProto2)) {
				*protocol_family = ed[i].protocol_family;
				return (0);
			}
		}
	break;
	}

	return (ENOENT);
}

/*
 * On embedded, ether_frameout is practicaly ether_frameout_extended.
 * On non-embedded, ether_frameout has long been exposed as a public KPI,
 * and therefore its signature must remain the same (without the pre- and
 * postpend length parameters.)
 */
#if KPI_INTERFACE_EMBEDDED
int
ether_frameout(struct ifnet *ifp, struct mbuf **m,
    const struct sockaddr *ndest, const char *edst,
    const char *ether_type, u_int32_t *prepend_len, u_int32_t *postpend_len)
#else /* !KPI_INTERFACE_EMBEDDED */
int
ether_frameout(struct ifnet *ifp, struct mbuf **m,
    const struct sockaddr *ndest, const char *edst,
    const char *ether_type)
#endif /* KPI_INTERFACE_EMBEDDED */
{
#if KPI_INTERFACE_EMBEDDED
	return (ether_frameout_extended(ifp, m, ndest, edst, ether_type,
	    prepend_len, postpend_len));
#else /* !KPI_INTERFACE_EMBEDDED */
	return (ether_frameout_extended(ifp, m, ndest, edst, ether_type,
	    NULL, NULL));
#endif /* !KPI_INTERFACE_EMBEDDED */
}

/*
 * Ethernet output routine.
 * Encapsulate a packet of type family for the local net.
 * Use trailer local net encapsulation if enough data in first
 * packet leaves a multiple of 512 bytes of data in remainder.
 */
int
ether_frameout_extended(struct ifnet *ifp, struct mbuf **m,
    const struct sockaddr *ndest, const char *edst,
    const char *ether_type, u_int32_t *prepend_len, u_int32_t *postpend_len)
{
<<<<<<< HEAD
	struct ether_header *eh;
	int hlen;	/* link layer header length */
=======
	register struct ether_header *eh;
	int hlen;	/* link layer header length */
	struct arpcom *ac = IFP2AC(ifp);

>>>>>>> origin/10.3

	hlen = ETHER_HDR_LEN;

	/*
	 * If a simplex interface, and the packet is being sent to our
	 * Ethernet address or a broadcast address, loopback a copy.
	 * XXX To make a simplex device behave exactly like a duplex
	 * device, we should copy in the case of sending to our own
	 * ethernet address (thus letting the original actually appear
	 * on the wire). However, we don't do that here for security
	 * reasons and compatibility with the original behavior.
	 */
	if ((ifp->if_flags & IFF_SIMPLEX) &&
<<<<<<< HEAD
	    ((*m)->m_flags & M_LOOP) && lo_ifp != NULL) {
		if ((*m)->m_flags & M_BCAST) {
			struct mbuf *n = m_copy(*m, 0, (int)M_COPYALL);
			if (n != NULL) {
				dlil_output(lo_ifp, ndest->sa_family,
				    n, NULL, ndest, 0, NULL);
			}
		} else if (_ether_cmp(edst, IF_LLADDR(ifp)) == 0) {
			dlil_output(lo_ifp, ndest->sa_family, *m,
			    NULL, ndest, 0, NULL);
			return (EJUSTRETURN);
		}
	}

=======
	    ((*m)->m_flags & M_LOOP)) {
	    if (lo_dlt == 0) 
            dlil_find_dltag(APPLE_IF_FAM_LOOPBACK, 0, PF_INET, &lo_dlt);

	    if (lo_dlt) {
            if ((*m)->m_flags & M_BCAST) {
                struct mbuf *n = m_copy(*m, 0, (int)M_COPYALL);
                if (n != NULL)
                    dlil_output(lo_dlt, n, 0, ndest, 0);
            } 
            else 
            {
                if (bcmp(edst,  ac->ac_enaddr, ETHER_ADDR_LEN) == 0) {
                    dlil_output(lo_dlt, *m, 0, ndest, 0);
                    return EJUSTRETURN;
                }
            }
	    }
	}
    
    
>>>>>>> origin/10.1
	/*
	 * Add local net header.  If no space in first mbuf,
	 * allocate another.
	 */
	M_PREPEND(*m, sizeof (struct ether_header), M_DONTWAIT, 0);
	if (*m == NULL)
		return (EJUSTRETURN);

	if (prepend_len != NULL)
		*prepend_len = sizeof (struct ether_header);
	if (postpend_len != NULL)
		*postpend_len = 0;

	eh = mtod(*m, struct ether_header *);
	(void) memcpy(&eh->ether_type, ether_type, sizeof(eh->ether_type));
	(void) memcpy(eh->ether_dhost, edst, ETHER_ADDR_LEN);
	ifnet_lladdr_copy_bytes(ifp, eh->ether_shost, ETHER_ADDR_LEN);

	return (0);
}

<<<<<<< HEAD
errno_t
ether_check_multi(ifnet_t ifp, const struct sockaddr *proto_addr)
=======


__private_extern__ int
ether_add_if(struct ifnet *ifp)
>>>>>>> origin/10.3
{
#pragma unused(ifp)
	errno_t	result = EAFNOSUPPORT;
	const u_char *e_addr;

<<<<<<< HEAD
<<<<<<< HEAD
	/*
	 * AF_SPEC and AF_LINK don't require translation. We do
	 * want to verify that they specify a valid multicast.
	 */
	switch(proto_addr->sa_family) {
	case AF_UNSPEC:
		e_addr = (const u_char*)&proto_addr->sa_data[0];
		if ((e_addr[0] & 0x01) != 0x01)
			result = EADDRNOTAVAIL;
		else
			result = 0;
		break;

	case AF_LINK:
		e_addr = CONST_LLADDR((const struct sockaddr_dl*)
		    (uintptr_t)(size_t)proto_addr);
		if ((e_addr[0] & 0x01) != 0x01)
			result = EADDRNOTAVAIL;
		else
			result = 0;
		break;
	}
=======
    ifp->if_framer = ether_frameout;
    ifp->if_demux  = ether_demux;
    ifp->if_event  = 0;
    ifp->if_resolvemulti = ether_resolvemulti;
    ifp->if_nvlans = 0;
>>>>>>> origin/10.3

	return (result);
=======
    for (i=0; i < MAX_INTERFACES; i++)
        if (ether_desc_blk[i].n_count == 0)
            break;

    if (i == MAX_INTERFACES)
        return ENOMEM;

    ether_desc_blk[i].block_ptr = _MALLOC(ETHER_DESC_BLK_SIZE * sizeof(struct en_desc),
                                            M_IFADDR, M_WAITOK);
    if (ether_desc_blk[i].block_ptr == 0)
        return ENOMEM;

    ether_desc_blk[i].n_count = ETHER_DESC_BLK_SIZE;
    bzero(ether_desc_blk[i].block_ptr, ETHER_DESC_BLK_SIZE * sizeof(struct en_desc));

    ifp->family_cookie = i;
    
    return 0;
}

__private_extern__ int
ether_del_if(struct ifnet *ifp)
{
    if ((ifp->family_cookie < MAX_INTERFACES) &&
        (ether_desc_blk[ifp->family_cookie].n_count))
    {
        FREE(ether_desc_blk[ifp->family_cookie].block_ptr, M_IFADDR);
        ether_desc_blk[ifp->family_cookie].block_ptr = NULL;
        ether_desc_blk[ifp->family_cookie].n_count = 0;
        ether_desc_blk[ifp->family_cookie].n_max_used = 0;
        return 0;
    }
    else
        return ENOENT;
>>>>>>> origin/10.1
}

<<<<<<< HEAD
=======
__private_extern__ int
ether_init_if(struct ifnet *ifp)
{
    register struct ifaddr *ifa;
    register struct sockaddr_dl *sdl;

    ifa = ifnet_addrs[ifp->if_index - 1];
    if (ifa == 0) {
            printf("ether_ifattach: no lladdr!\n");
            return (EINVAL);
    }
    sdl = (struct sockaddr_dl *)ifa->ifa_addr;
    sdl->sdl_type = IFT_ETHER;
    sdl->sdl_alen = ifp->if_addrlen;
    bcopy((IFP2AC(ifp))->ac_enaddr, LLADDR(sdl), ifp->if_addrlen);

    return 0;
}


>>>>>>> origin/10.3
int
ether_ioctl(ifnet_t ifp, u_int32_t command, void *data)
{
<<<<<<< HEAD
#pragma unused(ifp, command, data)
	return (EOPNOTSUPP);
=======
    struct rslvmulti_req *rsreq = (struct rslvmulti_req *) data;
    int error = 0;
    struct sockaddr_dl *sdl;
    struct sockaddr_in *sin;
    u_char *e_addr;


    switch (command) {
        case SIOCRSLVMULTI: 
        switch(rsreq->sa->sa_family) {
            case AF_UNSPEC:
                /* AppleTalk uses AF_UNSPEC for multicast registration.
                 * No mapping needed. Just check that it's a valid MC address.
                 */
                e_addr = &rsreq->sa->sa_data[0];
                if ((e_addr[0] & 1) != 1)
                    return EADDRNOTAVAIL;
                *rsreq->llsa = 0;
                return EJUSTRETURN;
            
            
            case AF_LINK:
                /* 
                 * No mapping needed. Just check that it's a valid MC address.
                 */
                sdl = (struct sockaddr_dl *)rsreq->sa;
                e_addr = LLADDR(sdl);
                if ((e_addr[0] & 1) != 1)
                    return EADDRNOTAVAIL;
                *rsreq->llsa = 0;
                return EJUSTRETURN;
                
            default:
                return EAFNOSUPPORT;
        }
        
        default:
            return EOPNOTSUPP;
    }
>>>>>>> origin/10.1
}

__private_extern__ int
ether_family_init(void)
{
<<<<<<< HEAD
	errno_t	error = 0;

	/* Register protocol registration functions */
	if ((error = proto_register_plumber(PF_INET, APPLE_IF_FAM_ETHERNET,
	    ether_attach_inet, ether_detach_inet)) != 0) {
		printf("proto_register_plumber failed for PF_INET error=%d\n",
		    error);
		goto done;
	}
#if INET6
	if ((error = proto_register_plumber(PF_INET6, APPLE_IF_FAM_ETHERNET,
	    ether_attach_inet6, ether_detach_inet6)) != 0) {
		printf("proto_register_plumber failed for PF_INET6 error=%d\n",
		    error);
		goto done;
	}
#endif /* INET6 */
#if VLAN
	vlan_family_init();
#endif /* VLAN */
#if BOND
	bond_family_init();
#endif /* BOND */
#if IF_BRIDGE
	bridgeattach(0);
#endif /* IF_BRIDGE */
done:

	return (error);
=======
    int  i;
    struct dlil_ifmod_reg_str  ifmod_reg;
<<<<<<< HEAD
=======
    struct dlil_protomod_reg_str enet_protoreg;
    extern int vlan_family_init(void);
>>>>>>> origin/10.3

    if (ivedonethis)
        return 0;

    ivedonethis = 1;

    ifmod_reg.add_if = ether_add_if;
    ifmod_reg.del_if = ether_del_if;
    ifmod_reg.add_proto = ether_add_proto;
    ifmod_reg.del_proto = ether_del_proto;
    ifmod_reg.ifmod_ioctl = ether_ifmod_ioctl;
    ifmod_reg.shutdown    = ether_shutdown;

    if (dlil_reg_if_modules(APPLE_IF_FAM_ETHERNET, &ifmod_reg)) {
        printf("WARNING: ether_family_init -- Can't register if family modules\n");
        error = EIO;
	goto done;
    }

<<<<<<< HEAD
    for (i=0; i < MAX_INTERFACES; i++)
        ether_desc_blk[i].n_count = 0;

    return 0;
>>>>>>> origin/10.1
=======

    /* Register protocol registration functions */
    
    bzero(&enet_protoreg, sizeof(enet_protoreg));
    enet_protoreg.attach_proto = ether_attach_inet;
    enet_protoreg.detach_proto = ether_detach_inet;
    
    if (error = dlil_reg_proto_module(PF_INET, APPLE_IF_FAM_ETHERNET, &enet_protoreg) != 0) {
	printf("ether_family_init: dlil_reg_proto_module failed for AF_INET error=%d\n", error);
	goto done;
    }
    
    enet_protoreg.attach_proto = ether_attach_inet6;
    enet_protoreg.detach_proto = ether_detach_inet6;
    
    if (error = dlil_reg_proto_module(PF_INET6, APPLE_IF_FAM_ETHERNET, &enet_protoreg) != 0) {
	printf("ether_family_init: dlil_reg_proto_module failed for AF_INET6 error=%d\n", error);
	goto done;
    }
    vlan_family_init();

 done:
    thread_funnel_switch(NETWORK_FUNNEL, KERNEL_FUNNEL);

    return (error);
>>>>>>> origin/10.3
}
