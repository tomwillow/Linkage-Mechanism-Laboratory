#pragma once
#include "..\Common\DetectMemoryLeak.h"
#include "..\Control\TListView.h"
#include "..\Control\TCanvas.h"

#include "..\Control\TEdit.h"

#include "..\Control\TMainWindow.h"

extern TMainWindow win;
TListView::TListView()
{
	iRowCount = 0;
	iColumnCount = 0;
	pShape = &(win.m_Shape);
	pTreeViewContent = &(win.RightWindow.TreeViewContent);
}


TListView::~TListView()
{
}

LRESULT TListView::WndProc(WNDPROC wndproc, HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_USER:
	{
			//if (tempEdit.Text != NULL)
			{
				//刷新TreeViewContent
				pTreeViewContent->DeleteAllItems();
				pTreeViewContent->Initial();
				pTreeViewContent->AddAllItem();

				pTreeViewContent->SelectById(id);//刷新ListView

				//pShape->GetElementById(id)->NoticeListView(this);

				//恢复位置
				ListView_Scroll(hWnd,origin.x,origin.y);
				win.pSolver->RefreshEquations();
				win.Canvas.Invalidate();
			}
		break;
	}
	case WM_VSCROLL:
		//assert(0);
		break;
	case WM_MOUSEWHEEL:
		if (tempEdit.GetVisible())
		{
			RECT rect = GetGridRectInMargin(tempEdit.ListItemIndex, 1);//点击项内容区的坐标

			tempEdit.SetPosition(rect);
			PostMessage(hWnd, WM_PAINT, 0, 0);
		}
		break;
	case WM_LBUTTONDOWN:
	case WM_RBUTTONDOWN:
	{
		int xPos = LOWORD(lParam);
		int yPos = HIWORD(lParam);

		int index = ListView_GetHotItem(hWnd);//点击项的序号
		RECT rect = GetGridRectInMargin(index, 1);//点击项内容区的坐标
		bool bClickCol1 =xPos > ListView_GetColumnWidth(hWnd, 0);//点击的内容区而不是项目名


		tempEdit.SetVisible(false);

		if (index != -1)//确实选中
		{
			if (bClickCol1)//点的第2列
			{
				//显示控件

				//得到视图原点
				ListView_GetOrigin(hWnd, &origin);

				switch (vecCtrlType[index])
				{
				case CTRLTYPE_EDIT:
				case CTRLTYPE_COOR_EDIT:
				case CTRLTYPE_INT_EDIT:
				case CTRLTYPE_DOUBLE_EDIT:
				case CTRLTYPE_ANGLE_VALUE_EDIT:
				case CTRLTYPE_COLOR_HEX:
				case CTRLTYPE_LINE_WIDTH:
				case CTRLTYPE_COOR_P1_EDIT:
				case CTRLTYPE_COOR_P2_EDIT:
				case CTRLTYPE_LEN_EDIT:
				case CTRLTYPE_ANGLE_EDIT:
				{

					//设置Edit并显示
					tempEdit.eCtrlType = vecCtrlType[index];
					tempEdit.pContent = vecpContent[index];
					tempEdit.ListItemIndex = index;
					tempEdit.SetPosition(rect);

					TCHAR buf[100];
					ListView_GetItemText(hWnd, index, 1, buf, sizeof(buf)*sizeof(TCHAR));
					tempEdit.SetText(buf);

					//赋予修改权
					tempEdit.pElement = pShape->GetElementById(id);

					tempEdit.SetVisible(true);
					SetFocus(tempEdit.m_hWnd);

					POINT ptClickPos;
					ptClickPos.x = xPos - rect.left;
					ptClickPos.y = yPos - rect.top;

					PostMessage(tempEdit.m_hWnd, WM_LBUTTONDOWN, 0, (LPARAM)MAKELONG(ptClickPos.x,ptClickPos.y));

					win.Canvas.Invalidate();

					return NULL;

					break;
				}
				case CTRLTYPE_NULL:
					break;
				default:
					assert(0);
					break;
				}
			}

		}
		break;
	}
	}
	return CallWindowProc(wndproc, hWnd, uMsg, wParam, lParam);
}

void TListView::DeleteAllItems()
{
	ListView_DeleteAllItems(m_hWnd);
	iRowCount = 0;
	iColumnCount = 0;

	vecCtrlType.clear();
	vecpContent.clear();
}

int TListView::GetItemCount()
{
	return ListView_GetItemCount(m_hWnd);
}

