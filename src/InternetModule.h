
#ifndef	InternetModule_h
#define InternetModule_h

#define MAX_NO_OF_REQUESTS					20

#include "ModuleParent.h"
#include "HttpResponse.h"
#include "HttpRequest.h"


//Output Function ID's for Internet calss
#define INTERNET_GET						0x09
#define INTERNET_POST						0x0A
#define INTERNET_PUT						0x0B
#define INTERNET_DELETE						0x0C
#define INTERNET_CANCEL_ALL_REQUESTS		0x0D
#define INTERNET_SET_AUTHENTICATION			0x0E
#define INTERNET_CLEAR_AUTHENTICATIOM		0x0F
#define INTERNET_SET_DEFAULT_MAX_RESPONSE	0x10
#define INTERNET_QUERY_JSON					0x14
#define INTERNET_QUERY_JSON_ARRAY_LENGTH	0x17

//Input Function ID's for Internet class
#define INTERNET_GET_ERROR					0x06

//Literals
#define	REQUEST_CAN_NOT_BE_FOUND			0x00
#define	NOT_CONNECTED_TO_NETWORK			0x01	
#define	URL_IS_NOT_FOUND					0x02
#define	ALREADY_EXECUTING_REQUEST			0x03
#define	URL_IS_WRONG						0x04



class InternetModule : public ModuleParent
{
public:
	//Constructor 
	InternetModule();
	//Senders 
	bool performGet(HttpRequest &);
	bool performPost(HttpRequest &);
	bool performPut(HttpRequest &);
	bool performDelete(HttpRequest &);
	void cancelAllRequests();
	void ignoreResponse(HttpRequest &);
	void setBasicAuthentication(const char * ,const char *);
	void clearBasicAuthentication();
	void setIntialResponseMaxBytesCount(int );
	//Set on for userFunction
	void setOnError(void (*)(int,int));
private:
	bool isSetOnErrorCallBackAssigned;
	void processData();
	bool addToRequestsArray(HttpRequest &);
	void (*internetErrorCallBack)(int,int);
	HttpRequest * requestsArray[MAX_NO_OF_REQUESTS];
friend class HttpRequest;
};
//Extern Object
extern InternetModule Internet;
#endif