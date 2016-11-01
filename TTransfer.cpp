#include "TTransfer.h"


TTransfer::TTransfer()
{
}


TTransfer::~TTransfer()
{
}

double TTransfer::TCHAR2double(TCHAR s[])
{
	return _tcstod(s, (TCHAR **)(s + _tcslen(s)));
}