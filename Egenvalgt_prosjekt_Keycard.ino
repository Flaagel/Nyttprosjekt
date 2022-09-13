[10:12] Emil F
#include <Keypad.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#include <MFRC522v2.h>
#include <MFRC522DriverSPI.h>
#include <MFRC522DriverPinSimple.h>
#include <MFRC522Debug.h>
const byte ROWS = 4;
const byte COLS = 4;String password = "2468";
String input = "";
bool acceptPin = false;char hexaKeys[ROWS][COLS] = {
{'1', '2', '3', 'A'},
{'4', '5', '6', 'B'},
{'7', '8', '9', 'C'},
{'*', '0', '#', 'D'}
};int LED_YELLOW = 10;
int LED_GREEN = 11;
int LED_RED = 12;byte rowPins[ROWS] = {9, 8, 7, 6};
byte colPins[COLS] = {5, 4, 3, 2};Keypad customKeypad = Keypad(makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS);LiquidCrystal_I2C lcd(0x27,16,2); // set the LCD address to 0x27 for a 16 chars and 2 line displayMFRC522DriverPinSimple ss_pin(10); // Create pin driver. See typical pin layout above.SPIClass &spiClass = SPI; // Alternative SPI e.g. SPI2 or from library e.g. softwarespi.const SPISettings spiSettings = SPISettings(SPI_CLOCK_DIV4, MSBFIRST, SPI_MODE0); // May have to be set if hardware is not fully compatible to Arduino specifications.MFRC522DriverSPI driver{ss_pin, spiClass, spiSettings}; // Create SPI driver.MFRC522 mfrc522{driver}; // Create MFRC522 instance.unsigned long lastActionMillis = 0; // will store last time LED was updated
unsigned long backlightMillis = 0; // will store last time LED was updated// constants won't change:
const long timeout = 10000;
const long backlighttimeout = 20000;void resetInput()
{
acceptPin = false;
displayMessage("Avgrennset område", "Scan kort");
lcd.backlight();
backlightMillis = millis();
}void displayMessage(String heading, String text)
{
lcd.clear();
lcd.setCursor(0,0);
lcd.print(heading);
lcd.setCursor(0,1);
lcd.print(text);
lcd.backlight();
}void setup(){
Serial.begin(9600); lcd.init(); resetInput();
pinMode(LED_YELLOW, OUTPUT);
pinMode(LED_GREEN, OUTPUT);
pinMode(LED_RED, OUTPUT); mfrc522.PCD_Init(); // Init MFRC522 board.
MFRC522Debug::PCD_DumpVersionToSerial(mfrc522, Serial); // Show details of PCD - MFRC522 Card Reader details.
}/**
* mfrc522.PICC_IsNewCardPresent() should be checked before
* @return the card UID
*/
unsigned long getID(){
if ( ! mfrc522.PICC_ReadCardSerial()) { //Since a PICC placed get Serial and continue
return -1;
}
unsigned long hex_num;
hex_num = mfrc522.uid.uidByte[0] << 24;
hex_num += mfrc522.uid.uidByte[1] << 16;
hex_num += mfrc522.uid.uidByte[2] << 8;
hex_num += mfrc522.uid.uidByte[3];
mfrc522.PICC_HaltA(); // Stop reading
return hex_num;
}void startPinInput(){
acceptPin = true;
input = "";
displayMessage("Enter pin", "");
//Serial.println("waiting for pin input");
digitalWrite(LED_YELLOW, HIGH);
lastActionMillis = millis();
}void loop(){
if(acceptPin == false) { if ( !mfrc522.PICC_IsNewCardPresent()) {
//Serial.println("No new card"); unsigned long currentMillis = millis(); if (currentMillis - backlightMillis >= backlighttimeout) {
lcd.noBacklight();
}
return;
} lastActionMillis = millis();
Serial.println("getting card info");
unsigned long uid = getID();
if(uid != -1){
Serial.print("Card detected, UID: "); Serial.println(uid);
if(uid == 4294945516) {
Serial.println("card accepted"); displayMessage("Velkommen", "Emil Florvaag");
delay(2000);
startPinInput();
} else {
Serial.println("card not accepted");
displayMessage("Card not accepted", "");
delay(2000);
resetInput();
}
} return;
} unsigned long currentMillis = millis(); if (currentMillis - lastActionMillis >= timeout) {
displayMessage("Timeout", "");
delay(2000);
resetInput();
acceptPin = false;
} char customKey = customKeypad.getKey();
if (customKey){
lastActionMillis = millis();
if(customKey == '*')
{
if(acceptPin == true) {
digitalWrite(LED_YELLOW, LOW);
if(input == password) {
displayMessage("Pin accepted", "");
//Serial.println("Pin accepted");
digitalWrite(LED_GREEN, HIGH);
delay(2000);
digitalWrite(LED_GREEN, LOW);
}
else
{
displayMessage("Pin not accepted", "");
//Serial.println("Pin NOT accepted");
digitalWrite(LED_RED, HIGH);
delay(2000);
digitalWrite(LED_RED, LOW);
} resetInput();
}
}
else{
if(acceptPin == true) {
input += customKey; String stars = ""; for(int i = 0; i < input.length(); i++){
stars += "*";
} displayMessage("Enter pin", stars);
//Serial.println("input: " + input);
}
else{
//Serial.println("not accepting input");
}
}
//Serial.println(customKey);
}
}

//TEST
