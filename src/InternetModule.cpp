
#define FROM_DABBLE_LIBRARY
#include "DabbleESP32.h"
#include "InternetModule.h"



InternetModule::InternetModule() : ModuleParent(INTERNET_ID)
{
	isSetOnErrorCallBackAssigned=false;
	for(int i=0;i<MAX_NO_OF_REQUESTS;i++){
		requestsArray[i]=NULL;
	}
}

bool InternetModule::addToRequestsArray(HttpRequest & request)
{
	int i;
	if(request.callbacksRequested!=0)
	{
		for (i = 0; i < MAX_NO_OF_REQUESTS; i++)
		{
			if(requestsArray[i]==&request)return true;
		}
		
		for (i = 0; i < MAX_NO_OF_REQUESTS; i++)
		{
			if(requestsArray[i]==NULL)break;
		}

		if(i>=MAX_NO_OF_REQUESTS)return false;
		else
		{
			requestsArray[i]=&request;
			return true;
		}
	}
	else {
		for (i = 0; i < MAX_NO_OF_REQUESTS; i++)
		{
			if(requestsArray[i]==&request)requestsArray[i]=NULL;
		}
		return true;
	}
}

bool InternetModule::performGet(HttpRequest & request)
{	
	bool isAdded=addToRequestsArray(request);
	if(isAdded)
	{
	
		Dabble.sendModuleFrame(INTERNET_ID,0,INTERNET_GET,2,
							 new FunctionArg(2,request.localRequestId),
							 new FunctionArg(1,&(request.callbacksRequested)));
	}
	
	return isAdded;
}

bool InternetModule::performPost(HttpRequest & request)
{	
	
	bool isAdded=addToRequestsArray(request);
	if(isAdded)
	{
		Dabble.sendModuleFrame(INTERNET_ID,0,INTERNET_POST,2,
						 new FunctionArg(2,request.localRequestId),
						 new FunctionArg(1,&(request.callbacksRequested)));
	}
	
	return isAdded;
}

bool InternetModule::performPut(HttpRequest & request)
{	
	bool isAdded=addToRequestsArray(request);
	if(isAdded)
	{
	
		Dabble.sendModuleFrame(INTERNET_ID,0,INTERNET_PUT,2,
							 new FunctionArg(2,request.localRequestId),
							 new FunctionArg(1,&(request.callbacksRequested)));
	}
	
	return isAdded;
}

bool InternetModule::performDelete(HttpRequest & request)
{	
	bool isAdded=addToRequestsArray(request);
	if(isAdded)
	{
	
		Dabble.sendModuleFrame(INTERNET_ID,0,INTERNET_DELETE,2,
							 new FunctionArg(2,request.localRequestId),
							 new FunctionArg(1,&(request.callbacksRequested)));
	}
	
	return isAdded;
}

void InternetModule::ignoreResponse(HttpRequest & request)
{
	request.ignoreResponse();
}

void InternetModule::cancelAllRequests(void)
{
	Dabble.sendModuleFrame(INTERNET_ID,0,INTERNET_CANCEL_ALL_REQUESTS,0);
}

void InternetModule::setBasicAuthentication(const char * userName,const char * password)
{
	//Check length of string 
	int userNameLength = strlen(userName);
	int passwordLength = strlen(password);
	if(!userNameLength || !passwordLength) return;
	Dabble.sendModuleFrame(INTERNET_ID,0,INTERNET_SET_AUTHENTICATION,2,
		                 new FunctionArg(userNameLength,(byte*)userName),
		                 new FunctionArg(passwordLength,(byte*)password));
}

void InternetModule::clearBasicAuthentication()
{
	Dabble.sendModuleFrame(INTERNET_ID,0,INTERNET_CLEAR_AUTHENTICATIOM,0);
}

void InternetModule::setIntialResponseMaxBytesCount(int size)
{
	byte sizeArray[2] ;
  	sizeArray[1] = (size >> 8) & 0xFF;
  	sizeArray[0] = size & 0xFF;
	Dabble.sendModuleFrame(INTERNET_ID,0,INTERNET_SET_DEFAULT_MAX_RESPONSE,1,new FunctionArg(2,sizeArray));
}

