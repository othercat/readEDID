//
//  handEDID.c
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
#include "modes.h"
// TODO: rewrite
// FIXME: cleanup 'static' variables

typedef unsigned char byte;
/* byte must be 8 bits */

/* int must be at least 16 bits */

/* long must be at least 32 bits */

#define VERSION "2.0.0"

#define DIE_MSG( x ) \
{ MSG( x ); exit( 1 ); }


#define UPPER_NIBBLE( x ) \
(((128|64|32|16) & (x)) >> 4)

#define LOWER_NIBBLE( x ) \
((1|2|4|8) & (x))

#define COMBINE_HI_8LO( hi, lo ) \
( (((unsigned)hi) << 8) | (unsigned)lo )

#define COMBINE_HI_4LO( hi, lo ) \
( (((unsigned)hi) << 4) | (unsigned)lo )

const byte edid_v1_header[] = { 0x00, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0x00 };

const byte edid_v1_descriptor_flag[] = { 0x00, 0x00 };


#define EDID_LENGTH                             0x80

#define EDID_HEADER                             0x00
#define EDID_HEADER_END                         0x07

#define ID_MANUFACTURER_NAME                    0x08
#define ID_MANUFACTURER_NAME_END                0x09
#define ID_MODEL				0x0a

#define ID_SERIAL_NUMBER			0x0c

#define MANUFACTURE_WEEK			0x10
#define MANUFACTURE_YEAR			0x11

#define EDID_STRUCT_VERSION                     0x12
#define EDID_STRUCT_REVISION                    0x13

#define DPMS_FLAGS				0x18
#define ESTABLISHED_TIMING_1                    0x23
#define ESTABLISHED_TIMING_2                    0x24
#define MANUFACTURERS_TIMINGS                   0x25

#define DETAILED_TIMING_DESCRIPTIONS_START      0x36
#define DETAILED_TIMING_DESCRIPTION_SIZE        18
#define NO_DETAILED_TIMING_DESCRIPTIONS         4



#define DETAILED_TIMING_DESCRIPTION_1           0x36
#define DETAILED_TIMING_DESCRIPTION_2           0x48
#define DETAILED_TIMING_DESCRIPTION_3           0x5a
#define DETAILED_TIMING_DESCRIPTION_4           0x6c



#define PIXEL_CLOCK_LO     (unsigned)dtd[ 0 ]
#define PIXEL_CLOCK_HI     (unsigned)dtd[ 1 ]
#define PIXEL_CLOCK        (COMBINE_HI_8LO( PIXEL_CLOCK_HI,PIXEL_CLOCK_LO )*10000)

#define H_ACTIVE_LO        (unsigned)dtd[ 2 ]

#define H_BLANKING_LO      (unsigned)dtd[ 3 ]

#define H_ACTIVE_HI        UPPER_NIBBLE( (unsigned)dtd[ 4 ] )

#define H_ACTIVE           COMBINE_HI_8LO( H_ACTIVE_HI, H_ACTIVE_LO )

#define H_BLANKING_HI      LOWER_NIBBLE( (unsigned)dtd[ 4 ] )

#define H_BLANKING         COMBINE_HI_8LO( H_BLANKING_HI, H_BLANKING_LO )




#define V_ACTIVE_LO        (unsigned)dtd[ 5 ]

#define V_BLANKING_LO      (unsigned)dtd[ 6 ]

#define V_ACTIVE_HI        UPPER_NIBBLE( (unsigned)dtd[ 7 ] )

#define V_ACTIVE           COMBINE_HI_8LO( V_ACTIVE_HI, V_ACTIVE_LO )

#define V_BLANKING_HI      LOWER_NIBBLE( (unsigned)dtd[ 7 ] )

#define V_BLANKING         COMBINE_HI_8LO( V_BLANKING_HI, V_BLANKING_LO )



#define H_SYNC_OFFSET_LO   (unsigned)dtd[ 8 ]
#define H_SYNC_WIDTH_LO    (unsigned)dtd[ 9 ]

#define V_SYNC_OFFSET_LO   UPPER_NIBBLE( (unsigned)dtd[ 10 ] )
#define V_SYNC_WIDTH_LO    LOWER_NIBBLE( (unsigned)dtd[ 10 ] )

#define V_SYNC_WIDTH_HI    ((unsigned)dtd[ 11 ] & (1|2))
#define V_SYNC_OFFSET_HI   (((unsigned)dtd[ 11 ] & (4|8)) >> 2)

