/*
 * Copyright (c) 1992,1993-1995 Argonaut Technologies Limited. All rights reserved.
 *
 * $Id: host.h 2.9 1997/08/13 10:42:18 JOHNG Exp $
 * $Locker: $
 */
#ifndef _HOST_H_
#define _HOST_H_

#ifndef _BRENDER_H_
#include "brender.h"
#endif

/*
 * Structure used to describe host information
 */
typedef struct host_info {
	br_uint_32 size;
	char identifier[40];
	br_uint_32 capabilities;
	br_token processor_family;
	br_token processor_type;
} host_info;

#define HOST_CAPS_REAL_MEMORY			0x00000001	/* Can allocate/read/write real-mode mem.	*/
#define HOST_CAPS_REAL_INT_CALL			0x00000002	/* Can invoke real mode interrupts			*/
#define HOST_CAPS_REAL_INT_HOOK			0x00000004	/* Can hook real mode interrupts			*/
#define HOST_CAPS_PROTECTED_INT_CALL	0x00000008	/* Can invoke protected mode interrupts		*/
#define HOST_CAPS_PROTECTED_INT_HOOK	0x00000010	/* Can hook prot. mode interrupts			*/
#define HOST_CAPS_ALLOC_SELECTORS		0x00000020	/* Can allocate new selectors				*/
#define HOST_CAPS_PHYSICAL_MAP			0x00000040	/* Can map physical memory -> linear		*/
#define HOST_CAPS_EXCEPTION_HOOK		0x00000080	/* Can hook exceptions						*/
#define HOST_CAPS_BASE_SELECTORS_WRITE	0x00000100	/* Can modify base/limit of cs,ds,es,ss selectors	*/
#define HOST_CAPS_PORTS					0x00000200	/* Can use IO ports							*/
#define HOST_CAPS_MMX					0x00000400	/* Has MMX extensions						*/
#define HOST_CAPS_FPU					0x00000800	/* Has hardware FPU							*/
#define HOST_CAPS_CMOV                  0x00001000  /* Has CMOV extensions */


#ifndef _HOST_P_H_
#include "host_p.h"
#endif

#endif

