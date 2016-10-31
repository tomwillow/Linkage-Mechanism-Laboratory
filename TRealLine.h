#pragma once

#include "TLine.h"
#include "DPOINT.h"

class TRealLine :public TLine
{
public:
	DPOINT ptBegin, ptEnd;
	TRealLine();
	~TRealLine();
	TRealLine(int iStyle, int iWidth, COLORREF crColor);
	void TRealLine::CalcLength();
};