#define H_SYNC_WIDTH_HI    (((unsigned)dtd[ 11 ] & (16|32)) >> 4)
#define H_SYNC_OFFSET_HI   (((unsigned)dtd[ 11 ] & (64|128)) >> 6)


#define V_SYNC_WIDTH       COMBINE_HI_4LO( V_SYNC_WIDTH_HI, V_SYNC_WIDTH_LO )
#define V_SYNC_OFFSET      COMBINE_HI_4LO( V_SYNC_OFFSET_HI, V_SYNC_OFFSET_LO )

#define H_SYNC_WIDTH       COMBINE_HI_4LO( H_SYNC_WIDTH_HI, H_SYNC_WIDTH_LO )
#define H_SYNC_OFFSET      COMBINE_HI_4LO( H_SYNC_OFFSET_HI, H_SYNC_OFFSET_LO )

#define H_SIZE_LO          (unsigned)dtd[ 12 ]
#define V_SIZE_LO          (unsigned)dtd[ 13 ]

#define H_SIZE_HI          UPPER_NIBBLE( (unsigned)dtd[ 14 ] )
#define V_SIZE_HI          LOWER_NIBBLE( (unsigned)dtd[ 14 ] )

#define H_SIZE             COMBINE_HI_8LO( H_SIZE_HI, H_SIZE_LO )
#define V_SIZE             COMBINE_HI_8LO( V_SIZE_HI, V_SIZE_LO )

#define H_BORDER           (unsigned)dtd[ 15 ]
#define V_BORDER           (unsigned)dtd[ 16 ]

#define FLAGS              (unsigned)dtd[ 17 ]

#define INTERLACED         (FLAGS&128)
#define SYNC_TYPE	   (FLAGS&3<<3)  /* bits 4,3 */
#define SYNC_SEPARATE	   (3<<3)
#define HSYNC_POSITIVE	   (FLAGS & 4)
#define VSYNC_POSITIVE     (FLAGS & 2)

#define MONITOR_NAME            0xfc
#define MONITOR_LIMITS          0xfd
#define UNKNOWN_DESCRIPTOR      -1
#define DETAILED_TIMING_BLOCK   -2


#define DESCRIPTOR_DATA         5
#define V_MIN_RATE              block[ 5 ]
#define V_MAX_RATE              block[ 6 ]
#define H_MIN_RATE              block[ 7 ]
#define H_MAX_RATE              block[ 8 ]

#define MAX_PIXEL_CLOCK         (((int)block[ 9 ]) * 10)
#define GTF_SUPPORT		block[10]

#define DPMS_ACTIVE_OFF		(1 << 5)
#define DPMS_SUSPEND		(1 << 6)
#define DPMS_STANDBY		(1 << 7)

char* myname;

void MSG( const char* x )
{
    fprintf( stderr, "%s: %s\n", myname, x );
}


int
parse_edid( byte* edid );


int
parse_timing_description( byte* dtd );


int
parse_monitor_limits( byte* block );

int
block_type( byte* block );

char*
get_monitor_name( byte const*  block );

char*
get_vendor_sign( byte const* block );

int
parse_dpms_capabilities( byte flags );

/*
 int
 main( int argc, char** argv )
 {
 byte edid[ EDID_LENGTH ];
 FILE* edid_file;
 
 myname = argv[ 0 ];
 fprintf( stderr, "%s: parse-edid version %s\n", myname, VERSION );
 
 if ( argc > 2 )
 {
 DIE_MSG( "syntax: [input EDID file]" );
 }
 else
 {
 if ( argc == 2 )
 {
 edid_file = fopen( argv[ 1 ], "rb" );
 if ( !edid_file )
 DIE_MSG( "unable to open file for input" );
 }
 
 else
 edid_file = stdin;
 }
 
 if ( fread( edid, sizeof( byte ), EDID_LENGTH, edid_file )
 != EDID_LENGTH )
 
 {
 DIE_MSG( "IO error reading EDID" );
 }
 
 fclose( edid_file );
 
 return parse_edid( edid );
 }
 */

