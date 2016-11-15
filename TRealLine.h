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
	void TRealLine::SetPoint(DPOINT dptBegin, DPOINT dptEnd);
	void TRealLine::SetPoint(DPOINT dptBegin, double dLength, double dAngle);
	void TRealLine::NoticeListView(TListView *pListView);
};

