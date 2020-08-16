#include <Arduino.h>
#include "A4988.h"
//#include <BasicStepperDriver.h>
//#include <MultiDriver.h>
//#include <SyncDriver.h>

// using a 200-step motor (most common)
//for a nano on a CNC v4 board modified
 #define MOTOR_STEPS 200
 #define RPM 10
 
 //microsteppig pins
 #define MS1 12
 #define MS2 11
 #define MS3 10
 
 //movement pins
 #define DIR 4
 #define STEP 7
 
#define ENABLE 8 // optional (just delete ENABLE from everywhere if not used)
//#define SLEEP 13 // optional (just delete SLEEP from everywhere if not used)
 A4988 stepper(MOTOR_STEPS, DIR, STEP, ENABLE, MS1, MS2, MS3);
 //A4988 stepper(MOTOR_STEPS, DIR, STEP, SLEEP, MS1, MS2, MS3);


// Rotary Encoder Module connections
const int PinSW  = 3;    // Reading Push Button switch
const int PinCLK = 2;    // Generating interrupts using CLK signal
const int PinDT  = 5;    // Reading DT signal

//LED & shift register description
const int latchPin = 9;  // Pin connected to Pin 12 of 74HC595 (Latch)
const int dataPin  = 13;  // Pin connected to Pin 14 of 74HC595 (Data)
const int clockPin = 6;  // Pin connected to Pin 11 of 74HC595 (Clock)
const char common = 'a';    // common anode  or use const char common = 'c';    // common cathode
volatile byte bits;

int motorSteps = 5;
int motorDirection = 1;  // CW or CCW rotation

// Variables to debounce Rotary Encoder
long TimeOfLastDebounce = 0;
int DelayofDebounce = 0.01;
int DelayofHold = 10000;  // miliseconds to hold motor on after a move 10 sec = 10000

// Store previous Pins state
int PreviousCLK;   
int PreviousDATA;


void setup()
{
    //stepper
  stepper.begin(RPM);
    // if using enable/disable on ENABLE pin (active LOW) instead of SLEEP uncomment next line
    stepper.setEnableActiveState(LOW);
    stepper.enable();


Serial.begin(9600);  //diag

    //encoder
  // Put current pins state in variables
  PreviousCLK=digitalRead(PinCLK);
  PreviousDATA=digitalRead(PinDT);

  // Set the Switch pin to use Arduino PULLUP resistors
  pinMode(PinSW, INPUT_PULLUP);

    //LED & shift register
    pinMode(latchPin,OUTPUT);
    pinMode(dataPin,OUTPUT);
    pinMode(clockPin,OUTPUT);

    //LED
    bits = convertToBits(motorSteps) ;
    //bits = bits | B00000001;  // add decimal point
    updateLED(bits);    // display alphanumeric digit  
    //
    digitalWrite(ENABLE, HIGH);  //shut of motor holding. if on at this point.
}



void loop ()  {

  // If enough time has passed check the rotary encoder
  if ((millis() - TimeOfLastDebounce) > DelayofDebounce) {
    
    check_rotary();  // Rotary Encoder check routine below
    
    PreviousCLK=digitalRead(PinCLK);
    PreviousDATA=digitalRead(PinDT);
    
    TimeOfLastDebounce=millis();  // Set variable to current millis() timer
    //delay(250); 
  }

  //after 10 seconds release motor hold
  if (digitalRead(ENABLE) == LOW)  {
    if ((millis() - TimeOfLastDebounce) > DelayofHold) {

    //Serial.println("10 sec mark"); //prints 
    //Serial.println(motorSteps); //prints 
    
      //turn off DP 
      // generate characters to display for hexidecimal numbers 0 to F
      //displays the same number on the LED, just removes the decimal point as a unlocked motor indicator
      bits = convertToBits(motorSteps) ;
      bits = bits ^ B00000001;  // change decimal point
      updateLED(bits);    // display alphanumeric digit  
      digitalWrite(ENABLE, HIGH);  //shut of motor holding.
     }
   } 
   
   
  // Check if Rotary Encoder switch was pressed
  if (digitalRead(PinSW) == LOW) {
    motorSteps --;  
    if (motorSteps < 1) {
      motorSteps = 5; // Reset counter to 9
    }
    //LED
    bits = convertToBits(motorSteps) ;
    //bits = bits | B00000001;  // add decimal point
    updateLED(bits);    // display alphanumeric digit  
    
    Serial.println("button"); //prints 
    Serial.println(motorSteps); //prints 
    delay(200);
  }
    
}

