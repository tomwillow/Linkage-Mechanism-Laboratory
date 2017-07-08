#pragma once
#include <Windows.h>

#include "TElement.h"
#include "DPOINT.h"
class TFramePoint:public TElement
{
public:
	TFramePoint();
	~TFramePoint();
	void TFramePoint::NoticeListView(TListView *pListView) override;
	const TCHAR * TFramePoint::GetElementTypeName(TCHAR name[]) override;//得到类型名称
};

