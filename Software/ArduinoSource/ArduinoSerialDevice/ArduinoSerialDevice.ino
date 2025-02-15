
#include <math.h>
#include <stdint.h>

#include <SoftwareSerial.h>

#include "Servo42CDriver.hpp"
#include "SerialCommands.hpp"
#include "MotorConfig.hpp"


namespace TiggerConfig {

  namespace Primary {
    namespace Pins {
      static constexpr uint8_t Enable = 3;
    }
  }
}

bool streamEnabled = true;
uint8_t streamSource = SerialCommands::Motor::Pitch::ReadPosition;

SoftwareSerial serialPitch(11, 12);
Servo42CDriver motorPitch(serialPitch);

SoftwareSerial serialYaw(6, 7);
Servo42CDriver motorYaw(serialYaw);

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
      // Toggle digital write
      // buff = [JOB_DIGITAL_WRITE, VALUE]
      // buff = [uint8_t, uint8_t] 
      if (serialBuffer[1]>0) {
        digitalWrite(TiggerConfig::Primary::Pins::Enable, HIGH);
      }
      else {
        digitalWrite(TiggerConfig::Primary::Pins::Enable, LOW);
      }
    }
    else if(serialBuffer[0] == SerialCommands::Trigger::Secondary) {
      // Toggle digital read
      // buff = [JOB_DIGITAL_READ, PIN_NUM]
      // buff = [uint8_t, uint8_t] 
    }
    else if(serialBuffer[0] == SerialCommands::Motor::Pitch::WritePositionAbsolute) {
      // Set motor position or speed
      // buff = [WritePositionAbsolute, Degree]
      // buff = [uint8_t, int32_t]
      int16_t degree = ((int16_t) serialBuffer[1] << (8*0)) | ((int16_t) serialBuffer[2] <<  (8*1)) | ((int16_t) serialBuffer[3] <<  (8*2)) | ((int16_t) serialBuffer[4] <<  (8*3));
      
      if (degree >= 360 || degree <= -360) {
        int16_t div = degree / 360;
        degree -= div*360;
      }

      motorPitch.moveAngleAbsolut(degree * MotorConfig::Pitch::GearRatio, MotorConfig::Pitch::Rpm * MotorConfig::Pitch::GearRatio, true); 
    }
    else if(serialBuffer[0] == SerialCommands::Motor::Yaw::WritePositionAbsolute) {
      // Set motor position or speed
      // buff = [WritePositionAbsolute, Degree]
      // buff = [uint8_t, int32_t]
      int16_t degree = ((int16_t) serialBuffer[1] << (8*0)) | ((int16_t) serialBuffer[2] <<  (8*1)) | ((int16_t) serialBuffer[3] <<  (8*2)) | ((int16_t) serialBuffer[4] <<  (8*3));

      motorYaw.moveAngleAbsolut(degree * MotorConfig::Yaw::GearRatio, MotorConfig::Yaw::Rpm * MotorConfig::Yaw::GearRatio, true); 
    }
    else if(serialBuffer[0] == SerialCommands::Motor::Pitch::WritePositionRelative) {
      // Set motor position or speed
      // buff = [WritePositionRelative, Degree]
      // buff = [uint8_t, int32_t]
      int16_t degree = ((int16_t) serialBuffer[1] << (8*0)) | ((int16_t) serialBuffer[2] <<  (8*1)) | ((int16_t) serialBuffer[3] <<  (8*2)) | ((int16_t) serialBuffer[4] <<  (8*3));

      motorPitch.moveAngleRelative(degree * MotorConfig::Pitch::GearRatio, MotorConfig::Pitch::Rpm * MotorConfig::Pitch::GearRatio, true); 
    }
    else if(serialBuffer[0] == SerialCommands::Motor::Yaw::WritePositionRelative) {
      // Set motor position or speed
      // buff = [WritePositionRelative, Degree]
      // buff = [uint8_t, int32_t]
      int32_t degree = ((int32_t) serialBuffer[1] << (8*0)) | ((int32_t) serialBuffer[2] <<  (8*1)) | ((int32_t) serialBuffer[3] <<  (8*2)) | ((int32_t) serialBuffer[4] <<  (8*3));

      motorYaw.moveAngleRelative(degree * MotorConfig::Yaw::GearRatio, MotorConfig::Yaw::Rpm * MotorConfig::Yaw::GearRatio, true); 
    }
    else if(serialBuffer[0] == SerialCommands::Motor::Pitch::WriteSpeed) {
      // Set motor position or speed
      // buff = [JOB_MOTOR_WRITE_POSITION, SPEED]
      // buff = [uint8_t, int16_t] 
      int16_t speed = ((int16_t) serialBuffer[1] << (8*0)) | ((int16_t) serialBuffer[2] <<  (8*1));

      speed = speed > 100 ? speed = 100 : speed;
      speed = speed < -100 ? speed = -100 : speed;
      
      // map to max range of speed
      int16_t rpm = map(speed, -100, 100, -1 * MotorConfig::Pitch::MaxRpm * MotorConfig::Pitch::GearRatio, MotorConfig::Pitch::MaxRpm * MotorConfig::Pitch::GearRatio);

      motorPitch.moveSpeed(rpm); 
    }
    else if(serialBuffer[0] == SerialCommands::Motor::Yaw::WriteSpeed) {
      // Set motor position or speed
      // buff = [JOB_MOTOR_WRITE_POSITION, SPEED]
      // buff = [uint8_t, int16_t] 
      int16_t speed = ((int16_t) serialBuffer[1] << (8*0)) | ((int16_t) serialBuffer[2] <<  (8*1));
      
      speed = speed > 100 ? speed = 100 : speed;
      speed = speed < -100 ? speed = -100 : speed;
      
      // map to max range of speed
      int16_t rpm = map(speed, -100, 100, -1 * MotorConfig::Yaw::MaxRpm * MotorConfig::Yaw::GearRatio, MotorConfig::Yaw::MaxRpm * MotorConfig::Yaw::GearRatio);

      motorYaw.moveSpeed(rpm); 
    }
    else if(serialBuffer[0] == SerialCommands::Motor::WriteSpeed) {
      // Set motor position or speed for both axis
      // buff = [JOB_MOTOR_WRITE_POSITION, SPEED]
      // buff = [uint8_t, int16_t] 
      int16_t speedPitch = ((int16_t) serialBuffer[1] << (8*0)) | ((int16_t) serialBuffer[2] <<  (8*1));
      int16_t speedYaw = ((int16_t) serialBuffer[3] << (8*0)) | ((int16_t) serialBuffer[4] <<  (8*1));

      speedPitch = speedPitch > 100 ? speedPitch = 100 : speedPitch;
      speedPitch = speedPitch < -100 ? speedPitch = -100 : speedPitch;
      
      // map to max range of speed
      int16_t rpmPitch = map(speedPitch, -100, 100, -1 * MotorConfig::Pitch::MaxRpm * MotorConfig::Pitch::GearRatio, MotorConfig::Pitch::MaxRpm * MotorConfig::Pitch::GearRatio);

      motorPitch.moveSpeed(rpmPitch); 

      speedYaw = speedYaw > 100 ? speedYaw = 100 : speedYaw;
      speedYaw = speedYaw < -100 ? speedYaw = -100 : speedYaw;
      
      // map to max range of speed
      int16_t rpmYaw = map(speedYaw, -100, 100, -1 * MotorConfig::Yaw::MaxRpm * MotorConfig::Yaw::GearRatio, MotorConfig::Yaw::MaxRpm * MotorConfig::Yaw::GearRatio);

      motorYaw.moveSpeed(rpmYaw); 
    }
    else if(serialBuffer[0] == SerialCommands::Motor::Pitch::ReadPosition) {
      // Read motor position
      // buff = [JOB_MOTOR_READ_POSITION]
      // buff = [uint8_t] 
      // return = int32_t

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
      // Read motor position
      // buff = [JOB_MOTOR_READ_POSITION]
      // buff = [uint8_t] 
      // return = int32_t

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
}
