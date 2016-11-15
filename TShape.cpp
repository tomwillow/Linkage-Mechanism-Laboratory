#pragma once
#include "TShape.h"

TShape::TShape()
{
}


TShape::~TShape()
{
}

void TShape::AddRealLine(TRealLine realline)
{
	Element.push_back(realline);
		Element[Element.size()-1].ptBegin = realline.ptBegin;
		Element[Element.size() - 1].ptEnd = realline.ptEnd;
}

void TShape::AddFramePoint(TFramePoint framepoint)
{
	Element.push_back(framepoint);
	Element[Element.size() - 1].dpt = framepoint.dpt;
}

void TShape::DeleteElement(int index)
{
	std::vector<TElement>::iterator iter = Element.begin() + index;
	Element.erase(iter);
}

//void TShape::AddRealLine(TRealLine realline)
//{
//	RealLine.push_back(realline);
//	Element.push_back(realline);
//}

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