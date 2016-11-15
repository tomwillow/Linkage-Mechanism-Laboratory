#pragma once
#include "TShape.h"

TShape::TShape()
{
}


TShape::~TShape()
{
}

void TShape::AddRealLine(TRealLine &realline)
{
	TRealLine *tempRealLine = new TRealLine;
	*tempRealLine = realline;
	Element.push_back(tempRealLine);
}

void TShape::AddBar(TBar *bar)
{
	TBar *tempBar = new TBar;
	*tempBar = *bar;
	Element.push_back(tempBar);
}

void TShape::AddFramePoint(TFramePoint &framepoint)
{
	TFramePoint *tempFramePoint = new TFramePoint;
	*tempFramePoint = framepoint;
	Element.push_back(tempFramePoint);
}

void TShape::DeleteElement(int index)
{
	delete Element[index];
	std::vector<TElement *>::iterator iter = Element.begin() + index;
	Element.erase(iter);
}