#include <LiquidCrystal_I2C.h>
#include "const.h"

#ifndef _BREW_DISPLAY
#define _BREW_DISPLAY

/* encapuslate all the display logic */
class Display {
private:
  LiquidCrystal_I2C lcd;
  RUN_MODE _currentMode = RUN_MODE_UNDEFINED;
  int _setupCursorPos = -1;
  bool _displayOn;
  int _cursorMap[4] = {0,1,3,4};
  
public:
  Display()
    : lcd(0x27, 16, 2) {}
  void init() {
    // setup and init the display
    lcd.init();
    on();
  }
  LiquidCrystal_I2C* getLcd() { return &lcd; }
  void setTime(Time time) {
    lcd.setCursor(9, 0);
    lcd.printf("T %02d:%02d", time.hr, time.min);
  }
  void setTriggerTime(Trigger time) {
    lcd.setCursor(9, 1);
    lcd.printf("S %02d:%02d", time.hour, time.min);
  }
  void setMode(RUN_MODE mode) {
    if (mode != _currentMode) {
      lcd.setCursor(0, 0);
      lcd.print("       ");     // clear the region
      lcd.setCursor(0, 0);
      if (mode == RUN_MODE_ACTIVE)
        lcd.print("Active");
      else
        lcd.print("Timer");
      _currentMode = mode;
    }
  }
  
  bool isOn() { return _displayOn; }
  void off() 
  { 
    lcd.noBacklight();
    _displayOn = false;
  }
  void on() 
  { 
    lcd.backlight();
    _displayOn = true; 
  }
  
 
  void reset() 
  {
    _currentMode = RUN_MODE_UNDEFINED;
    lcd.clear();
  }
};

#endif