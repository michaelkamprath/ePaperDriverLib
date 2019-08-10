#ifndef __ePaperSettings_GoodDisplay__
#define __ePaperSettings_GoodDisplay__

//
// Model GDEW026Z39 - 2.6 Inch 3-color ePaper display
//

const uint8_t deviceConfiguration_GDEW026Z39[] PROGMEM = 
{
	// booster soft start
	0,	0x06,
	3,	0x17,	0x17,	0x17,

	// power on command. Always "wait for ready" after this sequence
	0,	0x04,
	0xFF,		// wait for ready

	// Panel Setting
	0,	0x00,
	1,	0x0F,

	//Resolution 152x296 pixels
	0,	0x61,
	3,	0x98,	0x01,	0x28,
		
	
	//Vcom and data interval setting
	0,	0x50,
	1,	0x77
};
const uint8_t deviceConfigurationSize_GDEW026Z39 PROGMEM = 23;

//
// Model GDEW029Z10 - 2.9 inch Three colors red e-paper display
//

const uint8_t deviceConfiguration_GDEW029Z10[] PROGMEM = 
{

	// booster soft start
	0,	0x06,
	3,	0x17,	0x17,	0x17,

	// power on command. Always "wait for ready" after this sequence
	0,	0x04,
	0xFF,		// wait for ready

	// Panel Setting
	0,	0x00,
	2,	0x0F, 0x0D,

	//Resolution 128x296
	0,	0x61,
	3,	0x80,	0x01,	0x28,		
	
	//Vcom and data interval setting
	0,	0x50,
	2,	0x77
};
const uint8_t deviceConfigurationSize_GDEW029Z10 PROGMEM = 24;

//
// Model GDEW0371Z80 - 3.71 inch Three colors red e-paper display
//

const uint8_t deviceConfiguration_GDEW0371Z80[] PROGMEM = 
{

	// booster soft start
	0,	0x06,
	3,	0x17,	0x17,	0x1D,

// 		panel power setting command
// 	0,	0x01,	
// 	5,	0x07,	0x07,	0x3F,	0x3F,	0x0D,

	// power on command. Always "wait for ready" after this sequence
	0,	0x04,
	0xFF,		// wait for ready

	// Panel Setting
	0,	0x00,
	1,	0x0F,

	// 	PLL Control
// 	0,	0x30,
// 	1,	0x09,

	//Resolution 240x416
	0,	0x61,
	3,	0xf0,	0x01,	0xa0,

// 		VCOM_DC Setting
// 	0,	0x82,
// 	1,	0x12,
		
	
	//Vcom and data interval setting
	0,	0x50,
	2,	0x11, 0x07
};
const uint8_t deviceConfigurationSize_GDEW0371Z80 PROGMEM = 24;

#endif //__ePaperSettings_GoodDisplay__
