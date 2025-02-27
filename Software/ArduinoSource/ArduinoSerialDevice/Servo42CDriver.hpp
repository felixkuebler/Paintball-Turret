#include <stdint.h>
#include <math.h>

#include <SoftwareSerial.h>


/**
 * https://github.com/makerbase-mks/MKS-SERVO42C/wiki/Serial-communication-description
 */
class Servo42CDriver {

private:

  enum UartCommands : uint8_t {
    ReadEncoder = 0x30,
    ReadMotorAngle = 0x36,
    SetZeroMode = 0x90,
    SetZero = 0x91,
    SetZeroSpeed = 0x92,
    MoveToZero = 0x94,
    MoveSpeed = 0xF6,
    Stop = 0xF7,
    MoveAngleRelative = 0xFD,
  };

  SoftwareSerial* uart;
  HardwareSerial* debug;

  uint8_t uartId;
  uint8_t microStep;
  uint8_t stepsPerRevolution = 200;

  double encoderZeroOffset = 0;
  double cachedAngle = 0;

public:

  /**
   * 
   */
  Servo42CDriver::Servo42CDriver(SoftwareSerial& uart, uint8_t uartId = 0xE0, HardwareSerial* debug = nullptr) : 
    uart(&uart),
    debug(debug),
    uartId(uartId),
    microStep(16),
    stepsPerRevolution(200) {}

  /**
   * Getter function for the cached angle
   * This angle member variable is updated when a readAngle call was issued.
   * 
   * @return the value of the last cached angle
   */
  double Servo42CDriver::getCachedAngle() {
    return cachedAngle;
  }
  
  /**
   * Sets the local encoder offset vartibale to the current encoder position.
   * This is used for the moveAngleAbsolute function that is relative to this offset.
   * 
   * @param timeout (optional, default = 0xFFFF) time in ms waiting on the angle via uart
   * @return true if setting the offset was successfull
   */
  bool Servo42CDriver::setZero(uint16_t timeout = 0xFFFF) {
    // read current encoder angle from motor and store in offset memeber variable
    double encoderAngle = 0;
    bool responseResult = this->uartCmdReadAngleEncoder(encoderAngle, timeout);
    this->encoderZeroOffset = encoderAngle;

    return responseResult;
  }

  /**
   * Read the angle of the motor.
   * This will call the uart command for reading the encoder angle
   * An offset will be appplied based on the setZero() command
   * 
   * @param return reference for the encoder angle
   * @param timeout (optional, default = 0xFFFF) time in ms waiting on the angle via uart
   * @return true if readAngle was successfully commanded
   */
  bool Servo42CDriver::readAngle(double& angle, uint16_t timeout = 0xFFFF) {

    // read value via uart command 
    double encoderAngle = 0;
    bool responseResult = this->uartCmdReadAngleEncoder(encoderAngle, timeout);

    // apply offset
    angle = encoderAngle-this->encoderZeroOffset;

    // update cached angle
    cachedAngle = angle;
    
    return responseResult;
  }

  /**
   * Stops the motor
   * 
   * @param sync (optional, default = false) true if function will block until motor has stopped
   * @param timeout (optional, default = 0xFFFF) time in ms the sync operation will block
   * @return true if stop was successfully commanded
   */
  bool Servo42CDriver::stop(bool sync = false, uint16_t timeout = 0xFFFF) {
    return this->uartCmdStop(sync, timeout);
  }

  /**
   * Move the motor forward / reverse in a constant speed
   * This will call the uart command for moving with constant speed
   * 
   * @param rpm desired rpm, sign indicated direction of rotation
   * @param sync (optional, default = false) true if function will block until speed has been set
   * @param timeout (optional, default = 0xFFFF) time in ms the sync operation will block
   * @return true if moveSpeed was successfully commanded
   */
  bool Servo42CDriver::moveSpeed(int16_t rpm, bool sync = false, uint16_t timeout = 0xFFFF) {
    return this->uartCmdMoveSpeed(rpm, sync, timeout);
  }

