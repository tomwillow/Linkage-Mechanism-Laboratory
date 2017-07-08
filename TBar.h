#pragma once
#include "TRealLine.h"

class TBar :public TRealLine
{
public:
	TBar();
	~TBar();
	virtual const TCHAR * TBar::GetElementTypeName(TCHAR name[]) override;//得到类型名称
};

