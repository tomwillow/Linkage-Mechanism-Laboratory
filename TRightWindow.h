#pragma once
#include "TWindow.h"
#include "TListView.h"
#include "TTreeView.h"

class TRightWindow :public TWindow
{
private:
	void TRightWindow::OnDraw(HDC hdc);
	void TRightWindow::OnCreate(HWND hWnd, WPARAM wParam, LPARAM lParam);
	void TRightWindow::OnSize(WPARAM wParam, LPARAM lParam);
	void OnLButtonDown(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	void TRightWindow::SetListViewPos();
	void TRightWindow::SetTreeViewPos();
public:
	TListView ListView;
	TTreeView TreeView;
	TRightWindow();
	~TRightWindow();
};

