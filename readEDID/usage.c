/*
 * Copyright (c) 2001-2007 Apple Inc. All Rights Reserved.
 *
 * @APPLE_LICENSE_HEADER_START@
 * 
 * This file contains Original Code and/or Modifications of Original Code
 * as defined in and that are subject to the Apple Public Source License
 * Version 2.0 (the 'License'). You may not use this file except in
 * compliance with the License. Please obtain a copy of the License at
 * http://www.opensource.apple.com/apsl/ and read it before using this
 * file.
 * 
 * The Original Code and all software distributed under the License are
 * distributed on an 'AS IS' basis, WITHOUT WARRANTY OF ANY KIND, EITHER
 * EXPRESS OR IMPLIED, AND APPLE HEREBY DISCLAIMS ALL SUCH WARRANTIES,
 * INCLUDING WITHOUT LIMITATION, ANY WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE, QUIET ENJOYMENT OR NON-INFRINGEMENT.
 * Please see the License for the specific language governing rights and
 * limitations under the License.
 * 
 * @APPLE_LICENSE_HEADER_END@
 */
/*
 *  usage.c
 *  bless
 *
 *  Created by Shantonu Sen <ssen@apple.com> on Wed Nov 14 2001.
 *  Copyright (c) 2001-2007 Apple Inc. All Rights Reserved.
 *
 *  $Id: usage.c,v 1.31 2005/12/05 12:59:30 ssen Exp $
 *
 */

#include <stdlib.h>
#include <stdio.h>

#include "blessedid.h"

#include "enums.h"
#include "structs.h"
#include "protos.h"

void usage(void) {
    fprintf(stderr, "Usage: %s [options]\n", getprogname());
    fputs(
"\t--help\t\t\tThis usage statement\n"
"\n"
"Info Mode:\n"
"\t--getFullData\t\tSuppress full output and print the main display EDID data\n"
"\t--getProductId\t\tSuppress normal output and print the main display Product ID\n"
"\t--version\t\tPrint version number\n"
"\t--verbose\t\tVerbose output\n"
"\n"
"Display Mode:\n"
//"\t--getMainDisplayIdx\tSuppress normal output and print the active main display index\n"
//"\t--listDisplayIdx\t\tList the whole display indexes\n"
"\t--verbose\t\tVerbose output\n"
          ,
          stderr);
    
    exit(1);
}

/* Basically lifted from the man page */
void usage_short(void) {
    fprintf(stderr, "Usage: %s [options]\n", getprogname());
    fputs(
"readedid --help\n"
"\n"
"readedid --getFullData [--verbose]\n"
"\n"
"readedid --getProductId [--verbose]\n"
"\n"
"readedid --getManufactureDate [--verbose]\n"
,
	  stderr);
    exit(1);
}
