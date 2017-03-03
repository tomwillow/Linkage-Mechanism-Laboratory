#pragma once
#include "TFramePoint.h"

#include "TListView.h"
TFramePoint::TFramePoint()
{
	_tcscpy(Name, TEXT(""));
	eType = ELEMENT_FRAMEPOINT;

	vecIsJoint.resize(1);
}


TFramePoint::~TFramePoint()
{
}

void TFramePoint::NoticeListView(TListView *pListView)
{
	pListView->DeleteAllItems();

	TCHAR buffer[16];

	pListView->id = id;
	pListView->AddAttributeItem(TEXT("ID"),CTRLTYPE_NULL,NULL, TEXT("%d"), id);
	pListView->AddAttributeItem(TEXT("名称"), CTRLTYPE_EDIT, &Name, Name);
	pListView->AddAttributeItem(TEXT("类型"), CTRLTYPE_NULL, NULL, TEXT("机架"));
	pListView->AddAttributeItem(TEXT("线型"), CTRLTYPE_NULL, NULL, GetLineStyleName(this->logpenStyle.lopnStyle, buffer));
	pListView->AddAttributeItem(TEXT("线宽"), CTRLTYPE_NULL, NULL, TEXT("%d"), this->logpenStyle.lopnWidth);
	pListView->AddAttributeItem(TEXT("颜色"), CTRLTYPE_NULL, NULL, TEXT("0x%X"), this->logpenStyle.lopnColor);
	pListView->AddAttributeItem(TEXT("P0"), CTRLTYPE_COOR_EDIT, &dpt, TEXT("%.3f,%.3f"), dpt.x, dpt.y);
}