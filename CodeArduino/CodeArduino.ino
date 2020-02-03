#include <Servo.h> //adds servo libary
#include <SPI.h>
#include <MFRC522.h> //adds RFID libary
#include <Wire.h>

#define SS_PIN 10
#define RST_PIN 9
MFRC522 mfrc522(SS_PIN, RST_PIN);   // Create MFRC522 instance.

const int DISPLAY_SHIFT = A0; //outputs for 74595 shift register
const int DISPLAY_LATCH = A1; 
const int DISPLAY_DATA  = A2; 
const int FREE_1        = A3; 
const int IN_A0         = A5; //analog input
const int LED_GREEN     = 0; //output
const int LED_RED       = 1; //output
const int BUTTON        = 2; //input
const int SPEAKER       = 3; //output
const int SERVO         = 6; //output
const int DISTANCEMETER = 7; //input
const int IN_D0         = 8; //digital input (0 or 1)


int cm = 0; //value of the distance at the beginning, because its unknown
Servo servo;

const int A =  64; //Segment "a"
const int B =  32; //Segment "b"
const int C =   2; //Segment "c"
const int D =   1; //Segment "d"
const int E =  16; //Segment "e"
const int F = 128; //Segment "f"
const int G =   4; //Segment "g"
int segments[11]; //array for all the numbers 0-9


void setup() {
  Serial.begin(9600);   //initiate a serial communication
  SPI.begin();      //initiate  SPI bus
  mfrc522.PCD_Init();   //initiate MFRC522
  
  pinMode(DISPLAY_SHIFT, OUTPUT);
  pinMode(DISPLAY_LATCH, OUTPUT);
  pinMode(DISPLAY_DATA, OUTPUT);
  pinMode (IN_A0, INPUT);
  pinMode(LED_GREEN, OUTPUT);
  pinMode(LED_RED, OUTPUT);
  pinMode(BUTTON, INPUT);
  pinMode(SPEAKER, OUTPUT); //output for sound
  servo.attach(SERVO); //attaches the servo to pin 6
  pinMode (IN_D0, INPUT);

  //pinMode(DIGIT_3, OUTPUT);
  
  servo.write(7); //writes the position on the servo
  Serial.println("Present your card to the reader...");
  Serial.println();


  //~ gives opposite (CA display)
  segments[0] = ~(A + B + C + D + E + F    ); //writes 0 puts A,B,C,D,E,F   LOW
  segments[1] = ~(    B + C                ); //writes 1 puts   B,C         LOW
  segments[2] = ~(A + B +     D + E +     G); //writes 2 puts A,B,  D,E,  G LOW
  segments[3] = ~(A + B + C + D +         G); //writes 3 puts A,B,C,D,    G LOW
  segments[4] = ~(    B + C +         F + G); //writes 4 puts   B,C,    F,G LOW
  segments[5] = ~(A +     C + D +     F + G); //writes 5 puts A,  C,D,  F,G LOW
  segments[6] = ~(A +     C + D + E + F + G); //writes 6 puts A,  C,D,E,F,G LOW
  segments[7] = ~(A + B + C                ); //writes 7 puts A,B,C         LOW
  segments[8] = ~(A + B + C + D + E + F + G); //writes 8 puts A,B,C,D,E,F,G LOW
  segments[9] = ~(A + B + C + D +     F + G); //writes 9 puts A,B,C,D,  F,G LOW
}


int n = 2; //spots left
//getal shows number n on the 7segment display
void getal(int n) {
  if (n > 9 || n < 0){
    shift(~segments[8]);
    }
  if (n <= 9){
    shift(segments[n]);
  }
}

void shift(byte d) {
  digitalWrite(DISPLAY_LATCH, LOW);
  digitalWrite(DISPLAY_SHIFT, LOW);

  for (byte i = 0; i < 8; i++) {
    bool b = (d & 1) > 0; 
    digitalWrite(DISPLAY_DATA, b ? HIGH : LOW);
    digitalWrite(DISPLAY_SHIFT, HIGH);
    digitalWrite(DISPLAY_SHIFT, LOW);
    d >>= 1;
  }
  digitalWrite(DISPLAY_LATCH, HIGH);
  digitalWrite(DISPLAY_LATCH, LOW);
} 

