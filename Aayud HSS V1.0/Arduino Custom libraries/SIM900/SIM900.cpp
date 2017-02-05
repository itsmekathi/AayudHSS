// SIM900 library
// Version 1.0
#include <SIM900.h>


// Constructor function
SIM900::SIM900(int rxpin,int txpin):
SoftwareSerial(rxpin,txpin)
{
 verbose=false;
}


void SIM900::Boot(){
  int counter=0;
  while(counter++ < 15){
    if (verbose) Serial.print(".");
    delay(1000);
  }
  if (verbose) Serial.println();
  
}

void SIM900::Verbose(boolean var1){
	verbose=var1;
}

void SIM900::Reset(){
  this->println("AT+CFUN=1,1"); // Reset Modem
  delay(200);
  this->ReadLine();
}


int SIM900::ReadLine(){
  static int pos=0;
  char nc;
  while (this->available()){
    nc=this->read();
    if (nc == '\n' or (pos > MAXMSGLEN) or ((millis()> lastrec + SERIALTIMEOUT)and (pos > 0)) ){
      nc='\0';
      lastrec=millis();
      inmessage[pos]=nc;
     pos=0;
     if (verbose) Serial.println(inmessage);
      return 1;
    }
    else if (nc=='\r') {
    }
    else{
      inmessage[pos++]=nc;
      lastrec=millis();
    }
  }
  return 0;
}


char * SIM900::Sender(){
	return sendernumber;
}


char * SIM900::Rcpt(){
	return rcpt;
}

char * SIM900::Message(){
	return inmessage;
}


void SIM900::Sender(char * var1){
	sprintf(sendernumber,"%s",var1);
}


void SIM900::Rcpt(char * var1){
	sprintf(rcpt,"%s",var1);
}

void SIM900::Message(char * var1){
	sprintf(outmessage,"%s",var1);
}



////////////////////////////////////////////////////////////////////////////////
////////////////////   SMS functions /////////////////////////////////////////
void SIM900::EndSMS(){
  this->print(char(26));  // ASCII equivalent of Ctrl-Z
  Serial.println();

  //delay(5 * 1000); // the SMS module needs time to return to OK status
}

void SIM900::StartSMS(){
  this->println("AT+CMGF=1"); // set SMS mode to text
  delay(200);
  this->ReadLine();
  this->print("AT+CMGS=");

  this->print(char(34)); // ASCII equivalent of "

  Serial.print(rcpt);
  this->print(rcpt);

  this->println(char(34));  // ASCII equivalent of "

  delay(500); // give the module some thinking time
  this->ReadLine();

}

void SIM900::DeleteAllSMS(){
  this->println("AT+CMGD=1,4"); // delete all SMS
  delay(200);
  this->ReadLine();
}

void SIM900::SendSMS(char * cellnumber,char * outmsg){
  this->Rcpt(cellnumber);
  if (verbose) Serial.println(rcpt);
  this->StartSMS();
  this->Message(outmsg);
  Serial.print(outmessage);
  this->print(outmessage);
  this->EndSMS();
  delay(500);
  this->ReadLine();
}

void SIM900::SendSMS(){
  if (verbose) Serial.println(rcpt);
  if (verbose) Serial.println(outmessage);
  this->StartSMS();
  Serial.print(outmessage);
  this->print(outmessage);
  this->EndSMS();
  delay(500);
  this->ReadLine();
}


//////////////////////////////////////////////////////////////////////////////////////
//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX///
//////////////////////////////////////////////////////////////////////////////////////
///////////////////////  Call routines //////////////////////////////////////////////

/**************************************************
Method checks status of call
return:
	CALL_NONE			- no call activity
	CALL_INCOM_VOICE	- incoming voice
	CALL_ACTIVE_VOICE	- active voice
	CALL_NO_RESPONSE	- no response to the AT command
	CALL_COMM_LINE_BUSY	- comm line is not free
**************************************************/
byte SIM900::CallStatus(void)			
{
	// Implements the AT+CPAS 
	this->println("AT+CPAS");
	delay(1000);
	this->ReadLine();
	return 1;
}

// Picksup the incoming call
void SIM900::PickUp(void)
{
	// Implements the ATA function
	this->println("ATA");
	delay(5000);
	this->ReadLine();
	return;
}

// Hangs up the current call
void SIM900::HangUp(void)
{
	this->println("ATH");
	this->ReadLine();
	return;
}

// Calls the phone number specified
void SIM900::Call(char * cellNumber)
{
	// Implement the calling mechanism
	this->Rcpt(cellNumber);
	if (verbose) Serial.println(rcpt);
	// ATDxxxxxx;<CR>
	this->print("ATD");
	this->print(rcpt);
	this->println(";");
	this->ReadLine();
}

void SIM900::Call()
{
	if (verbose) Serial.println(rcpt);
	// ATDxxxxxx;<CR>
	this->print("ATD");
	this->print(rcpt);
	this->println(";");
	this->ReadLine();
}

/*************************************************
Method calls he number stored at the specified SIM osition
sim_position: position in the SIM <1....>
			  e.g. Call(1);
**************************************************/

void SIM900::Call(int sim_position)
{
	// ATD>"SM" 1;<CR>
	this->print("ATD>\"SM\" ");
	this->print(sim_position);
	this->println(";");
	this->ReadLine();
	return;

}


//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX////
/////////////////////////////////////////////////////////////////////////////////////////