  /**
   * Move motor to an angle with a specific speed
   * This will call the uart command for moving to a specific angle
   *
   * @param angle the angle of rotation, relative to current position
   * @param rpm the speed of rotation
   * @param sync (optional, default = false) true if function will block until angle is reached
   * @param timeout (optional, default = 0xFFFF) time in ms the sync operation will block
   * @return true if moveAngleRelative was successfully commanded
   */
  bool Servo42CDriver::moveAngleRelative(double angle, int16_t rpm, bool sync = false, uint16_t timeout = 0xFFFF) {
    return this->uartCmdMoveAngleRelative(angle, rpm, sync, timeout);
  }

 /**
  * Move motor to an absolute angle with a specific speed.
  * The angle is based on the current encoder position.
  * The function makes use of the readAngle and moveAngleRelative functions
  * 
  * @param angle the angle of rotation, absolute in the context of the encoder value
  * @param rpm the speed of rotation
  * @param sync (optional, default = false) true if function will block until angle is reached
  * @param timeout (optional, default = 0xFFFF) time in ms the sync operation will block
  * @return true if moveAngleAbsolute was successfully commanded
  */
  bool Servo42CDriver::moveAngleAbsolut(double angle, int16_t rpm, bool sync = false, uint16_t timeout = 0xFFFF) {

    // read current encoder angle from motor
    double encoderAngle = 0;
    bool responseResultReadAngle = this->readAngle(encoderAngle);

    if (responseResultReadAngle) {

      // calculate the target angle from the current encoder position
      double targetAngle = angle - encoderAngle;

      // command relative movement based on the calculated offset
      bool responseResultMoveAngle = this->moveAngleRelative(targetAngle, rpm, sync, timeout);

      return responseResultReadAngle && responseResultMoveAngle;
    }

    return responseResultReadAngle;
  }

protected:

  /**
   * Set the mode of zeroMode（Same as the " 0_Mode " option on screen）
   * Command19 : e0 90 xx tCHK
   * xx :
   *  00-Disable;
   *  01-DirMode;
   *  02-NearMode
   * Return e0 , result (uint8_t), rCHK.
   * 
   * @param sync (optional, default = false) true if function will block the mode has been set
   * @param timeout (optional, default = 0xFFFF) time in ms the sync operation will block
   * @return true if setZeroMode was successfully commanded
   */
  bool Servo42CDriver::uartCmdSetZeroMode(bool sync = false, uint16_t timeout = 0xFFFF) {
    uint8_t cmdLen = 2;
    uint8_t cmd[cmdLen] = {this->UartCommands::SetZeroMode, 0x02};
    this->writeCommand(this->uartId, cmd, cmdLen);

    if (sync) {
      uint8_t respLen = 1;
      uint8_t resp[respLen] = {0x00};
      return this->readResponse(this->uartId, resp, respLen, timeout);
    }
    
    return true;
  }

  /**
   * Set the zero of zeroMode（Same as the " Set 0 " option on screen）
   * Command20 : e0 91 xx tCHK
   * Return e0 , result (uint8_t), rCHK.
   * 
   * @param sync (optional, default = false) true if function will block until zero has been set
   * @param timeout (optional, default = 0xFFFF) time in ms the sync operation will block
   * @return true if setZero was successfully commanded
   */
  bool Servo42CDriver::uartCmdSetZero(bool sync = false, uint16_t timeout = 0xFFFF) {
    uint8_t cmdLen = 2;
    uint8_t cmd[cmdLen] = {this->UartCommands::SetZero, 0x00};
    this->writeCommand(this->uartId, cmd, cmdLen);

    if (sync) {
      uint8_t respLen = 1;
      uint8_t resp[respLen] = {0x00};
      return this->readResponse(this->uartId, resp, respLen, timeout);
    }
    
    return true;
  }

