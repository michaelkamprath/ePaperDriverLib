#include "ePaperCanvas.h"
#include "ePaperDeviceConfigurations.h"

#define DEBUG 1

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

const uint8_t ePaperCanvas::bitmasks[] = {0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80};

ePaperCanvas::ePaperCanvas(
	int16_t w,
	int16_t h,
	ePaperColorMode mode
)	:	Adafruit_GFX(w, h),
		_bufferSize(0),
		_blackBuffer(NULL),
		_colorBuffer(NULL),
		_mode(mode)
{
	_bufferSize = width()*((height()+7)/8);
	DEBUG_PRINT("Allocating buffers with size = ");
	DEBUG_PRINT(_bufferSize);
	DEBUG_PRINT("\n");
	_blackBuffer = (uint8_t *)malloc(_bufferSize);
	if (_mode != CMODE_BW) {
		_colorBuffer = (uint8_t *)malloc(_bufferSize);
	}
}
		
ePaperCanvas::~ePaperCanvas()
{
	if (_blackBuffer) {
		free(_blackBuffer);
	}
	if (_colorBuffer) {
		free(_colorBuffer);
	}
}

void ePaperCanvas::drawPixel(int16_t x, int16_t y, ePaperColorType color)
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

		switch(color) {
			case ePaper_WHITE:
				// setting white is turning the black pixel off
				_blackBuffer[buffer_index] &= ~(buffer_bit_mask); 
				if ((getColorMode() == CMODE_3COLOR)||(getColorMode() == CMODE_4GRAY)) {
					// turn the color off where we set white
					_colorBuffer[buffer_index] &= ~(buffer_bit_mask); 
				} 
				break;
			case ePaper_BLACK:
				// turn appropriate black pixel on
				_blackBuffer[buffer_index] |=  (buffer_bit_mask);
				if (getColorMode() == CMODE_3COLOR) {
					// turn the color off where we set black
					_colorBuffer[buffer_index] &= ~(buffer_bit_mask); 
				} else if (getColorMode() == CMODE_4GRAY) {
					// turn the color on where we set black
					_colorBuffer[buffer_index] |=  (buffer_bit_mask);
				}
				break;
			case ePaper_GRAY1:
				if (getColorMode() == CMODE_4GRAY) {
					// white is off, color is on
					_blackBuffer[buffer_index] &= ~(buffer_bit_mask); 
					_colorBuffer[buffer_index] |=  (buffer_bit_mask);
				}
				break;
			case ePaper_GRAY2:
				if (getColorMode() == CMODE_4GRAY) {
					// white is on, color is off
					_blackBuffer[buffer_index] |=  (buffer_bit_mask);
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

void ePaperCanvas::fillScreen(uint16_t color)
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

void  ePaperCanvas::invertDisplay(boolean i)
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
    @brief  Sets the device image buffer directly. 
    @param	blackBitMap		the bitmap to set the bit and white image to. High (1) values set
    						the pixel black, low (0) sets the pixel white. 
    @param	blackBitMapSize	the size of the black bitmap in total bytes.
    @param	blackBitMapIsProgMem	indicates whether the passed bitmap resides in PROGMEM or not.
    @return None (void).
    @note   Since this sets the image buffer directly, the bit map must be correctly sized
    		for the device's demensions. Also, any rotation that has been set is ignored. 
*/
void ePaperCanvas::setDeviceImage( 
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
void ePaperCanvas::setDeviceImage( 
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


void ePaperCanvas::drawBitImage( 
	int16_t loc_x, int16_t loc_y,
	int16_t img_w, int16_t img_h,
	const uint8_t* blackBitMap,
	uint16_t blackBitMapSize,
	bool blackBitMapIsProgMem,
	const uint8_t* colorBitMap,
	uint16_t colorBitMapSize,
	bool colorBitMapIsProgMem
)
{

	for (int16_t i = 0; i < img_w; i++ ) {
		for (int16_t j = 0; j < img_h; j++ ) {
			int16_t buffer_index = (j*img_w + i)/8;
			int8_t buffer_bit_mask = (1 << (7-(j*img_w + i)&7));
			
			if (blackBitMap && colorBitMap && (getColorMode() == CMODE_4GRAY)) {
				// in 4 gray mode need to interpret the bits
				if( (buffer_index < blackBitMapSize)&&(buffer_index <= colorBitMapSize)) {
					uint8_t blackByteVal = blackBitMapIsProgMem ? pgm_read_byte(&blackBitMap[buffer_index]) : blackBitMap[buffer_index];
					uint8_t colorByteVal = colorBitMapIsProgMem ? pgm_read_byte(&colorBitMap[buffer_index]) : colorBitMap[buffer_index];
					bool isBlack = blackByteVal&buffer_bit_mask ? true : false;
					bool isColor = colorByteVal&buffer_bit_mask ? true : false;
					ePaperColorType c = ePaper_BLACK;
					if (isBlack&&isColor) {
						// both on is black
						c = ePaper_BLACK;
					}
					else if (isBlack && (!isColor)) {
						c = ePaper_GRAY2;
					}
					else if ((!isBlack) && isColor) {
						c = ePaper_GRAY1;
					}
					else if ((!isBlack) && (!isColor)) {
						c = ePaper_WHITE;
					}
					
					this->drawPixel(loc_x+i, loc_y+j, c);
				}
				
			}
			else if (getColorMode() != CMODE_4GRAY) {
				// use B&W or 3 Color mode
				if (blackBitMap && _blackBuffer && (buffer_index < blackBitMapSize)) {
					uint8_t byteVal = blackBitMapIsProgMem ? pgm_read_byte(&blackBitMap[buffer_index]) : blackBitMap[buffer_index];
					bool isBlack = byteVal&buffer_bit_mask ? true : false;
					this->drawPixel(loc_x+i, loc_y+j, isBlack ? ePaper_BLACK : ePaper_WHITE );
				}
				if (colorBitMap && _colorBuffer && (buffer_index <= colorBitMapSize)) {
					uint8_t byteVal = colorBitMapIsProgMem ? pgm_read_byte(&colorBitMap[buffer_index]) : colorBitMap[buffer_index];
					bool isColor = byteVal&buffer_bit_mask ? true : false;
					// only set red, let B&W image be the "background"
					if (isColor) {
						this->drawPixel(loc_x+i, loc_y+j, ePaper_COLOR);
					}
				}
			}
		}
		yield();
	}
}	
