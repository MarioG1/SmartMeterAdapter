#include <utility/wifi_drv.h>
#include "LedControl.h" 

/**
 * Class to control the RBG LED on the ArduinoUno WiFi
 * The LED is connected to the WiFI module so we need to communication with the WiFi module to controll the LED
 **/ 
LEDControl::LEDControl() { 
  onColor[0] = 0;
  onColor[1] = 0;
  onColor[2] = 0;

  offColor[0] = 0;
  offColor[1] = 0;
  offColor[2] = 0; 
}

/**
 * Configure OutPuts via WiFiDrv/WiFi module for the RBG Led
 **/ 
void LEDControl::init() {
  WiFiDrv::pinMode(25, OUTPUT);  //RED
  WiFiDrv::pinMode(26, OUTPUT);  //GREEN
  WiFiDrv::pinMode(27, OUTPUT);  //BLUE
  WiFiDrv::analogWrite(25, 0);  // for configurable brightness
  WiFiDrv::analogWrite(26, 0);  // for configurable brightness
  WiFiDrv::analogWrite(27, 0);  // for configurable brightness  
}

/**
 * Set the "ON" Color of the LED
 **/ 
void LEDControl::setOnColor(int red, int green, int blue) {
    onColor[0] = red;
    onColor[1] = green;
    onColor[2] = blue;
}

/**
 * Set the "OFF" Color of the LED
 **/ 
void LEDControl::setOffColor(int red, int green, int blue) {
    offColor[0] = red;
    offColor[1] = green;
    offColor[2] = blue;
}

/**
 * Enable the LED with set colors (setOnColor)
 **/ 
void LEDControl::enable() {
  WiFiDrv::analogWrite(25, onColor[0]);
  WiFiDrv::analogWrite(26, onColor[1]);
  WiFiDrv::analogWrite(27, onColor[2]);
}

/**
 * Disable the LED with set colors (setOffColor)
 **/ 
void LEDControl::disable() {
  WiFiDrv::analogWrite(25, offColor[0]);
  WiFiDrv::analogWrite(26, offColor[1]);
  WiFiDrv::analogWrite(27, offColor[2]);
}
