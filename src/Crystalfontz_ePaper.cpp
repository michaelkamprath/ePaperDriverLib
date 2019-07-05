//     Crystalfontz ePaper Display Lib for Arduino Project
//     Copyright (C) 2019 Michael Kamprath
//
//     This file is part of Crystalfontz ePaper Display Lib for Arduino Project.
// 
//     Crystalfontz ePaper Display Lib for Arduino Project is free software: you can 
//	   redistribute it and/or modify it under the terms of the GNU General Public License
//     as published by the Free Software Foundation, either version 3 of the License, or
//     (at your option) any later version.
// 
//     Crystalfontz ePaper Display Lib for Arduino Project is distributed in the hope that 
// 	   it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
//     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//     GNU General Public License for more details.
// 
//     You should have received a copy of the GNU General Public License
//     along with Shift Register LED Matrix Project.  If not, see <http://www.gnu.org/licenses/>.
//
//     This project and its creators are not associated with Crystalfontz, nor is this 
//     project officially endorsed or reviewed for correctness by Crystalfontz. This 
//     project is an open source effort by the community to make a usable library for the
//     Crystalfontz line of ePaper Displays.
//

#include <Arduino.h>
#include <SPI.h>
#include "Crystalfontz_ePaper.h"
#include "Crystalfontz_ePaper_Settings.h"

#ifdef DEBUG
#define DEBUG_PRINTLN(s) Serial.println(s)
#define DEBUG_PRINT(s) Serial.print(s)
#define DEBUG_PRINTFORMAT(s,f) Serial.print(s,f)
#else
#define DEBUG_PRINTLN(s)
#define DEBUG_PRINT(s)
#define DEBUG_PRINTFORMAT(s,f)
#endif

const uint8_t* ePaperDisplay::deviceConfiguration(ePaperDisplay::DEVICE_MODEL model)
{
	switch (model) {
		case CFAP176264A0_0270:
			return deviceConfiguration_CFAP176264A0_0270;
			break;
		default:
			return 0;
			break;
	}
}

uint8_t ePaperDisplay::deviceConfigurationSize(ePaperDisplay::DEVICE_MODEL model)
{
	switch (model) {
		case CFAP176264A0_0270:
			return pgm_read_byte(&deviceConfigurationSize_CFAP176264A0_0270);
			break;
		default:
			return 0;
			break;
	}
}

int ePaperDisplay::deviceSizeVertical(ePaperDisplay::DEVICE_MODEL model)
{
	switch (model) {
		case CFAP176264A0_0270:
			return 264;
			break;
		default:
			return 0;
			break;
	}
}

