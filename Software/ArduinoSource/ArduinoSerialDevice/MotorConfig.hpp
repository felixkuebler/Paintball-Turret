namespace MotorConfig {

    namespace Pitch {
      static constexpr uint8_t Steps = 200;
      static constexpr uint8_t MicroSteps = 8;

      static constexpr uint8_t Rpm = 8;
      static constexpr uint8_t MinRpm = 0;
      static constexpr uint8_t MaxRpm = 20;

      static constexpr uint8_t GearTeeth1 = 16;
      static constexpr uint8_t GearTeeth2 = 68;
      static constexpr float GearRatio = static_cast<float>(GearTeeth2)/static_cast<float>(GearTeeth1);
      
      namespace Pins {
        static constexpr uint8_t Rx = 11;
        static constexpr uint8_t Tx = 12;
        static constexpr uint8_t Endstop = A3;
      }
    }

    namespace Yaw {
      static constexpr uint8_t Steps = 200;
      static constexpr uint8_t MicroSteps = 16;

      static constexpr uint8_t Rpm = 8;
      static constexpr uint8_t MinRpm = 0;
      static constexpr uint8_t MaxRpm = 20;

      static constexpr uint8_t GearTeeth1 = 16;
      static constexpr uint8_t GearTeeth2 = 156;
      static constexpr float GearRatio = static_cast<float>(GearTeeth2)/static_cast<float>(GearTeeth1);
      
      namespace Pins {
        static constexpr uint8_t Rx = 6;
        static constexpr uint8_t Tx = 7;
      }
    }
}
