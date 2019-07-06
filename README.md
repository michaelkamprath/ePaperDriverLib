# Crystalfontz_ePaper_lib
An unofficial Arduino library for the Crystalfontz ePaper line of displays. This library implements the Adafruit GFX interface, providing the Arduino community a familiar API for manipulating the pixels on the screen.  This library is currently designed to only support SPI communications with the ePaper device.

This library supports the following colors in the GFX:

* `ePaper_WHITE` - Sets the pixel white
* `ePaper_BLACK` - Sets the pixel black
* `ePaper_COLOR` - If the ePaper device supports a third color (e.g., red), sets the pixel to that color.
* `ePaper_INVERSE` - If the pixel is currently white, set it black, and vide vera. If the pixel is currently the device's third color, set it white.

These color constants are defined in the `Crystalfontz_ePaper.h` header.

To initialize an ePaper device object, you simply declare it as follows:
```
ePaperDisplay	device(
		ePaperDisplay::CFAP176264A0_0270,	// device constant indicating what model it is
		D1,		// microtroller pin connected to display's BUSY pin
		D2,		// microtroller pin connected to display's RST pin
		D3,		// microtroller pin connected to display's D/C pin
		D8		// microtroller pin connected to display's CS pin
	)
```
Here, ESP8266 pin naming is used. Change as you need. The rest of the ePaper device pins are SPI and power. Connect to your microcontroller as appropriate. 

## Supported Models

* **[CFAP176264A0-0270](https://www.crystalfontz.com/product/cfap176264a00270-3-color-epaper-display)** - 2.7 inch 3-color ePaper display

## Requirements
The following Arduino libraries are required to make this library work: 

* [Adafruit GFX Library](https://github.com/adafruit/Adafruit-GFX-Library)

# Disclaimer 

This project and its creators are not associated with Crystalfontz or Adafruit, nor is this project officially endorsed or reviewed for correctness by Crystalfontz or Adafruit. This project is an open source effort by the community to make a usable library for the Crystalfontz line of ePaper Displays.
