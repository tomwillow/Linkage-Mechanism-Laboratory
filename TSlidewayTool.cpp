#pragma once
#include "TSlidewayTool.h"

#include "TShape.h"
#include "TSlideway.h"


TSlidewayTool::TSlidewayTool()
{
	myElementType = ELEMENT_SLIDEWAY;
}


TSlidewayTool::~TSlidewayTool()
{
}


TElement * TSlidewayTool::AddIntoShape(TRealLine &RealLine)
{
	RealLine.eType = myElementType;
	TSlideway Slideway;
	Slideway= RealLine;
	return pShape->AddElement(&Slideway);
}

void TSlidewayTool::AddCoincide(TConstraintCoincide *pCoincide, int id, TConfiguration *pConfig)//所有加入约束经过此处
{

}