//function to play sound
void playSound(){
  tone(SPEAKER, 523, 1000); //play tone 60 (C5 = 523 Hz), from tinkercad
}

//function to turn the servo up, so the car can pass
void turnServoUp(){
  delay(2000);
  //turns servo from 0 to 90 degrees in one step
  servo.write(90); //writes the postition on the servo
  playSound();
}

//function to turn the servo down, so no more cars can pass
void turnServoDown(){
  delay(2000);
  //turns servo from 90 to 0 degrees in one step
  servo.write(7); //writes the position on the servo
  playSound();
}

//reads the sound distance in microsecnds and returns it
long readDistance(int triggerPin, int echoPin)
{
  pinMode(triggerPin, OUTPUT);  //clear the trigger
  digitalWrite(triggerPin, LOW);
  delayMicroseconds(10);
  //sets the trigger pin to HIGH state for 10 microseconds
  digitalWrite(triggerPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(triggerPin, LOW);
  pinMode(echoPin, INPUT);
  //reads the echo pin, and returns sound wave travel time in microseconds
  return pulseIn(echoPin, HIGH);
}

//calculates the distance in cm
void calculateDistance(){
  cm = 0.01723 * readDistance(11, 11); //got the constant from tinkercad, the 11, 11 are the pins that are used for the inputs
}


bool accesID(){
  //Show UID on serial monitor
  Serial.print("UID tag :");
  String content= "";
  byte letter;
  for (byte i = 0; i < mfrc522.uid.size; i++) 
  {
     Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
     Serial.print(mfrc522.uid.uidByte[i], HEX);
     content.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
     content.concat(String(mfrc522.uid.uidByte[i], HEX));
  }
  Serial.println();
  Serial.print("Message : ");
  content.toUpperCase();
  //when the ID card has access
  if (content.substring(1) == "B7 AD 47 52") //the UID of the card/cards that you want to give access
  { 
    return (content.substring(1) == "B7 AD 47 52");
  } else {
    return (content.substring(1) != "B7 AD 47 52");
  }
}

bool buttonPress() {
  bool press;
  int buttonState;
  //static keeps the value between calls and not changing it to LOW every loop
  static int prevButtonState = LOW; //makes the variable prevButtonState start at LOW
  buttonState = LOW;
  buttonState = digitalRead(BUTTON); //makes the variable buttonState what has the value of the input of pin 13 (HIGH/LOW)
  press = (buttonState == LOW && prevButtonState == LOW); //gives a true or false from the 'if' statement to press
  prevButtonState = buttonState; //changes the prevButtonState to the buttonState
  return press; //returns true or false
}

int value_A0;
bool value_D0;
void lightDetector(){
  value_A0 = analogRead(IN_A0); // reads the analog input from the IR distance sensor
  value_D0 = digitalRead(IN_D0);// reads the digital input from the IR distance sensor
  
  Serial.print("A0:");
  Serial.print(value_A0); // prints analog value on the LCD module
  Serial.println();
  Serial.print("D0:");
  Serial.print(value_D0); // prints digital value on the LCD module
  Serial.println();
  delay(1000);
}

/*
void switchDigit(){
  if (car == parked){
      digit -= 1;
    }
}
*/
void loop() {
  getal(n);
  // Look for new cards
  if ( ! mfrc522.PICC_IsNewCardPresent()) 
  {
    return;
  }
  // Select one of the cards
  if ( ! mfrc522.PICC_ReadCardSerial()) 
  {
    return;
  }
  lightDetector();
  if (accesID())
    {
      //the ID card has access
      Serial.println("Authorized access");
      Serial.println();
      turnServoUp();
      delay(3000); //needs to be changed with IR
      if(value_D0 == 1){
        turnServoDown();
      }
      n -= 1;
    }
   else {
      //the ID card doesn't have access
      Serial.println("Access denied");
      Serial.println();
      turnServoDown();
    }
  if (n <= 0)
  {
    digitalWrite(LED_RED, HIGH);
    Serial.println("Parking lot is full");  
    Serial.println();
  }
    
}
