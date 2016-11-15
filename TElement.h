#pragma once
#include <Windows.h>

#include "DPOINT.h"

class TListView;
class TElement
{
public:
	TCHAR Name[64];
	DPOINT dpt;
	LOGPEN logpenStyleShow, logpenStyle;
	TElement();
	~TElement();
	virtual void TElement::NoticeListView(TListView *pListView){}
	TCHAR * TElement::GetLineStyleName(UINT linestyle, TCHAR name[]);
};