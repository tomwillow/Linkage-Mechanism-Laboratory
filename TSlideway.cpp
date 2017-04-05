#pragma once
#include "tchar_head.h"
#include <stdio.h>

#include "TSlideway.h"
#include "TListView.h"

TSlideway::TSlideway()
{
	_tcscpy(Name, TEXT(""));
	eType = ELEMENT_SLIDEWAY;

	ShadowQuadrant = 4;
	ShadowLength = 40;
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
}