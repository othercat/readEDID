/*
ReadEDID sample for RADAR #3162841
arekkusu 10/26/03

parse-edid 1.4.1 source googled from linux distro.
*/

#include <ApplicationServices/ApplicationServices.h>
#include <CoreFoundation/CoreFoundation.h>
#include <IOKit/graphics/IOGraphicsLib.h>

extern int parse_edid( unsigned char* edid );


int main (int argc, const char * argv[]) {
    CFNumberRef refresh = nil;
	float refreshHz = -1;
	
	printf("RADAR #3162841 sample:\n");
	printf("This sample demonstrates the miscommunication between IOKit and\n");
	printf("CoreGraphics about the refresh rate of LCD displays.\n\n");	

	// Ask CoreGraphics what it thinks the refresh rate is. (Normal high level API)
	CFDictionaryRef screen_props = CGDisplayCurrentMode(CGMainDisplayID());
    if (screen_props) refresh = CFDictionaryGetValue(screen_props, kCGDisplayRefreshRate);
	if (refresh) CFNumberGetValue(refresh, kCFNumberFloatType, &refreshHz);
	if (refreshHz > -1) printf("CoreGraphics reports your main display refresh rate is %f Hz.\n\n", refreshHz);
	else				printf("CoreGraphics had a problem reading your main display refresh rate.\n\n");
	

    unsigned char i, EDID[128];
    CFRange allrange = {0, 128};
	CFDictionaryRef displayDict = nil;
	CFDataRef EDIDValue = nil;

    // Now ask IOKit about the EDID reported in the display device (low level)
    io_connect_t displayPort = CGDisplayIOServicePort(CGMainDisplayID());
	if (displayPort) displayDict = IOCreateDisplayInfoDictionary(displayPort, 0);       
    if (displayDict) EDIDValue = CFDictionaryGetValue(displayDict, CFSTR(kIODisplayEDIDKey));
	if (EDIDValue) {	/* this will fail on i.e. televisions connected to powerbook s-video output */ 
		CFDataGetBytes(EDIDValue, allrange, EDID);
		printf("IOKit reports the following EDID for your main display:\n\n");
		for (i = 0; i < 128; i++){
			printf("%02X ", EDID[i]);
			if ((i+1)%16 == 0) printf("\n");
		}
		printf("\n");
		parse_edid(EDID);
		
		printf("\nIf there is a valid reported 'vfreq' in the EDID,\nand CoreGraphics reported 0.000000,\nthen RADAR #3162841 is still valid.\n");
	}
	else printf("IOKit had a problem reading your main display EDID.\nAs long as CoreGraphics did not report 0.000000, this is OK.\n");
    return 0;
}