#define CHECK_BIT(var,pos)                ((var) & (1<<(pos)))
#define EDID_COMBINE_HI_8LO( hi, lo )     ((((unsigned)hi) << 8) | (unsigned)lo )
#define EDID_LENGTH                       0x80
#define EDID_SYS_PATH                     "/sys/class/drm"
#define EDID_VIDEO_INPUT_DFP1_COMPAT      0x01
#define EDID_VIDEO_INPUT_LEVEL(x)         (((x) & 0x60) >> 5)
#define EDID_DPMS_ACTIVE_OFF              (1 << 5)
#define EDID_DPMS_SUSPEND                 (1 << 6)
#define EDID_DPMS_STANDBY                 (1 << 7)
#define EDID_STD_TIMING_HRES(ptr)         ((((ptr)[0]) * 8) + 248)
#define EDID_STD_TIMING_VFREQ(ptr)        ((((ptr)[1]) & 0x3f) + 60)
#define EDID_STD_TIMING_RATIO(ptr)        ((ptr)[1] & 0xc0)
#define EDID_BLOCK_IS_DET_TIMING(ptr)     ((ptr)[0] | (ptr)[1])
#define EDID_DET_TIMING_DOT_CLOCK(ptr)    (((ptr)[0] | ((ptr)[1] << 8)) * 10000)
#define EDID_HACT_LO(ptr)                 ((ptr)[2])
#define EDID_HBLK_LO(ptr)                 ((ptr)[3])
#define EDID_HACT_HI(ptr)                 (((ptr)[4] & 0xf0) << 4)
#define EDID_HBLK_HI(ptr)                 (((ptr)[4] & 0x0f) << 8)
#define EDID_DET_TIMING_HACTIVE(ptr)      (EDID_HACT_LO(ptr) | EDID_HACT_HI(ptr))
#define EDID_DET_TIMING_HBLANK(ptr)       (EDID_HBLK_LO(ptr) | EDID_HBLK_HI(ptr))
#define EDID_VACT_LO(ptr)                 ((ptr)[5])
#define EDID_VBLK_LO(ptr)                 ((ptr)[6])
#define EDID_VACT_HI(ptr)                 (((ptr)[7] & 0xf0) << 4)
#define EDID_VBLK_HI(ptr)                 (((ptr)[7] & 0x0f) << 8)
#define EDID_DET_TIMING_VACTIVE(ptr)      (EDID_VACT_LO(ptr) | EDID_VACT_HI(ptr))
#define EDID_DET_TIMING_VBLANK(ptr)       (EDID_VBLK_LO(ptr) | EDID_VBLK_HI(ptr))
#define EDID_HOFF_LO(ptr)                 ((ptr)[8])
#define EDID_HWID_LO(ptr)                 ((ptr)[9])
#define EDID_VOFF_LO(ptr)                 ((ptr)[10] >> 4)
#define EDID_VWID_LO(ptr)                 ((ptr)[10] & 0xf)
#define EDID_HOFF_HI(ptr)                 (((ptr)[11] & 0xc0) << 2)
#define EDID_HWID_HI(ptr)                 (((ptr)[11] & 0x30) << 4)
#define EDID_VOFF_HI(ptr)                 (((ptr)[11] & 0x0c) << 2)
#define EDID_VWID_HI(ptr)                 (((ptr)[11] & 0x03) << 4)
#define EDID_DET_TIMING_HSYNC_OFFSET(ptr) (EDID_HOFF_LO(ptr) | EDID_HOFF_HI(ptr))
#define EDID_DET_TIMING_HSYNC_WIDTH(ptr)  (EDID_HWID_LO(ptr) | EDID_HWID_HI(ptr))
#define EDID_DET_TIMING_VSYNC_OFFSET(ptr) (EDID_VOFF_LO(ptr) | EDID_VOFF_HI(ptr))
#define EDID_DET_TIMING_VSYNC_WIDTH(ptr)  (EDID_VWID_LO(ptr) | EDID_VWID_HI(ptr))
#define EDID_HSZ_LO(ptr)                  ((ptr)[12])
#define EDID_VSZ_LO(ptr)                  ((ptr)[13])
#define EDID_HSZ_HI(ptr)                  (((ptr)[14] & 0xf0) << 4)
#define EDID_VSZ_HI(ptr)                  (((ptr)[14] & 0x0f) << 8)
#define EDID_DET_TIMING_HSIZE(ptr)        (EDID_HSZ_LO(ptr) | EDID_HSZ_HI(ptr))
#define EDID_DET_TIMING_VSIZE(ptr)        (EDID_VSZ_LO(ptr) | EDID_VSZ_HI(ptr))
#define EDID_DET_TIMING_HBORDER(ptr)      ((ptr)[15])
#define EDID_DET_TIMING_VBORDER(ptr)      ((ptr)[16])
#define EDID_DET_TIMING_FLAGS(ptr)        ((ptr)[17])
#define EDID_DET_TIMING_VSOBVHSPW(ptr)    ((ptr)[11])

