#pragma once
#include <Windows.h>
#include "TRealLine.h"
#include "DPOINT.h"

class TShape
{
public:
	UINT uiLineNum;
	TRealLine *RealLine;
	TShape();
	~TShape();
	void TShape::AddLine(DPOINT ptBegin, DPOINT ptEnd, int iStyle, int iWidth, COLORREF crColor);
	void DeleteNewestLine();
};

