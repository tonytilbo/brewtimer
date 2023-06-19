#ifndef _BREWTIMER_CONFIGURATION
#define _BREWTIMER_CONFIGURATION
#include <LiquidCrystal_I2C.h>
#include "const.h"

enum CONFIG_MODE {
  CONFIG_MODE_TRIGGER,
  CONFIG_MODE_TIME,
  CONFIG_MODE_DATE
};

class Configuration {
  private:
    LiquidCrystal_I2C lcd;
    CONFIG_MODE _mode = CONFIG_MODE_TRIGGER;

  public:
    Configuration() 
     : lcd(0x27, 16, 2) {}

    void Start() {
      bool running = true;
      Serial.println("Entering setup loop");
      init();
      while (running) {
        char cmd = getSingleKey();
        if (cmd == 'A') {
          // switch modes
          changeMode();
        }
        if (cmd == 'C')
          running = false;
      }

      Serial.println("Leaving setup loop");
    }

  private:
    void init() {
      lcd.backlight();
      _mode = CONFIG_MODE_TRIGGER;
      initTriggerMode();
    }
    void changeMode() {
      Serial.println("Changing setup mode");
      if (_mode == CONFIG_MODE_TRIGGER) {
        _mode = CONFIG_MODE_TIME;
        initTimeMode();
      } else {
        _mode = CONFIG_MODE_TRIGGER;
        initTriggerMode();
      }
    }
    void initTriggerMode() {
      Serial.println("Setup trigger mode");
      // setup LCD
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Set trigger");
      lcd.setCursor(0, 1);

      Serial.printf("%02d:%02d\r\n", trigger.hour, trigger.min);
      lcd.printf("%02d:%02d", trigger.hour, trigger.min);    
      lcd.setCursor(0, 0);
      lcd.blink();
    }
    void initTimeMode () {
      Serial.println("Setup time mode");
      // setup LCD
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Set time");
      lcd.setCursor(0, 1);
      Time now = rtc.time();
      lcd.printf("%02d:%02d",now.hr, now.min);    
      lcd.setCursor(0, 1);
      //lcd.blink();   
    }
};

#endif