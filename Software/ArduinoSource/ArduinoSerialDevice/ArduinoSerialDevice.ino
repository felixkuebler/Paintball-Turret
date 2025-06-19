
#include <math.h>
#include <stdint.h>

#include <SoftwareSerial.h>

#include "Servo42CDriver.hpp"
#include "SerialCommands.hpp"
#include "MotorConfig.hpp"

#include "Endstop.hpp"


namespace TiggerConfig {

  namespace Primary {
    namespace Pins {
      static constexpr uint8_t Enable = 3;
    }
  }
}

SoftwareSerial serialPitch(MotorConfig::Pitch::Pins::Rx, MotorConfig::Pitch::Pins::Tx);
Servo42CDriver motorPitch(serialPitch);

SoftwareSerial serialYaw(MotorConfig::Yaw::Pins::Rx, MotorConfig::Yaw::Pins::Tx);
Servo42CDriver motorYaw(serialYaw);

Endstop endstopPitch(MotorConfig::Pitch::Pins::Endstop);
Endstop endstopYaw(MotorConfig::Yaw::Pins::Endstop, true);

bool streamEnabled = false;
uint8_t streamSource = SerialCommands::Motor::Pitch::ReadPosition;

uint8_t serialBuffer[255];
uint8_t buffPointer=0;
bool bufferComplete = false;

void setup() {
  Serial.begin(115200);

  serialYaw.begin(115200);
  serialPitch.begin(115200);

  pinMode(TiggerConfig::Primary::Pins::Enable, OUTPUT);
  
  motorPitch.setZero(500);
  motorYaw.setZero(500);
}

