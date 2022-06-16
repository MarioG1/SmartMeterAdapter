#include <Arduino.h>

#include "..\LEDControl\LEDControl.h"
#include "DeviceStatus.h" 

/**
 * Class to use which uses the RBG LED to show the current status of the application
 **/ 
DeviceStatus::DeviceStatus(){
}

/**
 * Initialize the RBL LED and the "status" LED
 **/ 
void DeviceStatus::Init() {
  led.init();

  //Set main Status LED
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);
}

/**
 * Sets the LED to an given status (color)
 **/ 
void DeviceStatus::setStatus(int st) {
  switch(st) {
    case READY:
      led.setOnColor(0,128,0);
      led.setOffColor(0,128,0);
    break;
    case BUSY:
      led.setOnColor(128,64,0);
      led.setOffColor(128,64,0);
    break;
    case WIFI_CONNECT:
      led.setOnColor(0,0,128);
      led.setOffColor(0,0,0);
    break;
    case WIFI_ERROR:
      led.setOnColor(128,0,0);
      led.setOffColor(0,128,0);
      break;
    case ERROR:
      led.setOnColor(128,0,0);
      led.setOffColor(128,0,0);
    break;
    default:
      led.setOnColor(0,0,0);
      led.setOffColor(0,0,0);
    break;
  }

  //Switch on LED on Status change
  if(st != currentStatus || !lastPeriodStatus) {
    led.enable();
  }
  currentStatus = st;
}

/**
 * Fix Color LED is switched on/off in an intervall to act as an heartbeat.
 * @param intervall in ms teh LED should blink
 **/ 
void DeviceStatus::blinkCycle(int interval) {
  if(millis() > (lastCycle + interval) || millis() < lastCycle) {
      lastCycle = millis();

      if(lastPeriodStatus) {
        led.enable();
        digitalWrite(LED_BUILTIN, LOW);
      } else {
        led.disable();
        digitalWrite(LED_BUILTIN, HIGH);
      }

      lastPeriodStatus = !lastPeriodStatus;
  }
}