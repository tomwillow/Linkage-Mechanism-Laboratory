#pragma once
#include "DetectMemoryLeak.h"
#include <vector>
#include <Windows.h>

#include "String.h"

#include "DPOINT.h"

enum EnumElementType{ELEMENT_NULL,
	ELEMENT_REALLINE,
	ELEMENT_FRAMEPOINT,
	ELEMENT_BAR,
	ELEMENT_SLIDEWAY,
	ELEMENT_SLIDER,
	CONSTRAINT_COINCIDE,
	CONSTRAINT_COLINEAR};

class TShape;
class TListView;
class TElement
{
private:
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

	TElement();
	virtual ~TElement();
	void TElement::SetStyle(const LOGPEN &logpen);//设置样式
	void TElement::SetStyle(int iStyle, int iWidth, COLORREF crColor);//设置样式
	TCHAR * TElement::GetLineStyleName(UINT linestyle, TCHAR name[]);//得到线型名称
	TCHAR * TElement::GetElementTypeName(TCHAR name[]);//得到类型名称
	TElement& TElement::operator=(const TElement &element);
	virtual void TElement::NoticeListView(TListView *pListView){}//
	virtual void TElement::BuildpDpt(){ assert(0); }
	virtual bool TElement::WriteFile(HANDLE &hf, DWORD &now_pos);
	virtual bool TElement::ReadFile(HANDLE &hf, DWORD &now_pos,TShape *pShape);
};