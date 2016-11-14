#pragma once
#include <Windows.h>
#include <math.h>

#include "TElement.h"

class TLine:public TElement
{
public:
	POINT ptBegin, ptEnd;
	LOGPEN logpenStyleShow,logpenStyle;
	double dLength;
	TLine *pNext = NULL;
	TLine();
	~TLine();
	void TLine::SetStyle(int iStyle, int iWidth, COLORREF crColor);
	void TLine::CalcLength();
};

