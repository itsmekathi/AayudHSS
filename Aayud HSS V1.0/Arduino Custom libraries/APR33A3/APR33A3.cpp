/*
  APR33A3.cpp - Library for playing and Recording audio 
  Created by Kathiravan B, April 25, 2015.
  Not Released to public domain.
*/

#include "APR33A3.h"
#include <stdio.h>
#include <string.h>
#include <inttypes.h>
#include "Arduino.h"

APR33A3::APR33A3(uint8_t busyPin, uint8_t rec, uint8_t m0, uint8_t m1, uint8_t m2, uint8_t m3)
{
	_busyPin = busyPin;
	_rec = rec;
	_m0 = m0;
	_m1 = m1;
	_m2 = m2;
	_m3 = m3;

	pinMode(_busyPin, INPUT);
	digitalWrite(_busyPin,HIGH);		//Turn on internal pullup
	pinMode(_rec,OUTPUT);
	pinMode(_m0,OUTPUT);
	pinMode(_m1,OUTPUT);
	pinMode(_m2,OUTPUT);
	pinMode(_m3,OUTPUT);
	pinInit();

}

void APR33A3::pinInit()
{
	digitalWrite(_rec,HIGH);
	digitalWrite(_m0,HIGH);
	digitalWrite(_m1,HIGH);
	digitalWrite(_m2,HIGH);
	digitalWrite(_m3,HIGH);
}



void APR33A3::playAudioTillBusy(uint8_t channel)
{
	digitalWrite(_rec,HIGH);		//Set rec pin HIGH so as to be in playing mode
	waitTillBusy();
	switch(channel)
	{
	case 0:
		digitalWrite(_m0,HIGH);
		delay(30);
		digitalWrite(_m0,LOW);
		delay(30);
		
		break;
	case 1:
		digitalWrite(_m1,HIGH);
		delay(30);
		digitalWrite(_m1,LOW);
		delay(30);
		
		break;
	case 2:
		digitalWrite(_m2,HIGH);
		delay(30);
		digitalWrite(_m2,LOW);
		delay(30);
		
		break;
	case 3:
		digitalWrite(_m3,HIGH);
		delay(30);
		digitalWrite(_m3,LOW);
		delay(30);
		
		break;
	}
	pinInit();
	waitTillBusy();
}

void APR33A3::waitTillBusy()
{
	   while ( !checkBusyAPR33a3());
}

int APR33A3::checkBusyAPR33a3()
{
	byte buttonState;
   delay(30);
  buttonState = digitalRead(_busyPin);
  if(buttonState == HIGH)
  {
    return 1;
  }
  else
 {
   return 0;
 }
}
//Function is used to Record audio 
void APR33A3::recordAudio(uint8_t channel)
{
	//Make rec in low to enter record mode
	digitalWrite(_rec,LOW);
	
	switch(channel)
	{
	case 0:
		digitalWrite(_m0,HIGH);
		delay(30);
		digitalWrite(_m0,LOW);
		break;
	case 1:
		digitalWrite(_m1,HIGH);
		delay(30);
		digitalWrite(_m1,LOW);
		break;
	case 2:
		digitalWrite(_m2,HIGH);
		delay(30);
		digitalWrite(_m2,LOW);
		break;
	case 3:
		digitalWrite(_m3,HIGH);
		delay(30);
		digitalWrite(_m3,LOW);
		break;
	}
}

void APR33A3::stopRecording()
{
	pinInit();
}
