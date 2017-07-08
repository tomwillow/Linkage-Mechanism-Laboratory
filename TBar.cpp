#pragma once
#include "tchar_head.h"
#include <stdio.h>

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

const TCHAR * TBar::GetElementTypeName(TCHAR name[])//得到类型名称
{
	return _tcscpy(name, TEXT("连杆"));
}

//不通知listview，由realline通知