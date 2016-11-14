#include "TListView.h"

#include "TEdit.h"

TListView::TListView()
{
	iRowCount = 0;
	iColumnCount = 0;
	//tempEdit = NULL;
}


TListView::~TListView()
{
	//DeleteAllEdit();
	//tempEdit不需要delete，因为tempEdit指向的是pEdit最后一个元素，在前面已经delete掉了。
	//if (tempEdit != NULL)
	//	delete tempEdit;
}

void TListView::DeleteAllEdit()
{
	//for (int i = 0; i < pEdit.size(); i++)
	//{
	//	DestroyWindow(pEdit[i]->m_hWnd);
	//	delete pEdit[i];
	//}
	//pEdit.clear();
}

WNDPROC TListView::oldEditProc;
LRESULT CALLBACK TListView::subEditProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	TListView * pListView;
	pListView = (TListView *)GetWindowLong(hWnd, GWL_USERDATA);
	if (pListView)
		return pListView->WndProc(oldEditProc, hWnd, uMsg, wParam, lParam);
	else
		return CallWindowProc(oldEditProc, hWnd, uMsg, wParam, lParam);
}

LRESULT TListView::WndProc(WNDPROC wndproc, HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_LBUTTONDOWN:
	case WM_RBUTTONDOWN:
		return CallWindowProc(wndproc, hWnd, uMsg, wParam, lParam);
	//case WM_PAINT:
	//	UpdateControl();
	//	return CallWindowProc(wndproc, hWnd, uMsg, wParam, lParam);
	}
	return CallWindowProc(wndproc, hWnd, uMsg, wParam, lParam);
}

void TListView::UpdateControl()
{
	//RECT ClientRect;
	//::GetClientRect(m_hWnd, &ClientRect);
	//for (int i = 0; i < pEdit.size(); i++)
	//{
	//	pEdit[i]->SetVisible(false);
	//}
	//for (int i = 0; i < pEdit.size(); i++)
	//{
	//	pEdit[i]->SetVisible(false);
	//	RECT rect = GetGridRectInMargin(i, 1);
	//	if (rect.bottom <= ClientRect.top + 25) //顶端溢出
	//		continue;
	//	if (rect.top >= ClientRect.bottom)//底端溢出
	//		break;
	//	
	//		pEdit[i]->SetPos(rect);
	//		pEdit[i]->SetVisible(true);

	//}	
	//for (int i = 0; i < pEdit.size(); i++)
	//{
	//	RECT rect = GetGridRectInMargin(i, 1);
	//	if (rect.bottom <= ClientRect.top + 25 //顶端溢出
	//		|| rect.top >= ClientRect.bottom)//底端溢出
	//	{
	//		pEdit[i]->SetPos(rect);
	//		pEdit[i]->SetVisible(false);
	//	}
	//	else
	//	{
	//		pEdit[i]->SetPos(rect);
	//		pEdit[i]->SetVisible(true);
	//	}

	//}
}

void TListView::DeleteAllItems()
{
	ListView_DeleteAllItems(m_hWnd);
	iRowCount = 0;
	iColumnCount = 0;

	DeleteAllEdit();
}

int TListView::GetItemCount()
{
	return ListView_GetItemCount(m_hWnd);
}

void CDECL TListView::AddAttributeItem(TCHAR szName[], TCHAR szEditFormat[],...)
{
	InsertItem(iRowCount, 0, szName);
	//tempEdit = new TEdit;
	//pEdit.push_back(tempEdit);
	//pEdit.back()->CreateEditEx(m_hWnd, iEditId, m_hInst, 0);
	//pEdit.back()->SetPos(GetGridRectInMargin(index, 1));
	//pEdit.back()->SetFont((HFONT)SendMessage(m_hWnd, WM_GETFONT, 0, 0));

	TCHAR szBuffer[1024];
	va_list pArgList;
	va_start(pArgList, szEditFormat);
	_vsntprintf_s(szBuffer, sizeof(szBuffer) / sizeof(TCHAR), szEditFormat, pArgList);
	va_end(pArgList);

	InsertItem(iRowCount - 1, 1, szBuffer);
	//pEdit.back()->SetText(szBuffer);
	//pEdit.back()->SetVisible(true);
}

void CDECL TListView::SetAttributeItemValue(int index, TCHAR szFormat[], ...)
{

}

void TListView::CreateListViewEx(HWND hParent, UINT id, HINSTANCE hInst)
{
	m_hInst = hInst;
	m_hWnd = ::CreateWindowEx(0, 
		WC_LISTVIEW,
		0,
		WS_CHILD | WS_VISIBLE | LVS_ICON | LVS_REPORT | LVS_SINGLESEL ,//|LVS_EDITLABELS|WS_BORDER
		0, 0, 0, 0, hParent, (HMENU)id, hInst, 0);

	//设置扩展风格：添加格子线以及整行选中
	DWORD styles = LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES;
	ListView_SetExtendedListViewStyleEx(m_hWnd, styles, styles);

	SetWindowLong(m_hWnd, GWL_USERDATA, (LONG)this);
	oldEditProc = (WNDPROC)::SetWindowLongPtr(m_hWnd, GWLP_WNDPROC, (LONG_PTR)subEditProc);
}


void TListView::InsertColumn(int index, TCHAR text[], int width, int styleLVCFMT)
{
	LVCOLUMN lvc = { 0 };
	lvc.mask = LVCF_WIDTH | LVCF_TEXT|LVCF_FMT |LVCF_SUBITEM|LVCF_ORDER;
	lvc.cx = width;
	lvc.fmt = styleLVCFMT;
	lvc.pszText = text;
	lvc.iSubItem = index;
	lvc.iOrder = index;
	SendMessage(m_hWnd, LVM_INSERTCOLUMN, index, (LPARAM)&lvc);
	iColumnCount++;
}

void TListView::AddColumn(TCHAR text[], int width, int styleLVCFMT)
{
	InsertColumn(iColumnCount, text, width, styleLVCFMT);
}

void CDECL TListView::InsertItem(int index, int subitem, TCHAR szFormat[], ...)
{
	TCHAR szBuffer[1024];
	va_list pArgList;
	va_start(pArgList, szFormat);
	_vsntprintf_s(szBuffer, sizeof(szBuffer) / sizeof(TCHAR), szFormat, pArgList);
	va_end(pArgList);

	LVITEM lvi = { 0 };
		lvi.mask = LVIF_TEXT;
		lvi.iItem = index;
		lvi.pszText = szBuffer;
	if (subitem == 0)
	{
		lvi.iSubItem = 0;
		::SendMessage(m_hWnd, LVM_INSERTITEM, 0, (LPARAM)&lvi);
		iRowCount++;
	}
	else
	{
		lvi.iSubItem = subitem;
		::SendMessage(m_hWnd, LVM_SETITEM, 0, (LPARAM)&lvi);
	}

}

RECT TListView::GetGridRectInMargin(int index,int subitem)
{
	RECT rect;
	rect = GetGridRect(index, subitem);
	rect.left += 1;
	rect.top += 1;
	rect.right -= 1;
	rect.bottom -= 1;
	return rect;
}

RECT TListView::GetGridRect(int index, int subitem)
{
	RECT rect;
	ListView_GetSubItemRect(m_hWnd, index, subitem, LVIR_BOUNDS, &rect);
	//对于第一列，需要特殊处理
	if (subitem == 0)
	{
		int nWidth0 = ListView_GetColumnWidth(m_hWnd, 0);
		rect.right = rect.left + nWidth0;
	}
	return rect;
}