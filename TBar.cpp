#pragma once
#include "tchar_head.h"
#include <stdio.h>

#include "TDraw.h"
#include "TBar.h"

TBar::TBar()
{
	TBar::dLength = 0;
	_tcscpy(Name, TEXT(""));
	eType = ELEMENT_BAR;

	CanBeDragged = true;
}

TBar::~TBar()
{
}

const String TBar::GetElementTypeName()//得到类型名称
{
	return TEXT("连杆");
}

//不通知listview，由realline通知

void TBar::Draw(HDC hdc, const TConfiguration* pConfig)
{
	TDraw::DrawBar(hdc, this, pConfig);
	//if (bDrawSquare) DrawPickSquare(hdc, pConfig);
}

TBar& TBar::operator=(const TRealLine &RealLine)
{
	TRealLine::operator=(RealLine);
	return *this;
}