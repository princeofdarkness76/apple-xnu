/*
 * Copyright (c) 2000-2002 Apple Computer, Inc. All rights reserved.
 *
<<<<<<< HEAD
 * @APPLE_OSREFERENCE_LICENSE_HEADER_START@
=======
 * @APPLE_LICENSE_HEADER_START@
<<<<<<< HEAD
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
=======
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
>>>>>>> origin/10.3
 * @APPLE_LICENSE_HEADER_END@
>>>>>>> origin/10.2
 */
/* THIS FILE HAS BEEN PRODUCED AUTOMATICALLY */
#ifndef __DEVFS_DEVFS_PROTO_H__
#define __DEVFS_DEVFS_PROTO_H__

#include  <sys/appleapiopts.h>

__BEGIN_DECLS
#ifdef __APPLE_API_PRIVATE
int	devfs_sinit(void);
devdirent_t *	dev_findname(devnode_t * dir, const char *name);
int	dev_add_name(const char * name, devnode_t * dirnode, devdirent_t * back, 
    devnode_t * dnp, devdirent_t * *dirent_pp);
int	dev_add_node(int entrytype, devnode_type_t * typeinfo, devnode_t * proto,
	     devnode_t * *dn_pp, struct devfsmount *dvm);
void	devnode_free(devnode_t * dnp);
int	dev_dup_plane(struct devfsmount *devfs_mp_p);
void	devfs_free_plane(struct devfsmount *devfs_mp_p);
int	dev_free_name(devdirent_t * dirent_p);
int	devfs_dntovn(devnode_t * dnp, struct vnode **vn_pp, struct proc * p);
int	dev_add_entry(const char *name, devnode_t * parent, int type, devnode_type_t * typeinfo,
	      devnode_t * proto, struct devfsmount *dvm, devdirent_t * *nm_pp);
int	devfs_mount(struct mount *mp, vnode_t devvp, user_addr_t data,
	    vfs_context_t context);
int	devfs_kernel_mount(char * mntname);
#endif /* __APPLE_API_PRIVATE */
__END_DECLS

#endif /* __DEVFS_DEVFS_PROTO_H__ */
/* THIS FILE PRODUCED AUTOMATICALLY */
/* DO NOT EDIT (see reproto.sh) */
