#pragma once
#include "TRealLine.h"
class TSlideway :
	public TRealLine
{
public:
	int ShadowQuadrant;
	int ShadowLength;
	TSlideway();
	~TSlideway();
	TSlideway& TSlideway::operator=(const TSlideway &Slideway);
	TSlideway& TSlideway::operator=(const TRealLine &RealLine);
	virtual bool TSlideway::WriteFile(HANDLE &hf, DWORD &now_pos);
	virtual bool TSlideway::ReadFile(HANDLE &hf, DWORD &now_pos,TShape *pShape);
};

