//Playing Audio demo

#include <APR33A3.h>

APR33A3 audio1(2,3,4,5,6,7);

void setup(){}
void loop(){
  audio1.playAudioTillBusy(0);
  delay(2000);
  audio1.playAudioTillBusy(1);
  delay(2000);
  audio1.playAudioTillBusy(2);
  delay(2000);
  audio1.playAudioTillBusy(3);
  delay(2000);
  audio1.playAudioTillBusy(0);
  delay(2000);
  audio1.playAudioTillBusy(1);
  delay(2000);
  audio1.playAudioTillBusy(2);
  delay(2000);
  audio1.playAudioTillBusy(3);
  delay(2000);
}
