
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
