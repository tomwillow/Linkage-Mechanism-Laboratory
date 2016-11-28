#pragma once
#include <tchar.h>
#include <stdio.h>

#include "TDraw.h"
#include "TLine.h"

TLine::TLine()
{
	TLine::dLength = 0;
	_tcscpy(Name, TEXT("TLine"));
}



TLine::~TLine()
{
}

void TLine::CalcLength()
{
	dLength = sqrt(pow(double(ptEnd.x - ptBegin.x), 2) + pow(double(ptEnd.y - ptBegin.y), 2));
	dAngle = TDraw::GetAngleFromPointScreen(ptBegin, ptEnd);
}