struct edid_block {
    uint8_t  header[8];               // EDID header "00 FF FF FF FF FF FF 00"
    uint16_t manufacturerCode;        // EISA 3-character ID
    uint16_t productCode;             // Vendor assigned code
    uint32_t serialNumber;            // Serial number
    uint8_t  manufacturedWeek;        // Week number
    uint8_t  manufacturedYear;        // Year number + 1990
    uint8_t  version;                 // EDID version
    uint8_t  revision;                // EDID revision
    uint8_t  videoInputDefinition;
    uint8_t  maxHorizontalImageSize;  // in cm
    uint8_t  maxVerticalImageSize;    // in cm
    uint8_t  displayGamma;            // gamma
    uint8_t  dpmSupport;              // DPMS
    uint8_t  redGreenLowBits;         // Rx1 Rx0 Ry1 Ry0 Gx1 Gx0 Gy1Gy0
    uint8_t  blueWhiteLowBits;        // Bx1 Bx0 By1 By0 Wx1 Wx0 Wy1 Wy0
    uint8_t  redX;                    // Red-x Bits 9 - 2
    uint8_t  redY;                    // Red-y Bits 9 - 2
    uint8_t  greenX;                  // Green-x Bits 9 - 2
    uint8_t  greenY;                  // Green-y Bits 9 - 2
    uint8_t  blueX;                   // Blue-x Bits 9 - 2
    uint8_t  blueY;                   // Blue-y Bits 9 - 2
    uint8_t  whiteX;                  // White-x Bits 9 - 2
    uint8_t  whiteY;                  // White-x Bits 9 - 2
    uint8_t  establishedTimings[3];
    uint8_t  standardTimings[16];
    uint8_t  descriptionBlock1[18];
    uint8_t  descriptionBlock2[18];
    uint8_t  descriptionBlock3[18];
    uint8_t  descriptionBlock4[18];
    uint8_t  extensions;              // Number of (optional) 128-byte EDID extension blocks
    uint8_t  checksum;
} __attribute__((packed));


// from NetBSD edid code
const char *edid_established_modes[] = {
    "1280x1024 @ 75Hz",
    "1024x768 @ 75Hz",
    "1024x768 @ 70Hz",
    "1024x768 @ 60Hz",
    "1024x768 @ 87Hz",
    "832x624 @ 75Hz",
    "800x600 @ 75Hz",
    "800x600 @ 72Hz",
    "800x600 @ 60Hz",
    "800x600 @ 56Hz",
    "640x480 @ 75Hz",
    "640x480 @ 72Hz",
    "640x480 @ 67Hz",
    "640x480 @ 60Hz",
    "720x400 @ 88Hz",
    "720x400 @ 70Hz"
};

int last_stb_mode = 0;

char *
manufacturer_abbrev(uint8_t *edid)
{
    struct edid_block *eb = (struct edid_block *)edid;
    static char mcode[8];
    uint16_t h;
    uint8_t *block = (uint8_t *)&(eb->manufacturerCode);
    
    h = EDID_COMBINE_HI_8LO(block[0], block[1]);
    mcode[0] = ((h>>10) & 0x1f) + 'A' - 1;
    mcode[1] = ((h>>5) & 0x1f) + 'A' - 1;
    mcode[2] = (h & 0x1f) + 'A' - 1;
    mcode[3] = 0;
    
    return mcode;
}



char *
display_gamma(uint8_t *edid)
{
    struct edid_block *eb = (struct edid_block *)edid;
    static char mcode[8];
    float g1 = (float)eb->displayGamma;
    float g2 = 1.00 + (g1/100);
    
    sprintf(mcode, "%.3f", g2);
    
    return mcode;
}

