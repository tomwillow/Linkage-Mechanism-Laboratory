#pragma once
#include "tchar_head.h"

namespace TTransfer
{
	double TCHAR2int(TCHAR s[]);
	double TCHAR2double(TCHAR s[]);
	double TCHAR2double(TCHAR *start, TCHAR *end);
	TCHAR * int2TCHAR(int i, TCHAR s[], int Radix = 10);
}