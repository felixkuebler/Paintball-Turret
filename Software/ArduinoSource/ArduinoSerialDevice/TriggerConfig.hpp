extern TriggerDriver::TriggerType;

namespace TiggerConfig {
 
  namespace Primary {
    static constexpr TriggerDriver::TriggerType TriggerType = TriggerDriver::TriggerType::FullAuto;
    static constexpr uint16_t PulseTimeOn = 50;
    static constexpr uint16_t PulseTimeOff = 100;
    
    namespace Pins {
      static constexpr uint8_t Enable = 3;
    }

  }
}
