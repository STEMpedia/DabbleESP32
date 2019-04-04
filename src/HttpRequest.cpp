#define FROM_DABBLE_LIBRARY
#include "DabbleESP32.h"
#include "InternetModule.h"


int HttpRequest::totalRequests = 0;
// #ifdef INTERNET_MODULE
HttpRequest::HttpRequest(const char * _url)
{
	int reqId=++totalRequests;
	localRequestId[0] = reqId & 0xFF;
	localRequestId[1] = (reqId >> 8) & 0xFF;
	callbacksRequested = 0;
	url = NULL;
	response.requestId = getId();
	if(!strlen(_url)) return;
	if(!DabbleClass::isInitialized())
	{
		int urlLength=strlen(_url);
		url = (char*)malloc(sizeof(char)*(urlLength+1));
		for (int j=0; j<urlLength; j++)
		{
			url[j]=_url[j];
		}
		url[urlLength]='\0';
		DabbleClass::addToUnSentRequestsArray(this);
		isInitFrameSent=false;
	}
	else
	{
		sendInitFrame(_url);
	}
}
// #endif
void HttpRequest::sendInitFrame()
{
	if(!isInitFrameSent)
	{
		sendInitFrame(url);
		free (url);
	}
}

void HttpRequest::sendInitFrame(const char * _url)
{
	//Check length of string 
	int _urlLength = strlen(_url);
	if(!_urlLength) return;
	Dabble.sendModuleFrame(INTERNET_ID,0,HTTP_REQUEST_URL,2,new FunctionArg(2,(byte*)localRequestId),
														  new FunctionArg(_urlLength,(byte*)_url));
	isInitFrameSent=true;
}

void HttpRequest::setUrl(const char * urlName)
{
	//Check length of string 
	int urlNameLength = strlen(urlName);
	if(!urlNameLength) return;
	Dabble.sendModuleFrame(INTERNET_ID,0,HTTP_SET_URL,2,
						 new FunctionArg(2,localRequestId),
						 new FunctionArg(urlNameLength,(byte*)urlName));
}



void HttpRequest::addHeader(const char * headerName,const char * data)
{
	//Check length of string 
	int headerNameLength = strlen(headerName);
	int dataLength = strlen(data);
	if(!headerNameLength || !dataLength) return;
	Dabble.sendModuleFrame(INTERNET_ID,0,HTTP_ADD_HEADER,3,
		                 new FunctionArg(2,localRequestId),
		                 new FunctionArg(headerNameLength,(byte*)headerName),
		                 new FunctionArg(dataLength,(byte*)data));
}


void HttpRequest::addParameter(const char * paramName,const char * data)
{
	//Check length of string 
	int paramNameLength = strlen(paramName);
	int dataLength = strlen(data);
	if(!paramNameLength || !dataLength) return;
	Dabble.sendModuleFrame(INTERNET_ID,0,HTTP_ADD_PARAMETER,3,
		                 new FunctionArg(2,localRequestId),
		                 new FunctionArg(paramNameLength,(byte*)paramName),
		                 new FunctionArg(dataLength,(byte*)data));
}

void HttpRequest::addLastImageAsParameter(const char * paramName,byte imageSource,byte encoding)
{
	//Check length of string 
	int paramNameLength = strlen(paramName);
	if(!paramNameLength) return;
	Dabble.sendModuleFrame(INTERNET_ID,0,HTTP_ADD_LAST_IMAGE_AS_PARAM,4,
		                 new FunctionArg(2,localRequestId),
		                 new FunctionArg(paramNameLength,(byte*)paramName),
		                 new FunctionArg(1,&imageSource),
		                 new FunctionArg(1,&encoding));
}

void HttpRequest::addLastImageAsRawEntity(byte imageSource)
{
	Dabble.sendModuleFrame(INTERNET_ID,0,HTTP_ADD_LAST_IMAGE_AS_RAW_ENTITY,2,
		                 new FunctionArg(2,localRequestId),
		                 new FunctionArg(1,&imageSource));
}

void HttpRequest::addRawData(const char * data)
{
	//Check length of string 
	int dataLength = strlen(data);
	if(!dataLength) return;
	Dabble.sendModuleFrame(INTERNET_ID,0,HTTP_ADD_RAW_DATA,2,
						 new FunctionArg(2,localRequestId),
						 new FunctionArg(dataLength,(byte*)data));
}

int HttpRequest::getId(void)
{
	return (localRequestId[1] << 8) | localRequestId[0];
}

void HttpRequest::deleteHeaders(void)
{
	Dabble.sendModuleFrame(INTERNET_ID,0,HTTP_DELETE_HEADER,1,new FunctionArg(2,localRequestId));
}

void HttpRequest::deleteParameters(void)
{
	Dabble.sendModuleFrame(INTERNET_ID,0,HTTP_DELETE_PARAMETER,1,new FunctionArg(2,localRequestId));
}

void HttpRequest::deleteCallBacks(void)
{
	callbacksRequested=0;
}

void HttpRequest::setContentType(const char * contentType)
{
	//Check length of string 
	int contentTypeLength = strlen(contentType);
	if(!contentTypeLength) return;
	Dabble.sendModuleFrame(INTERNET_ID,0,HTTP_SET_CONTENT_TYPE,2,
						 new FunctionArg(2,localRequestId),
						 new FunctionArg(contentTypeLength,(byte*)contentType));
}

void HttpRequest::setParametersContentEncoding(const char * contentEncoding)
{
	//Check length of string 
	int contentEncodingLength = strlen(contentEncoding);
	if(!contentEncodingLength) return;
	Dabble.sendModuleFrame(INTERNET_ID,0,HTTP_SET_CONTENT_ENCODING,2,
						 new FunctionArg(2,localRequestId),
						 new FunctionArg(contentEncodingLength,(byte*)contentEncoding));
}

void HttpRequest::ignoreResponse(void)
{
	Dabble.sendModuleFrame(INTERNET_ID,0,HTTP_IGNORE_REQUEST,1,new FunctionArg(2,localRequestId));
}

void HttpRequest::setOnSuccess(void (*userFunction)(HttpResponse &))
{
	callbacksRequested |= SUCCESS_CALLBACK_BIT;
	successCallBack=userFunction;
}

void HttpRequest::setOnFailure(void (*userFunction)(HttpResponse &))
{
	callbacksRequested |= FAILURE_CALLBACK_BIT;
	failureCallBack=userFunction;
}

void HttpRequest::setOnStart(void (*userFunction)())
{
	callbacksRequested |= START_CALLBACK_BIT;
	startCallBack=userFunction;
}

void HttpRequest::setOnFinish(void (*userFunction)())
{
	callbacksRequested |= FINISH_CALLBACK_BIT;
	finishCallBack=userFunction;
}


HttpResponse & HttpRequest::getResponse()
{
	return response;
}

HttpRequest::~HttpRequest()
{
	for (int i = 0; i < MAX_NO_OF_REQUESTS; i++)
	{
		if(Internet.requestsArray[i] == this)
			{
				Internet.requestsArray[i]=NULL;
				return;
			}
	}
};