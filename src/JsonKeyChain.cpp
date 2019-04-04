
#define FROM_DABBLE_LIBRARY
#include "DabbleESP32.h"
#include "InternetModule.h"
#include "JsonKeyChain.h"



JsonKeyChain::JsonKeyChain()
{
  counter=0;
  request=0;
}

JsonKeyChain::JsonKeyChain(int id)
{
  counter=0;
  request = id;
}

/* Copy Constructor */ 
JsonKeyChain::JsonKeyChain(const JsonKeyChain& old)
{
	counter=old.counter;
  request=old.request;
	if(counter>0){
		for(int i=0;i<counter;i++)
		{
			keysArray[i]=new JsonKey(*old.keysArray[i]);
		}
	}
}

JsonKeyChain & JsonKeyChain::operator[](int key)
{
    if(counter<MAX_JSON_KEY_DEPTH)keysArray[counter++]=new JsonKey(key);
    return *this;
}

JsonKeyChain & JsonKeyChain::operator[](const char *key)
{
    if(counter<MAX_JSON_KEY_DEPTH)keysArray[counter++]=new JsonKey(key);
    return *this;
}

bool JsonKeyChain::operator==(const JsonKeyChain& other) 
{
    if(counter!=other.counter)return false;
    for(int i=0;i<counter;i++)
    {
      if(keysArray[i]->isString()!=other.keysArray[i]->isString())
      {
        return false;
      }
      else if(keysArray[i]->isString() && strcmp(keysArray[i]->getString(),other.keysArray[i]->getString()))
      {
        return false;
      }
      else if(keysArray[i]->isNumber() && keysArray[i]->getNumber()!=other.keysArray[i]->getNumber())
      {
        return false;
      }
    }
    return true;
}

bool JsonKeyChain::operator!=(const JsonKeyChain& other)
{
  return !(*this == other);
}

void JsonKeyChain::dispose()
{
  for(int i=0;i<counter;i++)
    {
      delete keysArray[i];
    }
  counter=0;
}

void JsonKeyChain::query()
{
  sendQueryFrame(INTERNET_QUERY_JSON);
}

void JsonKeyChain::queryArrayLength()
{
  sendQueryFrame(INTERNET_QUERY_JSON_ARRAY_LENGTH);
}

void JsonKeyChain::sendQueryFrame(byte functionId)
{
  if(counter>16||counter==0||request==0)return;
  int types=0;
  FunctionArg **arguments =(FunctionArg**)malloc(sizeof(FunctionArg *)*(counter+2));
  for(int i=2;i<counter+2;i++)
    {
      if(keysArray[i-2]->isString())arguments[i]=new FunctionArg(strlen(keysArray[i-2]->getString()),(byte *)keysArray[i-2]->getString());
      else 
      {
        byte integerArray[2] ;
        integerArray[1] = (keysArray[i-2]->getNumber() >> 8) & 0xFF;
        integerArray[0] = keysArray[i-2]->getNumber() & 0xFF;
        arguments[i]=new FunctionArg(2,integerArray,true); 
      }
      types|=((keysArray[i-2]->isString())<<(i-2));
    }
    byte integerArray[2] ;
    integerArray[1] = (types >> 8) & 0xFF;
    integerArray[0] = types & 0xFF;
    arguments[1]=new FunctionArg(2,integerArray);

    byte requestIdArray[2] ;
    requestIdArray[1] = (request >> 8) & 0xFF;
    requestIdArray[0] = request & 0xFF;
    arguments[0]=new FunctionArg(2,requestIdArray);

    Dabble.sendModuleFrame(INTERNET_ID,0,functionId,counter+2,arguments);

    for(int i=2;i<counter+2;i++)
    {
      delete arguments[i];
    }
    delete arguments[1];
    delete arguments[0];
    free(arguments);
    dispose();
}

JsonKeyChain::~JsonKeyChain()
{
    dispose();
}