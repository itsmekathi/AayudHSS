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


//************************************************************************************************************************
**   DEFINITIONS
**///

#define STATUS_LED 13
#define OUT0 9
#define OUT1 10
#define AUDIO_PLAY_TIMES 3          // Defines the number of time the audio will be played through the phone
#define AFTER_CALL_DELAY 10000      // 10 seconds delay after you send the call command
#define AUDIO_CHANNEL 0             // Defines the channel on which the audio is stored


#define NUMBER_STORE_START_LOCATION 10
#define NUMBER_LENGTH 13


// Size of the command buffer
#define MAX_CMD 16 
char serialDisplayBuffer[40]; //Buffer large enough for the largest string to be displayed
//Make the input command lenght to 15 so as to accept command plus phone numbers
const byte maxCmdLength = 15;
char cmdRPROGMEMBuffer[maxCmdLength+1] ={0,0,0,0,0,0,0,0,0,0,0};
char inputCommandBuffer[maxCmdLength+1] ={0,0,0,0,0,0,0,0,0,0,0,0,0,0};
byte k = 0,i= 0;


// LCD Display Strings stored in PROGRAM Memory
#define ENTER_COMMAND 0
#define MODIFY_AUDIO 1
#define MODIFY_PHONE_NUMBER 2
#define PRESS_START 3
#define PRESS_END 4
#define LCD_INITIALIZING 5
#define READY 6
#define INVALID_COMMAND 7
#define CANCELLED 8
#define CONFIRMED 9
#define CONFIRMATION 10
#define ENTER_PHONE_NUMBER 11
#define CALLING_NUMBER 12
#define HANGING_UP 13 
#define SENDING_MESSAGE 14
#define ASK_SERIAL_DATA 15
#define ENTERING_PROGRAMMING_MODE 16
#define SYSTEM_READY 17
#define ENTERING_MAIN_CODE 18
#define LCD_STARTUP_MESSAGE 19
#define GSM_STARTUP_ERROR 20
#define GSM_STARTUP_SUCCESSFULL 21


//Store LCD Strings that are supposed to be displayed in PROGRAM Memory so as to save RAM Space
const char lcdEnterCmd[] PROGMEM = {"Enter Command"};
const char lcdModifyAudio[] PROGMEM = {"Modify Audio "};
const char lcdModifyPhoneNumber[] PROGMEM = {"Modify phone Number "};
const char lcdPressStarToStart[] PROGMEM = {"Press * to START"};
const char lcdPressHashToEnd[] PROGMEM = {"Press # to END"};
const char lcdInitializing[] PROGMEM = {"Initializing"};
const char lcdReady[] PROGMEM ={"Ready"};
const char lcdInvalidCommand[] PROGMEM ={"Invalid Command"};
const char lcdCancelled[] PROGMEM = {"Cancelled.."};
const char lcdConfirmed[] PROGMEM = {"Confirmed.."};
const char lcdConfirmation[] PROGMEM = {"* for Y & #for N"};
const char lcdEnterNumber[] PROGMEM = {"Enter phone NO:"};
const char lcdCallingNumber[] PROGMEM = {"Calling Number: "};
const char lcdHangingUp[] PROGMEM = {"Hanging up :"};
const char lcdSendingMessage[] PROGMEM = {"Messaging Number :"};
const char lcdAskSerialData[] PROGMEM ={"Enter data to go to Programming mode"};
const char lcdEnteringProgrammingMode[] PROGMEM ={"Entering programming Mode"};
const char lcdSystemReasy[] PROGMEM ={"System Ready"};
const char lcdEnteringMainCode[] PROGMEM = {"Entering main code"};
const char lcdStartUpMessage[] PROGMEM = {"Aayud MS HSS V 2.0"};
const char lcdGSMStartUpError[] PROGMEM = {"GSM Initialization Error"};
const char lcdGSMStartUpSuccess[] PROGMEM = {"GSM Initialization Successfull"};