void loop() {

  while(Serial.available() > 0 && buffPointer<255 && !bufferComplete) {
    serialBuffer[buffPointer] = (uint8_t)Serial.read();
      
    if (serialBuffer[buffPointer]=='#' ) {
      bufferComplete=true;
    }

    buffPointer++;
  }
  
  if (bufferComplete) {

    // reset buffer for next serial read
    bufferComplete = false;
    buffPointer=0;

    if(serialBuffer[0] == SerialCommands::Trigger::Primary) {
      // Toggle primary trigger
      // serialBuffer = [uint8_t, uint8_t] 
      // serialBuffer = [SerialCommands::Trigger::Primary, Value]
      if (serialBuffer[1]>0) {
        digitalWrite(TiggerConfig::Primary::Pins::Enable, HIGH);
      }
      else {
        digitalWrite(TiggerConfig::Primary::Pins::Enable, LOW);
      }
    }
    else if(serialBuffer[0] == SerialCommands::Trigger::Secondary) {
      // Toggle secondary trigger
      // serialBuffer = [uint8_t, uint8_t] 
      // serialBuffer = [SerialCommands::Trigger::Secondary, Value]
    }
    else if(serialBuffer[0] == SerialCommands::Motor::Pitch::WritePositionAbsolute) {
      // Set motor pitch position absolute in relation to a given reference position
      // serialBuffer = [uint8_t, int32_t]
      // serialBuffer = [SerialCommands::Motor::Pitch::WritePositionAbsolute, Degree]
      int16_t degree = ((int16_t) serialBuffer[1] << (8*0)) | ((int16_t) serialBuffer[2] <<  (8*1)) | ((int16_t) serialBuffer[3] <<  (8*2)) | ((int16_t) serialBuffer[4] <<  (8*3));
      
      if (degree >= 360 || degree <= -360) {
        int16_t div = degree / 360;
        degree -= div*360;
      }

      motorPitch.moveAngleAbsolut(degree * MotorConfig::Pitch::GearRatio, MotorConfig::Pitch::Rpm * MotorConfig::Pitch::GearRatio, true); 
    }
    else if(serialBuffer[0] == SerialCommands::Motor::Yaw::WritePositionAbsolute) {
      // Set motor yaw position absolute in relation to a given reference position
      // serialBuffer = [uint8_t, int32_t]
      // serialBuffer = [SerialCommands::Motor::Yaw::WritePositionAbsolute, Degree]
      int16_t degree = ((int16_t) serialBuffer[1] << (8*0)) | ((int16_t) serialBuffer[2] <<  (8*1)) | ((int16_t) serialBuffer[3] <<  (8*2)) | ((int16_t) serialBuffer[4] <<  (8*3));

      motorYaw.moveAngleAbsolut(degree * MotorConfig::Yaw::GearRatio, MotorConfig::Yaw::Rpm * MotorConfig::Yaw::GearRatio, true); 
    }
    else if(serialBuffer[0] == SerialCommands::Motor::Pitch::WritePositionRelative) {
      // Set motor pitch position relative to the current position
      // serialBuffer = [uint8_t, int32_t]
      // serialBuffer = [SerialCommands::Motor::Pitch::WritePositionRelative, Degree]
      int16_t degree = ((int16_t) serialBuffer[1] << (8*0)) | ((int16_t) serialBuffer[2] <<  (8*1)) | ((int16_t) serialBuffer[3] <<  (8*2)) | ((int16_t) serialBuffer[4] <<  (8*3));

      motorPitch.moveAngleRelative(degree * MotorConfig::Pitch::GearRatio, MotorConfig::Pitch::Rpm * MotorConfig::Pitch::GearRatio, true); 
    }
    else if(serialBuffer[0] == SerialCommands::Motor::Yaw::WritePositionRelative) {
      // Set motor yaw position relative to the current position
      // serialBuffer = [uint8_t, int32_t]
      // serialBuffer = [SerialCommands::Motor::Yaw::WritePositionRelative, Degree]
      int32_t degree = ((int32_t) serialBuffer[1] << (8*0)) | ((int32_t) serialBuffer[2] <<  (8*1)) | ((int32_t) serialBuffer[3] <<  (8*2)) | ((int32_t) serialBuffer[4] <<  (8*3));

      motorYaw.moveAngleRelative(degree * MotorConfig::Yaw::GearRatio, MotorConfig::Yaw::Rpm * MotorConfig::Yaw::GearRatio, true); 
    }
    else if(serialBuffer[0] == SerialCommands::Motor::Pitch::WriteSpeed) {
      // Set motor pitch speed
      // serialBuffer = [uint8_t, int16_t] 
      // serialBuffer = [SerialCommands::Motor::Pitch::WriteSpeed, Speed]
      int16_t speed = ((int16_t) serialBuffer[1] << (8*0)) | ((int16_t) serialBuffer[2] <<  (8*1));

      speed = speed > 100 ? speed = 100 : speed;
      speed = speed < -100 ? speed = -100 : speed;
      
      // map to max range of speed
      int16_t rpm = map(speed, -100, 100, -1 * MotorConfig::Pitch::MaxRpm * MotorConfig::Pitch::GearRatio, MotorConfig::Pitch::MaxRpm * MotorConfig::Pitch::GearRatio);


      // check if an endstop was triggered
      // the rpm is invers to the angle -> angle > 0 & rmp > 0
      if (  !endstopPitch.isTriggered() ||
            (endstopPitch.isTriggered() && motorPitch.getCachedAngle() > 0 && rpm > 0) ||
            (endstopPitch.isTriggered() && motorPitch.getCachedAngle() < 0 && rpm < 0) ) {
        motorPitch.moveSpeed(rpm); 
      }
      else {
        motorPitch.stop(); 
      }
    }
    else if(serialBuffer[0] == SerialCommands::Motor::Yaw::WriteSpeed) {
      // Set motor yaw speed
      // serialBuffer = [uint8_t, int16_t] 
      // serialBuffer = [SerialCommands::Motor::Yaw::WriteSpeed, Speed]
      int16_t speed = ((int16_t) serialBuffer[1] << (8*0)) | ((int16_t) serialBuffer[2] <<  (8*1));
      
      speed = speed > 100 ? speed = 100 : speed;
      speed = speed < -100 ? speed = -100 : speed;
      
      // map to max range of speed
      int16_t rpm = map(speed, -100, 100, -1 * MotorConfig::Yaw::MaxRpm * MotorConfig::Yaw::GearRatio, MotorConfig::Yaw::MaxRpm * MotorConfig::Yaw::GearRatio);

      motorYaw.moveSpeed(rpm); 
    }
    else if(serialBuffer[0] == SerialCommands::Motor::Pitch::ReadPosition) {
      // Read motor pitch position
      // serialBuffer = [uint8_t] 
      // serialBuffer = [SerialCommands::Motor::Pitch::ReadPosition]
      // outputBuffer = [uint32_t]
      // outputBuffer = [Degree]
      double angle = 0;
      bool ret = motorPitch.readAngle(angle, 50);

      int32_t angleInt = angle / MotorConfig::Pitch::GearRatio;
      
      uint8_t outputBuffer[6];
      outputBuffer[0] = SerialCommands::SyncWord;
      outputBuffer[1] = ret;
      outputBuffer[2] = (angleInt >> (8*0)) & 0xff;
      outputBuffer[3] = (angleInt >> (8*1)) & 0xff;
      outputBuffer[4] = (angleInt >> (8*2)) & 0xff;
      outputBuffer[5] = (angleInt >> (8*3)) & 0xff;
  
      // send message
      Serial.write((char*)outputBuffer, sizeof(outputBuffer));
    }
    else if(serialBuffer[0] == SerialCommands::Motor::Yaw::ReadPosition){
      // Read motor yaw position
      // serialBuffer = [uint8_t] 
      // serialBuffer = [SerialCommands::Motor::Yaw::ReadPosition]
      // outputBuffer = [uint32_t]
      // outputBuffer = [Degree]
      double angle = 0;
      bool ret = motorYaw.readAngle(angle, 50);

      int32_t angleInt = angle / MotorConfig::Yaw::GearRatio;

      uint8_t outputBuffer[6];
      outputBuffer[0] = SerialCommands::SyncWord;
      outputBuffer[1] = ret;
      outputBuffer[2] = (angleInt >> (8*0)) & 0xff;
      outputBuffer[3] = (angleInt >> (8*1)) & 0xff;
      outputBuffer[4] = (angleInt >> (8*2)) & 0xff;
      outputBuffer[5] = (angleInt >> (8*3)) & 0xff;
  
      // send message
      Serial.write((char*)outputBuffer, sizeof(outputBuffer));
    }
    else if(serialBuffer[0] == SerialCommands::Motor::Pitch::Calibrate){
      // Enable/disable the streaming of data
      // serialBuffer = [uint8_t] 
      // serialBuffer = [SerialCommands::EnableDataStream]

      calibratePitch();
    }
    else if(serialBuffer[0] == SerialCommands::Motor::Yaw::Calibrate){
      // Trigger yaw calibration
      calibrateYaw();
    }
    else if(serialBuffer[0] == SerialCommands::Motor::WriteSpeed) {
      // Set motor speed for both axis
      // serialBuffer = [uint8_t, int16_t, int16_t] 
      // serialBuffer = [SerialCommands::Motor::WriteSpeed, SpeedPitch, SpeedYaw]
      int16_t speedPitch = ((int16_t) serialBuffer[1] << (8*0)) | ((int16_t) serialBuffer[2] <<  (8*1));
      int16_t speedYaw = ((int16_t) serialBuffer[3] << (8*0)) | ((int16_t) serialBuffer[4] <<  (8*1));

      speedPitch = speedPitch > 100 ? speedPitch = 100 : speedPitch;
      speedPitch = speedPitch < -100 ? speedPitch = -100 : speedPitch;
      
      // map to max range of speed
      int16_t rpmPitch = map(speedPitch, -100, 100, -1 * MotorConfig::Pitch::MaxRpm * MotorConfig::Pitch::GearRatio, MotorConfig::Pitch::MaxRpm * MotorConfig::Pitch::GearRatio);

      // check if an endstop was triggered
      // the rpm is invers to the angle -> angle > 0 & rmp > 0
      if (  !endstopPitch.isTriggered() ||
            (endstopPitch.isTriggered() && motorPitch.getCachedAngle() > 0 && rpmPitch > 0) ||
            (endstopPitch.isTriggered() && motorPitch.getCachedAngle() < 0 && rpmPitch < 0) ) {
        motorPitch.moveSpeed(rpmPitch); 
      }
      else {
        motorPitch.stop(); 
      }
      
      speedYaw = speedYaw > 100 ? speedYaw = 100 : speedYaw;
      speedYaw = speedYaw < -100 ? speedYaw = -100 : speedYaw;
      
      // map to max range of speed
      int16_t rpmYaw = map(speedYaw, -100, 100, -1 * MotorConfig::Yaw::MaxRpm * MotorConfig::Yaw::GearRatio, MotorConfig::Yaw::MaxRpm * MotorConfig::Yaw::GearRatio);

      motorYaw.moveSpeed(rpmYaw); 
    }
    else if(serialBuffer[0] == SerialCommands::EnableDataStream){
      // Enable/disable the streaming of data
      // serialBuffer = [uint8_t] 
      // serialBuffer = [SerialCommands::EnableDataStream]
      streamEnabled = (bool)serialBuffer[1] ;
    }
    
    // clear buffer
    for (uint8_t i=0; i<255; i++) serialBuffer[i]=0;
  }

  
  if (streamEnabled && Serial.available() <= 0) {

    double angle = 0;
    bool readSuccess = false;
    int32_t angleInt = 0;
    
    uint8_t outputBuffer[6];
    outputBuffer[0] = SerialCommands::SyncWord;
    
    if (streamSource == SerialCommands::Motor::Pitch::ReadPosition) {
      
      outputBuffer[1] = SerialCommands::Motor::Pitch::ReadPosition;
      
      readSuccess = motorPitch.readAngle(angle, 50);
      angleInt = angle / MotorConfig::Pitch::GearRatio;
      
      streamSource = SerialCommands::Motor::Yaw::ReadPosition;
    } 
    else if (streamSource == SerialCommands::Motor::Yaw::ReadPosition){

      outputBuffer[1] = SerialCommands::Motor::Yaw::ReadPosition;

      readSuccess = motorYaw.readAngle(angle, 50);
      angleInt = angle / MotorConfig::Yaw::GearRatio;
      
      streamSource = SerialCommands::Motor::Pitch::ReadPosition;
    }

    if (readSuccess) {
      outputBuffer[2] = (angleInt >> (8*0)) & 0xff;
      outputBuffer[3] = (angleInt >> (8*1)) & 0xff;
      outputBuffer[4] = (angleInt >> (8*2)) & 0xff;
      outputBuffer[5] = (angleInt >> (8*3)) & 0xff;

      // send message
      Serial.write((char*)outputBuffer, sizeof(outputBuffer));
    }
  }

  endstopPitch.read();
  if (endstopPitch.isStateChange() && endstopPitch.isTriggered()) {
    motorPitch.stop(); 
  }
}

