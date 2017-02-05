#include <SIM900.h>
#include <APR33A3.h>
#include <avr/pgmspace.h>
#include <EEPROM.h>

// GSM rx,tx
SIM900 cell(5,6);


// APR33A3  busy,rec,m0,m1,m2,m3
APR33A3 audio1(A0,A2,A1,A3,A4,A5);


#define STATUS_LED 13           // Status led for various indication
volatile int intVal = 0;        // used in interrupt service routines

#define OUT0 9
#define OUT1 10
#define AUDIO_PLAY_TIMES 3          // Defines the number of time the audio will be played through the phone
#define AFTER_CALL_DELAY 10000      // 10 seconds delay after you send the call command
#define AUDIO_CHANNEL 0             // Defines the channel on which the audio is stored




//************************************************************************************************************************
#define NUMBER_STORE_START_LOCATION 10
#define NUMBER_LENGTH 13
#define STATUS_LED 13


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

char message[] = "Thief has entered the house, please take immediate action";


//Set up a table to refer your string
//Note : Add in the same order as the #define is used to reference the same
const char* const lcd_DisplayTable[] PROGMEM = {lcdEnterCmd,lcdModifyAudio,lcdModifyPhoneNumber, lcdPressStarToStart,lcdPressHashToEnd
,lcdInitializing,lcdReady, lcdInvalidCommand,lcdCancelled,lcdConfirmed, lcdConfirmation, lcdEnterNumber, lcdCallingNumber,lcdHangingUp,lcdSendingMessage, lcdAskSerialData
,lcdEnteringProgrammingMode, lcdSystemReasy, lcdEnteringMainCode};

//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX


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


void setup() {
  // Setup Serial and SIM900 baudrates
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
  const byte USER_COUNT = 2, ALERT_CYCLE = 1;
  byte i, k;
  Serial.print("Value of user count is ");
  Serial.println(USER_COUNT);
  Serial.print("Value of alert cycle is ");
  Serial.println(ALERT_CYCLE);
  
  Serial.print("value is " );
  Serial.println(intVal);
  // put your main code here, to run repeatedly:
  if(intVal == 1)
  {
    digitalWrite(OUT0,HIGH);
    digitalWrite(OUT1,HIGH);
    for( i = 0; i< ALERT_CYCLE ; i++){
      Serial.print("In alert cycle ");
      Serial.println(i);
      for (k=0;k< USER_COUNT;k++)
      {
        Serial.print("Alerting user ");
        Serial.println(k);
        AlertUser(k);
        delay(5000);        // Delay between alerting each user
      }
    } 
    // Set Int val to 2 to avoid re-executing the same block after alerting the user and also to avoid make the OUT0 and OUT1 low
    intVal = 2;   
   }
   else if (intVal == 0)
  {
    // No action to perform
    digitalWrite(OUT0,LOW);
    digitalWrite(OUT1,LOW);
    delay(1000);
  }
}

void SetParameters(){
  //Be in programming mode infinitely
  while(1){
    // put your main code here, to run repeatedly:
    Serial.flush();
    SerialPrintFromPROGMEM(ENTER_COMMAND);
    acceptAndExecuteCommand();
    //Delay between Each command entry
    delay(1000);
  }
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

 void SerialPrintFromPROGMEM(byte cmdOffset)
{
  strcpy_P(serialDisplayBuffer,(char*)pgm_read_word(&(lcd_DisplayTable[cmdOffset])));
  Serial.println(serialDisplayBuffer);  
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
  // Run the HangUp code for safety
  cell.flush();                               // clear the buffer before exit
  HangUp();
  delay(2000);
  sendMessage(offset);
  Serial.println("In line 418");
  delay(4000);                               // Extra delay of 10 secounds
  Serial.println("In line 420");
  call(offset);
  delay(AFTER_CALL_DELAY);
  Serial.println("In line 423");
  for (i = 0; i<AUDIO_PLAY_TIMES;i++){
    audio1.playAudioTillBusy(AUDIO_CHANNEL);
    delay(250);                               // Delay between each audio play cycle
  }
  delay(400);                                 // Dealy of 0.4 seconds before exiting the call
  HangUp();                                   // Hang up the Call
  delay(4000);                                // Delay after hang up
  Serial.println("In line 431");
  cell.flush();                               // clear the buffer before exit

}




