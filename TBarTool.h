#pragma once
#include "TLineTool.h"

class TBarTool:public TLineTool
{
public:
	TBarTool();
	~TBarTool();
	void TBarTool::AddIntoShape(TRealLine &RealLine);
};