void
print_std_timing(uint8_t *stb)
{
    unsigned  hres, vres, freq;
    float aspectratio;
    
    if ((stb[0] == 1 && stb[1] == 1) ||
        (stb[0] == 0 && stb[1] == 0) ||
        (stb[0] == 0x20 && stb[1] == 0x20))
        return;
    
    hres = EDID_STD_TIMING_HRES(stb);
    switch (EDID_STD_TIMING_RATIO(stb)) {
        case 0x00:
            aspectratio = 10.0/16;
            break;
        case 0x40:
            aspectratio = 3.0/4;
            break;
        case 0x80:
            aspectratio = 4.0/5;
            break;
        case 0xC0:
        default:
            aspectratio = 9.0/16;
            break;
    }
    vres = hres * aspectratio;
    freq = EDID_STD_TIMING_VFREQ(stb);
    
    printf("       <Resolution>\n");
    printf("           <Mode>%d</Mode>\n", last_stb_mode);
    printf("           <XResolution>%d</XResolution>\n", hres);
    printf("           <YResolution>%d</YResolution>\n", vres);
    printf("           <AspectRatio>%.3f</AspectRatio>\n", aspectratio);
    printf("           <VertRefreshFreq units=\"Hz\">%d</VertRefreshFreq>\n", freq);
    printf("       </Resolution>\n");
    
    last_stb_mode++;
}



void
print_description_block(BLContextPtr context,uint8_t *data, int num)
{
    static char mcode[20];
    char *c  = mcode;
    
    switch (*(data + 3)) {
        case  0xFA:
            print_std_timing(data + 5);
            break;
            
        case  0xFB:
            break;
    }
    
    blesscontextprintf(context, kBLLogLevelVerbose,  "   DescriptorBlock number<%d>\n", num);

    switch (*(data + 3)) {
        case  0xFC:
        case  0xFF:
        case  0xFE:
            memcpy(mcode, data + 5, 13);
            while (*c != 0x0A)
                c++;
            *c = '\0';
            if (*(data + 3) == 0xFC) {
                printf("       ModelNumber \"%s\"\n", mcode);
            } else if (*(data + 3) == 0xFF) {
                printf("       SerialNumber \"%s\"\n", mcode);
            } else {
                printf("       Comment \"%s\"\n", mcode);
            }
            break;
            
        case  0xFD:
            printf("       MinVertRefreshFreq \"%d HZ\"\n", *(data + 5));
            printf("       MaxVertRefreshFreq \"%d HZ\"\n", *(data + 6));
            printf("       MinHoriRefreshFreq \"%d KHZ\"\n", *(data + 7));
            printf("       MaxHoriRefreshFreq \"%d KHz\"\n", *(data + 8));
    }
    //blesscontextprintf(context, kBLLogLevelVerbose,  "   </DescriptorBlock>\n");

}

int
check_edid(uint8_t *edid)
{
    uint8_t i;
    uint8_t checksum = 0;
    const uint8_t eb_header[] = {0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00};
    struct edid_block *eb = (struct edid_block *)edid;
    
    // check EDID block checksum
    for (i = 0; i < EDID_LENGTH; i++) {
        checksum += edid[i];
    }
    if (checksum != 0) {
        return(1);
    }
    
    // check EDID header signature
    if (edid[0] == 0x00) {
        checksum = 0;
        for (i = 0; i < sizeof(eb); i++) {
            if (edid[i] == eb_header[i])
                checksum++;
        }
        if (checksum != 8) {
            return(1);
        }
    }
    
    // check EDID version
    if (eb->version != 1 ||  eb->revision > 4 ) {
        return(1);
    }
    
    return(0);
}

char modearray[128][128];
int native;
int currentmode;

