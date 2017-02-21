#pragma once
#include "TDragTool.h"

#include "resource.h"

TDragTool::TDragTool()
{
	eMode = SELECT_DRAG;
	SendMessage(hwndWin, WM_COMMAND, MAKELONG(ID_REFRESH, 0), 0);
}


TDragTool::~TDragTool()
{
}
