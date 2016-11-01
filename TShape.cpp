#pragma once
#include "TShape.h"


TShape::TShape()
{
	uiLineNum = 0;
	RealLine = NULL;
}


TShape::~TShape()
{
	free(RealLine);
}

void TShape::AddRealLine(TRealLine realline)
{
	uiLineNum++;
	RealLine = (TRealLine *)realloc(RealLine, uiLineNum*sizeof(TRealLine));
	RealLine[uiLineNum - 1] = realline;
}

void TShape::DeleteNewestLine()
{
	uiLineNum--;
	RealLine = (TRealLine *)realloc(RealLine, uiLineNum*sizeof(TRealLine));
}