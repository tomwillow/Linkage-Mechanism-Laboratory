#pragma once
#include <vector>
#include <Windows.h>

#include <CommCtrl.h>
#include "..\Common\tchar_head.h"
#include <stdio.h>
#pragma comment(lib, "comctl32.lib")

#include "TControl.h"
#include "TListViewEdit.h"
#include "..\Common\DPOINT.h"

#include "..\enumCtrlType.h"

class TTreeViewContent;
class TShape;
class TListView:public TControl
{
private:
	RECT rc;
	POINT origin;
	int iRowCount, iColumnCount;
	TListViewEdit tempEdit;
	void TListView::DeleteAllEdit();
	std::vector<enumCtrlType> vecCtrlType;
	std::vector<void *> vecpContent;
	TShape *pShape;
	TTreeViewContent *pTreeViewContent;
protected:
	//虚拟消息处理函数，可重载
	LRESULT TListView::WndProc(WNDPROC wndproc, HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) override;
public:
	TListView();
	~TListView();
	int id;
	void TListView::CreateListViewEx(HWND hParent, UINT id, HINSTANCE hInst);//创建ListView
	void TListView::AddColumn(TCHAR text[], int width, int styleLVCFMT = LVCFMT_LEFT);//添加列标签
	void TListView::InsertColumn(int index, TCHAR text[], int width, int styleLVCFMT = LVCFMT_LEFT);//插入列标签
	void CDECL TListView::InsertItem(int index, int subitem,const TCHAR szFormat[], ...);//插入项目
	void CDECL TListView::AddAttributeItem(const TCHAR szName[],enumCtrlType eCtrlType,void *pContent,const TCHAR szEditFormat[], ...);
	void CDECL TListView::SetAttributeItemValue(int index, TCHAR szFormat[], ...);//未用
	int TListView::GetItemCount();//得到项目数
	void TListView::DeleteAllItems();//清除全部项
	RECT TListView::GetGridRect(int index, int subitem);//根据行号和列号得到格子坐标
	RECT TListView::GetGridRectInMargin(int index, int subitem);//得到格子边框内的坐标
	//void TListView::LoadObject(int index);
};

