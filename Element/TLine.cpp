#pragma once
#include "..\Common\tchar_head.h"
#include <stdio.h>

#include "..\Draw\TDraw.h"
#include "..\Element\TLine.h"

TLine::TLine() :dAngle(angle)
{
	TLine::dLength = 0;
	_tcscpy(Name, TEXT("TLine"));
	eClass = ELEMENT_CLASS_NONE;
}

TLine::~TLine()
{
}

const String TLine::GetElementTypeName()//得到类型名称
{
	return TEXT("屏幕线");
}

TLine& TLine::operator=(const TLine &Line)
{
	TElement::operator=(Line);
	ptBegin = Line.ptBegin;
	ptEnd = Line.ptEnd;
	dLength = Line.dLength;
	return *this;
}