#pragma once
#include <tchar.h>

#include "TSlider.h"
#include "TListView.h"
#include "MyMath.h"

TSlider::TSlider()
{
	_tcscpy(Name, TEXT(""));
	eType = ELEMENT_SLIDER;
	
}


TSlider::~TSlider()
{
}


void TSlider::NoticeListView(TListView *pListView)
{
	pListView->DeleteAllItems();

	TCHAR buffer[16];

	pListView->id = id;
	pListView->AddAttributeItem(TEXT("ID"), CTRLTYPE_NULL, NULL, TEXT("%d"), id);
	pListView->AddAttributeItem(TEXT("名称"), CTRLTYPE_EDIT,&Name, Name);// stringToTCHAR(&Name,buffer)
	pListView->AddAttributeItem(TEXT("类型"), CTRLTYPE_NULL, NULL, GetElementTypeName(buffer));
	pListView->AddAttributeItem(TEXT("线型"), CTRLTYPE_NULL, NULL, GetLineStyleName(this->logpenStyle.lopnStyle, buffer));
	pListView->AddAttributeItem(TEXT("线宽"), CTRLTYPE_NULL, NULL, TEXT("%d"), this->logpenStyle.lopnWidth);
	pListView->AddAttributeItem(TEXT("颜色"), CTRLTYPE_NULL, NULL, TEXT("0x%X"), this->logpenStyle.lopnColor);
	pListView->AddAttributeItem(TEXT("原点"), CTRLTYPE_COOR_EDIT, &dpt, TEXT("%.3f,%.3f"), dpt.x,dpt.y);
	pListView->AddAttributeItem(TEXT("角度"), CTRLTYPE_ANGLE_VALUE_EDIT, &angle, TEXT("%f"), REG2DEG(angle));
	for (auto i = 0; i < vecDpt.size(); ++i)
	{
		_stprintf(buffer, TEXT("P%d"), i);
		pListView->AddAttributeItem(buffer, CTRLTYPE_COOR_EDIT, &(vecDpt[i]), TEXT("%.3f,%.3f"), vecDpt[i].x, vecDpt[i].y);
	}
	//pListView->AddAttributeItem(TEXT("P2"), CTRLTYPE_COOR_P2_EDIT, &ptEnd, TEXT("%.3f,%.3f"), ptEnd.x, ptEnd.y);
	//pListView->AddAttributeItem(TEXT("长度"), CTRLTYPE_LEN_EDIT, NULL, TEXT("%f"), dLength);
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