#include <SIM900.h>

SIM900 cell(2,3);

// Send SMS Demo

#define LED_PIN 13
void setup() {
  // put your setup code here, to run once:
  cell.Reset();
  Serial.begin(9600);
  cell.begin(9600);
  cell.Verbose(true);
  cell.Boot();
  cell.Rcpt("+919738460098");
  cell.Message("Hello world");
  cell.SendSMS();
  pinMode(LED_PIN,OUTPUT);

}

void loop() {
  // put your main code here, to run repeatedly:
  digitalWrite(LED_PIN,HIGH);
  delay(1000);
  digitalWrite(LED_PIN,LOW);
  delay(1000);
}
