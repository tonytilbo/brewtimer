#include "keypad.h"
#include "Display.h"
#include "const.h"

const int CLK_PIN =     19;
const int DAT_PIN =     20;
const int RST_PIN =     21;

// display and realtime clock
Display dsp = Display();
DS1302 rtc(RST_PIN, DAT_PIN, CLK_PIN);

// application settings
struct Trigger trigger;

// top level operation mode
void initRelay() 
{ 
  pinMode(RELAY_PIN, OUTPUT); 
  digitalWrite(RELAY_PIN, HIGH);
}
void openRelay() { digitalWrite(RELAY_PIN, LOW); }
void closeRelay() { digitalWrite(RELAY_PIN, HIGH); }
bool isDigit(char key) {
  char digits[] {'0','1','2','3','4','5','6','7','8','9'};
  for (int x=0;x<sizeof(digits);x++) {
    if (digits[x]==key) {
      return true;
    }
  }
  return false;
}

void writeTrigger(Trigger *trigger)
{
  uint8_t data[4];
  data[0]='t';
  data[1]='m';
  data[2]=trigger->hour;
  data[3]=trigger->min;
  
  rtc.writeProtect(false);
  rtc.writeRamBulk(data, 4);
  rtc.writeProtect(true);
}
void readTrigger(Trigger *trigger)
{
  uint8_t data[4];
  rtc.readRamBulk(data, 4);
  if (data[0]=='t' && data[1]=='m')
  {
    trigger->hour = data[2];
    trigger->min = data[3];    
  }
}

void runSetupMode(Display *dsp) {
  int digit = 0;
  bool running = true;
  dsp->initSetupMode(trigger);  
  int8_t values[4] = 
  { 
    trigger.hour/10%10, 
    trigger.hour%10, 
    trigger.min/10%10, 
    trigger.min%10
  };
  
  while (running) 
  {
    char key = getSingleKey();
    // command keys
    if (key == '#' && digit < 4)
    {
      // we scroll and if we are on the sec part of the hour
      // scroll again to get past the separator
      dsp->scrollRight();
      digit++;
      Serial.printf("Scroll right digit:[%2x]", digit);
      Serial.println();         
    }  
    if (key == '*' && digit > 0)
    {
      // scroll left
      dsp->scrollLeft();       
      digit--;
      Serial.printf("Scroll left digit:[%2x]", digit);
      Serial.println();  
    } 

    // check if this is a digit
    if (isDigit(key)) {
      int val = key-48;
      Serial.printf("Setting digit:[%d] position:[%d]", val, digit);
      Serial.println(); 

      if (digit < 4) 
      {
          dsp->setChar(key);
          values[digit++] = val;   
      }      

    }

    // Complete key, sets the value and goes back to menu
    if (key == 'C') 
    {      
       trigger.hour = values[0]*10+values[1];
       trigger.min = values[2]*10+values[3];
       writeTrigger(&trigger);  
      running = false;          
    }

    if (key == 'D')
    {
      // debug key, dumps values to the output
      Serial.printf("DEBUG: position:[%x] ", digit);
      Serial.print("value:");
      for(int i=0;i<4;i++)
      {
          Serial.printf("[%d] ", values[i]);        
      }
      Serial.println();   
    }

  }
  dsp->reset();  
}