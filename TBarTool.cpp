#pragma once
#include "TBarTool.h"

#include "TShape.h"
#include "TBar.h"

TBarTool::TBarTool()
{
}


TBarTool::~TBarTool()
{
}

void TBarTool::AddIntoShape(TRealLine &RealLine)
{
	RealLine.eType = ELEMENT_BAR;
	pShape->AddBar((TBar *)(&RealLine));
}