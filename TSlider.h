#pragma once
#include <vector>

#include <Windows.h>

#include "TElement.h"
class TSlider :
	public TElement
{
public:
	struct LinkLine
	{
		int index1;
		int index2;
	};
	std::vector<LinkLine> vecLine;//Á¬½Ó¼¯
	void TSlider::NoticeListView(TListView *pListView);
	TSlider();
	~TSlider();
};