int parse_extb(BLContextPtr context,byte* extb) {
	int i, curloc;
	//char nativename[64];
	byte sum =0;
	printf("\n#Extension block found. Parsing...\n");
	/*
     for (i = 0; i < 128; i++)
     printf("byte %xh is 0x%x\n", i, extb[i]);
     */
	//printf("Tag: %x\n", extb[0]);
	
	for (i=0;i<128;i++) {
		sum +=extb[i];
	}
	if (sum != 0x00) {
		printf("Extension block checksum failed\n");
	}
    
	if (extb[0] != 0x02)
		printf("only know about extension blocks of type 02h... ABORT!\n");
	curloc = extb[2];
	if (curloc == 0) {
		printf("#No data in the extension block\n");
		return 0;
	}
    
	
	//printf("There are %i bytes of data block\n", curloc - 4);
	
	if (curloc > 4) {
		if ((extb[4] & 0xE0) != 0x40) { //if the first one is not a video one
			printf("have data blocks, but not video ones... weird\n");
		}
        
		for (i=0;i<(extb[4]&0x1F);i++) {
            sprintf(modearray[currentmode], "%s", ceamodes[extb[5+i]&0x7F]);
            if ((extb[5+i]&0x80) == 0x80 && native == -1) {
                native = currentmode;
            }
            currentmode++;
		}
        
	}
    
	//starting 18-byte DTD's.
    
    
	//Copypaste the DTD stuff from above.
	int hactive, vactive, pixclk, hsyncoff, hsyncwidth, hblank, vsyncoff, vsyncwidth, vblank;
	//Parse for Detailed Timing Descriptors...
	for (i = curloc; i < curloc+(18*4); i += 0x12) { //read through descriptor blocks...
		if ((extb[i] != 0x00) && (extb[i+1] != 0x00)) { //a dtd
			hactive = extb[i+2] + ((extb[i+4] & 0xf0) << 4);
			hblank = extb[i+3] + ((extb[i+4] & 0x0f) << 8);
			vactive = extb[i+5] + ((extb[i+7] & 0xf0) << 4);
			vblank = extb[i+6] + ((extb[i+7] & 0x0f) << 8);
            
			//printf("\tModeline \t\"%dx%d\" ", hactive, vactive);
			
			if ((i == curloc) && ((extb[3]&0x0F) > 0)) {
				native = currentmode;
				//sprintf(nativename, "%dx%d", hactive, vactive);
			}
            
			pixclk = (extb[i+1] << 8) | (extb[i]);
            
			sprintf(modearray[currentmode], "%.2f ", (double)pixclk / 100.0);
			
			//I'm using Fremlin's nomenclature...
			//sync offset = front porch
			//sync width = sync pulse width
            
			hsyncoff = extb[i+8] | ((extb[i+11] & 0xC0) << 2);
			hsyncwidth = extb[i+9] | ((extb[i+11] & 0x30) << 4);
			vsyncoff = ((extb[i+10] & 0xf0) >> 4) | ((extb[i+11] & 0x0C) << 2);
			vsyncwidth = (extb[i+10] & 0x0f) | ((extb[i+11] & 0x03) << 4);
            
            
			sprintf(modearray[currentmode], "%s%u %u %u %u ", modearray[currentmode], hactive, hactive+hsyncoff, hactive+hsyncoff+hsyncwidth, hactive+hblank);
			sprintf(modearray[currentmode], "%s%u %u %u %u ", modearray[currentmode], vactive, vactive+vsyncoff, vactive+vsyncoff+vsyncwidth, vactive+vblank);
            
			if ( (extb[i+17]&0x80) || ((extb[i+17]&0x18) == 0x18) ) {
				sprintf(modearray[currentmode], "%s%shsync %svsync %s", modearray[currentmode], ((extb[i+17]&0x10) && extb[i+17]&0x02) ? "+": "-", ((extb[i+17]&0x10) && extb[i+17]&0x04) ? "+": "-", (extb[i+17]&0x80) ? "interlace": "");
                //hehe... there's been at least 2 bugs in the old parse-edid the whole time - somebody caught the htimings one, and I just caught two problems right here - lack of checking for analog sync and getting hsync and vsync backwards... yes, vsync and hsync have been flipped this whole time. Glad I'm rewriting
                
			}
			//printf("\n");
			currentmode++;
		}
	}
    
    return 0;
    
}


int
parse_edid_productid( BLContextPtr context,byte* edid )
{
    struct edid_block *s_edid =(struct edid_block *)edid;
    byte checksum = 0;
    int ret = 0;
    
    checksum =  check_edid((uint8_t *)s_edid);
    
    if (  checksum != 0  ) {
        blesscontextprintf(context, kBLLogLevelError,  "EDID checksum failed - data is corrupt. Continuing anyway.\n");
        ret = 1;
    } else
        blesscontextprintf(context, kBLLogLevelVerbose,  "EDID checksum passed.\n" );
    
    
    if ( strncmp( edid+EDID_HEADER, edid_v1_header, EDID_HEADER_END+1 ) )
    {
        blesscontextprintf(context, kBLLogLevelError, "first bytes don't match EDID version 1 header\n" );
        blesscontextprintf(context, kBLLogLevelError, "do not trust output (if any).\n" );
        ret = 1;
    }
    
    blesscontextprintf(context, kBLLogLevelVerbose,  "\n\t# EDID version %d revision %d\n", (int)edid[EDID_STRUCT_VERSION],(int)edid[EDID_STRUCT_REVISION]  );
    
    printf("%02X\n", s_edid->productCode);
    
    return ret;
}