void CDECL TListView::AddAttributeItem(const TCHAR szName[], enumCtrlType eCtrlType, void *pContent, const TCHAR szEditFormat[], ...)
{
	//插入标签
	InsertItem(iRowCount, 0, szName);

	TCHAR szBuffer[1024];
	va_list pArgList;
	va_start(pArgList, szEditFormat);
	_vsntprintf_s(szBuffer, sizeof(szBuffer) / sizeof(TCHAR), szEditFormat, pArgList);
	va_end(pArgList);

	//插入内容
	InsertItem(iRowCount - 1, 1, szBuffer);

	vecCtrlType.push_back(eCtrlType);
	vecpContent.push_back(pContent);
}

void CDECL TListView::SetAttributeItemValue(int index, TCHAR szEditFormat[], ...)
{
	TCHAR szBuffer[1024];
	va_list pArgList;
	va_start(pArgList, szEditFormat);
	_vsntprintf_s(szBuffer, sizeof(szBuffer) / sizeof(TCHAR), szEditFormat, pArgList);
	va_end(pArgList);

	ListView_SetItemText(m_hWnd, index, 1, szBuffer);
}

void TListView::CreateListViewEx(HWND hParent, UINT id, HINSTANCE hInst)
{
	m_hInst = hInst;
	m_hWnd = ::CreateWindowEx(0,
		WC_LISTVIEW,
		TEXT("TListView"),
		WS_CHILD | WS_VISIBLE | LVS_ICON | LVS_REPORT | LVS_SINGLESEL,//|LVS_EDITLABELS|WS_BORDER
		0, 0, 0, 0, hParent, (HMENU)id, hInst, 0);

	//CreateEx(0,
	//	WC_LISTVIEW,
	//	TEXT("TListView"),
	//	WS_CHILD | WS_VISIBLE | LVS_ICON | LVS_REPORT | LVS_SINGLESEL,//|LVS_EDITLABELS|WS_BORDER
	//	0, 0, 0, 0, hParent, (HMENU)id, hInst);

	//设置扩展风格：添加格子线以及整行选中
	DWORD styles = LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES;
	ListView_SetExtendedListViewStyleEx(m_hWnd, styles, styles);

	//创建Edit
	tempEdit.CreateEditEx(m_hWnd, 0, m_hInst);
	tempEdit.SetDefaultGuiFont();
	tempEdit.SetPosition(10, 10, 100, 20);
	tempEdit.SetVisible(false);

	RegisterProc();
}


void TListView::InsertColumn(int index, TCHAR text[], int width, int styleLVCFMT)
{
	LVCOLUMN lvc = { 0 };
	lvc.mask = LVCF_WIDTH | LVCF_TEXT | LVCF_FMT | LVCF_SUBITEM | LVCF_ORDER;
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

void CDECL TListView::InsertItem(int index, int subitem, const TCHAR szFormat[], ...)
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

RECT TListView::GetGridRectInMargin(int index, int subitem)
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

//void TListView::LoadObject(int index)
//{
//	this->DeleteAllItems();
//
//	TCHAR buffer[16];
//
//	switch (pShape->Element[index]->eType)
//	{
//	case ELEMENT_FRAMEPOINT:
//	{
//		TFramePoint *pElement = (TFramePoint *)pShape->Element[index];
//		AddAttributeItem(TEXT("ID"), CTRLTYPE_NULL, NULL, TEXT("%d"), pElement->id);
//		AddAttributeItem(TEXT("名称"), CTRLTYPE_EDIT, &(pElement->Name), pElement->Name);
//		AddAttributeItem(TEXT("类型"), CTRLTYPE_NULL, NULL, TEXT("机架"));
//		AddAttributeItem(TEXT("线型"), CTRLTYPE_NULL, NULL, pElement->GetLineStyleName(pElement->logpenStyle.lopnStyle, buffer));
//		AddAttributeItem(TEXT("线宽"), CTRLTYPE_NULL, NULL, TEXT("%d"), pElement->logpenStyle.lopnWidth);
//		AddAttributeItem(TEXT("颜色"), CTRLTYPE_NULL, NULL, TEXT("0x%X"), pElement->logpenStyle.lopnColor);
//		AddAttributeItem(TEXT("P0"), CTRLTYPE_COOR_EDIT, &(pElement->dpt), TEXT("%.3f,%.3f"), pElement->dpt.x, pElement->dpt.y);
//		break;
//	}
//	}
//}