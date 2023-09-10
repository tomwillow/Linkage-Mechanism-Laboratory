#pragma once
#include "..\Common\tchar_head.h"
#include <stdio.h>

#include "TSlideway.h"

#include "..\Draw\TDraw.h"
#include "..\Control\TListView.h"

TSlideway::TSlideway()
{
	_tcscpy(Name, TEXT(""));
	eType = ELEMENT_SLIDEWAY;
	eClass = ELEMENT_CLASS_FRAME;

	ShadowQuadrant = 4;
	ShadowLength = 40;
	SlidewayStyle = 0;
}

const String TSlideway::GetElementTypeName()//得到类型名称
{
	return TEXT("固定导轨");
}

TSlideway::~TSlideway()
{
}
TSlideway& TSlideway::operator=(const TRealLine &RealLine)
{
	TRealLine::operator=(RealLine);
	return *this;
}

TSlideway& TSlideway::operator=(const TSlideway &Slideway)
{
	TRealLine::operator=(Slideway);

	this->ShadowLength = Slideway.ShadowLength;
	this->ShadowQuadrant = Slideway.ShadowQuadrant;
	return *this;
}

bool TSlideway::WriteFile(HANDLE &hf, DWORD &now_pos)
{
	TRealLine::WriteFile(hf, now_pos);

	::WriteFile(hf, &ShadowLength, sizeof(ShadowLength), &now_pos, NULL);
	now_pos += sizeof(ShadowLength);
	::WriteFile(hf, &ShadowQuadrant, sizeof(ShadowQuadrant), &now_pos, NULL);
	now_pos += sizeof(ShadowQuadrant);
	::WriteFile(hf, &SlidewayStyle, sizeof(SlidewayStyle), &now_pos, NULL);
	now_pos += sizeof(SlidewayStyle);

	if (GetLastError() != ERROR_ALREADY_EXISTS && GetLastError() != 0)
		return false;
	else
		return true;
}

bool TSlideway::ReadFile(HANDLE &hf, DWORD &now_pos,TShape *pShape)
{
	TRealLine::ReadFile(hf, now_pos,pShape);

	::ReadFile(hf, &ShadowLength, sizeof(ShadowLength), &now_pos, NULL);
	now_pos += sizeof(ShadowLength);
	::ReadFile(hf, &ShadowQuadrant, sizeof(ShadowQuadrant), &now_pos, NULL);
	now_pos += sizeof(ShadowQuadrant);
	::ReadFile(hf, &SlidewayStyle, sizeof(SlidewayStyle), &now_pos, NULL);
	now_pos += sizeof(SlidewayStyle);

	if (GetLastError() != 0)
		return false;
	else
		return true;
}

void TSlideway::NoticeListView(TListView *pListView)
{
	TRealLine::NoticeListView(pListView);
	
	pListView->AddAttributeItem(TEXT("阴影长度"), CTRLTYPE_INT_EDIT, &ShadowLength, TEXT("%d"), ShadowLength);
	pListView->AddAttributeItem(TEXT("阴影位置"), CTRLTYPE_INT_EDIT, &ShadowQuadrant, TEXT("%d"), ShadowQuadrant);
	pListView->AddAttributeItem(TEXT("样式"), CTRLTYPE_INT_EDIT, &SlidewayStyle, TEXT("%d"), SlidewayStyle);
}

void TSlideway::Draw(HDC hdc, const TConfiguration* pConfig)
{
	TDraw::DrawSlideway(hdc, this, pConfig);
	//if (bDrawSquare) DrawPickSquare(hdc, pConfig);
}