
#include <math.h>
#include <stdint.h>

#define JOB_SET_OUTPUT 1
#define JOB_SET_INPUT 2
#define JOB_DIGITAL_READ 3
#define JOB_DIGITAL_WRITE 4
#define JOB_ANALOG_READ 5
#define JOB_ANALOG_WRITE 6

#define JOB_ATTACH_MOTOR 10
#define JOB_DETACH_MOTOR 11
#define JOB_MOTOR_WRITE 12
#define JOB_MOTOR_READ 13
#define JOB_MOTOR_AT_TARGET 14
#define JOB_MOTOR_OPTION 15

#define OPTION_MOTOR_RESET 1
#define OPTION_MOTOR_MIN_SPEED 2
#define OPTION_MOTOR_MAX_SPEED 3
#define OPTION_MOTOR_GEAR_RATIO 4
#define OPTION_MOTOR_INVERT_DIR 5
#define OPTION_MOTOR_MOTION_MODE 6
#define OPTION_MOTOR_RANGE 7


/*
*
*Motor with CPR-Encoder setup
*
*/
struct CPRMotor {

  // pin variables
  uint8_t pin1;
  uint8_t pin2;
  uint8_t encoderPinA;
  uint8_t encoderPinB;

  // position variables
  volatile int32_t encoderPos;
  int32_t setPosition;
  uint16_t maxRange; // maximum range in degree from -maxRange/2 to +maxRange/2

  uint8_t invertDir; // 0=non inverted, 1=inverted
  uint8_t motionMode; // 0=angular contolle, 1=speed controlle

  // position threshold
  uint16_t posThresh; // threashold for angular positioning
  uint8_t isAtTarget; // true if setPosiont = encoderPosition

  // speed variables
  uint8_t maxSpeed;
  uint8_t minSpeed;

  // ration between output shaft and encoder
  uint8_t gearRatio;

  // controller variables
  float Kp;
  float Ki;
  float Kd;
  float integral;
  float preError;
};

struct CPRMotor *motor; 


/*
*
*interrupt handler handles cpr encoder signal count
*
*/
void interruptHandler(){


  if (digitalRead(motor->encoderPinA) == digitalRead(motor->encoderPinB)) {
      motor->encoderPos--;   
  }
  else
  {
      motor->encoderPos++;    
  }
}


/*
*
*attach motor with cpr encoder at pin1, pin2 and interrupt pins 2,3
*
*/
void attachCPRMotor(uint8_t pin1, uint8_t pin2){
  
  motor =  (struct CPRMotor *)malloc(sizeof(struct CPRMotor));

  motor->pin1=pin1;
  motor->pin2=pin2;
  motor->encoderPinA=2;
  motor->encoderPinB=3;

  motor->encoderPos=0;
  motor->setPosition=0;
  //motor->maxRange = 9000; //for top
  motor->maxRange = 27000; // for bottom

  
  motor->invertDir = false;
  motor->motionMode = 0; // for angular controlle
  //motor->motionMode = 1; // for speed controlle

  motor->posThresh = 300;
  motor->isAtTarget = true;

  motor->maxSpeed=200;
  motor->minSpeed=100;

  //motor->gearRatio=5; // for top
  motor->gearRatio=10; // for bottom

  motor->Kp = 10;
  motor->Ki = 0;
  motor->Kd = 0;

  motor->integral = 0;
  motor->preError = 0;
  
  pinMode(motor->encoderPinA,INPUT);
  pinMode(motor->encoderPinB,INPUT);
  pinMode(motor->pin1,OUTPUT);
  pinMode(motor->pin2,OUTPUT);

  attachInterrupt(digitalPinToInterrupt(motor->encoderPinA), interruptHandler, CHANGE);
}


/*
*
*detach motor with cpr encoder
*
*/
void detachCPRMotor(){
  free(motor);
  detachInterrupt(0);
}




uint8_t serialBuffer[255];
uint8_t buffPointer=0;
bool bufferComplete = false;

uint64_t preTime = 0; 

/*
*
*setup for main loop :: only Serial setup, everything else on the go
*
*/
void setup() {
  Serial.begin(9600);
}

