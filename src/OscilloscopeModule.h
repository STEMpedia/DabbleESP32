#ifndef OscilloscopeModule_h
#define OscilloscopeModule_h
#include "ModuleParent.h"


#include <SPI.h>

class OscilloscopeModule:public ModuleParent
{
	
public:
 int vl=0;
 int vim=0;
 uint8_t ch_data1[2];    //0-MSB channel 1 , 1-LSB channel, 2-MSB channel 2, 3-LSB channel 2
 uint8_t ch_data2[2];
 
 
public:
OscilloscopeModule();

void send_channel_data();
void send_channel_data(int a,int b);
};
extern OscilloscopeModule Oscilloscope;
#endif