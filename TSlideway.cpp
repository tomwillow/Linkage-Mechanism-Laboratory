#pragma once
#include <tchar.h>
#include <stdio.h>

#include "TSlideway.h"


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