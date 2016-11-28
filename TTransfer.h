#pragma once
#include <iostream>
#include <tchar.h>

class TTransfer
{
public:
	TTransfer();
	~TTransfer();
	static double TTransfer::TCHAR2double(TCHAR s[]);
	static double TTransfer::TCHAR2double(TCHAR *start, TCHAR *end);
};

