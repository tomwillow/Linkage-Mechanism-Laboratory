#pragma once
#include "..\Common\tchar_head.h"

#include "TSlider.h"

#include "..\Tool\TAttach.h"
#include "..\TConfiguration.h"
#include "..\Draw\TDraw.h"
#include "..\Control\TListView.h"
#include "..\Common\MyMath.h"

TSlider::TSlider()
{
	_tcscpy(Name, TEXT(""));
	eType = ELEMENT_SLIDER;
	eClass = ELEMENT_CLASS_NORMAL;
	
	dpt_1 = { 1, 0 };

}


TSlider::~TSlider()
{
}

const String TSlider::GetElementTypeName()//得到类型名称
{
	return TEXT("滑块");
}

void TSlider::NoticeListView(TListView *pListView)
{
	TElement::NoticeListView(pListView);

	TCHAR buffer[16];

	pListView->AddAttributeItem(TEXT("原点"), CTRLTYPE_COOR_EDIT, &dpt, TEXT("%.3f,%.3f"), dpt.x, dpt.y);
	pListView->AddAttributeItem(TEXT("角度"), CTRLTYPE_ANGLE_VALUE_EDIT, &angle, TEXT("%f"), RAD2DEG(angle));

	for (size_t i = 0; i < vecDpt.size(); ++i)
	{
		_stprintf(buffer, TEXT("P%d"), i);
		pListView->AddAttributeItem(buffer, CTRLTYPE_COOR_EDIT, &(vecDpt[i]), TEXT("%.3f,%.3f"), vecDpt[i].x, vecDpt[i].y);
	}
}

bool TSlider::WriteFile(HANDLE &hf, DWORD &now_pos)
{
	TElement::WriteFile(hf, now_pos);

	size_t vecLineSize = vecLine.size();
	::WriteFile(hf, &(vecLineSize), sizeof(vecLineSize), &now_pos, NULL);
	now_pos += sizeof(vecLineSize);

	for (auto iLine : vecLine)
	{
		::WriteFile(hf, &iLine, sizeof(iLine), &now_pos, NULL);
		now_pos += sizeof(iLine);
	}

	if (GetLastError() != ERROR_ALREADY_EXISTS && GetLastError() != 0)
		return false;
	else
		return true;
}

bool TSlider::ReadFile(HANDLE &hf, DWORD &now_pos,TShape *pShape)
{
	TElement::ReadFile(hf, now_pos,pShape);

	size_t vecLineSize;
	::ReadFile(hf, &(vecLineSize), sizeof(vecLineSize), &now_pos, NULL);
	now_pos += sizeof(vecLineSize);

	LinkLine tempLink;
	for (size_t i = 0; i < vecLineSize;++i)
	{
		::ReadFile(hf, &tempLink, sizeof(tempLink), &now_pos, NULL);
		now_pos += sizeof(tempLink);
		vecLine.push_back(tempLink);
	}

	if (GetLastError() != 0)
		return false;
	else
		return true;
}

//Slider的-1点为(1,0)点，用于指定滑轨
const DPOINT TSlider::GetRelativePointByIndex(int PointIndexOfElement)const 
{
	if (PointIndexOfElement == -1)
		return dpt_1;
	else
		return vecDpt[PointIndexOfElement];
}

//Slider的-1点为(1,0)点，绝对坐标设置为原点
DPOINT TSlider::GetAbsolutePointByIndex(int PointIndexOfElement) const
{
	if (PointIndexOfElement == -1)
		return GetAbsolute(dpt_1, dpt, angle);
	else
		return TElement::GetAbsolutePointByIndex(PointIndexOfElement);
}

void TSlider::Draw(HDC hdc, const TConfiguration* pConfig)
{
	TDraw::DrawSlider(hdc, this, pConfig);
	//if (bDrawSquare) DrawPickSquare(hdc, pConfig);
}

void TSlider::DrawPickSquare(HDC hdc, const TConfiguration* pConfig)
{
	for (auto &Dpt : vecDpt)
		//画拾取方格
		TDraw::DrawPickSquare(hdc, pConfig->RealToScreen(TDraw::GetAbsolute(Dpt, dpt, angle)));
}

bool TSlider::Picked(const POINT &ptPos, const TConfiguration *pConfig)
{
	return TDraw::PointInSlider(ptPos, this, pConfig);
}

bool TSlider::InSelWindow(RECT rect, const TConfiguration *pConfig)
{
	POINT apt[4];
	TDraw::CalcSliderRectCoor(apt, pConfig->RealToScreen(dpt), angle, pConfig);

	std::vector<POINT> vecpt;
	for (auto link : vecLine)
	{
		vecpt.push_back(pConfig->RealToScreen(this->GetAbsolutePointByIndex(link.index1)));
		vecpt.push_back(pConfig->RealToScreen(this->GetAbsolutePointByIndex(link.index2)));
	}

	return APointInRect(rect, apt, 4) && VecPointInRect(rect,vecpt);
}

bool TSlider::InSelCross(RECT rect, const TConfiguration *pConfig)
{
	POINT apt[4];
	TDraw::CalcSliderRectCoor(apt, pConfig->RealToScreen(dpt), angle, pConfig);

	std::vector<POINT> vecpt;
	for (auto link : vecLine)
	{
		vecpt.push_back(pConfig->RealToScreen(this->GetAbsolutePointByIndex(link.index1)));
		vecpt.push_back(pConfig->RealToScreen(this->GetAbsolutePointByIndex(link.index2)));
	}

	return APointCrossRect(rect, apt, 4) || VecPointCrossRect(rect, vecpt);
}

bool TSlider::IsAttached(DPOINT dptNowPos, TAttach *pAttach, const TConfiguration *pConfig)
{
	if (pAttach->AttachLineByRelativeVecPt(dptNowPos, this, vecDpt, pConfig))
		return true;

	std::vector<DPOINT> vecdptAbsoluteSlider;
	vecdptAbsoluteSlider.push_back(GetAbsolutePointByIndex(-1));
	vecdptAbsoluteSlider.push_back(GetAbsolutePointByIndex(0));

	if (pAttach->AttachLineByAbsoluteVecPt(dptNowPos,this, vecdptAbsoluteSlider))
	{
		pAttach->iAttachLinePointIndex[0] = -1;
		pAttach->iAttachLinePointIndex[1] = 0;
		return true;
	}
	return false;
}

bool TSlider::PointIsAttached(DPOINT dptNowPos, TAttach *pAttach, const TConfiguration *pConfig)
{
	for (auto iter = vecDpt.begin(); iter != vecDpt.end(); ++iter)
	{
		if (pAttach->AttachPointByElement(dptNowPos, TDraw::GetAbsolute(*iter, dpt, angle), iter - vecDpt.begin(), this, pConfig))
			return true;
	}
	return false;
}