#pragma once
#define _CRT_SECURE_NO_WARNINGS
#define _CRT_NON_CONFORMING_SWPRINTFS

#include <Windows.h>
#include "TTransfer.h"


TTransfer::TTransfer()
{
}


TTransfer::~TTransfer()
{
}

double TTransfer::TCHAR2double(TCHAR s[])
{
	return _tcstod(s, NULL);//(TCHAR **)(s + _tcslen(s))
}

double TTransfer::TCHAR2double(TCHAR *start, TCHAR *end)
{
	int len = end - start;
	TCHAR *temp = new TCHAR[len + 1];
	_tcsncpy(temp, start,len);
	temp[len] = TEXT('\0');
	double value = _tcstod(temp, NULL);
	delete[] temp;
	return value;
}