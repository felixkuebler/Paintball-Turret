
#include <math.h>
#include <stdint.h>

#include "BasicStepperDriver.h"

namespace SerialCommands {
  namespace Io {
    static constexpr uint8_t SetOutput = 1;
    static constexpr uint8_t SetInput = 2;
    static constexpr uint8_t DigitalRead = 3;
    static constexpr uint8_t DigitalWrite = 4;
    static constexpr uint8_t AnalogRead = 5;
    static constexpr uint8_t AnalogWrite = 6;
  }

  namespace Motor {
    namespace Pitch {
       static constexpr uint8_t WritePositionAbsolute = 10;
       static constexpr uint8_t WritePositionRelative = 11;
       static constexpr uint8_t WriteSpeed = 12;
       static constexpr uint8_t ReadPosition = 13;
    }
    namespace Yaw {
       static constexpr uint8_t WritePositionAbsolute = 20;
       static constexpr uint8_t WritePositionRelative = 21;
       static constexpr uint8_t WriteSpeed = 22;
       static constexpr uint8_t ReadPosition = 23;
    }
  }
}

namespace StepperMotorConfig {

    namespace Pitch {
      static constexpr uint8_t Steps = 200;
      static constexpr uint8_t MicroSteps = 8;

      static constexpr uint8_t Rpm = 120;
      static constexpr uint8_t MinRpm = 0;
      static constexpr uint8_t MaxRpm = 150;

      static constexpr uint8_t GearTeeth1 = 16;
      static constexpr uint8_t GearTeeth2 = 68;
      static constexpr float GearRation = static_cast<float>(GearTeeth2)/static_cast<float>(GearTeeth1);
      
      namespace Pins {
        static constexpr uint8_t Enable = 8;
        static constexpr uint8_t Dir = 7;
        static constexpr uint8_t Step = 6;
      }

      uint8_t currentPosition = 0;
    }

    namespace Yaw {
      static constexpr uint8_t Steps = 200;
      static constexpr uint8_t MicroSteps = 16;

      static constexpr uint8_t Rpm = 120;
      static constexpr uint8_t MinRpm = 0;
      static constexpr uint8_t MaxRpm = 150;

      static constexpr uint8_t GearTeeth1 = 16;
      static constexpr uint8_t GearTeeth2 = 156;
      static constexpr float GearRation = static_cast<float>(GearTeeth2)/static_cast<float>(GearTeeth1);
      
      namespace Pins {
        static constexpr uint8_t Enable = 13;
        static constexpr uint8_t Dir = 12;
        static constexpr uint8_t Step = 11;
      }

      uint8_t currentPosition = 0;
    }
}

BasicStepperDriver motorPitch(StepperMotorConfig::Pitch::Steps, StepperMotorConfig::Pitch::Pins::Dir, StepperMotorConfig::Pitch::Pins::Step,StepperMotorConfig::Pitch::Pins::Enable);
BasicStepperDriver motorYaw(StepperMotorConfig::Yaw::Steps, StepperMotorConfig::Yaw::Pins::Dir, StepperMotorConfig::Yaw::Pins::Step, StepperMotorConfig::Yaw::Pins::Enable);

uint8_t serialBuffer[255];
uint8_t buffPointer=0;
bool bufferComplete = false;

uint64_t preTime = 0; 


