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

//
// Some elements of this file were heavily inspired by Adafruit's of their driver for their
// SSD1306 monochrome device:
//		https://github.com/adafruit/Adafruit_SSD1306
// 

#include <Arduino.h>
#include <SPI.h>
#include "ePaperDriver.h"
#include "ePaperDeviceConfigurations.h"

#define DEBUG 0

#if DEBUG
#define DEBUG_PRINTLN(s) Serial.println(s)
#define DEBUG_PRINT(s) Serial.print(s)
#define DEBUG_PRINTFORMAT(s,f) Serial.print(s,f)
#else
#define DEBUG_PRINTLN(s)
#define DEBUG_PRINT(s)
#define DEBUG_PRINTFORMAT(s,f)
#endif

#define swap_coordinates(a, b) \
  (((a) ^= (b)), ((b) ^= (a)), ((a) ^= (b))) ///< No-temp-var swap operation


ePaperDisplay::ePaperDisplay(
		ePaperDeviceModel model,
		int deviceReadyPin,
		int deviceResetPin,
		int deviceDataCommandPin,
		int deviceSelectPin
	) :	Adafruit_GFX(
				ePaperDeviceConfigurations::deviceSizeHorizontal(model),
				ePaperDeviceConfigurations::deviceSizeVertical(model)
			),
		_model( model ),
		_deviceReadyPin( deviceReadyPin ),
		_deviceResetPin( deviceResetPin ),
		_deviceDataCommandPin( deviceDataCommandPin ),
		_deviceSelectPin( deviceSelectPin ),
		_configuration(ePaperDeviceConfigurations::deviceConfiguration(model)),
		_configurationSize(ePaperDeviceConfigurations::deviceConfigurationSize(model)),
		_bufferSize(0),
		_blackBuffer(nullptr),
		_colorBuffer(nullptr)
{
	pinMode(_deviceSelectPin, OUTPUT);
	pinMode(_deviceResetPin, OUTPUT);
	pinMode(_deviceDataCommandPin, OUTPUT);
	pinMode(_deviceReadyPin, INPUT);
	
	_bufferSize = width()*((height()+7)/8);
	_blackBuffer = (uint8_t *)malloc(_bufferSize);
	if (ePaperDeviceConfigurations::deviceHasThirdColor(model)) {
		_colorBuffer = (uint8_t *)malloc(_bufferSize);
	}
	
	
	SPI.beginTransaction(SPISettings(2000000, MSBFIRST, SPI_MODE0));
	SPI.begin();
	
	DEBUG_PRINTLN("ePaperDisplay object constructed");
	DEBUG_PRINT("_deviceReadyPin = ");
	DEBUG_PRINT(_deviceReadyPin);	
	DEBUG_PRINT(", _deviceResetPin = ");
	DEBUG_PRINT(_deviceResetPin);	
	DEBUG_PRINT(", _deviceDataCommandPin = ");
	DEBUG_PRINT(_deviceDataCommandPin);	
	DEBUG_PRINT(", _deviceSelectPin = ");
	DEBUG_PRINT(_deviceSelectPin);	
	DEBUG_PRINT("\n\n");	
}

ePaperDisplay::~ePaperDisplay()
{
}

void ePaperDisplay::waitForReady(void) const
{
	DEBUG_PRINT("Waiting until epaper device is complete .");
	while (0 == digitalRead(_deviceReadyPin)) {
		yield();
		DEBUG_PRINT(".");
	}
	DEBUG_PRINT("  Done!\n");
}

void ePaperDisplay::resetDriver(void) const
{
	DEBUG_PRINTLN("reset driver");
	digitalWrite(_deviceResetPin, LOW);
	delay(200);
	digitalWrite(_deviceResetPin, HIGH);
	delay(200);
}

