#pragma once
#include "TWindow.h"
#include "TListView.h"
#include "TTreeViewContent.h"

class TRightWindow :public TWindow
{
private:
	int margin;
	int TitleHeight;
	RECT rcRegion1, rcRegion2;
	RECT rcTitle1, rcTitle2;
	void TRightWindow::OnDraw(HDC hdc);
	void TRightWindow::OnCreate(HWND hWnd, WPARAM wParam, LPARAM lParam); 
	void TRightWindow::OnNotify(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	void TRightWindow::OnSize(WPARAM wParam, LPARAM lParam);
	void OnLButtonDown(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	void TRightWindow::SetListViewPos();
	void TRightWindow::SetTreeViewPos();
public:
	TListView ListView;
	TTreeViewContent TreeViewContent;
	TRightWindow();
	~TRightWindow();
};

