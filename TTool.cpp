#pragma once
#include "TTool.h"

#include "TMainWindow.h"

extern TMainWindow win;
TTool::TTool()
{
	//pwin = &win;
	hwndWin = win.m_hWnd;
	pConfig = &(win.m_Configuration);
	pCanvas = &(win.Canvas);
	pShape = &(win.m_Shape);
	pListView = &(win.RightWindow.ListView);
	pTreeViewContent = &(win.RightWindow.TreeViewContent);
	//pSolver = &(win.m_Solver);
	pSolver = win.pSolver;
}


TTool::~TTool()
{
}