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
    LiquidCrystal_I2C *p_lcd;
    CONFIG_MODE _mode = CONFIG_MODE_TRIGGER;
    int _cursorMap[4] = {0,1,3,4};
    int _cursorPos = 0;
    int8_t _values[4];
    bool _isDirty = false;

  public:
    Configuration(LiquidCrystal_I2C *pLcd) {
      p_lcd = pLcd;
    }

    void Start() {
      bool running = true;
      int digit = 0;      
      Serial.println("Entering setup loop");
      init();

      // main control loop
      while (running) {
        char cmd = getSingleKey();

        if (cmd == 'A') {               // switch mode
          changeMode();
        }

        //Scroll keys
        if (cmd == '#') {               
          scrollRight();
        }
        if (cmd == '*') {            
          scrollLeft();
        }
        if (isDigit(cmd)) {
          int val = cmd-48;
          p_lcd->print(val);
          _values[_cursorPos] = val;
          scrollRight();
          _isDirty = true;
        }
        if (cmd == 'B') {
          Save();
        }
        if (cmd == 'C') {
          Save();
          running = false;
        }
        if (cmd == 'D') {
          // debug key, dumps values to the output
          Serial.printf("DEBUG: position:[%x] ", _cursorPos);
          Serial.print("value:");
          for(int i=0;i<4;i++) {
              Serial.printf("[%d] ", _values[i]);        
            }
            Serial.println();   
          }
      }

      Serial.println("Leaving setup loop");
    }

  private:
    void init() {
      p_lcd->backlight();
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
      // setup LCD
      p_lcd->clear();
      p_lcd->setCursor(0,0);
      p_lcd->print("Set trigger");
      p_lcd->setCursor(0, 1);
      p_lcd->printf("%02d:%02d", trigger.hour, trigger.min);       
      p_lcd->setCursor(0, 1);
      p_lcd->blink();

      // reset
      _values[0] = trigger.hour/10%10;
      _values[1] = trigger.hour%10;
      _values[2] = trigger.min/10%10;
      _values[2] = trigger.min%10;
      _cursorPos = 0;
      _isDirty = false;  
    }
    void initTimeMode () {
      // setup LCD
      p_lcd->clear();
      p_lcd->setCursor(0,0);
      p_lcd->print("Set time");
      p_lcd->setCursor(0, 1);
      Time now = rtc.time();
      p_lcd->printf("%02d:%02d",now.hr, now.min);    
      p_lcd->setCursor(0, 1);
      p_lcd->blink(); 

      // reset
      _values[0] = now.hr/10%10;
      _values[1] = now.hr%10;
      _values[2] = now.min/10%10;
      _values[2] = now.min%10;
      _cursorPos = 0;
      _isDirty = false;  
    }
    void scrollRight() {
      if (_cursorPos < sizeof(_cursorMap))
        p_lcd->setCursor(_cursorMap[++_cursorPos], 1);   
    }
    void scrollLeft() {
      if (_cursorPos > 0)
        p_lcd->setCursor(_cursorMap[--_cursorPos], 1);      
    }
    bool isDigit(char key) {
      char digits[] {'0','1','2','3','4','5','6','7','8','9'};
      for (int x=0;x<sizeof(digits);x++) {
        if (digits[x]==key) {
          return true;
        }
      }
      return false;
    }
    void Save() {
      if (_isDirty) {
        if (_mode == CONFIG_MODE_TRIGGER) {
          Serial.println("Saving trigger");
          trigger.hour = _values[0]*10+_values[1];
          trigger.min = _values[2]*10+_values[3];
          writeTrigger(&trigger);
        } else {
          Serial.println("Saving time");
          Time now = rtc.time();
          now.hr = _values[0]*10+_values[1];
          now.min = _values[2]*10+_values[3];
          now.sec = 0;
          rtc.time(now);
        }
      }
    }
};

#endif