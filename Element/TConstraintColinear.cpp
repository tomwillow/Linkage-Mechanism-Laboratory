#pragma once

#include "..\Common\tchar_head.h"

#include "..\Draw\TDraw.h"
#include "..\Element\TShape.h"
#include "..\Control\TListView.h"
#include "TConstraintColinear.h"


TConstraintColinear::TConstraintColinear()
{
	eType = CONSTRAINT_COLINEAR;
	eClass = ELEMENT_CLASS_CONSTRAINT;
	_tcscpy(Name, TEXT(""));
	PointBeginIndexOfElement[0] = -1;
	PointBeginIndexOfElement[1] = -1;
	PointEndIndexOfElement[0] = -1;
	PointEndIndexOfElement[1] = -1;

}


TConstraintColinear::~TConstraintColinear()
{
}

const String TConstraintColinear::GetElementTypeName()//得到类型名称
{
	return TEXT("共线");
}

void TConstraintColinear::NoticeListView(TListView *pListView)
{
	pListView->DeleteAllItems();

	TCHAR buffer[64];

	pListView->id = id;
	pListView->AddAttributeItem(TEXT("ID"), CTRLTYPE_NULL, NULL, TEXT("%d"), id);
	pListView->AddAttributeItem(TEXT("名称"), CTRLTYPE_EDIT, &Name, Name);
	pListView->AddAttributeItem(TEXT("类型"), CTRLTYPE_NULL, NULL, GetElementTypeName().c_str());
	pListView->AddAttributeItem(TEXT("线型"), CTRLTYPE_NULL, NULL, GetLineStyleName(this->logpenStyle.lopnStyle).c_str());
	pListView->AddAttributeItem(TEXT("线宽"), CTRLTYPE_NULL, NULL, TEXT("%d"), this->logpenStyle.lopnWidth);
	pListView->AddAttributeItem(TEXT("颜色"), CTRLTYPE_NULL, NULL, TEXT("0x%X"), this->logpenStyle.lopnColor);

	wsprintf(buffer, TEXT("ID:%d P%d-P%d = ID:%d P%d-P%d"), pElement[0]->id,PointBeginIndexOfElement[0],PointEndIndexOfElement[0],
		pElement[1]->id,PointBeginIndexOfElement[1], PointEndIndexOfElement[1] );

	pListView->AddAttributeItem(TEXT("Value"), CTRLTYPE_NULL, NULL, buffer);
}

bool TConstraintColinear::WriteFile(HANDLE &hf, DWORD &now_pos)
{
	if (!TConstraint::WriteFile(hf, now_pos))
		return false;

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
	if (!TConstraint::ReadFile(hf, now_pos, pShape))
		return false;

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

void TConstraintColinear::Draw(HDC hdc, const TConfiguration* pConfig)
{
	TDraw::DrawConstraintColinear(hdc, this, pConfig);
	//if (bDrawSquare) DrawPickSquare(hdc, pConfig);
}

void TConstraintColinear::DrawPickSquare(HDC hdc, const TConfiguration* pConfig)
{
	POINT ptCenter1, ptCenter2;
	if (TDraw::ShowConstraintColinearDotLine(this, ptCenter1, ptCenter2, pConfig))
	{
		//画拾取方格
		TDraw::DrawPickSquare(hdc, ptCenter1);
		TDraw::DrawPickSquare(hdc, ptCenter2);
	}
	else
	{
		//画重合点
		TDraw::DrawPickSquare(hdc, ptCenter1);
	}
}

bool TConstraintColinear::Picked(const POINT &ptPos, const TConfiguration *pConfig)
{
	return TDraw::PickConstraintColinear(ptPos,this, pConfig);
}