int ePaperDisplay::deviceSizeHorizontal(ePaperDisplay::DEVICE_MODEL model)
{
	switch (model) {
		case CFAP176264A0_0270:
			return 176;
			break;
		default:
			return 0;
			break;
	}
}
ePaperDisplay::ePaperDisplay(
		ePaperDisplay::DEVICE_MODEL model,
		int deviceReadyPin,
		int deviceResetPin,
		int deviceDataCommandPin,
		int deviceSelectPin
	) :	_model( model ),
		_deviceReadyPin( deviceReadyPin ),
		_deviceResetPin( deviceResetPin ),
		_deviceDataCommandPin( deviceDataCommandPin ),
		_deviceSelectPin( deviceSelectPin ),
		_deviceSizeVertical(ePaperDisplay::deviceSizeVertical(model)),
		_deviceSizeHorizontal(ePaperDisplay::deviceSizeHorizontal(model)),
		_configuration(ePaperDisplay::deviceConfiguration(model)),
		_configurationSize(ePaperDisplay::deviceConfigurationSize(model))
{
	pinMode(_deviceSelectPin, OUTPUT);
	pinMode(_deviceResetPin, OUTPUT);
	pinMode(_deviceDataCommandPin, OUTPUT);
	pinMode(_deviceReadyPin, INPUT);
	
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
	DEBUG_PRINTLN("Sending data to device:");
	digitalWrite(_deviceDataCommandPin, HIGH);
	DEBUG_PRINT("    0x");
	for (uint16_t i = 0; i < arraySize; i++ ) {
		digitalWrite(_deviceSelectPin, LOW);
		uint8_t data;
		if (isProgMem) {
			data = pgm_read_byte(&dataArray[i]);
		} else {
			data = dataArray[i];
		}
		SPI.transfer(data);
		if ( i < 32 ) {
			DEBUG_PRINTFORMAT(data, HEX);
			if (i < arraySize-1 && i < 32) {
				DEBUG_PRINT(", 0x");
			}
		}
		digitalWrite(_deviceSelectPin, HIGH);
		yield();
	}
	if ( arraySize >= 32 ) {
		DEBUG_PRINT(" ..... ");
	}
	DEBUG_PRINT("\n");
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

void ePaperDisplay::powerUpDevice(void) const
{
	DEBUG_PRINTLN("powering up device");
	DEBUG_PRINTLN("resetting driver");
	resetDriver();
	DEBUG_PRINTLN("sending configuration");
	sendCommandAndDataSequenceFromProgMem(_configuration, _configurationSize);	
	DEBUG_PRINTLN("done setting up device.\n");
}

void ePaperDisplay::setDeviceImage( 
	const uint8_t* blackBitMap,
	uint16_t blackBitMapSize,
	bool blackBitMapIsProgMem
)
{
	setDeviceImage(blackBitMap, blackBitMapSize, blackBitMapIsProgMem, nullptr, 0, false);
}

void ePaperDisplay::setDeviceImage( 
	const uint8_t* blackBitMap,
	uint16_t blackBitMapSize,
	bool blackBitMapIsProgMem,
	const uint8_t* colorBitMap,
	uint16_t colorBitMapSize,
	bool colorBitMapIsProgMem
)
{
	DEBUG_PRINTLN("\nsending new bitmap");
	if ( blackBitMap != nullptr ) {
		sendCommand(0x10);
		sendData(blackBitMap, blackBitMapSize, blackBitMapIsProgMem);
		yield();
	}
	if ( colorBitMap != nullptr ) {
		sendCommand(0x13);
		sendData(colorBitMap, colorBitMapSize, colorBitMapIsProgMem);
		yield();
	}
	
	sendCommand(0x12);
	waitForReady();
	DEBUG_PRINTLN("bitmap has been sent!");
}

void ePaperDisplay::blankDeviceWhite(void)
{
	int totalBytes = width()*height()/8;
	uint8_t offData = 0x00;
	
	sendCommand(0x10);
	for (int i = 0; i < totalBytes; i++ ) {
		sendData(&offData, 1, false);
		yield();
	}
	sendCommand(0x13);
	for (int i = 0; i < totalBytes; i++ ) {
		sendData(&offData, 1, false);
		yield();
	}
	sendCommand(0x12);
	waitForReady();
}

void ePaperDisplay::blankDeviceBlack(void) {
	int totalBytes = width()*height()/8;
	uint8_t offData = 0x00;
	uint8_t onData = 0xFF;
	
	sendCommand(0x10);
	for (int i = 0; i < totalBytes; i++ ) {
		sendData(&onData, 1, false);
		yield();
	}
	sendCommand(0x13);
	for (int i = 0; i < totalBytes; i++ ) {
		sendData(&offData, 1, false);
		yield();
	}
	sendCommand(0x12);
	waitForReady();
}

void ePaperDisplay::blankDeviceColor(void) {
	int totalBytes = width()*height()/8;
	uint8_t offData = 0x00;
	uint8_t onData = 0xFF;
	
	sendCommand(0x10);
	for (int i = 0; i < totalBytes; i++ ) {
		sendData(&offData, 1, false);
		yield();
	}
	sendCommand(0x13);
	for (int i = 0; i < totalBytes; i++ ) {
		sendData(&onData, 1, false);
		yield();
	}
	sendCommand(0x12);
	waitForReady();
}

