#pragma once
#include "TLineTool.h"

class TBarTool:public TLineTool
{
public:
	TBarTool();
	~TBarTool();
	TElement * TBarTool::AddIntoShape(TRealLine &RealLine);
	//virtual void TBarTool::AddCoincide(TConstraintCoincide *pCoincide, int id, TConfiguration *pConfig) override;//所有加入约束经过此处
};

