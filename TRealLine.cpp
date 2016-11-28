#pragma once
#include "TRealLine.h"

#include "TDraw.h"
#include "TListView.h"

TRealLine::TRealLine()
{
	TRealLine::dLength = 0;
	_tcscpy(Name, TEXT(""));
	eType = ELEMENT_REALLINE;
}

TRealLine::~TRealLine()
{
}

void TRealLine::NoticeListView(TListView *pListView)
{
	pListView->DeleteAllItems();

	TCHAR buffer[16];

	pListView->AddAttributeItem(TEXT("ID"), TEXT("%d"), id);
	pListView->AddAttributeItem(TEXT("名称"),Name);// stringToTCHAR(&Name,buffer)
	pListView->AddAttributeItem(TEXT("类型"),GetElementTypeName(buffer));
	pListView->AddAttributeItem(TEXT("线型"), GetLineStyleName(this->logpenStyle.lopnStyle, buffer));
	pListView->AddAttributeItem(TEXT("线宽"), TEXT("%d"), this->logpenStyle.lopnWidth);
	pListView->AddAttributeItem(TEXT("颜色"), TEXT("0x%X"), this->logpenStyle.lopnColor);
	pListView->AddAttributeItem(TEXT("P1"), TEXT("%.3f,%.3f"), ptBegin.x, ptBegin.y);
	pListView->AddAttributeItem(TEXT("P2"), TEXT("%.3f,%.3f"), ptEnd.x, ptEnd.y);
	pListView->AddAttributeItem(TEXT("长度"), TEXT("%f"), dLength);
	pListView->AddAttributeItem(TEXT("角度"), TEXT("%f"), REG2DEG(dAngle));
}

void TRealLine::CalcLength()
{
	dLength = sqrt((ptEnd.x - ptBegin.x)*(ptEnd.x - ptBegin.x) + (ptEnd.y - ptBegin.y)*(ptEnd.y - ptBegin.y));
	dAngle = TDraw::GetAngleFromPointReal(ptBegin, ptEnd);
}

void TRealLine::SetPoint(DPOINT dptBegin, DPOINT dptEnd)
{
	TRealLine::ptBegin = dptBegin;
	TRealLine::ptEnd = dptEnd;
	CalcLength();
}

void TRealLine::SetPoint(DPOINT dptBegin, double dLength, double dAngle)
{
	TRealLine::ptBegin = dptBegin;
	TRealLine::ptEnd.x = dptBegin.x+dLength*cos(dAngle);
	TRealLine::ptEnd.y = dptBegin.y+dLength*sin(dAngle);
	this->dLength = dLength;
	this->dAngle = dAngle;
}