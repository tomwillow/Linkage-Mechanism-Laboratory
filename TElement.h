#pragma once
#include <Windows.h>

#include "DPOINT.h"

enum EnumElementType{ELEMENT_NULL,ELEMENT_REALLINE,ELEMENT_FRAMEPOINT,ELEMENT_BAR,
	CONSTRAINT_COINCIDE};

class TListView;
class TElement
{
public:
	int id;
	bool available;
	EnumElementType eType;
	TCHAR Name[64];
	LOGPEN logpenStyleShow, logpenStyle;
	TElement();
	~TElement();
	void TElement::SetStyle(int iStyle, int iWidth, COLORREF crColor);
	virtual void TElement::NoticeListView(TListView *pListView){}
	TCHAR * TElement::GetLineStyleName(UINT linestyle, TCHAR name[]);
	TCHAR * TElement::GetElementTypeName(TCHAR name[]);
};