#include "tchar_head.h"
#include "MyMath.h"
#include <Windows.h>
#include "String.h"
#include "TTransfer.h"

double precision = 1e-12;

// ‰»Î 1.0232 3
// ‰≥ˆ 102 1.02 2
void SignificantDigit(double dNum, int iDigit, int &iResult, double &dResult,int &iPrecisionDigit)
{
	TCHAR sz[64];
	_stprintf(sz, TEXT("%f"), dNum);

	TCHAR *start = sz;
	TCHAR *end = sz + _tcslen(sz);

	while (*start == TEXT('0') || *start == TEXT('.') || *start == TEXT('-'))
	{
		++start;
		if (*start == TEXT('\0'))
			return;
	}

	TCHAR *now = start;
	while (now <= start + iDigit-1 && *now!=TEXT('\0'))
	{
		if (*now == TEXT('.')) ++iDigit;
		++now;
	}

	if (dNum > pow(10, iDigit-1))
	{
		while (*now != TEXT('.') && *now != TEXT('\0'))
			*now++ = TEXT('0');
	}
	*now = TEXT('\0');

	dResult = TTransfer::TCHAR2double(sz);

	TCHAR *dot = _tcschr(sz, TEXT('.'));
	if (dot != NULL)
	{
		iPrecisionDigit = 0;
		while (*(dot+1) != TEXT('\0'))
		{
			swap(*dot, *(dot+1));
			++dot;
			++iPrecisionDigit;
		}
		swap(*dot, *(dot+1));
	}
	else
	{
		if (dResult > 0)
			iPrecisionDigit = -((int)_tcslen(sz)-1);
		else
			iPrecisionDigit = _tcslen(sz)-1;
	}

	if (dNum > pow(10, iDigit-1))
	{
		*(start + iDigit) = TEXT('\0');
	}
	iResult = TTransfer::TCHAR2int(sz);
}