void ePaperDisplay::sendCommand( uint8_t cmd ) const
{
	DEBUG_PRINT("Sending command to device: 0x");
	DEBUG_PRINTFORMAT(cmd, HEX);
	DEBUG_PRINT("\n");
	digitalWrite(_deviceDataCommandPin, LOW);
	digitalWrite(_deviceSelectPin, LOW);
	SPI.transfer(cmd);
	digitalWrite(_deviceSelectPin, HIGH);
}

void ePaperDisplay::sendData( const uint8_t *dataArray, uint16_t arraySize, bool isProgMem ) const
{
	DEBUG_PRINTLN("Sending data to device...");
	digitalWrite(_deviceDataCommandPin, HIGH);
	for (uint16_t i = 0; i < arraySize; i++ ) {
		digitalWrite(_deviceSelectPin, LOW);
		uint8_t data;
		if (isProgMem) {
			data = pgm_read_byte(&dataArray[i]);
		} else {
			data = dataArray[i];
		}
		SPI.transfer(data);
		digitalWrite(_deviceSelectPin, HIGH);
		yield();
	}
	DEBUG_PRINTLN("    Done sending data to device.");
}

void ePaperDisplay::sendCommandAndDataSequenceFromProgMem( const uint8_t *dataArray, uint16_t arraySize) const
{
	uint16_t index = 0;
	
	while (index < arraySize) {
		// read byte
		uint8_t b = pgm_read_byte(&dataArray[index]);
		if (b == 0x00) {
			// send next byte as command
			index++;
			sendCommand(pgm_read_byte(&dataArray[index]));
			index++;
		} else if (b == 0xFF ) {
			waitForReady();
			index++;
		} else {
			// b is and array length. send the next b bytes as dataArray
			index++;
			sendData(&dataArray[index], b, true);
			index += b;
		}
	}

}

void ePaperDisplay::initializeDevice(void) const
{
	DEBUG_PRINTLN("powering up device");
	DEBUG_PRINTLN("resetting driver");
	resetDriver();
	DEBUG_PRINTLN("sending configuration");
	sendCommandAndDataSequenceFromProgMem(_configuration, _configurationSize);	
	DEBUG_PRINTLN("done setting up device.\n");
}

void ePaperDisplay::powerOn(void) const
{
	sendCommand(0x04);
	waitForReady();
}

void ePaperDisplay::powerOff(void) const
{
	uint8_t data = 0x00;
	
	sendCommand(0x02);
	sendCommand(0x03);
	sendData(&data, 1, false);
}


