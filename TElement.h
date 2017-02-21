#pragma once
#include <Windows.h>

#include "DPOINT.h"

enum EnumElementType{ELEMENT_NULL,
	ELEMENT_REALLINE,
	ELEMENT_FRAMEPOINT,
	ELEMENT_BAR,
	ELEMENT_SLIDEWAY,
	CONSTRAINT_COINCIDE};

class TListView;
class TElement
{
public:
	int id;
	bool available;//未启用
	EnumElementType eType;//类型
	TCHAR Name[64];//名称
	LOGPEN logpenStyleShow, logpenStyle;//显示样式，本身样式

	DPOINT dpt;
	double angle;

	TElement();
	~TElement();
	void TElement::SetStyle(int iStyle, int iWidth, COLORREF crColor);//设置样式
	virtual void TElement::NoticeListView(TListView *pListView){}//
	TCHAR * TElement::GetLineStyleName(UINT linestyle, TCHAR name[]);//得到线性名称
	TCHAR * TElement::GetElementTypeName(TCHAR name[]);//得到类型名称
};