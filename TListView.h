#pragma once
#include <vector>
#include <Windows.h>

#include <CommCtrl.h>
#include <tchar.h>
#include <stdio.h>
#pragma comment(lib, "comctl32.lib")

class TEdit;
class TListView
{
private:
	TEdit *tempEdit;
	std::vector<TEdit *> pEdit;
	void TListView::DeleteAllEdit();
protected:
	static WNDPROC oldEditProc;
	static LRESULT CALLBACK subEditProc(HWND wnd, UINT msg, WPARAM wParam, LPARAM lParam);

	//虚拟消息处理函数，可重载
	virtual LRESULT TListView::WndProc(WNDPROC wndproc, HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
public:
	HWND m_hWnd;
	HINSTANCE m_hInst;
	TListView();
	~TListView();
	void TListView::CreateListViewEx(HWND hParent, UINT id, HINSTANCE hInst);//创建ListView
	void TListView::InsertColumn(int index, TCHAR text[], int width, int styleLVCFMT=LVCFMT_CENTER);
	void CDECL TListView::InsertItem(int index, int subitem, TCHAR szFormat[], ...);
	void CDECL TListView::InsertAttributeItem(int index, TCHAR szName[], int iEditId, TCHAR szEditFormat[], ...);
	void CDECL TListView::SetAttributeItemValue(int index, TCHAR szFormat[], ...);
	void TListView::UpdateControl();
	void TListView::DeleteAllItems();
	RECT TListView::GetGridRect(int index, int subitem);
	RECT TListView::GetGridRectInMargin(int index, int subitem);
};

