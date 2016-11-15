#pragma once
#include "TRightWindow.h"
#include <CommCtrl.h>
#include <tchar.h>
#include <stdio.h>
#pragma comment(lib, "comctl32.lib")

#include "resource.h"
#include "TEdit.h"
#include "TDraw.h"

TRightWindow::TRightWindow()
{
}


TRightWindow::~TRightWindow()
{
}

void TRightWindow::OnDraw(HDC hdc)
{
	TDraw::FillRect(hdc, &ClientRect, RGB(240, 240, 240));
}

void TRightWindow::OnLButtonDown(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	ShowMessage(TEXT("%d"), ListView_GetItemCount(ListView.m_hWnd));
}

void TRightWindow::OnCreate(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	//此时m_hWnd尚未更新，不可使用。只能用hWnd。
	TreeView.CreateTreeViewEx(hWnd, IDC_TREEVIEW, m_hInst);
	SetTreeViewPos();

	ListView.CreateListViewEx(hWnd, IDC_LISTVIEW, m_hInst);
	SetListViewPos();
	ListView.AddColumn(TEXT("项目"), 80);
	ListView.AddColumn(TEXT("属性"), 93);

	//ListView.InsertAttributeItem(0, TEXT("名称"), 200, TEXT("%d"), 1);
	//ListView.InsertAttributeItem(1, TEXT("类型"), 200, TEXT("机架"), 0);
	//ListView.InsertAttributeItem(2, TEXT("线型"), 200, TEXT("实线"), 0);
	//ListView.InsertAttributeItem(3, TEXT("线宽"), 200, TEXT("1"), 0);
	//ListView.InsertAttributeItem(4, TEXT("颜色"), 200, TEXT("白色"), 0);
	//ListView.InsertAttributeItem(5, TEXT("X1"), 200, TEXT("%f"), 0.23);
	//ListView.InsertAttributeItem(6, TEXT("Y1"), 200, TEXT("%f"), 0.11);
	//ListView.InsertAttributeItem(7, TEXT("X2"), 200, TEXT("%f"), 0.34);
	//ListView.InsertAttributeItem(8, TEXT("Y2"), 200, TEXT("%f"), 0.13);
	//ListView.InsertAttributeItem(9, TEXT("长度"), 200, TEXT("%d"), 100);
	//for (int i = 10; i < 30;i++)
	//	ListView.InsertAttributeItem(i, TEXT("X"), 200, TEXT("%d"), i);

}

void TRightWindow::SetListViewPos()
{
	::MoveWindow(ListView.m_hWnd,
		ClientRect.left + 5,
		ClientRect.top + ClientRect.bottom / 2,
		ClientRect.right - 10,
		ClientRect.bottom - 10 - ClientRect.bottom / 2, true);
}

void TRightWindow::SetTreeViewPos()
{
	::MoveWindow(TreeView.m_hWnd,
		ClientRect.left + 5,
		ClientRect.top + 5 ,
		ClientRect.right - 10,
		ClientRect.bottom - 10 - ClientRect.bottom / 2, true);
}

void TRightWindow::OnSize(WPARAM wParam, LPARAM lParam)
{
	SetTreeViewPos();
	SetListViewPos();
}