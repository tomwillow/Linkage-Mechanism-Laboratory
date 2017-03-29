#pragma once

#include "TLine.h"
#include "DPOINT.h"

class TRealLine :public TLine
{
public:
	DPOINT &ptBegin, ptEnd;
	TRealLine();
	~TRealLine();
	void TRealLine::SetPoint(DPOINT dptBegin, DPOINT dptEnd);
	void TRealLine::SetPoint(DPOINT dptBegin, double dLength, double dAngle);
	void TRealLine::NoticeListView(TListView *pListView);
	void TRealLine::SetPointByIvoryLine(int iIvoryLine, DPOINT dptBegin, double length, double angle);
	void TRealLine::SetPointByDegAngle(DPOINT dptBegin, double dLength, double dAngle);
	TRealLine& TRealLine::operator=(const TRealLine &realline);
	virtual bool TRealLine::WriteFile(HANDLE &hf, DWORD &now_pos);
	virtual bool TRealLine::ReadFile(HANDLE &hf, DWORD &now_pos,TShape *pShape);
};

