//
//  handDisplayAll.c
//  readEDID
//
//  Created by Li Richard on 8/15/14.
//  Copyright (c) 2014 Richard Li. All rights reserved.
//

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/mount.h>
#include <sys/paths.h>
#include <string.h>

#include "enums.h"
#include "structs.h"

#include "blessedid.h"
#include "protos.h"

#import <Foundation/Foundation.h>
#import <ApplicationServices/ApplicationServices.h>
#import <CoreFoundation/CoreFoundation.h>
#import <IOKit/graphics/IOGraphicsLib.h>

typedef unsigned char byte;

extern int parse_edid_full( BLContextPtr context,byte* edid );
extern int parse_edid_productid( BLContextPtr context,byte* edid );

int modeDisplayProductID(BLContextPtr context, struct clarg actargs[klast]) {
    //int ret = 0;
	
    if(!(geteuid() == 0)) {
		blesscontextprintf(context, kBLLogLevelVerbose,  "Not run as root, current uid = %d\n",geteuid() );
		//if (!(geteuid() == 501)) {
		//	blesscontextprintf(context, kBLLogLevelError,  "Not run as administrator\n" );
		//	return 1;
		//}
    }
    
	
    /* try to get the full display data and update a label if present */
	//if(actargs[kgetfulldata].present)
	{
        unsigned char i, EDID[128];
        CFRange allrange = {0, 128};
        CFDictionaryRef displayDict = nil;
        CFDataRef EDIDValue = nil;
        
        // Now ask IOKit about the EDID reported in the display device (low level)
        io_connect_t displayPort = CGDisplayIOServicePort(CGMainDisplayID());
        if (displayPort)
            displayDict = IOCreateDisplayInfoDictionary(displayPort, 0);
        
        if (displayDict)
            EDIDValue = CFDictionaryGetValue(displayDict, CFSTR(kIODisplayEDIDKey));
        
        blesscontextprintf(context, kBLLogLevelVerbose,  "EDID size: %d\n",(int)CFDataGetLength(EDIDValue));

        if (EDIDValue) {	/* this will fail on i.e. televisions connected to powerbook s-video output */
            CFDataGetBytes(EDIDValue, allrange, EDID);
            blesscontextprintf(context, kBLLogLevelVerbose,  "IOKit reports the following EDID for your main display:\n\n" );
            
            for (i = 0; i < 128; i++){
                blesscontextprintf(context, kBLLogLevelVerbose,  "%02X ", EDID[i] );
                if ((i+1)%16 == 0) blesscontextprintf(context, kBLLogLevelVerbose,  "\n");
            }
            for (i = 0; i < 128; i++){
                if (isalpha(EDID[i]))
                    blesscontextprintf(context, kBLLogLevelVerbose,  "%c ", EDID[i] );
                else
                    blesscontextprintf(context, kBLLogLevelVerbose,  "  " );
                if ((i+1)%16 == 0) blesscontextprintf(context, kBLLogLevelVerbose,  "\n");
            }
            blesscontextprintf(context, kBLLogLevelVerbose,  "\n");
            
            parse_edid_productid(context,EDID);

        }
        else blesscontextprintf(context, kBLLogLevelVerbose,  "IOKit had a problem reading your main display EDID.\n");
        
        if (displayDict)
            CFRelease(displayDict);
	}
    
	
	
    return 0;
}