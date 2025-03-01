namespace SerialCommands {

  namespace Trigger {
    static constexpr uint8_t Primary = 1;
    static constexpr uint8_t Secondary = 2;
  }

  namespace Motor {

    namespace Pitch {
       static constexpr uint8_t WritePositionAbsolute = 10;
       static constexpr uint8_t WritePositionRelative = 11;
       static constexpr uint8_t WriteSpeed = 12;
       static constexpr uint8_t ReadPosition = 13;
       static constexpr uint8_t Calibrate = 19;
    }

    namespace Yaw {
       static constexpr uint8_t WritePositionAbsolute = 20;
       static constexpr uint8_t WritePositionRelative = 21;
       static constexpr uint8_t WriteSpeed = 22;
       static constexpr uint8_t ReadPosition = 23;
    }

    static constexpr uint8_t WritePositionAbsolute = 30;
    static constexpr uint8_t WritePositionRelative = 31;
    static constexpr uint8_t WriteSpeed = 32;
    static constexpr uint8_t ReadPosition = 33;
  }

  static constexpr uint8_t EnableDataStream = 40;
  static constexpr uint8_t SyncWord = '#';
}
