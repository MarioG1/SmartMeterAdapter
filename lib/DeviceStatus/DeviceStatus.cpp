#include <Arduino.h>

#include "..\LEDControl\LEDControl.h"
#include "DeviceStatus.h" 

DeviceStatus::DeviceStatus(){
}

void DeviceStatus::Init() {
  led.init();

  //Set main Status LED
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);
}

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