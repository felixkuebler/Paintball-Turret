#include <stdint.h>

class Endstop {

private:

  uint8_t pin;

  bool invertLogic = false;
  
  bool lastState = false;
  
  bool stateChange = false;

public:

  Endstop::Endstop(uint8_t pin, bool invertLogic = false) : 
    pin(pin),
    invertLogic(invertLogic) {
    pinMode(pin, INPUT);
  }

  bool Endstop::read() {
    bool state = digitalRead(pin) == !invertLogic;

    if (state != lastState) {
      stateChange = true;
    }

    lastState = state;
    return state;
  }

  bool Endstop::isStateChange() {
    bool stateChangeTmp = stateChange;
    stateChange = false;
    return stateChangeTmp;
  }

  bool Endstop::isTriggered() {
    return lastState;
  }
};
