#pragma once
#include "DetectMemoryLeak.h"
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

const TCHAR * TFramePoint::GetElementTypeName(TCHAR name[])//得到类型名称
{
	return _tcscpy(name, TEXT("机架"));
}

void TFramePoint::NoticeListView(TListView *pListView)
{
	TElement::NoticeListView(pListView);

	pListView->AddAttributeItem(TEXT("P0"), CTRLTYPE_COOR_EDIT, &dpt, TEXT("%.3f,%.3f"), dpt.x, dpt.y);
}