/*
*
*main loop
*
*/
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

    if(serialBuffer[0] == JOB_DIGITAL_WRITE){
      // Toggle digital write
      // buff = [JOB_DIGITAL_WRITE, PIN_NUM, VALUE]
      // buff = [uint8_t, uint8_t, uint8_t] 
      digitalWrite(serialBuffer[1], serialBuffer[2]>0);
    }
    else if(serialBuffer[0] == JOB_DIGITAL_READ){
      // Toggle digital read
      // buff = [JOB_DIGITAL_READ, PIN_NUM]
      // buff = [uint8_t, uint8_t] 
      // return = uint8_t
      Serial.write((char)((uint8_t)digitalRead(serialBuffer[1])));
    }
    else if(serialBuffer[0] == JOB_ANALOG_WRITE){  
      // Toggle analog write
      // buff = [JOB_ANALOG_WRITE, PIN_NUM, VALUE]
      // buff = [uint8_t, uint8_t, uint8_t]     
      analogWrite(serialBuffer[1], serialBuffer[2]);
    }
    else if(serialBuffer[0] == JOB_ANALOG_READ){
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
    else if(serialBuffer[0] == JOB_SET_OUTPUT){
      // Set pin mode as output
      // buff = [JOB_SET_OUTPUT, PIN_NUM]
      // buff = [uint8_t, uint8_t]  
      pinMode(serialBuffer[1], OUTPUT);
    }
    else if(serialBuffer[0] == JOB_SET_INPUT){
      // Set pin mode as input
      // buff = [JOB_SET_INPUT, PIN_NUM]
      // buff = [uint8_t, uint8_t] 
      pinMode(serialBuffer[1], INPUT);
    }
    else if(serialBuffer[0] == JOB_ATTACH_MOTOR){
      // Attach CPR-Motor at pin, pin2 and intterupt pin 2,3
      // buff = [JOB_ATTACH_MOTOR, PIN_NUM, PIN_NUM]
      // buff = [uint8_t, uint8_t, uint8_t] 
      attachCPRMotor(serialBuffer[1], serialBuffer[2]);
    }
    else if(serialBuffer[0] == JOB_DETACH_MOTOR){
      // Detach attached CPR-Motor
      // buff = [JOB_DETACH_MOTOR]
      // buff = [uint8_t]
      detachCPRMotor();
    }
    else if(serialBuffer[0] == JOB_MOTOR_OPTION){
      if(serialBuffer[1] == OPTION_MOTOR_MIN_SPEED){
        // Set motor min speed
        // buff = [JOB_MOTOR_OPTION, OPTION_MOTOR_MIN_SPEED, SPEED]
        // buff = [uint8_t, uint8_t, uint8_t] 
        if(motor){
          motor->minSpeed=serialBuffer[2];
        }
      }
      else if(serialBuffer[1] == OPTION_MOTOR_MAX_SPEED){
         // Set motor max speed
        // buff = [JOB_MOTOR_OPTION, OPTION_MOTOR_MAX_SPEED, SPEED]
        // buff = [uint8_t, uint8_t, uint8_t] 
        if(motor){
          motor->maxSpeed=serialBuffer[2];
        }
      }
      else if(serialBuffer[1] == OPTION_MOTOR_RESET){
        // Reset motor positions
        // buff = [JOB_MOTOR_OPTION, OPTION_MOTOR_RESET]
        // buff = [uint8_t, uint8_t] 
        if(motor){
            motor->encoderPos=0;
            motor->setPosition=0;
        }
      }
      else if(serialBuffer[1] == OPTION_MOTOR_GEAR_RATIO){
        // Set motor gear ratio
        // buff = [JOB_MOTOR_OPTION, OPTION_MOTOR_GEAR_RATION, GEAR_RATIO]
        // buff = [uint8_t, uint8_t, uint8_t] 
        if(motor){
            motor->gearRatio=serialBuffer[2];
        }
      }
      else if(serialBuffer[1] == OPTION_MOTOR_RANGE){
        // Set motor angular range
        // buff = [JOB_MOTOR_OPTION, OPTION_MOTOR_RANGE, RANGE]
        // buff = [uint8_t, uint8_t, uint16_t] 
        if(motor){
            motor->maxRange=((uint16_t) serialBuffer[2] << (8*0)) + ((uint16_t) serialBuffer[3] << (8*1));
        }
      }
      else if(serialBuffer[1] == OPTION_MOTOR_INVERT_DIR){
        // Invert motor direction
        // buff = [JOB_MOTOR_OPTION, OPTION_MOTOR_INVERT_DIR, INVERTED]
        // buff = [uint8_t, uint8_t, uint8_t] 
        if(motor){
            motor->invertDir=serialBuffer[2];
        }
      }
      else if(serialBuffer[1] == OPTION_MOTOR_MOTION_MODE){
        // Set mode of motion
        // buff = [JOB_MOTOR_OPTION, OPTION_MOTOR_MOTION_MODE, MODE]
        // buff = [uint8_t, uint8_t, uint8_t] 
        if(motor){
            motor->motionMode=serialBuffer[2];
        }
      }
    }
    else if(serialBuffer[0] == JOB_MOTOR_WRITE){
      // Set motor position or speed
      // buff = [JOB_MOTOR_WRITE, DEGREE_SPEED]
      // buff = [uint8_t, int32_t] 
      if(motor){
        int32_t degree_speed = ((int32_t) serialBuffer[1] << (8*0)) + ((int32_t) serialBuffer[2] <<  (8*1)) + ((int32_t) serialBuffer[3] <<  (8*2)) + ((int32_t) serialBuffer[4] <<  (8*3));

        if (motor->invertDir) degree_speed*= -1;
        if (!motor->motionMode) degree_speed*=motor->gearRatio;
        
        motor->setPosition = degree_speed;
      }
    }
    else if(serialBuffer[0] == JOB_MOTOR_READ){
      // Read motor position
      // buff = [JOB_MOTOR_READ]
      // buff = [uint8_t] 
      // return = int32_t
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
    }
    else if(serialBuffer[0] == JOB_MOTOR_AT_TARGET){
      // Check if motor is at target position
      // buff = [JOB_MOTOR_AT_TARGET]
      // buff = [uint8_t] 
      // return = uint8_t
      Serial.write((char)(motor->isAtTarget));
    }
    
    // clear buffer
    for (uint8_t i=0; i<255; i++) serialBuffer[i]=0;
  }  

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
      uint8_t output = abs(motor->setPosition);
      output = map(output, 0, 100, 0, 255);

      // Restrict to max
      if( output > motor->maxSpeed ) output = motor->maxSpeed;
      if( output < motor->minSpeed ) output = 0;

      if(0 < motor->setPosition && motor->encoderPos < ((int32_t)motor->maxRange * (int32_t)motor->gearRatio / 2)){
        motor->isAtTarget = false;
        analogWrite(motor->pin2, 0);
        analogWrite(motor->pin1, output);
      } else if(0 > motor->setPosition && motor->encoderPos > - ((int32_t)motor->maxRange * (int32_t)motor->gearRatio / 2)){
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
  delay(1);       
}
