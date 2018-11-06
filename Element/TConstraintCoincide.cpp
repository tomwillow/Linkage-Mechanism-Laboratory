#pragma once

#include "..\Common\tchar_head.h"
#include <stdio.h>

#include "..\Element\TConstraintCoincide.h"

#include "..\Draw\TDraw.h"
#include "..\TConfiguration.h"
#include "..\Element\TShape.h"

#include "..\Control\TListView.h"

TConstraintCoincide::TConstraintCoincide() 
{
	eType = CONSTRAINT_COINCIDE;
	eClass = ELEMENT_CLASS_CONSTRAINT;
	_tcscpy(Name, TEXT(""));

}


TConstraintCoincide::~TConstraintCoincide()
{
}

const String TConstraintCoincide::GetElementTypeName()//得到类型名称
{
	return TEXT("重合");
}

void TConstraintCoincide::RestorePointStyle()
{
	for (int i = 0; i < 2; i++)
		if (pElement[i] != NULL)
			for (auto iter = pElement[i]->vecIsJoint.begin(); iter != pElement[i]->vecIsJoint.end(); ++iter)
				for (auto iterId = (*iter).begin(); iterId != (*iter).end(); ++iterId)
				{
					if ((*iterId) == id)
					{
						iter->erase(iterId);
						break;
					}
				}

}

void TConstraintCoincide::BuildpDpt_inner(int i)
{
	//pDpt[i] = &(pElement[i]->GetRelativePointByIndex(PointIndexOfElement[i]));
	pElement[i]->vecIsJoint[PointIndexOfElement[i]].push_back(id);
}

//根据pElement和PointIndexOfElement得到pDpt，并设置vecIsJoint
void TConstraintCoincide::BuildpDpt()
{
	for (int i = 0; i < 2; i++)//0-1
	{
		BuildpDpt_inner(i);
	}
}

DPOINT TConstraintCoincide::GetLinkDpt(int index)
{
	return pElement[index]->GetAbsolutePointByIndex(PointIndexOfElement[index]);
}

void TConstraintCoincide::NoticeListView(TListView *pListView)
{
	pListView->DeleteAllItems();

	TCHAR buffer[32];

	pListView->id = id;
	pListView->AddAttributeItem(TEXT("ID"), CTRLTYPE_NULL, NULL, TEXT("%d"), id);
	pListView->AddAttributeItem(TEXT("名称"), CTRLTYPE_EDIT, &Name, Name);
	pListView->AddAttributeItem(TEXT("类型"), CTRLTYPE_NULL, NULL,GetElementTypeName().c_str());
	pListView->AddAttributeItem(TEXT("线型"), CTRLTYPE_NULL, NULL, GetLineStyleName(this->logpenStyle.lopnStyle).c_str());
	pListView->AddAttributeItem(TEXT("线宽"), CTRLTYPE_NULL, NULL, TEXT("%d"), this->logpenStyle.lopnWidth);
	pListView->AddAttributeItem(TEXT("颜色"), CTRLTYPE_NULL, NULL, TEXT("0x%X"), this->logpenStyle.lopnColor);

	wsprintf(buffer, TEXT("ID:%d.P%d = ID:%d.P%d"), pElement[0]->id, PointIndexOfElement[0], pElement[1]->id, PointIndexOfElement[1]);

	pListView->AddAttributeItem(TEXT("Value"), CTRLTYPE_NULL, NULL, buffer);
}

//保存时pElement存入id
bool TConstraintCoincide::WriteFile(HANDLE &hf, DWORD &now_pos)
{
	if (!TConstraint::WriteFile(hf, now_pos))
		return false;

	::WriteFile(hf, &(PointIndexOfElement[0]), sizeof(PointIndexOfElement[0]), &now_pos, NULL);
	now_pos += sizeof(PointIndexOfElement[0]);
	::WriteFile(hf, &(PointIndexOfElement[1]), sizeof(PointIndexOfElement[1]), &now_pos, NULL);
	now_pos += sizeof(PointIndexOfElement[1]);

	if (GetLastError() != ERROR_ALREADY_EXISTS && GetLastError() != 0)
		return false;
	else
		return true;
}

//读出时根据pElement中保存的id重新指向对应的元素
bool TConstraintCoincide::ReadFile(HANDLE &hf, DWORD &now_pos, TShape *pShape)
{
	if (!TConstraint::ReadFile(hf, now_pos, pShape))
		return false;

	::ReadFile(hf, &(PointIndexOfElement[0]), sizeof(PointIndexOfElement[0]), &now_pos, NULL);
	now_pos += sizeof(PointIndexOfElement[0]);
	::ReadFile(hf, &(PointIndexOfElement[1]), sizeof(PointIndexOfElement[1]), &now_pos, NULL);
	now_pos += sizeof(PointIndexOfElement[1]);

	BuildpDpt();

	if (GetLastError() != 0)
		return false;
	else
		return true;
}

void TConstraintCoincide::Draw(HDC hdc, const TConfiguration* pConfig)
{
	TDraw::DrawConstraintCoincide(hdc, this, pConfig);
	//if (bDrawSquare) DrawPickSquare(hdc, pConfig);
}

void TConstraintCoincide::DrawPickSquare(HDC hdc, const TConfiguration* pConfig)
{
	if (TDraw::ShowConstraintCoincideDotLine(this, pConfig))
	{
		//画拾取方格
		TDraw::DrawPickSquare(hdc, pConfig->RealToScreen(GetLinkDpt(0)));
		TDraw::DrawPickSquare(hdc, pConfig->RealToScreen(GetLinkDpt(1)));
	}
	else
	{
		//画重合点
		TDraw::DrawPickSquare(hdc, pConfig->RealToScreen(GetLinkDpt(0)));
	}
}

bool TConstraintCoincide::Picked(const POINT &ptPos, const TConfiguration *pConfig)
{
	return TDraw::PickConstraintCoincide(ptPos,this, pConfig);
}

//TConstraintCoincide& TConstraintCoincide::operator=(const TConstraintCoincide &element)
//{
//	TElement::operator=(element);
//
//	BuildpDpt();
//
//	return *this;
//}