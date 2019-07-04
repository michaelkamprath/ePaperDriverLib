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
#ifndef __Crystalfontz_ePaper__
#define __Crystalfontz_ePaper__

class ePaperDisplay {
public:
	typedef enum {
		// model: CFAP176264A0-0270 - 2.7 inch 3-color ePaper display
		CFAP176264A0_0270
	
	} DEVICE_MODEL;

private:
	const DEVICE_MODEL _model;
	const int _deviceReadyPin;
	const int _deviceResetPin;
	const int _deviceDataCommandPin;
	const int _deviceSelectPin;
	const int _deviceSizeVertical;
	const int _deviceSizeHorizontal;
	
	const uint8_t *_powerOnSettings;
	const uint8_t _powerOnSettingsSize;
	const uint8_t *_configuration;
	const uint8_t _configurationSize;

	void waitForReady(void) const;
	void resetDriver(void) const;

	static const uint8_t* powerOnSettings(ePaperDisplay::DEVICE_MODEL model);
	static uint8_t powerOnSettingsSize(ePaperDisplay::DEVICE_MODEL model);
	static const uint8_t* deviceConfiguration(ePaperDisplay::DEVICE_MODEL model);
	static uint8_t deviceConfigurationSize(ePaperDisplay::DEVICE_MODEL model);
	static int deviceSizeVertical(ePaperDisplay::DEVICE_MODEL model);
	static int deviceSizeHorizontal(ePaperDisplay::DEVICE_MODEL model);
	
protected:
	void sendCommand( uint8_t cmd ) const;
	void sendData( const uint8_t *dataArray, uint16_t arraySize, bool isProgMem ) const;
	void sendCommandAndDataSequenceFromProgMem( const uint8_t *dataArray, uint16_t arraySize) const;


public:

	ePaperDisplay(
		ePaperDisplay::DEVICE_MODEL model,
		int deviceReadyPin,
		int deviceResetPin,
		int deviceDataCommandPin,
		int deviceSelectPin
	);
	
	virtual ~ePaperDisplay();

	void powerUpDevice(void) const;

	ePaperDisplay::DEVICE_MODEL model(void) const			{ return _model; }

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
	
	void blankDeviceWhite(void);
	void blankDeviceBlack(void);
	void blankDeviceColor(void);
	
};

#endif // __Crystalfontz_ePaper__
