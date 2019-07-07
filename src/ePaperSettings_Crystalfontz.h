//     ePaper Driver Lib for Arduino Project
//     Copyright (C) 2019 Michael Kamprath
//
//     This file is part of Crystalfontz ePaper Display Lib for Arduino Project.
// 
//     ePaper Driver Lib for Arduino Project is free software: you can 
//	   redistribute it and/or modify it under the terms of the GNU General Public License
//     as published by the Free Software Foundation, either version 3 of the License, or
//     (at your option) any later version.
// 
//     ePaper Driver Lib for Arduino Project is distributed in the hope that 
// 	   it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
//     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//     GNU General Public License for more details.
// 
//     You should have received a copy of the GNU General Public License
//     along with Shift Register LED Matrix Project.  If not, see <http://www.gnu.org/licenses/>.
//
//     This project and its creators are not associated with Crystalfontz, Good display
//	   or any other manufacturer, nor is this  project officially endorsed or reviewed for 
//	   correctness by any ePaper manufacturer. 
//
#ifndef __ePaperSettings_Crystalfontz__
#define __ePaperSettings_Crystalfontz__
// This file contains all the device specific look up tables and configurations for the 
// ePaper devices this library supports. 

// the configuration is set up as a byte stream configuration bytes, commands, and data.
// The first byte read in a stream indicates whether the following bytes are a command or 
// data. If the byte is 0, then the next single byte is a command. If the byte is greater
// than 0 but less than 255 (0xFF), then it indicates the number of following bytes that 
// should be sent as data. If the byte is 255 (OxFF), then this library will wait until
// the ePaper device is ready, as indicated by its BUSY pin going high. This
// byte sequence can then be repeated to construct longer command and data sequences. 
// 
// Given that the data size indicated is an uint8_t and that 0xFF is reserved, any given data 
// block can only be 254 bytes in size. if more need to be sent, then just define multiple 
// consecutive  data blocks.

//
// MODEL CFAP176264A0-0270 - 2.7 inch 3-color ePaper display
//
const uint8_t deviceConfiguration_CFAP176264A0_0270[] PROGMEM = 
{
	// panel setting command
	0,	0x01,	
	5,	0x03,	0x00,	0x26,	0x26,	0x03,
	
	// power on command. Always "wait for ready" after this sequence
	0,	0x04,
	0xFF,		// wait for ready

	// booster soft start
	0,	0x06,
	3,	0x07,	0x07,	0x07,
	
	// power Optimization
	0,	0xF8,
	2,	0x60,	0xA5,
	0,	0xF8,
	2,	0x89,	0xA5,
	0,	0xF8,
	2,	0x90,	0x00,
	0,	0xF8,
	2,	0x93,	0x2A,	
	
	// this command resets the DFV_EN by sending a partial refresh command
	0,	0x16,
	1,	0x00,
	
	// PLL Control
	0,	0x30,
	1,	0x3A,

	//VCM DS Settings
	0,	0x82,
	1,	0x12,
	0,	0x50,
	1,	0x87,
	
	// set panel setting to call LUTs from OTP
	0,	0x00,
	1,	0xCF,	// 11001111
	
	// set the device resolution to 176x264
	0,	0x61,
	4,	0x00,	0xB0,	0x01,	0x08
};
const uint8_t deviceConfigurationSize_CFAP176264A0_0270 PROGMEM = 64;


#endif // __ePaperSettings_Crystalfontz__
