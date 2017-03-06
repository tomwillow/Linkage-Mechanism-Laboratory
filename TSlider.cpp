#pragma once
#include <tchar.h>

#include "TSlider.h"



TSlider::TSlider()
{
	_tcscpy(Name, TEXT(""));
	eType = ELEMENT_SLIDER;
	
}


TSlider::~TSlider()
{
}
