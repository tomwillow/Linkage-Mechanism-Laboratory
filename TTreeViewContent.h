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
public:
	struct TItem
	{
		int ObjectId;
		HTREEITEM hTreeItem;
	};
	std::vector<TItem> Item;
	TTreeViewContent();
	~TTreeViewContent();
	void TTreeViewContent::Initial();
	void TTreeViewContent::RefreshItem();
	int TTreeViewContent::GetIdFromHTreeView(HTREEITEM hTreeItem);
	void TTreeViewContent::DeleteAllItems();
	void TTreeViewContent::SelectById(int id);
	void TTreeViewContent::AddItem(TElement *Element,int id);
	void TTreeViewContent::DeleteById(int id);
	void TTreeViewContent::SelectNull();
};

