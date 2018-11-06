#pragma once
#include <Windows.h>

#include "..\Element\TElement.h"
#include "..\Common\DPOINT.h"
class TFramePoint:public TElement
{
private:
	RECT TFramePoint::GetFramePointBox(const TConfiguration *pConfig);
protected:
	virtual bool TFramePoint::InSelWindow(RECT rect, const TConfiguration *pConfig)override;
	virtual bool TFramePoint::InSelCross(RECT rect, const TConfiguration *pConfig)override;
public:
	TFramePoint();
	~TFramePoint();
	void TFramePoint::NoticeListView(TListView *pListView) override;
	virtual const String TFramePoint::GetElementTypeName() override;//得到类型名称
	virtual void TFramePoint::Draw(HDC hdc, const TConfiguration* pConfig) override;
	virtual void TFramePoint::DrawPickSquare(HDC hdc, const TConfiguration* pConfig) override;
	virtual bool TFramePoint::Picked(const POINT &ptPos, const TConfiguration *pConfig)override;

	virtual bool TFramePoint::WriteFile(HANDLE &hf, DWORD &now_pos)override{return TElement::WriteFile(hf, now_pos);}
	virtual bool TFramePoint::ReadFile(HANDLE &hf, DWORD &now_pos, TShape *pShape) override{return TElement::ReadFile(hf, now_pos,pShape);}

	virtual bool TFramePoint::PointIsAttached(DPOINT dptNowPos, TAttach *pAttach, const TConfiguration *pConfig)override;
};

