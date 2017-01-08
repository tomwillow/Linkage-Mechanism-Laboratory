#pragma once
#include "TFramePoint.h"

#include "TListView.h"
TFramePoint::TFramePoint()
{
	_tcscpy(Name, TEXT(""));
	eType = ELEMENT_FRAMEPOINT;
}


TFramePoint::~TFramePoint()
{
}

void TFramePoint::NoticeListView(TListView *pListView)
{
	pListView->DeleteAllItems();

	TCHAR buffer[16];

	pListView->AddAttributeItem(TEXT("ID"), TEXT("%d"), id);
	pListView->AddAttributeItem(TEXT("名称"), Name);
	pListView->AddAttributeItem(TEXT("类型"), TEXT("机架"));
	pListView->AddAttributeItem(TEXT("线型"), GetLineStyleName(this->logpenStyle.lopnStyle, buffer));
	pListView->AddAttributeItem(TEXT("线宽"), TEXT("%d"), this->logpenStyle.lopnWidth);
	pListView->AddAttributeItem(TEXT("颜色"), TEXT("0x%X"), this->logpenStyle.lopnColor);
	pListView->AddAttributeItem(TEXT("P0"), TEXT("%.3f,%.3f"),dpt.x, dpt.y);
}