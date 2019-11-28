const int LED1    = 1; //rood
const int LED2    = 2; //groen
const int SPEAKER = 3;
const int SERVO   = 4;
const int DMETER  = 5;
const int A       = 2; //Segment "a"
const int B       = 4; //Segment "b"
const int C       = 8; //Segment "c"
const int D       = 16; //Segment "d"
const int E       = 32; //Segment "e"
const int F       = 64; //Segment "f"
const int G       = 128; //Segment "g"
const int PINS[7] = {1,2,3,4,5,6,7};
int segments[2]; //array for all the numbers 0&1

void setup() {
  for(int i=0;i<7;i++)
  {
      pinMode(PINS[i], OUTPUT); 
  }
  write(1)
}

void write(int digit) {
    switch(digit)
    {
    case 0:
      write0();
        break;
    case 1:
      write1();
        break;
    }
}

void loop() {
  // put your main code here, to run repeatedly:

}
