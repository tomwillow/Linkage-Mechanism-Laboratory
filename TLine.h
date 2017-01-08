#pragma once
#include <Windows.h>
#include <math.h>

#include "TElement.h"

class TLine:public TElement
{
public:
	POINT ptBegin, ptEnd;
	double &dAngle;
	double dLength;
	TLine *pNext = NULL;
	TLine();
	~TLine();
	void TLine::CalcLength();
};

