

#ifndef HttpRequest_h
#define HttpRequest_h

//Output Function ID's for HTTP Request class
#define HTTP_REQUEST_URL					0x01
#define HTTP_SET_URL						0x02
#define HTTP_ADD_HEADER						0x03
#define	HTTP_ADD_PARAMETER					0x04
#define HTTP_DELETE_HEADER					0x05
#define HTTP_DELETE_PARAMETER				0x06
#define HTTP_SET_CONTENT_TYPE				0x07
#define HTTP_IGNORE_REQUEST					0x08
#define	HTTP_ADD_RAW_DATA					0x15
#define HTTP_SET_CONTENT_ENCODING			0x16
#define	HTTP_ADD_LAST_IMAGE_AS_PARAM		0x18
#define	HTTP_ADD_LAST_IMAGE_AS_RAW_ENTITY	0x19

//Input Function ID's for HTTP Request class
#define HTTP_GET_SUCCESS					0x01
#define HTTP_GET_FAILURE					0x02
#define HTTP_GET_STARTED					0x03
#define HTTP_GET_ON_FINISH					0x05

#define SUCCESS_CALLBACK_BIT				0x01
#define FAILURE_CALLBACK_BIT				0x02
#define START_CALLBACK_BIT					0x04
#define FINISH_CALLBACK_BIT					0x08
//Image Encoding Types
#define RAW	0x00
#define BASE64	0x01

class HttpRequest 
{
public:
	// #ifdef INTERNET_MODULE
	//Constructors
	HttpRequest(const char *);
	// #endif
	//Setters
	void setUrl(const char *);
	//Adders
	void addHeader(const char * ,const char *);
	//Adders
	void addParameter(const char * ,const char *);
	void addLastImageAsParameter(const char * ,byte =0,byte =0);
	void addLastImageAsRawEntity(byte =0);
	void addRawData(const char *);
	//Getters
	int getId();
	//Deleters
	void deleteHeaders(void);
	void deleteParameters(void);
	void deleteCallBacks();
	//Setters
	void setContentType(const char * );
	void setParametersContentEncoding(const char * );
	//Deleter
	void ignoreResponse(void);
	//Set On for userFunctions 
	void setOnSuccess(void (*)(HttpResponse&));
	void setOnFailure(void (*)(HttpResponse&));
	void setOnStart(void (*)());
	void setOnFinish(void (*)());
	void sendInitFrame();
	HttpResponse & getResponse();
	~HttpRequest();
private:
	bool isInitFrameSent;
	char * url;
	byte localRequestId[2];
	void (*successCallBack)(HttpResponse &);
	void (*failureCallBack)(HttpResponse &);
	void (*startCallBack)();
	void (*finishCallBack)();
	byte callbacksRequested;
	HttpResponse response;
	static int totalRequests;
	void sendInitFrame(const char *);
friend class InternetModule;
};

#endif