#include "..\Control\TCheckBox.h"


TCheckBox::TCheckBox()
{
}


TCheckBox::~TCheckBox()
{
}

bool TCheckBox::GetChecked()
{
	return BST_CHECKED == SendMessage(m_hWnd, BM_GETCHECK, 0, 0);
}

void TCheckBox::SetChecked(bool bChecked)
{
	SendMessage(m_hWnd, BM_SETCHECK, bChecked?BST_CHECKED:BST_UNCHECKED, 0);
}

void TCheckBox::SetCheckedAndBitmap(bool bChecked)//同时改变图片
{
	SetChecked(bChecked);
	if (bChecked)
		SetBitmapIsChecked();
	else
		SetBitmapIsUnChecked();
}

void TCheckBox::LoadCheckedIcon(HINSTANCE hInst, UINT uiIconId)
{
	m_hInst = hInst;
	hIconChecked = LoadIcon(m_hInst, MAKEINTRESOURCE(uiIconId));
}

void TCheckBox::LoadUnCheckedIcon(HINSTANCE hInst, UINT uiIconId)
{
	m_hInst = hInst;
	hIconUnChecked = LoadIcon(m_hInst, MAKEINTRESOURCE(uiIconId));
}

void TCheckBox::CreateIconCheckBox(HINSTANCE hInst, HWND hParent, int x, int y, int width, int height, UINT id)
{
	m_hParent = hParent;
	m_hInst = hInst;
	m_hWnd = CreateWindow(
		TEXT("button"),//类名，更改后按钮不显示
		TEXT("按钮"),
		WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX | BS_PUSHLIKE | BS_ICON,//BS_ICON |BS_PUSHBUTTON ,
		x, y,
		width, height,
		m_hParent, (HMENU)id,
		m_hInst,
		NULL);

	SendMessage(m_hWnd, BM_SETIMAGE, IMAGE_ICON, (LPARAM)hIconUnChecked);

	RegisterProc();
}


void TCheckBox::SetIconIsChecked()
{
	SendMessage(m_hWnd, BM_SETIMAGE, IMAGE_ICON, (LPARAM)hIconChecked);
}

void TCheckBox::SetIconIsUnChecked()
{
	SendMessage(m_hWnd, BM_SETIMAGE, IMAGE_ICON, (LPARAM)hIconUnChecked);
}


void TCheckBox::LoadCheckedBitmap(HINSTANCE hInst, UINT uiBitmapId)
{
	m_hInst = hInst;
	hBitmapChecked = LoadBitmap(m_hInst, MAKEINTRESOURCE(uiBitmapId));
}

void TCheckBox::LoadUnCheckedBitmap(HINSTANCE hInst, UINT uiBitmapId)
{
	m_hInst = hInst;
	hBitmapUnChecked = LoadBitmap(m_hInst, MAKEINTRESOURCE(uiBitmapId));
}

void TCheckBox::CreateBitmapCheckBox(HINSTANCE hInst, HWND hParent, int x, int y, int width, int height, UINT id)
{
	m_hParent = hParent;
	m_hInst = hInst;
	m_hWnd = CreateWindow(
		TEXT("button"),//类名，更改后按钮不显示
		TEXT("按钮"),
		WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX | BS_PUSHLIKE | BS_BITMAP,//BS_Bitmap |BS_PUSHBUTTON ,
		x, y,
		width, height,
		m_hParent, (HMENU)id,
		m_hInst,
		NULL);

	SendMessage(m_hWnd, BM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hBitmapUnChecked);

	RegisterProc();
}

void TCheckBox::SetBitmapIsChecked()
{
	SendMessage(m_hWnd, BM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hBitmapChecked);
}

void TCheckBox::SetBitmapIsUnChecked()
{
	SendMessage(m_hWnd, BM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hBitmapUnChecked);
}