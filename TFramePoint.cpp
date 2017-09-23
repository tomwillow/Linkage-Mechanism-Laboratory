#pragma once
#include "DetectMemoryLeak.h"
#include "TFramePoint.h"

#include "TConfiguration.h"
#include "TDraw.h"
#include "TListView.h"
TFramePoint::TFramePoint()
{
	_tcscpy(Name, TEXT(""));
	eType = ELEMENT_FRAMEPOINT;
	eClass = ELEMENT_CLASS_FRAME;

	vecIsJoint.resize(1);
}


TFramePoint::~TFramePoint()
{
}

const String TFramePoint::GetElementTypeName()//得到类型名称
{
	return TEXT("机架");
}

void TFramePoint::NoticeListView(TListView *pListView)
{
	TElement::NoticeListView(pListView);

	pListView->AddAttributeItem(TEXT("P0"), CTRLTYPE_COOR_EDIT, &dpt, TEXT("%.3f,%.3f"), dpt.x, dpt.y);
}

void TFramePoint::Draw(HDC hdc, const TConfiguration* pConfig)
{
	TDraw::DrawFramePoint(hdc, this, pConfig);
	//if (bDrawSquare) DrawPickSquare(hdc, pConfig);
}

RECT TFramePoint::GetFramePointBox(const TConfiguration *pConfig)
{
	RECT rect;
	POINT pt = pConfig->RealToScreen(dpt);
	rect.left = pt.x - pConfig->FRAMEPOINT_SECTION_H / 2;
	rect.right = pt.x + pConfig->FRAMEPOINT_SECTION_H / 2;
	rect.top = pt.y - pConfig->FRAMEPOINT_R;
	rect.bottom = pt.y + pConfig->FRAMEPOINT_H + pConfig->FRAMEPOINT_SECTION_H;
	return rect;
}

void TFramePoint::DrawPickSquare(HDC hdc, const TConfiguration* pConfig)
{
	//画拾取方格
	TDraw::DrawPickSquare(hdc, pConfig->RealToScreen(dpt));
}

bool TFramePoint::Picked(const POINT &ptPos, const TConfiguration *pConfig)
{
	return TDraw::PointInFramePoint(pConfig->RealToScreen(dpt), ptPos, pConfig);
}

bool TFramePoint::InSelWindow(RECT rect, const TConfiguration *pConfig)
{
	RegularRect(rect);
	RECT rcBox = GetFramePointBox(pConfig);
	
	return RectInRect(rect, rcBox);
}

bool TFramePoint::InSelCross(RECT rect, const TConfiguration *pConfig)
{
	RegularRect(rect);
	RECT rcBox = GetFramePointBox(pConfig);
	return RectCrossRect(rect, rcBox);
}