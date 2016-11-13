#pragma once
#include <Windows.h>

#include "DPOINT.h"
class TFramePoint
{
public:
	TFramePoint();
	~TFramePoint();
	DPOINT dpt;
	LOGPEN logpenStyle;
	void TFramePoint::SetStyle(int iStyle, int iWidth, COLORREF crColor);
};

