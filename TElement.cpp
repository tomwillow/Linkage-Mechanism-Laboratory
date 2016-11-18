#pragma once
#include <stdio.h>

#include "TElement.h"

#include "TListView.h"
TElement::TElement()
{
	id = -1;
	eType = ELEMENT_NULL;
	_tcscpy(Name,TEXT("undefined"));
}

TElement::~TElement()
{
	
}

void TElement::SetStyle(int iStyle, int iWidth, COLORREF crColor)
{
	logpenStyle.lopnStyle = iStyle;
	logpenStyle.lopnWidth.x = iWidth;
	logpenStyle.lopnColor = crColor;

	logpenStyleShow = logpenStyle;
}

TCHAR * TElement::GetElementTypeName(TCHAR name[])
{
	switch (eType)
	{
	case ELEMENT_REALLINE:
		_tcscpy(name, TEXT("线"));
		break;
	case ELEMENT_FRAMEPOINT:
		_tcscpy(name, TEXT("机架"));
		break;
	case ELEMENT_BAR:
		_tcscpy(name, TEXT("连杆"));
		break;
	case CONSTRAINT_COINCIDE:
		_tcscpy(name, TEXT("重合"));
		break;

	}
	return name;
}

TCHAR * TElement::GetLineStyleName(UINT linestyle, TCHAR name[])
{
	switch (linestyle)
	{
	case PS_SOLID:
		_tcscpy(name, TEXT("实线"));
		break;
	case PS_DASH:
		_tcscpy(name, TEXT("线段"));
		break;
	case PS_DOT:
		_tcscpy(name, TEXT("虚线"));
		break;
	case PS_DASHDOT:
		_tcscpy(name, TEXT("点划线"));
		break;
	case PS_DASHDOTDOT:
		_tcscpy(name, TEXT("双点划线"));
		break;
	}
	return name;
}