void InternetModule::processData()
{
	byte functionId = getDabbleInstance().getFunctionId();

	if(functionId == HTTP_GET_SUCCESS   || functionId == HTTP_GET_FAILURE           ||
	   functionId == HTTP_GET_STARTED   ||
	   functionId == HTTP_GET_ON_FINISH || functionId == RESPONSE_GET_NEXT_RESPONSE ||
	   functionId == RESPONSE_GET_ERROR || functionId == RESPONSE_INPUT_GET_HEADER	||
	   functionId == RESPONSE_GET_JSON  || functionId == RESPONSE_GET_JSON_ARRAY_LENGTH)
	{

		int requestId  = getDabbleInstance().getArgumentData(0)[0]|((getDabbleInstance().getArgumentData(0)[1])<<8);
		unsigned long totalBytesCount  = 0;
		int statusCode = 0;
		int error = 0;

		if(functionId ==HTTP_GET_SUCCESS||functionId == HTTP_GET_FAILURE)
			statusCode = getDabbleInstance().getArgumentData(1)[0]|((getDabbleInstance().getArgumentData(1)[1])<<8);
		else if (functionId == RESPONSE_GET_ERROR)
			error=getDabbleInstance().getArgumentData(1)[0];


		if(functionId == HTTP_GET_SUCCESS||functionId == HTTP_GET_FAILURE)
		{	
			 totalBytesCount  =(unsigned long)getDabbleInstance().getArgumentData(2)[0]
						 		|(((unsigned long)getDabbleInstance().getArgumentData(2)[1])<<8)
						 		|(((unsigned long)getDabbleInstance().getArgumentData(2)[2])<<16)
						 		|(((unsigned long)getDabbleInstance().getArgumentData(2)[3])<<24); 
		}
		
		char * data =NULL;
		int dataLength=0;
		int i;
		for (i = 0; i < MAX_NO_OF_REQUESTS; i++)
		{  
			if(requestsArray[i]!=NULL&&requestsArray[i]->getId()==requestId)
			{
				if((((requestsArray[i]->callbacksRequested) & SUCCESS_CALLBACK_BIT) && functionId == HTTP_GET_SUCCESS)||
				   (((requestsArray[i]->callbacksRequested) & FAILURE_CALLBACK_BIT) && functionId == HTTP_GET_FAILURE)||
				   (((requestsArray[i]->response.callbacksRequested) & RESPONSE_GET_NEXT_RESPONSE_BIT) && functionId == RESPONSE_GET_NEXT_RESPONSE))
				{
					byte dataArgumentNumber; 

					if(functionId == RESPONSE_GET_NEXT_RESPONSE)
					{
						dataArgumentNumber = 1;
						if(requestsArray[i]->response.bytesCount!= 0 && requestsArray[i]->response.isInit && requestsArray[i]->response.bytes!=NULL)
						{
							free(requestsArray[i]->response.bytes);
							requestsArray[i]->response.bytes = NULL;
						}
					}
					else
					{
						dataArgumentNumber = 3;
						requestsArray[i]->response.dispose(false);
					}
					
					if((functionId == RESPONSE_GET_NEXT_RESPONSE && requestsArray[i]->response.isInit) ||
					  ((functionId == HTTP_GET_SUCCESS||functionId == HTTP_GET_FAILURE) && totalBytesCount!=0))
					{
						dataLength = getDabbleInstance().getArgumentLength(dataArgumentNumber);
						if(dataLength!=0)
						{
							data=(char*)malloc(sizeof(char)*(dataLength+1));
							for (int j=0; j<dataLength; j++)
							{
								data[j]=getDabbleInstance().getArgumentData(dataArgumentNumber)[j];
							}

						data[dataLength]='\0';
						requestsArray[i]->response.bytesCount=dataLength;
						requestsArray[i]->response.index+=dataLength;
						requestsArray[i]->response.bytes=data;
						}
						else
						{
							requestsArray[i]->response.bytesCount=0;
							requestsArray[i]->response.bytes=NULL;
						}
					}

					if(functionId != RESPONSE_GET_NEXT_RESPONSE)
					{
						requestsArray[i]->response.isInit=true;
						requestsArray[i]->response.isDisposedTriggered=false;
						requestsArray[i]->response.statusCode=statusCode;
						requestsArray[i]->response.totalBytesCount=totalBytesCount;
					}
					if(!isInACallback())
					{
						if(((requestsArray[i]->callbacksRequested) & SUCCESS_CALLBACK_BIT) && functionId == HTTP_GET_SUCCESS)
							{
								enteringACallback();
								requestsArray[i]->successCallBack(requestsArray[i]->response);
								exitingACallback();
							}
						else if(((requestsArray[i]->callbacksRequested) & FAILURE_CALLBACK_BIT) && functionId == HTTP_GET_FAILURE)
							{
								enteringACallback();
								requestsArray[i]->failureCallBack(requestsArray[i]->response);
								exitingACallback();
							}
						else if((requestsArray[i]->response.callbacksRequested) & RESPONSE_GET_NEXT_RESPONSE_BIT)
							{
								enteringACallback();
								requestsArray[i]->response.getNextCallBack(requestsArray[i]->response);
								exitingACallback();
							}
					}
				}
				else if(!isInACallback())
				{
					if(((requestsArray[i]->callbacksRequested) & START_CALLBACK_BIT) && functionId == HTTP_GET_STARTED)
					{
						enteringACallback();
						requestsArray[i]->startCallBack();
						exitingACallback();
					}
					else if(((requestsArray[i]->callbacksRequested) & FINISH_CALLBACK_BIT) && functionId == HTTP_GET_ON_FINISH)
					{
						enteringACallback();
						requestsArray[i]->finishCallBack();
						exitingACallback();
					}
					else if(((requestsArray[i]->response.callbacksRequested) & RESPONSE_GET_ERROR_BIT) && functionId == RESPONSE_GET_ERROR) 
					{
						enteringACallback();
						requestsArray[i]->response.getErrorCallBack(error);	
						exitingACallback();
					}
					else if(((requestsArray[i]->response.callbacksRequested) & RESPONSE_INPUT_GET_HEADER_BIT) && functionId == RESPONSE_INPUT_GET_HEADER)
					{
						byte headerNameLength = getDabbleInstance().getArgumentLength(1);

						char  headerName [headerNameLength+1];
						for(int k = 0 ;k<headerNameLength ;k++)
						{
							headerName[k]=getDabbleInstance().getArgumentData(1)[k];
						}
						headerName[headerNameLength]='\0';


						byte headerValueLength = getDabbleInstance().getArgumentLength(2);

						char  headerValue [headerValueLength+1];
						for(int j = 0 ;j<headerValueLength ;j++)
						{
							headerValue[j]=getDabbleInstance().getArgumentData(2)[j];
						}
						headerValue[headerValueLength]='\0';
						enteringACallback();
						requestsArray[i]->response.getHeaderCallBack(headerName,headerValue);
						exitingACallback();
					}
					else if(functionId == RESPONSE_GET_JSON || functionId == RESPONSE_GET_JSON_ARRAY_LENGTH)
					{
						int keyChainTypes = getDabbleInstance().getArgumentData(2)[0]|((getDabbleInstance().getArgumentData(2)[1])<<8);

						byte argumentNo = getDabbleInstance().getArgumentNo();
						if(argumentNo - 3 <= MAX_JSON_KEY_DEPTH)
						{
							JsonKeyChain responseJsonChain;
							for(int j=3;j<argumentNo;j++)
							{
								if((keyChainTypes & (1 << (j-3))))
								{
									byte jsonKeyValueLength = getDabbleInstance().getArgumentLength(j);
									char  jsonKeyValue[jsonKeyValueLength+1];
									for(int k = 0 ;k<jsonKeyValueLength ;k++)
									{
										jsonKeyValue[k]=getDabbleInstance().getArgumentData(j)[k];
									}
									jsonKeyValue[jsonKeyValueLength]='\0';
									responseJsonChain[jsonKeyValue];
								}
								else
								{
									int jsonKeyValue = getDabbleInstance().getArgumentData(j)[0]|((getDabbleInstance().getArgumentData(j)[1])<<8);
									responseJsonChain[jsonKeyValue];
								}
							}

							if(((requestsArray[i]->response.callbacksRequested) & RESPONSE_GET_JSON_BIT) && functionId == RESPONSE_GET_JSON)
							{
								byte jsonResponseLength = getDabbleInstance().getArgumentLength(1);

								char  jsonResponseValue[jsonResponseLength+1];
								for(int k = 0 ;k<jsonResponseLength ;k++)
								{
									jsonResponseValue[k]=getDabbleInstance().getArgumentData(1)[k];
								}
								jsonResponseValue[jsonResponseLength]='\0';
								enteringACallback(); 	
								requestsArray[i]->response.getJsonCallBack(responseJsonChain,jsonResponseValue);
								exitingACallback();
							}
							else if(((requestsArray[i]->response.callbacksRequested) & RESPONSE_GET_JSON_ARRAY_LENGTH_BIT) && functionId == RESPONSE_GET_JSON_ARRAY_LENGTH)
							{
								unsigned long arrayLength  =(unsigned long)getDabbleInstance().getArgumentData(1)[0]
								 		|(((unsigned long)getDabbleInstance().getArgumentData(1)[1])<<8)
								 		|(((unsigned long)getDabbleInstance().getArgumentData(1)[2])<<16)
								 		|(((unsigned long)getDabbleInstance().getArgumentData(1)[3])<<24);
								enteringACallback(); 		
								requestsArray[i]->response.getJsonArrayLengthCallBack(responseJsonChain,arrayLength);
								exitingACallback();
							}
						}
					}
				}
				break;
			}
		}	

	}
	else if(functionId == INTERNET_GET_ERROR && isSetOnErrorCallBackAssigned)
	{
		int reqid  = getDabbleInstance().getArgumentData(0)[0]|((getDabbleInstance().getArgumentData(0)[1])<<8);
		int errorNumber  = getDabbleInstance().getArgumentData(1)[0];
		if(!isInACallback())
		{
			enteringACallback();
			(*internetErrorCallBack)(reqid,errorNumber);
			exitingACallback();
		}
	}
}

void InternetModule::setOnError(void (*userFunction)(int requestid, int errorNumber))
{
	isSetOnErrorCallBackAssigned = true;
	internetErrorCallBack = userFunction;
}

