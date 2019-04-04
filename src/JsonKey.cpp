
#include "JsonKey.h"




JsonKey::JsonKey(int key)
  {
    number=key;
    isText=false;
  }
/* Copy Constructor */
JsonKey::JsonKey(const JsonKey & old)
{
	isText=old.isText;
	if(isText)
	{
		int textSize=strlen(old.text);
		text=(char *)malloc(sizeof(char)*(textSize+1));
		for (int i = 0; i < textSize; i++)
		{
		  text[i]=old.text[i];
		}
		text[textSize]='\0';
	}
	else
	{
		number=old.number;
	}
}

JsonKey::JsonKey(const char * key)
{
	int textSize=strlen(key);
	text=(char *)malloc(sizeof(char)*(textSize+1));
	for (int i = 0; i < textSize; i++)
	{
	  text[i]=key[i];
	}
	text[textSize]='\0';
	isText=true;
}

bool JsonKey::isString()
{
    return isText;
}

bool JsonKey::isNumber()
{
    return !isText;
}

const char * JsonKey::getString()
{
    return text;
}

int JsonKey::getNumber()
{
    return number;
}


JsonKey::~JsonKey()
{
    if(isText)free(text);
}  