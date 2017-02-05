/*
  APR33A3.h - Library for playing and recording audio
  Created by Kathiravan B.
  Not Released to public domain.
*/

#ifndef APR33A3_h
#define APR33A3_h

#include <inttypes.h>			//Needed to use uint8_t

class APR33A3
{
  public:
	  APR33A3(uint8_t busyPin,uint8_t rec, uint8_t m0, uint8_t m1, uint8_t m2, uint8_t m3);
	  //void playAudio(uint8_t channel);
	  void playAudioTillBusy(uint8_t channel);
	  void pinInit();
	  void recordAudio(uint8_t channel);
	  void stopRecording();
  private:
	  void waitTillBusy();
	  int checkBusyAPR33a3();
	  uint8_t _busyPin;
	  uint8_t _rec;
	  uint8_t _m0,_m1,_m2,_m3,_m4,_m5,_m6,_m7;

};

#endif