//void MotorMove(int motorSteps, int motorDirection)  {
void MotorMove()  {
      /*
     * Microstepping mode: 1, 2, 4, 8, 16 or 32 (where supported by driver)
     * Mode 1 is full speed.
     * Mode 32 is 32 microsteps per step.
     * The motor should rotate just as fast (at the set RPM),
     * but movement precision is increased, which may become visually apparent at lower RPMs.
     */
    // In 1:8 microstepping mode, one revolution takes 8 times as many microsteps
    //stepper.setMicrostep(8);   // Set microstep mode to 1:8
    //stepper.move(8 * MOTOR_STEPS);    // forward revolution
    //stepper.move(-8 * MOTOR_STEPS);   // reverse revolution

    Serial.println("move sub"); //prints 
    Serial.println(motorSteps); //prints 
    Serial.println(motorDirection); //prints 
 
    if (digitalRead(ENABLE) == HIGH)  {
      digitalWrite(ENABLE, LOW); //turn on motor coils
      
       //  motor hold
      //displays the same number on the LED, just adds the decimal point as a locked motor indicator
      bits = convertToBits(motorSteps) ;
      bits = bits | B00000001;  // add decimal point
      updateLED(bits);    // display alphanumeric digit  
       
    } 

   
  switch (motorSteps) {
    case 1:
      // H H H 1/16 step
    stepper.setMicrostep(16);   // Set microstep mode to 1:16
    stepper.move(motorDirection);    
      break;
    case 2:
      // H H L 1/8 step
    stepper.setMicrostep(8);   // Set microstep mode to 1:8
    stepper.move(motorDirection);    
      break;
    case 3:
      // L H L 1/4 step
    stepper.setMicrostep(4);   // Set microstep mode to 1:4
    stepper.move(motorDirection);    
      break;
    case 4:
      // H L L 1/2 step
    stepper.setMicrostep(2);   // Set microstep mode to 1:2
    stepper.move(motorDirection);    
      break;
    case 5:
      // L L L full step
    stepper.setMicrostep(1);   // Set microstep mode to 1:1
    stepper.move(motorDirection);    
      break;
    case 6:
      // 2 step
    stepper.setMicrostep(1);   // Set microstep mode to 2:1
    stepper.move(motorDirection*2);    
      break;
    case 7:
      // 3 step
    stepper.setMicrostep(1);   // Set microstep mode to 3:1
    stepper.move(motorDirection*3);    
      break;
    case 8:
      // 4 step
    stepper.setMicrostep(1);   // Set microstep mode to 4:1
    stepper.move(motorDirection*4);    
      break;
    case 9:
      // 5 step
    stepper.setMicrostep(1);   // Set microstep mode to 5:1
    stepper.move(motorDirection*5);    
      break;
             
      }
} 


void updateLED(byte eightBits) {
  
  if (common == 'a') {                  // using a common anonde display?
    eightBits = eightBits ^ B11111111;  // then flip all bits using XOR 
  }
  digitalWrite(latchPin, LOW);  // prepare shift register for data
  shiftOut(dataPin, clockPin, LSBFIRST, eightBits); // send data
  digitalWrite(latchPin, HIGH); // update display
}

byte convertToBits(int someNumber) {
  switch (someNumber) {
    case 0:
      return B11111100;
      break;
    case 1:
      return B01100000;
      break;
    case 2:
      return B11011010;
      break;
    case 3:
      return B11110010;
      break;
    case 4:
      return B01100110;
      break;
    case 5:
      return B10110110;
      break;
    case 6:
      return B10111110;
      break;
    case 7:
      return B11100000;
      break;
    case 8:
      return B11111110;
      break;
    case 9:
      return B11110110;
      break;
    case 10:
      return B11101110; // Hexidecimal A
      break;
    case 11:
      return B00111110; // Hexidecimal B
      break;
    case 12:
      return B10011100; // Hexidecimal C or use for Centigrade
      break;
    case 13:
      return B01111010; // Hexidecimal D
      break;
    case 14:
      return B10011110; // Hexidecimal E
      break;
    case 15:
      return B10001110; // Hexidecimal F or use for Fahrenheit
      break;  
    default:
      return B10010010; // Error condition, displays three vertical bars
      break;   
  }
}


// Check if Rotary Encoder was moved
void check_rotary() {
 
 if ((PreviousCLK == 0) && (PreviousDATA == 1)) {
    if ((digitalRead(PinCLK) == 1) && (digitalRead(PinDT) == 0)) {
      motorDirection = 1;
      MotorMove();
    }
    if ((digitalRead(PinCLK) == 1) && (digitalRead(PinDT) == 1)) {
      motorDirection = -1;
      MotorMove();
    }
  }

if ((PreviousCLK == 1) && (PreviousDATA == 0)) {
    if ((digitalRead(PinCLK) == 0) && (digitalRead(PinDT) == 1)) {
      motorDirection = 1;
      MotorMove();
    }
    if ((digitalRead(PinCLK) == 0) && (digitalRead(PinDT) == 0)) {
      motorDirection = -1;
      MotorMove();
    }
  }

if ((PreviousCLK == 1) && (PreviousDATA == 1)) {
    if ((digitalRead(PinCLK) == 0) && (digitalRead(PinDT) == 1)) {
      motorDirection = 1;
      MotorMove();
    }
    if ((digitalRead(PinCLK) == 0) && (digitalRead(PinDT) == 0)) {
      motorDirection = -1;
      MotorMove();
    }
  }  

if ((PreviousCLK == 0) && (PreviousDATA == 0)) {
    if ((digitalRead(PinCLK) == 1) && (digitalRead(PinDT) == 0)) {
      motorDirection = 1;
      MotorMove();
    }
    if ((digitalRead(PinCLK) == 1) && (digitalRead(PinDT) == 1)) {
      motorDirection = -1;
      MotorMove();
    }
  }     
       
 }
