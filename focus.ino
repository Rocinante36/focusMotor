#include <Stepper.h>
                    
volatile boolean turnDetected;  // need volatile for Interrupts
volatile boolean rotationDirection;  // CW or CCW rotation

// Rotary Encoder Module connections
const int pinSW  = 2;    // Reading Push Button switch
const int pinCLK = 3;    // Generating interrupts using CLK signal
const int pinDT  = 4;    // Reading DT signal

//LED & shift register description
const int latchPin = 5;  // Pin connected to Pin 12 of 74HC595 (Latch)
const int dataPin  = 6;  // Pin connected to Pin 14 of 74HC595 (Data)
const int clockPin = 7;  // Pin connected to Pin 11 of 74HC595 (Clock)
const char common = 'a';    // common anode  or use const char common = 'c';    // common cathode
volatile byte bits;

//stepper motor/driver decription
int in1Pin = 10;
int in2Pin = 11;
int in3Pin = 12;
int in4Pin = 13;
volatile int motorSteps = 9;
volatile int motorDirection = 1;
Stepper focusMotor(200, in1Pin, in2Pin, in3Pin, in4Pin);  

void setup()
{
    //stepper
    pinMode(in1Pin, OUTPUT);
    pinMode(in2Pin, OUTPUT);
    pinMode(in3Pin, OUTPUT);
    pinMode(in4Pin, OUTPUT);

    //encoder
    pinMode(pinCLK,INPUT);  // Trips when the encoder moves
    //digitalWrite(pinCLK, HIGH);
    attachInterrupt (1,rotaryDetect,CHANGE); // interrupt 1 always connected to pin 3 on Arduino UNO
    pinMode(pinDT,INPUT);   //  Tells the direction the encoder moved.
    //digitalWrite(pinDT, HIGH);
    pinMode(pinSW,INPUT);   // Encoder push button clicked
    //digitalWrite(pinSW, HIGH);
    attachInterrupt (0,pushButton,FALLING); // interrupt 0 always connected to pin 2 on Arduino UNO

    //LED & shift register
    pinMode(latchPin,OUTPUT);
    pinMode(dataPin,OUTPUT);
    pinMode(clockPin,OUTPUT);

    //LED
    bits = convertToBits(motorSteps) ;
    bits = bits | B00000001;  // add decimal point
    updateLED(bits);    // display alphanumeric digit


    
}

// Interrupt routine runs if CLK goes from HIGH to LOW (encoder turns)
//set a flag for main loops to turn the stepper
void rotaryDetect ()  {
    delay(4);  // delay for Debouncing
    if (digitalRead(pinCLK))
      rotationDirection = digitalRead(pinDT);
    else
    rotationDirection = !digitalRead(pinDT);
    turnDetected = true;
}

//Interrupt routine runs if SW goes from HIGH to LOW (encoder push button)
//subtract 1 from stepper speed
//turn off motor holding
//display spped on LED
void pushButton(){
    //lower speed or roll over to top speed if at minimum
    motorSteps--; 
    if (motorSteps == 0) {
        motorSteps = 9;
    }
    // output speed to LED
    bits = convertToBits(motorSteps) ;
    updateLED(bits);    // display alphanumeric digit

    //release motor holding - kill power
    digitalWrite(in1Pin,LOW);
    digitalWrite(in2Pin,LOW);
    digitalWrite(in3Pin,LOW);
    digitalWrite(in4Pin,LOW);
}

void loop ()  {
// Runs if rotation was detected
  if (turnDetected)  {
      turnDetected = false;  // do NOT repeat IF loop until new rotation detected

      // generate characters to display for hexidecimal numbers 0 to F
      //displays the same number on the LED, just adds the decimal point as a locked motor indicator
      bits = convertToBits(motorSteps) ;
       bits = bits | B00000001;  // add decimal point
       updateLED(bits);    // display alphanumeric digit        

       // Which direction to move Stepper motor
       if (rotationDirection) { // Move motor CCW
          motorDirection = 1;
       } else {
              motorDirection = -1;
          }
       //move stepper
       focusMotor.setSpeed(100);   
       focusMotor.step(motorSteps * motorDirection);
       //delay(200);  
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
