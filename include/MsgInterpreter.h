#pragma once
#ifndef MSG_INTERPRETER_H__
#define MSG_INTERPRETER_H__

#include "CommenHeaders.h"
using std::string;
typedef unsigned int ACTION;
struct MESSAGE {
	ACTION type;
	string subject;
	string object;
	string content;
};

int messageLength(MESSAGE &);
MESSAGE interprete(string & bytes, int flag);   
bool counterInterprete(const std::string &, std::string &);

#endif