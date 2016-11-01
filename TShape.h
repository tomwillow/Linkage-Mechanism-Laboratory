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
	void TShape::AddRealLine(TRealLine realline);
	void DeleteNewestLine();
};

