#pragma once
#include <vector>

#include <Windows.h>

#include "TElement.h"
class TSlider :
	public TElement
{
public:
	std::vector<DPOINT> vecDpt;
	struct LinkLine
	{
		int index1;
		int index2;
	};
	std::vector<LinkLine> vecLine;
	TSlider();
	~TSlider();
};

