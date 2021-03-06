#include <ePaperDriver.h>
#include <Fonts/FreeSansBoldOblique12pt7b.h>
#include <Fonts/FreeSansBold24pt7b.h>
#include "Image.h"

ePaperDisplay *device;

void setup() {
	Serial.begin(115200);
	delay(500);
#if defined( ESP8266 )
	device = new ePaperDisplay( GDEW0371W7, D1, D2, D3, D8 );
#elif defined ( ESP32 )
	device = new ePaperDisplay( GDEW0371W7, 21, 2, 4, 5 );
#elif defined(__AVR_ATmega1284__) || defined(__AVR_ATmega1284P__)
	// assume using MightyCore for the ATmega1284
	device = new ePaperDisplay( GDEW0371W7, PIN_PB1, PIN_PB2, PIN_PB3, PIN_PB4 );
#else
	device = new ePaperDisplay( GDEW0371W7, 3, 4, 5, 10 );
#endif
}

void loop() {
	//
	// demonstrate using the Adafruit GFX primitives to draw an image
	//
	device->setFont(&FreeSansBoldOblique12pt7b);
	device->setTextColor(ePaper_BLACK);
	device->setRotation(0);
	
	const __FlashStringHelper	* str = F("Hello World!");
	int16_t x1, y1;
	uint16_t w, h;

	device->getTextBounds(str, 0, 0, &x1, &y1, &w, &h);
  
	int centerX = device->width()/2;
	int centerY = device->height()/2;
	int radius = device->width()/2 - 1;

	device->clearDisplay();
	device->fillCircle(centerX, centerY, radius, ePaper_BLACK);
	device->fillTriangle(
		centerX, centerY-radius,
		centerX - 2*radius*sin(PI/3.0)*sin(PI/6.0), centerY + radius*(2.0*sin(PI/3.0)*sin(PI/3.0)-1.0),
		centerX + 2*radius*sin(PI/3.0)*sin(PI/6.0), centerY + radius*(2.0*sin(PI/3.0)*sin(PI/3.0)-1.0),  
		ePaper_GRAY2		
	);
	device->fillTriangle(
		centerX, centerY + radius*(2.0*sin(PI/3.0)*sin(PI/3.0)-1.0),
		centerX - radius*sin(PI/6.0)*sin(PI/3.0), centerY - radius*(1.0-sin(PI/3.0)*sin(PI/3.0)),
		centerX + radius*sin(PI/6.0)*sin(PI/3.0), centerY - radius*(1.0-sin(PI/3.0)*sin(PI/3.0)),  
		ePaper_GRAY1		
	);
	
	device->setCursor(centerX-w/2, device->height() - 8);
	device->print(str);
		
	device->refreshDisplay();
	delay(10000);

	// clean screen
	device->clearDisplay();
	device->refreshDisplay();

	//
	// demonstrate drawing to a "rotated" screen
	//
	device->setFont(&FreeSansBold24pt7b);
	device->setTextColor(ePaper_INVERSE1);
	device->setRotation(1);
	device->clearDisplay();
	device->fillTriangle(
		0, 0,
		device->width()-1, 0,
		0, device->height()-1,
		ePaper_GRAY2		
	);
	device->fillTriangle(
		0, device->height()-1,
		device->width()-1, 0,
		device->width()-1, device->height()-1,
		ePaper_GRAY1		
	);
	
	device->getTextBounds(str, 0, 0, &x1, &y1, &w, &h);
	device->setCursor(device->width()/2-w/2, device->height()/2 - 14 );
	device->print(str);
	device->setCursor(device->width()/2-w/2, device->height()/2 + 14 + h );
	device->print(str);
	
	device->refreshDisplay();
	delay(10000);
	
	//
	// demonstrate drawing a pixel image to the screen 
	//
	device->setRotation(0);
	device->clearDisplay(); 
	device->drawBitImage(
		0, 0,
		IMAGE_WIDTH, IMAGE_HEIGHT,
		image_data1,
		IMAGE_ARRAY_SIZE,
		true,
		image_data2,
		IMAGE_ARRAY_SIZE,
		true
	);
	device->refreshDisplay();
	delay(10000);

}
