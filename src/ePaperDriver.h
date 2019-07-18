//     ePaper Driver Lib for Arduino Project
//     Copyright (C) 2019 Michael Kamprath
//
//     This file is part of ePaper Driver Lib for Arduino Project.
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
#ifndef __ePaperDriver__
#define __ePaperDriver__
#include <Adafruit_GFX.h>
#include "ePaperDeviceModels.h"

// these are the color values supported
#define ePaper_WHITE	0
#define ePaper_BLACK	1
#define ePaper_COLOR	2		// used if the ePaper display ahs a third color
#define ePaper_INVERSE1	3		// b -> w, w -> b, c -> w
#define ePaper_INVERSE2	4		// b -> c or w, w -> b, c -> b
#define ePaper_INVERSE3	5		// b -> w, w -> c or b, c -> b


class ePaperDisplay : public Adafruit_GFX {
public:

	
private:
	const ePaperDeviceModel _model;
	const int _deviceReadyPin;
	const int _deviceResetPin;
	const int _deviceDataCommandPin;
	const int _deviceSelectPin;
	
	const uint8_t *_configuration;
	const uint8_t _configurationSize;

	uint16_t _bufferSize;
	uint8_t *_blackBuffer;
	uint8_t *_colorBuffer;
	
	void waitForReady(void) const;
	void resetDriver(void) const;
	
protected:
	void sendCommand( uint8_t cmd ) const;
	void sendData( const uint8_t *dataArray, uint16_t arraySize, bool isProgMem, bool invertBits = false ) const;
	void sendCommandAndDataSequenceFromProgMem( const uint8_t *dataArray, uint16_t arraySize) const;


public:

	ePaperDisplay(
		ePaperDeviceModel model,
		int deviceReadyPin,
		int deviceResetPin,
		int deviceDataCommandPin,
		int deviceSelectPin
	);
	
	virtual ~ePaperDisplay();

	void initializeDevice(void) const;
	void powerOn(void) const;
	void powerOff(void) const;
	
	ePaperDeviceModel model(void) const			{ return _model; }

	//
	// Adafruit GFX support
	//
	
	virtual void drawPixel(int16_t x, int16_t y, uint16_t color);
	
	virtual void fillScreen(uint16_t color);
	virtual void invertDisplay(boolean i);
	
	//
	//
	//
	
	void refreshDisplay(void);
	void clearDisplay(void);
	
	void setDeviceImage( 
				const uint8_t* blackBitMap,
				uint16_t blackBitMapSize,
				bool blackBitMapIsProgMem
			);

	void setDeviceImage( 
				const uint8_t* blackBitMap,
				uint16_t blackBitMapSize,
				bool blackBitMapIsProgMem,
				const uint8_t* colorBitMap,
				uint16_t colorBitMapSize,
				bool colorBitMapIsProgMem
			);
		
};

#endif // __ePaperDriver__
