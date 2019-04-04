
#define FROM_DABBLE_LIBRARY
#include "DabbleESP32.h"
#include "InternetModule.h"


HttpResponse::HttpResponse()
{
	isInit=false;
	isDisposedTriggered=false;
	statusCode=0;
	totalBytesCount=0;
	bytes=NULL;
	bytesCount = 0;
	requestId = 0;
	index =0;
	callbacksRequested = 0;
}

int HttpResponse::getStatusCode()
{
	return statusCode;
}

int HttpResponse::getBytesCount()
{
	return bytesCount;
}

char * HttpResponse::getBytes()
{
	return bytes;
}

unsigned long HttpResponse::getTotalBytesCount()
{
	return totalBytesCount;
}

unsigned long HttpResponse::getCurrentIndex()
{
	return index;
}

void HttpResponse::getTheseBytes(unsigned long start,int size)
{
	if(isInit)
	{
		index=start;
		byte startArray[4] ;
	  	startArray[0] = start & 0xFF;
	  	startArray[1] = (start >> 8) & 0xFF;
	  	startArray[2] = (start >> 16) & 0xFF;
	  	startArray[3] = (start >> 24) & 0xFF;
		
	  	byte sizeArray[2] ;
	  	sizeArray[1] = (size >> 8) & 0xFF;
	  	sizeArray[0] = size & 0xFF;

	  	byte reqId[2] ;
	  	reqId[1] = (requestId >> 8) & 0xFF;
	  	reqId[0] = requestId & 0xFF;
		
		Dabble.sendModuleFrame(INTERNET_ID,0,RESPONSE_GET_NEXT_BYTES,3,new FunctionArg(2,reqId),
																	 new FunctionArg(4,startArray),
																	 new FunctionArg(2,sizeArray));
	}
	
}

void HttpResponse::getNextBytes(int size)
{
	getTheseBytes(index,size);
}

void HttpResponse::setOnNextResponseBytesUpdate(void (*userFunction)(HttpResponse &))
{
	callbacksRequested |= RESPONSE_GET_NEXT_RESPONSE_BIT;
	getNextCallBack = userFunction;
}

void HttpResponse::setOnError(void (*userFunction)(int errorNumber))
{
	callbacksRequested |= RESPONSE_GET_ERROR_BIT;
	getErrorCallBack = userFunction;
}

void HttpResponse::setOnJsonResponse(void (*userFunction)(JsonKeyChain & chain,char data[]))
{
	callbacksRequested |= RESPONSE_GET_JSON_BIT;
	getJsonCallBack = userFunction;
}

void HttpResponse::setOnJsonArrayLengthResponse(void (*userFunction)(JsonKeyChain & chain,unsigned long size))
{
	callbacksRequested |= RESPONSE_GET_JSON_ARRAY_LENGTH_BIT;
	getJsonArrayLengthCallBack = userFunction;
}

bool HttpResponse::isSentFully()
{
	return (index>=totalBytesCount);
}

void HttpResponse::dispose(bool sendFrame)
{
	isDisposedTriggered = true;
	if(isInit && bytesCount!=0 && bytes!=NULL)
	{
		free(bytes);
	}
	isInit=false;
	bytes=NULL;
	index = 0;
	bytesCount = 0;
	statusCode = 0;
	totalBytesCount = 0;
	byte reqId[2] ;
  	reqId[1] = (requestId >> 8) & 0xFF;
  	reqId[0] = requestId & 0xFF;
	
	if(sendFrame)
	{
		Dabble.sendModuleFrame(INTERNET_ID,0,RESPONSE_DISPOSE,1,new FunctionArg(2,reqId));
		callbacksRequested=0;
	}
}

bool HttpResponse::isDisposed()
{
	return isDisposedTriggered;
}

void HttpResponse::resetIndex(unsigned long x)
{
	index=x;
}

void HttpResponse::getHeader(const char * headerName , void (*userFunction)(char incomingheaderName [],char IncomingHeaderValue[]))
{
	if(isInit)
	{
		//Check length of string 
		int headerNameLength = strlen(headerName);
		if(!headerNameLength) return;
		callbacksRequested |= RESPONSE_INPUT_GET_HEADER_BIT;
		getHeaderCallBack = userFunction;
		
		byte reqId[2] ;
	  	reqId[1] = (requestId >> 8) & 0xFF;
	  	reqId[0] = requestId & 0xFF;
	  	
		Dabble.sendModuleFrame(INTERNET_ID,0,RESPONSE_GET_HEADER,2,new FunctionArg(2,reqId),new FunctionArg(headerNameLength,(byte *)headerName));
	}
	
}

JsonKeyChain HttpResponse::operator[](int key)
{
    JsonKeyChain chain(requestId);
    return chain[key];
}

JsonKeyChain HttpResponse::operator[](const char *key)
{
	if(!strlen(key)) return 0;
    JsonKeyChain chain(requestId);
    return chain[key];
}

HttpResponse::~HttpResponse()
{
	if(isInit && bytesCount!=0)free(bytes);
}