#pragma once
#include "TSlidewayTool.h"

#include "..\Control\TTreeViewContent.h"
#include "..\Element\TShape.h"
#include "..\Element\TSlideway.h"

#include "..\TConfiguration.h"

TSlidewayTool::TSlidewayTool()
{
	myElementType = ELEMENT_SLIDEWAY;

	delete MoveLine;
	MoveLine = new TSlideway;
	MoveLine->SetStyle(pConfig);

	sType = TEXT("滑道");
	bCanBuildCoincide = false;
}


TSlidewayTool::~TSlidewayTool()
{
}


TElement * TSlidewayTool::AddIntoShape(TRealLine &RealLine)
{
	RealLine.vecDpt.push_back(RealLine.GetRelativePointByIndex(0));
	RealLine.vecDpt.push_back(RealLine.GetRelativePointByIndex(1));

	RealLine.eType = myElementType;
	TSlideway Slideway;
	Slideway= RealLine;
	return pShape->AddElement(&Slideway);
}

void TSlidewayTool::AddIntoTreeViewContent(TElement *Element, int id)
{
	if (Element->eType == ELEMENT_REALLINE)
	{
		Element->eType = myElementType;
		TSlideway Slideway;
		Slideway = *(TRealLine*)Element;
		pTreeViewContent->AddItem(&Slideway, pShape->iNextId);
	}
	else
		TLineTool::AddIntoTreeViewContent(Element, id);
}

//void TSlidewayTool::AddCoincide(TConstraintCoincide *pCoincide, int id, TConfiguration *pConfig)//所有加入约束经过此处
//{
//
//}