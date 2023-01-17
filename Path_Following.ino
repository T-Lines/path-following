#include <Servo.h>
#include <EEPROM.h>
//constants for servos, LDRs, and Infrared 
const int LSTOPS = EEPROM.read(0); //96
const int RSTOPS = EEPROM.read(2); //96
const int SLOW = EEPROM.read(1)/2; //10
const int MEDIUM = EEPROM.read(1); // 20
const int MDARK = 350; 
const int MLIGHT = 600; //eeprom 8
const int LDARK = 500;
const int LLIGHT = 600; //eeprom 4
const int RDARK = 475; 
const int RLIGHT = 650; //eeprom 12 
const int TURN = 16; //NOTE NEEDS CALIBRATION
const int REFRESH = 38000;

//pin setup
const int Left = 6;
const int Right = 5;
const int GREEN = 7; //Use for LEFT turns
const int RED = 13; //Use for RIGHT turns
const int YELLOW = 12; //Use for Object Detection
Servo leftservo;
Servo rightservo;
#define PBR 2
#define PBL 4
#define INR 3
const int INFRA = 2;

//repeatedly used functions
void setLEDs(int green_state,int red_state,int yellow_state){
    digitalWrite(GREEN,green_state);
    digitalWrite(RED,red_state);
    digitalWrite(YELLOW,yellow_state);
}
void setspeed(int set1, int set2){ //For driving forwards
  leftservo.write(LSTOPS+set1);
  rightservo.write(RSTOPS-set2);
}
void ROTATE(int angle){
  if(angle<0){ //Right turns
    setLEDs(1,0,0);
    setspeed(20,-20);
    delay(TURN*(-1*angle));
  }
  else if(angle>0){ //Left turns
    setLEDs(0,1,0);
    setspeed(-20,20);
    delay(TURN*angle);
  }
  Stops();
  delay(20);
  setLEDs(0,0,0);
}
void Stops(){
  setspeed(0,0);
}

void waitKey(int pin){
    while(digitalRead(pin) == HIGH){
        delay(20);
    }
    while(digitalRead(pin) == LOW){
      delay(20);
    }
}

void setup() {
  // put your setup code here, to run once:
    Serial.begin(9600);
    pinMode(A0,INPUT); //Right LDR
    pinMode(A1,INPUT); //Mid LDR
    pinMode(A2,INPUT); //Left LDR
    pinMode(INFRA,INPUT);
    pinMode(INR,OUTPUT);
    pinMode(PBL,INPUT);
    pinMode(GREEN,OUTPUT); //Green LED = output
    pinMode(YELLOW,OUTPUT); //yellow LED = output
    pinMode(RED,OUTPUT); //red LED = output
    rightservo.attach(Right);
    leftservo.attach(Left);
    Stops();
    setLEDs(0,0,0);
    waitKey(PBR);  
}

void loop() {
  // put your main code here, to run repeatedly:

      //reading LDRs
    int RLDR = analogRead(A0);
    int MLDR = analogRead(A1);
    int LLDR = analogRead(A2);
    while (LLDR>LLIGHT and RLDR>RLIGHT){
        setspeed(MEDIUM,MEDIUM);
        RLDR = analogRead(A0);
        LLDR = analogRead(A2);
    }
    if (RLDR<RDARK){
      while(MLDR>MLIGHT){
          ROTATE(-1);
          MLDR = analogRead(A1);
     }
        ROTATE(-20);
    }//end of if
    else if (LLDR<LDARK){
      while(MLDR>MLIGHT){
          ROTATE(1);
          MLDR = analogRead(A1);
    }
        ROTATE(20);
    } //end of else if
}
