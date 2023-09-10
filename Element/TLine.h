#pragma once
#include <Windows.h>
#include <math.h>

#include "..\Element\TElement.h"

class TLine :public TElement
{
protected:
	virtual void TLine::Draw(HDC hdc, const TConfiguration* pConfig)override {}
	virtual void TLine::DrawPickSquare(HDC hdc, const TConfiguration* pConfig)override{}
public:
	POINT ptBegin, ptEnd;
	double &dAngle;
	double dLength;
	TLine();
	~TLine();
	TLine& TLine::operator=(const TLine &Line);
	virtual const String TLine::GetElementTypeName()override;//得到类型名称
	virtual bool TLine::WriteFile(HANDLE &hf, DWORD &now_pos)override{ return true; };
	virtual bool TLine::ReadFile(HANDLE &hf, DWORD &now_pos, TShape *pShape)override{ return true; };
	virtual void TLine::NoticeListView(TListView *pListView)override{};
};

