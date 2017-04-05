#pragma once
#include <vector>

#include "TTreeView.h"

class TElement;
class TShape;
class TTreeViewContent :
	public TTreeView
{
private:
	HTREEITEM hPrevObject;
	HTREEITEM hPrevConstraint;
	TShape *pShape;
	struct TItem
	{
		int ObjectId;
		HTREEITEM hTreeItem;
	};
	std::vector<TItem> Item;
public:
	TTreeViewContent();
	~TTreeViewContent();
	void TTreeViewContent::Initial();
	int TTreeViewContent::GetIdFromHTreeView(HTREEITEM hTreeItem);
	HTREEITEM TTreeViewContent::GetHTreeItemFromId(int id);
	void TTreeViewContent::SelectById(int id);//发送至RightWindow OnNotify
	void TTreeViewContent::AddItem(TElement *Element,int id);
	void TTreeViewContent::DeleteById(int id);
	void TTreeViewContent::SelectNull();
	void TTreeViewContent::DeleteAllItems();
	void TTreeViewContent::AddAllItem();//仅用于读入文件时使用
	LRESULT TTreeViewContent::WndProc(WNDPROC wndproc, HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) override;
};

