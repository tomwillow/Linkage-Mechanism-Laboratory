#pragma once
#include <Windows.h>

class TListView;
class TElement
{
public:
	TElement();
	~TElement();
	virtual void TElement::NoticeListView(TListView *pListView){}
	TCHAR * TElement::GetLineStyleName(UINT linestyle, TCHAR name[]);
};

