#pragma once

#include <tchar.h>
#include <stdio.h>
#include "TConstraintCoincide.h"

#include "TListView.h"

TConstraintCoincide::TConstraintCoincide() 
{
	eType = CONSTRAINT_COINCIDE;
	ElementId1 = -1;
	ElementId2 = -1;
	Element1PointIndex = -1;
	Element2PointIndex = -1;
	eElementType1 = ELEMENT_NULL;
	eElementType2 = ELEMENT_NULL;
	_tcscpy(Name, TEXT(""));
}


TConstraintCoincide::~TConstraintCoincide()
{
}

void TConstraintCoincide::NoticeListView(TListView *pListView)
{
	pListView->DeleteAllItems();

	TCHAR buffer[32];

	pListView->AddAttributeItem(TEXT("ID"), TEXT("%d"), id);
	pListView->AddAttributeItem(TEXT("名称"), Name);
	pListView->AddAttributeItem(TEXT("类型"), TEXT("重合"));
	wsprintf(buffer, TEXT("ID:%d.P%d = ID:%d.P%d"), ElementId1,Element1PointIndex,ElementId2,Element2PointIndex);
	pListView->AddAttributeItem(TEXT("Value"), buffer);
}