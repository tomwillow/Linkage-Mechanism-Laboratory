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
};

