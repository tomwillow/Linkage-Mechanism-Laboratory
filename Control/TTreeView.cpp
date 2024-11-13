#pragma once
#include "TTreeView.h"


TTreeView::TTreeView()
{
}


TTreeView::~TTreeView()
{
}


//LRESULT CALLBACK TTreeView::subControlProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
//{
//	TTreeView * pControl;
//	pControl = (TTreeView *)GetWindowLong(hWnd, GWL_USERDATA);
//
//	if (pControl)
//		return pControl->WndProc(oldControlProc, hWnd, uMsg, wParam, lParam);
//	else
//		return CallWindowProc(oldControlProc, hWnd, uMsg, wParam, lParam);
//}

void TTreeView::CreateTreeViewEx(HWND hParent, UINT id, HINSTANCE hInst)//创建TreeView
{

	// Create the Treeview control
	m_hInst = hInst;
	m_hWnd = CreateWindowEx(0, WC_TREEVIEW, TEXT("TTreeView"),
		TVS_HASLINES | TVS_LINESATROOT | TVS_HASBUTTONS | WS_CHILD | WS_VISIBLE |TVS_SHOWSELALWAYS,
		0,0,0,0,
		hParent, (HMENU)id, hInst, 0);

	//CreateEx(0, WC_TREEVIEW, TEXT("TTreeView"),
	//	TVS_HASLINES | TVS_LINESATROOT | TVS_HASBUTTONS | WS_CHILD | WS_VISIBLE | TVS_SHOWSELALWAYS,
	//	0, 0, 0, 0,
	//	hParent, (HMENU)id, hInst);
	// Store the Treeview control handle as the user data associated with the 
	// parent window so that it can be retrieved for later use.
	//SetWindowLongPtr(m_hWnd, GWLP_USERDATA, (LONG_PTR)m_hWnd);


	/////////////////////////////////////////////////////////////////////////
	// Set up and attach image lists to tree view common control.
	// 

	// Create an image list
	HIMAGELIST hImages = ImageList_Create(
		GetSystemMetrics(SM_CXSMICON),
		GetSystemMetrics(SM_CYSMICON),
		ILC_COLOR32 | ILC_MASK, 1, 1);

	// Get an instance handle for a source of icon images
	HINSTANCE hLib = LoadLibrary(TEXT("shell32.dll"));
	if (hLib)
	{
		for (int i = 4; i < 6; i++)
		{
			// Because the icons are loaded from system resources (i.e. they are 
			// shared), it is not necessary to free resources with 'DestroyIcon'.
			HICON hIcon = (HICON)LoadImage(hLib, MAKEINTRESOURCE(i), IMAGE_ICON,
				0, 0, LR_SHARED);
			ImageList_AddIcon(hImages, hIcon);
		}

		FreeLibrary(hLib);
		hLib = NULL;
	}

	// Attach image lists to tree view common control
	TreeView_SetImageList(m_hWnd, hImages, TVSIL_NORMAL);

	RegisterProc();
}

void TTreeView::Initial()
{
	/////////////////////////////////////////////////////////////////////////
	// Add items to the tree view common control.
	// 

	// Insert the first item at root level
	HTREEITEM hPrev = InsertTreeviewItem(TEXT("First"), TVI_ROOT);

	// Sub item of first item
	hPrev = InsertTreeviewItem(TEXT("Level01"), hPrev);

	// Sub item of 'level01' item
	hPrev = InsertTreeviewItem(TEXT("Level02"), hPrev);

	// Insert the second item at root level
	hPrev = InsertTreeviewItem(TEXT("Second"), TVI_ROOT);

	// Insert the third item at root level
	hPrev = InsertTreeviewItem(TEXT("Third"), TVI_ROOT);
}

HTREEITEM TTreeView::InsertTreeviewItem(const LPTSTR pszText, HTREEITEM htiParent)
{
	TVITEM tvi = { 0 };
	tvi.mask = TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE;
	tvi.pszText = pszText;
	tvi.cchTextMax = _tcslen(pszText);
	tvi.iImage = 0;

	TVINSERTSTRUCT tvis = { 0 };
	tvi.iSelectedImage = 1;
	tvis.item = tvi;
	tvis.hInsertAfter = 0;
	tvis.hParent = htiParent;

	return (HTREEITEM)SendMessage(m_hWnd, TVM_INSERTITEM, 0, (LPARAM)&tvis);
}