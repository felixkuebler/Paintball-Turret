
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

    enum MotionMode: uint8_t {
      none = 0,
      position = 1,
      rpm = 2
    };

    uint8_t NextActionTimeout = 100;
    uint8_t MaxConsecutiveActions = 200*8;

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
        static constexpr uint8_t Enable = 8;
        static constexpr uint8_t Dir = 7;
        static constexpr uint8_t Step = 6;
      }

      int32_t targetPosition = 0;
      int32_t currentPosition = 0;

      MotionMode motionMode = MotionMode::none;
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
        static constexpr uint8_t Enable = 13;
        static constexpr uint8_t Dir = 12;
        static constexpr uint8_t Step = 11;
      }

      int32_t targetPosition = 0;
      int32_t currentPosition = 0;

      MotionMode motionMode = MotionMode::none;
    }
}

BasicStepperDriver motorPitch(StepperMotorConfig::Pitch::Steps, StepperMotorConfig::Pitch::Pins::Dir, StepperMotorConfig::Pitch::Pins::Step,StepperMotorConfig::Pitch::Pins::Enable);
BasicStepperDriver motorYaw(StepperMotorConfig::Yaw::Steps, StepperMotorConfig::Yaw::Pins::Dir, StepperMotorConfig::Yaw::Pins::Step, StepperMotorConfig::Yaw::Pins::Enable);

uint8_t serialBuffer[255];
uint8_t buffPointer=0;
bool bufferComplete = false;

