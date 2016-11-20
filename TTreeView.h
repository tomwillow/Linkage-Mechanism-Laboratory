#pragma once

#include <Windows.h>

#include <CommCtrl.h>
#include <tchar.h>
#include <stdio.h>
#pragma comment(lib, "comctl32.lib")

#include "TControl.h"

class TTreeView:public TControl
{
public:
	TTreeView();
	~TTreeView();
	void TTreeView::CreateTreeViewEx(HWND hParent, UINT id, HINSTANCE hInst);//´´½¨TreeView
	HTREEITEM TTreeView::InsertTreeviewItem(const LPTSTR pszText, HTREEITEM htiParent);
	virtual void TTreeView::Initial();
};

