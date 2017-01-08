#pragma once
#include <Windows.h>

#include "TElement.h"
#include "DPOINT.h"
class TFramePoint:public TElement
{
public:
	TFramePoint();
	~TFramePoint();
	void TFramePoint::NoticeListView(TListView *pListView);
};

