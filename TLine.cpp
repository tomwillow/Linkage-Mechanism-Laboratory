#pragma once
#include <tchar.h>
#include <stdio.h>

#include "TDraw.h"
#include "TLine.h"

TLine::TLine() :dAngle(angle)
{
	TLine::dLength = 0;
	_tcscpy(Name, TEXT("TLine"));
}



TLine::~TLine()
{
}