int
parse_edid_full( BLContextPtr context,byte* edid )
{
    unsigned i;
    byte* block;
    struct edid_block *s_edid =(struct edid_block *)edid;
    char* monitor_name = NULL;
    char monitor_alt_name[100];
    byte checksum = 0;
    char *vendor_sign;
    int ret = 0;
    
    checksum =  check_edid((uint8_t *)s_edid);
    //for( i = 0; i < EDID_LENGTH; i++ )
    //checksum += edid[ i ];
    
    if (  checksum != 0  ) {
        blesscontextprintf(context, kBLLogLevelError,  "EDID checksum failed - data is corrupt. Continuing anyway.\n");
        ret = 1;
    } else
        blesscontextprintf(context, kBLLogLevelNormal,  "EDID checksum passed.\n" );
    
    
    if ( strncmp( edid+EDID_HEADER, edid_v1_header, EDID_HEADER_END+1 ) )
    {
        blesscontextprintf(context, kBLLogLevelError, "first bytes don't match EDID version 1 header\n" );
        blesscontextprintf(context, kBLLogLevelError, "do not trust output (if any).\n" );
        ret = 1;
    }
    
    printf( "\n\t# EDID version %d revision %d\n", (int)edid[EDID_STRUCT_VERSION],(int)edid[EDID_STRUCT_REVISION] );
    
    vendor_sign = get_vendor_sign( edid + ID_MANUFACTURER_NAME );
    
    blesscontextprintf(context, kBLLogLevelVerbose, "Section \"Monitor\"\n" );
    
    block = edid + DETAILED_TIMING_DESCRIPTIONS_START;
    
    for( i = 0; i < NO_DETAILED_TIMING_DESCRIPTIONS; i++,
        block += DETAILED_TIMING_DESCRIPTION_SIZE )
    {
        
        if ( block_type( block ) == MONITOR_NAME )
        {
            monitor_name = get_monitor_name( block );
            break;
        }
    }
    
    if (!monitor_name) {
        /* Stupid djgpp hasn't snprintf so we have to hack something together */
        if(strlen(vendor_sign) + 10 > sizeof(monitor_alt_name))
            vendor_sign[3] = 0;
        
        sprintf(monitor_alt_name, "%s:%02x%02x",
                vendor_sign, edid[ID_MODEL], edid[ID_MODEL+1]) ;
        monitor_name = monitor_alt_name;
    }
    
    printf( "\tIdentifier \"%s\"\n", monitor_name );
    printf( "\tVendorName \"%s\"\n", vendor_sign );
    //printf( "\tModelName \"%s\"\n", monitor_name );
    printf("\tVendorId \"%02X\"\n", s_edid->manufacturerCode);
    printf("\tProductId \"%02X\"\n", s_edid->productCode);
    printf("\tManufactureWeek \"%d\"\n", s_edid->manufacturedWeek);
    printf("\tManufactureYear \"%d\"\n", s_edid->manufacturedYear + 1990);
    
    
    if (CHECK_BIT(s_edid->videoInputDefinition, 7))
        printf("\tSignalType \"DigitalSignal\"\n");
    else
        printf("\tSignalType \"AnalogSignal\"\n");
    printf("\tGamma \"%s\"\n", display_gamma((uint8_t *)s_edid));
    
    block = edid + DETAILED_TIMING_DESCRIPTIONS_START;
    
    for( i = 0; i < NO_DETAILED_TIMING_DESCRIPTIONS; i++,
        block += DETAILED_TIMING_DESCRIPTION_SIZE )
    {
        
        if ( block_type( block ) == MONITOR_LIMITS )
            parse_monitor_limits( block );
    }
    
    parse_dpms_capabilities(edid[DPMS_FLAGS]);
    
    block = edid + DETAILED_TIMING_DESCRIPTIONS_START;
    
    for( i = 0; i < NO_DETAILED_TIMING_DESCRIPTIONS; i++,
        block += DETAILED_TIMING_DESCRIPTION_SIZE )
    {
        
        if ( block_type( block ) == DETAILED_TIMING_BLOCK )
            parse_timing_description( block );
    }
    
    
    print_description_block(context,(uint8_t *)&(s_edid->descriptionBlock2), 2);
    print_description_block(context,(uint8_t *)&(s_edid->descriptionBlock3), 3);
    print_description_block(context,(uint8_t *)&(s_edid->descriptionBlock4), 4);
    
    blesscontextprintf(context, kBLLogLevelVerbose, "EndSection\n" );
    
    return ret;
}


