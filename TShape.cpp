#pragma once
#include "TShape.h"


TShape::TShape()
{
	//uiLineNum = 0;
	//RealLine = NULL;
}


TShape::~TShape()
{
	//free(RealLine);
}

void TShape::AddRealLine(TRealLine realline)
{
	//uiLineNum++;
	RealLine.push_back(realline);
	//RealLine = (TRealLine *)realloc(RealLine, uiLineNum*sizeof(TRealLine));
	//RealLine[uiLineNum - 1] = realline;
}

void TShape::AddFramePoint(TFramePoint fp)
{
	FramePoint.push_back(fp);
}

void TShape::DeleteRealLine(int index)
{
	std::vector<TRealLine>::iterator iter=RealLine.begin()+index;
	RealLine.erase(iter);
}

void TShape::DeleteNewestLine()
{
	//uiLineNum--;
	//RealLine = (TRealLine *)realloc(RealLine, uiLineNum*sizeof(TRealLine));
}