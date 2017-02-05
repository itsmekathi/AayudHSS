#include <Arduino.h>

 void SerialPrintFromPROGMEM(byte cmdOffset)
{
  strcpy_P(serialDisplayBuffer,(char*)pgm_read_word(&(lcd_DisplayTable[cmdOffset])));
  Serial.println(serialDisplayBuffer);  
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



