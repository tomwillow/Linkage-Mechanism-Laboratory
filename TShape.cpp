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


void TShape::AddLine(DPOINT ptBegin, DPOINT ptEnd,int iStyle,int iWidth,COLORREF crColor)
{
	uiLineNum++;
	RealLine = (TRealLine *)realloc(RealLine, uiLineNum*sizeof(TRealLine));
	RealLine[uiLineNum - 1].ptBegin = ptBegin;
	RealLine[uiLineNum - 1].ptEnd = ptEnd;
	RealLine[uiLineNum - 1].iStyle = iStyle;
	RealLine[uiLineNum - 1].iWidth = iWidth;
	RealLine[uiLineNum - 1].crColor = crColor;
}

void TShape::DeleteNewestLine()
{
	uiLineNum--;
	RealLine = (TRealLine *)realloc(RealLine, uiLineNum*sizeof(TRealLine));
}