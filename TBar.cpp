#pragma once
#include <tchar.h>
#include <stdio.h>

#include "TBar.h"

TBar::TBar()
{
	TBar::dLength = 0;
	_tcscpy(Name, TEXT(""));
	eType = ELEMENT_BAR;
}

TBar::~TBar()
{
}

//不通知listview，由realline通知