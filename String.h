#pragma once
#include <windows.h>
#include "tchar_head.h"

#include <iostream>
#include <string>
#include <fstream>

#ifdef _UNICODE

using std::wstring;
#define String wstring

//#define String std::basic_string<TCHAR>

using std::wofstream;
#define Ofstream wofstream

using std::wifstream;
#define Ifstream wifstream

using std::wcout;
#define COUT wcout;

#else

using std::string;
#define String string

using std::ofstream;
#define Ofstream ofstream

using std::ifstream;
#define Ifstream ifstream

using std::cout;
#define COUT cout;

#endif

inline String & operator<<(String &s, double d)
{
	TCHAR temp[32];
	_stprintf_s(temp, TEXT("%f"), d);
	s += temp;
	return s;
}

inline String & operator<<(String &s, size_t i)
{
	TCHAR temp[32];
#ifdef _UNICODE
	_itow_s(i, temp, 10);
#else
	_itoa(i, temp, 10);
#endif
	s += temp;
	return s;
}

inline String & operator<<(String &s, int i)
{
	TCHAR temp[32];
#ifdef _UNICODE
	_itow_s(i, temp, 10);
#else
	_itoa(i, temp, 10);
#endif
	s += temp;
	return s;
}

inline String & operator<<(String &s, TCHAR szStr[])
{
	s += szStr;
	return s;
}

inline String & operator<<(String &s,const String &s2)
{
	s += s2;
	return s;
}