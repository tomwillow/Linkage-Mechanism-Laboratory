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
	TRealLine& TRealLine::operator=(TRealLine &realline)
	{
		this->id = realline.id;
		this->available = realline.available;
		this->angle = realline.angle;
		this->dLength = realline.dLength;
		this->ptBegin = realline.dpt;
		this->ptEnd = realline.ptEnd;
		this->logpenStyle = realline.logpenStyle;
		this->logpenStyleShow = realline.logpenStyleShow;
		return *this;
	}
};

