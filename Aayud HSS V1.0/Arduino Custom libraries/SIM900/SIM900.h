#ifndef _SIM900_H
#define _SIM900_H
#include "Arduino.h"
#include <SoftwareSerial.h>

#define SERIALTIMEOUT 5000
#define PHONESIZE 13
#define MAXMSGLEN 100
#define OUTMESSAGE_SIZE 100

class SIM900 : public SoftwareSerial {
public:
  SIM900(int rxpin,int txpin);
  
  void Boot();
  void Reset();
  int ReadLine();
  void Verbose(boolean var1);
  
  void EndSMS();
  void StartSMS();
  void DeleteAllSMS();
  void SendSMS();
  void SendSMS(char * cellnumber,char * outmsg);
  void Sender(char * var1);
  char * Sender();
  void Rcpt(char * var1);
  char * Rcpt();
  void Message(char * var1);
  char * Message();

  byte CallStatus(void);			// Finds out the status of call
  void PickUp(void);				// Picks up an incoming call
  void HangUp(void);				// Hangs up an incomming call
  void Call(char* cellNumber);		// Calls the specific number
  void Call();						// Calls the numer which is already stored in rcpt variable
  void Call(int sim_position);		// Makes a call to the number stored at the specified SIM position

  
  boolean verbose;
  char sendernumber[PHONESIZE + 1];
  char rcpt[PHONESIZE + 1];
  char outmessage[OUTMESSAGE_SIZE];
  char inmessage[100];

  protected:
  unsigned long lastrec;
  
};

#endif