/*!
    @brief  Clear contents of display buffer (set all pixels to off).
    @return None (void).
    @note   Changes buffer contents only, no immediate effect on display.
            Follow up with a call to display(). Has the effect of setting
            the screen ePaper_WHITE.
*/
void ePaperDisplay::clearDisplay(void)
{
	fillScreen(ePaper_WHITE);
}
void ePaperDisplay::drawPixel(int16_t x, int16_t y, uint16_t color)
{
	if((x >= 0) && (x < width()) && (y >= 0) && (y < height())) {
		// Pixel is in-bounds. Rotate coordinates if needed.
		switch(getRotation()) {
			case 1:
				swap_coordinates(x, y);
				x = WIDTH - x - 1;
				break;
			case 2:
				x = WIDTH  - x - 1;
				y = HEIGHT - y - 1;
				break;
			case 3:
				swap_coordinates(x, y);
				y = HEIGHT - y - 1;
				break;
		}
		int16_t buffer_index = (y*WIDTH + x)/8;
		int8_t buffer_bit_mask = (1 << (7-(y*WIDTH + x)&7));
		
		DEBUG_PRINT(F("Setting pixel ( x = "));
		DEBUG_PRINT(x);
		DEBUG_PRINT(F(", y = "));
		DEBUG_PRINT(y);
		DEBUG_PRINT(F(" ), buffer_index = "));
		DEBUG_PRINT(buffer_index);
		DEBUG_PRINT(F(", buffer_bit_mask = 0x"));
		DEBUG_PRINTFORMAT(buffer_bit_mask, HEX);
		DEBUG_PRINT(F("\n"));
		
		switch(color) {
			case ePaper_WHITE:
				// setting white is turning the black pixel off
				_blackBuffer[buffer_index] &= ~(buffer_bit_mask); 
				if (_colorBuffer) {
					// turn the color off where we set white
					_colorBuffer[buffer_index] &= ~(buffer_bit_mask); 
				}
				
				break;
			case ePaper_BLACK:
				// turn appropriate black pixel on
				_blackBuffer[buffer_index] |=  (buffer_bit_mask);
				if (_colorBuffer) {
					// turn the color off where we set black
					_colorBuffer[buffer_index] &= ~(buffer_bit_mask); 
				}
				break;
			case ePaper_COLOR:
				// make sure black pixel is off
				_blackBuffer[buffer_index] &= ~(buffer_bit_mask); 
				if (_colorBuffer) {
					// turn the color on 
					_colorBuffer[buffer_index] |=  (buffer_bit_mask);
				}
				break;
			case ePaper_INVERSE1:
				// b -> w, w -> b, c -> w
				// to set the inverse, first see if the pixel has the color. If it does,
				// invert that (set to white). Otherwise, invert the B&W image.
				if (_colorBuffer) {
					if (_colorBuffer[buffer_index]&buffer_bit_mask) {
						_colorBuffer[buffer_index] &= ~(buffer_bit_mask); 
						break;
					}
				}
				_blackBuffer[buffer_index] ^= buffer_bit_mask;
				break;
			case ePaper_INVERSE2:
				// b -> c or w, w -> b, c -> b
				if (_colorBuffer) {
					if (_colorBuffer[buffer_index]&buffer_bit_mask) {
						_colorBuffer[buffer_index] &= ~(buffer_bit_mask); 
						_blackBuffer[buffer_index] |= buffer_bit_mask;
						break;
					}
				}
				if (_blackBuffer[buffer_index]&buffer_bit_mask) {
					_blackBuffer[buffer_index] &= ~(buffer_bit_mask); 
					if (_colorBuffer) {
						_colorBuffer[buffer_index] |= buffer_bit_mask;
					}
				}
				else {
					_blackBuffer[buffer_index] |= buffer_bit_mask;
				}
				break;
			case ePaper_INVERSE3:
				// b -> w, w -> c or b, c -> b
				if (_colorBuffer) {
					if (_colorBuffer[buffer_index]&buffer_bit_mask) {
						_colorBuffer[buffer_index] &= ~(buffer_bit_mask); 
						_blackBuffer[buffer_index] |= buffer_bit_mask;
						break;
					}
				}
				if (_blackBuffer[buffer_index]&buffer_bit_mask) {
					_blackBuffer[buffer_index] &= ~(buffer_bit_mask); 
				}
				else {
					if (_colorBuffer) {
						_colorBuffer[buffer_index] |= buffer_bit_mask;
					}
					else {
						_blackBuffer[buffer_index] |= buffer_bit_mask;
					}
				}
				break;
			default:
				// what color is this?
				break;			
		}
	}
	yield();
}

void ePaperDisplay::fillScreen(uint16_t color)
{
	uint8_t blackByte = 0;
	uint8_t colorByte = 0;

	switch (color) {
		case ePaper_WHITE:
			break;
		case ePaper_BLACK:
			blackByte = 0xFF;
			break;
		case ePaper_COLOR:
			colorByte = 0xFF;
			break;
		default:
		case ePaper_INVERSE1:
		case ePaper_INVERSE2:
		case ePaper_INVERSE3:
			Adafruit_GFX::fillScreen(color);
			return;
			break;
	}

	startWrite();
	if (_blackBuffer) {
		memset(_blackBuffer, blackByte, _bufferSize);
	}
	if (_colorBuffer) {
		memset(_colorBuffer, colorByte, _bufferSize);
	}
	endWrite();
}

