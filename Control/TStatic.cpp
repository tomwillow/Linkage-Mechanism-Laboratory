#include "TStatic.h"


TStatic::TStatic()
{
}


TStatic::~TStatic()
{
}

void TStatic::CreateStatic(HWND hParent, UINT uId, TCHAR text[], HINSTANCE hInst)
{
	m_hInst = hInst;
	m_hWnd = CreateWindow(
		TEXT("static"),//类名，更改后按钮不显示
		text,
		WS_CHILD | WS_VISIBLE|SS_CENTERIMAGE,
		0,
		0,
		0,
		0,
		hParent, (HMENU)uId,//id
		hInst,
		NULL);
}