  /**
   * Set the speed of zeroMode（Same as the " 0_Speed " option on screen）
   * Note: The mode of "0_Mode" needs to be set first.
   * Command21 : e0 92 xx tCHK
   * (Range 0~4, the smaller the value, the faster the speed)
   * Return e0 , result (uint8_t), rCHK.
   * 
   * @param sync (optional, default = false) true if function will block until speed has been set
   * @param timeout (optional, default = 0xFFFF) time in ms the sync operation will block
   * @return true if setZeroSpeed was successfully commanded
   */
  bool Servo42CDriver::uartCmdSetZeroSpeed(bool sync = false, uint16_t timeout = 0xFFFF) {
    uint8_t cmdLen = 2;
    uint8_t cmd[cmdLen] = {this->UartCommands::SetZeroSpeed, 0x04};
    this->writeCommand(this->uartId, cmd, cmdLen);

    if (sync) {
      uint8_t respLen = 1;
      uint8_t resp[respLen] = {0x00};
      return this->readResponse(this->uartId, resp, respLen, timeout);
    }

    return true;
  }
  
  /**
   * Return to zero（Same as the " Goto 0 " option on screen）
   * Command23 : e0 94 00 tCHK
   * Return e0 , result (uint8_t), rCHK.
   * 
   * @param sync (optional, default = false) true if function will block until zero is reached
   * @param timeout (optional, default = 0xFFFF) time in ms the sync operation will block
   * @return true if moveToZero was successfully commanded
   */
  bool Servo42CDriver::uartCmdMoveToZero(bool sync = false, uint16_t timeout = 0xFFFF) {
    uint8_t cmdLen = 2;
    uint8_t cmd[cmdLen] = {this->UartCommands::MoveToZero, 0x00};
    this->writeCommand(this->uartId, cmd, cmdLen);

    if (sync) {
      uint8_t respLen = 1;
      uint8_t resp[respLen] = {0x00};
      return this->readResponse(this->uartId, resp, respLen, timeout);
    }

    return true;
  }

  /**
   * Stop the motor
   * command 32: e0 f7 tCHK
   * Return e0,result (uint8_t), rCHK.
   * 
   * @param sync (optional, default = false) true if function will block until motor has been stopped
   * @param timeout (optional, default = 0xFFFF) time in ms the sync operation will block
   * @return true if stop was successfully commanded
   */
  bool Servo42CDriver::uartCmdStop(bool sync = false, uint16_t timeout = 0xFFFF) {
    uint8_t cmdLen = 1;
    uint8_t cmd[cmdLen] = {this->UartCommands::Stop};
    this->writeCommand(this->uartId, cmd, cmdLen);

    if (sync) {
      uint8_t respLen = 1;
      uint8_t resp[respLen] = {0x00};
      return this->readResponse(this->uartId, resp, respLen, timeout);
    }

    return true;
  }

  /**
   * Move the motor forward / reverse in a constant speed
   * command 31: e0 f6 xx tCHK
   * The XX highest bit indicates direction, and the XX lowest 7 bits indicate 128 speed gears
   * Return e0, result (uint8_t), rCHK.
   *
   * @param rpm desired rpm, sign indicated direction of rotation
   * @param sync (optional, default = false) true if function will block until speed has been set
   * @param timeout (optional, default = 0xFFFF) time in ms the sync operation will block
   * @return true if moveSpeed was successfully commanded
   */
  bool Servo42CDriver::uartCmdMoveSpeed(int16_t rpm, bool sync = false, uint16_t timeout = 0xFFFF) {
    // if rpm is set to zero use stop command
    if (rpm == 0) {
      return this->uartCmdStop();
    }

    // rpm = (speed × 30000)/(microStep × 200) for 1.8°motor
    // rpm = (speed × 30000)/(microStep × 400) for 0.9°motor

    uint32_t speed = (int32_t)abs(rpm)*this->microStep*this->stepsPerRevolution/30000;
    speed &= 0b01111111;
    speed = rpm<0 ? (speed | 0b10000000) : speed;

    uint8_t cmdLen = 2;
    uint8_t cmd[cmdLen] = {this->UartCommands::MoveSpeed, (uint8_t)speed};
    this->writeCommand(this->uartId, cmd, cmdLen);

    if (sync) {
      uint8_t respLen = 1;
      uint8_t resp[respLen] = {0x00};
      return this->readResponse(this->uartId, resp, respLen, timeout);
    }

    return true;
  }

