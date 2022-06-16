#include <utility/wifi_drv.h>
#include "LedControl.h" 

LEDControl::LEDControl() { 
  onColor[0] = 0;
  onColor[1] = 0;
  onColor[2] = 0;

  offColor[0] = 0;
  offColor[1] = 0;
  offColor[2] = 0; 
}

void LEDControl::init() {
  WiFiDrv::pinMode(25, OUTPUT);  //RED
  WiFiDrv::pinMode(26, OUTPUT);  //GREEN
  WiFiDrv::pinMode(27, OUTPUT);  //BLUE
  WiFiDrv::analogWrite(25, 0);  // for configurable brightness
  WiFiDrv::analogWrite(26, 0);  // for configurable brightness
  WiFiDrv::analogWrite(27, 0);  // for configurable brightness  
}

void LEDControl::setOnColor(int red, int green, int blue) {
    onColor[0] = red;
    onColor[1] = green;
    onColor[2] = blue;
}

void LEDControl::setOffColor(int red, int green, int blue) {
    offColor[0] = red;
    offColor[1] = green;
    offColor[2] = blue;
}

void LEDControl::enable() {
  WiFiDrv::analogWrite(25, onColor[0]);
  WiFiDrv::analogWrite(26, onColor[1]);
  WiFiDrv::analogWrite(27, onColor[2]);
}

void LEDControl::disable() {
  WiFiDrv::analogWrite(25, offColor[0]);
  WiFiDrv::analogWrite(26, offColor[1]);
  WiFiDrv::analogWrite(27, offColor[2]);
}
