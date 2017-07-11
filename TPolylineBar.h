#pragma once
#include "TElement.h"
class TPolylineBar :
	public TElement
{
public:
	TPolylineBar();
	~TPolylineBar();
	virtual const TCHAR * TPolylineBar::GetElementTypeName(TCHAR name[]) override;//得到类型名称
	void TPolylineBar::NoticeListView(TListView *pListView) override;
	virtual void TPolylineBar::Draw(HDC hdc, const TConfiguration* pConfig) override;
	virtual void TPolylineBar::DrawPickSquare(HDC hdc, const TConfiguration* pConfig) override;
	virtual bool TPolylineBar::Picked(const POINT &ptPos, const TConfiguration *pConfig)override;
};

