#include "TRealLine.h"


TRealLine::TRealLine()
{
}

TRealLine::TRealLine(int iStyle, int iWidth, COLORREF crColor)
{
	TRealLine::iStyle = iStyle;
	TRealLine::iWidth = iWidth;
	TRealLine::crColor = crColor;
}

TRealLine::~TRealLine()
{
}

void TRealLine::CalcLength()
{
	dLength = sqrt(pow(double(ptEnd.x - ptBegin.x), 2) + pow(double(ptEnd.y - ptBegin.y), 2));
}