#pragma once
#include "TShape.h"

TShape::TShape()
{
}


TShape::~TShape()
{
}

void TShape::AddElement(TElement element)
{
	Element.push_back(element);
}

void TShape::DeleteElement(int index)
{
	std::vector<TElement>::iterator iter = Element.begin() + index;
	Element.erase(iter);
}

//void TShape::AddRealLine(TRealLine realline)
//{
//	RealLine.push_back(realline);
//}
//
//void TShape::AddFramePoint(TFramePoint fp)
//{
//	FramePoint.push_back(fp);
//}
//
//void TShape::DeleteRealLine(int index)
//{
//	std::vector<TRealLine>::iterator iter=RealLine.begin()+index;
//	RealLine.erase(iter);
//}