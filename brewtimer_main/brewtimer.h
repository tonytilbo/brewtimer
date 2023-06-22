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