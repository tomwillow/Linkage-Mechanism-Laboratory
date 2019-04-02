#pragma once
#include <windows.h>
#include "..\Common\tchar_head.h"

#include <iostream>
#include <fstream>
#include <sstream>

#include <string>
#include <vector>

using std::endl;

#ifdef _UNICODE

using std::wstring;
#define String wstring

using std::wostream;
#define Ostream wostream

using std::wifstream;
#define Ifstream wifstream

using std::wofstream;
#define Ofstream wofstream

using std::wstringstream;
#define Stringstream wstringstream

using std::wistringstream;
#define Istringstream wistringstream

using std::wostringstream;
#define Ostringstream wostringstream

using std::wcout;
#define COUT wcout

using std::to_wstring;
#define To_string to_wstring

#else

using std::string;
#define String string

using std::ostream;
#define Ostream ostream

using std::ifstream;
#define Ifstream ifstream

using std::ofstream;
#define Ofstream ofstream

using std::stringstream;
#define Stringstream stringstream

using std::istringstream;
#define Istringstream istringstream

using std::ostringstream;
#define Ostringstream ostringstream

using std::cout;
#define COUT cout

using std::to_string;
#define To_string to_string

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
	_itoa_s(i, temp, 10);
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
	_itoa_s(i, temp, 10);
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

inline void Replace(String &src, const String sub,const String dest)
{
	auto it = String::npos;
	size_t pos = 0;
	size_t subn = sub.size();
	size_t destn = dest.size();
	while (1)
	{
		it = src.find(sub, pos);
		if (it == String::npos)
			break;

		src.replace(it, subn, dest);
		pos = it + destn;
	}
}

std::wstring stringToWstring(const std::string& str);
std::string wstringToString(const std::wstring& wstr);

std::vector<String> StrSliceToVector(String s);

std::vector<double> StrSliceToDoubleVector(String s);