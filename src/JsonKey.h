#ifndef JsonKey_h
#define JsonKey_h

#include "Arduino.h"

class JsonKey{
public:
  
  JsonKey(int );
  JsonKey(const char * );
  JsonKey(const JsonKey & old);
  bool isString();
  bool isNumber();
  
  const char * getString();
  int getNumber();

  ~JsonKey();
private:
  bool isText;
  int number;
  char * text;
};


#endif