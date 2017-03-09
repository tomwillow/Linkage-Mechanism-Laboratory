#pragma once
#include "DetectMemoryLeak.h"
#include <stdio.h>

#include "TElement.h"

#include "TListView.h"
TElement::TElement() :available(true)
{
	id = -1;
	eType = ELEMENT_NULL;
	dpt = { 0, 0 };
	angle = 0.0;

	_tcscpy(Name,TEXT("undefined"));

}

TElement::~TElement()
{
	
}

void TElement::SetStyle(const LOGPEN &logpen)//设置样式
{
	logpenStyleShow = logpenStyle = logpen;
}

//自动设置logpenStyle和logpenStyleShow
void TElement::SetStyle(int iStyle, int iWidth, COLORREF crColor)
{
	logpenStyle.lopnStyle = iStyle;
	logpenStyle.lopnWidth = { iWidth, 0 };
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
	case ELEMENT_SLIDEWAY:
		_tcscpy(name, TEXT("滑道"));
		break;
	case CONSTRAINT_COINCIDE:
		_tcscpy(name, TEXT("重合"));
		break;
	case ELEMENT_SLIDER:
		_tcscpy(name, TEXT("滑块"));
		break;
	default:
		assert(0);
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
	default:
		assert(0);
		break;
	}
	return name;
}

TElement& TElement::operator=(const TElement &element)
{
	this->id = element.id;
	this->available = element.available;
	_tcscpy(this->Name, element.Name);
	this->logpenStyle = element.logpenStyle;
	this->logpenStyleShow = element.logpenStyleShow;
	this->vecDpt = element.vecDpt;
	this->vecIsJoint = element.vecIsJoint;
	this->dpt = element.dpt;
	this->angle = element.angle;
	return *this;
}