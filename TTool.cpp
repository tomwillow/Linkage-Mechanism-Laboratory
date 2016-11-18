#pragma once
#include "TTool.h"

#include "TMainWindow.h"

#include "TTreeViewContent.h"

extern TMainWindow win;
TTool::TTool()
{
	hwndWin = win.m_hWnd;
	pConfig = &(win.m_Configuration);
	pCanvas = &(win.Canvas);
	pShape = &(win.m_Shape);
	pListView = &(win.RightWindow.ListView);
	pTreeViewContent = &(win.RightWindow.TreeViewContent);
}


TTool::~TTool()
{
}

//void TTool::RefreshTreeViewContent()
//{
//	pTreeViewContent->RefreshItem();
//}