#include <LiquidCrystal_I2C.h>
#include "DS1302.h"
#include "Keypad.h"
#include "brewtimer.h"
#include "Display.h"
#include "Configuration.h"

const long DISPLAY_TIMEOUT = 60000;   // 1 mins and display will turn itself off
RUN_MODE runMode = RUN_MODE_ACTIVE;   // startup run mode
long lastTick = 0;
long lastDisplayOn = 0;

// the setup function runs once when you press reset or power the board
void setup() {
  Serial.begin(115200);
  dsp.init();
  keyInit();
  initRelay();
  delay(1000);
  readTrigger(&trigger);
}

// the loop function runs over and over again forever
void loop() { 

  processCommand();
  
   if (runMode == RUN_MODE_ACTIVE) 
   {
      dsp.on();
      openRelay();
    } 
    else if (runMode == RUN_MODE_TIMER) 
    {          
     closeRelay();
     if (isTriggered())
     {
       changeRunMode(RUN_MODE_ACTIVE);
     }
  } 

  // refresh the display
  if (millis() - lastTick > 1000) 
  {
    updateDisplay();
    lastTick = millis();
  }

  // turn off display after  DISPLAY_TIMEOUT inactivity
  if (millis() - lastDisplayOn > DISPLAY_TIMEOUT && runMode == RUN_MODE_TIMER)
  {
      dsp.off();
  }
}

void processCommand() {
  char keyValue = getSingleKey();
  if (keyValue != '\0')
  {
    if (!dsp.isOn())
    {
      dsp.on();
      lastDisplayOn = millis();
    }
    else
    {
      // display is on let's process the users input
      switch (keyValue)
      {
        case 'A':
          changeRunMode(RUN_MODE_TIMER);
          break;
        case 'B':
          changeRunMode(RUN_MODE_ACTIVE);
          break;
        case 'C':
          initSetupMode();
          break;
        case 'D':
            dsp.off();
          break;    
      }
      lastDisplayOn = millis();
    }
  }
}

bool isTriggered () 
{
  Time now = rtc.time();  
  return now.hr == trigger.hour && now.min == trigger.min && now.sec <= 5;
}
void initSetupMode() {
    Configuration config(dsp.getLcd());
    config.Start();
    dsp.reset();
}
void changeRunMode(RUN_MODE mode) {
  if (runMode != mode) {
    runMode = mode;
    dsp.setMode(runMode);
  }
}
void updateDisplay() {
    Time now = rtc.time();
    dsp.setTime(now);
    dsp.setMode(runMode);
    dsp.setTriggerTime(trigger);
} 