void setup() {
  Serial.begin(9600);

  motorPitch.begin(StepperMotorConfig::Pitch::Rpm, StepperMotorConfig::Pitch::MicroSteps);
  motorYaw.begin(StepperMotorConfig::Yaw::Rpm, StepperMotorConfig::Yaw::MicroSteps);

  motorPitch.setEnableActiveState(LOW);
  motorYaw.setEnableActiveState(LOW);

  motorPitch.enable();
  motorYaw.enable();
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

    if(serialBuffer[0] == SerialCommands::Io::DigitalWrite){
      // Toggle digital write
      // buff = [JOB_DIGITAL_WRITE, PIN_NUM, VALUE]
      // buff = [uint8_t, uint8_t, uint8_t] 
      digitalWrite(serialBuffer[1], serialBuffer[2]>0);
    }
    else if(serialBuffer[0] == SerialCommands::Io::DigitalRead){
      // Toggle digital read
      // buff = [JOB_DIGITAL_READ, PIN_NUM]
      // buff = [uint8_t, uint8_t] 
      // return = uint8_t
      Serial.write((char)((uint8_t)digitalRead(serialBuffer[1])));
    }
    else if(serialBuffer[0] == SerialCommands::Io::AnalogWrite){  
      // Toggle analog write
      // buff = [JOB_ANALOG_WRITE, PIN_NUM, VALUE]
      // buff = [uint8_t, uint8_t, uint8_t]     
      analogWrite(serialBuffer[1], serialBuffer[2]);
    }
    else if(serialBuffer[0] == SerialCommands::Io::AnalogRead){
      // Toggle analog read
      // buff = [JOB_ANALOG_READ, PIN_NUM]
      // buff = [uint8_t, uint8_t]   
      // return = uint16_t
      uint16_t value = analogRead(serialBuffer[1]);
      
      uint8_t outputBuffer[2];
      outputBuffer[0] = (value >> (8*0)) & 0xff;
      outputBuffer[1] = (value >> (8*1)) & 0xff;
      // send message
      Serial.write((char*)outputBuffer, sizeof(outputBuffer));
    }
    else if(serialBuffer[0] == SerialCommands::Io::SetOutput){
      // Set pin mode as output
      // buff = [JOB_SET_OUTPUT, PIN_NUM]
      // buff = [uint8_t, uint8_t]  
      pinMode(serialBuffer[1], OUTPUT);
    }
    else if(serialBuffer[0] == SerialCommands::Io::SetInput){
      // Set pin mode as input
      // buff = [JOB_SET_INPUT, PIN_NUM]
      // buff = [uint8_t, uint8_t] 
      pinMode(serialBuffer[1], INPUT);
    }
    else if(serialBuffer[0] == SerialCommands::Motor::Pitch::WritePositionAbsolute){
      // Set motor position or speed
      // buff = [JOB_MOTOR_WRITE_POSITION, DEGREE]
      // buff = [uint8_t, int32_t]
      /*
      if(motor){
        int32_t degree = ((int32_t) serialBuffer[1] << (8*0)) + ((int32_t) serialBuffer[2] <<  (8*1)) + ((int32_t) serialBuffer[3] <<  (8*2)) + ((int32_t) serialBuffer[4] <<  (8*3));
        degree*=motor->gearRatio;
        if (motor->invertDir) degree*= -1;

        motor->motionMode = 0;
        motor->setPosition = degree;
      }
      */
      int16_t degree = ((int16_t) serialBuffer[1] << (8*0)) | ((int16_t) serialBuffer[2] <<  (8*1)) | ((int16_t) serialBuffer[3] <<  (8*2)) | ((int16_t) serialBuffer[4] <<  (8*3));
      motorPitch.rotate(degree);
    }
    else if(serialBuffer[0] == SerialCommands::Motor::Yaw::WritePositionAbsolute){
      // Set motor position or speed
      // buff = [JOB_MOTOR_WRITE_POSITION, DEGREE]
      // buff = [uint8_t, int32_t] 
      /*
      if(motor){
        int32_t degree = ((int32_t) serialBuffer[1] << (8*0)) + ((int32_t) serialBuffer[2] <<  (8*1)) + ((int32_t) serialBuffer[3] <<  (8*2)) + ((int32_t) serialBuffer[4] <<  (8*3));
        degree*=motor->gearRatio;
        if (motor->invertDir) degree*= -1;

        motor->motionMode = 0;
        motor->setPosition = degree;
      }
      */
      int16_t degree = ((int16_t) serialBuffer[1] << (8*0)) | ((int16_t) serialBuffer[2] <<  (8*1)) | ((int16_t) serialBuffer[3] <<  (8*2)) | ((int16_t) serialBuffer[4] <<  (8*3));
      motorYaw.rotate(degree);
    }
    else if(serialBuffer[0] == SerialCommands::Motor::Pitch::WritePositionRelative){
      // Set motor position or speed
      // buff = [WritePositionRelative, Degree]
      // buff = [uint8_t, int32_t]
      int16_t degree = ((int16_t) serialBuffer[1] << (8*0)) | ((int16_t) serialBuffer[2] <<  (8*1)) | ((int16_t) serialBuffer[3] <<  (8*2)) | ((int16_t) serialBuffer[4] <<  (8*3));
      motorPitch.rotate(degree*StepperMotorConfig::Pitch::GearRation);
    }
    else if(serialBuffer[0] == SerialCommands::Motor::Yaw::WritePositionRelative){
      // Set motor position or speed
      // buff = [WritePositionRelative, Degree]
      // buff = [uint8_t, int32_t]
      int16_t degree = ((int16_t) serialBuffer[1] << (8*0)) | ((int16_t) serialBuffer[2] <<  (8*1)) | ((int16_t) serialBuffer[3] <<  (8*2)) | ((int16_t) serialBuffer[4] <<  (8*3));
      motorYaw.rotate(degree*StepperMotorConfig::Yaw::GearRation);
    }
    else if(serialBuffer[0] == SerialCommands::Motor::Pitch::WriteSpeed){
      // Set motor position or speed
      // buff = [JOB_MOTOR_WRITE_POSITION, SPEED]
      // buff = [uint8_t, int16_t] 
      /*
      if(motor){
        int16_t speed = ((int16_t) serialBuffer[1] << (8*0)) + ((int16_t) serialBuffer[2] <<  (8*1));
        if (motor->invertDir) speed*= -1;

        motor->motionMode = 1;
        motor->setSpeed = speed;
      }
      */
    }
    else if(serialBuffer[0] == SerialCommands::Motor::Yaw::WriteSpeed){
      // Set motor position or speed
      // buff = [JOB_MOTOR_WRITE_POSITION, SPEED]
      // buff = [uint8_t, int16_t] 
      /*
      if(motor){
        int16_t speed = ((int16_t) serialBuffer[1] << (8*0)) + ((int16_t) serialBuffer[2] <<  (8*1));
        if (motor->invertDir) speed*= -1;

        motor->motionMode = 1;
        motor->setSpeed = speed;
      }
      */
    }
    else if(serialBuffer[0] == SerialCommands::Motor::Pitch::ReadPosition){
      // Read motor position
      // buff = [JOB_MOTOR_READ_POSITION]
      // buff = [uint8_t] 
      // return = int32_t
      /*
      int32_t positionDeg = motor->encoderPos/(int32_t)motor->gearRatio;
      if (motor->invertDir) positionDeg*= -1;

      uint8_t outputBuffer[4];
      if(motor){
        outputBuffer[0] = (positionDeg >> (8*0)) & 0xff;
        outputBuffer[1] = (positionDeg >> (8*1)) & 0xff;
        outputBuffer[2] = (positionDeg >> (8*2)) & 0xff;
        outputBuffer[3] = (positionDeg >> (8*3)) & 0xff;
      }
      else {
        outputBuffer[0] = 0;
        outputBuffer[1] = 0;
        outputBuffer[2] = 0;
        outputBuffer[3] = 0;
      }
      // send message
      Serial.write((char*)outputBuffer, sizeof(outputBuffer));
      */
    }
    else if(serialBuffer[0] == SerialCommands::Motor::Yaw::ReadPosition){
      // Read motor position
      // buff = [JOB_MOTOR_READ_POSITION]
      // buff = [uint8_t] 
      // return = int32_t
      /*
      int32_t positionDeg = motor->encoderPos/(int32_t)motor->gearRatio;
      if (motor->invertDir) positionDeg*= -1;

      uint8_t outputBuffer[4];
      if(motor){
        outputBuffer[0] = (positionDeg >> (8*0)) & 0xff;
        outputBuffer[1] = (positionDeg >> (8*1)) & 0xff;
        outputBuffer[2] = (positionDeg >> (8*2)) & 0xff;
        outputBuffer[3] = (positionDeg >> (8*3)) & 0xff;
      }
      else {
        outputBuffer[0] = 0;
        outputBuffer[1] = 0;
        outputBuffer[2] = 0;
        outputBuffer[3] = 0;
      }
      // send message
      Serial.write((char*)outputBuffer, sizeof(outputBuffer));
      */
    }
    
    // clear buffer
    for (uint8_t i=0; i<255; i++) serialBuffer[i]=0;
  }  

