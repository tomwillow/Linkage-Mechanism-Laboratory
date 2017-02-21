#pragma once
#include "TLineTool.h"

class TSlidewayTool :public TLineTool
{
private:
	void TSlidewayTool::AddCoincide(TConstraintCoincide *pCoincide, int id, TConfiguration *pConfig);//所有加入约束经过此处
public:
	TSlidewayTool();
	~TSlidewayTool();
	TElement * TSlidewayTool::AddIntoShape(TRealLine &RealLine);
};

