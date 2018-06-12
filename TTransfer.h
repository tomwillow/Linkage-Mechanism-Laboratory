#pragma once
#include "tchar_head.h"
#include <string>

namespace TTransfer
{
	int TCHAR2int(TCHAR s[]);
	TCHAR * int2TCHAR(int i, TCHAR s[], int Radix = 10);

	TCHAR * double2TCHAR(double d, TCHAR s[]);
	TCHAR * double2TCHAR(double d, TCHAR s[], int iDigit);
	double TCHAR2double(TCHAR s[]);
	double TCHAR2double(TCHAR *start, TCHAR *end);

	TCHAR * double2TCHAR_AutoTrim0(double d, TCHAR s[]);

	std::wstring char2wstring(const char *s);
	std::string unicode2string(const wchar_t *ws);
}