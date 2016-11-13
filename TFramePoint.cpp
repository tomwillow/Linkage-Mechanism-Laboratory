#include "TFramePoint.h"


TFramePoint::TFramePoint()
{
	dpt.x = 0;
	dpt.y = 0;
}


TFramePoint::~TFramePoint()
{
}

void TFramePoint::SetStyle(int iStyle, int iWidth, COLORREF crColor)
{
	logpenStyle.lopnStyle = iStyle;
	logpenStyle.lopnWidth.x = iWidth;
	logpenStyle.lopnColor = crColor;
}