#pragma once

#include "tchar_head.h"

#include "TShape.h"
#include "TListView.h"
#include "TConstraintColinear.h"


TConstraintColinear::TConstraintColinear()
{
	eType = CONSTRAINT_COLINEAR;
	pElement[0] = NULL;
	pElement[1] = NULL;
	_tcscpy(Name, TEXT(""));
	PointBeginIndexOfElement[0] = -1;
	PointBeginIndexOfElement[1] = -1;
	PointEndIndexOfElement[0] = -1;
	PointEndIndexOfElement[1] = -1;
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

	wsprintf(buffer, TEXT("ID:%d P%d-P%d = ID:%d P%d-P%d"), pElement[0]->id,PointBeginIndexOfElement[0],PointEndIndexOfElement[0],
		pElement[1]->id,PointBeginIndexOfElement[1], PointEndIndexOfElement[1] );

	pListView->AddAttributeItem(TEXT("Value"), CTRLTYPE_NULL, NULL, buffer);
}

bool TConstraintColinear::WriteFile(HANDLE &hf, DWORD &now_pos)
{
	TElement::WriteFile(hf, now_pos);

	::WriteFile(hf, &(pElement[0]->id), sizeof(pElement[0]->id), &now_pos, NULL);
	now_pos += sizeof(pElement[0]->id);
	::WriteFile(hf, &(pElement[1]->id), sizeof(pElement[1]->id), &now_pos, NULL);
	now_pos += sizeof(pElement[1]->id);

	::WriteFile(hf, &(PointBeginIndexOfElement[0]), sizeof(PointBeginIndexOfElement[0]), &now_pos, NULL);
	now_pos += sizeof(PointBeginIndexOfElement[0]);
	::WriteFile(hf, &(PointBeginIndexOfElement[1]), sizeof(PointBeginIndexOfElement[1]), &now_pos, NULL);
	now_pos += sizeof(PointBeginIndexOfElement[1]);
	::WriteFile(hf, &(PointEndIndexOfElement[0]), sizeof(PointEndIndexOfElement[0]), &now_pos, NULL);
	now_pos += sizeof(PointEndIndexOfElement[0]);
	::WriteFile(hf, &(PointEndIndexOfElement[1]), sizeof(PointEndIndexOfElement[1]), &now_pos, NULL);
	now_pos += sizeof(PointEndIndexOfElement[1]);

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

	::ReadFile(hf, &(PointBeginIndexOfElement[0]), sizeof(PointBeginIndexOfElement[0]), &now_pos, NULL);
	now_pos += sizeof(PointBeginIndexOfElement[0]);
	::ReadFile(hf, &(PointBeginIndexOfElement[1]), sizeof(PointBeginIndexOfElement[1]), &now_pos, NULL);
	now_pos += sizeof(PointBeginIndexOfElement[1]);
	::ReadFile(hf, &(PointEndIndexOfElement[0]), sizeof(PointEndIndexOfElement[0]), &now_pos, NULL);
	now_pos += sizeof(PointEndIndexOfElement[0]);
	::ReadFile(hf, &(PointEndIndexOfElement[1]), sizeof(PointEndIndexOfElement[1]), &now_pos, NULL);
	now_pos += sizeof(PointEndIndexOfElement[1]);

	if (GetLastError() != 0)
		return false;
	else
		return true;
}

void TConstraintColinear::GetLinkDpt(int iLinkIndex, DPOINT &P, DPOINT &Q)const
{
	P = pElement[iLinkIndex]->GetAbsolutePointByIndex(PointBeginIndexOfElement[iLinkIndex]);
	Q = pElement[iLinkIndex]->GetAbsolutePointByIndex(PointEndIndexOfElement[iLinkIndex]);
}