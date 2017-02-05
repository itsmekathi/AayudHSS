#include <APR33A3.h>
#include <avr/pgmspace.h>
#include <EEPROM.h>
#include <gprs.h>
#include <SoftwareSerial.h>
#include "definitions.h"
#include "setParameters.cpp"


// gprs(baudrate, tx, rx) it is one to one mapping for tx and rx pin from arduino to GSM modem
GPRS gprs(9600,8,7);
// APR33A3  busy,rec,m0,m1,m2,m3
APR33A3 audio1(A0,A2,A1,A3,A4,A5);


volatile int intVal = 0;        // used in interrupt service routines


void setup() {
  Serial.begin(9600);
  while(!Serial);
  Serial.println("GPRS - Call up demo");
  gprs.preInit();//power on SIM800
  delay(1000);
  while(0 !=gprs.init()) { //gprs init
      delay(1000);
      Serial.print("init error\r\n");
  }  
  Serial.println("Init success, start to call...");

   // If there was any data sent of serial port enter programming mode
  if(Serial.available())
  {
    SerialPrintFromPROGMEM(ENTERING_PROGRAMMING_MODE);
    SetParameters();
  }

  SerialPrintFromPROGMEM(ENTERING_MAIN_CODE);

  // Make OUT0 and OUT1 as output and make them low
  pinMode(OUT0,OUTPUT);
  pinMode(OUT1, OUTPUT);
  digitalWrite(OUT0,LOW);
  digitalWrite(OUT1,LOW);

  
  // Make the interrupt pins as input and enable the pullup resistors
  pinMode(2,INPUT);
  pinMode(3,INPUT);
  digitalWrite(2,HIGH);
  digitalWrite(3,HIGH);
  attachInterrupt(0, setVal, RISING);
  attachInterrupt(1, resetVal, RISING);
  pinMode(STATUS_LED,OUTPUT);
  digitalWrite(STATUS_LED,HIGH);  // Output HIGH to indicate system is ready

}

void loop() {
  // put your main code here, to run repeatedly:

}
