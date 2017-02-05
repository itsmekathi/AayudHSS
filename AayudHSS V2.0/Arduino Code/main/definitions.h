#include <Arduino.h>

#define STATUS_LED 13           // Status led for various indication
#define OUT0 9
#define OUT1 10
#define AUDIO_PLAY_TIMES 3          // Defines the number of time the audio will be played through the phone
#define AFTER_CALL_DELAY 10000      // 10 seconds delay after you send the call command
#define AUDIO_CHANNEL 0             // Defines the channel on which the audio is stored

//************************************************************************************************************************
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


