Project readEDID
=====

readEDID is originally from the 10-years-old idea “ReadEDID” to get the LCD fresh rate, but it has been totally rewritten for modern OSX, and more functions.

Histroy
-
---------
version 1.0.2

change CMAKE_OSX_SYSROOT to macosx10.9 for brew

version 1.0.1

add support with `Homebrew`

version 1.0.0

update with initial source.

first commit with the original Year2003 source. 

Requirement
-
---------

Mac OS X 10.6 and above version (10.10beta tested)

Build+Install
-
---------

Just fetch the code via git clone github.com:othercat/readEDID
Then compile it with Xcode.

I've also put the binary file that had been compiled from my Macbook Pro if someone does not want to compile the source.

Running
-
---------
There are three functions that this program currently supports: getFullData, getProductId 
and getManufactureDate.  All these functions operate on active displays [1].

Example 1:
   
   $ readEDID
    
   Result :

```
00 FF FF FF FF FF FF 00 06 10 A4 9C 00 00 00 00
16 13 01 03 80 21 15 78 0A E5 85 A3 54 4F 9C 26
0E 50 54 00 00 00 01 01 01 01 01 01 01 01 01 01
01 01 01 01 01 01 AB 22 A0 A0 50 84 1A 30 30 20
36 00 4B CF 10 00 00 19 00 00 00 01 00 06 10 30
00 00 00 00 00 00 00 00 0A 20 00 00 00 FE 00 4C
54 4E 31 35 34 42 54 30 38 00 0A 20 00 00 00 FC
00 43 6F 6C 6F 72 20 4C 43 44 0A 20 20 20 00 AC
EDID checksum passed.

	# EDID version 1 revision 3
	# Block type: 2:0 3:1
	# Block type: 2:0 3:fe
	# Block type: 2:0 3:fc
	Identifier "Color LCD"
	VendorName "APP"
	VendorId "1006"
	ProductId "9CA4"
	ManufactureWeek "22"
	ManufactureYear "2009"
	SignalType "DigitalSignal"
	Gamma "2.200"
	# Block type: 2:0 3:1
	# Block type: 2:0 3:fe
	# Block type: 2:0 3:fc
	# DPMS capabilities: Active off:no  Suspend:no  Standby:no

	Mode 	"1440x900"	# vfreq 59.901Hz, hfreq 55.469kHz
		DotClock	88.750000
		HTimings	1440 1488 1520 1600
		VTimings	900 903 909 926
		Flags	"-HSync" "-VSync"
	EndMode
	# Block type: 2:0 3:1
	# Block type: 2:0 3:fe
	# Block type: 2:0 3:fc
       Comment "LTN154BT08"
       ModelNumber "Color LCD"
       
 ```

[1]See discussion point for explanation of what active display means.
http://developer.apple.com/library/mac/documentation/GraphicsImaging/Reference/Quartz_Services_Ref/Reference/reference.html#//apple_ref/c/func/CGGetActiveDisplayList

About
-
---------

twitter: [@othercatlee](https://twitter.com/othercatlee/)

email: <othercat@gmail.com>

Licence
-
---------

   readEDID uses GPLv2 License. 
   
   Copyright (c) 2012-2013, Richard Li
   
   All rights reserved.

Documentation
-
---------

original Year2003 source: <http://homepage.mac.com/arekkusu/bugs/ReadEDID.dmg>
