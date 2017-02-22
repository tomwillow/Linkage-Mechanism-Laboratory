#pragma once

#include <tchar.h>
#include <stdio.h>

#include "TDraw.h"
#include "TConstraintCoincide.h"

#include "TListView.h"
#include "TRealLine.h"

TConstraintCoincide::TConstraintCoincide()
{
	eType = CONSTRAINT_COINCIDE;
	pElement1 = NULL;
	pElement2 = NULL;
	pDpt1 = NULL;
	pDpt2 = NULL;
	_tcscpy(Name, TEXT(""));
}


TConstraintCoincide::~TConstraintCoincide()
{
}

void TConstraintCoincide::BuildpDpt()
{
	switch (Element1PointIndex)
	{
	case 0:
	case 1:
		pDpt1 = &(pElement1->dpt);
		break;
	case 2:
		pDpt1 = &(((TRealLine *)this->pElement1)->ptEnd);
	}

	switch (Element2PointIndex)
	{
	case 0:
	case 1:
		pDpt2 = &(pElement2->dpt);
		break;
	case 2:
		pDpt2 = &(((TRealLine *)this->pElement2)->ptEnd);
	}
}

void TConstraintCoincide::NoticeListView(TListView *pListView)
{
	pListView->DeleteAllItems();

	TCHAR buffer[32];

	pListView->id = id;
	pListView->AddAttributeItem(TEXT("ID"), CTRLTYPE_NULL,NULL, TEXT("%d"), id);
	pListView->AddAttributeItem(TEXT("名称"), CTRLTYPE_EDIT,&Name, Name);
	pListView->AddAttributeItem(TEXT("类型"), CTRLTYPE_NULL, NULL, TEXT("重合"));
	pListView->AddAttributeItem(TEXT("线型"), CTRLTYPE_NULL, NULL, GetLineStyleName(this->logpenStyle.lopnStyle, buffer));
	pListView->AddAttributeItem(TEXT("线宽"), CTRLTYPE_NULL, NULL, TEXT("%d"), this->logpenStyle.lopnWidth);
	pListView->AddAttributeItem(TEXT("颜色"), CTRLTYPE_NULL, NULL, TEXT("0x%X"), this->logpenStyle.lopnColor);
	wsprintf(buffer, TEXT("ID:%d.P%d = ID:%d.P%d"), CTRLTYPE_NULL, NULL, pElement1->id, Element1PointIndex, pElement2->id, Element2PointIndex);
	pListView->AddAttributeItem(TEXT("Value"), CTRLTYPE_NULL, NULL, buffer);
}