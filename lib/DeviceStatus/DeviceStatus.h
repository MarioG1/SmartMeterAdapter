#ifndef DEVICESTATUS_H
#define DEVICESTATUS_H

#include "..\LEDControl\LEDControl.h"

class DeviceStatus {
  private:
    LEDControl led;
    int currentStatus = -1;
    unsigned long lastCycle = 0;
    boolean lastPeriodStatus = false;
  public:
    DeviceStatus();
    void Init();
    enum status {WIFI_CONNECT, WIFI_ERROR, READY, BUSY, ERROR, OFF};
    void setStatus(int st);
    void blinkCycle(int interval);
};

#endif