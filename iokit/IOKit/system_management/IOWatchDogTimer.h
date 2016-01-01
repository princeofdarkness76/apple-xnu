/*
 * Copyright (c) 2000 Apple Computer, Inc. All rights reserved.
 *
<<<<<<< HEAD
 * @APPLE_OSREFERENCE_LICENSE_HEADER_START@
=======
 * @APPLE_LICENSE_HEADER_START@
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
 * @APPLE_LICENSE_HEADER_END@
>>>>>>> origin/10.2
 */

#ifndef _IOWATCHDOGTIMER_H
#define _IOWATCHDOGTIMER_H

#include <IOKit/IOService.h>

class IOWatchDogTimer : public IOService
{
  OSDeclareAbstractStructors(IOWatchDogTimer);
  
protected:
  IONotifier *notifier;
  struct ExpansionData { };
  ExpansionData *reserved;
  
public:
  virtual bool start(IOService *provider) APPLE_KEXT_OVERRIDE;
  virtual void stop(IOService *provider) APPLE_KEXT_OVERRIDE;
  virtual IOReturn setProperties(OSObject *properties) APPLE_KEXT_OVERRIDE;
  virtual void setWatchDogTimer(UInt32 timeOut) = 0;
  
  OSMetaClassDeclareReservedUnused(IOWatchDogTimer,  0);
  OSMetaClassDeclareReservedUnused(IOWatchDogTimer,  1);
  OSMetaClassDeclareReservedUnused(IOWatchDogTimer,  2);
  OSMetaClassDeclareReservedUnused(IOWatchDogTimer,  3);
};

#endif /* !_IOWATCHDOGTIMER_H */
