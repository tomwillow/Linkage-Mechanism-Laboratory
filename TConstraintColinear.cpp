#pragma once

#include <tchar.h>

#include "TShape.h"
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

bool TConstraintColinear::WriteFile(HANDLE &hf, DWORD &now_pos)
{
	TElement::WriteFile(hf, now_pos);

	::WriteFile(hf, &(pElement[0]->id), sizeof(pElement[0]->id), &now_pos, NULL);
	now_pos += sizeof(pElement[0]->id);
	::WriteFile(hf, &(pElement[1]->id), sizeof(pElement[1]->id), &now_pos, NULL);
	now_pos += sizeof(pElement[1]->id);

	if (GetLastError() != ERROR_ALREADY_EXISTS && GetLastError() != 0)
		return false;
	else
		return true;
}

bool TConstraintColinear::ReadFile(HANDLE &hf, DWORD &now_pos,TShape *pShape)
{
	TElement::ReadFile(hf, now_pos,pShape);

	int id0, id1;
	::ReadFile(hf, &id0, sizeof(id0), &now_pos, NULL);
	now_pos += sizeof(id0);
	::ReadFile(hf, &id1, sizeof(id1), &now_pos, NULL);
	now_pos += sizeof(id1);

	pElement[0] = pShape->GetElementById(id0);
	pElement[1] = pShape->GetElementById(id1);

	if (GetLastError() != 0)
		return false;
	else
		return true;
}