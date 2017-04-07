#pragma once
#include "tchar_head.h"

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
	TElement::NoticeListView(pListView);

	TCHAR buffer[16];

	pListView->AddAttributeItem(TEXT("Ô­µã"), CTRLTYPE_COOR_EDIT, &dpt, TEXT("%.3f,%.3f"), dpt.x, dpt.y);
	pListView->AddAttributeItem(TEXT("½Ç¶È"), CTRLTYPE_ANGLE_VALUE_EDIT, &angle, TEXT("%f"), REG2DEG(angle));

	for (auto i = 0; i < vecDpt.size(); ++i)
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