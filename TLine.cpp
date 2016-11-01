#pragma once
#include "TLine.h"

TLine::TLine()
{
	TLine::dLength = 0;
}

void TLine::SetStyle(int iStyle, int iWidth, COLORREF crColor)
{
	logpenStyle.lopnStyle = iStyle;
	logpenStyle.lopnWidth.x = iWidth;
	logpenStyle.lopnColor = crColor;
}


TLine::~TLine()
{
}

void TLine::CalcLength()
{
	dLength = sqrt(pow(double(ptEnd.x - ptBegin.x), 2) + pow(double(ptEnd.y - ptBegin.y), 2));
}
