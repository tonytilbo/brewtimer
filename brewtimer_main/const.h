#ifndef _BREWTIMER_CONSTS
#define _BREWTIMER_CONSTS

const int RELAY_PIN =   16;

enum RUN_MODE {
  RUN_MODE_UNDEFINED,
  RUN_MODE_TIMER,
  RUN_MODE_ACTIVE,
  RUN_MODE_SETUP
};

class Trigger {
  public:
    uint8_t hour;
    uint8_t min;
};

#endif