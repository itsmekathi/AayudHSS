#include <Arduino.h>

void blinkPin(int i,int duration)
{
  
  digitalWrite(i,HIGH);
  delay(duration * 1000);
  digitalWrite(i,LOW);
  delay(duration *1000);
}