void calibratePitch() {

  const int16_t calibrationSpeed = 50;
  double angleMin = 0;
  double angleMax = 0;

  bool readSuccess = true;
  
  // map to max range of speed
  int16_t rpm = map(calibrationSpeed, -100, 100, -1 * MotorConfig::Pitch::MaxRpm * MotorConfig::Pitch::GearRatio, MotorConfig::Pitch::MaxRpm * MotorConfig::Pitch::GearRatio);

  // start moving int max direction
  motorPitch.moveSpeed(rpm);

  // read endstop until it is triggered
  // this requires the turret to be in a neutral position at start
  endstopPitch.read();
  while(!endstopPitch.isTriggered()) {
    endstopPitch.read();
  }

  // stop the motion as soon as max endstop was triggered
  motorPitch.stop();

  // delay is required before the read command
  // otherwise the read will fail because the motor controller is still buisy with stopping
  delay(500);

  // read the max endstop position
  readSuccess &= motorPitch.readAngle(angleMax, 500);
  angleMax /=  MotorConfig::Pitch::GearRatio;

  // start moving into min direction
  motorPitch.moveSpeed(-rpm);

  // read endstop until it is not triggered anymore
  endstopPitch.read();
  while(endstopPitch.isTriggered()) {
    endstopPitch.read();
  }
  // read endstop until its triggered
  while(!endstopPitch.isTriggered()) {
    endstopPitch.read();
  }

  // stop the motion as soon as minmax endstop was triggered
  motorPitch.stop();

  // delay is required before the read command
  // otherwise the read will fail because the motor controller is still buisy with stopping
  delay(500);

  // read min endstop position
  readSuccess &= motorPitch.readAngle(angleMin, 500);
  angleMin /=  MotorConfig::Pitch::GearRatio;

  // calculate center from both endstop positions
  double center = (angleMax-angleMin)/2;

  // only set new zero position if the calculation of the position wasnt currupted by faulty readouts
  if (readSuccess) {
    // moce the pitch axis to the center
    // this requires the pitch axis to behave symmetrically
    motorPitch.moveAngleRelative(center * MotorConfig::Pitch::GearRatio, MotorConfig::Pitch::Rpm * MotorConfig::Pitch::GearRatio, 500); 
    
    // set zero position to this location
    motorPitch.setZero(500);
  }
  
}

void calibrateYaw() {
  
  const int16_t calibrationSpeed = 50;

  bool readSuccess = true;
  
  // map to max range of speed
  int16_t rpm = map(calibrationSpeed, -100, 100, -1 * MotorConfig::Yaw::MaxRpm * MotorConfig::Yaw::GearRatio, MotorConfig::Yaw::MaxRpm * MotorConfig::Yaw::GearRatio);

  // start moving int max direction
  motorYaw.moveSpeed(rpm);

  // read endstop until it is triggered
  // this requires the turret to be in a neutral position at start
  endstopYaw.read();
  while(!endstopYaw.isTriggered()) {
    endstopYaw.read();
  }

  // stop the motion as soon as endstop was triggered
  motorYaw.stop();

  // delay is required before the read command
  // otherwise the read will fail because the motor controller is still buisy with stopping
  delay(500);

    // only set new zero position if the calculation of the position wasnt currupted by faulty readouts
  if (readSuccess) {

    // set zero position to this location
    motorYaw.setZero(500);
  }
}