  /**
   * Move motor to an angle with a specific speed.
   * command 34: e0 fd xx xx xx xx xx tCHK
   * The third byte(xx) defines the direction and speed, such as command 9.
   * The last two bytes(xx xx xx xx ) define the number of pulses.
   * If(xx xx xx xx = 00 00 0C 80),the motor(1.8 °, subdivision 16) will rotates one circle
   * Note: will return two result values, one when starting, one when finished
   * Return e0, result (uint8_t), rCHK.
   *
   * @param angle the angle of rotation, relative to current position
   * @param rpm the speed of rotation
   * @param sync (optional, default = false) true if function will block until angle is reached
   * @param timeout (optional, default = 0xFFFF) time in ms the sync operation will block
   * @return true if moveAngleRelative was successfully commanded
   */
  bool Servo42CDriver::uartCmdMoveAngleRelative(double angle, int16_t rpm, bool sync = false, uint16_t timeout = 0xFFFF) {
    // if rpm is set to zero no action needs to be taken
    if (rpm == 0) {
      return false;
    }

    // the direction of rotation is set via rpm
    rpm = abs(rpm);

    // rpm = (speed × 30000)/(microStep × 200) for 1.8°motor
    // rpm = (speed × 30000)/(microStep × 400) for 0.9°motor

    uint32_t speed = (int32_t)rpm*this->microStep*this->stepsPerRevolution/30000;

    // set the direction based on the angle sign
    speed &= 0b01111111;
    speed = angle>0 ? (speed | 0b10000000) : speed;
    
    // steps = rotations × microStep × 200 for 1.8°motor
    // steps = rotations × microStep × 400 for 0.9°motor

    uint32_t steps = (abs(angle)/360)*this->microStep*this->stepsPerRevolution;

    uint8_t cmdLen = 6;
    uint8_t cmd[cmdLen] = {
      this->UartCommands::MoveAngleRelative, 
      (uint8_t)speed,
      (uint8_t)(steps >> 3*8),
      (uint8_t)(steps >> 2*8),
      (uint8_t)(steps >> 1*8),
      (uint8_t)(steps >> 0*8)
    };
    this->writeCommand(this->uartId, cmd, cmdLen);

    // poll start status of rotation
    uint8_t respLen = 1;
    uint8_t respStart[respLen] = {0x00};
    bool responseResultStart = this->readResponse(this->uartId, respStart, respLen);

    if (sync) {
      // poll end status of rotation
      uint8_t respEnd[respLen] = {0x00};
      bool responseResultEnd = this->readResponse(this->uartId, respEnd, respLen, timeout);

      return responseResultStart && responseResultEnd;
    }

    return responseResultStart;
  }

  /**
   * Read the encoder value (the motor should be calibrated)
   * command 1 : e0 30 tCHK
   * Return e0, carry (int32), value(uint16), rCHK. The returned value ranges from 0 to FFFF.
   * carry: the carry vaule of the encoder
   * value: the current vaule of the encoder.(range 0~0xFFFF
   * When value is greater than 0xFFFF, carry +=1
   * When Value is less than 0, carry -=1
   * Note: The encoder value is updated regardless of whether the motor is enabled or not.
   * 
   * @param return reference for the encoder angle
   * @param timeout (optional, default = 0xFFFF) time in ms waiting on the angle via uart
   * @return true if readAngleEncoder was successfully commanded
   */
  bool Servo42CDriver::uartCmdReadAngleEncoder(double& angle, uint16_t timeout = 0xFFFF) {
    // if rpm is set to zero no action needs to be taken

    uint8_t cmdLen = 1;
    uint8_t cmd[cmdLen] = {this->UartCommands::ReadEncoder};
    this->writeCommand(this->uartId, cmd, cmdLen);

    uint8_t respLen = 6;
    uint8_t resp[respLen] = {0x00};
    bool responseResult = this->readResponse(this->uartId, resp, respLen, timeout);

    if (responseResult) {
      uint8_t uartId = resp[0];
      uint8_t checksum = resp[respLen-1];
      
      int32_t carry = (resp[0] << 3*8) | (resp[1] << 2*8) | (resp[2] << 1*8) | (resp[3] << 0*8);
  
      uint16_t value = (resp[4] << 1*8) | (resp[5] << 0*8);
      angle = (carry*360.0)+((value*360.0)/65535.0);
    }

    return responseResult;
  }