//Set up a table to refer your string
//Note : Add in the same order as the #define is used to reference the same
const char* const lcd_DisplayTable[] PROGMEM = {lcdEnterCmd,lcdModifyAudio,lcdModifyPhoneNumber, lcdPressStarToStart,lcdPressHashToEnd
,lcdInitializing,lcdReady, lcdInvalidCommand,lcdCancelled,lcdConfirmed, lcdConfirmation, lcdEnterNumber, lcdCallingNumber,lcdHangingUp,lcdSendingMessage, lcdAskSerialData
,lcdEnteringProgrammingMode, lcdSystemReasy, lcdEnteringMainCode, lcdStartUpMessage, lcdGSMStartUpError,lcdGSMStartUpSuccess};


char message[] = "Thief has entered the house, please take immediate action";

volatile int intVal = 0;        // used in interrupt service routines


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



void setup() {
  Serial.begin(9600);
  while(!Serial);
  SerialPrintFromPROGMEM(LCD_STARTUP_MESSAGE);
  gprs.preInit();//power on SIM800
  delay(1000);
  while(0 !=gprs.init()) { //gprs init
      delay(1000);
      SerialPrintFromPROGMEM(GSM_STARTUP_ERROR);
  }  
  SerialPrintFromPROGMEM(GSM_STARTUP_SUCCESSFULL);

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

// Functions used in interrupt routines
void setVal()
{
  intVal = 1;
}
void resetVal()
{
  intVal= 0;
  digitalWrite(OUT0,LOW);
  digitalWrite(OUT1,LOW);
}


/***************************************************************************************************/
/* Configuration Routines to congigure the number, modify the audio etc
 *  
 */
 
void SetParameters(){
  //Be in programming mode infinitely
  while(1){
    SerialPrintFromPROGMEM(ENTER_COMMAND);
    acceptAndExecuteCommand();
    delay(1000);
  }
}

 void SerialPrintFromPROGMEM(byte cmdOffset)
{
  strcpy_P(serialDisplayBuffer,(char*)pgm_read_word(&(lcd_DisplayTable[cmdOffset])));
  Serial.println(serialDisplayBuffer);  
}


void acceptAndExecuteCommand()
{
    Serial.flush();
    clearCommandBuffer();
    // Stay here untill data is availabel in serial port
    while(!Serial.available());
    delay(100); // let the serialbuffer fill up
    clearInputCommandBuffer();
    char ch;
    for(k=0;k<maxCmdLength;k++)
    {
      if(Serial.available())
      {
        ch = Serial.read();
        if(ch == '*')break;
        inputCommandBuffer[k] = ch;  
        }
        else break;   
  }
  
//Create a logic to compare data from EEPROM and place the result
    if( (inputCommandBuffer[0] == 'A') || (inputCommandBuffer[0] == 'a'))
    {
      // Modify autdio logic block
      SerialPrintFromPROGMEM(MODIFY_AUDIO);
      modifyAudio(atoi(&inputCommandBuffer[1]));
    }
    else if( (inputCommandBuffer[0] == 'B') || (inputCommandBuffer[0] == 'b'))
    {
      // Modify EEPROM first 10 bytes
      int value = atoi(&inputCommandBuffer[1]);
      Serial.println(value);
      switch ( value)
      {
        case 0:
        // Hang up the call
        SerialPrintFromPROGMEM(HANGING_UP);
        HangUp();
        break;
        default:
        SerialPrintFromPROGMEM(INVALID_COMMAND);        
      }
      
    }
    else if ( (inputCommandBuffer[0] == 'C')||(inputCommandBuffer[0] == 'c')  )
    {
      // Implement function to call the offset user
      call(atoi(&inputCommandBuffer[1]));
    }
    else if ((inputCommandBuffer[0] == 'M')||(inputCommandBuffer[0] == 'm')) 
    {
      // implement code to modify audio
      SerialPrintFromPROGMEM(MODIFY_PHONE_NUMBER);
      modifyPhoneNumber(atoi(&inputCommandBuffer[1]));
    }
    else if ((inputCommandBuffer[0] == 'S')||(inputCommandBuffer[0] == 's')){
      // Message sending routine
      sendMessage(atoi(&inputCommandBuffer[1]));
    }
    else
    {
      SerialPrintFromPROGMEM(INVALID_COMMAND);
    }
  
}


void modifyAudio(byte numChannel)
{
   SerialPrintFromPROGMEM(MODIFY_AUDIO);
  Serial.println(numChannel);
  //Write the logic to accept key and audio input from user and 
  audio1.playAudioTillBusy(numChannel);
  SerialPrintFromPROGMEM(PRESS_START);
  SerialPrintFromPROGMEM(PRESS_END);
  char ch= getControlKey();
  if(ch == '*')
  {
    audio1.recordAudio(numChannel);
    SerialPrintFromPROGMEM(PRESS_END);
    
    // Stay here till the user sends # on serial line
    Serial.flush();
    while(getControlKey() != '#');
    audio1.stopRecording();
    SerialPrintFromPROGMEM(CONFIRMED);
  }    
  else
  {
    SerialPrintFromPROGMEM(CANCELLED);
  }
}

void modifyPhoneNumber(byte numOffset)
{
  
   SerialPrintFromPROGMEM(MODIFY_PHONE_NUMBER);
  Serial.println(numOffset);
  
  loadPhoneNumberFromEEPROM(numOffset);
  Serial.println(inputCommandBuffer);
  SerialPrintFromPROGMEM(ENTER_PHONE_NUMBER);
  //Clear the Input command Buffer
  clearInputCommandBuffer();
  char key ;

// Read the input from Serial port
  Serial.flush();
  // Wait till data is present in serial port
  while(!Serial.available());
  delay(100);
  for(k=0;k<maxCmdLength;k++)
  {
    if(Serial.available()){
      key = Serial.read();
      if(key=='#')break;
      inputCommandBuffer[k] = key;
    }
    else break;
  }

  Serial.println(inputCommandBuffer);
    
  SerialPrintFromPROGMEM(CONFIRMATION);
  if(getControlKey() =='*') 
  {
    storePhoneNumberInEEPROM(numOffset);
   SerialPrintFromPROGMEM(CONFIRMED);
  }
  else 
  {
    SerialPrintFromPROGMEM(CANCELLED);
  }
  loadPhoneNumberFromEEPROM(numOffset);
  Serial.println(inputCommandBuffer);
  }
  

//Gets the confirmation keys from Keypad
char getControlKey()
{
  char key ;
  //Stay here untill data is available on serial port
  while(!Serial.available());
  delay(10);  // let the serial buffer fill up
  key = Serial.read();
  Serial.println(key);
   return key;
}

void storePhoneNumberInEEPROM(byte offset)
{
  for (k=0;k<NUMBER_LENGTH;k++)
  {
    EEPROM.write(NUMBER_STORE_START_LOCATION+(offset*NUMBER_LENGTH)+k,inputCommandBuffer[k]);
  }
}

void loadPhoneNumberFromEEPROM(byte offset)
{
  clearInputCommandBuffer();
  for(k=0;k<NUMBER_LENGTH;k++)
  {
    inputCommandBuffer[k] = (char)EEPROM.read(NUMBER_STORE_START_LOCATION+(offset*NUMBER_LENGTH)+k);
  }
}

//Clears the Input Command Buffer
void clearInputCommandBuffer()
{
  for (k = 0; k<maxCmdLength;k++)
  {
    inputCommandBuffer[k] ='\0';
  }
}

//Clears the Input Command Buffer
void clearCommandBuffer()
{
  for (k = 0; k<maxCmdLength;k++)
  {
    cmdRPROGMEMBuffer[k] ='\0';
  }
}



//Call Demo
void call(byte offset){
  loadPhoneNumberFromEEPROM(offset);
  SerialPrintFromPROGMEM(CALLING_NUMBER);
  Serial.println(inputCommandBuffer);
  cell.Call(inputCommandBuffer);
}

// send Message
void sendMessage(byte offset)
{
  loadPhoneNumberFromEEPROM(offset);
  SerialPrintFromPROGMEM(SENDING_MESSAGE);
  cell.Message(message);
  cell.Rcpt(inputCommandBuffer);
  cell.SendSMS();
}

void HangUp()
{
  cell.HangUp();
}

void AlertUser(byte offset)
{
  
}
}