/*!
    @brief      Invert the display. For B&W displays, this just swaps white and black.
    			For three color displays, this swaps black pixels with color pixels & vice versa.
    			Note that white pixels remain the same in this case.
    @param   i  True if you want to invert, false to make 'normal'
*/
void  ePaperDisplay::invertDisplay(boolean i)
{
	if (!i) return;
	
	if (_colorBuffer != nullptr) {
		startWrite();
		uint8_t *tempPtr = _colorBuffer;
		_colorBuffer = _blackBuffer;
		_blackBuffer = tempPtr;
		endWrite();
	} else {
		fillScreen(ePaper_INVERSE1);
	}
}


/*!
    @brief  Pushes the current image buffer contents to the ePaper device.
    @return None (void).
    @note   Pushes the current buffer contents to the ePaper device, and then triggers
    		a display refresh. This function does not return until the display refresh 
    		has completed.
*/
void ePaperDisplay::refreshDisplay(void)
{
	if (_blackBuffer != nullptr) {
		sendCommand(0x10);
		sendData(_blackBuffer, _bufferSize, false);
		yield();
	}
	if (_colorBuffer != nullptr) {
		sendCommand(0x13);
		sendData(_colorBuffer, _bufferSize, false);
		yield();
	}
	sendCommand(0x11);
	sendCommand(0x12);
	waitForReady();
}

/*!
    @brief  Sets the device image buffer directly. 
    @param	blackBitMap		the bitmap to set the bit and white image to. High (1) values set
    						the pixel black, low (0) sets the pixel white. 
    @param	blackBitMapSize	the size of the black bitmap in total bytes.
    @param	blackBitMapIsProgMem	indicates whether the passed bitmap resides in PROGMEM or not.
    @return None (void).
    @note   Since this sets the image buffer directly, the bit map must be correctly sized
    		for the device's demensions. Also, any rotation that has been set is ignored. 
*/
void ePaperDisplay::setDeviceImage( 
	const uint8_t* blackBitMap,
	uint16_t blackBitMapSize,
	bool blackBitMapIsProgMem
)
{
	setDeviceImage(blackBitMap, blackBitMapSize, blackBitMapIsProgMem, nullptr, 0, false);
}

/*!
    @brief  Sets the device image buffers directly. 
    @param	blackBitMap		the bitmap to set the black and white image to. High (1) values set
    						the pixel black, low (0) sets the pixel white. may be null.
    @param	blackBitMapSize	the size of the black bitmap in total bytes.
    @param	blackBitMapIsProgMem	indicates whether the passed bitmap resides in PROGMEM or not.
    @param	colorBitMap		the bitmap to set the color image to. High (1) values set
    						the pixel colored, low (0) sets the uncolored. 
    @param	colorBitMapSize	the size of the color bitmap in total bytes.
    @param	colorBitMapIsProgMem	indicates whether the passed bitmap resides in PROGMEM or not.
    @return None (void).
    @note   Since this sets the image buffers directly, the bit map must be correctly sized
    		for the device's demensions. Also, any rotation that has been set is ignored. 
*/
void ePaperDisplay::setDeviceImage( 
	const uint8_t* blackBitMap,
	uint16_t blackBitMapSize,
	bool blackBitMapIsProgMem,
	const uint8_t* colorBitMap,
	uint16_t colorBitMapSize,
	bool colorBitMapIsProgMem
)
{
	if (blackBitMap && _blackBuffer && (blackBitMapSize <= _bufferSize)) {
		if (blackBitMapIsProgMem) {
			memcpy_P(_blackBuffer, blackBitMap, blackBitMapSize);
		} else {
			memcpy(_blackBuffer, blackBitMap, blackBitMapSize);
		}
	}
	if (colorBitMap && _colorBuffer && (colorBitMapSize <= _bufferSize)) {
		if (blackBitMapIsProgMem) {
			memcpy_P(_colorBuffer, colorBitMap, colorBitMapSize);
		} else {
			memcpy(_colorBuffer, colorBitMap, colorBitMapSize);
		}
	}
}

