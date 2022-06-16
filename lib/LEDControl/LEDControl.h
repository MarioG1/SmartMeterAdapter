#ifndef LEDCONTROL_H
#define LEDCONTROL_H

class LEDControl {
  private:
    int onColor[3];
    int offColor[3];
  public:
    LEDControl();
    void init();
    void setOnColor(int red, int green, int blue);
    void setOffColor(int red, int green, int blue);
    void enable();
    void disable();
};

#endif