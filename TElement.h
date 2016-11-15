#pragma once
#include <Windows.h>

#include "DPOINT.h"

enum EnumElementType{ELEMENT_ELEMENT,ELEMENT_REALLINE,ELEMENT_FRAMEPOINT,ELEMENT_BAR};

class TListView;
class TElement
{
public:
	EnumElementType eType;
	TCHAR Name[64];
	LOGPEN logpenStyleShow, logpenStyle;

	//DPOINT dpt;
	//DPOINT ptBegin, ptEnd;
	TElement();
	~TElement();
	void TElement::SetStyle(int iStyle, int iWidth, COLORREF crColor);
	virtual void TElement::NoticeListView(TListView *pListView){}
	TCHAR * TElement::GetLineStyleName(UINT linestyle, TCHAR name[]);
};