#pragma once
#include <Windows.h>
#include <math.h>

#include "TConfiguration.h"

class TLine
{
public:
	POINT ptBegin, ptEnd;
	int iStyle, iWidth;
	COLORREF crColor;
	double dLength;
	TLine *pNext = NULL;
	TLine();
	TLine(int iStyle, int iWidth, COLORREF crColor);
	~TLine();
	void TLine::CalcLength();
};

