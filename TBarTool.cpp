#pragma once
#include "TBarTool.h"

#include "TShape.h"
#include "TBar.h"

TBarTool::TBarTool()
{
	myElementType = ELEMENT_BAR;
}


TBarTool::~TBarTool()
{
}

TElement * TBarTool::AddIntoShape(TRealLine &RealLine)
{
	RealLine.eType = myElementType;
	return pShape->AddElement((TBar *)&RealLine);
}