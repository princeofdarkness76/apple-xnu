/*
 * Copyright (c) 1998-2012 Apple Inc. All rights reserved.
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
 * NOTICE: This file was modified by SPARTA, Inc. in 2005 to introduce
 * support for mandatory and extensible security protections.  This notice
 * is included in support of clause 2.2 (b) of the Apple Public License,
 * Version 2.0.
 */
<<<<<<< HEAD
=======
const char * gIOKernelKmods =
<<<<<<< HEAD
"{
<<<<<<< HEAD
<<<<<<< HEAD
    'com.apple.kernel'                         = '1.3.7';
    'com.apple.kernel.bsd'                     = '1.0.7';
    'com.apple.kernel.iokit'                   = '1.0.7';
    'com.apple.kernel.libkern'                 = '1.0.7';
    'com.apple.kernel.mach'                    = '1.0.7';
    'com.apple.iokit.IOADBFamily'              = '1.0.7';
    'com.apple.iokit.IOCDStorageFamily'        = '1.0.7';
    'com.apple.iokit.IODVDStorageFamily'       = '1.0.7';
    'com.apple.iokit.IOGraphicsFamily'         = '1.0.7';
    'com.apple.iokit.IOHIDSystem'              = '1.0.7';
    'com.apple.iokit.IONDRVSupport'            = '1.0.7';
    'com.apple.iokit.IONetworkingFamily'       = '1.0.7';
    'com.apple.iokit.IOPCIFamily'              = '1.0.7';
    'com.apple.iokit.IOStorageFamily'          = '1.0.7';
    'com.apple.iokit.IOSystemManagementFamily' = '1.0.7';
=======
    'com.apple.kernel'                         = '5.5';
    'com.apple.kernel.bsd'                     = '5.5';
    'com.apple.kernel.iokit'                   = '5.5';
    'com.apple.kernel.libkern'                 = '5.5';
    'com.apple.kernel.mach'                    = '5.5';
    'com.apple.iokit.IOADBFamily'              = '1.1';
    'com.apple.iokit.IONVRAMFamily'            = '1.1';
    'com.apple.iokit.IOSystemManagementFamily' = '1.1';
    'com.apple.iokit.ApplePlatformFamily'      = '1.0';
    'com.apple.driver.AppleNMI'                = '1.0';
>>>>>>> origin/10.1
=======
    'com.apple.kernel'                         = '6.8';
    'com.apple.kernel.bsd'                     = '6.8';
    'com.apple.kernel.iokit'                   = '6.8';
    'com.apple.kernel.libkern'                 = '6.8';
    'com.apple.kernel.mach'                    = '6.8';
    'com.apple.iokit.IOADBFamily'              = '6.8';
    'com.apple.iokit.IONVRAMFamily'            = '6.8';
    'com.apple.iokit.IOSystemManagementFamily' = '6.8';
    'com.apple.iokit.ApplePlatformFamily'      = '6.8';
    'com.apple.driver.AppleNMI'                = '6.8';
>>>>>>> origin/10.2
}";
=======
"{"
"   'com.apple.kernel'                         = '7.9.0';"
"   'com.apple.kpi.bsd'                        = '7.9.0';"
"   'com.apple.kpi.iokit'                      = '7.9.0';"
"   'com.apple.kpi.libkern'                    = '7.9.0';"
"   'com.apple.kpi.mach'                       = '7.9.0';"
"   'com.apple.iokit.IONVRAMFamily'            = '7.9.0';"
"   'com.apple.driver.AppleNMI'                = '7.9.0';"
"   'com.apple.iokit.IOSystemManagementFamily' = '7.9.0';"
"   'com.apple.iokit.ApplePlatformFamily'      = '7.9.0';"
"   'com.apple.kernel.6.0'                     = '6.9.9';"
"   'com.apple.kernel.bsd'                     = '6.9.9';"
"   'com.apple.kernel.iokit'                   = '6.9.9';"
"   'com.apple.kernel.libkern'                 = '6.9.9';"
"   'com.apple.kernel.mach'                    = '6.9.9';"
"}";
>>>>>>> origin/10.3

>>>>>>> origin/10.0

const char * gIOKernelConfigTables =
"("
"   {"
"     'IOClass'         = IOPanicPlatform;"
"     'IOProviderClass' = IOPlatformExpertDevice;"
"     'IOProbeScore'    = 0:32;"
"   }"
")";

/* This stuff is no longer used at all but was exported in prior
 * releases, so we'll keep them around for PPC/i386 only.
 * See libkern's OSKext.cpp for other symbols, which have been moved
 * there for sanity.
 */
