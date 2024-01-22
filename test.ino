/**
  **************************
  * @file    vr_sample_control_led.ino
  * @author  JiapengLi
  * @brief   This file provides a demostration on 
              how to control led by using VoiceRecognitionModule
  **************************
  * @note:
        voice control led
  **************************
  * @section  HISTORY
    
    2013/06/13    Initial version.
  */
  
#include <SoftwareSerial.h>
#include "VoiceRecognitionV3.h"

/**        
  Connection
  Arduino    VoiceRecognitionModule
   2   ------->     TX
   3   ------->     RX
*/
VR myVR(2,3);    // 2:RX 3:TX, you can choose your favourite pins.

uint8_t s[7]; // save 
uint8_t buf[64];

int led = 13;
int switchPin = 5; 
// motor pin

#define in1 7
#define in2 8
#define in3 12
#define in4 13
#define rpwm 9
#define lpwm 11

#define stop    (0)
#define forward   (1) 
#define back   (2) 
#define right  (3) 
#define left  (4) 



/**
  @brief   Print signature, if the character is invisible, 
           print hexible value instead.
  @param   buf     --> command length
           len     --> number of parameters
*/
void printSignature(uint8_t *buf, int len)
{
  int i;
  for(i=0; i<len; i++){
    if(buf[i]>0x19 && buf[i]<0x7F){
      Serial.write(buf[i]);
    }
    else{
      Serial.print("[");
      Serial.print(buf[i], HEX);
      Serial.print("]");
    }
  }
}

/**
  @brief   Print signature, if the character is invisible, 
           print hexible value instead.
  @param   buf  -->  VR module return value when voice is recognized.
             buf[0]  -->  Group mode(FF: None Group, 0x8n: User, 0x0n:System
             buf[1]  -->  number of  which is recognized. 
             buf[2]  -->  Recognizer index(position) value of the recognized .
             buf[3]  -->  Signature length
             buf[4]~buf[n] --> Signature
*/
void printVR(uint8_t *buf)
{
  Serial.println("VR Index\tGroup\tNum\tSignature");

  Serial.print(buf[2], DEC);
  Serial.print("\t\t");

  if(buf[0] == 0xFF){
    Serial.print("NONE");
  }
  else if(buf[0]&0x80){
    Serial.print("UG ");
    Serial.print(buf[0]&(~0x80), DEC);
  }
  else{
    Serial.print("SG ");
    Serial.print(buf[0], DEC);
  }
  Serial.print("\t");

  Serial.print(buf[1], DEC);
  Serial.print("\t\t");
  if(buf[3]>0){
    printSignature(buf+4, buf[3]);
  }
  else{
    Serial.print("NONE");
  }
  Serial.println("\r\n");
}

void setup()
{
  /** initialize */
  myVR.begin(9600);
  
  Serial.begin(115200);
  Serial.println("Elechouse Voice Recognition V3 Module\r\nControl LED sample");
  
  pinMode(led, OUTPUT);
  pinMode(switchPin, INPUT_PULLUP); 
  // motor pin setup
  pinMode(in1, OUTPUT);
  pinMode(in2, OUTPUT);
  pinMode(rpwm, OUTPUT);
  pinMode(in3, OUTPUT);
  pinMode(in4, OUTPUT);
  pinMode(lpwm, OUTPUT);
  digitalWrite(in3,LOW);
  digitalWrite(in4,LOW);
  digitalWrite(in1,LOW);
  digitalWrite(in2,LOW);

    
  if(myVR.clear() == 0){
    Serial.println("Recognizer cleared.");
  }else{
    Serial.println("Not find VoiceRecognitionModule.");
    Serial.println("Please check connection and restart Arduino.");
    while(1);
  }
  
  if(myVR.load((uint8_t)forward) >= 0){
    Serial.println("on loaded");
  }
  
  if(myVR.load((uint8_t)back) >= 0){
    Serial.println("off loaded");
  }
  if(myVR.load((uint8_t)right) >= 0){
    Serial.println("right loaded");
  }
  
  if(myVR.load((uint8_t)left) >= 0){
    Serial.println("left loaded");
  }
  if(myVR.load((uint8_t)stop) >= 0){
    Serial.println("stop loaded");
  }
}

