#include "TButton.h"


TButton::TButton()
{
}


TButton::~TButton()
{
}

bool TButton::GetChecked()
{
	return BST_CHECKED == SendMessage(m_hWnd, BM_GETCHECK, 0, 0);
}

void TButton::SetChecked(bool bChecked)
{
	SendMessage(m_hWnd, BM_SETCHECK, bChecked?BST_CHECKED:BST_UNCHECKED, 0);
}

void TButton::SetCheckedAndBitmap(bool bChecked)//同时改变图片
{
	SetChecked(bChecked);
	if (bChecked)
		SetBitmapIsChecked();
	else
		SetBitmapIsUnChecked();
}

void TButton::LoadCheckedIcon(HINSTANCE hInst, UINT uiIconId)
{
	m_hInst = hInst;
	hIconChecked = LoadIcon(m_hInst, MAKEINTRESOURCE(uiIconId));
}

void TButton::LoadUnCheckedIcon(HINSTANCE hInst, UINT uiIconId)
{
	m_hInst = hInst;
	hIconUnChecked = LoadIcon(m_hInst, MAKEINTRESOURCE(uiIconId));
}

void TButton::CreateIconCheckBox(HINSTANCE hInst, HWND hParent, int x, int y, UINT id)
{
	m_iWidth = 24;
	m_iHeight = 24;
	m_hParent = hParent;
	m_hInst = hInst;
	m_hWnd = CreateWindow(
		TEXT("button"),//类名，更改后按钮不显示
		TEXT("按钮"),
		WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX | BS_PUSHLIKE | BS_ICON,//BS_ICON |BS_PUSHBUTTON ,
		x, y,
		m_iWidth, m_iHeight,
		m_hParent, (HMENU)id,
		m_hInst,
		NULL);

	SendMessage(m_hWnd, BM_SETIMAGE, IMAGE_ICON, (LPARAM)hIconUnChecked);

	RegisterProc();
}


void TButton::SetIconIsChecked()
{
	SendMessage(m_hWnd, BM_SETIMAGE, IMAGE_ICON, (LPARAM)hIconChecked);
}

void TButton::SetIconIsUnChecked()
{
	SendMessage(m_hWnd, BM_SETIMAGE, IMAGE_ICON, (LPARAM)hIconUnChecked);
}


void TButton::LoadCheckedBitmap(HINSTANCE hInst, UINT uiBitmapId)
{
	m_hInst = hInst;
	hBitmapChecked = LoadBitmap(m_hInst, MAKEINTRESOURCE(uiBitmapId));
}

void TButton::LoadUnCheckedBitmap(HINSTANCE hInst, UINT uiBitmapId)
{
	m_hInst = hInst;
	hBitmapUnChecked = LoadBitmap(m_hInst, MAKEINTRESOURCE(uiBitmapId));
}

void TButton::CreateBitmapCheckBox(HINSTANCE hInst, HWND hParent, int x, int y, UINT id)
{
	m_iWidth = 24;
	m_iHeight = 24;
	m_hParent = hParent;
	m_hInst = hInst;
	m_hWnd = CreateWindow(
		TEXT("button"),//类名，更改后按钮不显示
		TEXT("按钮"),
		WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX | BS_PUSHLIKE | BS_BITMAP,//BS_Bitmap |BS_PUSHBUTTON ,
		x, y,
		m_iWidth, m_iHeight,
		m_hParent, (HMENU)id,
		m_hInst,
		NULL);

	SendMessage(m_hWnd, BM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hBitmapUnChecked);

	RegisterProc();
}

void TButton::SetBitmapIsChecked()
{
	SendMessage(m_hWnd, BM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hBitmapChecked);
}

void TButton::SetBitmapIsUnChecked()
{
	SendMessage(m_hWnd, BM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hBitmapUnChecked);
}