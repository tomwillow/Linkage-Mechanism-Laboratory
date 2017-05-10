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
	RealLine.eType = myElementType;
	return pShape->AddElement((TBar *)&RealLine);
}

//void TBarTool::AddCoincide(TConstraintCoincide *pCoincide, int id, TConfiguration *pConfig)
//{
//	AddIntoTreeViewContent(pCoincide, id);
//	pShape->AddElement(pCoincide);
//}