void loop()
{
 int state = (digitalRead(switchPin));
  Serial.print(state);
  Serial.print("        ");

  if(state == 0){
    hand_control(5);
    voice_control();
    Serial.println("voice controll activated");
    


    delay(1000);
}
else{
  hand_control(8);
  Serial.println("hand controll activated");
  
}
}
void voice_control(){
   int ret;
  ret = myVR.recognize(buf, 50);
  if(ret>0){
    switch(buf[1]){
      case forward:
        /** turn on LED */
        //digitalWrite(led, HIGH);
        digitalWrite(in1,HIGH);
        digitalWrite(in2,LOW);
        analogWrite(rpwm,100);
        digitalWrite(in3,HIGH);
        digitalWrite(in4,LOW);
        analogWrite(lpwm,100);
        break;
      case back:
        /* turn off LED*/
        //digitalWrite(led, LOW);
        digitalWrite(in1,LOW);
        digitalWrite(in2,HIGH);
        analogWrite(rpwm,100);
        digitalWrite(in3,LOW);
        digitalWrite(in4,HIGH);
        analogWrite(lpwm,100);
        break;
        
        case right:
        digitalWrite(in1,LOW);
        digitalWrite(in2,HIGH);
        analogWrite(rpwm,100);
        digitalWrite(in3,HIGH);
        digitalWrite(in4,LOW);
        analogWrite(lpwm,100);
        break;
        case left:
        digitalWrite(in1,HIGH);
        digitalWrite(in2,LOW);
        analogWrite(rpwm,100);
        digitalWrite(in3,LOW);
        digitalWrite(in4,HIGH);
        analogWrite(lpwm,100);
        break;
        case stop:
        digitalWrite(in1,LOW);
        digitalWrite(in2,LOW);
        analogWrite(rpwm,0);
        digitalWrite(in3,LOW);
        digitalWrite(in4,LOW);
        analogWrite(lpwm,0);
        break;
        
      default:
        Serial.println(" function undefined");
        break;
    }
    /** voice recognized */
    printVR(buf);
  }
  
}
void hand_control(int a){
   if(a==5){
        digitalWrite(in1,LOW);
        digitalWrite(in2,LOW);
        analogWrite(rpwm,0);
        digitalWrite(in3,LOW);
        digitalWrite(in4,LOW);
        analogWrite(lpwm,0);
        return;
   }
  if (Serial.available() > 0) {
    String receivedData = Serial.readStringUntil('\n');
    
    // Convert the received data to an integer
    int direction = receivedData.toInt();
    
    // Print the received analog value
    Serial.print("Received Analog Value: ");
    Serial.println(direction);
    switch(direction){
      case 1:
        /** turn on LED */
        //digitalWrite(led, HIGH);
        digitalWrite(in1,HIGH);
        digitalWrite(in2,LOW);
        analogWrite(rpwm,100);
        digitalWrite(in3,HIGH);
        digitalWrite(in4,LOW);
        analogWrite(lpwm,100);
        break;
      case 2:
        /* turn off LED*/
        //digitalWrite(led, LOW);
        digitalWrite(in1,LOW);
        digitalWrite(in2,HIGH);
        analogWrite(rpwm,100);
        digitalWrite(in3,LOW);
        digitalWrite(in4,HIGH);
        analogWrite(lpwm,100);
        break;
        
        case 3:
        digitalWrite(in1,LOW);
        digitalWrite(in2,HIGH);
        analogWrite(rpwm,100);
        digitalWrite(in3,HIGH);
        digitalWrite(in4,LOW);
        analogWrite(lpwm,100);
        break;
        case 4:
        digitalWrite(in1,HIGH);
        digitalWrite(in2,LOW);
        analogWrite(rpwm,100);
        digitalWrite(in3,LOW);
        digitalWrite(in4,HIGH);
        analogWrite(lpwm,100);
        break;
        case 5:
        digitalWrite(in1,LOW);
        digitalWrite(in2,LOW);
        analogWrite(rpwm,0);
        digitalWrite(in3,LOW);
        digitalWrite(in4,LOW);
        analogWrite(lpwm,0);
        break;
        
      default:
        Serial.println(" function undefined");
        break;
    }
  
  }
  }
