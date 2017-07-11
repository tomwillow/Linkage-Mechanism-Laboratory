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
	void TRealLine::SetPointByIvoryLine(int iIvoryLine, DPOINT dptBegin, double length, double angle);
	void TRealLine::SetPointByDegAngle(DPOINT dptBegin, double dLength, double dAngle);
	TRealLine& TRealLine::operator=(const TRealLine &realline);

	virtual void TRealLine::Draw(HDC hdc, const TConfiguration* pConfig) override;
	virtual void TRealLine::DrawPickSquare(HDC hdc, const TConfiguration* pConfig) override;
	virtual const TCHAR * TRealLine::GetElementTypeName(TCHAR name[]) override;//得到类型名称
	virtual void TRealLine::NoticeListView(TListView *pListView) override;
	virtual bool TRealLine::WriteFile(HANDLE &hf, DWORD &now_pos)override;
	virtual bool TRealLine::ReadFile(HANDLE &hf, DWORD &now_pos, TShape *pShape)override;
	virtual const DPOINT TRealLine::GetRelativePointByIndex(int PointIndexOfElement) const override;
	virtual DPOINT TRealLine::GetAbsolutePointByIndex(int PointIndexOfElement) const override;
	virtual void TRealLine::SetPhi(double phi) override;
	virtual void TRealLine::ChangePos(DPOINT dptDelta)override;
	virtual bool TRealLine::Picked(const POINT &ptPos, const TConfiguration *pConfig)override;
};

