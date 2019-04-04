
#ifndef JsonKeyChain_h
#define JsonKeyChain_h

#include "JsonKey.h"

//This number should not exceed 16
#define MAX_JSON_KEY_DEPTH 8

class JsonKeyChain {

public:
  JsonKeyChain();
  JsonKeyChain(int );
  JsonKeyChain(const JsonKeyChain& old);
  JsonKeyChain & operator[](int );
  JsonKeyChain & operator[](const char *key);
  bool operator==(const JsonKeyChain& other);
  bool operator!=(const JsonKeyChain& other);
  void query();
  void queryArrayLength();
  void dispose();
  ~JsonKeyChain();

private:
  int counter;
  int request;
  JsonKey * keysArray[MAX_JSON_KEY_DEPTH];
  void sendQueryFrame(byte);
};

#endif