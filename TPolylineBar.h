#pragma once
#include "TElement.h"
class TPolylineBar :
	public TElement
{
public:
	TPolylineBar();
	~TPolylineBar();
	void TPolylineBar::NoticeListView(TListView *pListView) override;
};
