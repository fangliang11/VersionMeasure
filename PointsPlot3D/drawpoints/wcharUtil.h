///////////////////////////////////////////////////////////////////////////////
// wcharUtil.h
// ===========
// conversion utility between multi-byte char and wide char
// It also converts any number to char or wchar_t format. For example,
// toWchar(1) converts the number 1 to a wchar_t string, L"1".
//
//  AUTHOR: Fang Liang (fangliang1313@gmail.com)
// CREATED: 2018-12-20
// UPDATED: 2018-12-20
///////////////////////////////////////////////////////////////////////////////

#ifndef WCHAR_UTIL_H
#define WCHAR_UTIL_H

const wchar_t* toWchar(const char *str);                // convert char* to wchar_t*
const wchar_t* toWchar(float number, int precision=-1); // convert float to wchar_t*
const wchar_t* toWchar(double number, int precision=-1);// convert double float to wchar_t*
const wchar_t* toWchar(int number);                     // convert integer to wchar_t*
const wchar_t* toWchar(long number);                    // convert long integer to wchar_t*

const char* toChar(const wchar_t *str);                 // convert wchar_t* to char*
const char* toChar(float number, int precision=-1);     // convert float to char*
const char* toChar(double number, int precision=-1);    // convert double to char*
const char* toChar(int number);                         // convert integer to char*
const char* toChar(long number);                        // convert long integer to char*

#endif