/*
  if(motor){

    if (!motor->motionMode){  
      // Calculate time difference
      uint64_t currTime = millis();
      uint64_t dt = preTime - currTime;
      preTime = currTime;
  
      // Calculate error
      int32_t error = motor->setPosition - motor->encoderPos;
  
      // Integral term
      motor->integral += error*dt;
  
      // Calculate PID
      uint32_t output = abs((motor->Kp * error) + (motor->Ki * motor->integral) + (motor->Kd * ((error - motor->preError) / dt)));
      output = map(output, 1, 2^64, 1, 2^8);
      // TODO: conversion error
  
      // Restrict to max/min
      if( output > motor->maxSpeed ) output = motor->maxSpeed;
      if( output < motor->minSpeed ) output = motor->minSpeed;
  
      // Save error to previous error
      motor->preError = error;

      if(motor->encoderPos < ( motor->setPosition - motor->posThresh) && motor->encoderPos < ((int32_t)motor->maxRange * (int32_t)motor->gearRatio / 2)){
        motor->isAtTarget = false;
        analogWrite(motor->pin2, 0);
        analogWrite(motor->pin1, output);
      } else if(motor->encoderPos > (motor->setPosition + motor->posThresh) && motor->encoderPos > -((int32_t)motor->maxRange * (int32_t)motor->gearRatio / 2)){
        motor->isAtTarget = false;
        analogWrite(motor->pin1, 0);
        analogWrite(motor->pin2, output);
      }else{
        motor->isAtTarget = true;
        analogWrite(motor->pin2, 0);
        analogWrite(motor->pin1, 0);
      }
    }
    else
    {
      uint8_t output = abs(motor->setSpeed);
      output = map(output, 0, 100, 0, 255);

      if(0 < motor->setSpeed && motor->encoderPos < ((int32_t)motor->maxRange * (int32_t)motor->gearRatio / 2)){
        motor->isAtTarget = false;
        analogWrite(motor->pin2, 0);
        analogWrite(motor->pin1, output);
      } else if(0 > motor->setSpeed && motor->encoderPos > - ((int32_t)motor->maxRange * (int32_t)motor->gearRatio / 2)){
        motor->isAtTarget = false;
        analogWrite(motor->pin1, 0);
        analogWrite(motor->pin2, output);
      }else{
        motor->isAtTarget = true;
        analogWrite(motor->pin2, 0);
        analogWrite(motor->pin1, 0);
      }
    }
  }  

  */
  delay(1);       
}
