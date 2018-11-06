#include "TGroupBox.h"


TGroupBox::TGroupBox()
{
}


TGroupBox::~TGroupBox()
{
}

void TGroupBox::CreateGroupBox(HWND hParent, UINT uId, TCHAR text[], HINSTANCE hInst)
{
	m_hInst = hInst;
	m_hWnd = CreateWindow(
		TEXT("button"),//类名，更改后按钮不显示
		text,
		WS_CHILD | WS_VISIBLE | BS_GROUPBOX|WS_GROUP,
		0,
		0,
		0,
		0,
		hParent, (HMENU)uId,//id
		hInst,
		NULL);
}