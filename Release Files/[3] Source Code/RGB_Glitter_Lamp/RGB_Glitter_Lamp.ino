#include "AlaLedRgb.h"

// Arduino Light Animation (ALA) library - Copyright Bruno Portaluri - bportaluri.com
//
// Robust Rotary encoder reading - Copyright John Main - best-microcontroller-projects.com
//

#define CLK 11
#define DATA 12
#define Rpin 5
#define Gpin 3
#define Bpin 6
#define T A7 //Thermistor pin
#define Butt 2
#define Heat 9 //Heater pin 

int lim = 820; //Temp limiter
long k =0;

AlaLedRgb rgbLed;

// custom palette :         black     white     black     red       green     blue
AlaColor mycolors_[6] = { 0x000000, 0xFFFFFF, 0x000000, 0xFF0000, 0x00FF00, 0x0000FF };
AlaPalette mycolors = { 6, mycolors_ };


AlaSeq seq[] =
{
  { ALA_CYCLECOLORS, 3000, 3000, alaPalRgb },
  { ALA_OFF, 1000, 1000, alaPalNull },
  { ALA_FADECOLORSLOOP, 4000, 8000, alaPalRgb },
  { ALA_FADECOLORSLOOP, 500, 4000, mycolors },
  { ALA_OFF, 1000, 1000, alaPalNull },
  { ALA_FADECOLORS, 5000, 5000, mycolors },
  { ALA_OFF, 1000, 1000, alaPalNull },
  { ALA_ENDSEQ }
};

void setup() {
  pinMode(CLK, INPUT);
   pinMode(CLK, INPUT_PULLUP);
  pinMode(DATA, INPUT);
   pinMode(DATA, INPUT_PULLUP);
  pinMode(Butt, INPUT);
  pinMode(T, INPUT);
  pinMode(Heat, OUTPUT);
  pinMode(Rpin, OUTPUT);
  pinMode(Gpin, OUTPUT);
  pinMode(Bpin, OUTPUT);
  Serial.begin (9600);
  attachInterrupt (digitalPinToInterrupt(Butt),SIR,RISING);
   
  rgbLed.initPWM(Rpin, Gpin, Bpin); // initialize the RGB LED
  rgbLed.setBrightness(0x66FF44); // set the brightness of the LED calibrating white
  rgbLed.setAnimation(ALA_FADECOLORSLOOP, 10000, alaPalRgb);
}

static uint8_t prevNextCode = 0;
static uint16_t store=0;
static int8_t val;
static int8_t c =0;



void SIR() {
  while(1){
    k++;
    
      if(val=read_rotary() ){
      c +=val;
      Serial.print("val= ");Serial.println(val);
      delay(1);
        if(c>=3) c=0;
        if(c<=-1) c=2;
      Serial.println(c);
      k=0;  
      break;      
    }
      if(k>80000){
        k=0;
        break;
        }
   }
}





void loop() {
tempcheck();
//delay(1000);

  switch(c){
    case 0:
      digitalWrite(Rpin, HIGH);
      digitalWrite(Gpin, HIGH);
      digitalWrite(Bpin, HIGH);
   
    break;
   case 1:
      rgbLed.setAnimation(ALA_FADECOLORSLOOP, 10000, alaPalRgb);
      rgbLed.runAnimation();
   
    break;
   case 2:
      rgbLed.setAnimation(ALA_CYCLECOLORS, 10000, alaPalRgb);
      rgbLed.runAnimation();
   
      break;
    }
  
}

static int8_t read_rotary() { // A vald CW or CCW move returns 1, invalid returns 0.
  static int8_t rot_enc_table[] = {0,1,1,0,1,0,0,1,1,0,0,1,0,1,1,0};

  prevNextCode <<= 2;
  if (digitalRead(DATA)) prevNextCode |= 0x02;
  if (digitalRead(CLK)) prevNextCode |= 0x01;
  prevNextCode &= 0x0f;

   // If valid then store as 16 bit data.
   if  (rot_enc_table[prevNextCode] ) {
      store <<= 4;
      store |= prevNextCode;
      //if (store==0xd42b) return 1;
      //if (store==0xe817) return -1;
      if ((store&0xff)==0x2b) return -1;
      if ((store&0xff)==0x17) return 1;
   }
   return 0;
}

void tempcheck(){
  k++;
  if(analogRead(T)>=lim){
          if(k>100){
      digitalWrite(Heat,LOW);
      k=0;
      }
    }
    else {
      if(k>100){
        digitalWrite(Heat,HIGH);
        k=0;
      }
    }
  Serial.println(analogRead(T));  
}
  
