#pragma once

#include "TTransfer.h"
#include <Windows.h>

double TTransfer::TCHAR2double(TCHAR s[])
{
	return _tcstod(s, NULL);//(TCHAR **)(s + _tcslen(s))
}

double TTransfer::TCHAR2double(TCHAR *start, TCHAR *end)//¿ªÊ¼×Ö·û£¬½áÊø×Ö·û
{
	int len = end - start;
	TCHAR *temp = new TCHAR[len + 1];
	_tcsncpy(temp, start,len);
	temp[len] = TEXT('\0');
	double value = _tcstod(temp, NULL);
	delete[] temp;
	return value;
}

double TTransfer::TCHAR2int(TCHAR s[])
{
#ifdef _UNICODE
	return _wtoi(s);
#else
	return atoi(s);
#endif
}

TCHAR * TTransfer::int2TCHAR(int i,TCHAR s[],int Radix)
{
#ifdef _UNICODE
	return _itow(i, s, Radix);
#else
	return _itoa(i, s, Radix);
#endif
}