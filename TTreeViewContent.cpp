#pragma once
#include "DetectMemoryLeak.h"

#include "TTreeViewContent.h"

#include "TMainWindow.h"
#include "TShape.h"

extern TMainWindow win;
TTreeViewContent::TTreeViewContent()
{
	pShape = &(win.m_Shape);
}


TTreeViewContent::~TTreeViewContent()
{
}

LRESULT TTreeViewContent::WndProc(WNDPROC wndproc, HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_KEYDOWN:
		if (wParam == VK_DELETE)
		{
			win.m_ManageTool.Message(hWnd, uMsg, wParam, lParam);
			return NULL;
		}
		break;
	}
	return CallWindowProc(wndproc, hWnd, uMsg, wParam, lParam);
}

void TTreeViewContent::DeleteAllItems()
{
	TreeView_DeleteAllItems(m_hWnd);
	Item.clear();
}

void TTreeViewContent::DeleteById(int id)
{
	for (auto i = Item.begin(); i != Item.end();i++)
	{
		if (i->ObjectId == id)
		{
			SelectNull();
			TreeView_DeleteItem(m_hWnd, i->hTreeItem);
			Item.erase(i);
			break;
		}
	}
}

void TTreeViewContent::AddAllItem()
{
	for (auto pElement: pShape->Element)
	{
		AddItem(pElement, pElement->id);
	}
}

void TTreeViewContent::AddItem(TElement *Element, int id)
{
	HTREEITEM temp;
	TCHAR buffer[64], szTypeName[16];
	//ID:0 类型 名称
	wsprintf(buffer, TEXT("ID:%d "), id);
	Element->GetElementTypeName(szTypeName);
	wsprintf(buffer, TEXT("%s %s %s"), buffer, szTypeName, Element->Name);

	TItem tempItem;
	switch (Element->eType)
	{
	case ELEMENT_REALLINE:
	case ELEMENT_FRAMEPOINT:
	case ELEMENT_BAR:
	case ELEMENT_SLIDEWAY:
	case ELEMENT_SLIDER:
	case ELEMENT_POLYLINEBAR:
		temp = InsertTreeviewItem(buffer, hPrevObject);
		TreeView_Expand(m_hWnd, hPrevObject, TVE_EXPAND);
		tempItem.ObjectId = id;
		tempItem.hTreeItem = temp;
		Item.push_back(tempItem);
		break;
	case CONSTRAINT_COLINEAR:
	case CONSTRAINT_COINCIDE:
		temp = InsertTreeviewItem(buffer, hPrevConstraint);
		TreeView_Expand(m_hWnd, hPrevConstraint, TVE_EXPAND);
		tempItem.ObjectId = id;
		tempItem.hTreeItem = temp;
		Item.push_back(tempItem);
		break;
	default:
		assert(0);
		break;
	}
}

void TTreeViewContent::Initial()
{
	hPrevObject = InsertTreeviewItem(TEXT("元素"), TVI_ROOT);
	hPrevConstraint = InsertTreeviewItem(TEXT("约束"), TVI_ROOT);
	hPrevDriver = InsertTreeviewItem(TEXT("原动件"), TVI_ROOT);
}

void TTreeViewContent::SelectNull()
{
	TreeView_SelectItem(m_hWnd, NULL);
}

void TTreeViewContent::SelectById(int id)
{
	for (auto item:Item)
	{
		if (item.ObjectId == id)
		{
			TreeView_Select(m_hWnd, item.hTreeItem, TVGN_CARET);
		}
	}
}

HTREEITEM TTreeViewContent::GetHTreeItemFromId(int id)
{
	for (auto item: Item)
	{
		if (item.ObjectId==id)
		{
			return item.hTreeItem;
		}
	}
	return NULL;
}

int TTreeViewContent::GetIdFromHTreeView(HTREEITEM hTreeItem)
{
	for (auto item: Item)
	{
		if (item.hTreeItem == hTreeItem)
		{
			return item.ObjectId;
		}
	}
	return -1;
}