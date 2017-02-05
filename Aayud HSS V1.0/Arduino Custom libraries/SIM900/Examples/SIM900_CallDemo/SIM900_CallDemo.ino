#include <SIM900.h>


SIM900 cell(2,3);

#define LED_PIN 13
void setup(){  
 Serial.begin(9600); 
 cell.begin(9600);
  cell.Verbose(true);
  cell.Boot(); 
  Serial.println("Making the call");
  cell.Call("+919738460098");
  delay(10000);
  Serial.println("Hanging up the call");
  cell.HangUp();
  delay(2000);
  pinMode(LED_PIN,OUTPUT);
}


void loop(){
  digitalWrite(LED_PIN,HIGH);
  delay(1000);
  digitalWrite(LED_PIN,LOW);
  delay(1000); 
}