  /**
   * Read the angle of the motor shaft.
   * command 3 : e0 36 tCHK
   * The motor rotates one circle, the corresponding angle value range is 0~65535.
   * Return e0 , angle(int32_t),rCHK.
   * 
   * @param return reference for shaft angle
   * @param timeout (optional, default = 0xFFFF) time in ms waiting on the angle via uart
   * @return true if readAngleShaft was successfully commanded
   */
  bool Servo42CDriver::uartCmdReadAngleShaft(double& angle, uint16_t timeout = 0xFFFF) {
    // if rpm is set to zero no action needs to be taken

    uint8_t cmdLen = 1;
    uint8_t cmd[cmdLen] = {this->UartCommands::ReadMotorAngle};
    this->writeCommand(this->uartId, cmd, cmdLen);

    uint8_t respLen = 4;
    uint8_t resp[respLen] = {0x00};
    bool responseResult = this->readResponse(this->uartId, resp, respLen, timeout);

    if (responseResult) {
      uint8_t uartId = resp[0];
      uint8_t checksum = resp[respLen-1];
      
      int32_t angleInt = (resp[0] << 3*8) | (resp[1] << 2*8) | (resp[2] << 1*8) | (resp[3] << 0*8);
      angle = (angleInt*360.0)/65535.0;
    }

    return responseResult;
  }

private:

  /**
   * 
   */
  void Servo42CDriver::printDebug(uint8_t byte, bool flush = false) {
    if(this->debug != nullptr) {
      if (flush) {
        this->debug->print("0x");
        this->debug->println(byte, HEX);
      }
      else {
        this->debug->print("0x");
        this->debug->print(byte, HEX);
        this->debug->print(" ");
      }
    }
  }

  /**
   * 
   */
  void Servo42CDriver::writeCommand(uint8_t id, uint8_t* cmd, uint8_t cmdLen) {

    // clear any unrelated response messages
    this->clearResponse();

    // select this uart to receive a message
    this->uart->listen();
    
    // generate checksum for command message
    uint8_t checkSum = id;
    for (uint8_t i=0; i<cmdLen; i++){
      checkSum += cmd[i];
    }
    checkSum &= 0xFF;

    this->uart->write(id);
    this->printDebug(id);
    for (uint8_t i=0; i<cmdLen; i++){
      this->printDebug(cmd[i]);
      this->uart->write(cmd[i]);
    }
    this->uart->write(checkSum);
    this->printDebug(checkSum, true);
  }

  /**
   * 
   */
  bool Servo42CDriver::readResponse(uint8_t id, uint8_t* resp, uint8_t respLen, uint16_t timeout = 500) {
    // len + 1b byte id and 1 byte checksum
    uint8_t bufLen = respLen + 2;
    uint8_t buf[bufLen] = {0x00};

    uint16_t timeoutTrigger = timeout;
    uint8_t receivedData = 0;

    while (receivedData < bufLen && timeoutTrigger>0) {
      if (this->uart->available()) {
        buf[receivedData++] = this->uart->read();
        this->printDebug(buf[receivedData-1], receivedData >= bufLen);

        // reset timeout if a byte was availabe
        timeoutTrigger = timeout;
      }
      else {
        // sleep and decrease timout var if no message was available
        delay(10);
        timeoutTrigger -= 10;
      }
    }

    // generate checksum for response message
    uint8_t checkSum = buf[0];
    for (uint8_t i=1; i<respLen+1; i++){
      checkSum += buf[i];
    }
    checkSum &= 0xFF;

    // copy message data into return buffer
    memcpy(resp, &buf[1], respLen);

    bool success = true;
    success &= timeoutTrigger>0;
    success &= id == buf[0];
    success &= checkSum == buf[respLen+1];
    return success;
  }

  void Servo42CDriver::clearResponse() {
    while (this->uart->available()) {
      this->uart->read();
    }
  }

};
