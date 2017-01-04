#pragma once
#include "TBarTool.h"

#include "TShape.h"
#include "TBar.h"
#include "TSolver.h"

TBarTool::TBarTool()
{
	myElementType = ELEMENT_BAR;
}


TBarTool::~TBarTool()
{
}

void TBarTool::AddIntoShape(TRealLine &RealLine)
{
	RealLine.eType = ELEMENT_BAR;
	pShape->AddBar((TBar *)(&RealLine));
	pSolver->RefreshEquations(true);
}