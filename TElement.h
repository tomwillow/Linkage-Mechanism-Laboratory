#pragma once
#include <vector>
#include <Windows.h>

#include "String.h"

#include "DPOINT.h"

enum EnumElementType{
	ELEMENT_NULL,
	ELEMENT_REALLINE,
	ELEMENT_FRAMEPOINT,
	ELEMENT_BAR,
	ELEMENT_SLIDEWAY,
	ELEMENT_SLIDER,
	CONSTRAINT_COINCIDE,
	CONSTRAINT_COLINEAR,
	ELEMENT_POLYLINEBAR,
	DRIVER
};

class TConfiguration;
class TShape;
class TListView;
class TElement
{
private:
protected:
public:
	int id;
	bool available;//未启用
	EnumElementType eType;//类型
	TCHAR Name[64];//名称
	LOGPEN logpenStyleShow, logpenStyle;//显示样式，本身样式

	std::vector<DPOINT> vecDpt;//点集
	std::vector<std::vector<int>> vecIsJoint;

	DPOINT dpt;
	double angle;

	unsigned char alpha;
	bool CanBeDragged;

	TElement();
	virtual ~TElement();
	void TElement::SetStyle(const LOGPEN &logpen);//设置样式
	void TElement::SetStyle(TConfiguration *pConfig);//设置样式
	TCHAR * TElement::GetLineStyleName(UINT linestyle, TCHAR name[]);//得到线型名称
	virtual const TCHAR * TElement::GetElementTypeName(TCHAR name[]);//得到类型名称
	TElement& TElement::operator=(const TElement &element);
	virtual void TElement::BuildpDpt();
	virtual bool TElement::WriteFile(HANDLE &hf, DWORD &now_pos);
	virtual bool TElement::ReadFile(HANDLE &hf, DWORD &now_pos,TShape *pShape);
	virtual void TElement::NoticeListView(TListView *pListView);
	virtual const DPOINT TElement::GetRelativePointByIndex(int PointIndexOfElement) const;
	virtual DPOINT TElement::GetAbsolutePointByIndex(int PointIndexOfElement) const;
	virtual void TElement::SetX(double x);
	virtual void TElement::SetY(double y);
	virtual void TElement::SetPhi(double phi);
	void TElement::SetColor(COLORREF cr);
	void TElement::SetLineWidth(LONG width);
};

DPOINT GetAbsolute(const DPOINT &dpt, const DPOINT &Org, double angle);