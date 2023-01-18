#include <Servo.h>
#include <EEPROM.h>
//constants for servos, LDRs, and Infrared, CALIBRATE for your settings 
const int LSTOPS = EEPROM.read(0); //96 on my servos
const int RSTOPS = EEPROM.read(2); //96 on my servos
const int SLOW = EEPROM.read(1)/2; //10 sets speed
const int MEDIUM = EEPROM.read(1); // 20
const int MDARK = 350; 
const int MLIGHT = 600; //eeprom 8
const int LDARK = 500;
const int LLIGHT = 600; //eeprom 4
const int RDARK = 475; 
const int RLIGHT = 650; //eeprom 12 
const int TURN = 16; //milliseconds to turn 1 degree
const int REFRESH = 38000; //refresh rate of the infrared transmitter
const int CM = 108; //milliseconds to go 1cm


//pin setup
const int Left = 5; //left wheel
const int Right = 6; //right wheel
const int GREEN = 7; //Use for LEFT turns
const int RED = 13; //Use for RIGHT turns
const int YELLOW = 12; //Use for Object Detection
Servo leftservo;
Servo rightservo;
#define PBR 2
#define PBL 4
#define INR 3
const int INFRA = 2;

void setLEDs(int green_state,int red_state,int yellow_state){
   digitalWrite(GREEN,green_state);
   digitalWrite(RED,red_state);
   digitalWrite(YELLOW,yellow_state);
}

//Servo functions
void setspeed(int set1, int set2){
  leftservo.write(LSTOPS+set1);
  rightservo.write(RSTOPS-set2);
}

void robotForward(int roboSpeed){
  setspeed(roboSpeed,roboSpeed);
  setLEDs(0,0,0);
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


void DRIVE(int distance){
  robotForward(MEDIUM);
  delay(20+(distance*CM));
  Stops();
}

void waitKey(int pin){
  while(digitalRead(pin) == HIGH){
    delay(20);
  }
  while(digitalRead(pin) == LOW){
    delay(20);
  }
}

boolean checkObject(){
  tone(INR,REFRESH);
   if(digitalRead(INFRA)==LOW){
      Serial.println("Object detected");
      noTone(INR);
      setLEDs(0,0,1);
      return true;
   } else{
      noTone(INR);
      return false;
   }
}

void avoid(){ //Only works when objects are on a straight piece of the path
  boolean object = true;
  int distanceTravelled = 0;
  DRIVE(15);
  //clear width of object
  while (object == true){
    ROTATE(90);
    DRIVE(10);
    distanceTravelled = distanceTravelled+10;
    ROTATE(-90);
    object = checkObject();
  }
  //make sure clear of object
  ROTATE(-5);
  if(checkObject == true){
    ROTATE(95);
    DRIVE(10);
    distanceTravelled = distanceTravelled+10;
    ROTATE(-90);
  }
  else{
    ROTATE(5);
  }
  //clear length of object
  object = true;
  while (object == true){
    DRIVE(10);
    ROTATE(-90);
    object = checkObject();
    if(object == true){
      ROTATE(90);
    }
  }
  DRIVE(distanceTravelled);
  ROTATE(90); //should now be back on the path
}

void setup() {
  // put your setup code here, to run once:
  pinMode(A0,INPUT); //Left LDR
  pinMode(A1,INPUT); //Mid LDR
  pinMode(A2,INPUT); //Right LDR
  pinMode(GREEN,OUTPUT); //Green LED = output
  pinMode(YELLOW,OUTPUT); //yellow LED = output
  pinMode(RED,OUTPUT); //red LED = output
  setLEDs(0,0,0);
  pinMode(INFRA,INPUT); //infrared sensor
  pinMode(INR,OUTPUT); //infrared transmitter
  pinMode(PBR, INPUT); //right button = input
  pinMode(PBL, INPUT); //left button = input
  rightservo.attach(Right); //sets up right wheel
  leftservo.attach(Left); //sets up left wheel
}

void loop() {
  // put your main code here, to run repeatedly:
  
  int LLDR = analogRead(A2);
  int MLDR = analogRead(A1);
  int RLDR = analogRead(A0);
  
  while (LLDR>LLIGHT and RLDR>RLIGHT){
    robotForward(MEDIUM);    
    RLDR = analogRead(A0);
    LLDR = analogRead(A2);
    if(checkObject()==true){
      avoid();
    }
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
