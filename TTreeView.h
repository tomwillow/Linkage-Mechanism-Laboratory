#pragma once

#include <Windows.h>

#include <CommCtrl.h>
#include <tchar.h>
#include <stdio.h>
#pragma comment(lib, "comctl32.lib")

class TTreeView
{
public:
	HWND m_hWnd;
	HINSTANCE m_hInst;
	TTreeView();
	~TTreeView();
	void TTreeView::CreateTreeViewEx(HWND hParent, UINT id, HINSTANCE hInst);//´´½¨TreeView
	HTREEITEM TTreeView::InsertTreeviewItem(const LPTSTR pszText, HTREEITEM htiParent);
	virtual void TTreeView::Initial();
};

