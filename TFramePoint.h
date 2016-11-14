#pragma once
#include <Windows.h>

#include "TElement.h"
#include "DPOINT.h"
class TFramePoint:public TElement
{
public:
	TFramePoint();
	~TFramePoint();
	DPOINT dpt;
	LOGPEN logpenStyle;
	void TFramePoint::SetStyle(int iStyle, int iWidth, COLORREF crColor);
};

