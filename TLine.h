#pragma once
#include <Windows.h>
#include <math.h>

#include "TConfiguration.h"

class TLine
{
public:
	POINT ptBegin, ptEnd;
	LOGPEN logpenStyle;
	double dLength;
	TLine *pNext = NULL;
	TLine();
	~TLine();
	void TLine::SetStyle(int iStyle, int iWidth, COLORREF crColor);
	void TLine::CalcLength();
};

