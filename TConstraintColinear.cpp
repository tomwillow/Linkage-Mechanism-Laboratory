#pragma once

#include <tchar.h>

#include "TListView.h"
#include "TConstraintColinear.h"


TConstraintColinear::TConstraintColinear()
{
	eType = CONSTRAINT_COLINEAR;
	pElement[0] = NULL;
	pElement[1] = NULL;
	_tcscpy(Name, TEXT(""));
}


TConstraintColinear::~TConstraintColinear()
{
}

void TConstraintColinear::NoticeListView(TListView *pListView)
{
	pListView->DeleteAllItems();

	TCHAR buffer[32];

	pListView->id = id;
	pListView->AddAttributeItem(TEXT("ID"), CTRLTYPE_NULL, NULL, TEXT("%d"), id);
	pListView->AddAttributeItem(TEXT("名称"), CTRLTYPE_EDIT, &Name, Name);
	pListView->AddAttributeItem(TEXT("类型"), CTRLTYPE_NULL, NULL, TEXT("重合"));
	pListView->AddAttributeItem(TEXT("线型"), CTRLTYPE_NULL, NULL, GetLineStyleName(this->logpenStyle.lopnStyle, buffer));
	pListView->AddAttributeItem(TEXT("线宽"), CTRLTYPE_NULL, NULL, TEXT("%d"), this->logpenStyle.lopnWidth);
	pListView->AddAttributeItem(TEXT("颜色"), CTRLTYPE_NULL, NULL, TEXT("0x%X"), this->logpenStyle.lopnColor);

	wsprintf(buffer, TEXT("ID:%d = ID:%d"), pElement[0]->id, pElement[1]->id);

	pListView->AddAttributeItem(TEXT("Value"), CTRLTYPE_NULL, NULL, buffer);
}