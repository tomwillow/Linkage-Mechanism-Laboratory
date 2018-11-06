#pragma once
#include "TLineTool.h"

class TBarTool:public TLineTool
{
public:
	TBarTool();
	~TBarTool();
	TElement * TBarTool::AddIntoShape(TRealLine &RealLine)override;

	void TBarTool::AddIntoTreeViewContent(TElement *Element, int id)override;
};

