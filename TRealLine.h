#pragma once

#include "TLine.h"
#include "DPOINT.h"

class TRealLine :public TLine
{
public:
	DPOINT ptBegin, ptEnd;
	TRealLine();
	~TRealLine();
	void TRealLine::CalcLength();
	void TRealLine::SetPoint(DPOINT ptBegin, DPOINT ptEnd);
	void TRealLine::SetPoint(DPOINT ptBegin, double dLength, double dAngle);
	void TRealLine::NoticeListView(TListView *pListView);
};

