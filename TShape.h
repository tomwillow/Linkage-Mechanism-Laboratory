#pragma once
#include <Windows.h>
#include <vector>

#include "TRealLine.h"
#include "TFramePoint.h"
#include "DPOINT.h"

class TShape
{
public:
	std::vector<TElement> Element;
	std::vector<TRealLine> RealLine;
	std::vector<TFramePoint> FramePoint;
	TShape();
	~TShape();
	void TShape::AddElement(TElement element);
	//void TShape::DeleteElement(int index);
	void TShape::DeleteRealLine(int index);
	void TShape::AddRealLine(TRealLine realline);
	void TShape::AddFramePoint(TFramePoint fp);
};

