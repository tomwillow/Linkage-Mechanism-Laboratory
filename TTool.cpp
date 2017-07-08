#pragma once
#include "TTool.h"

#include "TMainWindow.h"

extern TMainWindow win;
TTool::TTool() :ClientRect(win.Canvas.ClientRect)
{
	//pwin = &win;
	hInst = win.m_hInst;
	hwndWin = win.m_hWnd;
	pConfig = &(win.m_Configuration);
	pCanvas = &(win.Canvas);
	pShape = &(win.m_Shape);
	pListView = &(win.RightWindow.ListView);
	pTreeViewContent = &(win.RightWindow.TreeViewContent);
	pSolver = win.pSolver;
}


TTool::~TTool()
{
}

void TTool::ResetTool()
{
	::PostMessage(hwndWin, WM_COMMAND, ID_SELECT, 0);
	RefreshCanvas();
}

void TTool::RefreshCanvas()
{
	pCanvas->Invalidate();
}

void TTool::RefreshEquations()
{
	pSolver->RefreshEquations();
}

void TTool::AddTreeViewItem(TElement *pElement, int iId)
{
	pTreeViewContent->AddItem(pElement, iId);
}