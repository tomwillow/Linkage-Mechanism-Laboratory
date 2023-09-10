#pragma once
#include "TToolbar.h"

TToolbar::TToolbar()
{
	m_hWnd = NULL;
	m_hInst = NULL;
	tbButtons = NULL;
	iButtonNum = 0; 

	bIsFlat = false;
	bTextOnRight = false;
	bAutoWrap = false;
}


TToolbar::~TToolbar()
{
	free(tbButtons);
}

LRESULT TToolbar::WndProc(WNDPROC wndproc, HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_USER:
		SetGroupChecked(wParam);
		break;
	}
		return CallWindowProc(wndproc, hWnd, uMsg, wParam, lParam);
}

void TToolbar::SetGroupChecked(int idCommand)
{
	int iGroup = -1;
	for (auto iter = mapGroup.begin(); iter != mapGroup.end(); ++iter)
	{
		if (iter->second.find(idCommand) != iter->second.end())
		{
			iGroup = iter->first;
			break;
		}
		
	}

	if (iGroup != -1)
	{
		for (auto iter = mapGroup[iGroup].begin(); iter != mapGroup[iGroup].end(); ++iter)
		{
			if (*iter==idCommand)
				SendMessage(m_hWnd, TB_SETSTATE, *iter, MAKELONG(TBSTATE_ENABLED | TBSTATE_CHECKED,0));
			else
				SendMessage(m_hWnd, TB_SETSTATE, *iter, MAKELONG(TBSTATE_ENABLED, 0));
		}
	}
}

void TToolbar::CreateToolbar(HWND hwndParent, HINSTANCE hInst)
{
	// Create the toolbar.
	m_hInst = hInst;
	m_hWnd = CreateWindowEx(0, TOOLBARCLASSNAME,NULL,
		WS_CHILD | WS_VISIBLE |TBSTYLE_ALTDRAG|CCS_ADJUSTABLE
		|(bIsFlat?TBSTYLE_FLAT:0)
		|(bTextOnRight?TBSTYLE_LIST:0)
		|(bAutoWrap?TBSTYLE_WRAPABLE:0),
		0, 0, 0, 0,
		hwndParent, NULL, hInst, NULL);


	SetWindowText(m_hWnd, TEXT("TToolbar"));

	RegisterProc();
}

//内部函数：创建图片列表
void TToolbar::CreateImageList(UINT uMsg,int cx, int cy, UINT BitmapID,COLORREF crMask)
{
	HIMAGELIST m_hImageList;
	m_hImageList = ImageList_Create(cx, cy, ILC_COLOR32 | ILC_MASK, 1, 1);//
	HANDLE hImage = NULL;
	hImage = LoadImage(m_hInst, (TCHAR *)BitmapID, IMAGE_BITMAP, 0, 0, LR_DEFAULTCOLOR);
	ImageList_AddMasked(m_hImageList, (HBITMAP)hImage, crMask);

	DeleteObject(hImage);
	SendMessage(m_hWnd, uMsg, (WPARAM)0, (LPARAM)m_hImageList);
}

//设置工具栏图片
void TToolbar::LoadImageList(int cx, int cy, UINT BitmapID,COLORREF crMask)
{
	CreateImageList(TB_SETIMAGELIST, cx, cy, BitmapID,crMask);
}

//设置工具栏悬浮图片
void TToolbar::LoadHotImageList(int cx, int cy, UINT BitmapID, COLORREF crMask)
{
	CreateImageList(TB_SETHOTIMAGELIST, cx, cy, BitmapID, crMask);
}

//设置工具栏禁用图片
void TToolbar::LoadDisabledImageList(int cx, int cy, UINT BitmapID, COLORREF crMask)
{
	CreateImageList(TB_SETDISABLEDIMAGELIST, cx, cy, BitmapID,crMask);
}

//添加按钮
void TToolbar::AddElement(int IconIndex,int idCommand,BYTE fsState,BYTE fsStyle,BYTE bReverse[],DWORD_PTR dwData,INT_PTR iString)
{
	iButtonNum++;
	tbButtons = (TBBUTTON *)realloc(tbButtons, iButtonNum*sizeof(TBBUTTON));
	ZeroMemory(&(tbButtons[iButtonNum - 1]), sizeof(TBBUTTON));
	tbButtons[iButtonNum - 1].iBitmap = IconIndex;
	tbButtons[iButtonNum - 1].idCommand = idCommand;
	tbButtons[iButtonNum - 1].fsState = fsState;
	tbButtons[iButtonNum - 1].fsStyle = fsStyle;
	if (bReverse!=NULL)
		memcpy(tbButtons[iButtonNum - 1].bReserved, bReverse, sizeof(tbButtons->bReserved));
	tbButtons[iButtonNum - 1].dwData = dwData;
	tbButtons[iButtonNum - 1].iString = iString;
}

//添加普通按钮：IconIndex为按钮序号，对应于图片切分后第几块。从0开始。
void TToolbar::AddButton(int IconIndex, int idCommand,bool Enable, TCHAR iString[])
{
	AddElement(IconIndex, idCommand, Enable?TBSTATE_ENABLED:0, BTNS_BUTTON, NULL, NULL, (INT_PTR)iString);
}

//添加分隔符：iWidth指分隔符左边界到下一个按钮左边界的距离
void TToolbar::AddSeparator(int iWidth)
{
	AddElement(iWidth, 0, 0, BTNS_SEP, NULL, NULL, NULL);
}

//添加一个Group按钮
void TToolbar::AddGroup(int IconIndex, int iGroupNum, int idCommand, bool Enable, TCHAR iString[])
{
	if (mapGroup.find(iGroupNum) != mapGroup.end())//有
		mapGroup[iGroupNum].insert(idCommand);
	else
	{
		//std::set<int> setAGroup;
		//setAGroup.insert(idCommand);
		//mapGroup.insert(std::map<int, std::set<int> >::value_type(iGroupNum, setAGroup));

		mapGroup[iGroupNum].insert(idCommand);
	}
	AddElement(IconIndex, idCommand, Enable ? TBSTATE_ENABLED : 0, BTNS_CHECKGROUP, NULL, NULL, (INT_PTR)iString);
}

//添加一个Check按钮
void TToolbar::AddCheck(int IconIndex, int idCommand, bool Enable, TCHAR iString[])
{
	AddElement(IconIndex, idCommand, Enable ? TBSTATE_ENABLED : 0, BTNS_CHECK, NULL, NULL, (INT_PTR)iString);
}

//显示工具栏
void TToolbar::ShowToolbar()
{
	// Add buttons.
	SendMessage(m_hWnd, TB_BUTTONSTRUCTSIZE, (WPARAM)sizeof(TBBUTTON), 0);
	SendMessage(m_hWnd, TB_ADDBUTTONS, (WPARAM)iButtonNum, (LPARAM)tbButtons);

	// Resize the toolbar, and then show it.
	SendMessage(m_hWnd, TB_AUTOSIZE, 0, 0);
}

//刷新工具栏
void TToolbar::FreshSize()
{
	// Resize the toolbar, and then show it.
	SendMessage(m_hWnd, TB_AUTOSIZE, 0, 0);
}
