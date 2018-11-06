#include "..\Control\TButton.h"


TButton::TButton()
{
}


TButton::~TButton()
{
}

void TButton::CreateButton(HWND hParent,UINT uId,TCHAR text[],HINSTANCE hInst)
{
	m_hInst = hInst;
	m_hWnd = CreateWindow(
		TEXT("button"),//类名，更改后按钮不显示
		text,
		WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
		0,
		0,
		0,
		0,
		hParent, (HMENU)uId,//id
		hInst,
		NULL);

	RegisterProc();
}