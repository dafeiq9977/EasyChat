#pragma once
#ifndef FILE_UTILS_H__
#define FILE_UTILS_H__
#include "CommenHeaders.h"
using std::vector;
using std::string;

void getFileNames(const string &, vector<string> &);
void creatFile(const string &, const string &);
#endif