#pragma once
#include "tchar_head.h"

namespace TTransfer
{
	int TCHAR2int(TCHAR s[]);
	TCHAR * int2TCHAR(int i, TCHAR s[], int Radix = 10);
	double TCHAR2double(TCHAR s[]);
	double TCHAR2double(TCHAR *start, TCHAR *end);
}