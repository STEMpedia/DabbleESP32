/*
  SMS module uses your smartphone to send SMS based on events occuring in your hardware board.
  NOTE:
    1)For Arduino Mega Connect Bluetooth on Serial3 pins.
    2)For Arduino Uno/Nano library uses SoftwareSerial,hence pin 2 and pin 3 are used
    as RX and TX pins respectively on SoftwareSerial.Hence with arduino Uno
    follow below connections for bluetooth.
    UNO  - BLUETOOTH
    2    - TX
    3    - RX
    3)For Uno/Nano keep bluetooth Baudrate below 38400.

    You can reduce the size of library compiled by enabling only those modules that you want to
    use. For this first define CUSTOM_SETTINGS followed by defining INCLUDE_modulename.

    Explore more on: https://thestempedia.com/docs/dabble/
*/

#define CUSTOM_SETTINGS
#define INCLUDE_SMS_MODULE
#include <DabbleESP32.h>
uint8_t pushButtonPin = 13;

void setup() {
  Serial.begin(115200);                          // make sure your Serial Monitor is also set at this baud rate.
  Dabble.begin("MyEsp32");                          //Change this baudrate as per your bluetooth baudrate. Connect bluetooth on digital pin 2(RX) and 3(TX) for Uno/Nano and on Serial3 pins for Mega.
  pinMode(pushButtonPin, INPUT_PULLUP);        //Since pin is internally pulled up hence connect one side of push button to ground so whenever button is pushed pin reads LOW logic.
 }

void loop() {
  Dabble.processInput();    //this function is used to refresh data obtained from smartphone.Hence calling this function is mandatory in order to get data properly from your mobile.
  if(digitalRead(pushButtonPin) == LOW)
  {
    SMS.sendMessage("1234567890","This is a test message for Dabble.");  //Contact Number, Content
    delay(1000);   //debounce delay
  }
  
}
