#pragma once
#include "TBarTool.h"

#include "TShape.h"
#include "TBar.h"
#include "TConstraintCoincide.h"

TBarTool::TBarTool()
{
	myElementType = ELEMENT_BAR;

	sType = TEXT("Á¬¸Ë");
	bCanBuildCoincide = true;
}


TBarTool::~TBarTool()
{
}

TElement * TBarTool::AddIntoShape(TRealLine &RealLine)
{
	RealLine.vecDpt.push_back(RealLine.GetRelativePointByIndex(0));
	RealLine.vecDpt.push_back(RealLine.GetRelativePointByIndex(1));

	RealLine.eType = myElementType;
	return pShape->AddElement((TBar *)&RealLine);
}

//void TBarTool::AddCoincide(TConstraintCoincide *pCoincide, int id, TConfiguration *pConfig)
//{
//	AddIntoTreeViewContent(pCoincide, id);
//	pShape->AddElement(pCoincide);
//}