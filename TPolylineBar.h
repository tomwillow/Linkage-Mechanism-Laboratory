#pragma once
#include "TElement.h"


class TPolylineBar :
	public TElement
{
protected:
	virtual bool TPolylineBar::InSelWindow(RECT rect, const TConfiguration *pConfig)override;
	virtual bool TPolylineBar::InSelCross(RECT rect, const TConfiguration *pConfig)override;
public:
	TPolylineBar();
	~TPolylineBar();
	virtual const String TPolylineBar::GetElementTypeName() override;//得到类型名称
	void TPolylineBar::NoticeListView(TListView *pListView) override;
	virtual void TPolylineBar::Draw(HDC hdc, const TConfiguration* pConfig) override;
	virtual void TPolylineBar::DrawPickSquare(HDC hdc, const TConfiguration* pConfig) override;
	virtual bool TPolylineBar::Picked(const POINT &ptPos, const TConfiguration *pConfig)override;
	virtual bool TPolylineBar::WriteFile(HANDLE &hf, DWORD &now_pos)override{ return TElement::WriteFile(hf, now_pos); };
	virtual bool TPolylineBar::ReadFile(HANDLE &hf, DWORD &now_pos, TShape *pShape) override{ return TElement::ReadFile(hf, now_pos,pShape); };

	virtual bool TPolylineBar::IsAttached(DPOINT dptNowPos, TAttach *pAttach, const TConfiguration *pConfig)override;
	virtual bool TPolylineBar::PointIsAttached(DPOINT dptNowPos, TAttach *pAttach, const TConfiguration *pConfig)override;
};

