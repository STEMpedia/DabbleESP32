#define FROM_DABBLE_LIBRARY
#include "DabbleESP32.h"
#include "SPI.h"
#include "OscilloscopeModule.h"

OscilloscopeModule::OscilloscopeModule():ModuleParent(OSCILLOSCOPE_ID)
{
	//init_ade791x();
}

void OscilloscopeModule::send_channel_data()
{
	//int a=ade791xread_v1();
	//int b=ade791xread_vim();
	int a=0;
	int b=0;
	#ifdef DEBUG
	 Serial.print("Ch1:");
     Serial.println(a);
	 #endif
	 
     ch_data1[1] = a & 0xFF;

     a=a >> 8;//shift value to teh right, result is 65472d for my DAC
     ch_data1[0] = (a & 0xFF) ;   //mask LSB, shift 8 bits to the right
	 #ifdef DEBUG
	 Serial.print("Ch2:");
     Serial.println(b);
	 #endif
	 
	 ch_data2[1] = b & 0xFF;

     b=b >> 8;//shift value to teh right, result is 65472d for my DAC
     ch_data2[0] = (b & 0xFF) ;   //mask LSB, shift 8 bits to the right
	 Dabble.sendModuleFrame(OSCILLOSCOPE_ID,0,0x02,2,new FunctionArg(2,ch_data1),new FunctionArg(2,ch_data2));
}

void OscilloscopeModule::send_channel_data(int a,int b)
{
	#ifdef DEBUG
	 Serial.print("Ch1:");
     Serial.println(a);
	 Serial.print("Ch1:");
     Serial.println(a,BIN);
	 #endif
     ch_data1[1] = a & 0xFF;

     a=a >> 8;//shift value to teh right, result is 65472d for my DAC
     ch_data1[0] = (a & 0xFF) ;   //mask LSB, shift 8 bits to the right
	 #ifdef DEBUG
	 Serial.print("Ch2:");
     Serial.println(b);
	 Serial.print("Ch2:");
     Serial.println(b,BIN);
	 #endif
	 ch_data2[1] = b & 0xFF;

     b=b >> 8;//shift value to teh right, result is 65472d for my DAC
     ch_data2[0] = (b & 0xFF) ;   //mask LSB, shift 8 bits to the right
	// Dabble.sendModuleFrame(OSCILLOSCOPE_ID,0,0x02,2,new FunctionArg(4,ch_data));
}

