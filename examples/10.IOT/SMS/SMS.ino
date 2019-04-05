/*
  SMS module uses your smartphone to send SMS based on events occuring in your hardware board.
  
  You can reduce the size of library compiled by enabling only those modules that you want to
  use. For this first define CUSTOM_SETTINGS followed by defining INCLUDE_modulename.

  Explore more on: https://thestempedia.com/docs/dabble/
*/

#define CUSTOM_SETTINGS
#define INCLUDE_SMS_MODULE
#include <DabbleESP32.h>
uint8_t pushButtonPin = 13;

void setup() {
  Serial.begin(115200);                        // make sure your Serial Monitor is also set at this baud rate.
  Dabble.begin("MyEsp32");                     //set bluetooth name of your device    
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
