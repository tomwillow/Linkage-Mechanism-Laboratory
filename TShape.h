#pragma once
#include <Windows.h>
#include <vector>

#include "TBar.h"
#include "TRealLine.h"
#include "TFramePoint.h"
#include "DPOINT.h"

class TShape
{
public:
	std::vector<TElement *> Element;
	TShape();
	~TShape();
	void TShape::DeleteElement(int index);
	void TShape::AddRealLine(TRealLine &realline);
	void TShape::AddBar(TBar *bar);
	void TShape::AddFramePoint(TFramePoint &framepoint);
};

