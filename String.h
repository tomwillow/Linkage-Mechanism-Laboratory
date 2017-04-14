#pragma once
#include <windows.h>
#include "tchar_head.h"

#include <string>
#include <fstream>

#ifdef _UNICODE

using std::wstring;
#define String wstring

using std::wofstream;
#define Ofstream wofstream

using std::wifstream;
#define Ifstream wifstream

#else

using std::string;
#define String string

using std::ofstream;
#define Ofstream ofstream

using std::ifstream;
#define Ifstream ifstream

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
	_itow(i, temp, 10);
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