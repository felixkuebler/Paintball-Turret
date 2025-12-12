#include <stdint.h>

class TriggerDriver {

public:

  enum TriggerType : uint8_t {
    Safe = 0,
    SemiAuto = 1,
    FullAuto = 2
  };
  
private:

  uint8_t pin;

  TriggerType type;
  uint16_t onTime;
  uint16_t offTime;

  bool enable = false;
  uint32_t timer;

public:

  TriggerDriver::TriggerDriver(uint8_t pin, TriggerType type = TriggerType::Safe, uint16_t onTime = 100, uint16_t offTime = 100) : 
    pin(pin),
    type(type),
    onTime(onTime),
    offTime(offTime) {
    pinMode(pin, OUTPUT);
  }

  void TriggerDriver::setTrigger(bool enable) {
    this->enable = enable;
    this->timer = millis();
  }

  void TriggerDriver::setTriggerType(TriggerType type) {
    this->type = type;
  }

  bool TriggerDriver::run() {
    if (this->type == TriggerType::SemiAuto) {

      uint32_t timeDiff = millis()- this->timer;
      
      if (this->enable && (timeDiff < this->onTime)) {
        digitalWrite(this->pin, HIGH);
      }
      else {
        digitalWrite(this->pin, LOW);
      }
    }
    else if (this->type == TriggerType::FullAuto) {
      uint32_t timeDiff = millis()- this->timer;

      if (this->enable) {
        if (digitalRead(this->pin)== LOW && timeDiff >= this->offTime) {
          digitalWrite(this->pin, HIGH);
          this->timer = millis();
        }
        else if (digitalRead(this->pin)== HIGH && timeDiff >= this->onTime) {
          digitalWrite(this->pin, LOW);
          this->timer = millis();
        }
      }
      else {
        digitalWrite(this->pin, LOW);
      }
    }
    else {
      digitalWrite(this->pin, LOW);
    }
  }

  bool TriggerDriver::isTriggered() {
    return enable;
  }
};
