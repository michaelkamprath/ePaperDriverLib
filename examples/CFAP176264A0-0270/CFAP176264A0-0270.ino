#include "Crystalfontz_ePaper.h"
#include "CrystalfontzDemoImage.h"

ePaperDisplay *device;

void setup() {
#if defined( ESP8266 )
  device = new ePaperDisplay( ePaperDisplay::CFAP176264A0_0270, D1, D2, D3, D8 );
#else
  device = new ePaperDisplay( ePaperDisplay::CFAP176264A0_0270, 3, 4, 5, 10 );
#endif
  device->powerUpDevice();

}

void loop() {
  device->setDeviceImage(
      Splash_Mono_1BPP, 5808, true,
      Splash_Red_1BPP, 5808, true
  );
  delay(5000);
  device->blankDeviceWhite();
  delay(5000);
  device->blankDeviceBlack();
  delay(5000);
  device->blankDeviceColor();
  delay(5000);
}
