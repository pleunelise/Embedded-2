#include <Wire.h>
const int IN_A0 = A5; //analog input
const int IN_D0 = 8; //digital input (0 or 1)

void setup() {
  Serial.begin(9600);   //start up the serial communication
  pinMode (IN_A0, INPUT);
  pinMode (IN_D0, INPUT);
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

void loop() {
  lightDetector();
}


// adjust the blue box on the back of sensor to get other value
// first test with car before changing value
