//
//  main.c
//  readEDID
//
//  Created by Li Richard on 8/15/14.
//  Copyright (c) 2014 Richard Li. All rights reserved.
//
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
 *  main.c
 *  readEDID
 *
 *  Created by Li Richard <othercat@gmail.com> on Fri Aug 15 2014.
 *  Copyright (c) 2014 Richard Li. All rights reserved.
 *
 *  $Id: blessedid.c,v 1.00 2014/08/15 18:21:05 GMT+8 lirichard Exp $
 *
 */
//#include <CoreFoundation/CoreFoundation.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <unistd.h>
#include <getopt.h>
#include <err.h>
#include <sys/stat.h>
#include <sys/mount.h>
#include <sys/types.h>
#include <sys/sysctl.h>

#include "enums.h"
#include "structs.h"

#include "blessedid.h"
#include "bless_private.h"
#include "protos.h"

struct clarg actargs[klast];

/*
 * To add an option, allocate an enum in enums.h, add a getopt_long entry here,
 * add to main(), add to usage and man page
 */

/* options descriptor */
static struct option longopts[] = {
	{ "getFullData",        no_argument,			0,              kgetfulldata },
	{ "getProductId",       no_argument,			0,              kgetproductid },
	{ "getManufactureDate",	no_argument,			0,              kgetmanufacturedate },
	{ "help",               no_argument,			0,              khelp },
	{ "verbose",            no_argument,			0,              kverbose },
	{ "version",            no_argument,            0,              kversion },
	{ 0,                    0,                      0,              0 }
};

extern int modeDisplayAll(BLContextPtr context, struct clarg actargs[klast]);
extern int modeDisplayNormal(BLContextPtr context, struct clarg actargs[klast]);
extern int modeDisplayProductID(BLContextPtr context, struct clarg actargs[klast]);

int main (int argc, char * argv[])
{
	
    int ch, longindex;
    BLContext context;
    struct blesscon bcon;
    double blessdiskVersionNumber = 1.00;
	
    bcon.quiet = 0;
    bcon.verbose = 0;
	
    context.version = 0;
    context.logstring = blesslog;
    context.logrefcon = &bcon;
	
    if(argc == 0) {
        usage_short();
    }
    
    if(getenv("BL_PRINT_ARGUMENTS")) {
        int i;
        for(i=0; i < argc; i++) {
            fprintf(stderr, "argv[%d] = '%s'\n", i, argv[i]);
        }
    }
    
	
    
    while ((ch = getopt_long_only(argc, argv, "", longopts, &longindex)) != -1) {
        
        switch(ch) {
            case khelp:
                usage();
                break;
            case kquiet:
                break;
            case kverbose:
                bcon.verbose = 1;
                break;
            case kversion:
                printf("%.1f by richardli at 2014/08/15 18:21:05 GMT+8\n", blessdiskVersionNumber);
                exit(0);
                break;
            case kpayload:
                actargs[ch].present = 1;
                break;
            case ksave9:
                // ignore, this is now always saved as alternateos
                break;
            case '?':
            case ':':
                usage_short();
                break;
            default:
                // common handling for all other options
            {
                struct option *opt = &longopts[longindex];
                
                
                if(actargs[ch].present) {
                    warnx("Option \"%s\" already specified", opt->name);
                    usage_short();
                    break;
                } else {
                    actargs[ch].present = 1;
                }
                
                switch(opt->has_arg) {
                    case no_argument:
                        actargs[ch].hasArg = 0;
                        break;
                    case required_argument:
                        actargs[ch].hasArg = 1;
                        strlcpy(actargs[ch].argument, optarg, sizeof(actargs[ch].argument));
                        break;
                    case optional_argument:
                        if(argv[optind] && argv[optind][0] != '-') {
                            actargs[ch].hasArg = 1;
                            strlcpy(actargs[ch].argument, argv[optind], sizeof(actargs[ch].argument));
                        } else {
                            actargs[ch].hasArg = 0;
                        }
                        break;
                }
            }
                break;
        }
    }
	
    argc -= optind;
    argc += optind;
    
    /* There are 1 public mode of execution: disk
     * There is 0 private mode:
     * These are all one-way function jumps.
     */
	
    /* If it was requested, print out the Finder Info words */
    if(actargs[kgetfulldata].present) {
        return modeDisplayAll(&context, actargs);
    }
	
    
    if(actargs[kgetproductid].present) {
        return modeDisplayProductID(&context, actargs);
    }
	/*
	if(actargs[kgetmanufacturedate].present) {
		return modeDisplayManufactureDate(&context, actargs);
	}
	*/
    /* default */
    return modeDisplayNormal(&context, actargs);
    
	
}


int blesscontextprintf(BLContextPtr context, int loglevel, char const *fmt, ...) {
    int ret;
    char *out;
    va_list ap;
	
    va_start(ap, fmt);
    ret = vasprintf(&out, fmt, ap);
    va_end(ap);
    
    if((ret == -1) || (out == NULL)) {
        return context->logstring(context->logrefcon, loglevel, "Memory error, log entry not available");
    }
	
    ret = context->logstring(context->logrefcon, loglevel, out);
    free(out);
    return ret;
}

