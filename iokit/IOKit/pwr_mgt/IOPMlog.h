/*
 * Copyright (c) 1998-2000 Apple Computer, Inc. All rights reserved.
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
enum PMLogEnum {
<<<<<<< HEAD
    kPMLogSetParent = 1,            //  1	0x05070004
    kPMLogAddChild,                 //  2	0x05070008
    kPMLogRemoveChild,              //  3	0x0507000c
    kPMLogControllingDriver,        //  4	0x05070010
    kPMLogControllingDriverErr1,    //  5	0x05070014 - bad power state array version
    kPMLogControllingDriverErr2,    //  6	0x05070018 - power states already registered
    kPMLogControllingDriverErr3,    //  7	0x0507001c
    kPMLogControllingDriverErr4,    //  8	0x05070020 - power driver is invalid
    kPMLogInterestedDriver,         //  9	0x05070024
    kPMLogAcknowledgeErr1,          // 10	0x05070028 - unknown entity called acknowledgePowerChange
    kPMLogChildAcknowledge,         // 11	0x0507002c
    kPMLogDriverAcknowledge,        // 12	0x05070030 - interested driver acknowledges
    kPMLogAcknowledgeErr2,          // 13	0x05070034 - object has already acked
    kPMLogAcknowledgeErr3,          // 14	0x05070038 - not expecting any acks
    kPMLogAcknowledgeErr4,          // 15	0x0507003c - not expecting acknowledgeSetPowerState
    kPMLogDriverAcknowledgeSet,     // 16	0x05070040 - controlling driver acknowledges
    kPMLogWillChange,               // 17	0x05070044
    kPMLogDidChange,                // 18	0x05070048
    kPMLogRequestDomain,            // 19	0x0507004c
    kPMLogMakeUsable,               // 20	0x05070050
    kPMLogChangeStateTo,            // 21	0x05070054
    kPMLogChangeStateToPriv,        // 22	0x05070058
    kPMLogSetAggressiveness,        // 23	0x0507005c
    kPMLogCriticalTemp,             // 24	0x05070060
    kPMLogOverrideOn,               // 25	0x05070064
    kPMLogOverrideOff,              // 26	0x05070068
    kPMLogChangeStateForRootDomain, // 27	0x0507006c
    kPMLogCSynchronizePowerTree,    // 28	0x05070070
    kPMLogChangeDone,               // 29	0x05070074
    kPMLogCtrlDriverTardy,          // 30	0x05070078 - controlling driver didn't acknowledge
    kPMLogIntDriverTardy,           // 31	0x0507007c - interested driver didn't acknowledge
    kPMLogStartAckTimer,            // 32	0x05070080
    kPMLogStartParentChange,        // 33	0x05070084
    kPMLogAmendParentChange,        // 34	0x05070088
    kPMLogStartDeviceChange,        // 35	0x0507008c
    kPMLogRequestDenied,            // 36	0x05070090 - parent denied domain state change request
    kPMLogControllingDriverErr5,    // 37	0x05070094 - too few power states
    kPMLogProgramHardware,          // 38	0x05070098
    kPMLogInformDriverPreChange,    // 39	0x0507009c
    kPMLogInformDriverPostChange,   // 40	0x050700a0
    kPMLogRemoveDriver,             // 41	0x050700a4 - NOT USED
    kPMLogSetIdleTimerPeriod,       // 42	0x050700a8
    kPMLogSystemWake,               // 43	0x050700ac
    kPMLogAcknowledgeErr5,          // 44	0x050700b0
    kPMLogClientAcknowledge,        // 45	0x050700b4
    kPMLogClientTardy,              // 46	0x050700b8 - application didn't acknowledge
    kPMLogClientCancel,             // 47	0x050700bc - NOT USED
    kPMLogClientNotify,             // 48	0x050700c0 - client sent a notification
    kPMLogAppNotify,                // 49	0x050700c4 - application sent a notification
    kPMLogSetClockGating,           // 50	0x050700c8 - NOT USED
    kPMLogSetPowerGating,           // 51	0x050700cc - NOT USED
    kPMLogSetPinGroup,              // 52	0x050700d0 - NOT USED
    kPMLogIdleCancel,               // 53	0x050700d4 - device unidle during change
    kPMLogSleepWakeTracePoint,      // 54   0x050700d8 - kIOPMTracePoint markers
    kPMLogQuiescePowerTree,         // 55   0x050700dc
=======
    kPMLogSetParent = 1,		//  1	0x05100004
    kPMLogAddChild,			//  2	0x05100008
    kPMLogRemoveChild,			//  3	0x0510000c
    kPMLogControllingDriver,		//  4	0x05100010
    kPMLogControllingDriverErr1,	//  5	0x05100014 - bad power state array version
    kPMLogControllingDriverErr2,	//  6	0x05100018 - power states already registered
    kPMLogControllingDriverErr3,	//  7	0x0510001c
    kPMLogControllingDriverErr4,	//  8	0x05100020 - power driver is invalid
    kPMLogInterestedDriver,		//  9	0x05100024
    kPMLogAcknowledgeErr1,		// 10	0x05100028 - unknown entity called acknowledgePowerChange
    kPMLogChildAcknowledge,		// 11	0x0510002c
    kPMLogDriverAcknowledge,		// 12	0x05100030 - interested driver acknowledges
    kPMLogAcknowledgeErr2,		// 13	0x05100034 - object has already acked
    kPMLogAcknowledgeErr3,		// 14	0x05100038 - not expecting any acks
    kPMLogAcknowledgeErr4,		// 15	0x0510003c - not expecting acknowledgeSetPowerState
    kPMLogDriverAcknowledgeSet,		// 16	0x05100040 - controlling driver acknowledges
    kPMLogWillChange,			// 17	0x05100044
    kPMLogDidChange,			// 18	0x05100048
    kPMLogRequestDomain,		// 19	0x0510004c
    kPMLogMakeUsable,			// 20	0x05100050
    kPMLogChangeStateTo,		// 21	0x05100054
    kPMLogChangeStateToPriv,		// 22	0x05100058
    kPMLogSetAggressiveness,		// 23	0x0510005c
    kPMLogCriticalTemp,			// 24	0x05100060
    kPMLogOverrideOn,			// 25	0x05100064
    kPMLogOverrideOff,			// 26	0x05100068
    kPMLogEnqueueErr,			// 27	0x0510006c - change queue overflow
    kPMLogCollapseQueue,		// 28	0x05100070
    kPMLogChangeDone,			// 29	0x05100074
    kPMLogCtrlDriverTardy,		// 30	0x05100078 - controlling driver didn't acknowledge
    kPMLogIntDriverTardy,		// 31	0x0510007c - interested driver didn't acknowledge
    kPMLogStartAckTimer,		// 32	0x05100080
    kPMLogStartParentChange,		// 33	0x05100084
    kPMLogAmendParentChange,		// 34	0x05100088
    kPMLogStartDeviceChange,		// 35	0x0510008c
    kPMLogRequestDenied,		// 36	0x05100090 - parent denied domain state change request
    kPMLogControllingDriverErr5,	// 37	0x05100094 - too few power states
    kPMLogProgramHardware,		// 38	0x05100098
    kPMLogInformDriverPreChange,	// 39	0x0510009c
    kPMLogInformDriverPostChange,	// 40	0x051000a0
    kPMLogRemoveDriver,			// 41	0x051000a4
    kPMLogSetIdleTimerPeriod,		// 42	0x051000a8
    kPMLogSystemWake,			// 43	0x051000ac
    kPMLogAcknowledgeErr5,		// 44	0x051000b0
    kPMLogClientAcknowledge,		// 45	0x051000b4
    kPMLogClientTardy,			// 46	0x051000b8 - application didn't acknowledge
    kPMLogClientCancel,			// 47	0x051000bc
    kPMLogClientNotify,			// 48	0x051000c0 - client sent a notification
    kPMLogAppNotify,			// 49	0x051000c4 - application sent a notification
    kPMLogSetClockGating,		// 50	0x051000c8 - platform device specific clock control
    kPMLogSetPowerGating,		// 51	0x051000cc - platform device specific power control
    kPMLogSetPinGroup,			// 52	0x051000d0 - platform device specific gpio control
    kPMLogIdleCancel,			// 53	0x051000d4 - device unidle during change
>>>>>>> origin/10.5
    kIOPMlogLastEvent
};