void setup() {
  Serial.begin(9600);

  motorPitch.begin(StepperMotorConfig::Pitch::Rpm * StepperMotorConfig::Pitch::GearRatio, StepperMotorConfig::Pitch::MicroSteps);
  motorYaw.begin(StepperMotorConfig::Yaw::Rpm * StepperMotorConfig::Yaw::GearRatio, StepperMotorConfig::Yaw::MicroSteps);

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
      // buff = [WritePositionAbsolute, Degree]
      // buff = [uint8_t, int32_t]
      int16_t degree = ((int16_t) serialBuffer[1] << (8*0)) | ((int16_t) serialBuffer[2] <<  (8*1)) | ((int16_t) serialBuffer[3] <<  (8*2)) | ((int16_t) serialBuffer[4] <<  (8*3));
      
      if (degree >= 360 || degree <= -360) {
        int16_t div = degree / 360;
        degree -= div*360;
      }

      StepperMotorConfig::Pitch::targetPosition = degree;
      int16_t absDegree = degree - StepperMotorConfig::Pitch::currentPosition;

      motorPitch.setRPM(StepperMotorConfig::Pitch::Rpm * StepperMotorConfig::Pitch::GearRatio);
      motorPitch.startRotate(absDegree * StepperMotorConfig::Pitch::GearRatio);

      StepperMotorConfig::Pitch::motionMode = StepperMotorConfig::MotionMode::position;
    }
    else if(serialBuffer[0] == SerialCommands::Motor::Yaw::WritePositionAbsolute){
      // Set motor position or speed
      // buff = [WritePositionAbsolute, Degree]
      // buff = [uint8_t, int32_t] 
      int16_t degree = ((int16_t) serialBuffer[1] << (8*0)) | ((int16_t) serialBuffer[2] <<  (8*1)) | ((int16_t) serialBuffer[3] <<  (8*2)) | ((int16_t) serialBuffer[4] <<  (8*3));
      
      if (degree >= 360 || degree <= -360) {
        int16_t div = degree / 360;
        degree -= div*360;
      }

      StepperMotorConfig::Yaw::targetPosition = degree;
      int16_t absDegree = degree - StepperMotorConfig::Yaw::currentPosition;

      motorYaw.setRPM(StepperMotorConfig::Yaw::Rpm * StepperMotorConfig::Yaw::GearRatio);
      motorYaw.startRotate(absDegree * StepperMotorConfig::Yaw::GearRatio);

      StepperMotorConfig::Yaw::motionMode = StepperMotorConfig::MotionMode::position;
    }
    else if(serialBuffer[0] == SerialCommands::Motor::Pitch::WritePositionRelative){
      // Set motor position or speed
      // buff = [WritePositionRelative, Degree]
      // buff = [uint8_t, int32_t]
      int16_t degree = ((int16_t) serialBuffer[1] << (8*0)) | ((int16_t) serialBuffer[2] <<  (8*1)) | ((int16_t) serialBuffer[3] <<  (8*2)) | ((int16_t) serialBuffer[4] <<  (8*3));
      StepperMotorConfig::Pitch::targetPosition = StepperMotorConfig::Pitch::currentPosition + degree;

      motorPitch.setRPM(StepperMotorConfig::Pitch::Rpm * StepperMotorConfig::Pitch::GearRatio);
      motorPitch.startRotate(degree * StepperMotorConfig::Pitch::GearRatio);

      StepperMotorConfig::Pitch::motionMode = StepperMotorConfig::MotionMode::position;
    }
    else if(serialBuffer[0] == SerialCommands::Motor::Yaw::WritePositionRelative){
      // Set motor position or speed
      // buff = [WritePositionRelative, Degree]
      // buff = [uint8_t, int32_t]
      int32_t degree = ((int32_t) serialBuffer[1] << (8*0)) | ((int32_t) serialBuffer[2] <<  (8*1)) | ((int32_t) serialBuffer[3] <<  (8*2)) | ((int32_t) serialBuffer[4] <<  (8*3));
      StepperMotorConfig::Yaw::targetPosition = StepperMotorConfig::Yaw::currentPosition + degree;

      motorYaw.setRPM(StepperMotorConfig::Yaw::Rpm * StepperMotorConfig::Yaw::GearRatio);
      motorYaw.startRotate(degree * StepperMotorConfig::Yaw::GearRatio);

      StepperMotorConfig::Yaw::motionMode = StepperMotorConfig::MotionMode::position;
    }
    else if(serialBuffer[0] == SerialCommands::Motor::Pitch::WriteSpeed){
      // Set motor position or speed
      // buff = [JOB_MOTOR_WRITE_POSITION, SPEED]
      // buff = [uint8_t, int16_t] 
      int16_t speed = ((int16_t) serialBuffer[1] << (8*0)) | ((int16_t) serialBuffer[2] <<  (8*1));

      // extract direction from speed
      int16_t dir = speed < 0 ? -1 : 1;

      // limit speed to 0-100
      speed = abs(speed);
      speed = speed > 100 ? speed = 100 : speed;

      // calculate rpm from speed
      int16_t rpm = map(speed, 0, 100, 0, StepperMotorConfig::Pitch::MaxRpm * StepperMotorConfig::Pitch::GearRatio);

      if (rpm <= 0) {
        motorPitch.stop();
        StepperMotorConfig::Pitch::motionMode = StepperMotorConfig::MotionMode::none;
      }
      else {
        double degree = dir*90;
        StepperMotorConfig::Pitch::targetPosition = StepperMotorConfig::Pitch::currentPosition + degree;

        motorPitch.setRPM(rpm);
        motorPitch.startRotate(degree * StepperMotorConfig::Pitch::GearRatio);
        StepperMotorConfig::Pitch::motionMode = StepperMotorConfig::MotionMode::rpm;
      }
    }
    else if(serialBuffer[0] == SerialCommands::Motor::Yaw::WriteSpeed){
      // Set motor position or speed
      // buff = [JOB_MOTOR_WRITE_POSITION, SPEED]
      // buff = [uint8_t, int16_t] 
      int16_t speed = ((int16_t) serialBuffer[1] << (8*0)) | ((int16_t) serialBuffer[2] <<  (8*1));

      // extract direction from speed
      int16_t dir = speed < 0 ? -1 : 1;

      // limit speed to 0-100
      speed = abs(speed);
      speed = speed > 100 ? speed = 100 : speed;

      // calculate rpm from speed
      int16_t rpm = map(speed, 0, 100, 0, StepperMotorConfig::Yaw::MaxRpm * StepperMotorConfig::Yaw::GearRatio);

      if (rpm <= 0) {
        motorYaw.stop();
        StepperMotorConfig::Yaw::motionMode = StepperMotorConfig::MotionMode::none;
      }
      else {
        double degree = dir*90;
        StepperMotorConfig::Yaw::targetPosition = StepperMotorConfig::Yaw::currentPosition + degree;

        motorYaw.setRPM(rpm);
        motorYaw.startRotate(degree * StepperMotorConfig::Yaw::GearRatio);
        StepperMotorConfig::Yaw::motionMode = StepperMotorConfig::MotionMode::rpm;
      }
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

  uint8_t motorConsecutiveStep = 0;

  // controll motor steps and only do serial communication if there is time inbetween
  while ( !Serial.available() &&
          motorYaw.nextAction() + motorPitch.nextAction() < StepperMotorConfig::NextActionTimeout && 
          motorYaw.nextAction() + motorPitch.nextAction() > 0 && 
          motorConsecutiveStep++ <= StepperMotorConfig::MaxConsecutiveActions){}

  if (  StepperMotorConfig::Pitch::motionMode == StepperMotorConfig::MotionMode::rpm &&
        motorPitch.getStepsRemaining() < StepperMotorConfig::MaxConsecutiveActions) {
      motorPitch.startRotate(motorPitch.getDirection()*90*StepperMotorConfig::Pitch::GearRatio);
  }

  if (  StepperMotorConfig::Yaw::motionMode == StepperMotorConfig::MotionMode::rpm && 
        motorYaw.getStepsRemaining() < StepperMotorConfig::MaxConsecutiveActions) {
    motorYaw.startRotate(motorYaw.getDirection()*90*StepperMotorConfig::Yaw::GearRatio);
  }

  StepperMotorConfig::Pitch::currentPosition = StepperMotorConfig::Pitch::targetPosition - motorPitch.getDirection() * (motorPitch.getStepsRemaining() * 360 / (motorPitch.getSteps() * motorPitch.getMicrostep()))/StepperMotorConfig::Pitch::GearRatio;
  StepperMotorConfig::Yaw::currentPosition = StepperMotorConfig::Yaw::targetPosition - motorYaw.getDirection() * (motorYaw.getStepsRemaining() * 360 / (motorYaw.getSteps() * motorYaw.getMicrostep()))/StepperMotorConfig::Yaw::GearRatio;     
}
