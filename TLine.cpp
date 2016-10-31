#pragma once
#include "TLine.h"

TLine::TLine()
{
	TLine::dLength = 0;
}

TLine::TLine(int iStyle, int iWidth, COLORREF crColor)
{
	TLine::iStyle=iStyle;
	 TLine::iWidth=iWidth;
	 TLine::crColor = crColor;
	 TLine::dLength = 0;
}


TLine::~TLine()
{
}

void TLine::CalcLength()
{
	dLength = sqrt(pow(double(ptEnd.x - ptBegin.x), 2) + pow(double(ptEnd.y - ptBegin.y), 2));
}
