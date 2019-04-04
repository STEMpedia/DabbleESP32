/*
  

   Explore more on: https://thestempedia.com/docs/dabble/
*/
#define CUSTOM_SETTINGS
#define INCLUDE_SENSOR_MODULE
#define INCLUDE_DATALOGGER_MODULE
#include <DabbleESP32.h>
bool isFileOpen = true;
uint8_t closeFileSignalPin = 2;   //pull-down gpio 2 pin to ground and connect a push button on it such that pin reads logic HIGH when push button is pressed.

void initializeFile(){
  DataLogger.createFile("Microphone");
  DataLogger.createColumn("Decibel");
}

void setup() { 
  pinMode(closeFileSignalPin,INPUT);
  Serial.begin(115200);    // make sure your Serial Monitor is also set at this baud rate.
  Dabble.begin("Myesp32");    
  DataLogger.sendSettings(&initializeFile);
}

void loop() {
  Dabble.processInput();             //this function is used to refresh data obtained from smartphone.Hence calling this function is mandatory in order to get data properly from your mobile.
  if( isFileOpen == true)
  {
    print_Sound_data();
    DataLogger.send("Decibel",Sensor.getdata_Sound());
  }
  if((digitalRead(closeFileSignalPin) == HIGH) && isFileOpen == true)
  {
    isFileOpen = false;
    DataLogger.stop();
  }
}

void print_Sound_data()
{
  Serial.print("SOUND: ");
  Serial.println(Sensor.getdata_Sound(), 3);
  Serial.println();
}