int
parse_timing_description( byte* dtd )
{
    int htotal, vtotal;
    htotal = H_ACTIVE + H_BLANKING;
    vtotal = V_ACTIVE + V_BLANKING;
    
    printf( "\tMode \t\"%dx%d\"", H_ACTIVE, V_ACTIVE );
    printf( "\t# vfreq %3.3fHz, hfreq %6.3fkHz\n",
           (double)PIXEL_CLOCK/((double)vtotal*(double)htotal),
           (double)PIXEL_CLOCK/(double)(htotal*1000));
    
    printf( "\t\tDotClock\t%f\n", (double)PIXEL_CLOCK/1000000.0 );
    
    printf( "\t\tHTimings\t%u %u %u %u\n", H_ACTIVE,
           H_ACTIVE+H_SYNC_OFFSET,
           H_ACTIVE+H_SYNC_OFFSET+H_SYNC_WIDTH,
           htotal );
    
    printf( "\t\tVTimings\t%u %u %u %u\n", V_ACTIVE,
           V_ACTIVE+V_SYNC_OFFSET,
           V_ACTIVE+V_SYNC_OFFSET+V_SYNC_WIDTH,
           vtotal );
    
    if ( INTERLACED || (SYNC_TYPE == SYNC_SEPARATE)) {
        printf( "\t\tFlags\t%s\"%sHSync\" \"%sVSync\"\n",
               INTERLACED ? "\"Interlace\" ": "",
               HSYNC_POSITIVE ? "+": "-",
               VSYNC_POSITIVE ? "+": "-");
    }
    
    printf( "\tEndMode\n" );
    
    return 0;
}


int
block_type( byte* block )
{
    if ( !strncmp( edid_v1_descriptor_flag, block, 2 ) )
    {
        printf("\t# Block type: 2:%x 3:%x\n", block[2], block[3]);
        
        /* descriptor */
        
        if ( block[ 2 ] != 0 )
            return UNKNOWN_DESCRIPTOR;
        
        
        return block[ 3 ];
    } else {
        
        /* detailed timing block */
        
        return DETAILED_TIMING_BLOCK;
    }
}

char*
get_monitor_name( byte const* block )
{
    static char name[ 13 ];
    unsigned i;
    byte const* ptr = block + DESCRIPTOR_DATA;
    
    
    for( i = 0; i < 13; i++, ptr++ )
    {
        
        if ( *ptr == 0xa )
        {
            name[ i ] = 0;
            return name;
        }
        
        name[ i ] = *ptr;
    }
    
    
    return name;
}


char* get_vendor_sign( byte const* block )
{
    static char sign[4];
    unsigned short h;
    
    /*
     08h	WORD	big-endian manufacturer ID (see #00136)
     bits 14-10: first letter (01h='A', 02h='B', etc.)
     bits 9-5: second letter
     bits 4-0: third letter
     */
    h = COMBINE_HI_8LO(block[0], block[1]);
    sign[0] = ((h>>10) & 0x1f) + 'A' - 1;
    sign[1] = ((h>>5) & 0x1f) + 'A' - 1;
    sign[2] = (h & 0x1f) + 'A' - 1;
    sign[3] = 0;
    return sign;
}

int
parse_monitor_limits( byte* block )
{
    printf( "\tHorizSync %u-%u\n", H_MIN_RATE, H_MAX_RATE );
    printf( "\tVertRefresh %u-%u\n", V_MIN_RATE, V_MAX_RATE );
    if ( MAX_PIXEL_CLOCK == 10*0xff )
        printf( "\t# Max dot clock not given\n" );
    else
        printf( "\t# Max dot clock (video bandwidth) %u MHz\n", (int)MAX_PIXEL_CLOCK );
    
    if ( GTF_SUPPORT )
    {
        printf( "\t# EDID version 3 GTF given: contact author\n" );
    }
    
    return 0;
}

int
parse_dpms_capabilities(byte flags)
{
    printf("\t# DPMS capabilities: Active off:%s  Suspend:%s  Standby:%s\n\n",
           (flags & DPMS_ACTIVE_OFF) ? "yes" : "no",
           (flags & DPMS_SUSPEND)    ? "yes" : "no",
           (flags & DPMS_STANDBY)    ? "yes" : "no");
    return 0;
}

